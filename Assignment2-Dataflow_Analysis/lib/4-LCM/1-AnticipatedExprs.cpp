#include "LCM.h"

using namespace llvm;

AnalysisKey AnticipatedExprs::Key;

/// @todo(CSCD70) Please complete this file.
bool AnticipatedExprs::transferFunc(const Instruction &Inst,
                                    const DomainVal_t &IDV, DomainVal_t &ODV) {
  const BinaryOperator *BO = dyn_cast<const BinaryOperator>(&Inst);
  DomainVal_t TDV = IDV;
  if (BO) {
    for (std::size_t I = 0; I < TDV.size(); I++) {
      // IN - kill
      if (DomainVector[I].contain(getRealValue(&Inst))) {
        TDV[I] = TDV[I] & dfa::Bool();
      }
    }
    // GEN
    if (Inst.isBinaryOp()) {
      TDV[DomainIdMap.at(dfa::Expression(BO->getOpcode(),getRealValue(BO->getOperand(0)),getRealValue(BO->getOperand(1))))] = dfa::Bool::top();
    }
  }
  if (TDV == ODV)
    return false;
  ODV = TDV;
  return true;
}

const Value *AnticipatedExprs::getRealValue(const Value *Val) const {
  if (PhiMap.find(Val) != PhiMap.end())
    return getRealValue(PhiMap.at(Val));
  else
    return Val;
}

AnticipatedExprs::MeetOperands_t
AnticipatedExprs::getMeetOperands(const BasicBlock &BB) const {
  MeetOperands_t Operands;

  MeetBBConstRange_t MeetBBConstRange = getMeetBBConstRange(BB);
  for (const llvm::BasicBlock *MeetBB : MeetBBConstRange) {
    DomainVal_t DV = InstDomainValMap.at(&(MeetBB->front()));
    Operands.push_back(DV);
  }
  return Operands;
}

AnticipatedExprs::Result AnticipatedExprs::run(Function &F,
                                               FunctionAnalysisManager &FAM) {
  for(auto &BB : F) {
    for (llvm::Instruction &Inst : BB) {
      const PHINode *PHI = dyn_cast<const PHINode>(&Inst);
      if(PHI) {
        const Value *PHIVal = dyn_cast<const Value>(&Inst);
        for (unsigned I = 0; I < PHI->getNumIncomingValues(); I++) {
          const Value *Val = PHI->getIncomingValue(I);
          PhiMap.insert(std::make_pair(Val, PHIVal));
        }
      }
    }
  }
  
  for(auto &BB : F) {
    for (llvm::Instruction &Inst : BB) {
      const BinaryOperator *BO = dyn_cast<const BinaryOperator>(&Inst);
      if(BO) {
        dfa::Expression Expr{BO->getOpcode(), getRealValue(BO->getOperand(0)), getRealValue(BO->getOperand(1))};
        if(DomainIdMap.find(Expr) == DomainIdMap.end()) {
          DomainVector.push_back(Expr);
          DomainIdMap.insert(std::make_pair(Expr, DomainVector.size() - 1));
        }
      }
    }
  }

  for (llvm::BasicBlock &BB : F) {
    for (llvm::Instruction &Inst : BB) {
      InstDomainValMap.insert({&Inst, dfa::Intersect<dfa::Bool>().top(DomainIdMap.size())});
    }
  }
  while (traverseCFG(F))
    ;
  Framework_t::printInstDomainValMap(F);
  llvm::outs() << "---------------------------\n";

  return std::make_tuple(DomainIdMap, DomainVector, BVs, InstDomainValMap);
}