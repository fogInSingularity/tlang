#include "compiler_runtime_conf.h"
#include "flags_parser.h"
#include "tlang_ir.h"
#include "frontend.h"

int main(const int argc, char* const* argv) {
  CompilerRuntimeConfig config = {0};
  ParseFlagsAndConfigure(argc, argv, &config);

  IR* ir = IR_Ctor(config.output_ir, config.ir_out_filename);

  if (config.do_front_pass) {
    FrontError error = kFrontError_Success;
    Frontend front = {};
    error = FrontCtor(&front, &config);
    if (error == kFrontError_Success) {
      error = FrontPass(&front, ir);
    } else {
      FrontThrowError(error);
    }
    FrontDtor(&front);
  }

  if (config.do_middle_pass) {}
  if (config.do_back_pass) {}

  IR_Out(ir);

  return 0;
}
