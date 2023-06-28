#include "DFA.h"

using namespace llvm;

AnalysisKey AvailExprs::Key;

bool AvailExprs::transferFunc(const Instruction &Inst, const DomainVal_t &IDV,
                             DomainVal_t &ODV) {

  /// @done(CSCD70) Please complete this method.
  // IN - kill
  const llvm::BinaryOperator* BO = llvm::dyn_cast<const llvm::BinaryOperator>(&Inst);
  DomainVal_t TDV = IDV;
  if(BO) {
    for(std::size_t i = 0; i < TDV.size(); i++) {
      if(DomainVector[i].contain(&Inst)) {
        TDV[i] = TDV[i] & dfa::Bool();
      }
    }
    // GEN
    if(Inst.isBinaryOp()) {
      TDV[DomainIdMap.at(dfa::Expression(*BO))] = dfa::Bool::top();
    }
  }
  if(TDV == ODV)
    return false;
  InstDomainValMap[&Inst] = TDV;
  return true;
}
