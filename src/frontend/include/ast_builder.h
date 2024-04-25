#ifndef ASTBUILDER_H_
#define ASTBUILDER_H_

#include "tree.h"
#include "darray.h"
#include "token_def.h"
#include "logging.h"
#include "utils.h"
#include "ast_dump.h"

typedef enum AstError {
  kAstError_Success = 0,
} AstError;

AstError AstBuilder(DArray* token_arr, Tree* ast_tree);

#endif // ASTBUILDER_H_
