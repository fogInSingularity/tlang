#ifndef LIB_BINFILE_H_
#define LIB_BINFILE_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib_config.h"

#include "my_typedefs.h"
#include "my_assert.h"

typedef struct {
  char* buf;
  size_t buf_size;
} BinData;

void GetData(BinData* data, FILE* file);
void FreeData(BinData* data);

#endif // LIB_BINFILE_H_
