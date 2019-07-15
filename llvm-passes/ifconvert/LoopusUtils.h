//===- LoopusUtils.h - Some utitlity functions ----------------------------===//
//===----------------------------------------------------------------------===//

#ifndef _LOOPUS_LOOPSUUTILS_H_INCLUDE_
#define _LOOPUS_LOOPSUUTILS_H_INCLUDE_

#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Type.h"

#include <string>

/// \brief This class represents a call to a builtin function. It stores the
/// \brief called function and one possibly existing argument.
struct BuiltInFunctionCall {
  enum BuiltInFunction {
    BIF_Undefined       = 0      ,
    BIF_GetWorkDim      = 1 <<  0,
    BIF_GetGlobalSize   = 1 <<  1,
    BIF_GetGlobalID     = 1 <<  2,
    BIF_GetLocalSize    = 1 <<  3,
    BIF_GetEnqLocalSize = 1 <<  4,
    BIF_GetLocalID      = 1 <<  5,
    BIF_GetNumGroups    = 1 <<  6,
    BIF_GetGroupID      = 1 <<  7,
    BIF_GetGlobalOffset = 1 <<  8,
    BIF_GetGlobLinearID = 1 <<  9,
    BIF_GetLocLinearID  = 1 << 10
  };

  BuiltInFunctionCall()
    : CallTarget(BuiltInFunction::BIF_Undefined), hasArg(false), ArgValue(0),
    PromotedArg(0), ArgType(0) {
  }

  /// \brief Creates a new representation for a call to a builtin.
  /// \param BIF The called of the built in function.
  BuiltInFunctionCall(BuiltInFunction BIF)
    : CallTarget(BIF), hasArg(false), ArgValue(0), PromotedArg(0), ArgType(0) {
  }

  /// \brief Creates a new representation for a call to a builtin.
  /// \param BIF The called of the built in function.
  /// \param The value of the provided argument.
  BuiltInFunctionCall(BuiltInFunction BIF, long AV)
    : CallTarget(BIF), hasArg(true), ArgValue(AV), PromotedArg(0), ArgType(0) {
  }

  /// \brief Compares two calls to builtin functions. They must match in the
  /// \brief called function. If the function expects an argument the argument
  /// \brief value must be equal too. If the function does not take an argument
  /// \brief its value is ignored and does not matter.
  bool equals(const BuiltInFunctionCall &RHS) const {
    if (this->CallTarget != RHS.CallTarget) {
      return false;
    }
    assert((this->hasArg == RHS.hasArg)
        && "Calls to same builtin with different arg specifications!");
    if (this->hasArg != RHS.hasArg) {
      return false;
    }
    if (this->hasArg == true) {
      // Evaluate arg value
      if (this->ArgValue != RHS.ArgValue) {
        return false;
      }
    }
    return true;
  }

  bool operator==(const BuiltInFunctionCall &RHS) const {
    return this->equals(RHS);
  }

  BuiltInFunction CallTarget;
  bool hasArg;
  long ArgValue;

  /// \brief Points to the argument that was created for this specific call.
  llvm::Argument *PromotedArg;
  /// \brief The type of future argument for the new function.
  llvm::Type *ArgType;
};

namespace Loopus {

  namespace OpenCLAddressSpaces {
    const unsigned ADDRSPACE_PRIVATE  = 0;
    const unsigned ADDRSPACE_GLOBAL   = 1;
    const unsigned ADDRSPACE_CONSTANT = 2;
    const unsigned ADDRSPACE_LOCAL    = 3;
    const unsigned ADDRSPACE_GENERIC  = 4;
  }

  namespace LoopMDStrings {
    const std::string LLVM_UNROLL_DISABLE("llvm.loop.unroll.disable");
    const std::string LLVM_UNROLL_ENABLE("llvm.loop.unroll.enable");
    const std::string LLVM_UNROLL_FULL("llvm.loop.unroll.full");
    const std::string LLVM_UNROLL_COUNT("llvm.loop.unroll.count");
    const std::string LLVM_UNROLL_RUNTIMEDISABLE("llvm.loop.unroll.runtime.disable");

    const std::string LOOPUS_SHIFTREG_NONE("oclacc.loopus.loop.shiftreg.none");
    const std::string LOOPUS_SHIFTREG_PARTIAL("oclacc.loopus.loop.shiftreg.partial");
    const std::string LOOPUS_SHIFTREG_FULL("oclacc.loopus.loop.shiftreg.full");

    const std::string OPENCL_UNROLL_COUNT("opencl.loop.unroll_hint");
  }

  namespace KernelMDStrings {
    const std::string OPENCL_KERNELS_ROOT("opencl.kernels");
    const std::string OCLACC_ISWORKITEM("oclacc.workitem");
    const std::string OCLACC_ISSINGLE("oclacc.single");
    const std::string OCLACC_PROMARGLIST("oclacc.promargs");

    const std::string SPIR_ARG_ADDR_SPACES("kernel_arg_addr_space");
    const std::string SPIR_ARG_ACCESS_QUAL("kernel_arg_access_qual");
    const std::string SPIR_ARG_OPTIONAL("kernel_arg_optional_qual");
    const std::string SPIR_ARG_TYPE("kernel_arg_type");
    const std::string SPIR_ARG_BASETYPE("kernel_arg_base_type");
    const std::string SPIR_ARG_TYPE_QUAL("kernel_arg_type_qual");
    const std::string SPIR_ARG_NAME("kernel_arg_name");

    const std::string SPIR_REQD_WGSIZE("reqd_work_group_size");
  }

  namespace PromotedArgStrings {
    const std::string OCLACC_BIF_UNDEF("udef");
    const std::string OCLACC_BIF_WDIM("wdim");
    const std::string OCLACC_BIF_GLOBSZ("gsz");
    const std::string OCLACC_BIF_GLOBID("gid");
    const std::string OCLACC_BIF_LOCSZ("lsz");
    const std::string OCLACC_BIF_ENQLOCSZ("elsz");
    const std::string OCLACC_BIF_LOCID("lid");
    const std::string OCLACC_BIF_NUMGROUPS("numg");
    const std::string OCLACC_BIF_GROUPID("grid");
    const std::string OCLACC_BIF_GLOBOFF("goff");
    const std::string OCLACC_BIF_GLOBLINID("glinid");
    const std::string OCLACC_BIF_LOCLINID("llinid");
  }

  unsigned getNumPredecessors(const llvm::BasicBlock *BB);
  unsigned getNumSuccessors(const llvm::BasicBlock *BB);
  llvm::BasicBlock* getSingleSuccessor(llvm::BasicBlock *BB);
  const llvm::BasicBlock* getSingleSuccessor(const llvm::BasicBlock *BB);
  llvm::BasicBlock* getUniqueSuccessor(llvm::BasicBlock *BB);
  const llvm::BasicBlock* getUniqueSuccessor(const llvm::BasicBlock *BB);

  unsigned eraseFromParentRecursively(llvm::Instruction *I, bool keepSideEffects = true);
  unsigned moveBefore(llvm::Instruction *SourceI, llvm::Instruction *TargetI);

}

#endif

