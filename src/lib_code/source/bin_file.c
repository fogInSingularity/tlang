#include "bin_file.h"

#include "alloc_wraper.h"

//static-----------------------------------------------------------------------

static size_t FileSize(FILE* file);
static void FileRead(BinData* data, FILE* file);

//global-----------------------------------------------------------------------

void GetData(BinData* data, FILE* file) {
  ASSERT(data != NULL);
  ASSERT(file != NULL);

  data->buf_size = FileSize(file);

  data->buf = (char*)CALLOCW(data->buf_size + 1, sizeof(char));

  FileRead(data, file);
}

void FreeData(BinData* data) {
  ASSERT(data != NULL);

  FREEW(data->buf);
  data->buf= NULL;

  data->buf_size = 0;
}

//static-----------------------------------------------------------------------

void FileRead(BinData* data, FILE* file) {
  ASSERT(data != NULL);
  ASSERT(file != NULL);

  size_t data_read = fread(data->buf, sizeof(char), data->buf_size, file);
  ASSERT(data_read == data->buf_size);

  data->buf[data->buf_size] = '\0';
}

static size_t FileSize(FILE* file) {
  ASSERT(file != NULL);

  fseek(file, 0, SEEK_END);

  size_t size = (size_t)ftell(file);

  fseek(file, 0, SEEK_SET);

  return size;
}
