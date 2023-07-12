
#include "DFA.h"

using namespace llvm;

AnalysisKey AvailExprs::Key;

bool AvailExprs::transferFunc(const Instruction &Inst, const DomainVal_t &IDV,
                             DomainVal_t &ODV) {

  /// @done(CSCD70) Please complete this method.
  const llvm::BinaryOperator* BO = llvm::dyn_cast<const llvm::BinaryOperator>(&Inst);
  DomainVal_t TDV = IDV;
  if(BO) {
    for(std::size_t I = 0; I < TDV.size(); I++) {
      // IN - kill
      if(DomainVector[I].contain(&Inst)) {
        TDV[I] = TDV[I] & dfa::Bool();
      }
    }
    // GEN
    if(Inst.isBinaryOp()) {
      TDV[DomainIdMap.at(dfa::Expression(*BO))] = dfa::Bool::top();
    }
  }
  if(TDV == ODV)
    return false;
  ODV = TDV;
  return true;
}

