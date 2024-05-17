#ifndef FRONTEND_H_
#define FRONTEND_H_

#include <stdbool.h>

#include "compiler_runtime_conf.h"
#include "darray.h"
#include "bin_file.h"
#include "tree.h"
#include "tlang_ir.h"

typedef struct Frontend {
  bool is_valid;
  BinData source_data;
  DArray token_array;
  Tree ast;
  // ast dump:
  bool output_ast_dot;
  const char* ast_out_dot_filename;
} Frontend;

typedef enum {
  kFrontError_Success            = 0,
  kFrontError_CtorBadAlloc       = 1,
  kFrontError_CantOpenSourceFile = 2,
  kFrontError_BadDArrayCtor      = 3,
  kFrontError_BadLexAnalyse      = 4,
  kFrontError_BadAst             = 5,
  kFrontError_InvalidFront       = 6,
} FrontError;

FrontError FrontCtor(Frontend* front, const CompilerRuntimeConfig* config);
void FrontDtor(Frontend* front);
void FrontThrowError(FrontError error);

FrontError FrontPass(Frontend* front, IR* ir);

#endif // FRONTEND_H_
