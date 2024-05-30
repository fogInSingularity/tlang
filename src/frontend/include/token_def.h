#ifndef TOKENDEF_H_
#define TOKENDEF_H_

#include "string_view.h"
#include "my_typedefs.h"

typedef enum TokenType {
  kTokenType_Uninit      = 0,
  kTokenType_KeyWord     = 1,
  kTokenType_Identifier  = 2,
  kTokenType_Const       = 3,
  kTokenType_StringLit   = 4,
  kTokenType_Operator    = 5,
  kTokenType_Punctuation = 6,
  kTokenType_TreeSup     = 7,
} TokenType;

typedef enum ConstType {
  kConstType_Uninit = 0,
  kConstType_Double = 1,
  kConstType_Int    = 2,
  kConstType_Char   = 3,
} ConstType;

typedef struct ConstValue {
  ConstType type;
  union {
    double  dble_cnst;
    ssize_t int_cnst;
    char    char_cnst;
  };
} ConstValue;

typedef StringView Identifier;
typedef StringView StringLit;

typedef struct DebugInfo {
  Index line;
  Index symbol;
} DebugInfo;

typedef enum KeyWord {
#define KEY_WORD(kw_id_, kw_, enum_name_) \
  enum_name_ = kw_id_,

#include "tlang_key_words.inc"

#undef KEY_WORD
} KeyWord;

typedef enum Operator {
#define OPERATOR(op_id_, op_, enum_name_) \
  enum_name_ = op_id_,

#include "tlang_operators.inc"

#undef OPERATOR
} Operator;

typedef enum Punctuation {
#define PUNCTUATION(punc_id_, punc_, enum_name_) \
  enum_name_ = punc_id_,

#include "tlang_punctuation.inc"

#undef PUNCTUATION
} Punctuation;

typedef enum TreeSup {
  kTreeSup_Uninit            = 0,

  kTreeSup_VarSpec           = 1,
  kTreeSup_VarInit           = 2,
  kTreeSup_AssignExpr        = 3,
  kTreeSup_IfBranch          = 4,
  kTreeSup_ElseStBranch      = 5,
  kTreeSup_ElseIfBranch      = 6,
  kTreeSup_WhileLoop         = 7,
  kTreeSup_ValueExpr         = 8,
  kTreeSup_Statement         = 9,
  kTreeSup_ConnectIfElse     = 10,
  kTreeSup_ConnectNoElse     = 11,
  kTreeSup_FunctionCall      = 12,
  kTreeSup_FunctionDef       = 13,
  kTreeSup_FunctionNoParam   = 14,
  kTreeSup_FunctionParamList = 15,
  kTreeSup_ReturnExpr        = 16,
} TreeSup;

typedef struct Token {
  TokenType type;
  DebugInfo debug;
  union {
    KeyWord     key_word;
    Identifier  idnt;
    ConstValue  cnst;
    StringLit   str_lit;
    Operator    op;
    Punctuation punc;
    TreeSup     tree_sup;
  };
} Token;

#endif // TOKENDEF_H_
