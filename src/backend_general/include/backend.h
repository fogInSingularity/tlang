#ifndef BACKED_H_
#define BACKED_H_

#include "compiler_runtime_conf.h"
#include "arch_def.h"
#include "tlang_ir.h"
#include <stdint.h>

#define N_TARGET_ARCHS 8

typedef enum BackendError {
  kBackendError_Success             = 0,
  kBackendError_UnknownArchitecture = 1,
} BackendError;

typedef struct Backend {
  bool is_valid;
  const char* elf_filename;

  bool do_dump;
  const char* asm_out_filename;
  ArchDefinition arch_list[N_TARGET_ARCHS];

  int64_t target_architecture;
  BackendError (*TranslateFromIRToTarget) (struct Backend* backend, IR* ir);
} Backend;

BackendError Backend_Ctor(Backend* backend, const CompilerRuntimeConfig* config);
void Backend_Dtor(Backend* backend);
void Backend_ThrowError(BackendError error);

void Backend_AddTarget(Backend* backend, ArchDefinition* target_arch);
BackendError Backend_Pass(Backend* backend, IR* ir);

#endif // BACKED_H_
