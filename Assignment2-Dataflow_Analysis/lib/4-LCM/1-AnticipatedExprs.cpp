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
  initPhiMap(F);
  
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