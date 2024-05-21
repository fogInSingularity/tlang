#ifndef LIB_MYASSERT_H_
#define LIB_MYASSERT_H_

#include <assert.h>
#include <stdbool.h>

#include "lib_config.h"

#if defined(DEBUG)
#define ASSERT(expr) my_assert(expr, #expr, __FILE__, __PRETTY_FUNCTION__, __LINE__);
// #define ASSERT(expr) assert(expr);
#else
#define ASSERT(expr) (void)(expr);
#endif

void my_assert(bool expr,
               const char* expr_str,
               const char* file,
               const char* func,
               int line);

#endif  // LIB_MYASSERT_H_
