#ifndef ASTDUMP_H_
#define ASTDUMP_H_

#include "tree.h"
#include "darray.h"
#include "utils.h"

void DumpAst(const TreeNode* node, const TreeNode* root, FILE* dump_file);

#endif // ASTDUMP_H_
