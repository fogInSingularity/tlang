#include "ast_to_ir.h"

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

#include "my_assert.h"

// macro -----------------------------------------------------------------------

//NOTE
#define ASSIGN_IR_NODE(node_name, dest_id, dest_type, ir_operator, src1_id, src1_type, src2_id, src2_type) \
  node_name = (IRNode){.dest_id = dest_id, \
                       .dest_type = kIROperandType_ ## dest_type, \
                       .ir_operator = kIROperator_ ## ir_operator, \
                       .src1_id = src1_id, \
                       .src1_type = kIROperandType_ ## src1_type, \
                       .src2_id = src2_id, \
                       .src2_type = kIROperandType_ ## src2_type};

// static ----------------------------------------------------------------------

#define kMaxCharBufSize 20

static void TranslateFunctionToIrBlock(TreeNode* func_node,
                                       IR* ir);
static void TranslateFuncNameAndParam(TreeNode* func_node,
                                      IRBlock* func_ir_block,
                                      IRNameTable* global_nt,
                                      int64_t* global_id);
// static void TranslateFuncStatement(TreeNode* func_node,
//                                    IRBlock* func_ir_block);

static ListNode* TranslateStatement(TreeNode* st_tnode,
                                    IRBlock* func_ir_block,
                                    ListNode* last_ir_node,
                                    IRNameTable* global_nt);

static ListNode* TranslateSingleStatement(TreeNode* single_st_tnode,
                                          IRBlock* func_ir_block,
                                          ListNode* last_ir_node,
                                          IRNameTable* global_nt);

static ListNode* TranslateVarInit(TreeNode* single_st_tnode,
                                  IRBlock* func_ir_block,
                                  ListNode* last_ir_node,
                                  IRNameTable* global_nt);

static ListNode* TranslateAssignExpr(TreeNode* single_st_tnode,
                                     IRBlock* func_ir_block,
                                     ListNode* last_ir_node,
                                     IRNameTable* global_nt);

static ListNode* TranslateValueExpr(TreeNode* single_st_tnode,
                                    IRBlock* func_ir_block,
                                    ListNode* last_ir_node,
                                    IRNameTable* global_nt,
                                    int64_t* res_id_out);

static ListNode* TranslateReturnExpr(TreeNode* single_st_tnode,
                                     IRBlock* func_ir_block,
                                     ListNode* last_ir_node,
                                     IRNameTable* global_nt);

static ListNode* TranslateIfBranch(TreeNode* if_br_tnode,
                                   IRBlock* func_ir_block,
                                   ListNode* last_ir_node,
                                   IRNameTable* global_nt);

static ListNode* TranslateElseBranch(TreeNode* else_br_tnode,
                                     IRBlock* func_ir_block,
                                     ListNode* last_ir_node,
                                     IRNameTable* global_nt);

static ListNode* TranslateWhileLoop(TreeNode* while_loop_tnode,
                                    IRBlock* func_ir_block,
                                    ListNode* last_ir_node,
                                    IRNameTable* global_nt);

static ListNode* TranslateFunctionCall(TreeNode* func_call_tnode,
                                      IRBlock* func_ir_block,
                                      ListNode* last_ir_node,
                                      IRNameTable* global_nt,
                                      int64_t* res_id_out);

static ListNode* TranslateValueItself(TreeNode* value_tnode,
                                      IRBlock* func_ir_block,
                                      ListNode* last_ir_node,
                                      IRNameTable* global_nt,
                                      int64_t* res_id_out);

static int64_t GetNParam(TreeNode* func_node);
// return next alavailable local_id
static int64_t AddParamToLocalNT(TreeNode* func_node,
                                 IRNameTable* local_nt);
static ListNode* CreateTmpNode(IRBlock* func_ir_block,
                               ListNode* last_ir_node,
                               int64_t* res_id_out);
static void AddNameToLocalNT(IRNameTable* ir_name_table,
                             int64_t new_id,
                             const char* new_name,
                             IROperandType new_ir_op_type);
static IROperator OperatorToIR(Operator op);

// global ----------------------------------------------------------------------

IR* TranslateAstToIr(Tree* ast) {
  ASSERT(ast != NULL);

  IR* ir = IR_Ctor();
  if (ir == NULL) { return NULL; }

  TreeNode* iter_node = ast->root.r_child;
  while (iter_node != NULL) {
    TranslateFunctionToIrBlock(iter_node, ir);

    iter_node = iter_node->r_child;
  }

//NOTE
  IRNameTable_Dump(ir->global_nt);
  IR_Dump(ir);
  IR_Out(ir, stdout);

  return ir;
}

// static ----------------------------------------------------------------------

static void TranslateFunctionToIrBlock(TreeNode* func_node, IR* ir) {
  ASSERT(func_node != NULL);
  ASSERT(ir != NULL);

  ListNode* new_ir_block_node = List_CtorNodeAtEnd(ir->ir_blocks);
  if (new_ir_block_node == NULL) { $ return ; }

  IRBlock* new_ir_block = List_AccessData(new_ir_block_node);
  new_ir_block->ir_nodes = List_Ctor(sizeof(IRNode));
  if (new_ir_block->ir_nodes == NULL) { $ return ; }

  new_ir_block->local_nt = IRNameTable_Ctor();
  if (new_ir_block->local_nt == NULL) { $ return ; }

  TranslateFuncNameAndParam(func_node,
                            new_ir_block,
                            ir->global_nt,
                            &ir->last_global_id);

  ListNode* last_ir_node =  List_LastNode(new_ir_block->ir_nodes);
  TranslateStatement(func_node->l_child->r_child, new_ir_block, last_ir_node, ir->global_nt);

//NOTE
  IRNameTable_Dump(new_ir_block->local_nt);
}

static void TranslateFuncNameAndParam(TreeNode* func_node,
                                      IRBlock* func_ir_block,
                                      IRNameTable* global_nt,
                                      int64_t* last_global_id) {
  ASSERT(func_node != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(global_nt != NULL);
  ASSERT(last_global_id != NULL);

  TreeNode* func_name_tnode = func_node->l_child->l_child;
  int64_t func_id = (*last_global_id)++;

  List* ir_nodes_list = func_ir_block->ir_nodes;

  ListNode* func_name_lnode = List_CtorNodeAtStart(ir_nodes_list);
  IRNode* func_ir_node = List_AccessData(func_name_lnode);

  int64_t n_param = GetNParam(func_node);
  *func_ir_node = (IRNode){.dest_id = func_id,
                           .dest_type = kIROperandType_Function,
                           .ir_operator = kIROperator_Func,
                           .src1_id = n_param,
                           .src1_type = kIROperandType_NParam,
                           .src2_id = 0,
                           .src2_type = kIROperandType_ZeroInit};

  func_ir_block->last_local_id = AddParamToLocalNT(func_node,
                                                   func_ir_block->local_nt);

//FIXME might contain bug
  bool is_inserted = IRNameTable_Insert(global_nt,
                                        func_name_tnode->data.idnt.str,
                                        func_name_tnode->data.idnt.len,
                                        func_id,
                                        kIROperandType_Function,
                                        n_param);
  if (!is_inserted) { ASSERT(0 && ";("); }

  return ;
}

static ListNode* TranslateStatement(TreeNode* st_tnode,
                                    IRBlock* func_ir_block,
                                    ListNode* last_ir_node,
                                    IRNameTable* global_nt) {
  ASSERT(st_tnode != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);
  ASSERT(st_tnode->data.type == kTokenType_TreeSup
         && st_tnode->data.tree_sup == kTreeSup_Statement);

  TreeNode* iter_tnode = st_tnode->l_child;
  while (iter_tnode != NULL) {
    last_ir_node = TranslateSingleStatement(iter_tnode,
                                            func_ir_block,
                                            last_ir_node,
                                            global_nt);
    iter_tnode = iter_tnode->r_child;
  }

  return last_ir_node;
}

static ListNode* TranslateSingleStatement(TreeNode* single_st_tnode,
                                          IRBlock* func_ir_block,
                                          ListNode* last_ir_node,
                                          IRNameTable* global_nt) {
  ASSERT(single_st_tnode != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);

  ListNode* iter_node = NULL;

  switch (single_st_tnode->data.tree_sup) {
    case kTreeSup_VarInit:
      iter_node = TranslateVarInit(single_st_tnode,
                                   func_ir_block,
                                   last_ir_node,
                                   global_nt);
      break;
    case kTreeSup_AssignExpr:
      iter_node = TranslateAssignExpr(single_st_tnode,
                                      func_ir_block,
                                      last_ir_node,
                                      global_nt);
      break;
    case kTreeSup_ValueExpr:
      int64_t res_ir_node_unused = 0;
      iter_node = TranslateValueExpr(single_st_tnode,
                                     func_ir_block,
                                     last_ir_node,
                                     global_nt,
                                     &res_ir_node_unused);
      break;
    case kTreeSup_ReturnExpr:
      iter_node = TranslateReturnExpr(single_st_tnode,
                                      func_ir_block,
                                      last_ir_node,
                                      global_nt);
      break;
    case kTreeSup_IfBranch:
      iter_node = TranslateIfBranch(single_st_tnode,
                                     func_ir_block,
                                     last_ir_node,
                                     global_nt);
      break;
    case kTreeSup_WhileLoop:
      iter_node = TranslateWhileLoop(single_st_tnode,
                                     func_ir_block,
                                     last_ir_node,
                                     global_nt);
      break;
    case kTreeSup_Uninit:
    default:
      PRINT_UINT(single_st_tnode->data.tree_sup);
      ASSERT(0 && ":(");
      break;
  }

  return iter_node;
}

static ListNode* TranslateVarInit(TreeNode* single_st_tnode,
                                  IRBlock* func_ir_block,
                                  ListNode* last_ir_node,
                                  IRNameTable* global_nt) {
  ASSERT(single_st_tnode != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);

  int64_t res_id = 0;
  TreeNode* value_expr_tnode = single_st_tnode->l_child->r_child->r_child;
  last_ir_node = TranslateValueExpr(value_expr_tnode,
                                    func_ir_block,
                                    last_ir_node,
                                    global_nt,
                                    &res_id);

  TreeNode* var_name_tnode = single_st_tnode->l_child->l_child->l_child;
  int64_t var_id = func_ir_block->last_local_id;
  bool is_inserted = IRNameTable_Insert(func_ir_block->local_nt,
                     var_name_tnode->data.idnt.str,
                     var_name_tnode->data.idnt.len,
                     func_ir_block->last_local_id++,
                     kIROperandType_Variable,
                     0);
  if (!is_inserted) { ASSERT(0); }

  ListNode* new_ir_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes,
                                              last_ir_node);
  IRNode* new_ir_nd = List_AccessData(new_ir_lnode);
  *new_ir_nd = (IRNode){.dest_id = var_id,
                        .dest_type = kIROperandType_Variable,
                        .ir_operator = kIROperator_Assign,
                        .src1_id = res_id,
                        .src1_type = kIROperandType_Variable,
                        .src2_id = 0,
                        .src2_type = kIROperandType_ZeroInit};

  return new_ir_lnode;
}

static ListNode* TranslateAssignExpr(TreeNode* single_st_tnode,
                                     IRBlock* func_ir_block,
                                     ListNode* last_ir_node,
                                     IRNameTable* global_nt) {
  ASSERT(single_st_tnode != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);

  TreeNode* var_name = single_st_tnode->l_child;
  TreeNode* value_tnode = single_st_tnode->l_child->l_child->l_child;
  int64_t res_id_out = 0;
  last_ir_node = TranslateValueExpr(value_tnode,
                                    func_ir_block,
                                    last_ir_node,
                                    global_nt,
                                    &res_id_out);

  IRName lookup_tmp = {};
  bool is_found = IRNameTable_LookUpByStr(func_ir_block->local_nt,
                                          var_name->data.idnt.str,
                                          var_name->data.idnt.len,
                                          &lookup_tmp);
  if (!is_found) { ASSERT(0 && ":("); } //FIXME
  int64_t var_id = lookup_tmp.name_id;

  ListNode* assign_ir_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, last_ir_node);
  IRNode* assign_ir_nd = List_AccessData(assign_ir_lnode);
  *assign_ir_nd = (IRNode){.dest_id = var_id,
                           .dest_type = kIROperandType_Variable,
                           .ir_operator = kIROperator_Assign,
                           .src1_id = res_id_out,
                           .src1_type = kIROperandType_Variable,
                           .src2_id = 0,
                           .src2_type = kIROperandType_ZeroInit};

  return assign_ir_lnode;
}

static ListNode* TranslateValueExpr(TreeNode* single_st_tnode,
                                    IRBlock* func_ir_block,
                                    ListNode* last_ir_node,
                                    IRNameTable* global_nt,
                                    int64_t* res_id_out) {
  ASSERT(single_st_tnode != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);
  ASSERT(res_id_out != NULL);

  return TranslateValueItself(single_st_tnode->l_child,
                              func_ir_block,
                              last_ir_node,
                              global_nt,
                              res_id_out);
}

static ListNode* TranslateReturnExpr(TreeNode* single_st_tnode,
                                     IRBlock* func_ir_block,
                                     ListNode* last_ir_node,
                                     IRNameTable* global_nt) {
  ASSERT(single_st_tnode != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);

  TreeNode* value_tnode = single_st_tnode->l_child->l_child;
  int64_t res_id = 0;
  last_ir_node = TranslateValueExpr(value_tnode,
                                    func_ir_block,
                                    last_ir_node,
                                    global_nt,
                                    &res_id);

  ListNode* return_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, last_ir_node);
  IRNode* return_ir_nd = List_AccessData(return_lnode);
  *return_ir_nd = (IRNode){.dest_id = res_id,
                           .dest_type = kIROperandType_Variable,
                           .ir_operator = kIROperator_Return,
                           .src1_id = 0,
                           .src1_type = kIROperandType_ZeroInit,
                           .src2_id = 0,
                           .src2_type = kIROperandType_ZeroInit};

  return return_lnode;
}

static ListNode* TranslateIfBranch(TreeNode* if_br_tnode,
                                    IRBlock* func_ir_block,
                                    ListNode* last_ir_node,
                                    IRNameTable* global_nt) {
  ASSERT(if_br_tnode != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);
  ASSERT(if_br_tnode->data.type == kTokenType_TreeSup
         && if_br_tnode->data.tree_sup == kTreeSup_IfBranch);

  char buf_for_str_gen[kMaxCharBufSize] = {0};
  bool is_inserted = false;

  // exit label:
  ListNode* exit_lbl_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, last_ir_node);
  IRNode* exit_lbl_ir_nd = List_AccessData(exit_lbl_lnode);
  int64_t exit_id = func_ir_block->last_local_id++;
  *exit_lbl_ir_nd = (IRNode){.dest_id = exit_id,
                             .dest_type = kIROperandType_LabelName,
                             .ir_operator = kIROperator_Label,
                             .src1_id = 0,
                             .src1_type = kIROperandType_ZeroInit,
                             .src2_id = 0,
                             .src2_type = kIROperandType_ZeroInit};

  snprintf(buf_for_str_gen, kMaxCharBufSize, "exit%ld", exit_id);
  size_t exit_name_len = strlen(buf_for_str_gen);
  is_inserted = IRNameTable_Insert(func_ir_block->local_nt, buf_for_str_gen, exit_name_len, exit_id, kIROperandType_LabelName, 0);
  if (!is_inserted) { ASSERT(0 && ":("); }

  // cond:
  //                     sgle st == if br | connect | kw_if value
  TreeNode* cond_tnode = if_br_tnode->l_child->l_child->r_child;
  int64_t res_id = 0;
  last_ir_node = TranslateValueExpr(cond_tnode,
                                    func_ir_block,
                                    last_ir_node,
                                    global_nt,
                                    &res_id);

  int64_t tmp_id_not_cond = 0;
  last_ir_node = CreateTmpNode(func_ir_block, last_ir_node, &tmp_id_not_cond);

  ListNode* not_cond_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, last_ir_node);
  IRNode* not_cond_ir_nd = List_AccessData(not_cond_lnode);
  *not_cond_ir_nd = (IRNode){.dest_id = tmp_id_not_cond,
                             .dest_type = kIROperandType_Variable,
                             .ir_operator = kIROperator_Not,
                             .src1_id = res_id,
                             .src1_type = kIROperandType_Variable,
                             .src2_id = 0,
                             .src2_type = kIROperandType_ZeroInit};

  int64_t else_br_label_id = func_ir_block->last_local_id++;
  snprintf(buf_for_str_gen, kMaxCharBufSize, "out%ld", else_br_label_id);
  size_t else_br_label_len = strlen(buf_for_str_gen);
  is_inserted = IRNameTable_Insert(func_ir_block->local_nt, buf_for_str_gen, else_br_label_len, else_br_label_id, kIROperandType_LabelName, 0);
  if (!is_inserted) { ASSERT(0 && ":("); }

  ListNode* jmp_if_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, not_cond_lnode);
  IRNode* jmp_if_ir_nd = List_AccessData(jmp_if_lnode);
  *jmp_if_ir_nd = (IRNode){.dest_id = else_br_label_id,
                           .dest_type = kIROperandType_LabelName,
                           .ir_operator = kIROperator_JumpIf,
                           .src1_id = tmp_id_not_cond,
                           .src1_type = kIROperandType_Variable,
                           .src2_id = 0,
                           .src2_type = kIROperandType_ZeroInit};

  //                             sgle st == if br | connect | kw_if value
  TreeNode* statement_if_tnode = if_br_tnode->l_child->l_child->l_child;
  ListNode* end_of_statement_lnode = TranslateStatement(statement_if_tnode,
                                                        func_ir_block,
                                                        jmp_if_lnode,
                                                        global_nt);

  ListNode* jmp_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, end_of_statement_lnode);
  IRNode* jmp_ir_nd = List_AccessData(jmp_lnode);
  *jmp_ir_nd = (IRNode){.dest_id = exit_id,
                        .dest_type = kIROperandType_LabelName,
                        .ir_operator = kIROperator_Jump,
                        .src1_id = 0,
                        .src1_type = kIROperandType_ZeroInit,
                        .src2_id = 0,
                        .src2_type = kIROperandType_ZeroInit};

  ListNode* else_br_label_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, jmp_lnode);
  IRNode* else_br_label_ir_nd = List_AccessData(else_br_label_lnode);
  *else_br_label_ir_nd = (IRNode){.dest_id = else_br_label_id,
                                  .dest_type = kIROperandType_LabelName,
                                  .ir_operator = kIROperator_Label,
                                  .src1_id = 0,
                                  .src1_type = kIROperandType_ZeroInit,
                                  .src2_id = 0,
                                  .src2_type = kIROperandType_ZeroInit};

  //                            if br            connect   else
  TreeNode* else_branch_tnode = if_br_tnode->l_child->r_child;
  if (else_branch_tnode != NULL) {
    ListNode* end_of_else = TranslateElseBranch(else_branch_tnode,
                                                func_ir_block,
                                                else_br_label_lnode,
                                                global_nt);
  }

  return exit_lbl_lnode;
}

static ListNode* TranslateElseBranch(TreeNode* else_br_tnode,
                                     IRBlock* func_ir_block,
                                     ListNode* last_ir_node,
                                     IRNameTable* global_nt) {
  ASSERT(else_br_tnode != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);
  ASSERT(else_br_tnode->data.type == kTokenType_TreeSup
         && (else_br_tnode->data.tree_sup == kTreeSup_ElseIfBranch
             || else_br_tnode->data.tree_sup == kTreeSup_ElseStBranch));

  TreeNode* else_type_tnode = else_br_tnode->l_child->l_child;

  if (else_br_tnode->data.tree_sup == kTreeSup_ElseIfBranch) {
    return TranslateIfBranch(else_type_tnode,
                             func_ir_block,
                             last_ir_node,
                             global_nt);
  } else if (else_br_tnode->data.tree_sup == kTreeSup_ElseStBranch) {
    return TranslateStatement(else_type_tnode,
                              func_ir_block,
                              last_ir_node,
                              global_nt);
  }

  ASSERT(0 && "invalid tree");
  return NULL;
}


static ListNode* TranslateWhileLoop(TreeNode* while_loop_tnode,
                                    IRBlock* func_ir_block,
                                    ListNode* last_ir_node,
                                    IRNameTable* global_nt) {
  ASSERT(while_loop_tnode != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);
  ASSERT(while_loop_tnode->data.type == kTokenType_TreeSup
         && while_loop_tnode->data.tree_sup == kTreeSup_WhileLoop);

  int64_t lbl_for_check_id = func_ir_block->last_local_id++;
  int64_t lbl_for_to_st_id = func_ir_block->last_local_id++;
  // add to nt
  AddNameToLocalNT(func_ir_block->local_nt, lbl_for_check_id, "check", kIROperandType_LabelName);
  AddNameToLocalNT(func_ir_block->local_nt, lbl_for_to_st_id, "while_loop", kIROperandType_LabelName);

  ListNode* jmp_to_check_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, last_ir_node);
  IRNode* jmp_to_check_ir_nd = List_AccessData(jmp_to_check_lnode);
  *jmp_to_check_ir_nd = (IRNode){.dest_id = lbl_for_check_id,
                                 .dest_type = kIROperandType_LabelName,
                                 .ir_operator = kIROperator_Jump,
                                 .src1_id = 0,
                                 .src1_type = kIROperandType_ZeroInit,
                                 .src2_id = 0,
                                 .src2_type = kIROperandType_ZeroInit};

  ListNode* lbl_for_to_st_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, jmp_to_check_lnode);
  IRNode* lbl_for_to_st_ir_nd = List_AccessData(lbl_for_to_st_lnode);
  *lbl_for_to_st_ir_nd = (IRNode){.dest_id = lbl_for_to_st_id,
                                  .dest_type = kIROperandType_LabelName,
                                  .ir_operator = kIROperator_Label,
                                  .src1_id = 0,
                                  .src1_type = kIROperandType_ZeroInit,
                                  .src2_id = 0,
                                  .src2_type = kIROperandType_ZeroInit};

  ListNode* lbl_for_check_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, lbl_for_to_st_lnode);
  IRNode* lbl_for_check_ir_nd = List_AccessData(lbl_for_check_lnode);
  *lbl_for_check_ir_nd = (IRNode){.dest_id = lbl_for_check_id,
                                 .dest_type = kIROperandType_LabelName,
                                 .ir_operator = kIROperator_Label,
                                 .src1_id = 0,
                                 .src1_type = kIROperandType_ZeroInit,
                                 .src2_id = 0,
                                 .src2_type = kIROperandType_ZeroInit};

  int64_t value_id = 0;
  TreeNode* value_expr_tnode = while_loop_tnode->l_child->r_child;
  last_ir_node = TranslateValueExpr(value_expr_tnode,
                                    func_ir_block,
                                    lbl_for_check_lnode,
                                    global_nt,
                                    &value_id);

  ListNode* jmp_if_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, last_ir_node);
  IRNode* jmp_if_ir_nd = List_AccessData(jmp_if_lnode);
  *jmp_if_ir_nd = (IRNode){.dest_id = lbl_for_to_st_id,
                           .dest_type = kIROperandType_LabelName,
                           .ir_operator = kIROperator_JumpIf,
                           .src1_id = value_id,
                           .src1_type = kIROperandType_Variable,
                           .src2_id = 0,
                           .src2_type = kIROperandType_ZeroInit};

  TreeNode* st_tnode = while_loop_tnode->l_child->l_child;
  TranslateStatement(st_tnode,
                     func_ir_block,
                     lbl_for_to_st_lnode,
                     global_nt);

  return jmp_if_lnode;
}

static ListNode* TranslateValueItself(TreeNode* value_tnode,
                                      IRBlock* func_ir_block,
                                      ListNode* last_ir_node,
                                      IRNameTable* global_nt,
                                      int64_t* res_id_out) {
  ASSERT(value_tnode != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);
  ASSERT(res_id_out != NULL);

  if ((value_tnode->data.type == kTokenType_TreeSup)
      && (value_tnode->data.tree_sup == kTreeSup_FunctionCall)) {
    return TranslateFunctionCall(value_tnode,
                                 func_ir_block,
                                 last_ir_node,
                                 global_nt,
                                 res_id_out);
  }

  int64_t res_id = 0;
  ListNode* tmp_ir_lnode = CreateTmpNode(func_ir_block, last_ir_node, &res_id);
  *res_id_out = res_id;

  int64_t id_for_left_res = 0;
  int64_t id_for_right_res = 0;
  if (value_tnode->data.type == kTokenType_Operator) {
    last_ir_node = TranslateValueItself(value_tnode->l_child,
                                        func_ir_block,
                                        last_ir_node,
                                        global_nt,
                                        &id_for_left_res);
    last_ir_node = TranslateValueItself(value_tnode->r_child,
                                        func_ir_block,
                                        last_ir_node,
                                        global_nt,
                                        &id_for_right_res);
  }

  ListNode* assign_ir_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, tmp_ir_lnode);
  IRNode* assign_ir_nd = List_AccessData(assign_ir_lnode);
  if (value_tnode->data.type == kTokenType_Identifier) {
    IRName lookup_tmp = {};
    bool is_found = IRNameTable_LookUpByStr(func_ir_block->local_nt,
                                            value_tnode->data.idnt.str,
                                            value_tnode->data.idnt.len,
                                            &lookup_tmp);
    if (!is_found) { $ return NULL; } // FIXME if identirier not found than return error

    int64_t identifier_id = lookup_tmp.name_id;

    *assign_ir_nd = (IRNode){.dest_id = res_id,
                             .dest_type = kIROperandType_Variable,
                             .ir_operator = kIROperator_Assign,
                             .src1_id = identifier_id,
                             .src1_type = kIROperandType_Variable,
                             .src2_id = 0,
                             .src2_type = kIROperandType_ZeroInit};
  } else if (value_tnode->data.type == kTokenType_Const) {
    *assign_ir_nd = (IRNode){.dest_id = res_id,
                             .dest_type = kIROperandType_Variable,
                             .ir_operator = kIROperator_Assign,
                             .src1_id = value_tnode->data.cnst.int_cnst,
                             .src1_type = kIROperandType_Const,
                             .src2_id = 0,
                             .src2_type = kIROperandType_ZeroInit};
  } else if (value_tnode->data.type == kTokenType_Operator) {
    *assign_ir_nd = (IRNode){.dest_id = res_id,
                             .dest_type = kIROperandType_Variable,
                             .ir_operator = OperatorToIR(value_tnode->data.op),
                             .src1_id = id_for_left_res,
                             .src1_type = kIROperandType_Variable,
                             .src2_id = id_for_right_res,
                             .src2_type = kIROperandType_Variable};
  }

  return assign_ir_lnode;
}

static ListNode* TranslateFunctionCall(TreeNode* func_call_tnode,
                                       IRBlock* func_ir_block,
                                       ListNode* last_ir_node,
                                       IRNameTable* global_nt,
                                       int64_t* res_id_out) {
  ASSERT(func_call_tnode != NULL);
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);
  ASSERT(res_id_out != NULL);

  int64_t call_tmp_id = 0;
  last_ir_node = CreateTmpNode(func_ir_block, last_ir_node, &call_tmp_id);

  TreeNode* func_name_tnode = func_call_tnode->l_child;
  int64_t func_call_id = 0;
  int64_t func_n_param_exp = 0;
  IRName func_call_ir_name = {};
  bool is_found = IRNameTable_LookUpByStr(global_nt, func_name_tnode->data.idnt.str, func_name_tnode->data.idnt.len, &func_call_ir_name);
  if (!is_found) { ASSERT(0 && "cant find such function"); }
  func_call_id = func_call_ir_name.name_id;
  func_n_param_exp = func_call_ir_name.n_param;

  ListNode* call_start_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, last_ir_node);
  IRNode* call_start_ir_nd = List_AccessData(call_start_lnode);
  *call_start_ir_nd = (IRNode){.dest_id = call_tmp_id,
                               .dest_type = kIROperandType_Variable,
                               .ir_operator = kIROperator_CallStart,
                               .src1_id = func_call_id,
                               .src1_type = kIROperandType_Function,
                               .src2_id = func_n_param_exp,
                               .src2_type = kIROperandType_Const};

  TreeNode* param_list_tnode = func_call_tnode->l_child->l_child;

  TreeNode* iter_param_tnode = NULL;
  ListNode* iter_param_lnode = call_start_lnode;
  ListNode* iter_value_lnode = last_ir_node;
  int64_t count_param = 0;
  if (param_list_tnode->data.tree_sup != kTreeSup_FunctionNoParam) {
    iter_param_tnode = param_list_tnode->l_child;
  }

  int64_t value_id = 0;
  while (iter_param_tnode != NULL) {
    iter_value_lnode = TranslateValueExpr(iter_param_tnode, func_ir_block, iter_value_lnode, global_nt, &value_id);
    iter_param_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, iter_param_lnode);
    IRNode* param_ir_nd = List_AccessData(iter_param_lnode);
    *param_ir_nd = (IRNode){.dest_id = func_call_id,
                            .dest_type = kIROperandType_Function,
                            .ir_operator = kIROperator_Parameter,
                            .src1_id = value_id,
                            .src1_type = kIROperandType_Variable,
                            .src2_id = 0,
                            .src2_type = kIROperandType_ZeroInit};

    iter_param_tnode = iter_param_tnode->r_child;
    count_param++;
  }

  ListNode* call_end_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, iter_param_lnode);
  IRNode* call_end_ir_nd = List_AccessData(call_end_lnode);
  *call_end_ir_nd = (IRNode){.dest_id = call_tmp_id,
                             .dest_type = kIROperandType_Variable,
                             .ir_operator = kIROperator_CallEnd,
                             .src1_id = func_call_id,
                             .src1_type = kIROperandType_Function,
                             .src2_id = count_param,
                             .src2_type = kIROperandType_Const};

  *res_id_out = call_tmp_id;
  return call_end_lnode;
}

static int64_t GetNParam(TreeNode* func_node) {
  ASSERT(func_node != NULL);

  TreeNode* param_sup_node = func_node->l_child->l_child->l_child;
  if (param_sup_node->data.tree_sup == kTreeSup_FunctionNoParam) {
    return 0;
  }

  int64_t param_counter = 0;
  TreeNode* iter_var_spec = param_sup_node->l_child;
  while (iter_var_spec != NULL) {
    param_counter++;
    iter_var_spec = iter_var_spec->r_child;
  }

  return param_counter;
}

static int64_t AddParamToLocalNT(TreeNode* func_node, IRNameTable* local_nt) {
  ASSERT(func_node != NULL);
  ASSERT(local_nt != NULL);

  TreeNode* param_sup_node = func_node->l_child->l_child->l_child;
  if (param_sup_node->data.tree_sup == kTreeSup_FunctionNoParam) {
    return 1  ;
  }

  int64_t id_iter = 1;
  TreeNode* iter_var_spec = param_sup_node->l_child;
  while (iter_var_spec != NULL) {
    TreeNode* param_node = iter_var_spec->l_child;
    bool is_inserted = IRNameTable_Insert(local_nt,
                       param_node->data.idnt.str,
                       param_node->data.idnt.len,
                       id_iter,
                       kIROperandType_Variable,
                       0);
    if (!is_inserted) { ASSERT(0); }

    id_iter++;
    iter_var_spec = iter_var_spec->r_child;
  }

  return id_iter;
}

static ListNode* CreateTmpNode(IRBlock* func_ir_block,
                               ListNode* last_ir_node,
                               int64_t* res_id_out) {
  ASSERT(func_ir_block != NULL);
  ASSERT(last_ir_node != NULL);
  ASSERT(res_id_out != NULL);

  char buf_for_str_gen[kMaxCharBufSize] = {0};

  int64_t tmp_id = func_ir_block->last_local_id++;
  snprintf(buf_for_str_gen, kMaxCharBufSize, "tmp%ld", tmp_id); //NOTE maybe __tmp
  size_t tmp_name_len = strlen(buf_for_str_gen);

  bool is_inserted = IRNameTable_Insert(func_ir_block->local_nt,
                                        buf_for_str_gen,
                                        tmp_name_len,
                                        tmp_id,
                                        kIROperandType_Variable,
                                        0);
  if (!is_inserted) { $ ASSERT(0 && ":("); }

  ListNode* new_tmp_lnode = List_CtorNodeAfter(func_ir_block->ir_nodes, last_ir_node);
  IRNode* new_tmp_ir_nd = List_AccessData(new_tmp_lnode);
  *new_tmp_ir_nd = (IRNode){.dest_id = tmp_id,
                            .dest_type = kIROperandType_Variable,
                            .ir_operator = kIROperator_Tmp,
                            .src1_id = 0,
                            .src1_type = kIROperandType_ZeroInit,
                            .src2_id = 0,
                            .src2_type = kIROperandType_ZeroInit};

  *res_id_out = tmp_id;
  return new_tmp_lnode;
}

static void AddNameToLocalNT(IRNameTable* ir_name_table,
                             int64_t new_id,
                             const char* new_name,
                             IROperandType new_ir_op_type) {
  ASSERT(ir_name_table != NULL);
  ASSERT(new_name != NULL);

  char buf_for_str_gen[kMaxCharBufSize] = {0};
  bool is_inserted = false;
  int error = 0;

  snprintf(buf_for_str_gen, kMaxCharBufSize, "%s%ld", new_name, new_id);
  size_t new_name_len = strlen(buf_for_str_gen);
  is_inserted = IRNameTable_Insert(ir_name_table, buf_for_str_gen, new_name_len, new_id, new_ir_op_type, 0);
  if (!is_inserted) { ASSERT(error && "unable to insert new name"); }
}

static IROperator OperatorToIR(Operator op) {
  switch (op) {
    case OP_PLUS:
      return kIROperator_Add;
    case OP_MINUS:
      return kIROperator_Sub;
    case OP_MULT:
      return kIROperator_Mul;
    case OP_AND:
      return kIROperator_And;
    case OP_OR:
      return kIROperator_Or;
    case OP_EQUAL:
      return kIROperator_Equal;
    case OP_NOT_EQUAL:
      return kIROperator_NotEqual;
    case OP_DIV:
      return kIROperator_Div;
    case OP_REM:
      return kIROperator_Rem;
    default:
      ASSERT(0 && ":(");
      return kIROperator_Uninit;
  }
}

#undef kMaxCharBufSize