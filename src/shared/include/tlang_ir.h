#ifndef TLANG_IR_H_
#define TLANG_IR_H_

#include <stdint.h>
#include <stdio.h>

#include "list.h"
#include "ir_def.h"
#include "ir_name_table.h"

typedef struct IRNode {
  int64_t       dest_id;
  IROperandType dest_type;
  IROperator ir_operator;
  int64_t       src1_id;
  IROperandType src1_type;
  int64_t       src2_id;
  IROperandType src2_type;
} IRNode;

typedef struct IRBlock {
  List* ir_nodes;
  IRNameTable* local_nt;
  int64_t last_local_id;
  int64_t number_of_local_vars;
} IRBlock;

typedef struct IR {
  List* ir_blocks;
  IRNameTable* global_nt;
  int64_t last_global_id;
  // ir dump
  bool do_dump;
  const char* dump_filename;
} IR;

// inits IR, IRBlocks should be inited separetly
IR* IR_Ctor(bool do_dump, const char* dump_filename);
void IR_Dump(IR* ir);
void IR_Out(IR* ir);

#endif // TLANG_IR_H_