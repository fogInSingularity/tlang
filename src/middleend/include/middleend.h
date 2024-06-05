#ifndef MIDDLEEND_H_
#define MIDDLEEND_H_

#include "compiler_runtime_conf.h"
#include "tlang_ir.h"

typedef enum MiddleendError {
  kMiddleendError_Success = 0,
} MiddleendError;

typedef struct Middleend {
  bool is_valid;
} Middleend;

MiddleendError Middleend_Ctor(Middleend* middle,
                              const CompilerRuntimeConfig* config);
void Middleend_Dtor(Middleend* middle);
void Middleend_ThrowError(MiddleendError error);

MiddleendError Middleend_Pass(Middleend* middle, IR* ir);

#endif // MIDDLEEND_H_