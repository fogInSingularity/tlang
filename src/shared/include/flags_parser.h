#ifndef FLAGS_PARSER_H_
#define FLAGS_PARSER_H_

#include "compiler_runtime_conf.h"

typedef enum FlagsParserError {
  kFlagsParserError_Ok = 0,
} FlagsParserError;

void ParseFlagsAndConfigure(const int argc,
                            char* const* argv,
                            CompilerRuntimeConfig* config);

#endif // FLAGS_PARSER_H_