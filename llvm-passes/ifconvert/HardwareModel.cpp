#include "HardwareModel.h"

#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Type.h"

Loopus::HardwareModel Loopus::HardwareModel::HWModel;

//===- RessourceEstimators ------------------------------------------------===//
Loopus::RessourceEstimatorBase::RessourceEstimatorBase(void) {
}

Loopus::RessourceEstimatorBase::~RessourceEstimatorBase(void) {
}

/// \brief Computes the estimated ressource usage of the given block.
/// \param BB The block for which the ressource usage should be estimated.
///
/// Computes the ressource usage for the given block by summing up the estimated
/// ressource usage for each instruction contained in the block.
unsigned Loopus::RessourceEstimatorBase::getRessourceUsage(const llvm::BasicBlock *BB) {
  if (BB == nullptr) { return 0; }

  int RUSum = 0;
  for (llvm::BasicBlock::const_iterator CBBIT = BB->begin(), CBBEND = BB->end();
      CBBIT != CBBEND; ++CBBIT) {
    RUSum += getRessourceUsage(&*CBBIT);
  }
  return RUSum;
}

/// \brief Computes the estimated ressource usage of the given function.
/// \param F The function for which the ressource usage should be estimated.
///
/// Computes the ressource usage for the given function by summing up the
/// estimated ressource usage for each block contained in the function.
unsigned Loopus::RessourceEstimatorBase::getRessourceUsage(const llvm::Function *F) {
  if (F == nullptr) { return 0; }

  int RUSum = 0;
  for (llvm::Function::const_iterator CFIT = F->begin(), CFEND = F->end();
      CFIT != CFEND; ++CFIT) {
    RUSum += getRessourceUsage(&*CFIT);
  }
  return RUSum;
}

/// \brief Computes the estimated ressource usage of the given loop.
/// \param L The loop for which the ressource usage should be estimated.
///
/// Computes the ressource usage for the given loop by summing up the
/// estimated ressource usage for each block contained in the loop.
unsigned Loopus::RessourceEstimatorBase::getRessourceUsage(const llvm::Loop *L) {
  if (L == nullptr) { return 0; }

  int RUSum = 0;
  for (llvm::Loop::block_iterator LIT = L->block_begin(), LEND = L->block_end();
      LIT != LEND; ++LIT) {
    RUSum += getRessourceUsage(*LIT);
  }
  return RUSum;
}


Loopus::BasicRessourceEstimator::BasicRessourceEstimator(void)
 : Loopus::RessourceEstimatorBase() {
}

Loopus::BasicRessourceEstimator::~BasicRessourceEstimator(void) {
}

/// \brief Computes the estimated ressource usage of the given value.
/// \param V The value for which the ressource usage should be estimated.
///
/// Computes the ressource usage for the given value. This basic estimator
/// assums costs of 1 for each value. Like this the other functions are simply
/// counting the instructions contained in the other structures (blocks,
/// functions and loops).
unsigned Loopus::BasicRessourceEstimator::getRessourceUsage(const llvm::Value *V) {
  if (V == nullptr) { return 0; }
  if (llvm::isa<llvm::BasicBlock>(V) == true) {
    return getRessourceUsage(llvm::dyn_cast<llvm::BasicBlock>(V));
  }
  return 1;
}


Loopus::SimpleRessourceEstimator::SimpleRessourceEstimator(void)
 : Loopus::RessourceEstimatorBase(), DEFAULT_TYPE_SIZE(32) {
}

Loopus::SimpleRessourceEstimator::~SimpleRessourceEstimator(void) {
}

unsigned Loopus::SimpleRessourceEstimator::getRessourceUsage(const llvm::Value *V) {
  if (V == nullptr) { return 0; }
  if (llvm::isa<llvm::BasicBlock>(V) == true) {
    // We are inspecting a basic block
    return getRessourceUsage(llvm::dyn_cast<llvm::BasicBlock>(V));
  } else if ((llvm::isa<llvm::Argument>(V) == true)
   || (llvm::isa<llvm::Constant>(V) == true)) {
    // We are checking an argument or a constant
    const llvm::Type *ValTy = V->getType();
    if (ValTy->isSized() == true) {
      return ValTy->getScalarSizeInBits();
    } else {
      return DEFAULT_TYPE_SIZE;
    }
  } else if (llvm::isa<llvm::Instruction>(V) == true) {
    const llvm::Instruction *CurI = llvm::dyn_cast<llvm::Instruction>(V);
    const unsigned IOpCode = CurI->getOpcode();
    int BaseCosts = 1;
    switch(IOpCode) {
      case llvm::Instruction::BinaryOps::Shl:
      case llvm::Instruction::BinaryOps::LShr:
      case llvm::Instruction::BinaryOps::AShr:
        BaseCosts = 3;
        break;
      case llvm::Instruction::BinaryOps::And:
      case llvm::Instruction::BinaryOps::Or:
      case llvm::Instruction::BinaryOps::Xor:
        BaseCosts = 5;
        break;

      case llvm::Instruction::BinaryOps::Add:
      case llvm::Instruction::BinaryOps::Sub:
        BaseCosts = 10;
        break;
      case llvm::Instruction::BinaryOps::Mul:
        BaseCosts = 15;
        break;
      case llvm::Instruction::BinaryOps::UDiv:
      case llvm::Instruction::BinaryOps::SDiv:
      case llvm::Instruction::BinaryOps::URem:
      case llvm::Instruction::BinaryOps::SRem:
        BaseCosts = 25;
        break;
      case llvm::Instruction::BinaryOps::FAdd:
      case llvm::Instruction::BinaryOps::FSub:
        BaseCosts = 30;
        break;
      case llvm::Instruction::BinaryOps::FMul:
      case llvm::Instruction::BinaryOps::FDiv:
      case llvm::Instruction::BinaryOps::FRem:
        BaseCosts = 45;
        break;

      case llvm::Instruction::TermOps::Br:
      case llvm::Instruction::TermOps::Switch:
        BaseCosts = 10; // Creates a split in the CFG
        break;

      case llvm::Instruction::MemoryOps::Alloca:
      case llvm::Instruction::MemoryOps::Load:
      case llvm::Instruction::MemoryOps::Store:
        BaseCosts = 15;
        break;
      case llvm::Instruction::MemoryOps::GetElementPtr:
        BaseCosts = 11;
        break;

      case llvm::Instruction::CastOps::Trunc:
      case llvm::Instruction::CastOps::ZExt:
      case llvm::Instruction::CastOps::SExt:
        BaseCosts = 5;
        break;
      case llvm::Instruction::CastOps::FPToUI:
      case llvm::Instruction::CastOps::FPToSI:
      case llvm::Instruction::CastOps::UIToFP:
      case llvm::Instruction::CastOps::SIToFP:
        BaseCosts = 18;
        break;
      case llvm::Instruction::CastOps::FPTrunc:
      case llvm::Instruction::CastOps::FPExt:
        BaseCosts = 20;
        break;
      case llvm::Instruction::CastOps::PtrToInt:
      case llvm::Instruction::CastOps::IntToPtr:
      case llvm::Instruction::CastOps::BitCast:
      case llvm::Instruction::CastOps::AddrSpaceCast:
        BaseCosts = 5;
        break;

      case llvm::Instruction::OtherOps::ICmp:
        BaseCosts = 12; // A sub instruction with compare
        break;
      case llvm::Instruction::OtherOps::FCmp:
        BaseCosts = 33; // A sub instruction with compare
        break;
      case llvm::Instruction::OtherOps::PHI:
        BaseCosts = 30; // A n:1 multiplexer
        break;
      case llvm::Instruction::OtherOps::Select:
        BaseCosts = 20; // A 2:1 multiplexer
        break;

      default:
        BaseCosts = 15;
        break;
    }
    // Now we add the bitwidth of the operands and of the result
    const llvm::Type *ResultTy = CurI->getType();
    if (ResultTy->isSized() == true) {
      BaseCosts += ResultTy->getScalarSizeInBits();
    } else {
      BaseCosts += DEFAULT_TYPE_SIZE;
    }
    for (llvm::User::const_op_iterator COPIT = CurI->op_begin(), COPEND = CurI->op_end();
        COPIT != COPEND; ++COPIT) {
      const llvm::Value *Op = COPIT->get();
      const llvm::Type *OpTy = Op->getType();
      if (OpTy->isSized() == true) {
        BaseCosts += OpTy->getScalarSizeInBits();
      } else {
        BaseCosts += DEFAULT_TYPE_SIZE;
      }
    }
    return BaseCosts;
  }

  // Hmm...
  const llvm::Type *VTy = V->getType();
  if (VTy->isSized() == true) {
    return VTy->getScalarSizeInBits();
  } else {
    return DEFAULT_TYPE_SIZE;
  }
}

unsigned Loopus::SimpleRessourceEstimator::getRessourceUsage(const llvm::BasicBlock *BB) {
  if (BB == nullptr) { return 0; }

  llvm::SmallPtrSet<const llvm::Value*, 16> ForeignInOps, ForeignOutOps;

  int RUSum = 0;
  for (llvm::BasicBlock::const_iterator CBBIT = BB->begin(), CBBEND = BB->end();
      CBBIT != CBBEND; ++CBBIT) {
    const llvm::Instruction *CurV = &*CBBIT;
    RUSum += getRessourceUsage(CurV);

    // Check the operands if any of them are coming from foreign blocks
    for (llvm::User::const_op_iterator COPIT = CurV->op_begin(), COPEND = CurV->op_end();
        COPIT != COPEND; ++COPIT) {
      const llvm::Value *OpV = COPIT->get();
      // If an operand is not computed inside the block where it is used we need
      // to provide additional input registers. To reflect this, instructions with
      // such operands are more expensive.
      if (llvm::isa<llvm::Argument>(OpV) == true) {
        ForeignInOps.insert(OpV);
      } else if (llvm::isa<llvm::Instruction>(OpV) == true) {
        if (llvm::dyn_cast<llvm::Instruction>(OpV)->getParent() != BB) {
          ForeignInOps.insert(OpV);
        }
      }
    } // End of operand-iterator

    // If the result of the instruction is used outside of the block we must
    // provide an output register for it.
    for (llvm::Value::const_user_iterator CURIT = CurV->user_begin(), CUREND = CurV->user_end();
        CURIT != CUREND; ++CURIT) {
      if (llvm::isa<llvm::Instruction>(*CURIT) == true) {
        const llvm::Instruction *UserI = llvm::dyn_cast<llvm::Instruction>(*CURIT);
        if (UserI->getParent() != BB) {
          ForeignOutOps.insert(CurV);
        }
      }
    } // End of user-iterator
  }

  // Now we add the additional costs for all instructions/operands that would
  // produce additional input/output registers. There we use the size that is
  // provided by the llvm::Type class as we do not need the exact size.
  for (const llvm::Value *InOp : ForeignInOps) {
    const llvm::Type *InOpTy = InOp->getType();
    if (InOpTy->isSized() == true) {
      RUSum += InOpTy->getScalarSizeInBits();
    } else {
      RUSum += DEFAULT_TYPE_SIZE;
    }
  }
  for (const llvm::Value *OutOp : ForeignOutOps) {
    const llvm::Type *OutOpTy = OutOp->getType();
    if (OutOpTy->isSized() == true) {
      RUSum += OutOpTy->getScalarSizeInBits();
    } else {
      RUSum += DEFAULT_TYPE_SIZE;
    }
  }
  return RUSum;
}
