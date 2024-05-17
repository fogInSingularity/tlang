#include "ir_def_to_str.h"

#include <stddef.h>

#include "my_assert.h"
#include "debug.h"
#include "ir_def.h"

const char* IROperatorToStr(IROperator ir_op) {
  #define DEF_IR_OPERATOR(op_name_, op_id_) \
    case op_id_: \
      return #op_name_;

  switch (ir_op) {
    #include "ir_operators.h"
    default:
      ASSERT(0 && ":(");
      return "Unknown operator";
  }
  return NULL;
}

const char* IROperandTypeToStr(IROperandType ir_type) {
  switch (ir_type) {
    case kIROperandType_Uninit:
      return "Uninit";
    case kIROperandType_Const:
      return "Const";
    case kIROperandType_Variable:
      return "Variable";
    case kIROperandType_Function:
      return "Function";
    case kIROperandType_Global:
      return "Global";
    case kIROperandType_ZeroInit:
      return "ZeroInit";
    case kIROperandType_NParam:
      return "NParam";
    case kIROperandType_LabelName:
      return "LabelName";
    default:
      ASSERT(0 && ":(");
      return "Unknown type";
  }
}