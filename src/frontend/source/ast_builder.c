#include "ast_builder.h"

//macro------------------------------------------------------------------------

#define CreateToken_(token_name_, token_arr_, index_, ...)                    \
  Token* token_name_ = DArray_At(token_arr_, index_);                         \
  if (token_name_ == NULL) { return NULL; }                                   \
  if (__VA_ARGS__) { return NULL; }                                           \
  index_ += 1;

#define CreateTokenNoCheck_(token_name_, token_arr_, index_)                  \
  Token* token_name_ = DArray_At(token_arr_, index_);                         \
  if (token_name_ == NULL) { return NULL; }

#define LEFT_SIDE_ (void*)1
#define RIGHT_SIDE_ (void*)2

#define GetNewNode_(node_name_, function_)                                    \
  TreeNode* node_name_ = function_;                                           \
  if (node_name_ == NULL) { return NULL; }

#define CreateNode_(node_name_, node_data_, node_parent_, child_side_)        \
  TreeNode* node_name_ = TreeCtorNode(node_data_, node_parent_);              \
  if (node_name_ == NULL) { return NULL; }                                    \
  if (node_parent_ == NULL) {                                                 \
    ;                                                                         \
  } else if (child_side_ == LEFT_SIDE_) {                                     \
    ((TreeNode*)node_parent_)->l_child = node_name_;                          \
  } else if (child_side_ == RIGHT_SIDE_) {                                    \
    ((TreeNode*)node_parent_)->r_child = node_name_;                          \
  }                                                                           \
  node_name_->parent = node_parent_;

#define ConnectPrntAndChld_(parent_, child_, child_side_)                     \
  if (child_side_ == LEFT_SIDE_) {                                            \
    parent_->l_child = child_;                                                \
  } else if (child_side_ == RIGHT_SIDE_) {                                    \
    parent_->r_child = child_;                                                \
  }                                                                           \
  child_->parent = parent_;

//static-----------------------------------------------------------------------

static void GetGrammar(DArray* token_arr, Tree* ast_tree);
static TreeNode* GetImportStatement  (DArray* token_arr, Index* token_index);
static TreeNode* GetEnumDecl         (DArray* token_arr, Index* token_index);
static TreeNode* GetEnumList         (DArray* token_arr, Index* token_index);
static TreeNode* GetEnumItem         (DArray* token_arr, Index* token_index);
static TreeNode* GetFuncDecl         (DArray* token_arr, Index* token_index);
static TreeNode* GetFuncParam        (DArray* token_arr, Index* token_index);
static TreeNode* GetFuncParamList    (DArray* token_arr, Index* token_index);
static TreeNode* GetVarSpec          (DArray* token_arr, Index* token_index);
static TreeNode* GetVarSpecNoMut     (DArray* token_arr, Index* token_index);
static TreeNode* GetTypeSpec         (DArray* token_arr, Index* token_index);
static TreeNode* GetConstValue       (DArray* token_arr, Index* token_index);
static TreeNode* GetStatement        (DArray* token_arr, Index* token_index);
static TreeNode* GetStatementList    (DArray* token_arr, Index* token_index);
static TreeNode* GetSingleStatement  (DArray* token_arr, Index* token_index);
static TreeNode* GetAssignExpr       (DArray* token_arr, Index* token_index);
static TreeNode* GetAssignOp         (DArray* token_arr, Index* token_index);
static TreeNode* GetVarInit          (DArray* token_arr, Index* token_index);
static TreeNode* GetValueExpr        (DArray* token_arr, Index* token_index);
static TreeNode* GetOrExpr           (DArray* token_arr, Index* token_index);
static TreeNode* GetAndExpr          (DArray* token_arr, Index* token_index);
static TreeNode* GetBitOrExpr        (DArray* token_arr, Index* token_index);
static TreeNode* GetBitXorExpr       (DArray* token_arr, Index* token_index);
static TreeNode* GetBitAndExpr       (DArray* token_arr, Index* token_index);
static TreeNode* GetEqualExpr        (DArray* token_arr, Index* token_index);
static TreeNode* GetOrderExpr        (DArray* token_arr, Index* token_index);
static TreeNode* GetBitShiftExpr     (DArray* token_arr, Index* token_index);
static TreeNode* GetPlusMinusExpr    (DArray* token_arr, Index* token_index);
static TreeNode* GetMultExpr         (DArray* token_arr, Index* token_index);
static TreeNode* GetBasicExpr        (DArray* token_arr, Index* token_index);
static TreeNode* GetFuncCall         (DArray* token_arr, Index* token_index);
static TreeNode* GetFuncCallParam    (DArray* token_arr, Index* token_index);
static TreeNode* GetFuncCallParamList(DArray* token_arr, Index* token_index);

//global-----------------------------------------------------------------------

AstError AstBuilder(DArray* token_arr, Tree* ast_tree) {
  ASSERT(token_arr != NULL);
  ASSERT(ast_tree != NULL);

  GetGrammar(token_arr, ast_tree);

  TreeDotDump(ast_tree, DumpAst);

  return kAstError_Success;
}

//static-----------------------------------------------------------------------

static void GetGrammar(DArray* token_arr, Tree* ast_tree) {
  ASSERT(token_arr != NULL);
  ASSERT(ast_tree != NULL);

  TreeNode* hold_nd = NULL;
  TreeNode* parent = &ast_tree->root;
  Index token_index = 0;

#define NodeJungling                                                          \
  if (hold_nd != NULL) {                                                      \
    parent->r_child = hold_nd;                                                \
    hold_nd->parent = parent;                                                 \
    parent = hold_nd;                                                         \
    continue;                                                                 \
  }
// end define

  do {
    hold_nd = GetImportStatement(token_arr, &token_index);
    NodeJungling;
    hold_nd = GetEnumDecl(token_arr, &token_index);
    NodeJungling;
    hold_nd = GetFuncDecl(token_arr, &token_index);
    NodeJungling;
    hold_nd = GetStatement(token_arr, &token_index);
    NodeJungling;
  } while (hold_nd != NULL && token_index < token_arr->n_elem);
}

static TreeNode* GetImportStatement(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(kw_import_tkn, token_arr, local_index,
       kw_import_tkn->type != kTokenType_KeyWord
    || kw_import_tkn->key_word != KW_IMPORT);

  CreateToken_(module_name_tkn, token_arr, local_index,
    module_name_tkn->type != kTokenType_Identifier);

  CreateNode_(kw_import_nd, kw_import_tkn, NULL, NULL);
  CreateNode_(module_name_nd, module_name_tkn, kw_import_nd, LEFT_SIDE_);

  *token_index = local_index;

  return kw_import_nd;
}

static TreeNode* GetEnumDecl(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(kw_enum_tkn, token_arr, local_index,
    kw_enum_tkn->type != kTokenType_KeyWord
    || kw_enum_tkn->key_word != KW_ENUM);

  CreateToken_(enum_name_tkn, token_arr, local_index,
    enum_name_tkn->type != kTokenType_Identifier);

  CreateToken_(punc_left_curl_br_tkn, token_arr, local_index,
    punc_left_curl_br_tkn->type != kTokenType_Punctuation
    || punc_left_curl_br_tkn->punc != PUNC_LEFT_CURL_BRACKET);

  GetNewNode_(enum_list, GetEnumList(token_arr, &local_index));

  CreateToken_(punc_right_curl_br_tkn, token_arr, local_index,
    punc_right_curl_br_tkn->type != kTokenType_Punctuation
    || punc_right_curl_br_tkn->punc != PUNC_RIGHT_CURL_BRACKET);

  CreateNode_(kw_enum_nd, kw_enum_tkn, NULL, NULL);
  CreateNode_(enum_name_nd, enum_name_tkn, kw_enum_nd, LEFT_SIDE_);

  ConnectPrntAndChld_(enum_name_nd, enum_list, LEFT_SIDE_);

  *token_index = local_index;

  return kw_enum_nd;
}

static TreeNode* GetEnumList(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  GetNewNode_(enum_item, GetEnumItem(token_arr, &local_index));

  CreateTokenNoCheck_(punc_comma_tkn, token_arr, local_index);
  if (punc_comma_tkn->type != kTokenType_Punctuation
      || punc_comma_tkn->punc != PUNC_COMMA) {
    *token_index = local_index;
    return enum_item;
  }
  local_index++;

  TreeNode* enum_list = GetEnumList(token_arr, &local_index);
  if (enum_list == NULL) {
    *token_index = local_index;
    return enum_item;
  }

  ConnectPrntAndChld_(enum_item, enum_list, LEFT_SIDE_);

  *token_index = local_index;

  return enum_item;
}

static TreeNode* GetEnumItem(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(enum_item_name_tkn, token_arr, local_index,
    enum_item_name_tkn->type != kTokenType_Identifier);

  CreateTokenNoCheck_(op_assign_tkn, token_arr, local_index);
  if (op_assign_tkn->type != kTokenType_Operator
      || op_assign_tkn->op != OP_ASSIGN) {
    *token_index = local_index;
    CreateNode_(enum_item_name_nd, enum_item_name_tkn, NULL, NULL);

    return enum_item_name_nd;
  }
  local_index++;

  GetNewNode_(cnst_value_nd, GetConstValue(token_arr, &local_index));

  CreateNode_(enum_item_name_nd, enum_item_name_tkn, NULL, NULL);

  ConnectPrntAndChld_(enum_item_name_nd, cnst_value_nd, RIGHT_SIDE_);

  *token_index = local_index;

  return enum_item_name_nd;
}

static TreeNode* GetFuncDecl(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(kw_fn_tkn, token_arr, local_index,
    kw_fn_tkn->type != kTokenType_KeyWord
    || kw_fn_tkn->key_word != KW_FN);
  CreateToken_(func_name_tkn, token_arr, local_index,
    func_name_tkn->type != kTokenType_Identifier);

  GetNewNode_(func_param_nd, GetFuncParam(token_arr, &local_index););

  CreateToken_(punc_colon_tkn, token_arr, local_index,
    punc_colon_tkn->type != kTokenType_Punctuation
    || punc_colon_tkn->punc != PUNC_COLON);
  GetNewNode_(type_spec_nd, GetTypeSpec(token_arr, &local_index));
  GetNewNode_(statement_nd, GetStatement(token_arr, &local_index));

  CreateNode_(kw_fn_nd, kw_fn_tkn, NULL, NULL);
  CreateNode_(func_name_nd, func_name_tkn, kw_fn_nd, LEFT_SIDE_);
  ConnectPrntAndChld_(func_name_nd, type_spec_nd, LEFT_SIDE_);
  ConnectPrntAndChld_(func_name_nd, func_param_nd, RIGHT_SIDE_);
  ConnectPrntAndChld_(kw_fn_nd, statement_nd, RIGHT_SIDE_);

  *token_index = local_index;

  return kw_fn_nd;
}

static TreeNode* GetFuncParam(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(punc_left_rnd_br_tkn, token_arr, local_index,
    punc_left_rnd_br_tkn->type != kTokenType_Punctuation
    || punc_left_rnd_br_tkn->punc != PUNC_LEFT_ROUND_BRACKET);

  CreateTokenNoCheck_(punc_right_rnd_br_try_tkn, token_arr, local_index);
  if (punc_right_rnd_br_try_tkn->type == kTokenType_Punctuation
      && punc_right_rnd_br_try_tkn->punc == PUNC_RIGHT_ROUND_BRACKET) {
    local_index++;

    Token kw_void_tkn = {.type = kTokenType_KeyWord, .key_word = KW_VOID};

    CreateNode_(kw_void_nd, &kw_void_tkn, NULL, NULL);

    *token_index = local_index;
    return kw_void_nd;
  }

  GetNewNode_(func_param_list_nd, GetFuncParamList(token_arr, &local_index));

  CreateToken_(punc_right_rnd_br_tkn, token_arr, local_index,
    punc_right_rnd_br_tkn->type != kTokenType_Punctuation
    || punc_right_rnd_br_tkn->punc != PUNC_RIGHT_ROUND_BRACKET);

  *token_index = local_index;

  return func_param_list_nd;
}

static TreeNode* GetFuncParamList(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  GetNewNode_(var_spec_nd, GetVarSpec(token_arr, &local_index));
  CreateTokenNoCheck_(punc_comma_tkn, token_arr, local_index);
  if (punc_comma_tkn->type != kTokenType_Punctuation
      || punc_comma_tkn->punc != PUNC_COMMA) {

    *token_index = local_index;
    return var_spec_nd;
  }
  local_index++;

  TreeNode* func_param_list_nd = GetFuncParamList(token_arr, &local_index);
  if (func_param_list_nd != NULL) {
    ConnectPrntAndChld_(var_spec_nd, func_param_list_nd, RIGHT_SIDE_);
  }

  *token_index = local_index;

  return var_spec_nd;
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

  *token_index = local_index;

  return var_spec_no_mut_nd;
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
               type_tkn->type != kTokenType_KeyWord
               && type_tkn->type != kTokenType_Identifier);

  if (type_tkn->type == kTokenType_Identifier) {
    CreateNode_(type_nd, type_tkn, NULL, NULL);

    *token_index = local_index;
    return type_nd;
  }

  switch (type_tkn->key_word) {
    case KW_I8:    case KW_U8:    case KW_I16: case KW_U16:
    case KW_I32:   case KW_U32:   case KW_I64: case KW_U64:
    case KW_ISIZE: case KW_USIZE: case KW_F32: case KW_F64:
    case KW_BOOL:  case KW_CHAR: {
      CreateNode_(type_nd, type_tkn, NULL, NULL);
      *token_index = local_index;
      return type_nd;
    }
    case KW_LET:    case KW_FN:     case KW_MUT:    case KW_ASM:
    case KW_IF:     case KW_ELSE:   case KW_BREAK:  case KW_CONTINUE:
    case KW_FALSE:  case KW_TRUE:   case KW_FOR:    case KW_WHILE:
    case KW_DO:     case KW_RETURN: case KW_IMPORT: case KW_ENUM:
    case KW_STRUCT: case KW_VOID:
    default:
      *token_index = local_index;
      return NULL;
  } //FIXME mb void is valid type?
}

static TreeNode* GetConstValue(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateTokenNoCheck_(cnst_try_tkn, token_arr, local_index);
  if (cnst_try_tkn->type == kTokenType_Const) {
    local_index++;
    *token_index = local_index;

    CreateNode_(cnst_try_nd, cnst_try_tkn, NULL, NULL);

    return cnst_try_nd;
  }

  CreateToken_(op_minus_tkn, token_arr, local_index,
    op_minus_tkn->type != kTokenType_Operator
    || op_minus_tkn->op != OP_MINUS);

  CreateToken_(cnst_tkn, token_arr, local_index,
    cnst_tkn->type != kTokenType_Const);

  Token new_cnst_tkn = *cnst_tkn;
  switch (new_cnst_tkn.cnst.type) {
    case kConstType_Int:
      new_cnst_tkn.cnst.int_cnst *= -1;
      break;
    case kConstType_Char:
      new_cnst_tkn.cnst.char_cnst *= -1;
      break;
    case kConstType_Double:
      new_cnst_tkn.cnst.dble_cnst *= -1;
      break;
    case kConstType_Uninit:
    default:
      ASSERT(0 && ":(");
      break;
  }

  CreateNode_(new_cnst_nd, &new_cnst_tkn, NULL, NULL);

  *token_index = local_index;

  return new_cnst_nd;
}

static TreeNode* GetStatement(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;
  CreateToken_(punc_left_curl_br_tkn, token_arr, local_index,
    punc_left_curl_br_tkn->type != kTokenType_Punctuation
    || punc_left_curl_br_tkn->punc != PUNC_LEFT_CURL_BRACKET);

  GetNewNode_(statement_list_nd, GetStatementList(token_arr, &local_index));
  CreateToken_(punc_right_curl_br_tkn, token_arr, local_index,
    punc_right_curl_br_tkn->type != kTokenType_Punctuation
    || punc_right_curl_br_tkn->punc != PUNC_RIGHT_CURL_BRACKET);

  *token_index = local_index;
  return statement_list_nd;
}

static TreeNode* GetStatementList(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  GetNewNode_(single_statement_nd, GetSingleStatement(token_arr, &local_index));

  CreateToken_(punc_semi_colon_tkn, token_arr, local_index,
    punc_semi_colon_tkn->type != kTokenType_Punctuation
    || punc_semi_colon_tkn->punc != PUNC_SEMICOLON);

  TreeNode* statement_list = GetStatementList(token_arr, &local_index);
  if (statement_list != NULL) {
    ConnectPrntAndChld_(single_statement_nd, statement_list, RIGHT_SIDE_);
  }

  *token_index = local_index;
  return single_statement_nd;
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

  ConnectPrntAndChld_(var_name_nd, assign_op_nd, LEFT_SIDE_);
  ConnectPrntAndChld_(assign_op_nd, value_expr_nd, LEFT_SIDE_);

  return var_name_nd;
}

static TreeNode* GetVarInit(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(kw_let_tkn, token_arr, local_index,
    kw_let_tkn->type != kTokenType_KeyWord
    || kw_let_tkn->key_word != KW_LET);

  GetNewNode_(var_spec_nd, GetVarSpec(token_arr, &local_index));

  CreateTokenNoCheck_(op_assign_try_tkn, token_arr, local_index);
  TreeNode* value_expr = NULL;
  if (op_assign_try_tkn->type == kTokenType_Operator
      && op_assign_try_tkn->op == OP_ASSIGN) {
    local_index++;
    value_expr = GetValueExpr(token_arr, &local_index);

    if (value_expr == NULL) { return NULL; }
  }

  CreateNode_(kw_let_nd, kw_let_tkn, NULL, NULL);

  ConnectPrntAndChld_(kw_let_nd, var_spec_nd, LEFT_SIDE_);

  if (value_expr != NULL) {
    ConnectPrntAndChld_(kw_let_nd, value_expr, RIGHT_SIDE_);
  }

  *token_index = local_index;
  return kw_let_nd;
}

static TreeNode* GetAssignOp(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(op_multy_assign_tkn, token_arr, local_index,
    op_multy_assign_tkn->type != kTokenType_Operator);

  switch (op_multy_assign_tkn->op) {
    case OP_ASSIGN:                 case OP_BIT_SHIFT_LEFT_ASSIGN:
    case OP_BIT_SHIFT_RIGHT_ASSIGN: case OP_PLUS_ASSIGN:
    case OP_MINUS_ASSIGN:           case OP_MULT_ASSIGN:
    case OP_DIV_ASSIGN:             case OP_REM_ASSIGN:
    case OP_BIT_OR_ASSIGN:          case OP_BIT_AND_ASSIGN:
    case OP_BIT_XOR_ASSIGN:
      CreateNode_(op_multy_assign_nd, op_multy_assign_tkn, NULL, NULL);
      *token_index = local_index;
      return op_multy_assign_nd;
    case OP_INC:             case OP_DEC:
    case OP_AND:             case OP_OR:
    case OP_BIT_SHIFT_RIGHT: case OP_BIT_SHIFT_LEFT:
    case OP_EQUAL:           case OP_NOT_EQUAL:
    case OP_LESS_OR_EQUAL:   case OP_GREATER_OR_EQUAL:
    case OP_GREATER:         case OP_LESS:
    case OP_PLUS:            case OP_MINUS:
    case OP_MULT:            case OP_DIV:
    case OP_BIT_OR:          case OP_BIT_AND:
    case OP_NOT:             case OP_BIT_NOT:
    case OP_REM:             case OP_BIT_XOR:
    default:
      return NULL;
  }
}

static TreeNode* GetValueExpr(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  return GetOrExpr(token_arr, token_index);
}

#define GenerateGetFunction(function_name_, left_func_name_, op_middle_token_name_, op_check_) \
  static TreeNode* function_name_(DArray* token_arr, Index* token_index) {    \
    ASSERT(token_arr != NULL);                                                \
    ASSERT(token_index != NULL);                                              \
                                                                              \
    Index local_index = *token_index;                                         \
                                                                              \
    GetNewNode_(left_node, left_func_name_(token_arr, &local_index));         \
                                                                              \
    CreateTokenNoCheck_(op_middle_token_name_, token_arr, local_index);       \
    if (op_middle_token_name_->type != kTokenType_Operator                    \
        || (op_check_)) {                                                     \
      *token_index = local_index;                                             \
      return left_node;                                                       \
    }                                                                         \
    local_index++;                                                            \
                                                                              \
    GetNewNode_(right_node, function_name_(token_arr, &local_index));         \
    CreateNode_(op_middle_node, op_middle_token_name_, NULL, NULL);           \
                                                                              \
    ConnectPrntAndChld_(op_middle_node, left_node, LEFT_SIDE_);               \
    ConnectPrntAndChld_(op_middle_node, right_node, RIGHT_SIDE_);             \
                                                                              \
    *token_index = local_index;                                               \
    return op_middle_node;                                                    \
  }
//end of def

GenerateGetFunction(GetOrExpr,        GetAndExpr,       op_or_tkn,
                    op_or_tkn->op != OP_OR);

GenerateGetFunction(GetAndExpr,       GetBitOrExpr,     op_and_tkn,
                    op_and_tkn->op != OP_AND);

GenerateGetFunction(GetBitOrExpr,     GetBitXorExpr,    op_bit_or_tkn,
                    op_bit_or_tkn->op != OP_BIT_OR);

GenerateGetFunction(GetBitXorExpr,    GetBitAndExpr,    op_bit_xor_tkn,
                    op_bit_xor_tkn->op != OP_BIT_XOR);

GenerateGetFunction(GetBitAndExpr,    GetEqualExpr,     op_bit_and_tkn,
                    op_bit_and_tkn->op != OP_BIT_AND);

GenerateGetFunction(GetEqualExpr,     GetOrderExpr,     op_equal_tkn,
                    op_equal_tkn->op != OP_EQUAL &&
                    op_equal_tkn->op != OP_NOT_EQUAL);

GenerateGetFunction(GetOrderExpr,     GetBitShiftExpr,  op_order_tkn,
                    op_order_tkn->op != OP_GREATER &&
                    op_order_tkn->op != OP_GREATER_OR_EQUAL &&
                    op_order_tkn->op != OP_LESS &&
                    op_order_tkn->op != OP_LESS_OR_EQUAL);

GenerateGetFunction(GetBitShiftExpr,  GetPlusMinusExpr, op_bit_shift_tkn,
                    op_bit_shift_tkn->op != OP_BIT_SHIFT_LEFT &&
                    op_bit_shift_tkn->op != OP_BIT_SHIFT_RIGHT);

GenerateGetFunction(GetPlusMinusExpr, GetMultExpr,      op_pm_tkn,
                    op_pm_tkn->op != OP_PLUS &&
                    op_pm_tkn->op != OP_MINUS);

GenerateGetFunction(GetMultExpr,      GetBasicExpr,     op_mult_tkn,
                    op_mult_tkn->op != OP_MULT &&
                    op_mult_tkn->op != OP_DIV &&
                    op_mult_tkn->op != OP_REM);

#undef GenerateGetFunction

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

static TreeNode* GetFuncCall(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(func_name_tkn, token_arr, local_index,
    func_name_tkn->type != kTokenType_Identifier);

  GetNewNode_(func_call_param_nd, GetFuncCallParam(token_arr, &local_index));
  CreateNode_(func_name_nd, func_name_tkn, NULL, NULL);
  ConnectPrntAndChld_(func_name_nd, func_call_param_nd, LEFT_SIDE_);

  *token_index = local_index;
  return func_name_nd;
}

static TreeNode* GetFuncCallParam(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  CreateToken_(punc_left_rnd_br_tkn, token_arr, local_index,
    punc_left_rnd_br_tkn->type != kTokenType_Punctuation
    || punc_left_rnd_br_tkn->punc != PUNC_LEFT_ROUND_BRACKET);
  CreateTokenNoCheck_(punc_right_rnd_br_try_tkn, token_arr, local_index);
  if (punc_right_rnd_br_try_tkn->type == kTokenType_Punctuation
      && punc_right_rnd_br_try_tkn->punc == PUNC_RIGHT_ROUND_BRACKET) {
    local_index++;

    Token void_tkn = {.type = kTokenType_KeyWord, .key_word = KW_VOID};

    CreateNode_(void_nd, &void_tkn, NULL, NULL);

    *token_index = local_index;
    return void_nd;
  }

  GetNewNode_(func_call_param_list_nd, GetFuncCallParamList(token_arr, &local_index));

  CreateToken_(punc_right_rnd_br_tkn, token_arr, local_index,
    punc_right_rnd_br_tkn->type != kTokenType_Punctuation
    || punc_right_rnd_br_tkn->punc != PUNC_RIGHT_ROUND_BRACKET);

  *token_index = local_index;
  return func_call_param_list_nd;
}

static TreeNode* GetFuncCallParamList(DArray* token_arr, Index* token_index) {
  ASSERT(token_arr != NULL);
  ASSERT(token_index != NULL);

  Index local_index = *token_index;

  GetNewNode_(value_expr_nd, GetValueExpr(token_arr, &local_index));

  CreateTokenNoCheck_(punc_comma_tkn, token_arr, local_index);

  TreeNode* func_param_list_nd = NULL;
  if (punc_comma_tkn->type == kTokenType_Punctuation
      && punc_comma_tkn->punc == PUNC_COMMA) {
    local_index++;
    func_param_list_nd = GetFuncCallParamList(token_arr, &local_index);
    if (func_param_list_nd == NULL) { return NULL; }

    ConnectPrntAndChld_(value_expr_nd, func_param_list_nd, RIGHT_SIDE_);
  }

  *token_index = local_index;
  return value_expr_nd;
}

//macro_undef------------------------------------------------------------------

#undef CreateNode_
#undef GetNewNode_
#undef CreateToken_
#undef CreateTokenNoCheck_
#undef LEFT_SIDE_
#undef RIGHT_SIDE_
