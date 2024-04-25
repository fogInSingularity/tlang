#include "frontend.h"

#include "file_wraper.h"

//global-----------------------------------------------------------------------

FrontError FrontCtor(Frontend* front,
                     const char* source_file,
                     const char* target_file) {
  ASSERT(front != NULL);
  ASSERT(source_file != NULL);
  ASSERT(target_file != NULL);

  FILE* source = FOpenW(source_file, "r");
  if (source == NULL) { return kFrontError_CantOpenSourceFile; }

  GetData(&front->source_data, source);
  FCloseW(source);

  DArrayError darr_error = DArray_Ctor(&front->token_array, sizeof(Token), 0);
  if (darr_error != kDArrayError_Success) { return kFrontError_BadDArrayCtor; }

  //FIXME tree ctor

  return kFrontError_Success;
}

void FrontDtor(Frontend* front) {
  ASSERT(front != NULL);

  FreeData(&front->source_data);
  DArray_Dtor(&front->token_array);
  TreeDtor(&front->ast);
}

FrontError FrontPass(Frontend* front) {
  ASSERT(front != NULL);

  LexicalError lex_error = Lexer(&front->source_data, &front->token_array);
  if (lex_error != kLexicalError_Success) { return kFrontError_BadLexAnalyse; }

  AstError ast_error = AstBuilder(&front->token_array, &front->ast);

  return kFrontError_Success;
}
