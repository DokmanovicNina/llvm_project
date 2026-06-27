#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include <unordered_set>
#include <vector>
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/InstIterator.h"

using namespace llvm;

namespace {
struct OurADCE : public FunctionPass {
  static char ID;
  OurADCE() : FunctionPass(ID) {}

  bool isTriviallyLive(Instruction &I) {
    if(I.isTerminator()) {
      return true;
    }
    if (isa<CallBase>(&I)) {
      return true;
    }
    if (auto *SI = dyn_cast<StoreInst>(&I)) {
      if (SI->isVolatile() || SI->isAtomic())
        return true;
    }
    if (auto *LI = dyn_cast<LoadInst>(&I)) {
      if (LI->isVolatile() || LI->isAtomic())
        return true;
    }
    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<AAResultsWrapperPass>();
    AU.setPreservesCFG();
  }

  bool runOnFunction(Function &F) override {
    AAResults &AA = getAnalysis<AAResultsWrapperPass>().getAAResults();
    std::unordered_set<Instruction *> Live;
    std::vector<Instruction *> WorkList;

    for(Instruction &I : instructions(F)) {
      if(isTriviallyLive(I)) {
        if(Live.insert(&I).second) {
          WorkList.push_back(&I);
        }
      }
    }

    std::vector<StoreInst *> Stores;
    for (Instruction &I : instructions(F)) {
      if (auto *SI = dyn_cast<StoreInst>(&I)) {
        Stores.push_back(SI);
      }
    }

    while(!WorkList.empty()) {
      Instruction *I = WorkList.back();
      WorkList.pop_back();

      if (auto *LI = dyn_cast<LoadInst>(I)) {
        Value *loadPtr = LI->getPointerOperand();

        BasicBlock *BB = LI->getParent();
        auto It = LI->getIterator();
        bool foundStore = false;
        while (It != BB->begin()) {
          --It;
          if (auto *SI = dyn_cast<StoreInst>(&*It)) {
            if (AA.alias(SI->getPointerOperand(), loadPtr)
                != AliasResult::NoAlias) {
              if (Live.insert(SI).second) {
                WorkList.push_back(SI);
              }

              foundStore = true;
              break;
                }
          }
        }
        if (!foundStore) {

          for (StoreInst *SI : Stores) {

            if (AA.alias(SI->getPointerOperand(),
                         loadPtr) != AliasResult::NoAlias) {
              if (Live.insert(SI).second) {
                WorkList.push_back(SI);
              }
                         }
          }
        }
      }

      for(Use &Op : I->operands()) {
        if(Instruction *OpInst = dyn_cast<Instruction>(Op.get())) {
          if(Live.insert(OpInst).second) {
            WorkList.push_back(OpInst);
          }
        }
      }
    }

    std::vector<Instruction *> ToRemove;
    for(Instruction &I : instructions(F)) {
      if(Live.find(&I) == Live.end()) {
        ToRemove.push_back(&I);
      }
    }

    bool Changed = false;
    for(auto It = ToRemove.rbegin(); It != ToRemove.rend(); ++It) {
      Instruction *I = *It;
      assert(I->use_empty() && "Dead instruction still has users!");
      I->eraseFromParent();
      Changed = true;
    }

    return Changed;
  }
};
}

char OurADCE::ID = 0;
static RegisterPass<OurADCE> X("our-adce",
                               "Our simplified Aggressive Dead Code Elimination",
                               false,
                               false);
