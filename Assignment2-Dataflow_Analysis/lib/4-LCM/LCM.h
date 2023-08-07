#pragma once // NOLINT(llvm-header-guard)

#include <DFA/Domain/Expression.h>
#include <DFA/Flow/BackwardAnalysis.h>
#include <DFA/Flow/ForwardAnalysis.h>
#include <DFA/MeetOp.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <map>

class LCM {
protected:
  std::map<const llvm::Value *, const llvm::Value *> PhiMap;

  const llvm::Value *getRealValue(const llvm::Value *Val) const {
    if (PhiMap.find(Val) != PhiMap.end())
      return getRealValue(PhiMap.at(Val));
    else
      return Val;
  };

  void initPhiMap(const llvm::Function &F) {
    for (auto &BB : F) {
      for (const llvm::Instruction &Inst : BB) {
        const llvm::PHINode *PHI = llvm::dyn_cast<const llvm::PHINode>(&Inst);
        if (PHI) {
          const llvm::Value *PHIVal = llvm::dyn_cast<const llvm::Value>(&Inst);
          for (unsigned I = 0; I < PHI->getNumIncomingValues(); I++) {
            const llvm::Value *Val = PHI->getIncomingValue(I);
            PhiMap.insert(std::make_pair(Val, PHIVal));
          }
        }
      }
    }
  };
};

class AnticipatedExprs final
    : public dfa::BackwardAnalysis<dfa::Expression, dfa::Bool,
                                   dfa::Intersect<dfa::Bool>>,
      public LCM,
      public llvm::AnalysisInfoMixin<AnticipatedExprs> {
private:
  using BackwardAnalysis_t = dfa::BackwardAnalysis<dfa::Expression, dfa::Bool,
                                                   dfa::Intersect<dfa::Bool>>;
  using typename BackwardAnalysis_t::Framework_t;
  using typename BackwardAnalysis_t::MeetBBConstRange_t;
  using typename BackwardAnalysis_t::MeetOperands_t;

  friend llvm::AnalysisInfoMixin<AnticipatedExprs>;
  static llvm::AnalysisKey Key;
  virtual std::string getName() const override { return "AnticipatedExprs"; }
  virtual bool transferFunc(const llvm::Instruction &, const DomainVal_t &,
                            DomainVal_t &) override;
  virtual MeetOperands_t
  getMeetOperands(const llvm::BasicBlock &BB) const override;

public:
  using DomainVal_t = typename BackwardAnalysis_t::DomainVal_t;
  // friend class WBAvailExprs;
  using Result = typename BackwardAnalysis_t::AnalysisResult_t;
  // using BackwardAnalysis_t::run;
  virtual Result run(llvm::Function &F,
                     llvm::FunctionAnalysisManager &FAM) override;
};

class WBAvailExprs final
    : public dfa::ForwardAnalysis<dfa::Expression, dfa::Bool,
                                  dfa::Intersect<dfa::Bool>>,
      public LCM,
      public llvm::AnalysisInfoMixin<WBAvailExprs> {
private:
  using ForwardAnalysis_t = dfa::ForwardAnalysis<dfa::Expression, dfa::Bool,
                                                 dfa::Intersect<dfa::Bool>>;

  friend llvm::AnalysisInfoMixin<WBAvailExprs>;
  static llvm::AnalysisKey Key;
  std::unordered_map<const llvm::Instruction *, AnticipatedExprs::DomainVal_t>
      AnticipatedExprsInstDomainValMap;

  virtual std::string getName() const override { return "WBAvailExprs"; }
  virtual bool transferFunc(const llvm::Instruction &, const DomainVal_t &,
                            DomainVal_t &) override;

public:
  using Result = typename ForwardAnalysis_t::AnalysisResult_t;
  // using ForwardAnalysis_t::run;
  virtual Result run(llvm::Function &F,
                     llvm::FunctionAnalysisManager &FAM) override;
};

class LCMWrapperPass : public llvm::PassInfoMixin<LCMWrapperPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {

    /// @todo(CSCD70) Get the result from the main body.
    // FAM.getResult<AnticipatedExprs>(F);
    FAM.getResult<WBAvailExprs>(F);
    return llvm::PreservedAnalyses::all();
  }
};
