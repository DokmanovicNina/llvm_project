#include "llvm/IR/Function.h"
#include <iostream>
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <unordered_map>
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/IRBuilder.h"
#include <vector>
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <unordered_set>
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/IR/Dominators.h"
#include <set>

using namespace llvm;

namespace {
    struct MyAggressiveInstcombinePass : public FunctionPass {
        static char ID;
        MyAggressiveInstcombinePass() : FunctionPass(ID) {}
        
        std::unordered_set<BasicBlock *> DeadBlocks;
        std::unordered_map<Value*, Value*> VariablesMap;
        DominatorTree *DT; 

        void getAnalysisUsage(AnalysisUsage &AU) const override {
            AU.addRequired<DominatorTreeWrapperPass>();
        }

        Value* resolveValue(Value *V) {
            if (!V) return nullptr;
            while (VariablesMap.find(V) != VariablesMap.end()) {
                V = VariablesMap[V];
            }
            return V;
        }

        Value* resolveOperandChain(Value *Op, Type *DestTy) {

            if (isa<ConstantInt>(Op)) {
                return Op;
            }

            Value *ResolvedOp = resolveValue(Op);

            CastInst *Cast = dyn_cast<CastInst>(ResolvedOp);
            if (!Cast) return nullptr;
            if (Cast->getOpcode() != Instruction::SExt && Cast->getOpcode() != Instruction::ZExt) {
                return nullptr;
            }

            Value *CastSrc = resolveValue(Cast->getOperand(0));
            TruncInst *Trunc = dyn_cast<TruncInst>(CastSrc);
            if (!Trunc) return nullptr;

            Value *WideValue = resolveValue(Trunc->getOperand(0));


            if (WideValue->getType() != DestTy) {
                return nullptr;
            }

            return WideValue;
        }

        bool OptimizeWideBinOp(Instruction *I) {
            BinaryOperator *BinOp = dyn_cast<BinaryOperator>(I);
            if (!BinOp) return false;

            Type *DestTy = BinOp->getType();

            if (isa<ConstantInt>(BinOp->getOperand(0)) && isa<ConstantInt>(BinOp->getOperand(1))) {
                return false;
            }

            Value *NewLeft = resolveOperandChain(BinOp->getOperand(0), DestTy);
            Value *NewRight = resolveOperandChain(BinOp->getOperand(1), DestTy);

            if (!NewLeft || !NewRight) return false;

            Instruction *LeftInst = dyn_cast<Instruction>(NewLeft);
            Instruction *RightInst = dyn_cast<Instruction>(NewRight);
            if (LeftInst && !DT->dominates(LeftInst->getParent(), BinOp->getParent())) return false;
            if (RightInst && !DT->dominates(RightInst->getParent(), BinOp->getParent())) return false;

            IRBuilder<> Builder(BinOp);
            Value *NewBinOp = Builder.CreateBinOp(BinOp->getOpcode(), NewLeft, NewRight, BinOp->getName() + ".direct");

            BinOp->replaceAllUsesWith(NewBinOp);

            RecursivelyDeleteTriviallyDeadInstructions(BinOp);

            return true;
        }

        bool RemoveOrChain(BranchInst *BI) {
            if (!BI->isConditional()) return false;

            ICmpInst *Cmp1 = dyn_cast<ICmpInst>(BI->getCondition());
            if (!Cmp1) return false;

            ConstantInt *Const1 = dyn_cast<ConstantInt>(Cmp1->getOperand(1));
            if (!Const1) return false; 

            CmpInst::Predicate Pred = Cmp1->getPredicate();
            Value *Variable = Cmp1->getOperand(0);
            
            BasicBlock *ThenBB = BI->getSuccessor(0);       
            BasicBlock *CurrentFalseBB = BI->getSuccessor(1); 

            IRBuilder<> Builder(BI);
            Value *AccumulatedCond = Cmp1; 
            bool Changed = false;

            while (BranchInst *NextBI = dyn_cast<BranchInst>(CurrentFalseBB->getTerminator())) {
                if (!NextBI->isConditional() || NextBI->getSuccessor(0) != ThenBB) 
                    break; 

                ICmpInst *Cmp2 = dyn_cast<ICmpInst>(NextBI->getCondition());
                if (!Cmp2) break;

                if (Cmp2->getPredicate() != Pred) 
                    break;

                if (VariablesMap[Cmp2->getOperand(0)] != VariablesMap[Variable]) 
                    break;

                ConstantInt *Const2 = dyn_cast<ConstantInt>(Cmp2->getOperand(1));
                if (!Const2) 
                    break; 

                DeadBlocks.insert(CurrentFalseBB);

                Value *NewCmp = Builder.CreateICmp(Pred, Variable, Const2, "cmp.combined");
                AccumulatedCond = Builder.CreateOr(AccumulatedCond, NewCmp, "or.cond");

                CurrentFalseBB = NextBI->getSuccessor(1);
                Changed = true;
            }

            if (Changed) {
                BI->setCondition(AccumulatedCond);
                BI->setSuccessor(1, CurrentFalseBB);
                return true;
            }

            return false;
        }

        bool runOnFunction(Function &F) override {
            DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();

            bool Changed = false;

            VariablesMap.clear();
            DeadBlocks.clear();

            for(BasicBlock &BB: F){
                for(Instruction &I: BB){
                    if (auto *SI = dyn_cast<StoreInst>(&I)) {
                        VariablesMap[SI->getPointerOperand()] = SI->getValueOperand();
                    }
                    else if (auto *LI = dyn_cast<LoadInst>(&I)) {
                        VariablesMap[&I] = LI->getPointerOperand();
                    }
                }
            }

            for (BasicBlock &BB : F) {
                if (DeadBlocks.count(&BB)) continue;

                if (BranchInst *BI = dyn_cast<BranchInst>(BB.getTerminator())) {
                    if (RemoveOrChain(BI)) {
                        Changed = true;
                    }
                }
            }

            for (BasicBlock &BB : F) {
                if (DeadBlocks.count(&BB)) continue;

                for (auto II = BB.begin(), IE = BB.end(); II != IE; ) {
                    Instruction &I = *II++;
                    if (OptimizeWideBinOp(&I)) {
                        Changed = true;
                    }
                }
            }

            for (BasicBlock *DB : DeadBlocks) {
                Instruction *Term = DB->getTerminator();
                IRBuilder<> DeadBuilder(Term);
                DeadBuilder.CreateUnreachable(); 
                Term->eraseFromParent();         
            }

            if (Changed) {
                EliminateUnreachableBlocks(F);
            }

            return Changed;
        }
    };
}

char MyAggressiveInstcombinePass::ID = 0;
static RegisterPass<MyAggressiveInstcombinePass> X("my-aggressive-instcombine-pass", "This is a simple version of llvm aggressive instcombine pass");