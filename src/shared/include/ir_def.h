#ifndef IR_DEF_H_
#define IR_DEF_H_

#include <stdint.h>
#include <stddef.h>

typedef enum IROperator {
  kIROperator_Uninit = 0,

#define DEF_IR_OPERATOR(enum_name_, enum_id_) \
  kIROperator_ ## enum_name_ = enum_id_,

#include "ir_operators.inc"
#undef DEF_IR_OPERATOR

} IROperator;

typedef enum IROperandType {
  kIROperandType_Uninit   = 0,

  kIROperandType_Const     = 1,
  kIROperandType_Variable  = 2,
  kIROperandType_Function  = 3,
  kIROperandType_Global    = 4,
  kIROperandType_ZeroInit  = 5,
  kIROperandType_NParam    = 6,
  kIROperandType_LabelName = 7,
} IROperandType;

typedef struct IRName {
  char* name_str;
  size_t len;

  int64_t name_id;
  IROperandType name_type;
  int64_t n_param;
} IRName;

#endif // IR_DEF_H_