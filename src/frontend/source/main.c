#include "frontend.h"

int main(const int argc, const char** argv) {
  FrontError error = kFrontError_Success;

  if (argc < 3) { $
    fprintf(stderr, "! Not enough files passed:\n");
    fprintf(stderr, "!   Expected: 3\n");
    fprintf(stderr, "!   Provided: %d\n", argc);

    return 0;
  } // ./front source_code tree_dump

  Frontend front = {};
  error = FrontCtor(&front, argv[1], argv[2]);

  if (error == kFrontError_Success) {
    error = FrontPass(&front);
  }

  FrontDtor(&front);

  return 0;
}
