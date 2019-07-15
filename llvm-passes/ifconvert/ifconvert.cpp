//===- ifconvert.cpp - if-conversion --------------------------------------===//
//===----------------------------------------------------------------------===//
//===-- Dejan Lukic  ------------------------------------------------------===//
//===-- April 2019  -------------------------------------------------------===//
//===-- Code from https://github.com/sifrrich/oclacc-llvm/ ----------------===//

#include "HDLFlattenCFG.h"
#include "LoopusUtils.h"
#include "NameMangling.h"
//#include "OpenCLMDKernels.h"
#define DEBUG(arg) arg//debug is avoided because it is not compiled due to insufficient resources

//#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
//#include "llvm/Support/raw_ostream.h"

#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Operator.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <algorithm>
#include <limits>
#include <set>

using namespace llvm;

#define DEBUG_TYPE "hdl-flattencfg"


// Add command line parameter for maximum size of speculated BBs
// The maximum size for a block that can be speculated. If the size is greater
// than the value defined by this option the block (can but) should not be
// speculated. The options does not define how the size is defined and measured
// but just defines the threshold.
cl::opt<unsigned> MaxSpecBlockSize("maxspecbbsize", cl::desc("Maximum size that a speculatively executed basic block may have."), cl::Optional, cl::init(std::numeric_limits<unsigned>::max()));
// If this option ist set (to true) it is assumed that load instructions can
// always be speculated.
cl::opt<bool> AlwaysSpeculateLoad("alwaysspecload", cl::desc("Assume that load instructions can always be executed speculatively."), cl::Optional, cl::init(false));

STATISTIC(StatsNumTrianglesSeen, "Number of seen triangles for if-conversion.");
STATISTIC(StatsNumDiamondsSeen, "Number of seen diamonds for if-conversion.");
STATISTIC(StatsNumFansSeen, "Number of seen fans for if-conversion.");
STATISTIC(StatsNumTrianglesConverted, "Number of if-converted triangles.");
STATISTIC(StatsNumDiamondsConverted, "Number of if-converted diamonds.");
STATISTIC(StatsNumFansConverted, "Number of if-converted fans.");
STATISTIC(StatsNumBlocksSpeculated, "Number of speculated blocks.");
STATISTIC(StatsNumBlocksRemoved, "Number of removed blocks.");
STATISTIC(StatsNumPhisAdapted, "Number of PHI nodes with adapted operands.");
STATISTIC(StatsNumPhisReplaced, "Number of PHI nodes that were replaced by selects.");
STATISTIC(StatsNumDomTreeIterations, "Number of iterations over the DOM Tree.");

//===- Implementation of worker class -------------------------------------===//
Loopus::HDLFlattenCFGWorker::HDLFlattenCFGWorker(AliasAnalysis *AlAnal,
    const DataLayout *DaLa, DominatorTree *DomTree, LoopInfo *LoIn, Pass *Pas,
    ScalarEvolution *ScalEv, Loopus::RessourceEstimatorBase *RessEst)
     : AA(AlAnal), DL(DaLa), DT(DomTree), LI(LoIn), P(Pas), SE(ScalEv), RE(RessEst) {
}

bool Loopus::HDLFlattenCFGWorker::runOnBasicBlock(BasicBlock *BB) {
  if ((DT == nullptr) || (RE == nullptr)) { return false; }
  if (BB == nullptr) { return false; }

  IfPattern ifpattern;
  SwitchPattern switchpattern;
  if (isHeadOfIfPattern(BB, &ifpattern) == true) {
    DEBUG(errs() << "   pat: ty=if\n"
                 << "        head=" << ifpattern.getHead()->getName() << "\n"
                 << "        tbb=" << ((ifpattern.getTBB() == nullptr) ? ("<null>") : (ifpattern.getTBB()->getName())) << "\n"
                 << "        fbb=" << ((ifpattern.getFBB() == nullptr) ? ("<null>") : (ifpattern.getFBB()->getName())) << "\n"
                 << "        tail=" << ifpattern.getTail()->getName() << "\n");
    if ((ifpattern.canIfConvert(DL) == true)
     && (ifpattern.shouldIfConvert(*RE) == true)) {
      // Update statistics
      if (ifpattern.isTriangle() == true) {
        DEBUG(errs() << "        subty=triangle\n");
        ++StatsNumTrianglesSeen;
      } else if (ifpattern.isDiamond() == true) {
        DEBUG(errs() << "        subty=diamond\n");
        ++StatsNumDiamondsSeen;
      } else {
        DEBUG(errs() << " Convertible unknown if-pattern with head at "
            << ifpattern.getHead()->getName() << " found... WTF?!\n");
      }
      // Perform if-conversion
      DEBUG(errs() << "  stat: enabled=true\n");
      ifpattern.doIfConversion(DT, AA, LI, P, SE);
      return true;
    } else {
      DEBUG(errs() << ">>Unconvertible unknown if-pattern with head at "
          << ifpattern.getHead()->getName() << " found\n");
    }

  } else if (isHeadOfSwitchPattern(BB, &switchpattern) == true) {
    DEBUG(errs() << "   pat: ty=switch\n"
                 << "        head=" << switchpattern.getHead()->getName() << "\n"
                 << "        def=" << switchpattern.hasDedicatedDefault() << "\n"
                 << "        tail=" << switchpattern.getTail()->getName() << "\n");
    if ((switchpattern.canIfConvert(DL) == true)
     && (switchpattern.shouldIfConvert(*RE) == true)) {
      // Update statistics
      DEBUG(errs() << "  stat: enabled=true\n");
      ++StatsNumFansSeen;
      // Perform if-conversion
      switchpattern.doIfConversion(DT, AA, LI, P, SE);
      return true;
    } else {
      DEBUG(errs() << ">>Unconvertible unknown switch-pattern with head at "
          << switchpattern.getHead()->getName() << " found\n");
    }
  }
  return false;
}

bool Loopus::HDLFlattenCFGWorker::runOnFunction(Function *F) {
  if ((DT == nullptr) || (RE == nullptr)) { return false; }
  if (F == nullptr) { return false; }

  bool GlobalChange = false;
  bool LocalChange = false;

  do {
    LocalChange = false;

    DEBUG(errs() << "New DOMTree iteration...\n");
    // Iterate over all blocks in postorder of the dominator tree: like this we
    // only erase blocks in case of a conversion that we already visited
    for (po_iterator<DominatorTree*> DTIT = po_begin(DT), DTEND = po_end(DT);
        DTIT != DTEND; ++DTIT) {
      // DEBUG(errs() << "Domtreeit at " << DTIT->getBlock()->getName() << "\n");
      bool MadeChanges = runOnBasicBlock(DTIT->getBlock());
      LocalChange |= MadeChanges;
      GlobalChange |= MadeChanges;
    }
    ++StatsNumDomTreeIterations;

    // Might be that we need to recompute the DOM Tree...
    // DT->recalculate(F);
  } while (LocalChange == true);

  return GlobalChange;
}

//===- Implementation of additional helper functions ----------------------===//
/// \brief Determines if \c BB is the head of a possibly valid if pattern.
/// \note Only rough validity and consistency checks are performed.
/// \param BB The possible head basic block of the pattern.
/// \param Pattern The \c IfPattern object to store the found pattern in.
///
/// Determines if \c BB could be the head of a possible triangle or diamond.
/// Note that only rough validity checks are performed. So it might be that
/// \c BB is head of a pattern but it is not possible to if-convert that
/// pattern. If \c BB is head of a pattern \c true is returned. In that case
/// \c Pattern is filled if provided. In other cases \c false is returned and
/// \c Pattern remains unchanged.
///
/// So the function recognizes the following patterns:
/// doA();              doA();
/// if (condition) {    if (condition) {
///   doB();              doB();
/// }                   } else {
/// doD();                doC();
///                     }
///                     doD();
bool Loopus::isHeadOfIfPattern(BasicBlock *BB, IfPattern *Pattern) {
  if (BB == nullptr) { return false; }

  // Get terminator for head basic block
  const Instruction *BBTI = BB->getTerminator();
  if (BBTI == nullptr) { return false; }
  // Check if terminator is a BranchInst (and not a switch,...)
  const BranchInst *BBBI = dyn_cast<BranchInst>(BBTI);
  if (BBBI == nullptr) { return false; }
  if (BBBI->isConditional() == false) { return false; }

  // In both cases (triangle and diamond) the head must exactly have
  // two successors
  const unsigned BBNumSuccessors = BBTI->getNumSuccessors();
  if (BBNumSuccessors != 2) { return false; }

  // Determine successors of BB
  BasicBlock *Succ0 = BBTI->getSuccessor(0);
  BasicBlock *Succ1 = BBTI->getSuccessor(1);
  assert (Succ0 != nullptr && "Successor 0 must not be null");
  assert (Succ1 != nullptr && "Successor 1 must not be null");
  if ((Succ0 == nullptr) || (Succ1 == nullptr)) { return false; }

  // Determine successors terminator instructions
  const Instruction *Succ0TI = Succ0->getTerminator();
  const Instruction *Succ1TI = Succ1->getTerminator();
  assert(Succ0TI != nullptr && "Terminator of successor 0 must not be null");
  assert(Succ1TI != nullptr && "Terminator of successor 1 must not be null");
  if ((Succ0TI == nullptr) || (Succ1TI == nullptr)) { return false; }

  const unsigned Succ0NumSuccessors = Succ0TI->getNumSuccessors();
  const unsigned Succ1NumSuccessors = Succ1TI->getNumSuccessors();

  if ((Succ0NumSuccessors == 1) && (Succ1NumSuccessors == 1)) {
    BasicBlock *Succ0Succ = Succ0TI->getSuccessor(0);
    BasicBlock *Succ1Succ = Succ1TI->getSuccessor(0);

    if (Succ0Succ == Succ1) {
      // We found a triangle
      if (Loopus::getNumPredecessors(Succ0) != 1) { return false; }
      // Write result to pattern if available
      if (Pattern != nullptr) {
        Pattern->setHead(BB);
        Pattern->setTBB(Succ0);
        Pattern->setFBB(nullptr);
        Pattern->setTail(Succ1);
      }
      return true;
    } else if (Succ1Succ == Succ0) {
      // We found a triangle
      if (Loopus::getNumPredecessors(Succ1) != 1) { return false; }
      // Write result to pattern if available
      if (Pattern != nullptr) {
        Pattern->setHead(BB);
        Pattern->setTBB(nullptr);
        Pattern->setFBB(Succ1);
        Pattern->setTail(Succ0);
      }
      return true;
    } else if (Succ0Succ == Succ1Succ) {
      // We found a diamond
      if (Loopus::getNumPredecessors(Succ0) != 1) { return false; }
      if (Loopus::getNumPredecessors(Succ1) != 1) { return false; }
      // Write result to pattern if available
      if (Pattern != nullptr) {
        Pattern->setHead(BB);
        Pattern->setTBB(Succ0);
        Pattern->setFBB(Succ1);
        Pattern->setTail(Succ0Succ);
      }
      return true;
    }
  } else if ((Succ0NumSuccessors == 1) && (Succ1NumSuccessors != 1)) {
    // This can just be a triangle...
    if (Loopus::getSingleSuccessor(Succ0) == Succ1) {
      // We found a triangle
      if (Loopus::getNumPredecessors(Succ0) != 1) { return false; }
      // Write result to pattern if available
      if (Pattern != nullptr) {
        Pattern->setHead(BB);
        Pattern->setTBB(Succ0);
        Pattern->setFBB(nullptr);
        Pattern->setTail(Succ1);
      }
      return true;
    }
  } else if ((Succ0NumSuccessors != 1) && (Succ1NumSuccessors == 1)) {
    // This can just be a triangle...
    if (Loopus::getSingleSuccessor(Succ1) == Succ0) {
      // We found a triangle
      if (Loopus::getNumPredecessors(Succ1) != 1) { return false; }
      // Write result to pattern if available
      if (Pattern != nullptr) {
        Pattern->setHead(BB);
        Pattern->setTBB(nullptr);
        Pattern->setFBB(Succ1);
        Pattern->setTail(Succ0);
      }
      return true;
    }
  } else if ((Succ0NumSuccessors != 1) && (Succ1NumSuccessors != 1)) {
    // No (easy) pattern possible for this CFG
    return false;
  }

  return false;
}

/// \brief Determines if \c BB is the head of a possibly valid switch pattern.
/// \note Only rough validity and consistency checks are performed.
/// \param BB The possible head basic block of the switch pattern.
/// \param Pattern The \c SwitchPattern object to store the found pattern in.
///
/// Determines if \c BB could be the head of a possible switch pattern.
/// Note that only rough validity checks are performed. So it might be that
/// \c BB is head of a pattern but it is not possible to if-convert that
/// pattern. If \c BB is head of a pattern \c true is returned. In that case
/// \c Pattern is filled if provided. In other cases \c false is returned and
/// \c Pattern remains unchanged.
///
/// So the function recognizes the following patterns:
/// doA();                   doA();
/// switch(variable) {       switch(variable) {
///   case 0:                  case 0: case 1: case 2:
///     doB();                   doB();
///     break;                   break;
///   case 3:                  case 3: case 4: case 5:
///     doC();                   doC();
///     break;                   break;
///   default:                 default:
///     doD();                   doD();
///     break;                   break;
/// }                        }
/// doE();                   doE();
bool Loopus::isHeadOfSwitchPattern(BasicBlock *BB, SwitchPattern *Pattern) {
  if (BB == nullptr) { return false; }

  // Get terminator for head basic block
  Instruction *BBTI = BB->getTerminator();
  if (BBTI == nullptr) { return false; }
  // Check if terminator is a SwitchInst (and not a branch,...)
  SwitchInst *BBSI = dyn_cast<SwitchInst>(BBTI);
  if (BBSI == nullptr) { return false; }

  // Check that the number of successors is consistent
  const unsigned BBNumSuccessors = BBTI->getNumSuccessors();
  if (BBNumSuccessors != BBSI->getNumCases() + 1) { return false; }

  // Now the tricky part: determine the tail
  // First we are looking for two different blocks
  BasicBlock *BB1 = BBSI->getDefaultDest();
  if (BB1 == nullptr) { return false; }
  BasicBlock *BB2 = nullptr;

  for (SwitchInst::CaseIt CSIT = BBSI->case_begin(), CSEND = BBSI->case_end();
      CSIT != CSEND; ++CSIT) {
    //deki_edit
    //BasicBlock *CaseBB = CSIT.getCaseSuccessor();
    BasicBlock *CaseBB = CSIT->getCaseSuccessor();
    if (CaseBB == nullptr) { continue; }
    Instruction *CaseTI = CaseBB->getTerminator();
    if (CaseTI == nullptr) { return false; }

    // Each case block must have exactly one successor (tail) but may have
    // several predecessors
    if (BB2 == nullptr) {
      if (CaseBB != BB1) {
        BB2 = CaseBB;
        break;
      }
    }
  }

  // Now its easy
  BasicBlock *TailBB = nullptr;
  if (BB2 != nullptr) {
    Instruction *BB1TI = BB1->getTerminator();
    Instruction *BB2TI = BB2->getTerminator();
    if (BB1TI->getNumSuccessors() != 1) {
      // BB1 must be our successor as it has two or more or zero successors.
      TailBB = BB1;
    } else if (BB2TI->getNumSuccessors() != 1) {
      // Same here...
      TailBB = BB2;
    } else {
      // Both blocks have exactly one successor
      if (BB2TI->getSuccessor(0) == BB1) {
        TailBB = BB1;
      } else if (BB1TI->getSuccessor(0) == BB2) {
        TailBB = BB2;
      } else if (BB2TI->getSuccessor(0) == BB1TI->getSuccessor(0)) {
        TailBB = BB2TI->getSuccessor(0);
      } else {
        return false;
      }
    }
  } else {
    // BB2 is null so we did not find any other block than BB1. So BB1 (the
    // default case block) is the same as all case blocks. So all of them
    // are branching to the same block which is considered as tail block.
    TailBB = BB1;
  }

  // Check that all case blocks have the tail as their successor
  for (SwitchInst::CaseIt CSIT = BBSI->case_begin(), CSEND = BBSI->case_end();
      CSIT != CSEND; ++CSIT) {
    //BasicBlock *CaseBB = CSIT.getCaseSuccessor();
    //deki_edit
    BasicBlock *CaseBB = CSIT->getCaseSuccessor();
    if (CaseBB == nullptr) { continue; }
    Instruction *CaseTI = CaseBB->getTerminator();
    if (CaseTI == nullptr) { return false; }

    if (CaseBB == TailBB) { continue; }

    // Each case block (except the tail block itself) must have exactly one
    // successor (tail) but may have several predecessors (when several cases
    // are branching into the same case block).
    if (CaseTI->getNumSuccessors() != 1) { return false; }

    if (Loopus::getSingleSuccessor(CaseBB) != TailBB) { return false; }
  }

  // Now we know that all case blocks have TailBB as their successor
  // Now check the default case
  BasicBlock *DefBB = BBSI->getDefaultDest();
  if (DefBB == nullptr) { return false; }
  if (DefBB == TailBB) {
    // There is no dedicated default case so nothing to check here...
  } else {
    // There is a dedicated default case. So same checks as for the cases
    const Instruction *DefTI = DefBB->getTerminator();
    if (DefTI == nullptr) { return false; }
    // Exactly one successor (and not quite sure but I think several
    // predecessors are possible
    if (DefTI->getNumSuccessors() != 1) { return false; }
    // Check that DefTI ends in TailBB
    if (DefTI->getSuccessor(0) != TailBB) { return false; }
  }

  // So we found a switch pattern
  if (Pattern != nullptr) {
    Pattern->setHead(BB);
    Pattern->setTail(TailBB);
  }

  return true;
}

/// \brief Determines if a BB can be safely executed speculatively.
/// \param BB The basic block under consideration.
bool Loopus::canExecuteSpeculatively(const BasicBlock *BB, const DataLayout *DL) {
  if (BB == nullptr) { return false; }

  // Test each instruction but not the terminator
  for (BasicBlock::const_iterator INSIT = BB->begin(),
      INSEND = std::prev(BB->end()); INSIT != INSEND; ++INSIT) {
    if (canExecuteSpeculatively(&*INSIT, DL) == false) {
      return false;
    }
  }

  return true;
}

bool Loopus::canExecuteSpeculatively(const Value *V, const DataLayout *DL) {
  if (V == nullptr) { return false; }

  const Operator *I = dyn_cast<Operator>(V);
  if (I == nullptr) {
    return false;
  }

  // Assuming debug info can be speculated
  if (isa<DbgInfoIntrinsic>(I) == true) {
    return true;
  }

  // Test for barrier function calls to be unsafe. Other function calls
  // are considered to be safe: there should not be function calls to other
  // non-kernel functions as those should already be inlined. ID-related builtin
  // calls are replaced by function parameters and arithmetic builtins are safe
  // to be speculated
  const unsigned iopcode = I->getOpcode();
  if (iopcode == Instruction::Call) {
    const CallInst *CI = dyn_cast<CallInst>(I);
    if (CI == nullptr) { return false; }
    const Function *calledfunc = CI->getCalledFunction();
    const std::string funcname = calledfunc->getName();
    if (ocl::NameMangling::isSynchronizationFunction(funcname) == true) {
      // The function is a synchronization function and we must not speculate
      // synchronization points
      return false;
    } else {
      if ((calledfunc->doesNotThrow() == true) && (calledfunc->onlyReadsMemory() == true)) {
        // This not really clean as the function might still have side-effects
        // but it is currently the best way to allow arithmetic functions to be
        // called without preventing transformation of found patterns...
        return true;
      }
    }
    return true;

  } else if (iopcode == Instruction::Load) {
    if (AlwaysSpeculateLoad == true) {
      return true;
    } else {
      return false;
    }
  } else if (iopcode == Instruction::Store) {
    return false;
  }
  //deki_edit
  // see arguments for isSafeToSpeculativelyExecute, because they are different
  //return isSafeToSpeculativelyExecute(I, DL);
  // for now use this, it is probably safe because argument 2 and 3 are only used for load instruction (see line 475)
  return isSafeToSpeculativelyExecute(V, nullptr, nullptr);
}

//===- Implementation of IfPattern class ----------------------------------===//
/// \brief Creates a new empty pattern.
Loopus::IfPattern::IfPattern(void)
 : HeadBB(nullptr), TBB(nullptr), FBB(nullptr), TailBB(nullptr) {
}

/// \brief Creates a new IfPattern with the given head and tail.
/// \note No checks for validity are performed!
Loopus::IfPattern::IfPattern(BasicBlock *Head, BasicBlock *Tail)
 : HeadBB(Head), TBB(nullptr), FBB(nullptr), TailBB(Tail) {
}

/// \brief Creates a new IfPattern with the given head and tail.
/// \note No checks for validity are performed!
///
/// Creates a new pattern representing an if-then-else construct.
Loopus::IfPattern::IfPattern(BasicBlock *Head, BasicBlock *Tail, BasicBlock *TrueBB,
    BasicBlock *FalseBB)
 : HeadBB(Head), TBB(TrueBB), FBB(FalseBB), TailBB(Tail) {
}

BasicBlock* Loopus::IfPattern::getHead(void)              { return HeadBB; }
const BasicBlock* Loopus::IfPattern::getHead(void) const  { return HeadBB; }
BasicBlock* Loopus::IfPattern::getTBB(void)               { return TBB; }
const BasicBlock* Loopus::IfPattern::getTBB(void) const   { return TBB; }
BasicBlock* Loopus::IfPattern::getFBB(void)               { return FBB; }
const BasicBlock* Loopus::IfPattern::getFBB(void) const   { return FBB; }
BasicBlock* Loopus::IfPattern::getTail(void)              { return TailBB; }
const BasicBlock* Loopus::IfPattern::getTail(void) const  { return TailBB; }

void Loopus::IfPattern::setHead(BasicBlock *newHead)  { HeadBB = newHead; }
void Loopus::IfPattern::setTBB(BasicBlock *newTBB)    { TBB = newTBB; }
void Loopus::IfPattern::setFBB(BasicBlock *newFBB)    { FBB = newFBB; }
void Loopus::IfPattern::setTail(BasicBlock *newTail)  { TailBB = newTail; }

/// \brief If the head block has only a TBB or a FBB it is returned.
///
/// If the head of the if-pattern only has a TBB or a FBB then it is returned.
/// Else (if both or none of them exists) null is returned.
BasicBlock* Loopus::IfPattern::getSingleBranchedBB(void) {
  if ((TBB == nullptr) && (FBB != nullptr)) {
    return FBB;
  } else if ((TBB != nullptr) && (FBB == nullptr)) {
    return TBB;
  } else {
    return nullptr;
  }
}

/// \brief If the head block has only a TBB or a FBB it is returned.
///
/// If the head of the if-pattern only has a TBB or a FBB then it is returned.
/// Else (if both or none of them exists) null is returned.
const BasicBlock* Loopus::IfPattern::getSingleBranchedBB(void) const {
  if ((TBB == nullptr) && (FBB != nullptr)) {
    return FBB;
  } else if ((TBB != nullptr) && (FBB == nullptr)) {
    return TBB;
  } else {
    return nullptr;
  }
}

/// \brief Returns \c true if the pattern is a triangle.
/// \note The validity and consistency are not checked.
bool Loopus::IfPattern::isTriangle(void) const {
  if ((HeadBB != nullptr) && (TailBB != nullptr)) {
    if (((TBB != nullptr) && (FBB == nullptr))
     || ((TBB == nullptr) && (FBB != nullptr))) {
      return true;
    }
  }
  return false;
}

/// \brief Returns \c true if the pattern is a diamond.
/// \note The validity and consistency are not checked.
bool Loopus::IfPattern::isDiamond(void) const {
  if ((HeadBB != nullptr) && (TailBB != nullptr)) {
    if ((TBB != nullptr) && (FBB != nullptr)) {
      return true;
    }
  }
  return false;
}

/// \brief Determines if the represented if-pattern can be if-converted.
bool Loopus::IfPattern::canIfConvert(const DataLayout *DL) const {
  // First validate the shape of the pattern
  if ((HeadBB == nullptr) || (TailBB == nullptr)) { return false; }

  // Get terminator for head basic block
  const Instruction *HeadTI = HeadBB->getTerminator();
  assert(HeadTI != nullptr && "Head BB does not have a valid terminator!");
  if (HeadTI == nullptr) { return false; }

  // Terminator must be a BranchInst (and not a switch,...)
  const BranchInst *HeadBI = dyn_cast<BranchInst>(HeadTI);
  if (HeadBI == nullptr) {
    DEBUG(errs() << "  Cannot convert as terminator of head BB is not a "
        << "BranchInst!\n");
    return false;
  }
  if (HeadBI->isConditional() == false) {
    DEBUG(errs() << "  Cannot convert pattern with non-conditional branch!\n");
    return false;
  }

  // In both cases (triangle and diamond) the head must exactly have
  // two successors
  const unsigned HeadNumSuccessors = HeadTI->getNumSuccessors();
  if (HeadNumSuccessors != 2) {
    DEBUG(errs() << "  Cannot convert as head BB does not exactly have two "
        << "successors!\n");
    return false;
  }

  // First we have to check if the triangle/diamond is properly shaped
  if (isTriangle() == true) {
    const BasicBlock *BranchedBB = getSingleBranchedBB();
    assert(BranchedBB != nullptr && "Branch for triangle must not be null!");
    if (BranchedBB == nullptr) { return false; }

    // Make sure that we do not have any loops: so all blocks need to differ
    if ((HeadBB == BranchedBB) || (HeadBB == TailBB)
     || (BranchedBB == TailBB)) {
      return false;
    }

    const Instruction *BranchedBBTI = BranchedBB->getTerminator();
    assert (BranchedBBTI != nullptr && "Terminator of branched BB must not be null!");
    if (BranchedBBTI == nullptr) { return false; }
    const BranchInst *BranchedBBBI = dyn_cast<BranchInst>(BranchedBBTI);
    if (BranchedBBBI == nullptr) {
      DEBUG(errs() << "  BB in branch must end with a BranchInst!\n");
      return false;
    }
    if (BranchedBBBI->isUnconditional() == false) {
      DEBUG(errs() << "  BB in branch must end with an unconditional branch!\n");
      return false;
    }

    // As block in branch must have head as single predecessor there must not be
    // any phis in it
    if (isa<PHINode>(BranchedBB->front()) == true) { return false; }

    // BB in branch of triangle must have head BB as single predecessor
    if (BranchedBB->getSinglePredecessor() != HeadBB) { return false; }

    // BB in branch of triangle must have tail BB as single successor
    if (Loopus::getSingleSuccessor(BranchedBB) != TailBB) { return false; }

    // Test successor of head BB (we already know that head BB has exactly two)
    if (HeadTI->getSuccessor(0) == BranchedBB) {
      if (BranchedBB != TBB) { return false; }
      if (FBB != nullptr) { return false; }
      if (HeadTI->getSuccessor(1) != TailBB) { return false; }
    } else if (HeadTI->getSuccessor(1) == BranchedBB) {
      if (BranchedBB != FBB) { return false; }
      if (TBB != nullptr) { return false; }
      if (HeadTI->getSuccessor(0) != TailBB) { return false; }
    } else {
      return false;
    }

    // Tail BB may have an arbitrary number of predecessors (at least 2) and
    // successors
    if (Loopus::getNumPredecessors(TailBB) < 2) { return false; }

  } else if (isDiamond() == true) {
    assert(TBB != nullptr && "True branch for diamond must not be null!");
    assert(FBB != nullptr && "False branch for diamond must not be null!");
    if ((TBB == nullptr) || (FBB == nullptr)) { return false; }

    // Make sure that all blocks are different
    if ((HeadBB == TBB) || (HeadBB == FBB) || (HeadBB == TailBB)
     || (TBB == FBB) || (TBB == TailBB)
     || (FBB == TailBB)) {
      return false;
    }

    const BranchInst *TBBBI = dyn_cast<BranchInst>(TBB->getTerminator());
    if (TBBBI == nullptr) {
      DEBUG(errs() << "  TBB must end with a BranchInst!\n");
      return false;
    }
    if (TBBBI->isUnconditional() == false) {
      DEBUG(errs() << "  TBB must end with an unconditional branch!\n");
      return false;
    }
    if (isa<PHINode>(TBB->front()) == true) { return false; }

    const BranchInst *FBBBI = dyn_cast<BranchInst>(FBB->getTerminator());
    if (FBBBI == nullptr) {
      DEBUG(errs() << "  FBB must end with a BranchInst!\n");
      return false;
    }
    if (FBBBI->isUnconditional() == false) {
      DEBUG(errs() << "  FBB must end with an unconditional branch!\n");
      return false;
    }
    if (isa<PHINode>(FBB->front()) == true) { return false; }

    // Both branches must have head BB as single predecessor and tail BB as
    // single successor
    if (TBB->getSinglePredecessor() != HeadBB) { return false; }
    if (Loopus::getSingleSuccessor(TBB) != TailBB) { return false; }
    if (FBB->getSinglePredecessor() != HeadBB) { return false; }
    if (Loopus::getSingleSuccessor(FBB) != TailBB) { return false; }

    // Test successors of head BB
    if (HeadTI->getSuccessor(0) != TBB) { return false; }
    if (HeadTI->getSuccessor(1) != FBB) { return false; }

    // Tail BB may have an arbitrary number of predecessors (at least 2) and
    // successors
    if (Loopus::getNumPredecessors(TailBB) < 2) { return false; }

  } else {
    // The pattern is neither a triangel nor a diamond
    return false;
  }

  // Now test if all instructions in TBB and FBB can be executed speculatively
  // and test their sizes
  if (TBB != nullptr) {
    // First make sure that there are no PHI nodes in TBB
    if (isa<PHINode>(TBB->front()) == true) {
      DEBUG(errs() << "  Cannot speculate TBB block with PHIs in it!\n");
      return false;
    }
    // Test if TBB can be speculated
    if (canExecuteSpeculatively(TBB, DL) == false) {
      DEBUG(errs() << "  Cannot speculatively execute BB " << TBB->getName()
          << "!\n");
      return false;
    }
  }
  if (FBB != nullptr) {
    // First make sure that there are no PHI nodes in FBB
    if (isa<PHINode>(FBB->front()) == true) {
      DEBUG(errs() << "  Cannot speculate FBB block with PHIs in it!\n");
      return false;
    }
    // Test if FBB can be speculated
    if (canExecuteSpeculatively(FBB, DL) == false) {
      DEBUG(errs() << "  Cannot speculatively execute BB " << FBB->getName()
          << "!\n");
      return false;
    }
  }

  // Test that Tail BB has any PHI nodes. If it does not have any there must be
  // other side effects. If tail BB does not have PHIs and both blocks do not
  // have side effects their code does not change the program state in any way
  // and could be omitted. So we are assuming that there are other side effects
  // if tail BB does not have any PHI nodes.
  bool hasPHIs = false;
  for (BasicBlock::const_iterator INSIT = TailBB->begin(),
      INSEND = TailBB->end(); INSIT != INSEND; ++INSIT) {

    // Break out if all phi nodes are tested
    const PHINode *PN = dyn_cast<PHINode>(&*INSIT);
    if (PN == nullptr) { break; }

    const Value *ThenValue = nullptr;
    const Value *ElseValue = nullptr;
    if (isTriangle() == true) {
      ThenValue = PN->getIncomingValueForBlock(getSingleBranchedBB());
      ElseValue = PN->getIncomingValueForBlock(HeadBB);
    } else if (isDiamond() == true) {
      ThenValue = PN->getIncomingValueForBlock(TBB);
      ElseValue = PN->getIncomingValueForBlock(FBB);
    } else {
      // Already checked above but just to be sure...
      return false;
    }
    if ((ThenValue == nullptr) || (ElseValue == nullptr)) { return false; }
    hasPHIs = true;

    const ConstantExpr *ThenCE = dyn_cast<ConstantExpr>(ThenValue);
    if (ThenCE != nullptr) {
      if (canExecuteSpeculatively(ThenCE, DL) == false) {
        return false;
      }
    }
    const ConstantExpr *ElseCE = dyn_cast<ConstantExpr>(ElseValue);
    if (ElseCE != nullptr) {
      if (canExecuteSpeculatively(ElseCE, DL) == false) {
        return false;
      }
    }

  }
  if (hasPHIs == false) {
    // There are no phis so assume that there are other side effects what means
    // that the block cannot be speculated
    // XXX: We might want to omit this test...
    DEBUG(errs() << "  Cannot convert pattern as no PHIs in tail BB "
        << TailBB->getName() << " were found so assuming side effects!\n");
    return false;
  }

  // Currently I don't know any other usefull tests...

  return true;
}

/// \brief Determines if the pattern should be converted.
///
/// Computes the costs of the conversion and determines if the pattern should be
/// converted or not.
bool Loopus::IfPattern::shouldIfConvert(Loopus::RessourceEstimatorBase &RE) const {
  if (TBB != nullptr) {
    const unsigned TBBSize = RE.getRessourceUsage(TBB);
    DEBUG(errs() << "    sz: tbb=" << TBBSize << "\n");
    if (TBBSize > MaxSpecBlockSize) {
      DEBUG(errs() << "  Will not speculate BB " << TBB->getName() << " as it "
          << "is too large...\n");
      return false;
    }
  }
  if (FBB != nullptr) {
    const unsigned FBBSize = RE.getRessourceUsage(FBB);
    DEBUG(errs() << "        fbb=" << FBBSize << "\n");
    if (FBBSize > MaxSpecBlockSize) {
      DEBUG(errs() << "  Will not speculate BB " << FBB->getName() << " as it "
          << "is too large...\n");
      return false;
    }
  }
  return true;
}

/// \brief Aapts the operands of the phi nodes in the tail BB.
bool Loopus::IfPattern::adaptPhiOps(AliasAnalysis *AA) {
  // We need to do:
  // 1. Add select instruction in head BB to switch between phi operands for
  //    TBB and FBB.
  // 2. Remove phi operands for TBB and FBB
  // 3. Add new phi operand for head BB with value of select instruction
  Instruction *HeadTI = HeadBB->getTerminator();
  if (HeadTI == nullptr) { return false; }
  BranchInst *HeadTBI = dyn_cast<BranchInst>(HeadTI);
  if (HeadTI == nullptr) { return false; }
  if (HeadTBI->isConditional() == false) { return false; }
  Value *TBCondition = HeadTBI->getCondition();
  if (TBCondition == nullptr) { return false; }

  bool changed = false;
  DEBUG(errs() << "   phi: method=adapt\n"
               << "        bb=" << TailBB->getName() << "\n");
  for (BasicBlock::iterator INSIT = TailBB->begin(), INSEND = TailBB->end();
      INSIT != INSEND; ++INSIT) {
    PHINode *PN = dyn_cast<PHINode>(&*INSIT);
    if (PN == nullptr) {
      // We processed all PHI Nodes
      break;
    }

    if (isDiamond() == true) {
      // For a diamond we need the values for TBB and FBB
      Value *TBBV = PN->getIncomingValueForBlock(TBB);
      Value *FBBV = PN->getIncomingValueForBlock(FBB);
      if ((TBBV == nullptr) || (FBBV == nullptr)) { continue; }

      // Now update the phi node in tail BB
      if (TBBV == FBBV) {
        // The values from TBB and FBB are same so we do not need a select
        // but simply use them directly in the phi node
        PN->removeIncomingValue(TBB, false);
        PN->removeIncomingValue(FBB, false);
        PN->addIncoming(TBBV, HeadBB);
        changed = true;
      } else {
        // The values from TBB and FBB are different so we need a select
        SelectInst *SI = SelectInst::Create(TBCondition, TBBV, FBBV, "", HeadTI);
        if (PN->hasName() == true) {
          SI->setName(PN->getName() + ".preselect");
        }
        AAMDNodes TBAAMDNode;
        PN->getAAMetadata(TBAAMDNode, false);
        SI->setAAMetadata(TBAAMDNode);
        SI->setDebugLoc(PN->getDebugLoc());
        if (AA != nullptr) {
          //AA->copyValue(PN, SI);
        }

        PN->removeIncomingValue(TBB, false);
        PN->removeIncomingValue(FBB, false);
        PN->addIncoming(SI, HeadBB);
        changed = true;
      }

    } else if (isTriangle() == true) {
      // For a triangle we need the values for the head BB and the one for the
      // single branched block
      BasicBlock *BranchedBB = getSingleBranchedBB();
      if (BranchedBB == nullptr) { continue; }
      Value *HBBV = PN->getIncomingValueForBlock(HeadBB);
      Value *BBBV = PN->getIncomingValueForBlock(BranchedBB);
      if ((HBBV == nullptr) || (BBBV == nullptr)) { continue; }

      // Now update the phi node in tail BB
      if (HBBV == BBBV) {
        // Values from HeadBB and branched block are equal so we do not need a
        // select but simply use the value(s) directly in the phi node
        PN->removeIncomingValue(HeadBB, false);
        PN->removeIncomingValue(BranchedBB, false);
        PN->addIncoming(HBBV, HeadBB);
        changed = true;
      } else {
        // The values from HeadBB and the branched block are different so we
        // need a select
        SelectInst *SI = nullptr;
        if ((TBB == nullptr) && (FBB != nullptr)) {
          SI = SelectInst::Create(TBCondition, HBBV, BBBV, "", HeadTI);
          changed = true;
        } else if ((TBB != nullptr) && (FBB == nullptr)) {
          SI = SelectInst::Create(TBCondition, BBBV, HBBV, "", HeadTI);
          changed = true;
        } else {
          continue;
        }
        if (PN->hasName() == true) {
          SI->setName(PN->getName() + ".preselect");
        }
        AAMDNodes TBAAMDNode;
        PN->getAAMetadata(TBAAMDNode, false);
        SI->setAAMetadata(TBAAMDNode);
        SI->setDebugLoc(PN->getDebugLoc());
        if (AA != nullptr) {
          //AA->copyValue(PN, SI);
        }

        PN->removeIncomingValue(HeadBB, false);
        PN->removeIncomingValue(BranchedBB, false);
        PN->addIncoming(SI, HeadBB);
        changed = true;
      }

    } else {
      continue;
    }

    // Update statistics
    ++StatsNumPhisAdapted;
  }

  return changed;
}

/// \brief Replaces the phi nodes in tail BB by select instructions.
bool Loopus::IfPattern::replacePhis(AliasAnalysis *AA) {
  // We need to do:
  // 1. Add select instruction into the HeadBB / TailBB - I'm not quite sure
  //    where to add the instruction but I think the target BB does not really
  //    matter.
  // 2. Replace all uses of the phi node by the select instruction
  Instruction *HeadTI = HeadBB->getTerminator();
  if (HeadTI == nullptr) { return false; }
  BranchInst *HeadTBI = dyn_cast<BranchInst>(HeadTI);
  if (HeadTI == nullptr) { return false; }
  if (HeadTBI->isConditional() == false) { return false; }
  Value *TBCondition = HeadTBI->getCondition();
  if (TBCondition == nullptr) { return false; }

  // Test if phi nodes can be replaced
  DEBUG(errs() << "   phi: method=replace\n"
               << "        bb=" << TailBB->getName() << "\n");
  for (BasicBlock::iterator INSIT = TailBB->begin(), INSEND = TailBB->end();
      INSIT != INSEND; ++INSIT) {
    PHINode *PN = dyn_cast<PHINode>(&*INSIT);
    if (PN == nullptr) {
      // We processed all PHI Nodes
      break;
    }

    // Make sure that the phi node really has TBB and FBB as input blocks
    if (PN->getNumIncomingValues() != 2) {
      DEBUG(errs() << ">>Cannot replace PHIs in " << TailBB->getName() << ". "
                   << "PHI with invalid number of in-operands found.\n");
      return false;
    }

    if (isDiamond() == true) {
      if (!((PN->getIncomingBlock(0) == TBB) && (PN->getIncomingBlock(1) == FBB))
       && !((PN->getIncomingBlock(0) == FBB) && (PN->getIncomingBlock(1) == TBB))) {
        DEBUG(errs() << ">>Cannot replace PHIs in " << TailBB->getName() << " "
                     << "as it has invalid incoming edges\n");
        return false;
      }
    } else if (isTriangle() == true) {
      BasicBlock *BBB = getSingleBranchedBB();
      if (BBB == nullptr) { return false; }
      if (!((PN->getIncomingBlock(0) == BBB) && (PN->getIncomingBlock(1) == HeadBB))
       && !((PN->getIncomingBlock(0) == HeadBB) && (PN->getIncomingBlock(1) == BBB))) {
        DEBUG(errs() << ">>Cannot replace PHIs in " << TailBB->getName() << " "
                     << "as it has invalid incoming edges\n");
        return false;
      }
    } else {
      return false;
    }
  }

  std::vector<Instruction*> PHIsToRemove;
  bool changed = false;
  for (BasicBlock::iterator INSIT = TailBB->begin(), INSEND = TailBB->end();
      INSIT != INSEND; ++INSIT) {
    PHINode *PN = dyn_cast<PHINode>(&*INSIT);
    if (PN == nullptr) {
      // We processed all PHI Nodes
      break;
    }

    if (isDiamond() == true) {
      // For a diamond we need the values for TBB and FBB
      Value *TBBV = PN->getIncomingValueForBlock(TBB);
      Value *FBBV = PN->getIncomingValueForBlock(FBB);
      if ((TBBV == nullptr) || (FBBV == nullptr)) { continue; }

      if (TBBV == FBBV) {
        // The values from TBB and FBB are same so we do not need a select
        // and as we should replace the phi node we can use the value directly
        // instead of a select or a phi node.
        PN->replaceAllUsesWith(TBBV);
        PHIsToRemove.push_back(PN);
        changed = true;
      } else {
        // The values from TBB and FBB are different so we need to replace the
        // phi node by a new select
        SelectInst *SI = SelectInst::Create(TBCondition, TBBV, FBBV, "", HeadTI);
        if (PN->hasName() == true) {
          SI->setName(PN->getName());
        }
        AAMDNodes TBAAMDNode;
        PN->getAAMetadata(TBAAMDNode, false);
        SI->setAAMetadata(TBAAMDNode);
        SI->setDebugLoc(PN->getDebugLoc());
        if (AA != nullptr) {
          //AA->copyValue(PN, SI);
        }

        PN->replaceAllUsesWith(SI);
        PHIsToRemove.push_back(PN);
        changed = true;
      }

    } else if (isTriangle() == true) {
      // For a triangle we need the values for the head BB and the one for the
      // single branched block
      BasicBlock *BranchedBB = getSingleBranchedBB();
      if (BranchedBB == nullptr) { continue; }
      Value *HBBV = PN->getIncomingValueForBlock(HeadBB);
      Value *BBBV = PN->getIncomingValueForBlock(BranchedBB);
      if ((HBBV == nullptr) || (BBBV == nullptr)) { continue; }

      if (HBBV == BBBV) {
        // The values from HeadBB and branched block are equal so we do not need
        // a select and as we should replace the phi node we can use the value
        // directly instead of a select or a phi node.
        PN->replaceAllUsesWith(HBBV);
        PHIsToRemove.push_back(PN);
        changed = true;
      } else {
        // The values from HeadBB and branched block are different so we need to
        // replace the phi node by a new select
        SelectInst *SI = nullptr;
        if ((TBB == nullptr) && (FBB != nullptr)) {
          SI = SelectInst::Create(TBCondition, HBBV, BBBV, "", HeadTI);
          changed = true;
        } else if ((TBB != nullptr) && (FBB == nullptr)) {
          SI = SelectInst::Create(TBCondition, BBBV, HBBV, "", HeadTI);
          changed = true;
        } else {
          continue;
        }
        if (PN->hasName() == true) {
          SI->setName(PN->getName());
        }
        AAMDNodes TBAAMDNode;
        PN->getAAMetadata(TBAAMDNode, false);
        SI->setAAMetadata(TBAAMDNode);
        SI->setDebugLoc(PN->getDebugLoc());
        if (AA != nullptr) {
          //AA->copyValue(PN, SI);
        }

        PN->replaceAllUsesWith(SI);
        PHIsToRemove.push_back(PN);
        changed = true;
      }

    } else {
      continue;
    }
  }

  // Now remove all phi nodes not needed any more
  for (Instruction *INS : PHIsToRemove) {
    // We do not have to check if the operands could be erased too as the
    // operands are used by the select instruction
    INS->eraseFromParent();
    if (AA != nullptr) {
      //AA->deleteValue(INS);
    }
    // Update statistics
    ++StatsNumPhisReplaced;
  }

  return changed;
}

/// \brief Updates the dominator tree in \c DT during conversion.
/// \param DT A pointer to the dominator tree.
/// \param mergedTail Indicates if the tail was merged into the header.
///
/// Updates the dominator after the TBB and FBB blocks were removed from the
/// CFG. The function then removes them from the dominator tree. If
/// \c mergedTail is \c true then the function assumes that the tail BB was
/// merged into the head BB and updates the dominator accordingly.
void Loopus::IfPattern::updateDomTree(DominatorTree *DT, bool mergedTail) {
  if (DT == nullptr) { return; }
  // We try to update the DomTree by removing the TBB and FBB (if available)
  // from the DomTree and updating their children (in the DomTree)

  DomTreeNode *RTDTN = DT->getRootNode();
  DomTreeNode *HeadDTN = DT->getNode(HeadBB);
  if (HeadDTN == nullptr) { return; }

  // Remove TBB from domtree
  if (TBB != nullptr) {
    DomTreeNode *TBBDTN = DT->getNode(TBB);
    if ((TBBDTN != nullptr) && (TBBDTN != RTDTN) && (TBBDTN != HeadDTN)) {
      if (TBBDTN->getNumChildren() == 0) {
        // TBB cannot be dominator of any blocks as tail BB is reachable via FBB
        // or the head. Everything below tail BB has at least tail BB as idom.
        DT->eraseNode(TBB);
      } else {
        DEBUG(errs() << "  BB " << TBB->getName() << " must not have any "
            << " children in DOM tree.\n");
      }
    }
  }

  // Remove FBB from domtree
  if (FBB != nullptr) {
    DomTreeNode *FBBDTN = DT->getNode(FBB);
    if ((FBBDTN != nullptr) && (FBBDTN != RTDTN) && (FBBDTN != HeadDTN)) {
      if (FBBDTN->getNumChildren() == 0) {
        // FBB cannot be dominator of any blocks as tail BB is reachable via TBB
        // or the head. Everything below tail BB has at least tail BB as idom.
        DT->eraseNode(FBB);
      } else {
        DEBUG(errs() << "  BB " << FBB->getName() << " must not have any "
            << " children in DOM tree.\n");
      }
    }
  }

  // Perform updates for merged tail BB
  if ((mergedTail == true) && (TailBB != nullptr)) {
    DomTreeNode *TailDTN = DT->getNode(TailBB);
    if ((TailDTN != nullptr) && (TailDTN != RTDTN) && (TailDTN != HeadDTN)) {
      // Tail can have successors in domtree but as we merged tail into head
      // all nodes having TailBB as idom now have HeadBB as idom
      std::vector<DomTreeNode*> TailChildren(TailDTN->begin(), TailDTN->end());
      for (DomTreeNode *DTN : TailChildren) {
        DT->changeImmediateDominator(DTN, HeadDTN);
      }
      DT->eraseNode(TailBB);
    }
  }
}

/// \brief Perform the actual if-conversion for this pattern.
bool Loopus::IfPattern::doIfConversion(DominatorTree *DT, AliasAnalysis *AA,
    LoopInfo *LI, Pass *P, ScalarEvolution *SE) {
  // Hoist instructions from branched blocks to head
  // Determine insertion point: speculated instructions will be inserted
  // immediately before the terminator of the head BB. We do *not* have to take
  // care about PHI nodes (as they always must be at the beginning of a block)
  // as the branched blocks (TBB and FBB) cannot and must not contain any phi
  // nodes as we only convert patterns where TBB and FBB each have exactly one
  // single predecessor
  if (TBB != nullptr) {
    //deki_edit
    llvm::BasicBlock::iterator it(HeadBB->getTerminator());
    HeadBB->getInstList().splice(it, TBB->getInstList(),
        TBB->begin(), std::prev(TBB->end()));
    DEBUG(errs() << "  spex: tbb=true\n");
    ++StatsNumBlocksSpeculated;
  }

  /*if (TBB != nullptr) {
    HeadBB->getInstList().splice(HeadBB->getTerminator(), TBB->getInstList(),
        TBB->begin(), std::prev(TBB->end()));
    DEBUG(errs() << "  spex: tbb=true\n");
    ++StatsNumBlocksSpeculated;
  }*/
  if (FBB != nullptr) {
    llvm::BasicBlock::iterator it(HeadBB->getTerminator());
    HeadBB->getInstList().splice(it, FBB->getInstList(),
        FBB->begin(), std::prev(FBB->end()));
    DEBUG(errs() << "  spex: fbb=true\n");
    ++StatsNumBlocksSpeculated;
  }

  bool AdditionalTailPreds = false;
  if (Loopus::getNumPredecessors(TailBB) != 2) {
    AdditionalTailPreds = true;
  }

  if (AdditionalTailPreds == true) {
    // Keep phi nodes in tail BB
    adaptPhiOps(AA);
  } else {
    // Replace phi nodes in tail BB by select instructions
    replacePhis(AA);
  }

  // Restore the links/branches
  Instruction *HeadTI = HeadBB->getTerminator();
  if (HeadTI == nullptr) { return false; }
  BranchInst *NewBI = BranchInst::Create(TailBB);
  ReplaceInstWithInst(HeadTI, NewBI);
  // Update alias information if available
  if (AA != nullptr) {
    //AA->replaceWithNewValue(HeadTI, NewBI);
  }
  // Now erase TBB and FBB
  if (TBB != nullptr) {
    TBB->eraseFromParent();
    ++StatsNumBlocksRemoved;
  }
  if (FBB != nullptr) {
    FBB->eraseFromParent();
    ++StatsNumBlocksRemoved;
  }

  // Now possibly merge HeadBB and TailBB if TailBB does not have any other
  // predecessors than HeadBB
  bool mergedTail = false;
  if ((TailBB != nullptr) && (TailBB->getSinglePredecessor() == HeadBB)) {
    if ((SE != nullptr) && (LI != nullptr)) {
      Loop *TailL = LI->getLoopFor(TailBB);
      if (TailL != nullptr) {
        if (TailL->isLoopExiting(TailBB) == true) {
          SE->forgetLoop(TailL);
        }
      }
    }
    //deki_edit
    //this is unusual, why is there P as an argument?
    //if (MergeBlockIntoPredecessor(TailBB, P) == false) {
    if (MergeBlockIntoPredecessor(TailBB) == false) {
      DEBUG(errs() << ">>Could not merge block " << TailBB->getName() << " "
          << "into predecessor although there is exactly one!\n");
    } else {
      mergedTail = true;
    }
  }

  // Update the DomTree
  updateDomTree(DT, mergedTail);

  // Update statistics
  if (isTriangle() == true) {
    ++StatsNumTrianglesConverted;
  } else if (isDiamond() == true) {
    ++StatsNumDiamondsConverted;
  }

  // If we are getting everything should have been successfull
  return true;
}

//===- Implementation of SwitchPattern class ------------------------------===//
/// \brief Creates a new empty switch pattern.
Loopus::SwitchPattern::SwitchPattern(void)
 : HeadBB(nullptr), TailBB(nullptr) {
}

/// \brief Creates a new SwitchPattern with the given head and tail.
/// \note No checks for validity are performed!
Loopus::SwitchPattern::SwitchPattern(BasicBlock *Head, BasicBlock *Tail)
 : HeadBB(Head), TailBB(Tail) {
}

BasicBlock* Loopus::SwitchPattern::getHead(void)              { return HeadBB; }
const BasicBlock* Loopus::SwitchPattern::getHead(void) const  { return HeadBB; }
BasicBlock* Loopus::SwitchPattern::getTail(void)              { return TailBB; }
const BasicBlock* Loopus::SwitchPattern::getTail(void) const  { return TailBB; }
void Loopus::SwitchPattern::setHead(BasicBlock *newHead)      { HeadBB = newHead; }
void Loopus::SwitchPattern::setTail(BasicBlock *newTail)      { TailBB = newTail; }

/// \brief Returns \c true if a dedicated default case (not the tail) exists.
bool Loopus::SwitchPattern::hasDedicatedDefault(void) const {
  const SwitchInst *SI = dyn_cast<SwitchInst>(HeadBB->getTerminator());
  assert(SI != nullptr && "Terminator of head must be a SwitchInst!");
  if (SI == nullptr) { return false; }

  const BasicBlock *DefBB = SI->getDefaultDest();
  if (DefBB == nullptr) {
    return false;
  } else {
    if (DefBB == TailBB) {
      return false;
    } else {
      return true;
    }
  }
}

bool Loopus::SwitchPattern::canIfConvert(const DataLayout *DL) const {
  // First validate the shape of the pattern
  if ((HeadBB == nullptr) || (TailBB == nullptr)) { return false; }

  // Get terminator for head basic blovk
  const Instruction* const HeadTI = HeadBB->getTerminator();
  assert(HeadTI != nullptr && "Head BB does not have a valid terminator!");
  if (HeadTI == nullptr) { return false; }

  // Terminator must be a SwitchInst
  const SwitchInst* const HeadSI = dyn_cast<SwitchInst>(HeadTI);
  if (HeadSI == nullptr) {
    DEBUG(errs() << "  Cannot convert as terminator of head BB is not a "
        << "SwitchInst!\n");
    return false;
  }

  // Check consistent number of successors of head
  const unsigned HeadNumSuccessors = HeadTI->getNumSuccessors();
  if (HeadNumSuccessors != HeadSI->getNumCases() + 1) { return false; }

  // Iterate over case blocks excluding the default case block
  for (SwitchInst::ConstCaseIt CSIT = HeadSI->case_begin(),
      CSEND = HeadSI->case_end(); CSIT != CSEND; ++CSIT) {
    //deki_edit
    //const BasicBlock* const CaseBB = CSIT.getCaseSuccessor();
    const BasicBlock* const CaseBB = CSIT->getCaseSuccessor();
    if (CaseBB == nullptr) { return false; }

    if (CaseBB != TailBB) {
      // Current case block is not the tail block
      const Instruction* const CaseTI = CaseBB->getTerminator();
      assert(CaseTI != nullptr && "Case BB does not have a valid terminator!");
      if (CaseTI == nullptr) { return false; }
      const BranchInst* const CaseBI = dyn_cast<BranchInst>(CaseTI);
      if (CaseBI == nullptr) { return false; }
      if (CaseBI->isUnconditional() == false) { return false; }

      // Make sure that there are no phi nodes
      if (isa<PHINode>(CaseBB->front()) == true) { return false; }
      // As a case BB might have HeadBB multiple times as predecessor we must
      // use getUniquePredecessor instead of getSinglePredecessor
      if (CaseBB->getUniquePredecessor() != HeadBB) { return false; }
      // Single successor must be tail
      if (Loopus::getSingleSuccessor(CaseBB) != TailBB) { return false; }
    }
  }

  // Now the same checks for the dedicated default case - if available
  const BasicBlock* const DefBB = HeadSI->getDefaultDest();
  if (DefBB == nullptr) { return false; }
  if (hasDedicatedDefault() == true) {
    const Instruction* const DefTI = DefBB->getTerminator();
    if (DefTI == nullptr) { return false; }
    const BranchInst* const DefBI = dyn_cast<BranchInst>(DefTI);
    if (DefBI == nullptr) { return false; }
    if (DefBI->isUnconditional() == false) { return false; }

    // As this is the dedicated default block there must not be any phi nodes
    if (isa<PHINode>(DefBB->front()) == true) { return false; }
    if (DefBB->getUniquePredecessor() != HeadBB) { return false; }
    if (Loopus::getSingleSuccessor(DefBB) != TailBB) { return false; }
  } else {
    // Some consistency checks...
    if (DefBB != TailBB) { return false; }
  }

  // Shape seems to be correct
  // So now test if all case blocks (and if needed the default block) can be
  // speculated
  for (SwitchInst::ConstCaseIt CSIT = HeadSI->case_begin(),
      CSEND = HeadSI->case_end(); CSIT != CSEND; ++CSIT) {
    //deki_edit    
    //const BasicBlock* const CaseBB = CSIT.getCaseSuccessor();
    const BasicBlock* const CaseBB = CSIT->getCaseSuccessor();
    if (CaseBB == nullptr) { return false; }
    if (CaseBB == TailBB) { continue; }
    if (canExecuteSpeculatively(CaseBB, DL) == false) {
      DEBUG(errs() << "  Cannot speculatively execute BB " << CaseBB->getName()
          << "!\n");
      return false;
    }
  }
  if ((DefBB != nullptr) && (hasDedicatedDefault() == true)) {
    if (canExecuteSpeculatively(DefBB, DL) == false) {
      DEBUG(errs() << "  Cannot speculatively execute BB " << DefBB->getName()
          << "!\n");
      return false;
    }
  }

  // Test that Tail BB has any PHI nodes. If it does not have any there must be
  // other side effects. If tail BB does not have PHIs and all case blocks do not
  // have side effects their code does not change the program state in any way
  // and could be omitted. So we are assuming that there are other side effects
  // if tail BB does not have any PHI nodes.
  bool hasPHIs = false;
  for (BasicBlock::const_iterator INSIT = TailBB->begin(),
      INSEND = TailBB->end(); INSIT != INSEND; ++INSIT) {

    // Break out if all phi nodes are tested
    const PHINode* const PN = dyn_cast<PHINode>(&*INSIT);
    if (PN == nullptr) { break; }

    // Now iterate over all case blocks of the switch
    for (SwitchInst::ConstCaseIt CSIT = HeadSI->case_begin(),
        CSEND = HeadSI->case_end(); CSIT != CSEND; ++CSIT) {
      //deki_edit
      //const BasicBlock* const CaseBB = CSIT.getCaseSuccessor();
      const BasicBlock* const CaseBB = CSIT->getCaseSuccessor();
      if (CaseBB == nullptr) { return false; }
      if (CaseBB == TailBB) { continue; }
      const Value* const CaseValue = PN->getIncomingValueForBlock(CaseBB);
      if (CaseValue == nullptr) { return false; }
      hasPHIs = true;

      const ConstantExpr* const CaseCE = dyn_cast<ConstantExpr>(CaseValue);
      if (CaseCE != nullptr) {
        if (canExecuteSpeculatively(CaseCE, DL) == false) {
          return false;
        }
      }
    }
    // Now same tests for the default block if it is dedicated
    if (hasDedicatedDefault() == true) {
      const BasicBlock* const DefBB = HeadSI->getDefaultDest();
      if (DefBB == nullptr) { return false; }
      const Value* const DefaultValue = PN->getIncomingValueForBlock(DefBB);
      if (DefaultValue == nullptr) { return false; }
      hasPHIs = true;

      const ConstantExpr* const DefCE = dyn_cast<ConstantExpr>(DefaultValue);
      if (DefCE != nullptr) {
        if (canExecuteSpeculatively(DefCE, DL) == false) {
          return false;
        }
      }
    }
  }
  if (hasPHIs == false) {
    // There are no phis so assume that there are other side effects what means
    // that the block cannot be speculated
    // XXX: We might want to omit this test...
    DEBUG(errs() << "  Cannot convert pattern as no PHIs in tail BB "
        << TailBB->getName() << " were found so assuming side effects!\n");
    return false;
  }

  // Once again: I don't know any other usefull tests...

  return true;
}

/// \brief Determines if the pattern should be converted.
///
/// Computes the costs of the conversion and determines if the pattern should be
/// converted or not.
bool Loopus::SwitchPattern::shouldIfConvert(Loopus::RessourceEstimatorBase &RE) const {
  const SwitchInst* const HeadSI = dyn_cast<SwitchInst>(HeadBB->getTerminator());
  if (HeadSI == nullptr) { return false; }
  // Check sizes of case blocks
  for (SwitchInst::ConstCaseIt CSIT = HeadSI->case_begin(),
      CSEND = HeadSI->case_end(); CSIT != CSEND; ++CSIT) {
    //deki_edit
    //const BasicBlock* const CaseBB = CSIT.getCaseSuccessor();
    const BasicBlock* const CaseBB = CSIT->getCaseSuccessor();
    if (CaseBB == nullptr) { return false; }
    if (CaseBB == TailBB) { continue; }
    const unsigned CBBSize = RE.getRessourceUsage(CaseBB);
    if (CBBSize > MaxSpecBlockSize) {
      DEBUG(errs() << "  Will not speculate BB " << CaseBB->getName() << " as "
          << "it is too large...\n");
      return false;
    }
  }
  // Check size of dedicated default block if available
  if (hasDedicatedDefault() == true) {
    const BasicBlock* const DefBB = HeadSI->getDefaultDest();
    if (DefBB == nullptr) { return false; }
    const unsigned DBBSize = RE.getRessourceUsage(DefBB);
    if (DBBSize > MaxSpecBlockSize) {
      DEBUG(errs() << "  Will not speculate BB " << DefBB->getName() << " as "
          << "it is too large...\n");
      return false;
    }
  }

  return true;
}

/// \brief Injects the needed compare instructions to transform the pattern.
/// \param ValueMap The map that associates the cases with their compares.
/// \param insertBefore The instruction before which the compares should be inserted.
///
/// Creates a compare instruction for each case of the switch. They will be
/// inserted before the instruction pointed to by \c insertBefore or before the
/// terminator of the head. The provided map will contain a mapping between
/// the cases (meaning their ConstantInt value) and their corresponding compare
/// instruction.
bool Loopus::SwitchPattern::injectCompares(ValueToValueMapTy &ValueMap,
    Instruction* insertBefore) {
  if (HeadBB == nullptr) { return false; }

  // Get the switch statement
  SwitchInst* const HeadSI = dyn_cast<SwitchInst>(HeadBB->getTerminator());
  if (HeadSI == nullptr) { return false; }

  // Validate insertBefore instruction
  if (insertBefore == nullptr) {
    insertBefore = HeadSI;
  }
  if (insertBefore->getParent() != HeadBB) { return false; }

  // Get the condition variable
  Value* const SICondVar = HeadSI->getCondition();
  if (SICondVar == nullptr) { return false; }

  // Now check all cases...
  for (SwitchInst::CaseIt CSIT = HeadSI->case_begin(),
      CSEND = HeadSI->case_end(); CSIT != CSEND; ++CSIT) {
    //deki_edit
    ConstantInt* const CaseVal = CSIT->getCaseValue();
    if (CaseVal == nullptr) { return false; }

    // Create the compare instruction
    ICmpInst *CaseCMP = new ICmpInst(insertBefore, CmpInst::Predicate::ICMP_EQ,
        SICondVar, CaseVal);
    if (CaseCMP == nullptr) { return false; }
    ValueMap[CaseVal] = CaseCMP;
  }

  // Note: we do not need a compare instruction for the default case as it will
  // be executed if all other statements are evaluated to false.

  return true;
}

bool Loopus::SwitchPattern::adaptPhiOps(ValueToValueMapTy &CaseCmpMap, AliasAnalysis *AA) {
  // We need to do:
  // 1. Add select instructions in head BB to switch between phi operands for
  //    the different cases and default. There will be a casecade starting with
  //    the condition for the first block and the default case value and that
  //    for the first block.
  // 2. Remove phi operands for all cases and default
  // 3. Add new phi operand for head BB with the value of the *LAST* select
  //    instruction
  SwitchInst *HeadSI = dyn_cast<SwitchInst>(HeadBB->getTerminator());
  if (HeadSI == nullptr) { return false; }

  bool changed = false;
  DEBUG(errs() << "   phi: method=adapt\n"
               << "        bb=" << TailBB->getName() << "\n");
  for (BasicBlock::iterator INSIT = TailBB->begin(), INSEND = TailBB->end();
      INSIT != INSEND; ++INSIT) {
    PHINode *PN = dyn_cast<PHINode>(&*INSIT);
    if (PN == nullptr) {
      // We processed all PHI Nodes
      break;
    }

    // Determine the value for the default case
    Value *DefValue = nullptr;
    if (hasDedicatedDefault() == true) {
      const BasicBlock* DefBB = HeadSI->getDefaultDest();
      DefValue = PN->getIncomingValueForBlock(DefBB);
    } else {
      DefValue = PN->getIncomingValueForBlock(HeadBB);
    }

    // Some data that we will use for each select instruction
    AAMDNodes TBAAMDNode;
    PN->getAAMetadata(TBAAMDNode, false);

    Value* prevVal = DefValue;
    Value *curVal = nullptr;
    for (SwitchInst::CaseIt CSIT = HeadSI->case_begin(),
        CSEND = HeadSI->case_end(); CSIT != CSEND; ++CSIT) {
      // Determine the value for current case
      //deki_edit
      const BasicBlock *CaseBB = CSIT->getCaseSuccessor();
      if (CaseBB != TailBB) {
        curVal = PN->getIncomingValueForBlock(CaseBB);
      } else {
        curVal = PN->getIncomingValueForBlock(HeadBB);
      }
      if (curVal == nullptr) { continue; }

      // Now create the select instruction using the following operands
      // - The CONDITION for the current case fetching it from the provided map
      // - In the TRUE case use the value that the phi node uses for the current
      //   case block
      // - In the FALSE case use the previously created select instruction that
      //   is stored in the prevVal
      // This is enough as by the LLVM IR language ref each value may at most
      // appear once in the switch table so there is at most one block for an
      // specific ConstantInt. So there is at most one icmp instruction that
      // evaluates to true and the value from that block will be selected by
      // casecade of select instructions. If none is true the default case
      // value will ripple through all the selects
      //deki_edit
      Value* CmpCondition = CaseCmpMap[CSIT->getCaseValue()];
      SelectInst *SI = SelectInst::Create(CmpCondition, curVal, prevVal, "", HeadSI);
      if (PN->hasName() == true) {
        SI->setName(PN->getName() + ".preselect");
        if (CaseBB->hasName() == true) {
          SI->setName(SI->getName() + "." + CaseBB->getName());
        }
      }
      SI->setAAMetadata(TBAAMDNode);
      SI->setDebugLoc(PN->getDebugLoc());
      if (AA != nullptr) {
        //AA->copyValue(PN, SI);
      }

      // Now set prevVal to the created select instruction to be ready for
      // processing next case
      curVal = SI;
      prevVal = curVal;
      curVal = nullptr;
    }

    // Now first remove the operands for all the cases and the default case
    for (SwitchInst::CaseIt CSIT = HeadSI->case_begin(),
        CSEND = HeadSI->case_end(); CSIT != CSEND; ++CSIT) {
      //deki_edit
      const BasicBlock *CaseBB = CSIT->getCaseSuccessor();
      if (CaseBB != TailBB) {
        PN->removeIncomingValue(CaseBB, false);
      } else {
        PN->removeIncomingValue(HeadBB, false);
      }
    }
    if (hasDedicatedDefault() == true) {
      const BasicBlock *DefBB = HeadSI->getDefaultDest();
      PN->removeIncomingValue(DefBB, false);
    } else {
      PN->removeIncomingValue(HeadBB, false);
    }

    // As prevVal is pointing to the last created select instruction it will be
    // used as operand for the phi node
    PN->addIncoming(prevVal, HeadBB);
    changed = true;

    // Update statistics
    ++StatsNumPhisAdapted;
  }

  return changed;
}

bool Loopus::SwitchPattern::replacePhis(ValueToValueMapTy &CaseCmpMap, AliasAnalysis *AA) {
  // We need to do:
  // 1. Add select instructions in head BB to switch between phi operands for
  //    the different cases and default. There will be a casecade starting with
  //    the condition for the first block and the default case value and that
  //    for the first block.
  // 2. Replace all uses of the phi node by the last created select instruction
  SwitchInst *HeadSI = dyn_cast<SwitchInst>(HeadBB->getTerminator());
  if (HeadSI == nullptr) { return false; }

  // At this point we are assuming that all operands of the phi node are coming
  // from the switch instruction as testing this is a bit complicated and
  // normally should be unnecessary at this point as those tests should already
  // be performed
  DEBUG(errs() << "   phi: method=replace\n"
               << "        bb=" << TailBB->getName() << "\n");
  std::vector<Instruction*> PHIsToRemove;
  bool changed = false;

  for (BasicBlock::iterator INSIT = TailBB->begin(), INSEND = TailBB->end();
      INSIT != INSEND; ++INSIT) {
    PHINode *PN = dyn_cast<PHINode>(&*INSIT);
    if (PN == nullptr) {
      // We processed all PHI Nodes
      break;
    }

    // Determine the value for the default case
    Value *DefValue = nullptr;
    if (hasDedicatedDefault() == true) {
      const BasicBlock* DefBB = HeadSI->getDefaultDest();
      DefValue = PN->getIncomingValueForBlock(DefBB);
    } else {
      DefValue = PN->getIncomingValueForBlock(HeadBB);
    }

    // Some data that we need for each created select instruction
    AAMDNodes TBAAMDNode;
    PN->getAAMetadata(TBAAMDNode, false);

    Value* prevVal = DefValue;
    Value *curVal = nullptr;
    for (SwitchInst::CaseIt CSIT = HeadSI->case_begin(),
        CSEND = HeadSI->case_end(); CSIT != CSEND; ++CSIT) {
      // Determine the value for current case
      //deki_edit
      const BasicBlock *CaseBB = CSIT->getCaseSuccessor();
      if (CaseBB != TailBB) {
        curVal = PN->getIncomingValueForBlock(CaseBB);
      } else {
        // Might be the case if case simply consists of a break; instruction
        curVal = PN->getIncomingValueForBlock(HeadBB);
      }
      if (curVal == nullptr) { continue; }
      // At this point the creation of the select instructions is performed in
      // the same way as in adaptPhiOps...
      //deki_edit
      Value* CmpCondition = CaseCmpMap[CSIT->getCaseValue()];
      SelectInst *SI = SelectInst::Create(CmpCondition, curVal, prevVal, "", HeadSI);
      if (PN->hasName() == true) {
        SI->setName(PN->getName() + ".preselect");
        if (CaseBB->hasName() == true) {
          SI->setName(SI->getName() + "." + CaseBB->getName());
        }
      }
      SI->setAAMetadata(TBAAMDNode);
      SI->setDebugLoc(PN->getDebugLoc());
      if (AA != nullptr) {
        //AA->copyValue(PN, SI);
      }

      // Now set prevVal to the created select instruction to be ready for
      // processing next case
      curVal = SI;
      prevVal = curVal;
      curVal = nullptr;
    }

    // As prevVal is pointing to the last created select instruction it will be
    // used to replace all occurences/uses of the processed phi node
    PN->replaceAllUsesWith(prevVal);
    PHIsToRemove.push_back(PN);
    changed = true;
  }

  // Now remove all phi nodes not needed any more
  unsigned CountPHIsRemoved = 0;
  for (Instruction *INS : PHIsToRemove) {
    // We do not have to check if the operands could be erased too as the
    // operands are used by the select instruction
    INS->eraseFromParent();
    if (AA != nullptr) {
      //AA->deleteValue(INS);
    }
    // Update statistics
    ++CountPHIsRemoved;
    ++StatsNumPhisReplaced;
  }
  DEBUG(errs() << "   phi: removed=" << CountPHIsRemoved << "\n");

  return changed;
}

/// \brief Updates the dominator tree in \c DT during conversion.
/// \param DT A pointer to the dominator tree.
/// \param removedBlocks The removed case and default blocks
/// \param mergedTail Indicates if the tail was merged into the header.
///
/// Updates the dominator tree after the case and default blocks were removed
/// from the CFG. The function then removes them from the dominator tree. If
/// \c mergedTail is \c true then the function assumes that the tail BB was
/// merged into the head BB and updates the dominator accordingly.
void Loopus::SwitchPattern::updateDomTree(DominatorTree *DT,
    const std::set<BasicBlock*> &removedBlocks, bool mergedTail) {
  if (DT == nullptr) { return; }
  // We try to update the DomTree by removing the case and default blocks
  // from the DomTree and updating their children (in the DomTree).

  DomTreeNode *RTDTN = DT->getRootNode();
  DomTreeNode *HeadDTN = DT->getNode(HeadBB);
  if (HeadDTN == nullptr) { return; }

  // Remove the nodes from domtree for erase blocks
  for (BasicBlock *BB : removedBlocks) {
    if (BB == nullptr) { continue; }

    DomTreeNode *BBDTN = DT->getNode(BB);
    if ((BBDTN == nullptr) || (BBDTN == RTDTN) || (BBDTN == HeadDTN)) { continue; }

    if (BBDTN->getNumChildren() == 0) {
      DT->eraseNode(BB);
    } else {
      DEBUG(errs() << "  BB " << BB->getName() << " must not have any "
          << " children in DOM tree.\n");
    }
  }

  // Now update the tail children if there are any and tail was merged into head
  if ((mergedTail == true) && (TailBB != nullptr)) {
    DomTreeNode *TailDTN = DT->getNode(TailBB);
    if ((TailDTN != nullptr) && (TailDTN != RTDTN) && (TailDTN != HeadDTN)) {
      // Tail can have successors in domtree but as we merged tail into head
      // all nodes having TailBB as idom now have HeadBB as idom
      std::vector<DomTreeNode*> TailChildren(TailDTN->begin(), TailDTN->end());
      for (DomTreeNode *DTN : TailChildren) {
        DT->changeImmediateDominator(DTN, HeadDTN);
      }
      DT->eraseNode(TailBB);
    }
  }
}

bool Loopus::SwitchPattern::doIfConversion(DominatorTree *DT, AliasAnalysis *AA,
    LoopInfo *LI, Pass *P, ScalarEvolution *SE) {
  // Some variables we will need...
  if ((HeadBB == nullptr) || (TailBB == nullptr)) { return false; }
  SwitchInst *HeadSI = dyn_cast<SwitchInst>(HeadBB->getTerminator());
  if (HeadSI == nullptr) { return false; }

  // First we need to create compare instructions do be able to determine which
  // case of the switch will create the results for possibly existing phi nodes.
  // In contrast to normal if-then-else patterns those compare results are not
  // yet available: the switch instruction only gets an integer type value and
  // does the comparison "internally".
  // For each case a compare will be created and results are stored in the
  // provided map.
  ValueToValueMapTy CaseCompareMap;
  DEBUG(errs() << "  stat: injecting compares...\n");
  if (injectCompares(CaseCompareMap) == false) {
    DEBUG(errs() << ">>Could not inject compare instructions into BB "
                 << HeadBB->getName() << "! Aborting...\n");
    return false;
  }

  // Now merge the blocks into the head block
  unsigned CountBlocksSpeculated = 0;
  for (SwitchInst::CaseIt CSIT = HeadSI->case_begin(),
      CSEND = HeadSI->case_end(); CSIT != CSEND; ++CSIT) {
    //deki_edit
    BasicBlock *CaseBB = CSIT->getCaseSuccessor();
    if (CaseBB == nullptr) { continue; }
    if (CaseBB == TailBB) { continue; }
    // Move instructions of current case block into header
    llvm::BasicBlock::iterator it(HeadSI);
    HeadBB->getInstList().splice(it, CaseBB->getInstList(), CaseBB->begin(),
        std::prev(CaseBB->end()));
    ++CountBlocksSpeculated;
    ++StatsNumBlocksSpeculated;
  }
  // Merge default block if it is dedicated
  if (hasDedicatedDefault() == true) {
    BasicBlock *DefBB = HeadSI->getDefaultDest();
    llvm::BasicBlock::iterator it(HeadSI);
    HeadBB->getInstList().splice(it, DefBB->getInstList(), DefBB->begin(),
        std::prev(DefBB->end()));
    ++CountBlocksSpeculated;
    ++StatsNumBlocksSpeculated;
  }
  DEBUG(errs() << "  spex: count=" << CountBlocksSpeculated << "\n");

  bool AdditionalTailPreds = false;
  // A bit tricky as several cases might share the same basic block and so we
  // have several successor but just one block that is predecessor of the tail
  for (pred_iterator BBPREDIT = pred_begin(TailBB),
      BBPREDEND = pred_end(TailBB); BBPREDIT != BBPREDEND; ++BBPREDIT) {
    if (TailBB == *BBPREDIT) {
      AdditionalTailPreds = true;
      break;
    }

    bool isSISucc = false;
    for (SwitchInst::CaseIt CSIT = HeadSI->case_begin(),
        CSEND = HeadSI->case_end(); CSIT != CSEND; ++CSIT) {
      //deki_edit
      if (CSIT->getCaseSuccessor() == *BBPREDIT) {
        // BBPREDIT points to block that is successor of head
        isSISucc = true;
        break;
      }
    }
    if (hasDedicatedDefault() == true) {
      if (HeadSI->getDefaultDest() == *BBPREDIT) {
        isSISucc = true;
      }
    } else {
      if (HeadBB == *BBPREDIT) {
        isSISucc = true;
      }
    }
    if (isSISucc == false) {
      AdditionalTailPreds = true;
      break;
    }
  }
  // Now we know if TailBB has other predecessors than those coming from the
  // switch instruction in HeadBB

  if (AdditionalTailPreds == true) {
    // Keep phi nodes in tail BB but adapt their operands
    adaptPhiOps(CaseCompareMap, AA);
  } else {
    // The phi nodes in tail BB can be replaced
    replacePhis(CaseCompareMap, AA);
  }

  // Collect blocks that should be erased and erase them
  std::set<BasicBlock*> BlocksToErase;
  for (SwitchInst::CaseIt CSIT = HeadSI->case_begin(),
      CSEND = HeadSI->case_end(); CSIT != CSEND; ++CSIT) {
    //deki_edit
    BasicBlock *CaseBB = CSIT->getCaseSuccessor();
    if (CaseBB == nullptr) { continue; }
    if (CaseBB == TailBB) { continue; }
    BlocksToErase.insert(CaseBB);
  }
  if (hasDedicatedDefault() == true) {
    BlocksToErase.insert(HeadSI->getDefaultDest());
  }
  // Restore the links/branches
  BranchInst *NewBI = BranchInst::Create(TailBB);
  ReplaceInstWithInst(HeadSI, NewBI);
  if (AA != nullptr) {
    //AA->replaceWithNewValue(HeadSI, NewBI);
  }
  // Erase collected blocks
  for (BasicBlock* EBB : BlocksToErase) {
    EBB->eraseFromParent();
    ++StatsNumBlocksRemoved;
  }

  // Now possibly merge HeadBB and TailBB if TailBB does not have any other
  // predecessors than HeadBB
  bool mergedTail = false;
  if ((TailBB != nullptr) && (TailBB->getSinglePredecessor() == HeadBB)) {
    if ((SE != nullptr) && (LI != nullptr)) {
      Loop *TailL = LI->getLoopFor(TailBB);
      if (TailL != nullptr) {
        if (TailL->isLoopExiting(TailBB) == true) {
          SE->forgetLoop(TailL);
        }
      }
    }
    if (MergeBlockIntoPredecessor(TailBB) == false) {
      DEBUG(errs() << ">>Could not merge block " << TailBB->getName() << " "
          << "into predecessor although there is exactly one!\n");
    } else {
      mergedTail = true;
    }
  }

  // Update dom tree
  updateDomTree(DT, BlocksToErase, mergedTail);

  // Update statistics
  ++StatsNumFansConverted;

  // If we are getting everything should have been successfull
  return true;
}

//===- Implementation of LLVM pass ----------------------------------------===//
//INITIALIZE_PASS_BEGIN(HDLFlattenCFG, "loopus-flatcfg", "If-Conversion",  true, false)
//INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass);
//deki_edit
//INITIALIZE_PASS_DEPENDENCY(DataLayoutPass);
//INITIALIZE_PASS_END(HDLFlattenCFG, "loopus-flatcfg", "If-Conversion",  true, false)

char HDLFlattenCFG::ID = 0;

//namespace llvm {
//  Pass* createHDLFlattenCFGPass() {
//    return new HDLFlattenCFG();
//  }
//}

HDLFlattenCFG::HDLFlattenCFG(void)
 : FunctionPass(ID) {
  //initializeHDLFlattenCFGPass(*PassRegistry::getPassRegistry());
}

void HDLFlattenCFG::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<DominatorTreeWrapperPass>();
  //deki_edit  
  //AU.addRequired<DataLayoutPass>();
  //AU.addPreserved<DataLayoutPass>();
}

bool HDLFlattenCFG::runOnFunction(Function &F) {
  //AliasAnalysis *AA = getAnalysisIfAvailable<AliasAnalysis>();
  //deki_edit  
  //DataLayoutPass *DLP = &getAnalysis<DataLayoutPass>();
  //const DataLayout *DL = (DLP != nullptr) ? &(DLP->getDataLayout()) : nullptr;
  DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  //LoopInfo *LI = getAnalysisIfAvailable<LoopInfo>();
  //ScalarEvolution *SE = getAnalysisIfAvailable<ScalarEvolution>();
  Loopus::SimpleRessourceEstimator SRE;

  //deki_edit
  //if ((DL == nullptr) || (DT == nullptr)) {
  if (DT == nullptr) {
    return false;
  }

  //Loopus::HDLFlattenCFGWorker HFCW(AA, DL, DT, LI, this, SE, &SRE);
  Loopus::HDLFlattenCFGWorker HFCW(nullptr, nullptr, DT, nullptr, this, nullptr, &SRE);
  return HFCW.runOnFunction(&F);
}

//char Hello3::ID = 0;
static RegisterPass<HDLFlattenCFG> X("loopus-flatcfg", "If-Conversion - adapted");
