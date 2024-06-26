#include "x86_64_backend.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "my_assert.h"
#include "debug.h"
#include "backend.h"
#include "file_wraper.h"
#include "special_names.h"

// types and def ---------------------------------------------------------------

typedef struct AsmNode {
  size_t asm_len;
  char* asm_str;
} AsmNode;

// static const char* KDefaultStr     = "; THIS ASM FILE GENERATED BY TLANG COMPILER";
static const char* kIO64LibIncl    = "%include \"src/x86_64_backend/asm/io64lib.asm\"";

static const char* kTmpRegister    = "rax";
static const char* kTmpRegisterLow = "al";
static const char* kStackRegister  = "rsp";
static const char* kBaseRegister   = "rbp";
static const int   kTmpSizeInBytes = sizeof(int64_t);
static const char* kReturnReg      = "rax";

#define kMaxCharBufSize 128
#define kMax8bitBufSize 128

// static ----------------------------------------------------------------------

static ListNode* AssignAsmStr(ListNode* asm_lnode,
                              const char* asm_str,
                              size_t asm_len);
static void TranslateIRToAsm(IR* ir, List* asm_list);
static void TranslateIRBlockToAsm(IR* ir,
                                  IRBlock* ir_block,
                                  List* asm_list);
static void DumpAsm(List* asm_list, const char* dump_filename);
static void DtorAsmList(List* asm_list);

// global ----------------------------------------------------------------------

BackendError x86_64_FromIRToTarget(Backend* backend, IR* ir) {
  ASSERT(backend != NULL);
  ASSERT(ir != NULL);

  PRINT_POINTER(backend);
  PRINT_POINTER(ir);

  List* asm_list = List_Ctor(sizeof(AsmNode));
  ASSERT(asm_list != NULL);
  TranslateIRToAsm(ir, asm_list);

  if (backend->do_dump) {
    DumpAsm(asm_list, backend->asm_out_filename);
  }

  DtorAsmList(asm_list);

  return kBackendError_Success;
}

// static ----------------------------------------------------------------------

static void TranslateIRToAsm(IR* ir, List* asm_list) {
  ASSERT(ir != NULL);
  ASSERT(asm_list != NULL);

  ListNode* iter_asm = List_CtorNodeAtStart(asm_list);
  AssignAsmStr(iter_asm, kIO64LibIncl, strlen(kIO64LibIncl));

  ListNode* iter_blocks = List_FirstNode(ir->ir_blocks);
  while (iter_blocks != NULL) {
    IRBlock* ir_block = List_AccessData(iter_blocks);

    TranslateIRBlockToAsm(ir, ir_block, asm_list);
    iter_blocks = List_NextNode(ir->ir_blocks, iter_blocks);
  }
}

static void DumpAsm(List* asm_list, const char* dump_filename) {
  ASSERT(asm_list != NULL);
  ASSERT(dump_filename != NULL);

  FILE* dump_file = F_OPEN_W(dump_filename, "w");
  ASSERT(dump_file != NULL);

  ListNode* iter_asm = List_FirstNode(asm_list);
  while (iter_asm != NULL) {
    AsmNode* asm_nd = List_AccessData(iter_asm);
    fprintf(dump_file, "%s\n", asm_nd->asm_str);

    iter_asm = List_NextNode(asm_list, iter_asm);
  }

  F_CLOSE_W(dump_file);
}

static void TranslateIRBlockToAsm(IR* ir,
                                  IRBlock* ir_block,
                                  List* asm_list) {
  ASSERT(ir != NULL);
  ASSERT(ir_block != NULL);
  ASSERT(asm_list != NULL);

  ListNode* iter_ir = List_FirstNode(ir_block->ir_nodes);
  ListNode* iter_asm = List_LastNode(asm_list);
  ASSERT(iter_asm != NULL);

  char buf_for_str_gen[kMaxCharBufSize] = {0};
  static int func_call_param_counter;

#define PUSH_ASM_STR(...) \
  do { \
    snprintf(buf_for_str_gen, kMaxCharBufSize, __VA_ARGS__); \
    size_t gen_len = strlen(buf_for_str_gen); \
    iter_asm = List_CtorNodeAfter(asm_list, iter_asm); \
    AssignAsmStr(iter_asm, buf_for_str_gen, gen_len); \
  } while (0)
// PUSH_ASM_STR

  IRName func_ir_name = {};

  while (iter_ir != NULL) {
    IRNode* ir_nd = List_AccessData(iter_ir);
    switch (ir_nd->ir_operator) {
      case kIROperator_Func: {
        bool is_found = IRNameTable_LookUpByValue(ir->global_nt,
                                                  ir_nd->dest_id,
                                                  &func_ir_name);
        if (!is_found) { ASSERT(0 && ":("); }
        bool is_main = strcmp(func_ir_name.name_str, "Main") == 0;
        if (is_main) {
          PUSH_ASM_STR("%s", kStart);
        }

        PUSH_ASM_STR("%s: ; n_param=%ld, n_locals=%ld",
                     func_ir_name.name_str,
                     func_ir_name.n_param,
                     ir_block->number_of_local_vars);
        PUSH_ASM_STR("\tpush %s\t; function enter(1)", kBaseRegister);
        PUSH_ASM_STR("\tmov %s, %s\t; function enter(2)",
                     kBaseRegister,
                     kStackRegister);
        PUSH_ASM_STR("\tsub %s, %ld\t; function enter(3)",
                     kStackRegister,
                     kTmpSizeInBytes * (ir_block->number_of_local_vars + 1));
      }
      break;
      case kIROperator_Tmp: {
        PUSH_ASM_STR("\t; tmp%ld alloc", ir_nd->dest_id);
      }
      break;
      case kIROperator_Label: {
        IRName lbl_ir_name = {};
        bool is_found = IRNameTable_LookUpByValue(ir->global_nt,
                                                  ir_nd->dest_id,
                                                  &lbl_ir_name);
        if (!is_found) { ASSERT(0 && ":("); }

        PUSH_ASM_STR(".%s_%s:", lbl_ir_name.name_str, func_ir_name.name_str);
      }
      break;
      case kIROperator_Assign: {
        if (ir_nd->src1_type == kIROperandType_Variable) {
          PUSH_ASM_STR("\tmov %s, [%s - %ld]\t; assign %ld <- %ld(var)",
                       kTmpRegister,
                       kBaseRegister, ir_nd->src1_id * kTmpSizeInBytes,
                       ir_nd->dest_id,
                       ir_nd->src1_id);
          PUSH_ASM_STR("\tmov [%s - %ld], %s\t; assign %ld <- %ld(var)",
                       kBaseRegister,
                       ir_nd->dest_id * kTmpSizeInBytes,
                       kTmpRegister,
                       ir_nd->dest_id,
                       ir_nd->src1_id);
        } else {
          PUSH_ASM_STR("\tmov qword [%s - %ld], %ld\t; assign %ld <- %ld(const)",
                       kBaseRegister, ir_nd->dest_id * kTmpSizeInBytes,
                       ir_nd->src1_id,
                       ir_nd->dest_id,
                       ir_nd->src1_id);
        }
      }
      break;

#define PUSH_OPERATION(op_name) \
  if (ir_nd->src1_type == kIROperandType_Variable) { \
    PUSH_ASM_STR("\tmov %s, [%s - %ld]\t; " #op_name " var(1): %ld <- %ld %ld",\
                 kTmpRegister, \
                 kBaseRegister, \
                 ir_nd->src1_id * kTmpSizeInBytes, \
                 ir_nd->dest_id, \
                 ir_nd->src1_id, \
                 ir_nd->src2_id); \
  } else { \
    PUSH_ASM_STR("\tmov %s, %ld\t; " #op_name " const(1): %ld <- %ld %ld", \
                 kTmpRegister, \
                 ir_nd->src1_id, \
                 ir_nd->dest_id, \
                 ir_nd->src1_id, \
                 ir_nd->src2_id); \
  } \
  if (ir_nd->src2_type == kIROperandType_Variable) { \
    PUSH_ASM_STR("\t" #op_name " %s, [%s - %ld]\t; " \
                 #op_name \
                 " var(2): %ld <- %ld %ld", \
                 kTmpRegister, \
                 kBaseRegister, \
                 ir_nd->src2_id * kTmpSizeInBytes, \
                 ir_nd->dest_id, \
                 ir_nd->src1_id, \
                 ir_nd->src2_id); \
  } else { \
    PUSH_ASM_STR("\t" \
                 #op_name \
                 " %s, %ld\t; " \
                 #op_name \
                 " const(2): %ld <- %ld %ld", \
                 kTmpRegister, \
                 ir_nd->src2_id, \
                 ir_nd->dest_id, \
                 ir_nd->src1_id, \
                 ir_nd->src2_id); \
  } \
  PUSH_ASM_STR("\tmov [%s - %ld], %s\t; " \
               #op_name \
               "(3): %ld <- %ld %ld", \
               kBaseRegister, \
               ir_nd->dest_id * kTmpSizeInBytes, \
               kTmpRegister, \
               ir_nd->dest_id, \
               ir_nd->src1_id, \
               ir_nd->src2_id);
// PUSH_OPERATION

      case kIROperator_Add: {
        PUSH_OPERATION(add);
      }
      break;
      case kIROperator_Sub: {
        PUSH_OPERATION(sub);
      }
      break;
      case kIROperator_Mul: {
        PUSH_OPERATION(imul);
      }
      break;
      case kIROperator_Div: { ASSERT(0); } //FIXME
      break;
      case kIROperator_And: {
        PUSH_OPERATION(and);
      }
      break;
      case kIROperator_Or: {
        PUSH_OPERATION(or);
      }
      break;

#define PUSH_CMP(op_name, set_name) \
        if (ir_nd->src1_type == kIROperandType_Variable) { \
          PUSH_ASM_STR("\tmov %s, [%s - %ld]\t; " \
                       #op_name \
                       " var(1): %ld <- %ld %ld", \
                       kTmpRegister, \
                       kBaseRegister, \
                       ir_nd->src1_id * kTmpSizeInBytes, \
                       ir_nd->dest_id, \
                       ir_nd->src1_id, \
                       ir_nd->src2_id); \
        } else { \
          PUSH_ASM_STR("\tmov %s, %ld\t; " \
                       #op_name \
                       " const(1): %ld <- %ld %ld", \
                       kTmpRegister, \
                       ir_nd->src1_id, \
                       ir_nd->dest_id, \
                       ir_nd->src1_id, \
                       ir_nd->src2_id); \
        } \
        if (ir_nd->src2_type == kIROperandType_Variable) { \
          PUSH_ASM_STR("\tcmp %s, [%s - %ld]\t; " \
                       #op_name \
                       " var(2): %ld <- %ld %ld", \
                       kTmpRegister, \
                       kBaseRegister, \
                       ir_nd->src2_id * kTmpSizeInBytes, \
                       ir_nd->dest_id, \
                       ir_nd->src1_id, \
                       ir_nd->src2_id); \
        } else { \
          PUSH_ASM_STR("\tcmp %s, %ld\t; " \
                       #op_name \
                       " const(2): %ld <- %ld %ld", \
                       kTmpRegister, \
                       ir_nd->src2_id, \
                       ir_nd->dest_id, \
                       ir_nd->src1_id, \
                       ir_nd->src2_id); \
        } \
        PUSH_ASM_STR("\tmov %s, 0\t\t; " \
                     #op_name \
                     "(3): %ld <- %ld %ld", \
                     kTmpRegister, \
                     ir_nd->dest_id, \
                     ir_nd->src1_id, \
                     ir_nd->src2_id); \
        PUSH_ASM_STR("\t" \
                     #set_name \
                     " %s\t\t\t; " \
                     #op_name \
                     "(4): %ld <- %ld %ld", \
                     kTmpRegisterLow, \
                     ir_nd->dest_id, \
                     ir_nd->src1_id, \
                     ir_nd->src2_id); \
        PUSH_ASM_STR("\tmov [%s - %ld], %s\t; " \
                     #op_name \
                     "", \
                     kBaseRegister, \
                     ir_nd->dest_id * kTmpSizeInBytes, \
                     kTmpRegister);
// PUSH_CMP

      case kIROperator_Equal: {
        PUSH_CMP(equal, sete);
      }
      break;
      case kIROperator_NotEqual: {
        PUSH_CMP(not_equal, setne);
      }
      break;
      case kIROperator_Not: {
        PUSH_ASM_STR("\tmov %s, [%s - %ld]\t; not %ld <- %ld",
                     kTmpRegister,
                     kBaseRegister,
                     ir_nd->src1_id * kTmpSizeInBytes,
                     ir_nd->dest_id,
                     ir_nd->src1_id);
        PUSH_ASM_STR("\txor %s, 1\t\t; not %ld <- %ld",
                     kTmpRegister,
                     ir_nd->dest_id,
                     ir_nd->src1_id);
        PUSH_ASM_STR("\tmov [%s - %ld], %s\t; not %ld <- %ld",
                     kBaseRegister,
                     ir_nd->dest_id * kTmpSizeInBytes,
                     kTmpRegister,
                     ir_nd->dest_id,
                     ir_nd->src1_id);
      }
      break;
      case kIROperator_JumpIf: {
        PUSH_ASM_STR("mov %s, [%s - %ld]\t; jump_if(1)",
                     kTmpRegister,
                     kBaseRegister,
                     ir_nd->src1_id * kTmpSizeInBytes);
        PUSH_ASM_STR("test %s, %s", kTmpRegister, kTmpRegister);

        IRName lbl_ir_name = {};
        bool is_found = IRNameTable_LookUpByValue(ir->global_nt,
                                                  ir_nd->dest_id,
                                                  &lbl_ir_name);
        if (!is_found) { ASSERT(0 && ":("); }

        PUSH_ASM_STR("jne .%s_%s", lbl_ir_name.name_str, func_ir_name.name_str);
      }
      break;
      case kIROperator_Jump: {
        IRName lbl_ir_name = {};
        bool is_found = IRNameTable_LookUpByValue(ir->global_nt,
                                                  ir_nd->dest_id,
                                                  &lbl_ir_name);
        if (!is_found) { ASSERT(0 && ":("); }

        PUSH_ASM_STR("jmp .%s_%s", lbl_ir_name.name_str, func_ir_name.name_str);
      }
      break;
      case kIROperator_Return: {
        PUSH_ASM_STR("\tmov %s, [%s - %ld]\t; return(1)",
                     kReturnReg,
                     kBaseRegister,
                     ir_nd->dest_id * kTmpSizeInBytes);
        PUSH_ASM_STR("\tmov %s, %s\t\t; return(2)",
                     kStackRegister,
                     kBaseRegister);
        PUSH_ASM_STR("\tpop %s\t; return(3)", kBaseRegister);
        PUSH_ASM_STR("ret\t\t\t; return(4)");
      }
      break;
      case kIROperator_CallStart: {
        PUSH_ASM_STR("\t; call start");
        func_call_param_counter = 0;
      }
      break;
      case kIROperator_Parameter: {
        PUSH_ASM_STR("\tmov %s, [%s - %ld]\t; param <- %ld",
                     kTmpRegister,
                     kBaseRegister,
                     ir_nd->src1_id * kTmpSizeInBytes,
                     ir_nd->src1_id);
        PUSH_ASM_STR("\tmov [%s - %d], %s\t; param <- %ld",
                     kStackRegister,
                     (3 + func_call_param_counter) * kTmpSizeInBytes,
                     kTmpRegister,
                     ir_nd->src1_id);

        func_call_param_counter++;
      }
      break;
      case kIROperator_CallEnd: {
        IRName func_call_name = {};
        bool is_found = IRNameTable_LookUpByValue(ir->global_nt,
                                                  ir_nd->src1_id,
                                                  &func_call_name);
        if (!is_found) { ASSERT(0 && ":("); }

        PUSH_ASM_STR("\tcall %s\t; function call", func_call_name.name_str);
        PUSH_ASM_STR("\tmov [%s - %ld], %s\t; return value assignment %ld <- func call",
                     kBaseRegister,
                     ir_nd->dest_id * kTmpSizeInBytes,
                     kReturnReg,
                     ir_nd->dest_id);
        PUSH_ASM_STR("\t;call end");
      }
      break;
      default:
        PRINT_UINT(ir_nd->ir_operator);
    }

    iter_ir = List_NextNode(ir_block->ir_nodes, iter_ir);
  }

  PUSH_ASM_STR("\n");

  // return iter_asm;
}

static ListNode* AssignAsmStr(ListNode* asm_lnode,
                              const char* asm_str,
                              size_t asm_len) {
  ASSERT(asm_lnode != NULL);
  ASSERT(asm_str != NULL);

  AsmNode* asm_nd = List_AccessData(asm_lnode);

  asm_nd->asm_len = 0;

  asm_nd->asm_str = calloc(1, asm_len + 1);
  ASSERT(asm_nd->asm_str != NULL);
  asm_nd->asm_len = asm_len;

  memcpy(asm_nd->asm_str, asm_str, asm_len);
  asm_nd->asm_str[asm_len] = '\0';

  return asm_lnode;
}

static void DtorAsmList(List* asm_list) {
  ASSERT(asm_list != NULL);

  ListNode* iter_asm = List_FirstNode(asm_list);
  while (iter_asm != NULL) {
    AsmNode* asm_nd = List_AccessData(iter_asm);

    free(asm_nd->asm_str);

    iter_asm = List_NextNode(asm_list, iter_asm);
  }

  List_DtorFull(asm_list);
}