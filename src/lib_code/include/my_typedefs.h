#ifndef LIB_MYTYPEDEFS_H_
#define LIB_MYTYPEDEFS_H_

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#include "lib_config.h"

typedef size_t Index;
typedef ssize_t ErrorIndex;
typedef uint8_t Byte;
typedef size_t Counter;
typedef ssize_t ErrorCounter;
// size_t should be used for size/capacity
// Index for array/loop indexing
// Counter for variables that used to count
// Error prefix means that negative numbers used for errors

#endif // LIB_MYTYPEDEFS_H_
