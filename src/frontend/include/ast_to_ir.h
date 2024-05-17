#ifndef AST_TO_IR_H_
#define AST_TO_IR_H_

#include "tlang_ir.h"
#include "tree.h"
 
IR* TranslateAstToIr(Tree* ast);

#endif // AST_TO_IR_H_