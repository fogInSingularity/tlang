#include "bin_file.h"

#include <stdbool.h>

#include "alloc_wraper.h"

//static-----------------------------------------------------------------------

static size_t FileSize(FILE* file);
static void FileRead(BinData* data, FILE* file, bool insert_0);

//global-----------------------------------------------------------------------

void GetData(BinData* data, FILE* file, bool insert_0) {
  ASSERT(data != NULL);
  ASSERT(file != NULL);

  data->buf_size = FileSize(file);

  if (insert_0) {
    data->buf = (char*)CALLOCW(data->buf_size + 1, sizeof(char));
  } else {
    data->buf = (char*)CALLOCW(data->buf_size, sizeof(char));
  }

  FileRead(data, file, insert_0);
}

void FreeData(BinData* data) {
  ASSERT(data != NULL);

  FREEW(data->buf);
  data->buf= NULL;

  data->buf_size = 0;
}

//static-----------------------------------------------------------------------

void FileRead(BinData* data, FILE* file, bool insert_0) {
  ASSERT(data != NULL);
  ASSERT(file != NULL);

  size_t data_read = fread(data->buf, sizeof(char), data->buf_size, file);
  ASSERT(data_read == data->buf_size);

  if (insert_0) {
    data->buf[data->buf_size] = '\0';
  }
}

static size_t FileSize(FILE* file) {
  ASSERT(file != NULL);

  fseek(file, 0, SEEK_END);

  size_t size = (size_t)ftell(file);

  fseek(file, 0, SEEK_SET);

  return size;
}
