#ifndef FRONTEND_H_
#define FRONTEND_H_

#include <stdbool.h>

#include "darray.h"
#include "bin_file.h"
#include "tree.h"
#include "tlang_ir.h"

typedef struct Frontend {
  bool is_valid;
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
  kFrontError_BadAst             = 5,
} FrontError;

FrontError FrontCtor(Frontend* front,
                     const char* source_file,
                     const char* target_file);
void FrontDtor(Frontend* front);

FrontError FrontPass(Frontend* front, IR* ir_out);

#endif // FRONTEND_H_
