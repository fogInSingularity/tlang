#ifndef COMPILER_RUNTIME_CONF_H_
#define COMPILER_RUNTIME_CONF_H_

#include <stdbool.h>

/*
  --ast-dump=<file_name>
  --ir-dump=<file_name>
  --asm_dump=<file_name>   -S
  --march=<arch_name>      -m
  --exe-name               -o
  --source-file            -c
  --skip-frontend
  --skip-middleend
  --skip-backend
*/

// guaranteed to live to the end
typedef struct CompilerRuntimeConfig {
  bool do_front_pass;
  bool do_middle_pass;
  bool do_back_pass;

  bool output_ast_dot;
  const char* ast_out_dot_filename;

  bool output_ir;
  const char* ir_out_filename;

  bool output_asm;
  const char* asm_out_filename;

  const char* source_filename;
  const char* elf_filename;
  const char* target_architecture;
} CompilerRuntimeConfig;

#endif // COMPILER_RUNTIME_CONF_H_