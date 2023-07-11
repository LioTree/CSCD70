#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

#define UNDEF 0
#define SC 1
#define NAC 2

struct Constant {
  int Value = 0;
  int ConstantType = UNDEF;

  Constant operator&(const Constant &Other) const {
    if (ConstantType == NAC || Other.ConstantType == NAC)
      return {.ConstantType = NAC};
    if (ConstantType == UNDEF)
      return {.Value = Other.Value, .ConstantType = Other.ConstantType};
    else if (Other.ConstantType == UNDEF)
      return {.Value = Value, .ConstantType = ConstantType};
    else if (Value == Other.Value)
      return {.Value = Value, .ConstantType = SC};
    else
      return {.ConstantType = NAC};
  }

  bool operator==(const Constant &other) const {
    if (ConstantType == other.ConstantType)
      return (ConstantType == SC && Value != other.Value) ? false : true;
    return false;
  }

  static Constant top() { return {}; }

  explicit operator std::string() const {
    if (ConstantType == UNDEF)
      return "UNDEF";
    else if (ConstantType == NAC)
      return "NAC";
    else
      return std::to_string(Value);
  }
};

void printConstant(Constant constant) {
  if (constant.ConstantType == NAC)
    std::cout << "NAC"
              << " ";
  else if (constant.ConstantType == UNDEF)
    std::cout << "UNDEF"
              << " ";
  else
    std::cout << constant.Value << " ";
}

void meetConstants(Constant constant1, Constant constant2) {
  // printConstant(constant1);
  std::cout << static_cast<std::string>(constant1) << "& "
            << static_cast<std::string>(constant2) << "= "
            << static_cast<std::string>(constant1 & constant2) << "\n";
  // printConstant(constant2);
  // std::cout << "= ";
  // printConstant(constant1 & constant2);
  // std::cout << "\n";
}

Constant generateConstant() {
  Constant constant;
  int random = rand() % 3;
  if (random == 0) {
    constant.ConstantType = NAC;
  } else if (random == 1) {
    constant.ConstantType = UNDEF;
  } else {
    constant.ConstantType = SC;
    constant.Value = rand() % 100;
  }
  return constant;
}

int main() {
  srand(time(nullptr));
  for (int i = 0; i < 20; i++) {
    Constant constant1 = generateConstant();
    Constant constant2 = generateConstant();
    meetConstants(constant1, constant2);
  }

  return 0;
}