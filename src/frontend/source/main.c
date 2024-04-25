#include "frontend.h"

int main(const int argc, const char** argv) {
  FrontError error = kFrontError_Success;

  if (argc < 3) {$ return 0; }

  Frontend front = {};
  error = FrontCtor(&front, argv[1], argv[2]);

  if (error == kFrontError_Success) {
    error = FrontPass(&front);
  }

  FrontDtor(&front);

  return 0;
}
