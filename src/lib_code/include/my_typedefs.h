#ifndef LIB_MYTYPEDEFS_H_
#define LIB_MYTYPEDEFS_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include "lib_config.h"

typedef size_t Index;
typedef ssize_t ErrorIndex;
typedef uint8_t Byte;
typedef size_t Counter;
typedef ssize_t ErrorCounter;
typedef int FileDescr;
// size_t should be used for size/capacity
// Index for array/loop indexing
// Counter for variables that used to count
// Error prefix means that negative numbers used for errors

typedef struct FatPointer {
    void* ptr;
    size_t size_in_bytes;
} FatPointer;

#endif  // LIB_MYTYPEDEFS_H_
