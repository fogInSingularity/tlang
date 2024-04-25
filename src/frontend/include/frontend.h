#ifndef FRONTEND_H_
#define FRONTEND_H_

#include <stdbool.h>

#include "my_assert.h"
#include "my_typedefs.h"
#include "debug.h"
#include "darray.h"
#include "bin_file.h"
#include "lexer.h"
#include "ast_builder.h"

typedef struct Frontend {
  BinData source_data;
  DArray token_array;
  Tree ast;
} Frontend;

typedef enum {
  kFrontError_Success            = 0,
  kFrontError_CtorBadAlloc       = 1,
  kFrontError_CantOpenSourceFile = 2,
  kFrontError_BadDArrayCtor      = 3,
  kFrontError_BadLexAnalyse      = 4,
} FrontError;

FrontError FrontCtor(Frontend* front,
                     const char* source_file,
                     const char* target_file);
void FrontDtor(Frontend* front);

FrontError FrontPass(Frontend* front);

#endif // FRONTEND_H_
