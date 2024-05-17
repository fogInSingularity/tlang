#include "tlang_ir.h"

#include <stdlib.h>

#include "my_assert.h"
#include "debug.h"
#include "file_wraper.h"
#include "ir_def_to_str.h"

// macro -----------------------------------------------------------------------

#define dump_print(...) fprintf(stderr, __VA_ARGS__);
#define kMaxBufSize 30

// static ----------------------------------------------------------------------

static char buf_for_gen_strs[kMaxBufSize] = {0};

static void IRBlock_Dump(IRBlock* ir_block);
static void IRNode_Dump(IRNode* ir_node);
void IRBlock_Out(IRBlock* ir_block, IR* ir, FILE* out_file);
void IRNode_Out(IRNode* ir_node, IRBlock* ir_block, IR* ir, FILE* out_file);

static const char* IdToStr(int64_t ir_id,
                           IROperandType ir_type,
                           IRNameTable* ir_nt,
                           IRNameTable* global_nt);

// global ----------------------------------------------------------------------

IR* IR_Ctor(bool do_dump, const char* dump_filename) {
  IR* new_ir = calloc(1, sizeof(IR));
  if (new_ir == NULL) { return NULL; }

  new_ir->ir_blocks = List_Ctor(sizeof(IRBlock));
  if (new_ir->ir_blocks == NULL) { free(new_ir); return NULL; }

  new_ir->global_nt = IRNameTable_Ctor();
  if (new_ir->global_nt == NULL) {
    List_Dtor(new_ir->ir_blocks);
    free(new_ir);
  }

  new_ir->last_global_id = 1;
  new_ir->do_dump = do_dump;
  new_ir->dump_filename = dump_filename;

  return new_ir;
}

void IR_Dump(IR* ir) {
  ASSERT(ir != NULL);

#if !defined (DEBUG)
  return ;
#endif // DEBUG

  dump_print("# IR [ %p ]\n", ir);
  dump_print("# global_nt [ %p ]\n", ir->global_nt)
  // dump_print("# {\n");

  ListNode* iter_node = List_FirstNode(ir->ir_blocks);
  while (iter_node != NULL) {
    IRBlock* ir_block = List_AccessData(iter_node);
    IRBlock_Dump(ir_block);
    iter_node = List_NextNode(ir->ir_blocks, iter_node);
  }

  // dump_print("# }\n");
}

void IR_Out(IR* ir) {
  ASSERT(ir != NULL);

  if (!ir->do_dump) { return ; }

  FILE* out_file = FOPENW(ir->dump_filename, "w");

  ListNode* iter_node = List_FirstNode(ir->ir_blocks);
  while (iter_node != NULL) {
    IRBlock* ir_block = List_AccessData(iter_node);
    IRBlock_Out(ir_block, ir, out_file);
    iter_node = List_NextNode(ir->ir_blocks, iter_node);
  }

  FCLOSEW(out_file);
}

// static ----------------------------------------------------------------------

static void IRBlock_Dump(IRBlock* ir_block) {
  ASSERT(ir_block != NULL);

  dump_print("#   IRBlock [ %p ]\n", ir_block);
  dump_print("#   local_nt [ %p ]\n", ir_block->local_nt);
  // dump_print("#   {\n")

  ListNode* iter_node = List_FirstNode(ir_block->ir_nodes);
  while (iter_node != NULL) {
    IRNode* ir_node = List_AccessData(iter_node);
    IRNode_Dump(ir_node);
    iter_node = List_NextNode(ir_block->ir_nodes, iter_node);
  }

  // dump_print("#   }\n")
}

static void IRNode_Dump(IRNode* ir_node) {
  ASSERT(ir_node != NULL);

  dump_print("#     IRNode [ %p ]\n", ir_node);
  // dump_print("#     {\n")
  dump_print("#       dest_id [ %ld ][ %s ]\n",
             ir_node->dest_id,
             IROperandTypeToStr(ir_node->dest_type));
  dump_print("#       ir_operator [ %s ]\n",
             IROperatorToStr(ir_node->ir_operator));
  dump_print("#       src1_id [ %ld ][ %s ]\n",
             ir_node->src1_id,
             IROperandTypeToStr(ir_node->src1_type));
  dump_print("#       src2_id [ %ld ][ %s ]\n",
             ir_node->src2_id,
             IROperandTypeToStr(ir_node->src2_type));
  // dump_print("#     }\n")
}

void IRBlock_Out(IRBlock* ir_block, IR* ir, FILE* out_file) {
  ASSERT(ir_block != NULL);
  ASSERT(out_file != NULL);

  ListNode* iter_node = List_FirstNode(ir_block->ir_nodes);
  while (iter_node != NULL) {
    IRNode* ir_node = List_AccessData(iter_node);
    IRNode_Out(ir_node, ir_block, ir, out_file);
    iter_node = List_NextNode(ir_block->ir_nodes, iter_node);
  }
}

//FIXME
void IRNode_Out(IRNode* ir_node, IRBlock* ir_block, IR* ir, FILE* out_file) {
  ASSERT(ir_node != NULL);
  ASSERT(out_file != NULL);

  const char* dest_name = IdToStr(ir_node->dest_id, ir_node->dest_type, ir_block->local_nt, ir->global_nt);
  const char* src1_name = IdToStr(ir_node->src1_id, ir_node->src1_type, ir_block->local_nt, ir->global_nt);
  const char* src2_name = IdToStr(ir_node->src2_id, ir_node->src2_type, ir_block->local_nt, ir->global_nt);

  fprintf(out_file,
          "%s %s %s %s\n",
          dest_name,
          IROperatorToStr(ir_node->ir_operator),
          src1_name,
          src2_name);
}

static const char* IdToStr(int64_t ir_id,
                           IROperandType ir_type,
                           IRNameTable* ir_nt,
                           IRNameTable* global_nt) {
  ASSERT(ir_nt != NULL);
  ASSERT(global_nt != NULL);

  switch (ir_type) {
    case kIROperandType_Uninit:
      return "WARNING, might have bug! Uninit";
    case kIROperandType_Const:
      snprintf(buf_for_gen_strs, kMaxBufSize, "%ld", ir_id);
      return buf_for_gen_strs;
    case kIROperandType_Variable: {
      IRName lookup_tmp = {};
      bool is_found = IRNameTable_LookUpByValue(ir_nt, ir_id, &lookup_tmp);
      if (!is_found) { ASSERT(0 && ":("); }

      return lookup_tmp.name_str;
    }
    case kIROperandType_Function: {
      IRName lookup_tmp = {};
      bool is_found = IRNameTable_LookUpByValue(global_nt, ir_id, &lookup_tmp);
      if (!is_found) { ASSERT(0 && ":("); }

      return lookup_tmp.name_str;
    }
    case kIROperandType_Global: {
      IRName lookup_tmp = {};
      bool is_found = IRNameTable_LookUpByValue(global_nt, ir_id, &lookup_tmp);
      if (!is_found) { ASSERT(0 && ":("); }

      return lookup_tmp.name_str;
    }
    case kIROperandType_ZeroInit:
      return ""; //NOTE may be '_'
    case kIROperandType_NParam:
      snprintf(buf_for_gen_strs, kMaxBufSize, "%ld", ir_id);
      return buf_for_gen_strs;
    case kIROperandType_LabelName:
      IRName lookup_tmp = {};
      bool is_found = IRNameTable_LookUpByValue(ir_nt, ir_id, &lookup_tmp);
      if (!is_found) { ASSERT(0 && ":("); }

      return lookup_tmp.name_str;
    default:
      ASSERT(0 && ":(");
      return "Unknown type";
  }
}

#undef dump_print
#undef kMaxBufSize