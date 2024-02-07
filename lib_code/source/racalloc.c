#include "recalloc.h"

void* Recalloc(void* ptr, size_t cap, size_t used_size) {
  ASSERT(ptr != NULL);

  ptr = realloc(ptr, cap);
  if (ptr == NULL) { return ptr; }

  memset((Byte*)ptr + used_size, 0, cap - used_size);

  return ptr;
}
