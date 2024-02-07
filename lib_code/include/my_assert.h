#ifndef LIB_MYASSERT_H_
#define LIB_MYASSERT_H_

#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "lib_config.h"

#include "color.h"

#ifdef ASSERT_ON
// #define ASSERT(expr) my_assert(expr, #expr, __FILE__, __PRETTY_FUNCTION__);
#define ASSERT(expr) assert(expr);
#else
#define ASSERT(expr) ;
#endif // ASSERT_ON

void my_assert(bool expr, const char* expr_str, const char* file, const char* func);

#endif // LIB_MYASSERT_H_
