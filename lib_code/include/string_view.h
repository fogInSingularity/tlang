#ifndef STRINGVIEW_H_
#define STRINGVIEW_H_

#include <stddef.h>

typedef struct StringView {
  const char* str;
  size_t len;
} StringView;

#endif // STRINGVIEW_H_
