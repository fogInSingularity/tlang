#include "lexer.h"

#include <ctype.h>

#include "token_def.h"
#include "utils.h"

// static ----------------------------------------------------------------------

static const ErrorIndex kTokenise_Success = -1;
static const ErrorIndex kRemComms_Failure = -1;

typedef enum TokenState {
  kTokenState_Parsed = 0,
  kTokenState_Skip   = 1,
  kTokenState_Error  = 2,
} TokenState;

static ErrorCounter RemoveComments(BinData* data);
static ErrorIndex Tokenise(BinData* data, DArray* token_array);
static TokenState TokenKeyword(BinData* data, DArray* token_array,
                               Counter* shift, DebugInfo* debug);
static TokenState TokenIdentifier(BinData* data, DArray* token_array,
                                  Counter* shift, DebugInfo* debug);
static TokenState TokenOperator(BinData* data, DArray* token_array,
                                Counter* shift, DebugInfo* debug);
static TokenState TokenConst(BinData* data, DArray* token_array,
                             Counter* shift, DebugInfo* debug);
static TokenState TokenConstChar(BinData* data, DArray* token_array,
                                 Counter* shift, DebugInfo* debug);
static TokenState TokenConstNum(BinData* data, DArray* token_array,
                                Counter* shift, DebugInfo* debug);
static TokenState TokenStrLit(BinData* data, DArray* token_array,
                              Counter* shift, DebugInfo* debug);
static TokenState TokenPunctuation(BinData* data, DArray* token_array,
                                   Counter* shift, DebugInfo* debug);
static Counter SkipSpaces(BinData* data, Counter* shift, DebugInfo* debug);
void ArrDump(const void* elem);

// global ----------------------------------------------------------------------

LexicalError Lexer(BinData* data, DArray* token_array) {
  ASSERT(data != NULL);
  ASSERT(token_array != NULL);

  ErrorCounter cnt_rem = RemoveComments(data);
  if (cnt_rem == kRemComms_Failure) { return kLexicalError_Comments; }

  ErrorIndex err_ind = kTokenise_Success;
  err_ind = Tokenise(data, token_array);
  if (err_ind != kTokenise_Success) { return kLexicalError_UnknownLex; }

  return kLexicalError_Success;
}

// static ----------------------------------------------------------------------

static ErrorCounter RemoveComments(BinData* data) {
  ASSERT(data != NULL);

  ErrorCounter removed = 0;
  char prev_char = '\0';
  Counter n_double_quat = 0;
  Counter n_single_quat = 0;

  char* move_str = data->buf;
  while (move_str < data->buf + data->buf_size) {
    if (n_double_quat % 2 == 0 && n_single_quat % 2 == 0) { // if not str_lit
      if (*move_str == '/' && prev_char == '/') { // if comment
        *(move_str - 1) = ' '; // <  safe  ^
        removed++;

        while (*move_str != '\n'
               && *move_str != '\0'
               && move_str < data->buf + data->buf_size) {
          *move_str = ' ';
          removed++;

          move_str++;
        }
        // if (move_str < data->buf + data->buf_size) {
        //   *move_str = ' ';
        //   removed++;
        // }
        prev_char = ' ';
      } else if (isspace(*move_str)) {
        ;
      } else if (*move_str == '\"') {
        n_double_quat++;
      } else if (*move_str == '\'') {
        n_single_quat++;
      }
    } else if (n_double_quat % 2 == 1) { // str_lit ""
      if (*move_str == '\"' && prev_char != '\\') {
        n_double_quat++;
      }
    } else if (n_single_quat % 2 == 1) { // str_lit ''
      if (*move_str == '\'' && prev_char != '\\') {
        n_single_quat++;
      }
    } else {
      ASSERT(0 && "UNKNOWN CASE");
    }

    prev_char = *move_str;
    move_str++;
  }

  return removed;
}

static ErrorIndex Tokenise(BinData* data, DArray* token_array) {
  ASSERT(data != NULL);
  ASSERT(token_array != NULL);

  Counter shift = 0;
  DebugInfo debug = {.line = 1, .symbol = 1};
  TokenState state = false;
  while (shift < data->buf_size) {
    SkipSpaces(data, &shift, &debug);

    state = TokenKeyword(data, token_array, &shift, &debug);
    if (state == kTokenState_Parsed) { continue; }

    state = TokenIdentifier(data, token_array, &shift, &debug);
    if (state == kTokenState_Parsed) { continue; }

    state = TokenOperator(data, token_array, &shift, &debug);
    if (state == kTokenState_Parsed) { continue; }

    state = TokenConst(data, token_array, &shift, &debug);
    if (state == kTokenState_Parsed) { continue; }

    state = TokenStrLit(data, token_array, &shift, &debug);
    if (state == kTokenState_Parsed) { continue; }

    state = TokenPunctuation(data, token_array, &shift, &debug);
    if (state == kTokenState_Parsed) { continue; }
  }

  DArray_Dump(token_array, ArrDump);

  return kTokenise_Success;
}

static TokenState TokenKeyword(BinData* data, DArray* token_array,
                               Counter* shift, DebugInfo* debug) {
  ASSERT(data != NULL);
  ASSERT(token_array != NULL);
  ASSERT(shift != NULL);
  ASSERT(debug != NULL);

  Token token = {};
  token.type = kTokenType_KeyWord;
  token.debug = *debug;

#define KEY_WORD(kw_id_, kw_, enum_name_) \
  if (strncmp(#kw_, data->buf + *shift, strlen(#kw_)) == 0) { \
    token.key_word = enum_name_; \
    DArray_PushBack(token_array, &token); \
    *shift += strlen(#kw_); \
    debug->symbol += strlen(#kw_); \
    \
    return kTokenState_Parsed; \
  } else

#include "tlang_key_words.h"

  /* else */ {
    return kTokenState_Skip;
  }

#undef KEY_WORD
}

static TokenState TokenIdentifier(BinData* data, DArray* token_array,
                                  Counter* shift, DebugInfo* debug) {
  ASSERT(data != NULL);
  ASSERT(token_array != NULL);
  ASSERT(shift != NULL);
  ASSERT(debug != NULL);

  const char* move_str = data->buf + *shift;
  if (!isalpha(*move_str)) {
    return kTokenState_Skip;
  }

  Token token = {};
  token.type = kTokenType_Identifier;
  token.idnt.str = data->buf + *shift;
  token.debug = *debug;

  while (move_str < data->buf + data->buf_size
         && (isalnum(*move_str) || *move_str == '_')) {
    move_str++;
  }
  token.idnt.len = (size_t)(move_str - (data->buf + *shift));
  DArray_PushBack(token_array, &token);
  *shift += token.idnt.len;
  debug->symbol += token.idnt.len;

  return kTokenState_Parsed;
}

static TokenState TokenOperator(BinData* data, DArray* token_array,
                                Counter* shift, DebugInfo* debug) {
  ASSERT(data != NULL);
  ASSERT(token_array != NULL);
  ASSERT(shift != NULL);
  ASSERT(debug != NULL);

  Token token = {};
  token.type = kTokenType_Operator;
  token.debug = *debug;

#define OPERATOR(op_id_, op_, enum_name_) \
  if (strncmp(op_, data->buf + *shift, strlen(op_)) == 0) { \
    token.op = enum_name_; \
    DArray_PushBack(token_array, &token); \
    *shift += strlen(op_); \
    debug->symbol += strlen(op_); \
    \
    return kTokenState_Parsed; \
  } else

#include "tlang_operators.h"

  /* else */ {
    return kTokenState_Skip;
  }

#undef OPERATOR
}

//NOTE
static TokenState TokenConst(BinData* data, DArray* token_array,
                             Counter* shift, DebugInfo* debug) {
  ASSERT(data != NULL);
  ASSERT(token_array != NULL);
  ASSERT(shift != NULL);
  ASSERT(debug != NULL);

  const char* move_str = data->buf + *shift;

  if (*move_str == '\'') {
    return TokenConstChar(data, token_array, shift, debug);
  } else if (isdigit(*move_str)) {
    return TokenConstNum(data, token_array, shift, debug);
  } else {
    return kTokenState_Skip;
  }
}

static TokenState TokenConstChar(BinData* data, DArray* token_array,
                                 Counter* shift, DebugInfo* debug) {
  ASSERT(data != NULL);
  ASSERT(token_array != NULL);
  ASSERT(shift != NULL);
  ASSERT(debug != NULL);

  const char* move_str = data->buf + *shift;
  ConstValue cnst = {};
  cnst.type = kConstType_Char;

  move_str++;
  if (*move_str == '\\') {
    move_str++;
    switch (*move_str) {
      case '0':
        cnst.char_cnst = '\0';
        break;
      case 'a':
        cnst.char_cnst = '\a';
        break;
      case 'b':
        cnst.char_cnst = '\b';
        break;
      case 't':
        cnst.char_cnst = '\t';
        break;
      case 'n':
        cnst.char_cnst = '\n';
        break;
      case 'v':
        cnst.char_cnst = '\v';
        break;
      case 'f':
        cnst.char_cnst = '\f';
        break;
      case 'r':
        cnst.char_cnst = '\r';
        break;
      default:
        return kTokenState_Skip;
        break;
    }
  } else {
    cnst.char_cnst = *move_str;
  }
  move_str++;
  if (*move_str != '\'') {
    return kTokenState_Skip;
  }
  move_str++;

  Token token = {};
  token.type = kTokenType_Const;
  token.cnst = cnst;
  token.debug = *debug;
  DArray_PushBack(token_array, &token);

  *shift += (size_t)(move_str - (data->buf + *shift));
  debug->symbol += (size_t)(move_str - (data->buf + *shift));

  return kTokenState_Parsed;
}

static TokenState TokenConstNum(BinData* data, DArray* token_array,
                                Counter* shift, DebugInfo* debug) {
  ASSERT(data != NULL);
  ASSERT(token_array != NULL);
  ASSERT(shift != NULL);
  ASSERT(debug != NULL);

  const char* move_str = data->buf + *shift;
  ConstValue cnst = {};
  cnst.type = kConstType_Int;
  cnst.int_cnst = 0;

  if (!isdigit(*move_str)) {
    return kTokenState_Skip;
  }

  while (move_str < data->buf + data->buf_size
         && isdigit(*move_str)) {
    cnst.int_cnst = cnst.int_cnst * 10 + (ssize_t)(*move_str - '0');
    move_str++;
  }

  if (*move_str == '.') {
    move_str++;

    cnst.type = kConstType_Double;
    cnst.dble_cnst = (double)cnst.int_cnst;

    double div = 10.0;

    while (move_str < data->buf + data->buf_size
         && isdigit(*move_str)) {
      cnst.dble_cnst += (*move_str - '0') / div;
      div *= 10.0;
      move_str++;
    }
  }

  Token token = {};
  token.type = kTokenType_Const;
  token.cnst = cnst;
  token.debug = *debug;
  DArray_PushBack(token_array, &token);

  *shift += (size_t)(move_str - (data->buf + *shift));
  debug->symbol += (size_t)(move_str - (data->buf + *shift));

  return kTokenState_Parsed;
}

static TokenState TokenStrLit(BinData* data, DArray* token_array,
                              Counter* shift, DebugInfo* debug) {
  ASSERT(data != NULL);
  ASSERT(token_array != NULL);
  ASSERT(shift != NULL);
  ASSERT(debug != NULL);

  const char* move_str = data->buf + *shift;
  if (*move_str != '\"') {
    return kTokenState_Skip;
  }
  move_str++;

  Token token = {};
  token.type = kTokenType_StringLit;
  token.str_lit.str = move_str;
  token.debug = *debug;

  char prev_char = ' ';
  while (move_str < data->buf + data->buf_size) {
    if (prev_char != '\\' && *move_str == '\"') {
      move_str++;
      break;
    } else if (prev_char == '\\' && *move_str == '\\') {
      prev_char = ' ';
    } else {
      prev_char = *move_str;
    }

    move_str++;
  }

  token.str_lit.len = (size_t)(move_str - (data->buf + *shift) - 2);
  DArray_PushBack(token_array, &token);
  *shift += (size_t)(move_str - (data->buf + *shift));
  debug->symbol += (size_t)(move_str - (data->buf + *shift));

  return kTokenState_Parsed;
}

static TokenState TokenPunctuation(BinData* data, DArray* token_array,
                                   Counter* shift, DebugInfo* debug) {
  ASSERT(data != NULL);
  ASSERT(token_array != NULL);
  ASSERT(shift != NULL);
  ASSERT(debug != NULL);

  Token token = {};
  token.type = kTokenType_Punctuation;
  token.debug = *debug;

#define PUNCTUATION(punc_id_, punc_, enum_name_) \
  if (strncmp(punc_, data->buf + *shift, strlen(punc_)) == 0) { \
    token.punc = enum_name_; \
    DArray_PushBack(token_array, &token); \
    *shift += strlen(punc_); \
    debug->symbol += strlen(punc_); \
    \
    return kTokenState_Parsed; \
  } else

#include "tlang_punctuation.h"

  /* else */ {
    return kTokenState_Skip;
  }

#undef PUNCTUATION
}

static Counter SkipSpaces(BinData* data,
                          Counter* shift,
                          DebugInfo* debug) {
  ASSERT(data != NULL);
  ASSERT(shift != NULL);
  ASSERT(debug != NULL);

  while (*shift < data->buf_size
         && isspace(*(data->buf + *shift))) {
    if (*(data->buf + *shift) == '\n') {
      debug->line++;
      debug->symbol = 1;
    } else {
      debug->symbol++;
    }
    (*shift)++;
  }

  return 0;
}

void ArrDump(const void* elem) {
  ASSERT(elem != NULL);

#if !defined(DEBUG)
  return ;
#endif // DEBUG

  const Token* token = elem;

  fprintf(stderr, "[ type %u ]", token->type);
  fprintf(stderr, "[ line %lu, symbol %lu ]", token->debug.line, token->debug.symbol);
  switch (token->type) {
    case kTokenType_KeyWord:
      fprintf(stderr, "[ %u ]", token->key_word);
      break;
    case kTokenType_Identifier:
      fprintf(stderr, "[ ");
      Putns(stderr, token->idnt.str, token->idnt.len);
      fprintf(stderr, " ]");
      break;
    case kTokenType_Operator:
      fprintf(stderr, "[ %u ]", token->op);
      break;
    case kTokenType_Const:
      if (token->cnst.type == kConstType_Int) {
        fprintf(stderr, "[ %ld ]", token->cnst.int_cnst);
      } else if (token->cnst.type == kConstType_Double) {
        fprintf(stderr, "[ %lf ]", token->cnst.dble_cnst);
      } else if (token->cnst.type == kConstType_Char) {
        fprintf(stderr, "[ \\%d ]", token->cnst.char_cnst);
      } else {
        ASSERT(0 && ":(");
      }
      break;
    case kTokenType_StringLit:
      fprintf(stderr, "[ ");
      Putns(stderr, token->str_lit.str, token->str_lit.len);
      fprintf(stderr, " ]");
      break;
    case kTokenType_Punctuation:
      fprintf(stderr, "[ %u ]", token->punc);
      break;
    case kTokenType_Uninit:
    default:
      ASSERT(0 && "FUCK ME");
      break;
  }
}
