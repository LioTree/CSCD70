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
      if (DomainVector[I].contain(&Inst)) {
        TDV[I] = TDV[I] & dfa::Bool();
      }
    }
    // GEN
    if (Inst.isBinaryOp()) {
      TDV[DomainIdMap.at(dfa::Expression(*BO))] = dfa::Bool::top();
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

  // MeetBBConstRange_t MeetBBConstRange = getMeetBBConstRange(BB);
  // for (const BasicBlock *MeetBB : MeetBBConstRange) {
  //   DomainVal_t DV = InstDomainValMap.at(&(MeetBB->front()));
  //   std::vector<std::pair<const Value *, const Value *>> PhiValues;
  //   std::set<dfa::Expression> PendingExpressions;

  //   for (auto &Phi : MeetBB->phis()) {
  //     for (auto &PhiIncomingBlock : Phi.blocks()) {
  //       if (&BB == PhiIncomingBlock) {
  //         auto It = DomainVector.begin();
  //         for (; It != DomainVector.end(); It++) {
  //           It = std::find_if(It, DomainVector.end(),
  //                             [&Phi](const dfa::Expression &Expr) {
  //                               return Expr.contain(dyn_cast<Value>(&Phi));
  //                             });
  //           int Index = std::distance(DomainVector.begin(), It);
  //           if (It != DomainVector.end() && DV[Index]) {
  //             DV[Index] = dfa::Bool();
  //             PendingExpressions.insert(*It);
  //             PhiValues.push_back(std::make_pair(
  //                 dyn_cast<Value>(&Phi),
  //                 Phi.getIncomingValueForBlock(PhiIncomingBlock)));
  //           }
  //         }
  //       }
  //     }
  //   }
  //   for (auto &Expr : PendingExpressions) {
  //     const dfa::Expression *NewExprPtr = &Expr;
  //     for (auto &PhiValuePair : PhiValues) {
  //       const dfa::Expression Temp = NewExprPtr->replaceValueWith(
  //           PhiValuePair.first, PhiValuePair.second);
  //       NewExprPtr = &Temp;
  //     }
  //     auto It = std::find(DomainVector.begin(), DomainVector.end(), *NewExprPtr);
  //     if (It != DomainVector.end()) {
  //       int Index = std::distance(DomainVector.begin(), It);
  //       DV[Index] = dfa::Bool().top();
  //     }
  //     else {
  //       DomainVector.push_back(*NewExprPtr);
  //       DomainIdMap.insert(std::make_pair(*NewExprPtr, DomainVector.size() - 1));
  //       DV.push_back(dfa::Bool().top());
  //     }
  //   }
  //   Operands.push_back(DV);
    return Operands;
  // }
}