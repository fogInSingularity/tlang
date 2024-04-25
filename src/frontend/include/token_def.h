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
    double dble_cnst;
    ssize_t int_cnst;
    char char_cnst;
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

#include "tlang_key_words.h"

#undef KEY_WORD
} KeyWord;

typedef enum Operator {
#define OPERATOR(op_id_, op_, enum_name_) \
  enum_name_ = op_id_,

#include "tlang_operators.h"

#undef OPERATOR
} Operator;

typedef enum Punctuation {
#define PUNCTUATION(punc_id_, punc_, enum_name_) \
  enum_name_ = punc_id_,

#include "tlang_punctuation.h"

#undef PUNCTUATION
} Punctuation;

typedef struct Token {
  TokenType type;
    KeyWord key_word;
  union {
    Identifier idnt;
    ConstValue cnst;
    StringLit str_lit;
    Operator op;
    Punctuation punc;
  };
  DebugInfo debug;
} Token;

#endif // TOKENDEF_H_
