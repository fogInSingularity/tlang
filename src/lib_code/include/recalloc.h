#ifndef LIB_RECALLOC_H_
#define LIB_RECALLOC_H_

#include <stdlib.h>
#include <memory.h>

#include "lib_config.h"

#include "my_assert.h"
#include "my_typedefs.h"

void* Recalloc(void* ptr, size_t new_cap, size_t used_size);

#endif // LIB_RECALLOC_H_
