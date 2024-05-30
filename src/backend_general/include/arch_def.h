#ifndef ARCH_DEF_H_
#define ARCH_DEF_H_

#include <stddef.h>

#include "x86_64_backend.h"

typedef struct IR IR;
typedef struct Backend Backend;
typedef enum BackendError BackendError;

typedef enum TargetArchitectureId {
  kTargetArchitecture_Unknown = 0,
  kTargetArchitecture_x86_64  = 1,
  kTargetArchitecture_SPU     = 2,
} TargetArchitectureId;

typedef struct ArchDefinition {
  const char* arch_name;
  TargetArchitectureId arch_id;
  BackendError (*TranslateFromIRToTarget) (Backend* backend, IR* ir);
} ArchDefinition;

// FIXME
static const ArchDefinition arch_list[] = {{"x86-64", kTargetArchitecture_x86_64, x86_64_FromIRToTarget},
                                           {"SPU"   , kTargetArchitecture_SPU   , NULL},
                                           {0}};

#endif // ARCH_DEF_H_