#include "middleend.h"

#include <stdbool.h>

#include "my_assert.h"
#include "debug.h"

#include "const_prop_pass.h"

// global ----------------------------------------------------------------------

MiddleendError Middleend_Ctor(Middleend* middle,
                              const CompilerRuntimeConfig* config) {
  ASSERT(middle != NULL);
  ASSERT(config != NULL);

  middle->is_valid = true;

  return kMiddleendError_Success;
}

void Middleend_Dtor(Middleend* middle) {
  ASSERT(middle != NULL);

  middle->is_valid = false;
}

void Middleend_ThrowError(MiddleendError error) {
  switch (error) {
    case kMiddleendError_Success:
      /* no error */
      break;
    default:
      ASSERT(0 && "unknown error");
  }
}

MiddleendError Middleend_Pass(Middleend* middle, IR* ir) {
  ASSERT(middle != NULL);
  ASSERT(ir != NULL);

  int64_t optimized = 0;

  do {
    optimized = 0;
    optimized += ConstPropPass(ir);
  } while (optimized > 0);

  return kMiddleendError_Success; //NOTE
}