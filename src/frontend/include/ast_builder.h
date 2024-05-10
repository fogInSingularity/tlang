#ifndef ASTBUILDER_H_
#define ASTBUILDER_H_

#include "tree.h"
#include "darray.h"

typedef enum AstError {
  kAstError_Success = 0,
} AstError;

AstError AstBuilder(DArray* token_arr, Tree* ast_tree);

#endif // ASTBUILDER_H_
