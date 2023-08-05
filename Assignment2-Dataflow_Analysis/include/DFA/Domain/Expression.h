#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/InstVisitor.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "Base.h"
#include "Utility.h"

namespace dfa {

struct Expression final : DomainBase<Expression> {
  const unsigned Opcode;
  const llvm::Value *const LHS = nullptr, *const RHS = nullptr;
  Expression(const llvm::BinaryOperator &BinaryOp)
      : Opcode(BinaryOp.getOpcode()), LHS(BinaryOp.getOperand(0)),
        RHS(BinaryOp.getOperand(1)) {}
  Expression(const unsigned Opcode, const llvm::Value *const LHS,
             const llvm::Value *const RHS)
      : Opcode(Opcode), LHS(LHS), RHS(RHS) {}

  bool operator==(const Expression &Other) const final {

    /// @done(CSCD70) Please complete this method.
    if (Other.Opcode == Opcode)
      return (LHS == Other.LHS && RHS == Other.RHS) ||
             (LHS == Other.RHS && RHS == Other.LHS);
    return false;
  }

  bool operator<(const Expression &Other) const {
    return Opcode < Other.Opcode && LHS < Other.LHS && RHS < Other.RHS;
  }

  bool contain(const llvm::Value *const Val) const final {

    /// @done(CSCD70) Please complete this method.
    if (Val == LHS || Val == RHS)
      return true;

    return false;
  }

  Expression replaceValueWith(const llvm::Value *const SrcVal,
                              const llvm::Value *const DstVal) const final {

    /// @done(CSCD70) Please complete this method.
    const llvm::Value *const newLHS = (SrcVal == LHS) ? DstVal : LHS;
    const llvm::Value *const newRHS = (SrcVal == RHS) ? DstVal : RHS;
    return Expression{Opcode, newLHS, newRHS};
  }

  using DomainBase<Expression>::DomainIdMap_t;
  using DomainBase<Expression>::DomainVector_t;

  struct Initializer : public llvm::InstVisitor<Initializer> {
    DomainIdMap_t &DomainIdMap;
    DomainVector_t &DomainVector;
    explicit Initializer(DomainIdMap_t &DomainIdMap,
                         DomainVector_t &DomainVector)
        : DomainIdMap(DomainIdMap), DomainVector(DomainVector) {}
    void visitBinaryOperator(llvm::BinaryOperator &);
  };
};

} // namespace dfa

llvm::raw_ostream &operator<<(llvm::raw_ostream &, const dfa::Expression &);

namespace std {

template <> struct hash<::dfa::Expression> {
  size_t operator()(const dfa::Expression &Expr) const {
    size_t HashVal = 0;

    /// @done(CSCD70) Please complete this method.
    hashCombine<unsigned, const llvm::Value *, const llvm::Value *>(
        &HashVal, Expr.Opcode, Expr.LHS, Expr.RHS);
    return HashVal;
  }
};

} // namespace std
