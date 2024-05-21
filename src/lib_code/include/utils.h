#ifndef LIB_UTILS_H_
#define LIB_UTILS_H_

#include <stddef.h>
#include <memory.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "lib_config.h"

#include "my_typedefs.h"

#if defined (MIN)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif // #ifndef MIN

#if defined (MAX)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif // #ifndef MAX

static const double kEpsilon = 0.00000001;

void SwapBytes(void* a, void* b, size_t size);
void FillBytes(void* dest, const void* src,
               Counter n_elem, size_t size_elem);
double ParseNum(const char* str, size_t len);
bool IsEqual(double a, double b);
double Log(double base, double arg);
void Putns(FILE* file, const char* str, size_t len); //NOTE

#endif // LIB_UTILS_H_
