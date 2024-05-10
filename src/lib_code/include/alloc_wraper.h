#ifndef ALLOC_WRAPER_H_
#define ALLOC_WRAPER_H_

#include <stddef.h>

#include "lib_config.h"

#define MALLOCW(size_) MallocW(size_, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define CALLOCW(nmemb_, size_) CallocW(nmemb_, size_, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define FREEW(ptr_) FreeW(ptr_, __FILE__, __LINE__, __PRETTY_FUNCTION__)

void* MallocW(size_t size,
              const char* debug_filename,
              const int debug_line,
              const char* debug_func);

void* CallocW(size_t nmemb,
              size_t size,
              const char* debug_filename,
              const int debug_line,
              const char* debug_func);

void FreeW(void* ptr,
          const char* debug_filename,
          const int debug_line,
          const char* debug_func);

#endif // ALLOC_WRAPER_H_
