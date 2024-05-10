#include "alloc_wraper.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "attr.h"

#if defined (ALLOC_WRAPER_LOG_ON)
static int64_t mem_alloced = 0;
#endif // ALLOC_WRAPER_LOG_ON

void* MallocW(size_t size,
              UNUSED const char* debug_filename,
              UNUSED const int debug_line,
              UNUSED const char* debug_func) {
#if defined (ALLOC_WRAPER_LOG_ON)
  fprintf(stderr, "# MallocW call:\n");
  fprintf(stderr, "#   from: [ %s ][ %d ][ %s ]\n", debug_filename, debug_line, debug_func);
  fprintf(stderr, "#   [ size ]: %lu\n", size);
  fprintf(stderr, "#   memory allocated before call: %ld\n", mem_alloced);
#endif // ALLOC_WRAPER_LOG_ON

  void* mem_ptr = malloc(size);

#if defined(ALLOC_WRAPER_LOG_ON)
  if (mem_ptr == NULL) {
    fprintf(stderr, "#   BAD ALLOC: %s\n", strerror(errno));
  } else {
    mem_alloced++;
    fprintf(stderr, "#   mem allocated successfuly\n");
  }
  fprintf(stderr, "#   [ void* ]: %p\n", mem_ptr);
  fprintf(stderr, "#   memory allocated after call: %ld\n", mem_alloced);
  fprintf(stderr, "# MallocW end of call\n");
#endif // ALLOC_WRAPER_LOG_ON

  return mem_ptr;
}

void* CallocW(size_t nmemb,
              size_t size,
              UNUSED const char* debug_filename,
              UNUSED const int debug_line,
              UNUSED const char* debug_func) {
#if defined (ALLOC_WRAPER_LOG_ON)
  fprintf(stderr, "# CallocW call:\n");
  fprintf(stderr, "#   from: [ %s ][ %d ][ %s ]\n", debug_filename, debug_line, debug_func);
  fprintf(stderr, "#   [ nmemb ]: %lu [ size ]: %lu\n", nmemb, size);
  fprintf(stderr, "#   memory allocated before call: %ld\n", mem_alloced);
#endif // ALLOC_WRAPER_LOG_ON

  void* mem_ptr = calloc(nmemb, size);

#if defined(ALLOC_WRAPER_LOG_ON)
  if (mem_ptr == NULL) {
    fprintf(stderr, "#   BAD ALLOC: %s\n", strerror(errno));
  } else {
    mem_alloced++;
    fprintf(stderr, "#   mem allocated successfuly\n");
  }
  fprintf(stderr, "#   [ void* ]: %p\n", mem_ptr);
  fprintf(stderr, "#   memory allocated after call: %ld\n", mem_alloced);
  fprintf(stderr, "# CallocW end of call\n");
#endif // ALLOC_WRAPER_LOG_ON

  return mem_ptr;
}

void FreeW(void* ptr,
          UNUSED const char* debug_filename,
          UNUSED const int debug_line,
          UNUSED const char* debug_func) {
#if defined (ALLOC_WRAPER_LOG_ON)
  fprintf(stderr, "# FreeW call:\n");
  fprintf(stderr, "#   from: [ %s ][ %d ][ %s ]\n", debug_filename, debug_line, debug_func);
  fprintf(stderr, "#   [ ptr ]: %p\n", ptr);
  fprintf(stderr, "#   memory allocated before call: %ld\n", mem_alloced);
#endif // ALLOC_WRAPER_LOG_ON

  free(ptr);
#if defined (ALLOC_WRAPER_LOG_ON)
  mem_alloced--;
#endif // ALLOC_WRAPER_LOG_ON

#if defined(ALLOC_WRAPER_LOG_ON)
  fprintf(stderr, "#   memory allocated after call: %ld\n", mem_alloced);
  fprintf(stderr, "# FreeW end of call\n");
#endif // ALLOC_WRAPER_LOG_ON
}
