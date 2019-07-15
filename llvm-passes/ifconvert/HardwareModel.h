//===- HardwareModel.h - Describes hardware ressources --------------------===//
//
// Defines constants, properties and ressources of the used target hardware.
//
//===----------------------------------------------------------------------===//

#ifndef _LOOPUS_HARDWAREMODEL_H_INCLUDE_
#define _LOOPUS_HARDWAREMODEL_H_INCLUDE_

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

namespace Loopus {

class HardwareModel {
private:
  HardwareModel(void)
   : GlobalMemoryBusWidth(64) {
  }
  HardwareModel(const HardwareModel&) = delete;
  HardwareModel& operator=(const HardwareModel&) = delete;

  static HardwareModel HWModel;

  // Width of the bus to the global memory in bits
  unsigned GlobalMemoryBusWidth;

public:
  static const HardwareModel& getHardwareModel(void) {
    return HardwareModel::HWModel;
  }

  unsigned getGMemBusWidthInBytes(void) const {
    return (GlobalMemoryBusWidth / 8);
  }

  unsigned getGMemBusWidthInBits(void) const {
    return GlobalMemoryBusWidth;
  }

};

class RessourceEstimatorBase {
  public:
    RessourceEstimatorBase(void);
    virtual ~RessourceEstimatorBase(void);

    virtual unsigned getRessourceUsage(const llvm::Value *V) =0;
    virtual unsigned getRessourceUsage(const llvm::BasicBlock *BB);
    virtual unsigned getRessourceUsage(const llvm::Function *F);
    virtual unsigned getRessourceUsage(const llvm::Loop *L);
};

class BasicRessourceEstimator : public RessourceEstimatorBase {
  public:
    BasicRessourceEstimator(void);
    virtual ~BasicRessourceEstimator(void);

    using RessourceEstimatorBase::getRessourceUsage;
    virtual unsigned getRessourceUsage(const llvm::Value *V) override;
};

class SimpleRessourceEstimator : public RessourceEstimatorBase {
  private:
    const unsigned DEFAULT_TYPE_SIZE;

  public:
    SimpleRessourceEstimator(void);
    virtual ~SimpleRessourceEstimator(void);

    using RessourceEstimatorBase::getRessourceUsage;
    virtual unsigned getRessourceUsage(const llvm::Value *V) override;
    virtual unsigned getRessourceUsage(const llvm::BasicBlock *BB) override;
};

} // End of Loopus namespace

#endif
