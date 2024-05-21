#include "ast_dump.h"

void DumpAst(const TreeNode* node, const TreeNode* root, FILE* dump_file) {
  ASSERT(dump_file != NULL);

  if (node == NULL) { return; }

  switch (node->data.type) {
    //----------------------------------------------------------
    case kTokenType_KeyWord:
      fprintf(dump_file,
              "node_%lu [shape = box, style = filled, fillcolor = \"#08d9d6\", label = \" type: keyword | ",
              (size_t)node);

      #define KEY_WORD(kw_id_, kw_, enum_name_) \
        case enum_name_: \
          fprintf(dump_file, "%s", #kw_); \
          break;

      switch (node->data.key_word) {
        #include "tlang_key_words.h"
        default:
          ASSERT(0 && ":(");
          break;
      }

      #undef KEY_WORD

      fprintf(dump_file, " \"];\n");
      break;
    //----------------------------------------------------------
    case kTokenType_Identifier:
      fprintf(dump_file,
              "node_%lu [shape = box, style = filled, fillcolor = \"#ff2e63\", label = \" type: identifier | ",
              (size_t)node);
      Putns(dump_file, node->data.idnt.str, node->data.idnt.len);
      fprintf(dump_file, " \"];\n");
      break;
    //----------------------------------------------------------
    case kTokenType_Const:
      fprintf(dump_file,
              "node_%lu [shape = box, style = filled, fillcolor = \"#6c5b7b\", label = \" type: const | %lX \"];\n",
              (size_t)node, (size_t)node->data.cnst.int_cnst);
      break;
    //----------------------------------------------------------
    case kTokenType_Operator:
      fprintf(dump_file,
              "node_%lu [shape = box, style = filled, fillcolor = \"#9dc08b\", label = \" type: operator | ",
              (size_t)node);

      #define OPERATOR(op_id_, op_, enum_name_) \
        case enum_name_: \
          fprintf(dump_file, "%s", op_); \
          break;

      switch (node->data.op) {
        #include "tlang_operators.h"
        default:
          ASSERT(0 && ":(");
          break;
      }

      #undef OPERATOR

      fprintf(dump_file, " \"];\n");
      break;
    //----------------------------------------------------------
    case kTokenType_Punctuation:
      fprintf(dump_file,
              "node_%lu [shape = box, style = filled, fillcolor = \"#3d84a8\", label = \" type: punc | ",
              (size_t)node);

      #define PUNCTUATION(punc_id, punc_, enum_name_) \
        case enum_name_: \
          fprintf(dump_file, "%s", punc_); \
          break;

      switch (node->data.punc) {
        #include "tlang_punctuation.h"
        default:
          ASSERT(0 && ":(");
          break;
      }

      #undef PUNCTUATION

      fprintf(dump_file, " \"];\n");
      break;
    //----------------------------------------------------------
    case kTokenType_StringLit:
      fprintf(dump_file,
              "node_%lu [shape = box, style = filled, fillcolor = \"#1fab89\", label = \" type: str_lit | ",
              (size_t)node);
      Putns(dump_file, node->data.str_lit.str, node->data.str_lit.len);
      fprintf(dump_file, " \"];\n");
      break;
    //----------------------------------------------------------
    case kTokenType_TreeSup:
      const char* tree_sup_option = NULL;
      switch (node->data.tree_sup) {
        case kTreeSup_Uninit:
          tree_sup_option = STRINGIFY(kTreeSup_Uninit);
          break;
        case kTreeSup_VarSpec:
          tree_sup_option = STRINGIFY(kTreeSup_VarSpec);
          break;
        case kTreeSup_VarInit:
          tree_sup_option = STRINGIFY(kTreeSup_VarInit);
          break;
        case kTreeSup_AssignExpr:
          tree_sup_option = STRINGIFY(kTreeSup_AssignExpr);
          break;
        case kTreeSup_IfBranch:
          tree_sup_option = STRINGIFY(kTreeSup_IfBranch);
          break;
        case kTreeSup_ElseIfBranch:
          tree_sup_option = STRINGIFY(kTreeSup_ElseIfBranch);
          break;
        case kTreeSup_ElseStBranch:
          tree_sup_option = STRINGIFY(kTreeSup_ElseStBranch);
          break;
        case kTreeSup_WhileLoop:
          tree_sup_option = STRINGIFY(kTreeSup_WhileLoop);
          break;
        case kTreeSup_ValueExpr:
          tree_sup_option = STRINGIFY(kTreeSup_ValueExpr);
          break;
        case kTreeSup_Statement:
          tree_sup_option = STRINGIFY(kTreeSup_Statement);
          break;
        case kTreeSup_FunctionDef:
          tree_sup_option = STRINGIFY(kTreeSup_FunctionDef);
          break;
        case kTreeSup_ConnectIfElse:
          tree_sup_option = STRINGIFY(kTreeSup_ConnectIfElse);
          break;
        case kTreeSup_ConnectNoElse:
          tree_sup_option = STRINGIFY(kTreeSup_ConnectNoElse);
          break;
        case kTreeSup_FunctionCall:
          tree_sup_option = STRINGIFY(kTreeSup_FunctionCall);
          break;
        case kTreeSup_FunctionNoParam:
          tree_sup_option = STRINGIFY(kTreeSup_FunctionNoParam);
          break;
        case kTreeSup_FunctionParamList:
          tree_sup_option = STRINGIFY(kTreeSup_FunctionParamList);
          break;
        case kTreeSup_ReturnExpr:
          tree_sup_option = STRINGIFY(kTreeSup_ReturnExpr);
          break;
        default:
          PRINT_UINT(node->data.tree_sup);
          ASSERT(0 && ":(");
      }
      fprintf(dump_file,
              "node_%lu [shape = box, style = filled, fillcolor = \"#d9edbf\", label = \" type: tree_sup | %s\"];\n",
              (size_t)node, tree_sup_option); 
      break;
    //----------------------------------------------------------
    case kTokenType_Uninit:
    default:
      ASSERT(0 && ":(");
      break;
  }

  if (node->l_child != NULL) {
    fprintf(dump_file, "node_%lu->node_%lu\n [color=blue]", (size_t)node, (size_t)node->l_child);
  }
  if (node->r_child != NULL) {
    fprintf(dump_file, "node_%lu->node_%lu [color=red]\n", (size_t)node, (size_t)node->r_child);
  }
  if (node->parent != NULL && node->parent != root) {
    fprintf(dump_file, "node_%lu->node_%lu [style=dotted]\n", (size_t)node, (size_t)node->parent);
  }
}
