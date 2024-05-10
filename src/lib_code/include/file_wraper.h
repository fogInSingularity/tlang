#ifndef FILE_WRAPER_H_
#define FILE_WRAPER_H_

#include <stdio.h>

#include "lib_config.h"

#define FOPENW(file_name_, modes_) FOpenW(file_name_, modes_, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define FCLOSEW(stream_) FCloseW(stream_, __FILE__, __LINE__, __PRETTY_FUNCTION__)

FILE* FOpenW(const char* file_name,
             const char* modes,
             const char* debug_filename,
             const int debug_line,
             const char* debug_func);

int FCloseW(FILE* stream,
            const char* debug_filename,
            const int debug_line,
            const char* debug_func);

//FIXME add macro for raw
#if defined (__linux__)
int RawCloseW(int fd);
int RawOpenW(const char* file_name, int flags);
#endif // __linux__

#endif // FILE_WRAPER_H_
