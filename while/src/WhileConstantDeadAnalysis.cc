// This file is part of While, an educational programming language and program
// analysis framework.
//
//   Copyright 2023 Florian Brandner
//
// While is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// While is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// While. If not, see <https://www.gnu.org/licenses/>.
//
// Contact: florian.brandner@telecom-paris.fr
//

// A simple analysis determining whether symbolic registers contain a constant
// value.

#include "WhileAnalysis.h"
#include "WhileLang.h"
#include "WhileCFG.h"
#include "WhileColor.h"

enum WhileConstantKind
{
  TOP,
  BOTTOM,
  CONSTANT
};

enum WhileReachability
{
  REACHABLE,
  DEAD
};

struct WhileConstantValue
{
  WhileConstantKind Kind;
  int Value;

  WhileConstantValue() : Kind(TOP), Value(0)
  {
  }

  WhileConstantValue(int value) : Kind(CONSTANT), Value(value)
  {
  }

  WhileConstantValue(WhileConstantKind kind) : Kind(kind), Value(0)
  {
  }
};

typedef std::map<int, WhileConstantValue> WhileConstantDomain;

struct WhileConstantDeadAnalysisDomain
{
  WhileConstantDomain originalConstantDomain;
  WhileReachability currentBlockReachability;
  WhileReachability takenBranchReachbility;
};

extern bool operator==(const WhileConstantValue &a, const WhileConstantValue &b);

bool operator==(const WhileConstantDeadAnalysisDomain &a, const WhileConstantDeadAnalysisDomain &b)
{
  return (
      a.originalConstantDomain == b.originalConstantDomain && a.currentBlockReachability == b.currentBlockReachability && a.takenBranchReachbility == b.takenBranchReachbility);
}

bool operator!=(const WhileConstantDeadAnalysisDomain &a, const WhileConstantDeadAnalysisDomain &b)
{
  return !(a == b);
}

extern std::ostream &operator<<(std::ostream &s, WhileReachability r);
extern std::ostream &operator<<(std::ostream &s, const WhileConstantValue &v);

std::ostream &operator<<(std::ostream &s, const WhileConstantDomain &v)
{
  s << "    [";
  bool first = true;
  for (const auto &[idx, c] : v)
  {
    if (!first)
      s << ", ";

    s << "R" << idx << "=" << c;
    first = false;
  }
  return s << "]";
  abort();
}

std::ostream &operator<<(std::ostream &s, const WhileConstantDeadAnalysisDomain &v)
{
  return s << v.originalConstantDomain << "\n"
           << "Fall Through: " << v.currentBlockReachability << "\n"
           << "Branch Taken: " << v.takenBranchReachbility << "\n";
}

struct WhileConstantDead : public WhileDataFlowAnalysis<WhileConstantDeadAnalysisDomain>
{
  std::ostream &dump_first(std::ostream &s,
                           const WhileConstantDeadAnalysisDomain &value) override
  {
    return s << CRESET << value.originalConstantDomain << "\n";
  }

  std::ostream &dump_pre(std::ostream &s,
                         const WhileConstantDeadAnalysisDomain &value) override
  {
    switch (value.currentBlockReachability)
    {
    case DEAD:
      return s << FRED;
    case REACHABLE:
      return s << FGREEN;
    };
    abort();
  }

  std::ostream &dump_post(std::ostream &s,
                          const WhileConstantDeadAnalysisDomain &value) override
  {
    return dump_first(s, value);
    // return s << CRESET << value.originalConstantDomain << "\n";
    // return s << CRESET << value << "\n";
  }

  static void updateRegisterOperand(const WhileInstr &instr, unsigned int idx,
                                    WhileConstantDeadAnalysisDomain &result,
                                    WhileConstantValue value)
  {
    const WhileOperand &op = instr.Ops[idx];
    switch (op.Kind)
    {
    case WREGISTER:
      assert(op.ValueOrIndex >= 0);
      result.originalConstantDomain[op.ValueOrIndex] = value;
      return;

    case WFRAMEPOINTER:
    case WIMMEDIATE:
    case WBLOCK:
    case WFUNCTION:
    case WUNKNOWN:
      assert("Operand is not a register.");
    }
    abort();
  }

  static WhileConstantValue readDataOperand(const WhileInstr &instr,
                                            unsigned int idx,
                                            const WhileConstantDeadAnalysisDomain &input)
  {
    const WhileOperand &op = instr.Ops[idx];
    switch (op.Kind)
    {
    case WREGISTER:
    {
      assert(op.ValueOrIndex >= 0);
      auto value = input.originalConstantDomain.find(op.ValueOrIndex);
      if (value == input.originalConstantDomain.end())
        return BOTTOM; // register undefined
      else
        return value->second;
    }
    case WIMMEDIATE:
      return op.ValueOrIndex;

    case WFRAMEPOINTER:
      return BOTTOM;

    case WBLOCK:
    case WFUNCTION:
    case WUNKNOWN:
      assert("Operand is not a data value.");
    }
    abort();
  }

  WhileConstantDeadAnalysisDomain transfer(const WhileInstr &instr, const WhileConstantDeadAnalysisDomain input) override
  {
    WhileConstantDeadAnalysisDomain result = input;
    const auto &ops = instr.Ops;
    switch (instr.Opc)
    {
    case WBRANCHZ:
    {
      // result.currentBlockReachability = REACHABLE;
      // result.takenBranchReachbility = REACHABLE;
      WhileConstantValue condition = readDataOperand(instr, 0, input);
      if (condition.Kind == CONSTANT)
      {
        if (condition.Value)
        {
          result.takenBranchReachbility = DEAD;
          result.currentBlockReachability = input.currentBlockReachability;
        }
        else
        {
          result.currentBlockReachability = DEAD;
          result.takenBranchReachbility = input.currentBlockReachability;
        }
      }
      else
      {
        result.currentBlockReachability = REACHABLE;
        result.takenBranchReachbility = REACHABLE;
      }
      break;
    }
    case WBRANCH:
    {
      result.currentBlockReachability = DEAD;
      result.takenBranchReachbility = REACHABLE;
      break;
    }
    case WRETURN:
    {
      result.currentBlockReachability = DEAD;
      result.takenBranchReachbility = DEAD;
      break;
    }
    case WSTORE:
      // do not write symbolic registers
      break;

    case WCALL:
    {
      // Ops: Fun Opd = Arg1, Arg2, ... ArgN
      assert(ops.size() > 2);
      updateRegisterOperand(instr, 1, result, BOTTOM);
      break;
    }

    case WLOAD:
    {
      // Ops: OpD = [BaseAddress + Offset]
      assert(ops.size() == 3);
      updateRegisterOperand(instr, 0, result, BOTTOM);
      break;
    }

    case WPLUS:
    {
      // Ops: OpD = OpA + OpB
      assert(ops.size() == 3);
      WhileConstantValue a = readDataOperand(instr, 1, input);
      WhileConstantValue b = readDataOperand(instr, 2, input);

      if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        updateRegisterOperand(instr, 0, result, a.Value + b.Value);
      else
        updateRegisterOperand(instr, 0, result, BOTTOM);
      break;
    }
    case WMINUS:
    {
      // Ops: OpD = OpA - OpB
      assert(ops.size() == 3);
      WhileConstantValue a = readDataOperand(instr, 1, input);
      WhileConstantValue b = readDataOperand(instr, 2, input);

      if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        updateRegisterOperand(instr, 0, result, a.Value - b.Value);
      else
        updateRegisterOperand(instr, 0, result, BOTTOM);
      break;
    }
    case WMULT:
    {
      // Ops: OpD = OpA * OpB
      assert(ops.size() == 3);
      WhileConstantValue a = readDataOperand(instr, 1, input);
      WhileConstantValue b = readDataOperand(instr, 2, input);

      if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        updateRegisterOperand(instr, 0, result, a.Value * b.Value);
      else
        updateRegisterOperand(instr, 0, result, BOTTOM);
      break;
    }
    case WDIV:
    {
      // Ops: OpD = OpA / OpB
      assert(ops.size() == 3);
      WhileConstantValue a = readDataOperand(instr, 1, input);
      WhileConstantValue b = readDataOperand(instr, 2, input);

      if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        updateRegisterOperand(instr, 0, result, a.Value / b.Value);
      else
        updateRegisterOperand(instr, 0, result, BOTTOM);
      break;
    }
    case WEQUAL:
    {
      // Ops: OpD = OpA == OpB
      assert(ops.size() == 3);
      WhileConstantValue a = readDataOperand(instr, 1, input);
      WhileConstantValue b = readDataOperand(instr, 2, input);

      if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        updateRegisterOperand(instr, 0, result, a.Value == b.Value);
      else
        updateRegisterOperand(instr, 0, result, BOTTOM);
      break;
    }
    case WUNEQUAL:
    {
      // Ops: OpD = OpA != OpB
      assert(ops.size() == 3);
      WhileConstantValue a = readDataOperand(instr, 1, input);
      WhileConstantValue b = readDataOperand(instr, 2, input);

      if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        updateRegisterOperand(instr, 0, result, a.Value != b.Value);
      else
        updateRegisterOperand(instr, 0, result, BOTTOM);
      break;
    }
    case WLESS:
    {
      // Ops: OpD = OpA < OpB
      assert(ops.size() == 3);
      WhileConstantValue a = readDataOperand(instr, 1, input);
      WhileConstantValue b = readDataOperand(instr, 2, input);

      if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        updateRegisterOperand(instr, 0, result, a.Value < b.Value);
      else
        updateRegisterOperand(instr, 0, result, BOTTOM);
      break;
    }
    case WLESSEQUAL:
    {
      // Ops: OpD = OpA <= OpB
      assert(ops.size() == 3);
      WhileConstantValue a = readDataOperand(instr, 1, input);
      WhileConstantValue b = readDataOperand(instr, 2, input);

      if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        updateRegisterOperand(instr, 0, result, a.Value <= b.Value);
      else
        updateRegisterOperand(instr, 0, result, BOTTOM);
      break;
    }
    };

    return result;
  }

  static WhileConstantValue join(const WhileConstantValue &a,
                                 const WhileConstantValue &b)
  {
    if (a.Kind == TOP)
      return b;
    else if (b.Kind == TOP)
      return a;
    else if (a == b)
      return a;
    else
      return BOTTOM;
  }

  WhileConstantDeadAnalysisDomain join(std::list<WhileConstantDeadAnalysisDomain> inputs) override
  {
    WhileConstantDeadAnalysisDomain result;
    for (WhileConstantDeadAnalysisDomain r : inputs)
    {
      for (const auto &[idx, value] : r.originalConstantDomain)
      {
        WhileConstantValue &resultvalue = result.originalConstantDomain[idx];
        resultvalue = join(resultvalue, value);
      }
      if (r.currentBlockReachability == REACHABLE || r.takenBranchReachbility == REACHABLE)
        result.currentBlockReachability = REACHABLE;
      // else
      //   result.currentBlockReachability = DEAD;
    }

    return result;
  }

  WhileConstantDeadAnalysisDomain join(const WhileBlock *bb) override
  {
    std::list<WhileConstantDeadAnalysisDomain> bbIn;
    for (const auto &[pred, kind] : bb->Pred)
    {
      if ((kind == WBRANCH_TAKEN && BBOut[pred].takenBranchReachbility == REACHABLE) ||
          (kind == WFALL_THROUGH && BBOut[pred].currentBlockReachability == REACHABLE))
        bbIn.emplace_back(BBOut[pred]);
    }

    WhileConstantDeadAnalysisDomain result = join(bbIn);
    if (bb->isEntry())
    {
      result.currentBlockReachability = REACHABLE;
      result.takenBranchReachbility = DEAD;
    }

    return result;
  }
};

struct WhileConstantDeadAnalysis : public WhileAnalysis
{
  void analyze(const WhileProgram &p) override
  {
    WhileConstantDead WCDA;
    WCDA.analyze(p);
    WCDA.dump(std::cout, p);
  };

  WhileConstantDeadAnalysis() : WhileAnalysis("WCDA",
                                              "Constant Dead Analysis")
  {
  }
};

WhileConstantDeadAnalysis WCDA;
