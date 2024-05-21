#include "tree.h"

#include <stdlib.h>
#include <memory.h>

#include "my_assert.h"
#include "file_wraper.h"
#include "alloc_wraper.h"

//static-----------------------------------------------------------------------

static void TreeDumpNode(const TreeNode* node,
                         const TreeNode* root,
                         FILE* dump_file,
                         DumpTreeNodeFunc* DumpNode);

//global-----------------------------------------------------------------------

void TreeCtor(Tree* tree, bool do_dump, const char* dump_filename) {
  ASSERT(tree != NULL);
  ASSERT(dump_filename != NULL);

  tree->root.l_child = NULL;
  tree->root.r_child = NULL;
  tree->root.parent = NULL;
  tree->do_dump = do_dump;
  tree->dump_filename = dump_filename;
}

void TreeDtor(Tree* tree) {
  ASSERT(tree != NULL);

  TreeDtorNode(tree->root.l_child);
  TreeDtorNode(tree->root.r_child);
}

void TreeDotDump(const Tree* tree, DumpTreeNodeFunc* DumpNode) {
  if (!tree->do_dump) { return ; }

  FILE* dump_file = F_OPEN_W(tree->dump_filename, "w");
  if (dump_file == NULL) { return; }

  fprintf(dump_file, "digraph {\n");
  TreeDumpNode(tree->root.r_child, &tree->root, dump_file, DumpNode);
  fprintf(dump_file, "}\n");

  F_CLOSE_W(dump_file);
}

TreeNode* TreeCtorNode(TreeElem* data, TreeNode* parent) {
  ASSERT(data != NULL);
  // ASSERT(parent != NULL);

  TreeNode* new_node = (TreeNode*)CALLOCW(1, sizeof(TreeNode));
  if (new_node == NULL) { return NULL; }

  memcpy(&new_node->data, data, sizeof(TreeElem));

  new_node->parent = parent;

  return new_node;
}

Counter TreeDtorNode(TreeNode* node) {
  if (node == NULL) { return 0; }

  Counter cnt = 0;

  cnt += TreeDtorNode(node->l_child);
  cnt += TreeDtorNode(node->r_child);
  FREEW(node);

  return cnt + 1;
}

//static-----------------------------------------------------------------------

static void TreeDumpNode(const TreeNode* node,
                         const TreeNode* root,
                         FILE* dump_file,
                         DumpTreeNodeFunc* DumpNode) {
  ASSERT(dump_file != NULL);
  ASSERT(DumpNode != NULL);

  if (node == NULL) { return; }

  DumpNode(node, root, dump_file);

  TreeDumpNode(node->l_child, root, dump_file, DumpNode);
  TreeDumpNode(node->r_child, root, dump_file, DumpNode);
}
