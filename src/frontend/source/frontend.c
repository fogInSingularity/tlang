#include "frontend.h"

#include "file_wraper.h"
#include "my_assert.h"
#include "my_typedefs.h"
#include "debug.h"
#include "ast_builder.h"
#include "lexer.h"
#include "tree.h"
#include "tlang_ir.h"
#include "ast_to_ir.h"

//global-----------------------------------------------------------------------

FrontError FrontCtor(Frontend* front, const CompilerRuntimeConfig* config) {
  ASSERT(front != NULL);
  ASSERT(config != NULL);

  front->is_valid = false;

  FILE* source = FOPENW(config->source_filename, "r");
  if (source == NULL) { return kFrontError_CantOpenSourceFile; }

  GetData(&front->source_data, source);
  FCLOSEW(source);

  DArrayError darr_error = DArray_Ctor(&front->token_array, sizeof(Token), 0);
  if (darr_error != kDArrayError_Success) { return kFrontError_BadDArrayCtor; }

  TreeCtor(&front->ast, config->output_ast_dot, config->ast_out_dot_filename);

  front->output_ast_dot = config->output_ast_dot;
  front->ast_out_dot_filename = config->ast_out_dot_filename;

  front->is_valid = true;

  return kFrontError_Success;
}

void FrontDtor(Frontend* front) {
  ASSERT(front != NULL);

  FreeData(&front->source_data);
  DArray_Dtor(&front->token_array);
  TreeDtor(&front->ast);

  front->is_valid = false;
}

void FrontThrowError(FrontError error) {
  switch (error) { //FIXME
    case kFrontError_Success:
      /* ok */
      break;
    case kFrontError_CtorBadAlloc:
      PRINT_STR(STRINGIFY(kFrontError_CtorBadAlloc));
      break;
    case kFrontError_CantOpenSourceFile:
      PRINT_STR(STRINGIFY(kFrontError_CantOpenSourceFile));
      break;
    case kFrontError_BadDArrayCtor:
      PRINT_STR(STRINGIFY(kFrontError_BadDArrayCtor));
      break;
    case kFrontError_BadLexAnalyse:
      PRINT_STR(STRINGIFY(kFrontError_BadLexAnalyse));
      break;
    case kFrontError_BadAst:
      PRINT_STR(STRINGIFY(kFrontError_BadAst));
      break;
    case kFrontError_InvalidFront:
      PRINT_STR(STRINGIFY(kFrontError_InvalidFront));
      break;
  }
}

FrontError FrontPass(Frontend* front, IR* ir) {
  ASSERT(front != NULL);

  if (!front->is_valid) { return kFrontError_InvalidFront; }

  LexicalError lex_error = Lexer(&front->source_data, &front->token_array);
  if (lex_error != kLexicalError_Success) { return kFrontError_BadLexAnalyse; }

  AstError ast_error = AstBuilder(&front->token_array, &front->ast);
  if (ast_error != kAstError_Success) { return kFrontError_BadAst; }

  TranslateAstToIr(&front->ast, ir);

  return kFrontError_Success;
}
