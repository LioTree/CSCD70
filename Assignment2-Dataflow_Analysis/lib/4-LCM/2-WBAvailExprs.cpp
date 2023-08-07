#include "LCM.h"

using namespace llvm;

AnalysisKey WBAvailExprs::Key;

/// @todo(CSCD70) Please complete this file.
bool WBAvailExprs::transferFunc(const Instruction &Inst, const DomainVal_t &IDV,
                                DomainVal_t &ODV) {
  const BinaryOperator *BO = dyn_cast<const BinaryOperator>(&Inst);
  DomainVal_t TDV = IDV;
  if (BO) {
    for (std::size_t I = 0; I < TDV.size(); I++) {
      // IN - kill
      if (DomainVector[I].contain(&Inst)) {
        TDV[I] = TDV[I] & dfa::Bool();
      }
    }
  }
  // GEN
  DomainVal_t AnticipatedDomainVal = AnticipatedExprsInstDomainValMap.at(&Inst);
  for (std::size_t I = 0; I < AnticipatedDomainVal.size(); I++) {
    TDV[I] = TDV[I] | AnticipatedDomainVal[I];
  }
  if (TDV == ODV)
    return false;
  ODV = TDV;
  return true;
}

WBAvailExprs::Result WBAvailExprs::run(Function &F,
                                       FunctionAnalysisManager &FAM) {
  initPhiMap(F);
  AnticipatedExprs::Result AnticipatedExprsResult =
      FAM.getResult<AnticipatedExprs>(F);
  for (auto &entry : std::get<0>(AnticipatedExprsResult))
    DomainIdMap.insert(entry);
  for (auto &entry : std::get<1>(AnticipatedExprsResult))
    DomainVector.push_back(entry);
  AnticipatedExprsInstDomainValMap = std::get<3>(AnticipatedExprsResult);

  for (llvm::BasicBlock &BB : F) {
    for (llvm::Instruction &Inst : BB) {
      InstDomainValMap.insert(
          {&Inst, dfa::Intersect<dfa::Bool>().top(DomainIdMap.size())});
    }
  }
  while (traverseCFG(F))
    ;
  Framework_t::printInstDomainValMap(F);
  llvm::outs() << "---------------------------\n";

  return std::make_tuple(DomainIdMap, DomainVector, BVs, InstDomainValMap);
}