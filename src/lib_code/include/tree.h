#ifndef TREE_H_
#define TREE_H_

#include "my_typedefs.h"
#include "token_def.h"
#include "debug.h"

typedef Token TreeElem;

typedef struct TreeNode {
  TreeElem data;
  struct TreeNode* l_child;
  struct TreeNode* r_child;
  struct TreeNode* parent;
} TreeNode;

typedef struct Tree {
  TreeNode root;
} Tree;

typedef enum TreeError {
  kTreeError_Success = 0,
} TreeError;

typedef void DumpTreeNodeFunc(const TreeNode* node,
                              const TreeNode* root,
                              FILE* dump_file);

void TreeCtor(Tree* tree);
void TreeDtor(Tree* tree);
TreeNode* TreeCtorNode(TreeElem* data, TreeNode* parent);
Counter TreeDtorNode(TreeNode* node);
void TreeDotDump(const Tree* tree, DumpTreeNodeFunc* DumpNode);

#endif // TREE_H_
