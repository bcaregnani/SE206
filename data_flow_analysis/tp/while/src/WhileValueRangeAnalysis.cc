#include "WhileAnalysis.h"
#include "WhileLang.h"
#include "WhileCFG.h"
#include "WhileColor.h"
#include <tuple>
using namespace std;


enum WhileConstantKind
{
  TOP,
  BOTTOM,
  CONSTANT
};

struct WhileValueRange
{
  WhileConstantKind Kind;
  int Valmin;
  int Valmax;

  WhileValueRange() : Kind(TOP), Valmin(0), Valmax(0)
  {
  }

  WhileValueRange(int min, int max) : Kind(CONSTANT), Valmin(min), Valmax(max)
  {
  }

  WhileValueRange(WhileConstantKind kind) : Kind(kind), Valmin(0), Valmax(0)
  {
  }
};

bool operator==(const WhileValueRange &a, const WhileValueRange &b)
{
  return (a.Kind == b.Kind && a.Valmin == b.Valmin && a.Valmax == b.Valmax);
}

typedef std::map<int, WhileValueRange> WhileValueRangeDomain;


std::ostream &operator<<(std::ostream &s, const WhileValueRange &v)
{
  switch (v.Kind)
  {
    case TOP:
      return s << FLIGHT_GRAY << "⊤" << "," << "⊤" << CRESET;
    case BOTTOM:
      return s << FRED << "⊥" << "," << "⊥" << CRESET;
    case CONSTANT:
      return s << FGREEN << v.Valmin << "," << v.Valmax << CRESET;
  };
  abort();
}




struct WhileConstantRange : public WhileDataFlowAnalysis<WhileValueRangeDomain>
{
  std::ostream &dump_first(std::ostream &s,
                           const WhileValueRangeDomain &value) override
  {
    s << "    [";
    bool first = true;
    // std::cout << value << std::endl;
    for(const auto&[idx, c] : value)
    {
      if (!first)
        s << ", ";

      s << "R" << idx << "={" << c << "}" ;
      first = false;
    }
    return s << "]\n";
  }

  

  std::ostream &dump_pre(std::ostream &s,
                         const WhileValueRangeDomain &value) override
  {
    return s;
  }

  std::ostream &dump_post(std::ostream &s,
                          const WhileValueRangeDomain &value) override
  {
    return dump_first(s, value);
  }

  static void updateRegisterOperand(const WhileInstr &instr, unsigned int idx,
                             WhileValueRangeDomain &result,
                             WhileValueRange value)
  {
    const WhileOperand &op = instr.Ops[idx];
    switch (op.Kind)
    {
      case WREGISTER:
        assert(op.ValueOrIndex >= 0);
        result[op.ValueOrIndex] = value;
        result[op.ValueOrIndex].Valmin = value.Valmin;
        result[op.ValueOrIndex].Valmax = value.Valmax;
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


  

  static WhileValueRange readDataOperand(const WhileInstr &instr,
                                         unsigned int idx,
                                         const WhileValueRangeDomain &input)
  {
    WhileValueRange result;
    const WhileOperand &op = instr.Ops[idx];
    switch (op.Kind)
    {
      case WREGISTER:
      {
        assert(op.ValueOrIndex >= 0);
        auto value = input.find(op.ValueOrIndex);
        if (value == input.end())
        {
          return BOTTOM; // register undefined
        }
        else
        {
          result.Kind = value->second.Kind;
          result.Valmin = value->second.Valmin;
          result.Valmax = value->second.Valmax;
          return result;
        }
        abort();
      }
      case WIMMEDIATE:
        result.Kind = CONSTANT;
        result.Valmin = op.ValueOrIndex;
        result.Valmax = op.ValueOrIndex;
        return result;

      case WFRAMEPOINTER:
          return BOTTOM;
          
      case WBLOCK:
      case WFUNCTION:
      case WUNKNOWN:
        assert("Operand is not a data value.");
    }
    abort();
  }

  

  WhileValueRangeDomain transfer(const WhileInstr &instr, const WhileValueRangeDomain input) override
  {
    WhileValueRangeDomain result = input;
    WhileValueRange aux;
    aux.Kind = CONSTANT;
    const auto &ops = instr.Ops;
    switch(instr.Opc)
    {
      case WBRANCHZ:
      case WBRANCH:
      case WRETURN:
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
        assert(ops.size() ==  3);
        updateRegisterOperand(instr, 0, result, BOTTOM);
        break;
      }

      case WPLUS:
      {
        // Ops: OpD = OpA + OpB
        assert(ops.size() ==  3);
        WhileValueRange a = readDataOperand(instr, 1, input);
        WhileValueRange b = readDataOperand(instr, 2, input);

        if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        {
          aux.Valmin = a.Valmin + b.Valmin;
          aux.Valmax = a.Valmax + b.Valmax;
          updateRegisterOperand(instr, 0, result, aux);
        }
        else
        {
          updateRegisterOperand(instr, 0, result, BOTTOM);
        }
        break;
      }
      case WMINUS:
      {
        // Ops: OpD = OpA - OpB
        assert(ops.size() ==  3);
        WhileValueRange a = readDataOperand(instr, 1, input);
        WhileValueRange b = readDataOperand(instr, 2, input);

        if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        {
          aux.Valmin = a.Valmin - b.Valmin;
          aux.Valmax = a.Valmax - b.Valmax;
          updateRegisterOperand(instr, 0, result, aux);
        }
        else
          updateRegisterOperand(instr, 0, result, BOTTOM);
        break;
      }
      case WMULT:
      {
        // Ops: OpD = OpA * OpB
        assert(ops.size() ==  3);
        WhileValueRange a = readDataOperand(instr, 1, input);
        WhileValueRange b = readDataOperand(instr, 2, input);

        if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        {
          aux.Valmin = a.Valmin * b.Valmin;
          aux.Valmax = a.Valmax * b.Valmax;
          updateRegisterOperand(instr, 0, result, aux);
        }
        else
          updateRegisterOperand(instr, 0, result, BOTTOM);
        break;
      }
      case WDIV:
      {
        // Ops: OpD = OpA / OpB
        assert(ops.size() ==  3);
        WhileValueRange a = readDataOperand(instr, 1, input);
        WhileValueRange b = readDataOperand(instr, 2, input);

        if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        {
          aux.Valmin = a.Valmin / b.Valmin;
          aux.Valmax = a.Valmax / b.Valmax;
          updateRegisterOperand(instr, 0, result, aux);
        }
        else
          updateRegisterOperand(instr, 0, result, BOTTOM);
        break;
      }
      case WEQUAL:
      {
        // Ops: OpD = OpA == OpB
        assert(ops.size() ==  3);
        WhileValueRange a = readDataOperand(instr, 1, input);
        WhileValueRange b = readDataOperand(instr, 2, input);

        if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        {
          if (a.Valmax < b.Valmin || b.Valmax < a.Valmin)
          { // Case of disjoint ranges
            aux.Valmin = 0;
            aux.Valmax = 0;
          } else if ( (a.Valmin == a.Valmax) && (b.Valmin == b.Valmax) && (a.Valmin == b.Valmax) )
          { // Case of range is 1 number and it is the same number for both registers
            aux.Valmin = 1;
            aux.Valmax = 1;
          } else
          { // Every other case we do not know, it could or could be not equal
            aux.Valmin = 0;
            aux.Valmax = 1;
          };
          updateRegisterOperand(instr, 0, result, aux);
        }
        else
          updateRegisterOperand(instr, 0, result, BOTTOM);
        break;
      }
      case WUNEQUAL:
      {
        // Ops: OpD = OpA != OpB
        assert(ops.size() ==  3);
        WhileValueRange a = readDataOperand(instr, 1, input);
        WhileValueRange b = readDataOperand(instr, 2, input);
        if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        {
          if (a.Valmax < b.Valmin || b.Valmax < a.Valmin)
          { // Case of disjoint ranges
            aux.Valmin = 1;
            aux.Valmax = 1;
          } else if ( (a.Valmin == a.Valmax) && (b.Valmin == b.Valmax) && (a.Valmin == b.Valmax) )
          { // Case of range is 1 number and it is the same number for both registers
            aux.Valmin = 0;
            aux.Valmax = 0;
          } else
          { // Every other case we do not know, it could or could be not equal
            aux.Valmin = 0;
            aux.Valmax = 1;
          };
          updateRegisterOperand(instr, 0, result, aux);
        }
        else
        {
          updateRegisterOperand(instr, 0, result, BOTTOM);
        }
        break;
      }
      case WLESS:
      {
        // Ops: OpD = OpA < OpB
        assert(ops.size() ==  3);
        WhileValueRange a = readDataOperand(instr, 1, input);
        WhileValueRange b = readDataOperand(instr, 2, input);

        if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        {
          if (a.Valmax < b.Valmin)
          { // Case of disjoint ranges
            aux.Valmin = 1;
            aux.Valmax = 1;
          } else if (a.Valmin > b.Valmax)
          { // Case of range is 1 number and it is the same number for both registers
            aux.Valmin = 0;
            aux.Valmax = 0;
          } else
          { // Every other case we do not know, it could or could be not equal
            aux.Valmin = 0;
            aux.Valmax = 1;
          };
          updateRegisterOperand(instr, 0, result, aux);
        }
        else
          updateRegisterOperand(instr, 0, result, BOTTOM);
        break;
      }
      case WLESSEQUAL:
      {
        // Ops: OpD = OpA <= OpB
        assert(ops.size() ==  3);
        WhileValueRange a = readDataOperand(instr, 1, input);
        WhileValueRange b = readDataOperand(instr, 2, input);

        if (a.Kind == CONSTANT && b.Kind == CONSTANT)
        {
          if (a.Valmax <= b.Valmin)
          { // Case of disjoint ranges
            aux.Valmin = 1;
            aux.Valmax = 1;
          } else if (a.Valmin > b.Valmax)
          { // Case of range is 1 number and it is the same number for both registers
            aux.Valmin = 0;
            aux.Valmax = 0;
          } else
          { // Every other case we do not know, it could or could be not equal
            aux.Valmin = 0;
            aux.Valmax = 1;
          };
          updateRegisterOperand(instr, 0, result, aux);
        }
        else
          updateRegisterOperand(instr, 0, result, BOTTOM);
        break;
      }
    };

    return result;
  }


  static WhileValueRange join(const WhileValueRange &a,
                              const WhileValueRange &b)
  {
    WhileValueRange result;
    result.Kind = CONSTANT;
    if (a.Kind == TOP)
      return b;
    else if (b.Kind == TOP)
      return a;
    else
    {    
      if (a.Valmin <= b.Valmin)
        result.Valmin = a.Valmin;
      else
        result.Valmin = b.Valmin;

      if (a.Valmax <= b.Valmax)
        result.Valmax = b.Valmax;
      else
        result.Valmax = a.Valmax;
    
      return result;
    }
  }

  


  WhileValueRangeDomain join(std::list<WhileValueRangeDomain> inputs) override
  {
    WhileValueRangeDomain result;
    for(WhileValueRangeDomain r : inputs)
    {
      for(const auto&[idx, value] : r)
      {
        WhileValueRange &resultvalue = result[idx];
        resultvalue = join(resultvalue, value);
      }
    }

    return result;
  }

  

};


struct WhileValueRangeAnalysis : public WhileAnalysis
{
  void analyze(const WhileProgram &p) override
  {
    WhileConstantRange WVRA;
    WVRA.analyze(p);
    WVRA.dump(std::cout, p);
  };

  WhileValueRangeAnalysis() : WhileAnalysis("WVRA",
                                                  "Value Range Analysis")
  {
  }
};

WhileValueRangeAnalysis WVRA;