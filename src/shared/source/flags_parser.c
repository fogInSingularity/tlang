#include "flags_parser.h"

#include <stddef.h>
#include <stdio.h>
#include <getopt.h>

#include "my_assert.h"
#include "debug.h"

// constans and types ----------------------------------------------------------

static const char* kElfFilenameDefault        = "tlang_program";
static const char* kTargetArchitectureDefault = "default_arch";
static const char* kSourceFilenameDefailt     = "default_source.tlang";

static const char* kAstDumpDefault            = "ast-dump.dot";
static const char* kIRDumpDefault             = "ir-dump";
static const char* kAsmDumpDefault            = "asm-dump.asm";

typedef enum CompilerOption {
  kCompilerOption_Zero          = 0,
  kCompilerOption_AstDump       = 1,
  kCompilerOption_IRDump        = 2,
  kCompilerOption_AsmDump       = 3,
  kCompilerOption_March         = 4,
  kCompilerOption_ExeName       = 5,
  kCompilerOption_SourceFile    = 6,
  kCompilerOption_SkipFrontend  = 7,
  kCompilerOption_SkipMiddleend = 8,
  kCompilerOption_SkipBackend   = 9,
} CompilerOption;

static int long_opt_found = 0;

static struct option kLongOption[] = {{"ast-dump",       required_argument, &long_opt_found, kCompilerOption_AstDump},
                                      {"ir-dump",        required_argument, &long_opt_found, kCompilerOption_IRDump},
                                      {"asm-dump",       required_argument, &long_opt_found, kCompilerOption_AsmDump},
                                      {"march",          required_argument, &long_opt_found, kCompilerOption_March},
                                      {"exe-name",       required_argument, &long_opt_found, kCompilerOption_ExeName},
                                      {"source-file",    required_argument, &long_opt_found, kCompilerOption_SourceFile},
                                      {"skip-frontend",  no_argument,       &long_opt_found, kCompilerOption_SkipFrontend},
                                      {"skip-middleend", no_argument,       &long_opt_found, kCompilerOption_SkipMiddleend},
                                      {"skip-backend",   no_argument,       &long_opt_found, kCompilerOption_SkipBackend},
                                      {0}};
static const char* kShortOption = "S:m:o:c:";

// global ----------------------------------------------------------------------

void ParseFlagsAndConfigure(const int argc,
                            char* const* argv,
                            CompilerRuntimeConfig* config) {
  ASSERT(argc != 0);
  ASSERT(argv != NULL);
  ASSERT(config != NULL);

  config->do_front_pass  = true;
  config->do_middle_pass = true;
  config->do_back_pass   = true;

  config->ast_out_dot_filename = kAstDumpDefault;
  config->ir_out_filename = kIRDumpDefault;
  config->asm_out_filename = kAsmDumpDefault;

  config->elf_filename = kElfFilenameDefault;
  config->target_architecture = kTargetArchitectureDefault;
  config->source_filename = kSourceFilenameDefailt;

  int get_opt_res = -1;
  int iter_opt = 0;
  do {
    get_opt_res = getopt_long(argc, argv, kShortOption, kLongOption, &iter_opt);
    if (get_opt_res == -1) { break; }
    if (get_opt_res != 0) { // short option
      switch (get_opt_res) {
        case 'S': // asm dump
          config->output_asm = true;
          config->asm_out_filename = optarg;
          PRINT_STR(optarg);
          break;
        case 'm': // target arch
          config->target_architecture = optarg;
          PRINT_STR(optarg);
          break;
        case 'o': // final exe filename
          config->elf_filename = optarg;
          PRINT_STR(optarg);
          break;
        case 'c':
          config->source_filename = optarg;
          PRINT_STR(optarg);
          break;
        default:
          break;
      }
    } else { // long option
      switch (long_opt_found) {
        case kCompilerOption_AstDump:
          config->output_ast_dot = true;
          config->ast_out_dot_filename = optarg;
          PRINT_STR(optarg);
          break;
        case kCompilerOption_IRDump:
          config->output_ir = true;
          config->ir_out_filename = optarg;
          PRINT_STR(optarg);
          break;
        case kCompilerOption_AsmDump:
          config->output_asm = true;
          config->asm_out_filename = optarg;
          PRINT_STR(optarg);
          break;
        case kCompilerOption_March:
          config->target_architecture = optarg;
          PRINT_STR(optarg);
          break;
        case kCompilerOption_ExeName:
          config->elf_filename = optarg;
          PRINT_STR(optarg);
          break;
        case kCompilerOption_SourceFile:
          config->source_filename = optarg;
          PRINT_STR(optarg);
          break;
        case kCompilerOption_SkipFrontend:
          // config->do_front_pass = false; //NOTE
          break;
        case kCompilerOption_SkipMiddleend:
          config->do_middle_pass = false;
          break;
        case kCompilerOption_SkipBackend:
          config->do_back_pass = false;
          break;
        default:
          break;
      }
    }

  } while (get_opt_res != -1);
}