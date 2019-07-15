//===- LoopusUtils.cpp ----------------------------------------------------===//
//===----------------------------------------------------------------------===//

#include "LoopusUtils.h"

#include "NameMangling.h"

#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm-c/Core.h"

#include <algorithm>
#include <deque>
#include <set>

/// \brief Returns the number of predecessors of the basic block.
unsigned Loopus::getNumPredecessors(const llvm::BasicBlock *BB) {
  if (BB == 0) { return 0; }

  llvm::const_pred_iterator PREDIT = llvm::pred_begin(BB),
    PREDEND = llvm::pred_end(BB);

  unsigned predcount = 0;
  for ( ; PREDIT != PREDEND; ++PREDIT) {
    ++predcount;
  }

  return predcount;
}

/// \brief Returns the number of successors of the basic block.
unsigned Loopus::getNumSuccessors(const llvm::BasicBlock *BB) {
  if (BB == 0) { return 0; }

  llvm::succ_const_iterator SUCCIT = llvm::succ_begin(BB),
    SUCCEND = llvm::succ_end(BB);

  unsigned succcount = 0;
  for ( ; SUCCIT != SUCCEND; ++SUCCIT) {
    ++succcount;
  }

  return succcount;
}

/// \brief Returns the one and only successor of the basic block.
llvm::BasicBlock* Loopus::getSingleSuccessor(llvm::BasicBlock *BB) {
  if (BB == 0) { return 0; }

  llvm::succ_iterator SUCCIT = llvm::succ_begin(BB),
    SUCCEND = llvm::succ_end(BB);
  if (SUCCIT == SUCCEND) {
    // There is no successor at all
    return 0;
  }

  llvm::BasicBlock* TheSUCC = *SUCCIT;
  ++SUCCIT;
  if (SUCCIT == SUCCEND) {
    // There are no other successors than TheSUCC
    return TheSUCC;
  } else {
    // There are more successors than TheSUCC
    return 0;
  }
}

/// \brief Returns the one and only successor of the basic block.
const llvm::BasicBlock* Loopus::getSingleSuccessor(const llvm::BasicBlock *BB) {
  return getSingleSuccessor(const_cast<llvm::BasicBlock*>(BB));
}

/// \brief Returns the unique successor of the basic block.
llvm::BasicBlock* Loopus::getUniqueSuccessor(llvm::BasicBlock *BB) {
  if (BB == 0) { return 0; }

  llvm::succ_iterator SUCCIT = succ_begin(BB), SUCCEND = succ_end(BB);
  if (SUCCIT == SUCCEND) {
    // There is no successor at all
    return 0;
  }

  llvm::BasicBlock* TheSUCC = *SUCCIT;
  ++SUCCIT;
  for ( ; SUCCIT != SUCCEND; ++SUCCIT) {
    if (*SUCCIT != TheSUCC) {
      // There are other successors than TheSUCC
      return 0;
    }
  }

  return TheSUCC;
}

/// \brief Returns the unique successor of the basic block.
const llvm::BasicBlock* Loopus::getUniqueSuccessor(const llvm::BasicBlock *BB) {
  return getUniqueSuccessor(const_cast<llvm::BasicBlock*>(BB));
  return 0;
}

/// \brief Erases the given instruction and all its operands if allowed.
/// \param I The instruction that should be erased.
/// \para, keepSideEffects Determines if instruction having side-effects may be erased.
/// \returns Returns the number of finally erased instructions.
///
/// Erases the given instruction and then tests all direct and indirect operands
/// used by that instruction if they can be erased, too. If \c keepSideEffects
/// is \c true instructions having side-effects are not erased although they
/// might not be used any more (except for the requested instruction \c I which
/// is always tried to erase).
unsigned Loopus::eraseFromParentRecursively(llvm::Instruction *I, bool keepSideEffects) {
  if (I == 0) { return 0; }

  unsigned InstsErased = 0;
  // We are using a list here to avoid that an operand is added to the worklist
  // several times (that would break the algorithm as the might be dangling
  // pointers)
  typedef std::set<llvm::Value*> WorklistTy;

  WorklistTy worklist;
  worklist.insert(I);

  while (worklist.empty() == false) {
    // Take next element from the worklist
    WorklistTy::iterator ItemPos = worklist.begin();
    llvm::Value *CurVal = *ItemPos;
    worklist.erase(ItemPos);

    // We do only consider instructions as other operands cannot be erased
    // easily
    if (llvm::isa<llvm::Instruction>(CurVal) == true) {
      llvm::Instruction *CurI = llvm::dyn_cast<llvm::Instruction>(CurVal);

      // Check if the instruction has side-effects and if so skip it except for
      // the instruction that was requested to be erased
      if ((CurI != I) && (keepSideEffects == true)) {
        if (llvm::isa<llvm::CallInst>(CurI) == true) {
          // Call instructions are always assumed to have side-effects
          continue;
        } else if (llvm::isa<llvm::BranchInst>(CurI) == true) {
          continue;
        } else if (CurI->mayHaveSideEffects() == true) {
          continue;
        }
      }

      // Check if we are allowed to erase the instruction
      if (CurI->hasNUsesOrMore(1) == false) {
        // Add all instructions operands as they might not be needed anymore...
        for (unsigned i = 0, e = CurI->getNumOperands(); i < e; ++i) {
          worklist.insert(CurI->getOperand(i));
        }

        // Now erase the instruction
        CurI->eraseFromParent();
        ++InstsErased;
      }
    }
  }

  return InstsErased;
}

/// \brief Tries to move the source instruction and all needed operands before the given target instruction.
///
/// Tries to move the given source instruction with all operands needed by it
/// before the specified target instruction.
unsigned Loopus::moveBefore(llvm::Instruction *SourceI, llvm::Instruction *TargetI) {
  if ((SourceI == nullptr) || (TargetI == nullptr)) {
    return 0;
  }

  llvm::BasicBlock* const SourceBB = SourceI->getParent();
  llvm::BasicBlock* const TargetBB = TargetI->getParent();
  if ((SourceBB == nullptr) || (TargetBB == nullptr)) {
    return 0;
  }

  // Test if we may move the instruction. At the same time we create the series
  // of basic blocks that describes how we get from the target block to the
  // source block.
  llvm::SmallVector<llvm::BasicBlock*, 8> BBOrdering;
  llvm::BasicBlock *CurBB = SourceBB;
  while (true) {
    BBOrdering.push_back(CurBB);
    if (CurBB == TargetBB) {
      break;
    }

    llvm::BasicBlock *NextBB = CurBB->getUniquePredecessor();
    if (NextBB == nullptr) {
      // The current basic block has either zero or more than one predecessor.
      // In those cases we cannot move the instruction.
      return 0;
    } else {
      CurBB = NextBB;
    }
  }

  // Now we have to collect all needed operands
  llvm::SmallPtrSet<llvm::Value*, 16> NeededOps;
  std::deque<llvm::Value*> Worklist;
  Worklist.push_back(SourceI);
  while (Worklist.empty() == false) {
    llvm::Value *CurOp = Worklist.front();
    Worklist.pop_front();

    if (CurOp == TargetI) {
      // We cannot move the target instruction before the target instruction
      // itself => loop. So stop...
      return 0;
    }

    if (NeededOps.count(CurOp) > 0) {
      // We already found the current operand
      continue;
    }
    // Check if the current operand is allowed to be moved
    if ((llvm::isa<llvm::BasicBlock>(CurOp) == true)
     || (llvm::isa<llvm::MetadataAsValue>(CurOp) == true)
     || (llvm::isa<llvm::InlineAsm>(CurOp) == true)) {
      // These operand types cannot be moved
      return 0;
    } else if ((llvm::isa<llvm::Constant>(CurOp) == true)
     || (llvm::isa<llvm::Argument>(CurOp) == true)) {
      continue;
    } else if (llvm::isa<llvm::Instruction>(CurOp) == true) {
      if ((llvm::isa<llvm::FenceInst>(CurOp) == true)
       || (llvm::isa<llvm::LandingPadInst>(CurOp) == true)
       || (llvm::isa<llvm::LoadInst>(CurOp) == true)
       || (llvm::isa<llvm::PHINode>(CurOp) == true)
       || (llvm::isa<llvm::StoreInst>(CurOp) == true)
       || (LLVMIsATerminatorInst(wrap(CurOp)) != nullptr)) { //deki_edit
//before change:       || (llvm::isa<llvm::TerminatorInst>(CurOp) == true)) { //deki_edit
        // These instructions cannot/should not be moved
        return 0;
      } else if (llvm::isa<llvm::CallInst>(CurOp) == true) {
        llvm::CallInst *CurOpCI = llvm::dyn_cast<llvm::CallInst>(CurOp);
        // Check if it is a sync barrier
        if (ocl::NameMangling::isSynchronizationFunction(CurOpCI->getCalledFunction()->getName()) == true) {
          return 0;
        }
        // Check if it has nouwind and onlyreadsmemory flags
        if ((CurOpCI->getCalledFunction()->doesNotThrow() == false)
         || (CurOpCI->getCalledFunction()->onlyReadsMemory() == false)) {
          return 0;
        }
      }
    } else {
      // Anything else will not moved
      return 0;
    }

    llvm::Instruction *CurOpI = llvm::dyn_cast<llvm::Instruction>(CurOp);
    // Check if the instruction must be moved. If the instruction is one of the
    // detected basic blocks it must be moved. If it is in any other
    // block it must not be moved.
    llvm::BasicBlock* const CurOpBB = CurOpI->getParent();
    if (CurOpBB != TargetBB) {
      if (std::count(BBOrdering.begin(), BBOrdering.end(), CurOpBB) <= 0) {
        // The operand must not be moved so continued with next operand
        continue;
      }
    } else {
      // The instruction is already in the target block so check if it appears
      // before or after the target instruction 
      bool FoundI = false;
      for (llvm::BasicBlock::iterator INSIT(TargetI), INSEND = TargetBB->end();
          INSIT != INSEND; ++INSIT) {
        llvm::Instruction *CheckI = &*INSIT;
        if (CheckI == CurOpI) {
          FoundI = true;
          break;
        }
      }
      if (FoundI == false) {
        // The current operand appears before the target instruction and must
        // not be moved.
        continue;
      }
    }

    NeededOps.insert(CurOp);
    // Now add all operands into the worklist
    for (unsigned i = 0, e = CurOpI->getNumOperands(); i < e; ++i) {
      Worklist.push_back(CurOpI->getOperand(i));
    }
  } // End of Worklist loop

  // Now we need to move all detected operands in the proper order. Therefore
  // we traverse the list of basic blocks in reverse order and scan each
  // instruction in each basic block: if such an instruction is a needed operand
  // we move before the target instruction
  unsigned movedInsts = 0;
  for (llvm::SmallVector<llvm::BasicBlock*, 8>::reverse_iterator BBRIT = BBOrdering.rbegin(),
      BBREND = BBOrdering.rend(); BBRIT != BBREND; ++BBRIT) {
    llvm::BasicBlock *CurBB = *BBRIT;

    llvm::BasicBlock::iterator INSIT = CurBB->begin(), INSEND = CurBB->end();
    if (CurBB == TargetBB) {
      INSIT = llvm::BasicBlock::iterator(TargetI);
      // We are skipping the target instruction itself as we do not move it
      ++INSIT;
    } else if (CurBB == SourceBB) {
      INSEND = llvm::BasicBlock::iterator(SourceI);
      // As we are using an exclusive end iterator
      ++INSEND;
    }
    
    // Now iterate over all instructions
    for ( ; INSIT != INSEND; ) {
      llvm::Instruction *CurI = &*INSIT;
      ++INSIT;

      if (NeededOps.count(CurI) <= 0) {
        // This instruction should not be moved
        continue;
      }

      // Move the instruction - I hope that we do not have to update AliasAnalysis
      // if we are using moveBefore...
      CurI->moveBefore(TargetI);
      ++movedInsts;
    }
  }

  return movedInsts;
}

