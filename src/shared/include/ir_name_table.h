#ifndef IR_NAME_TABLE_H_
#define IR_NAME_TABLE_H_

#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "ir_def.h"

typedef struct IRNameTable IRNameTable;

IRNameTable* IRNameTable_Ctor();
void IRNameTable_Dtor(IRNameTable* name_table);

void IRNameTable_Dump(IRNameTable* name_table);

// inserted successfuly?
bool IRNameTable_Insert(IRNameTable* name_table,
                        const char* str,
                        size_t len,
                        int64_t name_id,
                        IROperandType name_type,
                        int64_t n_param);
// is found?
bool IRNameTable_LookUpByStr(IRNameTable* name_table,
                             const char* str,
                             size_t len,
                             IRName* ir_name_out);
// must be used with caution because pointer invalidation
// is found?
bool IRNameTable_LookUpByValue(IRNameTable* name_table,
                               int64_t name_id,
                               IRName* ir_name_out);

#endif // IR_NAME_TABLE_H_
