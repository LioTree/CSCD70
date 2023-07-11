#include "DFA.h"

/// @todo(CSCD70) Please complete this file.
using namespace llvm;

AnalysisKey SCCP::Key;

void SCCP::printInstDomainValMap(const llvm::Instruction &Inst) const {
  using llvm::outs;
  outs() << Inst << "\n";
  int I = 0;
  for(auto &Var : DomainVector) {
    outs() << "\t" << Var << " :"; 
    const dfa::Constant &C = InstDomainValMap.at(&Inst)[I++];
    outs() << static_cast<std::string>(C) << "\n";
  }
}

bool SCCP::transferFunc(const Instruction &Inst, const DomainVal_t &IDV,
                        DomainVal_t &ODV) {
  return false;
}

SCCP::Result SCCP::run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM) {
  dfa::Variable::Initializer Init{DomainIdMap, DomainVector};
  Init.visit(F);
  for (llvm::BasicBlock &BB : F) {
    for (llvm::Instruction &Inst : BB) {
      InstDomainValMap.insert({&Inst, dfa::Intersect<dfa::Constant>().top(DomainIdMap.size())});
    }
  }
  llvm::BasicBlock &EntryBlock = F.getEntryBlock();
  FlowWL.push_back({nullptr, &EntryBlock});
  printInstDomainValMap(F);
  return std::make_tuple(DomainIdMap, DomainVector, BVs, InstDomainValMap);
}
