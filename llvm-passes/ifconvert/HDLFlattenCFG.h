//===- HDLFlattenCFG.h - Try flattening the CFG by using selects ----------===//
//
// Try to flatten the CFG by replacing branches by select instructions. Other
// LLVM passes might be needed to finally remove the possibly arising dead
// blocks.
//
//===----------------------------------------------------------------------===//

#ifndef _LOOPUS_HDLFLATTENCFG_H_INCLUDE_
#define _LOOPUS_HDLFLATTENCFG_H_INCLUDE_

#include "HardwareModel.h"

#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/ValueMapper.h"

#include <set>

namespace Loopus {
  class IfPattern {
    private:
      llvm::BasicBlock *HeadBB;
      llvm::BasicBlock *TBB;
      llvm::BasicBlock *FBB;
      llvm::BasicBlock *TailBB;

    protected:
      bool adaptPhiOps(llvm::AliasAnalysis *AA = nullptr);
      bool replacePhis(llvm::AliasAnalysis *AA = nullptr);
      void updateDomTree(llvm::DominatorTree *DT, bool mergedTail);

    public:
      IfPattern(void);
      IfPattern(llvm::BasicBlock *Head, llvm::BasicBlock *Tail);
      IfPattern(llvm::BasicBlock *Head, llvm::BasicBlock *Tail,
          llvm::BasicBlock *TBB, llvm::BasicBlock *FBB);

      llvm::BasicBlock* getHead(void);
      const llvm::BasicBlock* getHead(void) const;
      llvm::BasicBlock* getTBB(void);
      const llvm::BasicBlock* getTBB(void) const;
      llvm::BasicBlock* getFBB(void);
      const llvm::BasicBlock* getFBB(void) const;
      llvm::BasicBlock* getTail(void);
      const llvm::BasicBlock* getTail(void) const;
      void setHead(llvm::BasicBlock *newHead);
      void setTBB(llvm::BasicBlock *newTBB);
      void setFBB(llvm::BasicBlock *newFBB);
      void setTail(llvm::BasicBlock *newTail);
      llvm::BasicBlock* getSingleBranchedBB(void);
      const llvm::BasicBlock* getSingleBranchedBB(void) const;

      bool isTriangle(void) const;
      bool isDiamond(void) const;

      bool canIfConvert(const llvm::DataLayout *DL) const;
      bool shouldIfConvert(Loopus::RessourceEstimatorBase &RE) const;
      bool doIfConversion(llvm::DominatorTree *DT, llvm::AliasAnalysis *AA = nullptr,
          llvm::LoopInfo *LI = nullptr, llvm::Pass *P = nullptr,
          llvm::ScalarEvolution *SE = nullptr);
  };

  class SwitchPattern {
    private:
      llvm::BasicBlock *HeadBB;
      llvm::BasicBlock *TailBB;

    protected:
      bool injectCompares(llvm::ValueToValueMapTy &ValueMap,
          llvm::Instruction* insertBefore = nullptr);
      bool adaptPhiOps(llvm::ValueToValueMapTy &CaseCmpMap, llvm::AliasAnalysis *AA = nullptr);
      bool replacePhis(llvm::ValueToValueMapTy &CaseCmpMap, llvm::AliasAnalysis *AA = nullptr);
      void updateDomTree(llvm::DominatorTree *DT,
          const std::set<llvm::BasicBlock*> &removedBlocks, bool mergedTail);

    public:
      SwitchPattern(void);
      SwitchPattern(llvm::BasicBlock *Head, llvm::BasicBlock *Tail);

      llvm::BasicBlock* getHead(void);
      const llvm::BasicBlock* getHead(void) const;
      llvm::BasicBlock* getTail(void);
      const llvm::BasicBlock* getTail(void) const;
      void setHead(llvm::BasicBlock *newHead);
      void setTail(llvm::BasicBlock *newTail);

      bool hasDedicatedDefault(void) const;

      bool canIfConvert(const llvm::DataLayout *DL) const;
      bool shouldIfConvert(Loopus::RessourceEstimatorBase &RE) const;
      bool doIfConversion(llvm::DominatorTree *DT, llvm::AliasAnalysis *AA = nullptr,
          llvm::LoopInfo *LI = nullptr, llvm::Pass *P = nullptr,
          llvm::ScalarEvolution *SE = nullptr);
  };

  class HDLFlattenCFGWorker {
    private:
      llvm::AliasAnalysis *AA;
      const llvm::DataLayout *DL;
      llvm::DominatorTree *DT;
      llvm::LoopInfo *LI;
      llvm::Pass *P;
      llvm::ScalarEvolution *SE;
      RessourceEstimatorBase *RE;

    public:
      HDLFlattenCFGWorker(llvm::AliasAnalysis *AA, const llvm::DataLayout *DL,
          llvm::DominatorTree *DT, llvm::LoopInfo *LI, llvm::Pass *P,
          llvm::ScalarEvolution *SE, RessourceEstimatorBase *RE);

      bool runOnBasicBlock(llvm::BasicBlock *BB);
      bool runOnFunction(llvm::Function *F);
  };

  bool isHeadOfIfPattern(llvm::BasicBlock *BB, IfPattern *Pattern = nullptr);
  bool isHeadOfSwitchPattern(llvm::BasicBlock *BB, SwitchPattern *Pattern = nullptr);
  bool canExecuteSpeculatively(const llvm::BasicBlock *BB, const llvm::DataLayout *DL);
  bool canExecuteSpeculatively(const llvm::Value *V, const llvm::DataLayout *DL);
} // End of Loopus namespace

class HDLFlattenCFG : public llvm::FunctionPass {
  public:
    static char ID;

    HDLFlattenCFG(void);
    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
    virtual bool runOnFunction(llvm::Function &F) override;
};

#endif

