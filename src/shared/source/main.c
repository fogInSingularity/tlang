#include "compiler_runtime_conf.h"
#include "flags_parser.h"
#include "tlang_ir.h"
#include "frontend.h"
#include "backend.h"

int main(const int argc, char* const* argv) {
  CompilerRuntimeConfig config = {0};
  ParseFlagsAndConfigure(argc, argv, &config);

  IR* ir = IR_Ctor(config.output_ir, config.ir_out_filename); //FIXME

  if (config.do_front_pass) {
    FrontendError error = kFrontendError_Success;
    Frontend front = {};

    error = FrontendCtor(&front, &config);

    if (error == kFrontendError_Success) {
      error = FrontendPass(&front, ir);
    }

    FrontendThrowError(error);
    FrontendDtor(&front);
  }

  if (config.do_middle_pass) {}
  if (config.do_back_pass) {
    BackendError error = kBackendError_Success;
    Backend backend = {};

    error = BackendCtor(&backend, &config);

    if (error == kBackendError_Success) {
      error = BackendRun(&backend, ir);
    }

    BackendThrowError(error);
    BackendDtor(&backend);
  }

  IR_Out(ir);

  return 0;
}
