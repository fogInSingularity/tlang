#ifndef X86_64_BACKEND_H_
#define X86_64_BACKEND_H_

typedef struct IR IR;
typedef struct Backend Backend;
typedef enum BackendError BackendError;

BackendError x86_64_FromIRToTarget (Backend* backend, IR* ir);

#endif // X86_64_BACKEND_H_