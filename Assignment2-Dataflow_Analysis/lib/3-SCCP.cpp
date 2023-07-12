#include "DFA.h"

/// @todo(CSCD70) Please complete this file.
using namespace llvm;

AnalysisKey SCCP::Key;

void SCCP::printInstDomainValMap(const Instruction &Inst) const {
  outs() << Inst << "\n";
  int I = 0;
  for (auto &Var : DomainVector) {
    outs() << "\t" << Var << " :";
    const dfa::Constant &C = InstDomainValMap.at(&Inst)[I++];
    outs() << static_cast<std::string>(C) << "\n";
  }
}

// useless for SCCP
bool SCCP::transferFunc(const Instruction &Inst, const DomainVal_t &IDV,
                        DomainVal_t &ODV) {
  return false;
}

SCCP::Result SCCP::run(Function &F, FunctionAnalysisManager &FAM) {
  dfa::Variable::Initializer Init{DomainIdMap, DomainVector};
  Init.visit(F);
  for (auto &Var : DomainVector) {
    LatCells.insert(std::make_pair(Var, dfa::Constant().top()));
  }

  /*
  for (auto &LatCell : LatCells) {
    const dfa::Variable &Var = LatCell.first;
    dfa::Constant &C = LatCell.second;
    outs() << "Variable: " << Var << " " << static_cast<std::string>(C) << "\n";
  }
  */

  BasicBlock &EntryBlock = F.getEntryBlock();
  FlowWL.push({nullptr, &EntryBlock});

  while (!FlowWL.empty() || !SSAWL.empty()) {
    while (!FlowWL.empty()) {
      std::pair<const llvm::BasicBlock *, const llvm::BasicBlock *> Edge =
          FlowWL.front();
      FlowWL.pop();
      if (ExecFlags.find(Edge) == ExecFlags.end()) {
        ExecFlags.insert({Edge, true});
        for (const auto &Phi : (Edge.second)->phis()) {
          outs() << "Phi:" << Phi << "\n";
          visitPhi(Phi);
        }
        if (BBExecFlags.find(Edge.second) == BBExecFlags.end()) {
          BBExecFlags.insert({Edge.second, true});
          const Instruction *Inst = (Edge.second)->getFirstNonPHI();
          while (Inst != nullptr) {
            // outs() << "Inst:" << *Inst << "\n";
            visitInstruction(*Inst);
            Inst = Inst->getNextNonDebugInstruction();
          }
        }
      }
    }

    while (!SSAWL.empty()) {
      std::pair<const llvm::Instruction *, const llvm::Instruction *> Edge =
          SSAWL.front();
      SSAWL.pop();
      // const Instruction *Source = Edge.first;
      const Instruction *Sink = Edge.second;
      const PHINode *Phi = dyn_cast<const PHINode>(Sink);
      if (Phi)
        visitPhi(*Phi);
      else if (BBExecFlags.find(Sink->getParent()) != BBExecFlags.end())
        visitInstruction(*Sink);
    }
  }

  return std::make_tuple(DomainIdMap, DomainVector, BVs, InstDomainValMap);
}

dfa::Constant SCCP::getConstant(const Value *V) const {
  const ConstantInt *CI = dyn_cast<ConstantInt>(V);
  if (CI != nullptr) {
    dfa::Constant Result;
    Result.ConstantType = dfa::SC;
    Result.Value = CI->getSExtValue();
    return Result;
  } else {
    return LatCells.at(dfa::Variable(V));
  }
}

void SCCP::visitPhi(const PHINode &Phi) {
  dfa::Variable PhiVar = dfa::Variable(&Phi);
  dfa::Constant LatCellPhi = LatCells[PhiVar];
  for (unsigned int i = 0; i < Phi.getNumOperands(); i++) {
    if (ExecFlags.find(std::make_pair(Phi.getIncomingBlock(i),
                                      Phi.getParent())) != ExecFlags.end()) {
      Value *IncomingValue = Phi.getIncomingValue(i);
      dfa::Constant LatCellUk = getConstant(IncomingValue);
      /*
      ConstantInt *IncomingConstant = dyn_cast<ConstantInt>(IncomingValue);
      if (IncomingConstant != nullptr) {
        LatCellUk.ConstantType = dfa::SC;
        LatCellUk.Value = IncomingConstant->getSExtValue();
      } else {
        LatCellUk = LatCells[dfa::Variable(IncomingValue)];
      }
      */
      LatCellPhi = LatCellPhi & LatCellUk;
    }
  }
  if (LatCells[PhiVar] != LatCellPhi) {
    for (auto &U : Phi.uses()) {
      Instruction *I = dyn_cast<Instruction>(U.getUser());
      if (I) {
        SSAWL.push({&Phi, I});
      }
    }
  }
}

void SCCP::visitInstruction(const Instruction &Inst) {
  const BranchInst *BI = dyn_cast<const BranchInst>(&Inst);
  if (BI != nullptr) {
    visitBranch(BI);
    return;
  }
  const ICmpInst *CI = dyn_cast<const ICmpInst>(&Inst);
  if (CI != nullptr) {
    visitIcmp(CI);
    return;
  }
}

void SCCP::visitBranch(const BranchInst *BI) {
  if (BI->isUnconditional()) {
    FlowWL.push(std::make_pair(BI->getParent(), BI->getSuccessor(0)));
  } else {
    dfa::Variable ConditionVar = dfa::Variable(BI->getCondition());
    std::string ConditionVarStr =
        static_cast<std::string>(LatCells[ConditionVar]);
    // ConditionVarStr cannot be "UNDEF"
    if (ConditionVarStr == "NAC") {
      for (const BasicBlock *NextBB : BI->successors()) {
        FlowWL.push(std::make_pair(BI->getParent(), NextBB));
      }
    } else {
      if (ConditionVarStr == "1")
        FlowWL.push(std::make_pair(BI->getParent(), BI->getSuccessor(0)));
      else if (ConditionVarStr == "0")
        FlowWL.push(std::make_pair(BI->getParent(), BI->getSuccessor(1)));
    }
  }
}

void SCCP::visitIcmp(const llvm::ICmpInst *CI) {
  // dfa::Variable
}