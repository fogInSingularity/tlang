#include "const_prop_pass.h"

#include <stdint.h>
#include <stdbool.h>

#include "list.h"
#include "my_assert.h"
#include "debug.h"

// static ----------------------------------------------------------------------

static int64_t PropIRBlock(ListNode* ir_block_lnode);
static int64_t PropTmpInIRBlock(ListNode* ir_block_lnode);
static int64_t PropOperatorInIRBlock(ListNode* ir_block_lnode);
static bool CheckForAriphm(IROperator ir_op);
static int64_t PropSingleAriphm(IRNode* ir_nd);

// global ----------------------------------------------------------------------

int64_t ConstPropPass(IR* ir) {
  ASSERT(ir != NULL);

  int64_t proped = 0;
  List* ir_blocks = ir->ir_blocks;

  ListNode* iter_ir_blocks = List_FirstNode(ir_blocks);
  while (iter_ir_blocks != NULL) {
    proped += PropIRBlock(iter_ir_blocks);
$
    iter_ir_blocks = List_NextNode(ir_blocks, iter_ir_blocks);
  }

  return proped;
}

// static ----------------------------------------------------------------------

static int64_t PropIRBlock(ListNode* ir_block_lnode) {
  ASSERT(ir_block_lnode != NULL);

  int64_t opt_tmp_prop = PropTmpInIRBlock(ir_block_lnode);
  int64_t opt_opertor_prop = PropOperatorInIRBlock(ir_block_lnode);

  return opt_tmp_prop + opt_opertor_prop;
}

static int64_t PropTmpInIRBlock(ListNode* ir_block_lnode) {
  ASSERT(ir_block_lnode != NULL);

  IRBlock* ir_block = List_AccessData(ir_block_lnode);
  List* ir_nodes_list = ir_block->ir_nodes;

  int64_t opt_tmp = 0;

  ListNode* tmp_def_lnode = NULL;
  ListNode* tmp_assign_lnode = NULL;
  ListNode* tmp_evaluate_lnode = NULL;
  int64_t tmp_id = 0;

  ListNode* iter_ir_nd_lnode = List_FirstNode(ir_nodes_list);
  while (iter_ir_nd_lnode != NULL) {
    IRNode* ir_nd = List_AccessData(iter_ir_nd_lnode);
    if (ir_nd->ir_operator == kIROperator_Tmp) {
      tmp_def_lnode = iter_ir_nd_lnode;
      tmp_id = ir_nd->dest_id;

      iter_ir_nd_lnode = List_NextNode(ir_nodes_list, iter_ir_nd_lnode);
      break;
    }

    iter_ir_nd_lnode = List_NextNode(ir_nodes_list, iter_ir_nd_lnode);
  }

  while (iter_ir_nd_lnode != NULL) {
    IRNode* ir_nd = List_AccessData(iter_ir_nd_lnode);
    if ((ir_nd->ir_operator == kIROperator_Assign)
        && (ir_nd->dest_id == tmp_id)
        && (ir_nd->dest_type == kIROperandType_Variable)) {
      tmp_assign_lnode = iter_ir_nd_lnode;

      iter_ir_nd_lnode = List_NextNode(ir_nodes_list, iter_ir_nd_lnode);
      break;
    }

    iter_ir_nd_lnode = List_NextNode(ir_nodes_list, iter_ir_nd_lnode);
  }

  while (iter_ir_nd_lnode != NULL) {
    IRNode* ir_nd = List_AccessData(iter_ir_nd_lnode);
    bool tmp_op = ((ir_nd->src1_id == tmp_id)
                  && (ir_nd->src1_type == kIROperandType_Variable))
                  || ((ir_nd->src2_id == tmp_id)
                      && (ir_nd->src2_type == kIROperandType_Variable));
    if (tmp_op) {
      tmp_evaluate_lnode = iter_ir_nd_lnode;

      iter_ir_nd_lnode = List_NextNode(ir_nodes_list, iter_ir_nd_lnode);
      break;
    }

    iter_ir_nd_lnode = List_NextNode(ir_nodes_list, iter_ir_nd_lnode);
  }

  bool check_3_nodes = tmp_def_lnode == NULL
                      || tmp_assign_lnode == NULL
                      || tmp_evaluate_lnode == NULL;
  if (check_3_nodes) {
    PRINT_POINTER(tmp_def_lnode);
    return 0;
  }

  IRNode* tmp_assign_nd = List_AccessData(tmp_assign_lnode);

  int64_t tmp_assign_value = tmp_assign_nd->src1_id;
  IROperandType tmp_assign_type = tmp_assign_nd->src1_type;

  IRNode* tmp_evaluate_nd = List_AccessData(tmp_evaluate_lnode);
  if ((tmp_evaluate_nd->src1_type == kIROperandType_Variable)
      && (tmp_evaluate_nd->src1_id == tmp_id)) {
    tmp_evaluate_nd->src1_type = tmp_assign_type;
    tmp_evaluate_nd->src1_id = tmp_assign_value;
  } else if ((tmp_evaluate_nd->src2_type == kIROperandType_Variable)
            && (tmp_evaluate_nd->src2_id == tmp_id)) {
    tmp_evaluate_nd->src2_type = tmp_assign_type;
    tmp_evaluate_nd->src2_id = tmp_assign_value;
  } else {
    ASSERT(0 && ":(");
  }

  List_RemoveNode(ir_nodes_list, tmp_def_lnode);
  List_RemoveNode(ir_nodes_list, tmp_assign_lnode);

  opt_tmp++;

  return 1;
}

static int64_t PropOperatorInIRBlock(ListNode* ir_block_lnode) {
  ASSERT(ir_block_lnode != NULL);

  int64_t opt_ariphm = 0;

  IRBlock* ir_block = List_AccessData(ir_block_lnode);
  List* ir_nodes_list = ir_block->ir_nodes;

  ListNode* iter_ir_lnode = List_FirstNode(ir_nodes_list);
  while (iter_ir_lnode != NULL) {
    IRNode* ir_nd = List_AccessData(iter_ir_lnode);
    bool check_for_ariphm_prop = CheckForAriphm(ir_nd->ir_operator)
                                 && ((ir_nd->src1_type == kIROperandType_Const)
                                     && (ir_nd->src2_type == kIROperandType_Const));
    if (check_for_ariphm_prop) {
      opt_ariphm += PropSingleAriphm(ir_nd);
    }

    iter_ir_lnode = List_NextNode(ir_nodes_list, iter_ir_lnode);
  }

  return opt_ariphm;
}

static bool CheckForAriphm(IROperator ir_op) {
  switch (ir_op) {
    case kIROperator_Add:      case kIROperator_Sub: case kIROperator_Mul:
    case kIROperator_And:      case kIROperator_Or:  case kIROperator_Equal:
    case kIROperator_NotEqual: case kIROperator_Div: case kIROperator_Rem:
      return true;
  default:
    return false;
  }
}

static int64_t PropSingleAriphm(IRNode* ir_nd) {
  ASSERT(ir_nd != NULL);

  IRNode prop_nd = (IRNode){.dest_id = ir_nd->dest_id,
                            .dest_type = ir_nd->dest_type,
                            .ir_operator = kIROperator_Assign,
                            .src1_id = 0,
                            .src1_type = kIROperandType_Const,
                            .src2_id = 0,
                            .src2_type = kIROperandType_ZeroInit};

  int64_t new_src1_id = 0;

  switch (ir_nd->ir_operator) {
    case kIROperator_Add:
      new_src1_id = ir_nd->src1_id + ir_nd->src2_id;
      break;
    case kIROperator_Sub:
      new_src1_id = ir_nd->src1_id + ir_nd->src2_id;
      break;
    case kIROperator_Mul:
      new_src1_id = ir_nd->src1_id * ir_nd->src2_id;
      break;
    case kIROperator_And:
      new_src1_id = ir_nd->src1_id & ir_nd->src2_id;
      break;
    case kIROperator_Or:
      new_src1_id = ir_nd->src1_id | ir_nd->src2_id;
      break;
    case kIROperator_Equal:
      new_src1_id = ir_nd->src1_id == ir_nd->src2_id;
      break;
    case kIROperator_NotEqual:
      new_src1_id = ir_nd->src1_id != ir_nd->src2_id;
      break;
    case kIROperator_Div:
      new_src1_id = ir_nd->src1_id / ir_nd->src2_id;
      break;
    case kIROperator_Rem:
      new_src1_id = ir_nd->src1_id % ir_nd->src2_id;
      break;
    default:
      ASSERT(0 && "unexpected behavior");
  }

  prop_nd.src1_id = new_src1_id;

  *ir_nd = prop_nd;

  return 1;
}