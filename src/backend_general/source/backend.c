#include "backend.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "my_assert.h"
#include "debug.h"
#include "arch_def.h"

// static ----------------------------------------------------------------------

static int64_t ParseTargetArchitecture(Backend* backend,
                                      const char* target_architecture_name);

// global ----------------------------------------------------------------------

BackendError Backend_Ctor(Backend* backend, const CompilerRuntimeConfig* config) {
  ASSERT(backend != NULL);
  ASSERT(config != NULL);

  backend->is_valid = false;

  backend->do_dump = config->output_asm;
  backend->asm_out_filename = config->asm_out_filename;

  backend->elf_filename = config->elf_filename;

  backend->target_architecture = ParseTargetArchitecture(backend, config->target_architecture);
  if (backend->target_architecture < 0) {
    return kBackendError_UnknownArchitecture;
  }

  backend->is_valid = true;

  return kBackendError_Success;
}

void Backend_Dtor(Backend* backend) {
  ASSERT(backend != NULL);

  backend->asm_out_filename = NULL;
  backend->do_dump = false;

  backend->is_valid = false;
}

void Backend_ThrowError(BackendError error) {
  switch (error) {
    case kBackendError_Success:
      PRINT_STR(STRINGIFY(kBackendError_Success));
      break;
    case kBackendError_UnknownArchitecture:
      PRINT_STR(STRINGIFY(kBackendError_UnknownArchitecture));
      break;
    default:
      PRINT_STR("unknown error");
      break;
  }
}

void Backend_AddTarget(Backend* backend, ArchDefinition* target_arch) {
    ASSERT(backend != NULL);
    ASSERT(target_arch != NULL);

  int64_t i_arch = 0;
  for (; backend->arch_list[i_arch].arch_name != NULL; i_arch++) {}
  backend->arch_list[i_arch] = *target_arch;
}

BackendError Backend_Pass(Backend* backend, IR* ir) {
  if (backend->TranslateFromIRToTarget == NULL) { ASSERT(0 && "unsupported"); }
  return backend->TranslateFromIRToTarget(backend, ir);
}

// static ----------------------------------------------------------------------

static int64_t ParseTargetArchitecture(Backend* backend,
                                       const char* target_architecture_name) {
  ASSERT(target_architecture_name != NULL);
  ASSERT(target_architecture_name != NULL);

  for (int64_t i_arch = 0; backend->arch_list[i_arch].arch_name != NULL; i_arch++) {
    int chech_arch = strcmp(target_architecture_name, backend->arch_list[i_arch].arch_name);
    if (chech_arch == 0) {
      backend->TranslateFromIRToTarget = backend->arch_list[i_arch].TranslateFromIRToTarget;

      return i_arch;
    }
  }

  return -1;
}
