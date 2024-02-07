#include "utils.h"

void SwapBytes(void* a, void* b, size_t size) {
  ASSERT(a != NULL);
  ASSERT(b != NULL);

  if (a == b) { return; }

  Counter nFullOps = size >> 3; // колво шагов по 8
  Counter trailer = size & 0b111UL; // оставшиеся 7 байт

  Byte* aMove = (Byte*)a;
  Byte* bMove = (Byte*)b;
  for (Index i = 0; i < nFullOps; i++) {
    uint64_t temp = 0;
    memcpy(&temp, aMove, sizeof(uint64_t));
           memcpy(aMove, bMove, sizeof(uint64_t));
                  memcpy(bMove, &temp, sizeof(uint64_t));
    aMove += sizeof(uint64_t);
    bMove += sizeof(uint64_t);
  }

  if (trailer & 0b100UL) {
    uint32_t temp = 0;
    memcpy(&temp, aMove, sizeof(uint32_t));
           memcpy(aMove, bMove, sizeof(uint32_t));
                  memcpy(bMove, &temp, sizeof(uint32_t));
    aMove += sizeof(uint32_t);
    bMove += sizeof(uint32_t);
  }
  if (trailer & 0b010UL) {
      uint16_t temp = 0;
      memcpy(&temp, aMove, sizeof(uint16_t));
             memcpy(aMove, bMove, sizeof(uint16_t));
                    memcpy(bMove, &temp, sizeof(uint16_t));
      aMove += sizeof(uint16_t);
      bMove += sizeof(uint16_t);
  }
  if (trailer & 0b001UL) {
      uint8_t temp = 0;
      memcpy(&temp, aMove, sizeof(uint8_t));
             memcpy(aMove, bMove, sizeof(uint8_t));
                    memcpy(bMove, &temp, sizeof(uint8_t));
  }
}

void FillBytes(void* dest, const void* src, Counter n_elem, size_t size_elem) {
  ASSERT(dest != NULL);
  ASSERT(src != NULL);

  for (Index i = 0; i < n_elem; i++) {
    memcpy((uint8_t*)dest + i * size_elem, src, size_elem);
  }
}

double ParseNum(const char* str, size_t len) {
  ASSERT(str != NULL);

  // если в строке нашлась не цифра или больше 1 точки или знака то это NAN
  Counter cnt_dots = 0;
  Counter cnt_sign = 0;
  bool flag = true;

  const char* move_str = str;
  while (move_str < str + len) {
    if (isdigit(*move_str)) {
      ;
    } else if (*move_str == '.') {
      if (cnt_dots >= 1) {
        flag = false;
      } else {
        cnt_dots++;
      }
    } else if ((*move_str == '-') || (*move_str == '+')) {
      if (cnt_sign >= 1) {
        flag = false;
      } else {
        cnt_sign++;
      }
    } else {
      flag = false;
    }

    move_str++;
  }

  if (flag == false) {
    return NAN;
  } else {
    return atof(str);
  }
}

bool IsEqual(double a, double b) {
  return fabs(a - b) < kEpsilon;
}

double Log(double base, double arg) {
  if (IsEqual(base, 1.0)
      || IsEqual(arg, 0.0)
      || IsEqual(base, 0.0)) {
    return NAN;
  }
  if (arg < 0.0
      || base < 0.0) {
    return NAN;
  }

  return log(arg)/log(base);
}

void Putns(FILE* file, const char* str, size_t len) {
  ASSERT(file != NULL);
  ASSERT(str != NULL);

  for (Index i = 0; i < len; i++) {
    fputc(str[i], file);
  }
}
