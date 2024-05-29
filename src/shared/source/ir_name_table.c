#include "ir_name_table.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <alloca.h>
#include <stdio.h>

#include "debug.h"
#include "my_assert.h"
#include "ir_def.h"
#include "list.h"

// structs ---------------------------------------------------------------------

typedef struct IRNameTable {
  List* name_list;
} IRNameTable;

// static ----------------------------------------------------------------------

const char* IROperandTypeToStr(IROperandType ir_type);

// global ----------------------------------------------------------------------

IRNameTable* IRNameTable_Ctor() {
  IRNameTable* new_nt = calloc(1, sizeof(IRNameTable));
  if (new_nt == NULL) { return NULL; }

  new_nt->name_list = List_Ctor(sizeof(IRName));
  return new_nt;
}

void IRNameTable_Dtor(IRNameTable* name_table) {
  ASSERT(name_table != NULL);

  ListNode* iter_list = List_FirstNode(name_table->name_list);
  while (iter_list != NULL) {
    IRName* ir_name = List_AccessData(iter_list);
    free(ir_name->name_str);

    iter_list = List_NextNode(name_table->name_list, iter_list);
  }

  List_DtorFull(name_table->name_list);
  name_table->name_list = NULL;
}

void IRNameTable_Dump(IRNameTable* name_table) {
  ASSERT(name_table != NULL);

#if !defined (DEBUG)
  return ;
#endif // DEBUG

  int64_t n_nodes = List_NNodes(name_table->name_list);

  fprintf(stderr, "# IRNameTable: [ %p ] [ %ld ]\n", name_table, n_nodes);
  // fprintf(stderr, "# {\n");

  ListNode* iter_nt = List_FirstNode(name_table->name_list);
  while (iter_nt != NULL) {
    IRName* iter_elem = List_AccessData(iter_nt);
    if (iter_elem->name_type != kIROperandType_Function) {
      fprintf(stderr, "#   [ %s ]: [ %ld ][ %s ]\n", iter_elem->name_str, iter_elem->name_id, IROperandTypeToStr(iter_elem->name_type));
    } else {
      fprintf(stderr, "#   [ %s ]: [ %ld ][ %s ][ %ld ]\n", iter_elem->name_str, iter_elem->name_id, IROperandTypeToStr(iter_elem->name_type), iter_elem->n_param);
    }

    iter_nt = List_NextNode(name_table->name_list, iter_nt);
  }

  // fprintf(stderr, "# }\n");
}

// inserted successfuly?
bool IRNameTable_Insert(IRNameTable* name_table,
                        const char* str,
                        size_t len,
                        int64_t name_id,
                        IROperandType name_type,
                        int64_t n_param) {
  ASSERT(name_table != NULL);
  ASSERT(str != NULL);

  IRName ir_name = {};
  bool is_found = IRNameTable_LookUpByStr(name_table, str, len, &ir_name);
  if (is_found) { return false; }

  ListNode* new_nt_elem_nd = List_CtorNodeAtStart(name_table->name_list);
  IRName* new_nt_elem = List_AccessData(new_nt_elem_nd);
  new_nt_elem->name_str = calloc(1, (len + 1) * sizeof(char));
  if (new_nt_elem->name_str == NULL) { $ return false; }
  memcpy(new_nt_elem->name_str, str, len);
  new_nt_elem->name_str[len] = '\0';

  new_nt_elem->name_id = name_id;
  new_nt_elem->name_type = name_type;
  new_nt_elem->len = len;
  new_nt_elem->n_param = n_param;

  return true;
}

// is found?
bool IRNameTable_LookUpByStr(IRNameTable* name_table,
                             const char* str,
                             size_t len,
                             IRName* ir_name_out) {
  ASSERT(name_table != NULL);
  ASSERT(str != NULL);
  ASSERT(ir_name_out != NULL);

  char* hold_str = alloca((len + 1) * sizeof(char));

  memcpy(hold_str, str, len);
  hold_str[len] = '\0';

  ListNode* iter_list_node = List_FirstNode(name_table->name_list);
  while (iter_list_node != NULL) {
    IRName* nt_elem = List_AccessData(iter_list_node);
    int cmp_keys = strcmp(hold_str, nt_elem->name_str);
    if (cmp_keys == 0) { break; }

    iter_list_node = List_NextNode(name_table->name_list, iter_list_node);
  }

  if (iter_list_node != NULL) {
    IRName* nt_elem = List_AccessData(iter_list_node);
    memcpy(ir_name_out, nt_elem, sizeof(IRName));
    return true;
  } else {
    return false;
  }
}

// is found?
bool IRNameTable_LookUpByValue(IRNameTable* name_table,
                               int64_t name_id,
                               IRName* ir_name_out) {
  ASSERT(name_table != NULL);
  ASSERT(ir_name_out != NULL);

  ListNode* iter_list_node = List_FirstNode(name_table->name_list);
  while (iter_list_node != NULL) {
    IRName* nt_elem = List_AccessData(iter_list_node);
    if (nt_elem->name_id == name_id) { break; }

    iter_list_node = List_NextNode(name_table->name_list, iter_list_node);
  }

  if (iter_list_node != NULL) {
    IRName* nt_elem = List_AccessData(iter_list_node);
    memcpy(ir_name_out, nt_elem, sizeof(IRName));
    return true;
  } else {
    return false;
  }
}

// void IRNameTable_Remove(IRNameTable* name_table, const char* str, size_t len) {
//   // FIXME
// }
