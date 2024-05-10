#include "ast_builder.h"

#include "token_def.h"
#include "ast_dump.h"

// macro -----------------------------------------------------------------------

// iterate over token array and get token (with check, true == failure)
#define CreateToken_(token_name_, token_arr_, index_, failure_cond_)           \
  Token* token_name_ = DArray_At(token_arr_, index_);                          \
  if (token_name_ == NULL) { return NULL; }                                    \
  if (failure_cond_) { return NULL; }                                          \
  index_ += 1;

// no iteration, only get token without check
#define CreateTokenNoCheck_(token_name_, token_arr_, index_)                   \
  Token* token_name_ = DArray_At(token_arr_, index_);                          \
  if (token_name_ == NULL) { return NULL; }

#define CreateTokenNoNULLCheck_(token_name_, token_arr_, index_)               \
  Token* token_name_ = DArray_At(token_arr_, index_);

#define LEFT_SIDE_ (void*)1
#define RIGHT_SIDE_ (void*)2

// iterate over token array and check for ;
#define CheckForSemicolon_(token_name_, token_arr_, index_)                    \
  Token* token_name_ = DArray_At(token_arr_, index_);                          \
  if (token_name_ == NULL) { return NULL; }                                    \
  if (token_name_->punc != PUNC_SEMICOLON) { return NULL; }                    \
  index_ += 1;

// create new node and call a function for it
#define GetNewNode_(node_name_, function_)                                     \
  TreeNode* node_name_ = function_;                                            \
  if (node_name_ == NULL) { return NULL; }

// create new node, if node_parent_ != then hang it
#define CreateNode_(node_name_, node_data_, node_parent_, child_side_)         \
  TreeNode* node_name_ = TreeCtorNode(node_data_, node_parent_);               \
  if (node_name_ == NULL) { return NULL; }                                     \
  if (node_parent_ == NULL) {                                                  \
    ;                                                                          \
  } else if (child_side_ == LEFT_SIDE_) {                                      \
    ((TreeNode*)node_parent_)->l_child = node_name_;                           \
  } else if (child_side_ == RIGHT_SIDE_) {                                     \
    ((TreeNode*)node_parent_)->r_child = node_name_;                           \
  }                                                                            \
  node_name_->parent = node_parent_;

// connect child and parent
#define ConnectPrntAndChld_(parent_, child_, child_side_)                      \
  if (child_side_ == LEFT_SIDE_) {                                             \
    parent_->l_child = child_;                                                 \
  } else if (child_side_ == RIGHT_SIDE_) {                                     \
    parent_->r_child = child_;                                                 \
  }                                                                            \
  child_->parent = parent_;

// static ----------------------------------------------------------------------

static void ThrowError(DArray* token_arr, Index token_index);
static void GetGrammar(DArray* token_arr, Tree* ast_tree);

// asigment and vars
static TreeNode* GetTypeSpec         (DArray* token_arr, Index* token_index);
static TreeNode* GetVarSpecNoMut     (DArray* token_arr, Index* token_index);
static TreeNode* GetVarSpec          (DArray* token_arr, Index* token_index);
static TreeNode* GetAssignOp         (DArray* token_arr, Index* token_index);
static TreeNode* GetAssignExpr       (DArray* token_arr, Index* token_index);
static TreeNode* GetVarInit          (DArray* token_arr, Index* token_index);

// functions
static TreeNode* GetFuncDef          (DArray* token_arr, Index* token_index);
static TreeNode* GetFuncDefParam     (DArray* token_arr, Index* token_index);
static TreeNode* GetFuncDefParamList (DArray* token_arr, Index* token_index);
static TreeNode* GetFuncCall         (DArray* token_arr, Index* token_index);
static TreeNode* GetFuncCallParam    (DArray* token_arr, Index* token_index);
static TreeNode* GetFuncCallParamList(DArray* token_arr, Index* token_index);

// statements
static TreeNode* GetStatement        (DArray* token_arr, Index* token_index);
static TreeNode* GetStatementList    (DArray* token_arr, Index* token_index);
static TreeNode* GetSingleStatement  (DArray* token_arr, Index* token_index);

// expression
static TreeNode* GetValueExpr        (DArray* token_arr, Index* token_index);
static TreeNode* GetLogicalOrExpr    (DArray* token_arr, Index* token_index);
static TreeNode* GetLogicalAndExpr   (DArray* token_arr, Index* token_index);
static TreeNode* GetLogicalEqualExpr (DArray* token_arr, Index* token_index);
static TreeNode* GetPlusMinusExpr    (DArray* token_arr, Index* token_index);
static TreeNode* GetMultExpr         (DArray* token_arr, Index* token_index);
static TreeNode* GetBasicExpr        (DArray* token_arr, Index* token_index);
static TreeNode* GetReturnExpr       (DArray* token_arr, Index* token_index);

// branches
static TreeNode* GetIfBranch         (DArray* token_arr, Index* token_index);
static TreeNode* GetElseBranch       (DArray* token_arr, Index* token_index);
static TreeNode* GetWhileLoop        (DArray* token_arr, Index* token_index);

// global ----------------------------------------------------------------------

AstError AstBuilder(DArray* token_arr, Tree* ast_tree) {
  ASSERT(token_arr != NULL);
  ASSERT(ast_tree != NULL);

  GetGrammar(token_arr, ast_tree);

  TreeDotDump(ast_tree, DumpAst);

  return kAstError_Success;
}

// static ----------------------------------------------------------------------

static void ThrowError(DArray* token_arr, Index token_index) {
  ASSERT(token_arr != NULL);

  Token* new_token = DArray_At(token_arr, token_index);
  fprintf(stderr, "! Error encountered:\n");
  fprintf(stderr, "!   Line: [ %lu ] Symbol: [ %lu ]\n", new_token->debug.line, new_token->debug.symbol);
}

static void GetGrammar(DArray* token_arr, Tree* ast_tree) {
  ASSERT(token_arr != NULL);
  ASSERT(ast_tree != NULL);

  TreeNode* hold_nd = NULL;
  TreeNode* parent = &ast_tree->root;
  Index token_index = 0;

#define NodeJungling                                                           \
  if (hold_nd != NULL) {                                                       \
    parent->r_child = hold_nd;                                                 \
    hold_nd->parent = parent;                                                  \
    parent = hold_nd;                                                          \
    continue;                                                                  \
  }
// end define

  do {
    hold_nd = GetFuncDef(token_arr, &token_index);
    NodeJungling;

    ThrowError(token_arr, token_index);
  } while (hold_nd != NULL && token_index < token_arr->n_elem);
}

static TreeNode* GetFuncDef(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(kw_fn_tkn, token_arr, local_index,
               kw_fn_tkn->type != kTokenType_KeyWord
               || kw_fn_tkn->key_word != KW_FN);
  CreateToken_(func_name_tkn, token_arr, local_index,
               func_name_tkn->type != kTokenType_Identifier);

  GetNewNode_(func_param_nd, GetFuncDefParam(token_arr, &local_index));

  CreateToken_(punc_colon_tkn, token_arr, local_index,
               punc_colon_tkn->type != kTokenType_Punctuation
               || punc_colon_tkn->punc != PUNC_COLON);

  GetNewNode_(type_spec_nd, GetTypeSpec(token_arr, &local_index));
  GetNewNode_(statement_nd, GetStatement(token_arr, &local_index));

  Token fn_def_sup_tkn = {.type = kTokenType_TreeSup,
                          .tree_sup = kTreeSup_FunctionDef};
  CreateNode_(fn_def_sup_nd, &fn_def_sup_tkn, NULL, NULL);

  CreateNode_(kw_fn_nd, kw_fn_tkn, fn_def_sup_nd, LEFT_SIDE_);
  CreateNode_(func_name_nd, func_name_tkn, kw_fn_nd, LEFT_SIDE_);

  ConnectPrntAndChld_(func_name_nd, func_param_nd, LEFT_SIDE_);
  ConnectPrntAndChld_(func_name_nd, type_spec_nd, RIGHT_SIDE_);
  ConnectPrntAndChld_(kw_fn_nd, statement_nd, RIGHT_SIDE_);

  *token_index = local_index;

  return fn_def_sup_nd;
}

static TreeNode* GetFuncDefParam(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(punc_lr_bracket_tkn, token_arr, local_index,
               punc_lr_bracket_tkn->type != kTokenType_Punctuation
               || punc_lr_bracket_tkn->punc != PUNC_LEFT_ROUND_BRACKET);

  TreeNode* func_def_param_list = GetFuncDefParamList(token_arr, &local_index);

  CreateToken_(punc_rr_bracket_tkn, token_arr, local_index,
               punc_rr_bracket_tkn->type != kTokenType_Punctuation
               || punc_rr_bracket_tkn->punc != PUNC_RIGHT_ROUND_BRACKET);

  if (func_def_param_list != NULL) {
    Token func_def_param_sup_tkn = {.type = kTokenType_TreeSup,
                                    .tree_sup = kTreeSup_FunctionParamList};
    CreateNode_(func_def_param_sup_nd, &func_def_param_sup_tkn, NULL, NULL);
    ConnectPrntAndChld_(func_def_param_sup_nd, func_def_param_list, LEFT_SIDE_);

    *token_index = local_index;
    return func_def_param_sup_nd;
  } else {
    Token func_def_param_sup_tkn = {.type = kTokenType_TreeSup,
                                    .tree_sup = kTreeSup_FunctionNoParam};
    CreateNode_(func_def_param_sup_nd, &func_def_param_sup_tkn, NULL, NULL);

    *token_index = local_index;
    return func_def_param_sup_nd;
  }
}

static TreeNode* GetFuncDefParamList(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  GetNewNode_(var_spec_nd, GetVarSpec(token_arr, &local_index));

  if (var_spec_nd == NULL) {
    return NULL;
  } else {
    CreateTokenNoNULLCheck_(punc_comma_tkn, token_arr, local_index);
    if (punc_comma_tkn == NULL) {
      *token_index = local_index;
      return var_spec_nd;
    } else if (punc_comma_tkn->type != kTokenType_Punctuation
               || punc_comma_tkn->punc != PUNC_COMMA) {
      *token_index = local_index;
      return var_spec_nd;
    } else {
      local_index++;

      TreeNode* func_def_param_list_rec_nd = GetFuncDefParamList(token_arr,
                                                                 &local_index);

      if (func_def_param_list_rec_nd != NULL) {
        ConnectPrntAndChld_(var_spec_nd,
                            func_def_param_list_rec_nd,
                            RIGHT_SIDE_);
      }

      *token_index = local_index;
      return var_spec_nd;
    }
  }
}

static TreeNode* GetVarSpec(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateTokenNoCheck_(kw_mut_try_tkn, token_arr, local_index);
  if (kw_mut_try_tkn->type == kTokenType_KeyWord
      && kw_mut_try_tkn->key_word == KW_MUT) {
    local_index++;
  }

  GetNewNode_(var_spec_no_mut_nd, GetVarSpecNoMut(token_arr, &local_index));

  if (kw_mut_try_tkn->type == kTokenType_KeyWord
      && kw_mut_try_tkn->key_word == KW_MUT) {
    CreateNode_(kw_mut_try_nd, kw_mut_try_tkn, var_spec_no_mut_nd, RIGHT_SIDE_);
  }

  Token var_spec_sup_tkn = {.type = kTokenType_TreeSup,
                        .tree_sup = kTreeSup_VarSpec};
  CreateNode_(var_spec_sup_nd, &var_spec_sup_tkn, NULL, NULL);
  ConnectPrntAndChld_(var_spec_sup_nd, var_spec_no_mut_nd, LEFT_SIDE_);

  *token_index = local_index;

  return var_spec_sup_nd;
}

static TreeNode* GetVarSpecNoMut(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(var_name_tkn, token_arr, local_index,
    var_name_tkn->type != kTokenType_Identifier);

  CreateToken_(punc_colon_tkn, token_arr, local_index,
    punc_colon_tkn->type != kTokenType_Punctuation
    || punc_colon_tkn->punc != PUNC_COLON);

  GetNewNode_(type_spec_nd, GetTypeSpec(token_arr, &local_index));

  CreateNode_(var_name_nd, var_name_tkn, NULL, NULL);

  ConnectPrntAndChld_(var_name_nd, type_spec_nd, LEFT_SIDE_);

  *token_index = local_index;

  return var_name_nd;
}

static TreeNode* GetTypeSpec(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(type_tkn, token_arr, local_index,
               type_tkn->type != kTokenType_KeyWord);

  switch (type_tkn->key_word) {
    case KW_I64: {
      CreateNode_(type_nd, type_tkn, NULL, NULL);
      *token_index = local_index;
      return type_nd;
    }
    default:
      return NULL;
  }
}

static TreeNode* GetStatement(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(punc_left_curl_br_tkn, token_arr, local_index,
    punc_left_curl_br_tkn->type != kTokenType_Punctuation
    || punc_left_curl_br_tkn->punc != PUNC_LEFT_CURL_BRACKET);

  TreeNode* statement_list_nd = GetStatementList(token_arr, &local_index);
  CreateToken_(punc_right_curl_br_tkn, token_arr, local_index,
    punc_right_curl_br_tkn->type != kTokenType_Punctuation
    || punc_right_curl_br_tkn->punc != PUNC_RIGHT_CURL_BRACKET);

  Token statement_sup_tkn = {.type = kTokenType_TreeSup,
                             .tree_sup = kTreeSup_Statement};
  CreateNode_(statement_sup_nd, &statement_sup_tkn, NULL, NULL);

  if (statement_list_nd != NULL) {
    ConnectPrntAndChld_(statement_sup_nd, statement_list_nd, LEFT_SIDE_);
  }

  *token_index = local_index;
  return statement_sup_nd;
}

static TreeNode* GetStatementList(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;
  TreeNode* hold_nd = NULL;
  TreeNode* main_nd = NULL;
  bool flag = true;

  do {
    TreeNode* single_st_nd = GetSingleStatement(token_arr, &local_index);
    if (single_st_nd == NULL) { break; }

    if (hold_nd == NULL) {
      main_nd = single_st_nd;
    } else {
      ConnectPrntAndChld_(hold_nd, single_st_nd, RIGHT_SIDE_);
    }

    hold_nd = single_st_nd;
  } while (flag);

  *token_index = local_index;
  return main_nd;
}

static TreeNode* GetSingleStatement(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  TreeNode* var_init_nd = GetVarInit(token_arr, &local_index);
  if (var_init_nd != NULL) {
    *token_index = local_index;
    return var_init_nd;
  }

  TreeNode* assign_expr_nd = GetAssignExpr(token_arr, &local_index);
  if (assign_expr_nd != NULL) {
    *token_index = local_index;
    return assign_expr_nd;
  }

  TreeNode* value_expr_nd = GetValueExpr(token_arr, &local_index);
  if (value_expr_nd != NULL) {
    *token_index = local_index;
    return value_expr_nd;
  }

  TreeNode* return_expr_nd = GetReturnExpr(token_arr, &local_index);
  if (return_expr_nd != NULL) {
    *token_index = local_index;
    return return_expr_nd;
  }

  TreeNode* if_branch_nd = GetIfBranch(token_arr, &local_index);
  if (if_branch_nd != NULL) {
    *token_index = local_index;
    return if_branch_nd;
  }

  TreeNode* while_loop = GetWhileLoop(token_arr, &local_index);
  if (while_loop != NULL) {
    *token_index = local_index;
    return while_loop;
  }

  return NULL;
}

static TreeNode* GetAssignExpr(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(var_name_tkn, token_arr, local_index,
    var_name_tkn->type != kTokenType_Identifier);

  GetNewNode_(assign_op_nd, GetAssignOp(token_arr, &local_index));
  GetNewNode_(value_expr_nd, GetValueExpr(token_arr, &local_index));

  CreateNode_(var_name_nd, var_name_tkn, NULL, NULL);

  CheckForSemicolon_(punc_semi_colon_tkn, token_arr, local_index);

  ConnectPrntAndChld_(var_name_nd, assign_op_nd, LEFT_SIDE_);
  ConnectPrntAndChld_(assign_op_nd, value_expr_nd, LEFT_SIDE_);

  Token assign_expr_sup_tkn = {.type = kTokenType_TreeSup,
                               .tree_sup = kTreeSup_AssignExpr};
  CreateNode_(assign_expr_sup_nd, &assign_expr_sup_tkn, NULL, NULL);
  ConnectPrntAndChld_(assign_expr_sup_nd, var_name_nd, LEFT_SIDE_);

  *token_index = local_index;

  return assign_expr_sup_nd;
}

static TreeNode* GetVarInit(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(kw_let_tkn, token_arr, local_index,
               kw_let_tkn->type != kTokenType_KeyWord
               || kw_let_tkn->key_word != KW_LET);

  GetNewNode_(var_spec_nd, GetVarSpec(token_arr, &local_index));

  CreateToken_(op_assign_tkn, token_arr, local_index,
               op_assign_tkn->type != kTokenType_Operator
               || op_assign_tkn->op != OP_ASSIGN);

  GetNewNode_(value_expr_nd, GetValueExpr(token_arr, &local_index));

  CheckForSemicolon_(punc_semi_colon_tkn, token_arr, local_index);

  CreateNode_(kw_let_nd, kw_let_tkn, NULL, NULL);
  CreateNode_(op_assign_nd, op_assign_tkn, NULL, NULL);
  ConnectPrntAndChld_(kw_let_nd, var_spec_nd, LEFT_SIDE_);
  ConnectPrntAndChld_(kw_let_nd, op_assign_nd, RIGHT_SIDE_);

  ConnectPrntAndChld_(op_assign_nd, value_expr_nd, RIGHT_SIDE_);

  Token var_init_sup_tkn = {.type = kTokenType_TreeSup,
                            .tree_sup = kTreeSup_VarInit};
  CreateNode_(var_init_sup_nd, &var_init_sup_tkn, NULL, NULL);
  ConnectPrntAndChld_(var_init_sup_nd, kw_let_nd, LEFT_SIDE_);

  *token_index = local_index;
  return var_init_sup_nd;
}

static TreeNode* GetAssignOp(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(op_multy_assign_tkn, token_arr, local_index,
    op_multy_assign_tkn->type != kTokenType_Operator);

  switch(op_multy_assign_tkn->op) {
    case OP_ASSIGN: {
      CreateNode_(op_multy_assign_nd, op_multy_assign_tkn, NULL, NULL);
      *token_index = local_index;
      return op_multy_assign_nd;
    }
    default:
      return NULL;
  }
}

static TreeNode* GetValueExpr(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  GetNewNode_(plm_expr_nd, GetLogicalOrExpr(token_arr, token_index));

  Token value_expr_sup_tkn = {.type = kTokenType_TreeSup,
                              .tree_sup = kTreeSup_ValueExpr};
  CreateNode_(value_expr_sup_nd, &value_expr_sup_tkn, NULL, NULL);
  ConnectPrntAndChld_(value_expr_sup_nd, plm_expr_nd, LEFT_SIDE_);

  return value_expr_sup_nd;
}

static TreeNode* GetLogicalOrExpr(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  GetNewNode_(logical_and_nd, GetLogicalAndExpr(token_arr, &local_index));

  bool flag = true;
  TreeNode* hold_nd = logical_and_nd;
  TreeNode* main_md = NULL;

  while (flag) {
    flag = false;

    CreateTokenNoNULLCheck_(op_logical_or_tkn, token_arr, local_index);
    if (op_logical_or_tkn == NULL) { break; }
    if (op_logical_or_tkn->type != kTokenType_Operator
        || op_logical_or_tkn->op != OP_OR) {
      break;
    }
    local_index++;

    GetNewNode_(logical_and_expr_nd, GetLogicalAndExpr(token_arr, &local_index));

    CreateNode_(op_logical_or_nd, op_logical_or_tkn, NULL, NULL);
    ConnectPrntAndChld_(op_logical_or_nd, hold_nd, RIGHT_SIDE_);
    ConnectPrntAndChld_(op_logical_or_nd, logical_and_expr_nd, LEFT_SIDE_);
    hold_nd = op_logical_or_nd;

    if (main_md == NULL) {
      main_md = op_logical_or_nd;
    }

    flag = true;
  }

  *token_index = local_index;
  return main_md == NULL ? logical_and_nd : main_md;
}

static TreeNode* GetLogicalAndExpr(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  GetNewNode_(logical_equal_nd, GetLogicalEqualExpr(token_arr, &local_index));

  bool flag = true;
  TreeNode* hold_nd = logical_equal_nd;
  TreeNode* main_md = NULL;

  while (flag) {
    flag = false;

    CreateTokenNoNULLCheck_(op_logical_and_tkn, token_arr, local_index);
    if (op_logical_and_tkn == NULL) { break; }
    if (op_logical_and_tkn->type != kTokenType_Operator
        || op_logical_and_tkn->op != OP_AND) {
      break;
    }
    local_index++;

    GetNewNode_(logical_equal_expr_nd, GetLogicalEqualExpr(token_arr, &local_index));

    CreateNode_(op_logical_and_nd, op_logical_and_tkn, NULL, NULL);
    ConnectPrntAndChld_(op_logical_and_nd, hold_nd, RIGHT_SIDE_);
    ConnectPrntAndChld_(op_logical_and_nd, logical_equal_expr_nd, LEFT_SIDE_);
    hold_nd = op_logical_and_nd;

    if (main_md == NULL) {
      main_md = op_logical_and_nd;
    }

    flag = true;
  }

  *token_index = local_index;
  return main_md == NULL ? logical_equal_nd : main_md;
}

static TreeNode* GetLogicalEqualExpr(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  GetNewNode_(plm_expr_nd, GetPlusMinusExpr(token_arr, &local_index));

  bool flag = true;
  TreeNode* hold_nd = plm_expr_nd;
  TreeNode* main_md = NULL;

  while (flag) {
    flag = false;

    CreateTokenNoNULLCheck_(op_logical_equal_tkn, token_arr, local_index);
    if (op_logical_equal_tkn == NULL) { break; }
    if (op_logical_equal_tkn->type != kTokenType_Operator
        || (op_logical_equal_tkn->op != OP_EQUAL
            && op_logical_equal_tkn->op != OP_NOT_EQUAL)) {
      break;
    }
    local_index++;

    GetNewNode_(new_plm_expr_nd, GetLogicalEqualExpr(token_arr, &local_index));

    CreateNode_(op_logical_equal_nd, op_logical_equal_tkn, NULL, NULL);
    ConnectPrntAndChld_(op_logical_equal_nd, hold_nd, RIGHT_SIDE_);
    ConnectPrntAndChld_(op_logical_equal_nd, new_plm_expr_nd, LEFT_SIDE_);
    hold_nd = op_logical_equal_nd;

    if (main_md == NULL) {
      main_md = op_logical_equal_nd;
    }

    flag = true;
  }

  *token_index = local_index;
  return main_md == NULL ? plm_expr_nd : main_md;
}

static TreeNode* GetPlusMinusExpr(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  GetNewNode_(def_mul_nd, GetMultExpr(token_arr, &local_index));

  bool flag = true;
  TreeNode* hold_nd = def_mul_nd;

  while (flag) {
    flag = false;

    CreateTokenNoNULLCheck_(op_plm_tkn, token_arr, local_index);
    if (op_plm_tkn == NULL) { break; }
    if (op_plm_tkn->type != kTokenType_Operator
        || (op_plm_tkn->op != OP_PLUS
            && op_plm_tkn->op != OP_MINUS)) {
      break;
    }
    local_index++;

    GetNewNode_(mult_expr_nd, GetMultExpr(token_arr, &local_index));

    CreateNode_(op_plm_nd, op_plm_tkn, NULL, NULL);
    ConnectPrntAndChld_(op_plm_nd, hold_nd, RIGHT_SIDE_);
    ConnectPrntAndChld_(op_plm_nd, mult_expr_nd, LEFT_SIDE_);
    hold_nd = op_plm_nd;

    flag = true;
  }

  *token_index = local_index;
  return hold_nd;
}

static TreeNode* GetMultExpr(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  GetNewNode_(def_basic_nd, GetBasicExpr(token_arr, &local_index));

  bool flag = true;
  TreeNode* hold_nd = def_basic_nd;

  while (flag) {
    flag = false;

    CreateTokenNoNULLCheck_(op_mult_tkn, token_arr, local_index);
    if (op_mult_tkn == NULL) { break; }
    if (op_mult_tkn->type != kTokenType_Operator
        || (op_mult_tkn->op != OP_MULT
            && op_mult_tkn->op != OP_DIV
            && op_mult_tkn->op != OP_REM)) {
      break;
    }
    local_index++;

    GetNewNode_(basic_expr_nd, GetBasicExpr(token_arr, &local_index));

    CreateNode_(op_mult_nd, op_mult_tkn, NULL, NULL);
    ConnectPrntAndChld_(op_mult_nd, hold_nd, RIGHT_SIDE_);
    ConnectPrntAndChld_(op_mult_nd, basic_expr_nd, LEFT_SIDE_);
    hold_nd = op_mult_nd;

    flag = true;
  }

  *token_index = local_index;
  return hold_nd;
}

static TreeNode* GetBasicExpr(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  TreeNode* func_call_nd = GetFuncCall(token_arr, &local_index);
  if (func_call_nd != NULL) {
    *token_index = local_index;
    return func_call_nd;
  }

  CreateTokenNoCheck_(multy_tkn, token_arr, local_index);
  if (multy_tkn->type == kTokenType_Identifier
      || multy_tkn->type == kTokenType_Const
      || multy_tkn->type == kTokenType_StringLit) {
    CreateNode_(multy_nd, multy_tkn, NULL, NULL);
    local_index++;

    *token_index = local_index;
    return multy_nd;
  }

  CreateToken_(punc_left_rnd_br_tkn, token_arr, local_index,
    punc_left_rnd_br_tkn->type != kTokenType_Punctuation
    || punc_left_rnd_br_tkn->punc != PUNC_LEFT_ROUND_BRACKET);

  GetNewNode_(value_nd, GetValueExpr(token_arr, &local_index));

  CreateToken_(punc_right_rnd_br_tkn, token_arr, local_index,
    punc_right_rnd_br_tkn->type != kTokenType_Punctuation
    || punc_right_rnd_br_tkn->punc != PUNC_RIGHT_ROUND_BRACKET);

  *token_index = local_index;
  return value_nd;
}

static TreeNode* GetReturnExpr(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(kw_return_tkn, token_arr, local_index,
              kw_return_tkn->type != kTokenType_KeyWord
              || kw_return_tkn->key_word != KW_RETURN);

  GetNewNode_(value_expr_nd, GetValueExpr(token_arr, &local_index));
  CheckForSemicolon_(punc_semi_colon_tkn, token_arr, local_index);

  Token return_expr_sup_tkn = {.type = kTokenType_TreeSup,
                               .tree_sup = kTreeSup_ReturnExpr};

  CreateNode_(return_expr_sup_nd, &return_expr_sup_tkn, NULL, NULL);
  CreateNode_(kw_return_nd, kw_return_tkn, return_expr_sup_nd, LEFT_SIDE_);
  ConnectPrntAndChld_(kw_return_nd, value_expr_nd, LEFT_SIDE_);

  *token_index = local_index;
  return return_expr_sup_nd;
}

static TreeNode* GetFuncCall(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(func_name_tkn, token_arr, local_index,
    func_name_tkn->type != kTokenType_Identifier);

  GetNewNode_(func_call_param_nd, GetFuncCallParam(token_arr, &local_index));
  CreateNode_(func_name_nd, func_name_tkn, NULL, NULL);
  ConnectPrntAndChld_(func_name_nd, func_call_param_nd, LEFT_SIDE_);

  Token func_call_sup_tkn = {.type = kTokenType_TreeSup,
                             .tree_sup = kTreeSup_FunctionCall};
  CreateNode_(func_call_sup_nd, &func_call_sup_tkn, NULL, NULL);
  ConnectPrntAndChld_(func_call_sup_nd, func_name_nd, LEFT_SIDE_);

  *token_index = local_index;
  return func_call_sup_nd;
}

static TreeNode* GetFuncCallParam(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(punc_lr_bracket_tkn, token_arr, local_index,
               punc_lr_bracket_tkn->type != kTokenType_Punctuation
               || punc_lr_bracket_tkn->punc != PUNC_LEFT_ROUND_BRACKET);

  TreeNode* func_call_param_list = GetFuncCallParamList(token_arr, &local_index);

  CreateToken_(punc_rr_bracket_tkn, token_arr, local_index,
               punc_rr_bracket_tkn->type != kTokenType_Punctuation
               || punc_rr_bracket_tkn->punc != PUNC_RIGHT_ROUND_BRACKET);

  if (func_call_param_list != NULL) {
    Token func_call_param_sup_tkn = {.type = kTokenType_TreeSup,
                                    .tree_sup = kTreeSup_FunctionParamList};
    CreateNode_(func_call_param_sup_nd, &func_call_param_sup_tkn, NULL, NULL);
    ConnectPrntAndChld_(func_call_param_sup_nd, func_call_param_list, LEFT_SIDE_);

    *token_index = local_index;
    return func_call_param_sup_nd;
  } else {
    Token func_call_param_sup_tkn = {.type = kTokenType_TreeSup,
                                    .tree_sup = kTreeSup_FunctionNoParam};
    CreateNode_(func_call_param_sup_nd, &func_call_param_sup_tkn, NULL, NULL);

    *token_index = local_index;
    return func_call_param_sup_nd;
  }
}

static TreeNode* GetFuncCallParamList(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  GetNewNode_(value_expr_nd, GetValueExpr(token_arr, &local_index));

  if (value_expr_nd == NULL) {
    return NULL;
  } else {
    CreateTokenNoNULLCheck_(punc_comma_tkn, token_arr, local_index);
    if (punc_comma_tkn == NULL) {
      *token_index = local_index;
      return value_expr_nd;
    } else if (punc_comma_tkn->type != kTokenType_Punctuation
               || punc_comma_tkn->punc != PUNC_COMMA) {
      *token_index = local_index;
      return value_expr_nd;
    } else {
      local_index++;

      TreeNode* func_call_param_list_rec_nd = GetFuncCallParamList(token_arr,
                                                                   &local_index);

      if (func_call_param_list_rec_nd != NULL) {
        ConnectPrntAndChld_(value_expr_nd,
                            func_call_param_list_rec_nd,
                            RIGHT_SIDE_);
      }

      *token_index = local_index;
      return value_expr_nd;
    }
  }
}

static TreeNode* GetIfBranch(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(kw_if_tkn, token_arr, local_index,
               kw_if_tkn->type != kTokenType_KeyWord
               || kw_if_tkn->key_word != KW_IF);
  CreateToken_(punc_lr_bracket_tkn, token_arr, local_index,
               punc_lr_bracket_tkn->type != kTokenType_Punctuation
               || punc_lr_bracket_tkn->punc != PUNC_LEFT_ROUND_BRACKET);

  GetNewNode_(value_expr_nd, GetValueExpr(token_arr, &local_index));

  CreateToken_(punc_rr_bracket_tkn, token_arr, local_index,
               punc_rr_bracket_tkn->type != kTokenType_Punctuation
               || punc_rr_bracket_tkn->punc != PUNC_RIGHT_ROUND_BRACKET);

  GetNewNode_(statement_nd, GetStatement(token_arr, &local_index));

  CreateNode_(kw_if_nd, kw_if_tkn, NULL, NULL);
  ConnectPrntAndChld_(kw_if_nd, value_expr_nd, RIGHT_SIDE_);
  ConnectPrntAndChld_(kw_if_nd, statement_nd, LEFT_SIDE_);

  Token if_branch_sup_tkn = {.type = kTokenType_TreeSup,
                            .tree_sup = kTreeSup_IfBranch};
  CreateNode_(if_branch_sup_nd, &if_branch_sup_tkn, NULL, NULL);

  TreeNode* else_branch_nd = GetElseBranch(token_arr, &local_index);
  if (else_branch_nd != NULL) { // else_branch_nd can be NULL
    Token connect_if_else_sup_tkn = {.type = kTokenType_TreeSup,
                                     .tree_sup = kTreeSup_ConnectIfElse};
    CreateNode_(connect_if_else_sup_nd, &connect_if_else_sup_tkn, NULL, NULL);
    ConnectPrntAndChld_(if_branch_sup_nd, connect_if_else_sup_nd, LEFT_SIDE_);
    ConnectPrntAndChld_(connect_if_else_sup_nd, kw_if_nd, LEFT_SIDE_);
    ConnectPrntAndChld_(connect_if_else_sup_nd, else_branch_nd, RIGHT_SIDE_);
  } else {
    ConnectPrntAndChld_(if_branch_sup_nd, kw_if_nd, LEFT_SIDE_);
  }

  *token_index = local_index;
  return if_branch_sup_nd;
}

static TreeNode* GetElseBranch(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(kw_else_tkn, token_arr, local_index,
               kw_else_tkn->type != kTokenType_KeyWord
               || kw_else_tkn->key_word != KW_ELSE);

  TreeNode* if_branch_nd = GetIfBranch(token_arr, &local_index);
  if (if_branch_nd != NULL) {
    CreateNode_(kw_else_nd, kw_else_tkn, NULL, NULL);
    ConnectPrntAndChld_(kw_else_nd, if_branch_nd, LEFT_SIDE_);

    Token else_if_branch_sup_tkn = {.type = kTokenType_TreeSup,
                                    .tree_sup = kTreeSup_ElseIfBranch};
    CreateNode_(else_if_branch_sup_nd, &else_if_branch_sup_tkn, NULL, NULL);
    ConnectPrntAndChld_(else_if_branch_sup_nd, kw_else_nd, LEFT_SIDE_);

    *token_index = local_index;
    return else_if_branch_sup_nd;
  }

  TreeNode* st_branch_nd = GetStatement(token_arr, &local_index);
  if (st_branch_nd != NULL) {
    CreateNode_(kw_else_nd, kw_else_tkn, NULL, NULL);
    ConnectPrntAndChld_(kw_else_nd, st_branch_nd, LEFT_SIDE_);

    Token else_st_branch_sup_tkn = {.type = kTokenType_TreeSup,
                                    .tree_sup = kTreeSup_ElseStBranch};
    CreateNode_(else_st_branch_sup_nd, &else_st_branch_sup_tkn, NULL, NULL);
    ConnectPrntAndChld_(else_st_branch_sup_nd, kw_else_nd, LEFT_SIDE_);

    *token_index = local_index;
    return else_st_branch_sup_nd;
  }

  return NULL;
}

static TreeNode* GetWhileLoop(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(kw_while_tkn, token_arr, local_index,
               kw_while_tkn->type != kTokenType_KeyWord
               || kw_while_tkn->key_word != KW_WHILE);

  CreateToken_(punc_lr_bracket_tkn, token_arr, local_index,
               punc_lr_bracket_tkn->type != kTokenType_Punctuation
               || punc_lr_bracket_tkn->punc != PUNC_LEFT_ROUND_BRACKET);

  GetNewNode_(value_expr_nd, GetValueExpr(token_arr, &local_index));

  CreateToken_(punc_rr_bracket_tkn, token_arr, local_index,
               punc_rr_bracket_tkn->type != kTokenType_Punctuation
               || punc_rr_bracket_tkn->punc != PUNC_RIGHT_ROUND_BRACKET);

  GetNewNode_(statement_nd, GetStatement(token_arr, &local_index));

  CreateNode_(kw_while_nd, kw_while_tkn, NULL, NULL);
  ConnectPrntAndChld_(kw_while_nd, statement_nd, LEFT_SIDE_);
  ConnectPrntAndChld_(kw_while_nd, value_expr_nd, RIGHT_SIDE_);

  Token while_loop_sup_tkn = {.type = kTokenType_TreeSup,
                              .tree_sup = kTreeSup_WhileLoop};
  CreateNode_(while_loop_sup_nd, &while_loop_sup_tkn, NULL, NULL);
  ConnectPrntAndChld_(while_loop_sup_nd, kw_while_nd, LEFT_SIDE_);

  *token_index = local_index;

  return while_loop_sup_nd;
}

//macro_undef------------------------------------------------------------------

#undef CreateNode_
#undef GetNewNode_
#undef CreateToken_
#undef CreateTokenNoCheck_
#undef CheckForSemicolon_
#undef CreateTokenNoNULLCheck_
#undef LEFT_SIDE_
#undef RIGHT_SIDE_
