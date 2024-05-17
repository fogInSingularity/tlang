#include "list.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>

#include "my_assert.h"
#include "debug.h"

// List struct -----------------------------------------------------------------

typedef struct ListNode {
  struct ListNode* next_node;
  struct ListNode* prev_node;
} ListNode;

typedef struct List {
  size_t elem_size;
  int64_t n_nodes;
  ListNode* head;
  ListNode* tail;
} List;

// static ----------------------------------------------------------------------


// global ----------------------------------------------------------------------

List* List_Ctor(const size_t elem_size) {
  List* new_list = calloc(1, sizeof(List));
  if (new_list == NULL) { return NULL; }

  new_list->elem_size = elem_size;
  new_list->n_nodes = 0;

  ListNode* hold_head= calloc(1, sizeof(ListNode));
  ListNode* hold_tail = calloc(1, sizeof(ListNode));
  if ((hold_head == NULL) || (hold_tail == NULL)) {
    if (hold_head == NULL) { free(hold_head); }
    if (hold_tail == NULL) { free(hold_tail); }

    free(new_list);
    return NULL;
  }

  hold_head->next_node = hold_tail;
  hold_head->prev_node = NULL;
  hold_tail->next_node = NULL;
  hold_tail->prev_node = hold_head;

  new_list->head = hold_head;
  new_list->tail = hold_tail;

  return new_list;
}

int64_t List_Dtor(List* list) {
  ASSERT(list == NULL);

  int64_t nodes_remaind = list->n_nodes;

  free(list->head);
  free(list->tail);
  free(list);

  return nodes_remaind;
}

ListNode* List_NextNode(const List* list, const ListNode* node) {
  ASSERT(list != NULL);
  ASSERT(node != NULL);

  if (node->next_node == list->tail) { return NULL; }
  return node->next_node;
}

ListNode* List_PrevNode(const List* list, const ListNode* node) {
  ASSERT(list != NULL);
  ASSERT(node != NULL);

  if (node->prev_node == list->head) { return NULL; }
  return node->prev_node;
}

ListNode* List_FirstNode(const List* list) {
  if (list->n_nodes == 0) { return NULL; }
  return list->head->next_node;
}

ListNode* List_LastNode(const List* list) {
  if (list->n_nodes == 0) { return NULL; }
  return list->tail->prev_node;
}

void* List_AccessData(ListNode* node) {
  ASSERT(node != NULL);

  return (char*)node + sizeof(ListNode);
}

int64_t List_NNodes(List* list) {
  ASSERT(list != NULL);

  return list->n_nodes;
}

ListNode* List_CtorNodeAfter(List* list, ListNode* node) {
  ASSERT(list != NULL);
  ASSERT(node != NULL);

  ListNode* new_node = calloc(1, sizeof(ListNode) + list->elem_size);
  if (new_node == NULL) { return NULL; }

  ListNode* next_node = node->next_node;

  new_node->next_node = next_node;
  node->next_node = new_node;
  next_node->prev_node = new_node;
  new_node->prev_node = node;

  list->n_nodes++;

  return new_node;
}

ListNode* List_CtorNodeAtStart(List* list) {
  ASSERT(list != NULL);

  ListNode* new_node = calloc(1, sizeof(ListNode) + list->elem_size);
  if (new_node == NULL) { return NULL; }

  ListNode* head = list->head;
  ListNode* next_node = head->next_node;

  new_node->next_node = next_node;
  head->next_node = new_node;
  next_node->prev_node = new_node;
  new_node->prev_node = head;

  list->n_nodes++;

  return new_node;
}

ListNode* List_CtorNodeAtEnd(List* list) {
  ASSERT(list != NULL);

  ListNode* last_node = List_LastNode(list);
  if (last_node == NULL) {
    return List_CtorNodeAtStart(list);
  } else {
    return List_CtorNodeAfter(list, last_node);
  }
}

void List_RemoveNode(List* list, ListNode* node) {
  ASSERT(node != NULL);

  ListNode* next_node = node->next_node;
  ListNode* prev_node = node->prev_node;

  next_node->prev_node = prev_node;
  prev_node->next_node = next_node;

  free(node);

  list->n_nodes--;
}