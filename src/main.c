#include "arch_def.h"
#include "compiler_runtime_conf.h"
#include "flags_parser.h"
#include "tlang_ir.h"
#include "frontend.h"
#include "middleend.h"
#include "backend.h"
#include "x86_64_backend.h"

int main(const int argc, char* const* argv) {
  CompilerRuntimeConfig config = {0};
  ParseFlagsAndConfigure(argc, argv, &config);

  IR* ir = IR_Ctor(config.output_ir, config.ir_out_filename);

  if (config.do_front_pass) {
    FrontendError error = kFrontendError_Success;
    Frontend front = {};

    error = Frontend_Ctor(&front, &config);

    if (error == kFrontendError_Success) {
      error = Frontend_Pass(&front, ir);
    }

    Frontend_ThrowError(error);
    Frontend_Dtor(&front);
  }

  if (config.do_middle_pass) {
    MiddleendError error = kMiddleendError_Success;
    Middleend middle = {};

    error = Middleend_Ctor(&middle, &config);

    if (error == kMiddleendError_Success) {
      error = Middleend_Pass(&middle, ir);
    }

    Middleend_ThrowError(error);
    Middleend_Dtor(&middle);
  }

  if (config.do_back_pass) {
    BackendError error = kBackendError_Success;
    Backend backend = {};

    error = Backend_Ctor(&backend, &config);

    ArchDefinition x86_64_def = {
        .arch_name = "x86-64",
        .TranslateFromIRToTarget = x86_64_FromIRToTarget,
    };
    Backend_AddTarget(&backend, &x86_64_def);

    if (error == kBackendError_Success) {
      error = Backend_Pass(&backend, ir);
    }

    Backend_ThrowError(error);
    Backend_Dtor(&backend);
  }

  IR_Out(ir);

  IR_Dtor(ir);

  return 0;
}
