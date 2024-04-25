#ifndef FILE_WRAPER_H_
#define FILE_WRAPER_H_

#include <stdio.h>

FILE* FOpenW(const char* filename, const char* mode);
int FCloseW(FILE* stream);

#if defined (__linux__)
int RawCloseW(int fd);
int RawOpenW(const char* file_name, int flags);
#endif // __linux__

#endif // FILE_WRAPER_H_