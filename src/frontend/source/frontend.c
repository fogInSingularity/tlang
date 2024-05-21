#include "frontend.h"

#include "file_wraper.h"
#include "my_assert.h"
#include "debug.h"
#include "ast_builder.h"
#include "lexer.h"
#include "tree.h"
#include "tlang_ir.h"
#include "ast_to_ir.h"

//global-----------------------------------------------------------------------

FrontendError FrontendCtor(Frontend* front, const CompilerRuntimeConfig* config) {
  ASSERT(front != NULL);
  ASSERT(config != NULL);

  front->is_valid = false;

  FILE* source = F_OPEN_W(config->source_filename, "r");
  if (source == NULL) { return kFrontendError_CantOpenSourceFile; }

  GetData(&front->source_data, source, true);
  F_CLOSE_W(source);

  DArrayError darr_error = DArray_Ctor(&front->token_array, sizeof(Token), 0);
  if (darr_error != kDArrayError_Success) { return kFrontendError_BadDArrayCtor; }

  TreeCtor(&front->ast, config->output_ast_dot, config->ast_out_dot_filename);

  front->output_ast_dot = config->output_ast_dot;
  front->ast_out_dot_filename = config->ast_out_dot_filename;

  front->is_valid = true;

  return kFrontendError_Success;
}

void FrontendDtor(Frontend* front) {
  ASSERT(front != NULL);

  FreeData(&front->source_data);
  DArray_Dtor(&front->token_array);
  TreeDtor(&front->ast);

  front->is_valid = false;
}

void FrontendThrowError(FrontendError error) {
  switch (error) { // FIXME
    case kFrontendError_Success:
      /* ok */
      break;
    case kFrontendError_CtorBadAlloc:
      PRINT_STR(STRINGIFY(kFrontendError_CtorBadAlloc));
      break;
    case kFrontendError_CantOpenSourceFile:
      PRINT_STR(STRINGIFY(kFrontendError_CantOpenSourceFile));
      break;
    case kFrontendError_BadDArrayCtor:
      PRINT_STR(STRINGIFY(kFrontendError_BadDArrayCtor));
      break;
    case kFrontendError_BadLexAnalyse:
      PRINT_STR(STRINGIFY(kFrontendError_BadLexAnalyse));
      break;
    case kFrontendError_BadAst:
      PRINT_STR(STRINGIFY(kFrontendError_BadAst));
      break;
    case kFrontendError_InvalidFront:
      PRINT_STR(STRINGIFY(kFrontendError_InvalidFront));
      break;
    default:
      ASSERT(0 && ":(");
      break;
  }
}

FrontendError FrontendPass(Frontend* front, IR* ir) {
  ASSERT(front != NULL);

  if (!front->is_valid) { return kFrontendError_InvalidFront; }

  LexicalError lex_error = Lexer(&front->source_data, &front->token_array);
  if (lex_error != kLexicalError_Success) { return kFrontendError_BadLexAnalyse; }

  AstError ast_error = AstBuilder(&front->token_array, &front->ast);
  if (ast_error != kAstError_Success) { return kFrontendError_BadAst; }

  TranslateAstToIr(&front->ast, ir);

  return kFrontendError_Success;
}
