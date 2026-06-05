#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

using namespace llvm;

namespace {
struct OurSimplifyCFG : public FunctionPass {
  static char ID;
  OurSimplifyCFG() : FunctionPass(ID) {}

  unsigned countPredecessors(BasicBlock *BB) {
    unsigned Count = 0;
    for (BasicBlock *Pred : predecessors(BB)) {
      (void)Pred;
      Count++;
    }
    return Count;
  }

  bool removeUnreachableBlocks(Function &F) {
    std::vector<BasicBlock *> ToRemove;
    bool First = true;
    for (BasicBlock &BB : F) {
      if (First) {
        First = false;
        continue;
      }
      if (countPredecessors(&BB) == 0) {
        ToRemove.push_back(&BB);
      }
    }
    for (BasicBlock *BB : ToRemove) {
      BB->replaceAllUsesWith(UndefValue::get(BB->getType()));
      BB->dropAllReferences();
    }
    for (BasicBlock *BB : ToRemove) {
      BB->eraseFromParent();
    }
    return !ToRemove.empty();
  }

  bool simplifyTrivialPHIs(Function &F) {
    bool Changed = false;
    for (BasicBlock &BB : F) {
      if (countPredecessors(&BB) != 1) {
        continue;
      }
      std::vector<PHINode *> PHIsToRemove;
      for (Instruction &I : BB) {
        if (PHINode *PHI = dyn_cast<PHINode>(&I)) {
          if (PHI->getNumIncomingValues() == 1) {
            PHI->replaceAllUsesWith(PHI->getIncomingValue(0));
            PHIsToRemove.push_back(PHI);
          }
        } else {
          break;
        }
      }
      for (PHINode *PHI : PHIsToRemove) {
        PHI->eraseFromParent();
        Changed = true;
      }
    }
    return Changed;
  }

  bool mergeBlocks(Function &F) {
    bool Changed = false;
    bool LocalChange = true;
    while (LocalChange) {
      LocalChange = false;
      for (BasicBlock &BB : F) {
        if (&BB == &F.front()) {
          continue;
        }
        if (countPredecessors(&BB) != 1) {
          continue;
        }
        BasicBlock *Pred = *predecessors(&BB).begin();
        if (Pred == &BB) {
          continue;
        }
        BranchInst *PredBr = dyn_cast<BranchInst>(Pred->getTerminator());
        if (!PredBr || !PredBr->isUnconditional()) {
          continue;
        }
        if (PredBr->getSuccessor(0) != &BB) {
          continue;
        }
        PredBr->eraseFromParent();
        while (!BB.empty()) {
          Instruction &I = BB.front();
          I.removeFromParent();
          I.insertInto(Pred, Pred->end());
        }
        BB.replaceAllUsesWith(Pred);
        BB.eraseFromParent();
        LocalChange = true;
        Changed = true;
        break;
      }
    }
    return Changed;
  }

  bool runOnFunction(Function &F) override {
    bool Changed = false;
    bool LocalChange = true;
    while (LocalChange) {
      LocalChange = false;
      LocalChange |= removeUnreachableBlocks(F);
      LocalChange |= simplifyTrivialPHIs(F);
      LocalChange |= mergeBlocks(F);
      Changed |= LocalChange;
    }
    return Changed;
  }
};
}

char OurSimplifyCFG::ID = 0;
static RegisterPass<OurSimplifyCFG> X("our-simplifycfg",
                                      "Our simplified CFG simplification pass",
                                      false,
                                      false);
