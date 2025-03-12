#ifndef ARCH_DEF_H_
#define ARCH_DEF_H_

#include <stddef.h>

typedef struct IR IR;
typedef struct Backend Backend;
typedef enum BackendError BackendError;

typedef struct ArchDefinition {
  const char* arch_name;
  BackendError (*TranslateFromIRToTarget) (Backend* backend, IR* ir);
} ArchDefinition;

#endif // ARCH_DEF_H_
