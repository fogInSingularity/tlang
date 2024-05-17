#include "my_assert.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "color.h"

void my_assert(bool expr,
               const char* expr_str,
               const char* file,
               const char* func,
               int line) {
  if (expr) { return; }

  (void)fprintf(stderr, BOLD RED "#  ERROR:\n");
  (void)fprintf(stderr,          "#    %s\n", expr_str);
  (void)fprintf(stderr,          "#    file: %s:%d\n", file, line);
  (void)fprintf(stderr,          "#    func: %s\n" RESET, func);
  abort();
}
