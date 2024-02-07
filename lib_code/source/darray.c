#include "darray.h"

//static-----------------------------------------------------------------------

static const size_t kDArray_StandartAllocSize = 8;
static const size_t kDArray_MultiplyConst = 2;
static const size_t kDArray_NAllocRequests = 3;

static DArrayError DArray_ResizeUp(DArray* darr);
static DArrayError DArray_ResizeDown(DArray* darr);
static DArrayError DArray_Recalloc(DArray* darr, size_t new_cap_elem);

//global-----------------------------------------------------------------------

DArrayError DArray_Ctor(DArray* darr, size_t elem_size, size_t init_cap) {
  ASSERT(darr != NULL);

  darr->array = NULL;
  darr->n_elem = 0;
  darr->cap_elem = 0;
  darr->elem_size = 0;

  init_cap =   kDArray_StandartAllocSize > init_cap
             ? kDArray_StandartAllocSize : init_cap;

  darr->array = calloc(init_cap, elem_size);
  if (darr->array == NULL) { return kDArrayError_CtorCantAlloc; }

  darr->n_elem = 0;
  darr->cap_elem = init_cap;
  darr->elem_size = elem_size;

  return kDArrayError_Success;
}

void DArray_Dtor(DArray* darr) {
  ASSERT(darr != NULL);

  free(darr->array);
  darr->array = NULL;

  darr->n_elem = 0;
  darr->cap_elem = 0;
  darr->elem_size = 0;
}

void DArray_Dump(DArray* darr, DumpFunc* DumpElemFunc) {
  ASSERT(darr != NULL);

  fprintf(stderr, "# dynamic array [ %p ]\n", darr);
  fprintf(stderr, "# {\n");
  fprintf(stderr, "#   number of elements [ %lu ]\n", darr->n_elem);
  fprintf(stderr, "#   capacity of elements [ %lu ]\n", darr->cap_elem);
  fprintf(stderr, "#   element size [ %lu ]\n", darr->elem_size);
  fprintf(stderr, "#   array [ %p ]\n", darr->array);
  fprintf(stderr, "#   {\n");
  for (Index i = 0; i < darr->n_elem; i++) {
    fprintf(stderr, "#   [ %lu ]: ", i);
    void* elem = DArray_At(darr, i);
    DumpElemFunc(elem);
    fputc('\n', stderr);
  }
  fprintf(stderr, "#   }\n");
  fprintf(stderr, "# }\n");
}

void* DArray_At(DArray* darr, Index ind) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  if (ind >= darr->n_elem) { return NULL; }

  return (Byte*)darr->array + ind * darr->elem_size;
}

const void* DArray_AtCnst(const DArray* darr, Index ind) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  if (ind >= darr->n_elem) { return NULL; }

  return (Byte*)darr->array + ind * darr->elem_size;
}

void* DArray_Front(DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  if (DArray_IsEmpty(darr)) { return NULL; }

  return DArray_At(darr, 0);
}

const void* DArray_FrontCnst(const DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  if (DArray_IsEmpty(darr)) { return NULL; }

  return DArray_AtCnst(darr, 0);
}

void* DArray_Back(DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  if (DArray_IsEmpty(darr)) { return NULL; }

  return DArray_At(darr, darr->n_elem - 1);
}

const void* DArray_BackCnst(const DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  if (DArray_IsEmpty(darr)) { return NULL; }

  return DArray_AtCnst(darr, darr->n_elem - 1);
}

void* DArray_Data(DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  return darr->array;
}

const void* DArray_DataCnst(const DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  return darr->array;
}

DArrayError DArray_GetAt(const DArray* darr, Index ind, void* elem_ret) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);
  ASSERT(elem_ret != NULL);

  if (ind >= darr->n_elem) { return kDArrayError_GetAtOOR; }

  const void* elem_src = DArray_AtCnst(darr, ind);

  memcpy(elem_ret, elem_src, darr->elem_size);

  return kDArrayError_Success;
}

DArrayError DArray_SetAt(DArray* darr, Index ind, const void* elem) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);
  ASSERT(elem != NULL);

  if (ind >= darr->n_elem) { return kDArrayError_SetAtOOR; }

  void* elem_dest = DArray_At(darr, ind);

  memcpy(elem_dest, elem, darr->elem_size);

  return kDArrayError_Success;
}

bool DArray_IsEmpty(const DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  return darr->n_elem == 0 ? true : false;
}

Counter DArray_NElems(const DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  return darr->n_elem;
}

size_t DArray_Capacity(const DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  return darr->cap_elem;
}

DArrayError DArray_ShrinkToFit(DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  DArrayError error = kDArrayError_Success;
  error = DArray_Recalloc(darr, darr->n_elem);
  if (error != kDArrayError_Success) { return kDArrayError_Success; }

  darr->cap_elem = darr->n_elem;

  return kDArrayError_Success;
}

DArrayError DArray_Reserve(DArray* darr, size_t need_cap_elem) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  if (need_cap_elem <= darr->cap_elem) { return kDArrayError_Success; }

  DArrayError error = kDArrayError_Success;
  error = DArray_Recalloc(darr, need_cap_elem);
  if (error != kDArrayError_Success) { return kDArrayError_Success; }

  darr->cap_elem = need_cap_elem;

  return kDArrayError_Success;
}

DArrayError DArray_Clear(DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  size_t elem_size = darr->elem_size;

  DArray_Dtor(darr);
  return DArray_Ctor(darr, elem_size, kDArray_StandartAllocSize);
}

DArrayError DArray_Insert(DArray* darr, Index ind, void* elem) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);
  ASSERT(elem != NULL);

  if (ind == darr->n_elem) { return DArray_PushBack(darr, elem); }
  if (ind > darr->n_elem) { return kDArrayError_InsertOOR; }

  DArrayError error = kDArrayError_Success;

  error = DArray_Reserve(darr, darr->cap_elem + 1);
  if (error != kDArrayError_Success) { return error; }

  void* elem_dest = DArray_At(darr, ind + 1);
  void* elem_src = DArray_At(darr, ind);

  if (elem_dest == NULL) { return kDArrayError_AtOOR; }
  if (elem_src == NULL) { return kDArrayError_AtOOR; }

  memmove(elem_dest, elem_src, (darr->n_elem - ind) * darr->elem_size);
  memcpy(elem_src, elem, darr->elem_size);

  darr->n_elem++;

  return kDArrayError_Success;
}

DArrayError DArray_Erase(DArray* darr, Index ind) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  if (ind == darr->n_elem - 1) { return DArray_PopBackNoRet(darr); }
  if (ind >= darr->n_elem) { return kDArrayError_InsertOOR; }

  void* elem_dest = DArray_At(darr, ind);
  void* elem_src = DArray_At(darr, ind + 1);

  if (elem_dest == NULL) { return kDArrayError_AtOOR; }
  if (elem_src == NULL) { return kDArrayError_AtOOR; }

  memmove(elem_dest, elem_src, (darr->n_elem - ind - 1) * darr->elem_size);

  darr->n_elem--;

  return kDArrayError_Success;
}

DArrayError DArray_PushBack(DArray* darr, const void* elem) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);
  ASSERT(elem != NULL);

  DArrayError error = kDArrayError_Success;

  error = DArray_ResizeUp(darr);
  if (error != kDArrayError_Success) { return error; }

  darr->n_elem++;
  error = DArray_SetAt(darr, darr->n_elem - 1, elem);
  if (error != kDArrayError_Success) {
    darr->n_elem--;
    return error;
  }

  return kDArrayError_Success;
}

DArrayError DArray_PopBack(DArray* darr, void* elem_ret) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);
  ASSERT(elem_ret != NULL);

  if (DArray_IsEmpty(darr)) { return kDArrayError_PopBackOnEmpty; }

  DArrayError error = kDArrayError_Success;

  error = DArray_GetAt(darr, darr->n_elem - 1, elem_ret);
  if (error != kDArrayError_Success) { return error; }

  return DArray_PopBackNoRet(darr);
}

DArrayError DArray_PopBackNoRet(DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  if (DArray_IsEmpty(darr)) { return kDArrayError_PopBackOnEmpty; }

  void* old_elem = DArray_At(darr, darr->n_elem - 1);
  if (old_elem == NULL) { return kDArrayError_AtOOR; }

  memset(old_elem, 0, darr->elem_size);
  darr->n_elem--;

  return DArray_ResizeDown(darr);
}

//static-----------------------------------------------------------------------

static DArrayError DArray_ResizeUp(DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  DArrayError error = kDArrayError_Success;

  if (darr->cap_elem > darr->n_elem) { return kDArrayError_Success; }

  error = DArray_Recalloc(darr, darr->cap_elem * kDArray_MultiplyConst);
  if (error != kDArrayError_Success) { return kDArrayError_BadResizeUp; }

  darr->cap_elem *= kDArray_MultiplyConst;

  return kDArrayError_Success;
}

static DArrayError DArray_ResizeDown(DArray* darr) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  DArrayError error = kDArrayError_Success;

  if (darr->cap_elem < darr->n_elem * kDArray_MultiplyConst * 2) {
    return kDArrayError_Success;
  }

  if (darr->cap_elem <= kDArray_StandartAllocSize
      || darr->cap_elem <= kDArray_MultiplyConst) {
    return kDArrayError_Success;
  }

  error = DArray_Recalloc(darr, darr->cap_elem / kDArray_MultiplyConst);
  if (error != kDArrayError_Success) { return kDArrayError_BadResizeDown; }

  darr->cap_elem /= kDArray_MultiplyConst;

  return kDArrayError_Success;
}

static DArrayError DArray_Recalloc(DArray* darr, size_t new_cap_elem) {
  ASSERT(darr != NULL);
  ASSERT(darr->array != NULL);

  size_t n_requested = 0;
  void* hold = darr->array;

  do {
    darr->array = hold;
    darr->array = Recalloc(darr->array,
                           new_cap_elem * darr->elem_size,
                           darr->n_elem * darr->elem_size);

    n_requested++;
  } while ((darr->array == NULL) && (n_requested < kDArray_NAllocRequests));

  if (darr->array == NULL) {
    darr->array = hold;

    return kDArrayError_BadAlloc;
  }

  return kDArrayError_Success;
}
