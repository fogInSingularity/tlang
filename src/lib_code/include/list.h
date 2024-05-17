/*
  Library double linked list
  Objects should have fixed size;
  Constracting and Distracting in place should be done by user
*/

#ifndef LIST_H_
#define LIST_H_

#include <stddef.h>
#include <stdint.h>

typedef struct List List;
typedef struct ListNode ListNode;

List* List_Ctor(const size_t elem_size);
int64_t List_Dtor(List* list); // invalidates list

ListNode* List_NextNode(const List* list, const ListNode* node);
ListNode* List_PrevNode(const List* list, const ListNode* node);
ListNode* List_FirstNode(const List* list);
ListNode* List_LastNode(const List* list);

void* List_AccessData(ListNode* node);
int64_t List_NNodes(List* list);

ListNode* List_CtorNodeAfter(List* list, ListNode* node);
ListNode* List_CtorNodeAtStart(List* list);
ListNode* List_CtorNodeAtEnd(List* list);
void List_RemoveNode(List* list, ListNode* node); // invalidate node

#endif // LIST_H_