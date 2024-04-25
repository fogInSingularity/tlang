#include "my_assert.h"

void my_assert(bool expr, const char* expr_str, const char* file, const char* func) {
  if (expr) { return; }

  (void)fprintf(stderr, BOLD RED "#  ERROR:\n");
  (void)fprintf(stderr,          "#    %s\n", expr_str);
  (void)fprintf(stderr,          "#    file: %s\n", file);
  (void)fprintf(stderr,          "#    func: %s\n" RESET, func);
  abort();
}
