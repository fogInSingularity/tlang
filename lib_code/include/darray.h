#ifndef DARRAY_H_
#define DARRAY_H_

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "my_assert.h"
#include "my_typedefs.h"
#include "recalloc.h"

typedef void DumpFunc(const void* elem);

typedef enum {
  kDArrayError_Success        = 0,
  kDArrayError_CtorCantAlloc  = 1,
  kDArrayError_AtOOR          = 2,
  kDArrayError_GetAtOOR       = 3,
  kDArrayError_SetAtOOR       = 4,
  kDArrayError_BadAlloc       = 5,
  kDArrayError_BadResizeUp    = 6,
  kDArrayError_BadResizeDown  = 7,
  kDArrayError_InsertOOR      = 8,
  kDArrayError_EraseOOR       = 9,
  kDArrayError_PopBackOnEmpty = 10,
} DArrayError;

typedef struct DArray {
  void* array;
  Counter n_elem;
  size_t cap_elem;
  size_t elem_size;
} DArray;

DArrayError DArray_Ctor(DArray* darr, size_t elem_size, size_t init_cap);
void DArray_Dtor(DArray* darr);
void DArray_Dump(DArray* darr, DumpFunc* DumpElemFunc);

void* DArray_At(DArray* darr, Index ind);
const void* DArray_AtCnst(const DArray* darr, Index ind);
void* DArray_Front(DArray* darr);
const void* DArray_FrontCnst(const DArray* darr);
void* DArray_Back(DArray* darr);
const void* DArray_BackCnst(const DArray* darr);
void* DArray_Data(DArray* darr);
const void* DArray_DataCnst(const DArray* darr);

DArrayError DArray_GetAt(const DArray* darr, Index ind, void* elem_ret);
DArrayError DArray_SetAt(DArray* darr, Index ind, const void* elem);

bool DArray_IsEmpty(const DArray* darr);
Counter DArray_NElems(const DArray* darr);
size_t DArray_Capacity(const DArray* darr);


DArrayError DArray_ShrinkToFit(DArray* darr);
DArrayError DArray_Reserve(DArray* darr, size_t need_cap_elem);
DArrayError DArray_Clear(DArray* darr);
DArrayError DArray_Insert(DArray* darr, Index ind, void* elem);
DArrayError DArray_Erase(DArray* darr, Index ind);
DArrayError DArray_PushBack(DArray* darr, const void* elem);
DArrayError DArray_PopBack(DArray* darr, void* elem_ret);
DArrayError DArray_PopBackNoRet(DArray* darr);

#endif // DARRAY_H_
