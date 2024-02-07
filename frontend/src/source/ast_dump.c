#include "ast_dump.h"

void DumpAst(const TreeNode* node, FILE* dump_file) {
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
  if (node->parent != NULL) {
    fprintf(dump_file, "node_%lu->node_%lu [style=dotted]\n", (size_t)node, (size_t)node->parent);
  }
}
