#ifndef FILE_WRAPER_H_
#define FILE_WRAPER_H_

#include <stdio.h>

#include "lib_config.h"

#define F_OPEN_W(file_name_, modes_) FOpenW (file_name_, modes_, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define F_CLOSE_W(stream_)           FCloseW(stream_,            __FILE__, __LINE__, __PRETTY_FUNCTION__)

FILE* FOpenW(const char* file_name,
             const char* modes,
             const char* debug_filename,
             const int debug_line,
             const char* debug_func);

int FCloseW(FILE* stream,
            const char* debug_filename,
            const int debug_line,
            const char* debug_func);

#if defined (__linux__)
#define RAW_OPEN_W(file_name_, flags_) RawOpenW (file_name_, flags_, __FILE__, __LINE__, __PRETTY_FUNCTION__);
#define RAW_CLOSE_W(fd_)               RawCloseW(fd_,                __FILE__, __LINE__, __PRETTY_FUNCTION__);

int RawOpenW(const char* file_name,
             int flags,
             const char* debug_filename,
             const int debug_line,
             const char* debug_func);

int RawCloseW(int fd,
             const char* debug_filename,
             const int debug_line,
             const char* debug_func);
#endif // __linux__

#endif // FILE_WRAPER_H_
