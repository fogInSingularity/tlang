#include "backend.h"

#include <stddef.h>
#include <string.h>

#include "my_assert.h"
#include "debug.h"
#include "arch_def.h"

// static ----------------------------------------------------------------------

static TargetArchitectureId ParseTargetArchitecture(Backend* backend,
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
  if (backend->target_architecture == kTargetArchitecture_Unknown) {
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

BackendError Backend_Pass(Backend* backend, IR* ir) {
  if (backend->TranslateFromIRToTarget == NULL) { ASSERT(0 && "unsupported"); }
  return backend->TranslateFromIRToTarget(backend, ir);
}

// static ----------------------------------------------------------------------

static TargetArchitectureId ParseTargetArchitecture(Backend* backend,
                                                    const char* target_architecture_name) {
  ASSERT(target_architecture_name != NULL);
  ASSERT(target_architecture_name != NULL);

  const ArchDefinition* arch_def = arch_list;
  while (arch_def->arch_name != NULL) {
    int chech_arch = strcmp(target_architecture_name, arch_def->arch_name);
    if (chech_arch == 0) {
      backend->target_architecture = arch_def->arch_id;
      backend->TranslateFromIRToTarget = arch_def->TranslateFromIRToTarget;

      return arch_def->arch_id;
    }

    arch_def++;
  }

  return kTargetArchitecture_Unknown;
}