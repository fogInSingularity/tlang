#ifndef TREE_H_
#define TREE_H_

#include <stdio.h>

#include "my_typedefs.h"
#include "token_def.h"

typedef Token TreeElem;

typedef struct TreeNode {
  TreeElem data;
  struct TreeNode* l_child;
  struct TreeNode* r_child;
  struct TreeNode* parent;
} TreeNode;

typedef struct Tree {
  TreeNode root;

  bool do_dump;
  const char* dump_filename;
} Tree;

typedef enum TreeError {
  kTreeError_Success = 0,
} TreeError;

typedef void DumpTreeNodeFunc(const TreeNode* node,
                              const TreeNode* root,
                              FILE* dump_file);

void TreeCtor(Tree* tree, bool do_dump, const char* dump_filename);
void TreeDtor(Tree* tree);
TreeNode* TreeCtorNode(TreeElem* data, TreeNode* parent);
Counter TreeDtorNode(TreeNode* node);
void TreeDotDump(const Tree* tree, DumpTreeNodeFunc* DumpNode);

#endif // TREE_H_
