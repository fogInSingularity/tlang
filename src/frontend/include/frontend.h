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
  kFrontendError_Success            = 0,
  kFrontendError_CtorBadAlloc       = 1,
  kFrontendError_CantOpenSourceFile = 2,
  kFrontendError_BadDArrayCtor      = 3,
  kFrontendError_BadLexAnalyse      = 4,
  kFrontendError_BadAst             = 5,
  kFrontendError_InvalidFront       = 6,
} FrontendError;

FrontendError Frontend_Ctor(Frontend* front, const CompilerRuntimeConfig* config);
void Frontend_Dtor(Frontend* front);
void Frontend_ThrowError(FrontendError error);

FrontendError Frontend_Pass(Frontend* front, IR* ir);

#endif // FRONTEND_H_
