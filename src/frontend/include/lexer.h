#ifndef LEXER_H_
#define LEXER_H_

#include <ctype.h>

#include "token_def.h"
#include "darray.h"
#include "bin_file.h"
#include "utils.h"

typedef enum LexicalError {
  kLexicalError_Success = 0,
  kLexicalError_Comments = 1,
  kLexicalError_UnknownLex = 2,
} LexicalError;

LexicalError Lexer(BinData* data, DArray* token_array);

#endif // LEXER_H_

