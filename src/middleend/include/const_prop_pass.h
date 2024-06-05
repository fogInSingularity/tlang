#ifndef CONST_PROP_PASS_H_
#define CONST_PROP_PASS_H_

#include <stdint.h>

#include "tlang_ir.h"

int64_t ConstPropPass(IR* ir);

#endif // CONST_PROP_PASS_H_