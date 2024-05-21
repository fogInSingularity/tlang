#ifndef BACKED_H_
#define BACKED_H_

#include "compiler_runtime_conf.h"
#include "arch_def.h"
#include "tlang_ir.h"

typedef enum BackendError {
  kBackendError_Success             = 0,
  kBackendError_UnknownArchitecture = 1,
} BackendError;

typedef struct Backend {
  bool is_valid;
  const char* elf_filename;

  TargetArchitectureId target_architecture;
  BackendError (*TranslateFromIRToTarget) (struct Backend* backend, IR* ir);

  bool do_dump;
  const char* asm_out_filename;
} Backend;

BackendError BackendCtor(Backend* backend, const CompilerRuntimeConfig* config);
void BackendDtor(Backend* backend);
void BackendThrowError(BackendError error);

BackendError BackendRun(Backend* backend, IR* ir);

#endif // BACKED_H_