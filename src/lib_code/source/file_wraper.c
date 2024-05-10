#include "file_wraper.h"

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "my_assert.h"
#include "attr.h"

#if defined (FILE_WRAPER_LOG_ON)
static int64_t files_opened = 0;
#endif // FILE_WRAPER_LOG_ON

FILE* FOpenW(const char* file_name,
             const char* modes,
             UNUSED const char* debug_filename,
             UNUSED const int debug_line,
             UNUSED const char* debug_func) {
    ASSERT(file_name != NULL);
    ASSERT(modes != NULL);

#if defined (FILE_WRAPER_LOG_ON)
    fprintf(stderr, "# FOpenW call:\n");
    fprintf(stderr, "#   from: [ %s ][ %d ][ %s ]\n", debug_filename, debug_line, debug_func);
    fprintf(stderr, "#   [ filename ]: %s [ modes ]: %s\n", file_name, modes);
    fprintf(stderr, "#   files opened before call: %ld\n", files_opened);
#endif // FILE_WRAPER_LOG_ON

    FILE* new_file = fopen(file_name, modes);

#if defined(FILE_WRAPER_LOG_ON)
    if (new_file == NULL) {
        fprintf(stderr, "#   BAD FILE OPEN: %s\n", strerror(errno));
    } else {
        files_opened++;
        fprintf(stderr, "#   file opened succesfuly\n");
    }
    fprintf(stderr, "#   [ FILE* ]: %p\n", new_file);
    fprintf(stderr, "#   files opened after call: %ld\n", files_opened);
    fprintf(stderr, "# FOpenW end of call\n");
#endif // FILE_WRAPER_LOG_ON

    return new_file;
}

int FCloseW(FILE* stream,
            UNUSED const char* debug_filename,
            UNUSED const int debug_line,
            UNUSED const char* debug_func) {
    ASSERT(stream != NULL);

#if defined (FILE_WRAPER_LOG_ON)
    fprintf(stderr, "# FCloseW call:\n");
    fprintf(stderr, "#   from: [ %s ][ %d ][ %s ]\n", debug_filename, debug_line, debug_func);
    fprintf(stderr, "#   [ FILE* ]: %p\n", stream);
    fprintf(stderr, "#   files opened before call: %ld\n", files_opened);
#endif // FILE_WRAPER_LOG_ON

    int err = fclose(stream);

#if defined(FILE_WRAPER_LOG_ON)
    if (err != 0) {
        fprintf(stderr, "#   BAD FILE CLOSE: %s\n", strerror(errno));
    } else {
        files_opened--;
        fprintf(stderr, "#   file closed succesfuly\n");
    }
    fprintf(stderr, "#   files opened after call: %ld\n", files_opened);
    fprintf(stderr, "# FCloseW end of call\n");
#endif // FILE_WRAPER_LOG_ON

    return err;
}

// linux specific --------------------------------------------------------------

#if defined(__linux__)
#include <fcntl.h>
#include <unistd.h>

#if defined(FILE_WRAPER_LOG_ON)
static int64_t files_raw_opened = 0;
#endif // FILE_WRAPER_LOG_ON

// static const char* FlagsToStr(int flags); //FIXME

int RawOpenW(const char* file_name, int flags) {
    ASSERT(file_name != NULL);

#if defined (FILE_WRAPER_LOG_ON)
    fprintf(stderr, "# RawOpenW call:\n");
    fprintf(stderr, "#   [ filename ]: %s [ flags ]: %d\n", file_name, flags);
    fprintf(stderr, "#   files raw opened before call: %ld\n", files_raw_opened);
#endif // FILE_WRAPER_LOG_ON

    int new_fd = open(file_name, flags);

#if defined(FILE_WRAPER_LOG_ON)
    if (new_fd == -1) {
        fprintf(stderr, "#   BAD RAW OPEN: %s\n", strerror(errno));
    } else {
        files_raw_opened++;
        fprintf(stderr, "#   file raw opened succesfuly\n");
    }
    fprintf(stderr, "#   [ file desriptor ]: %d\n", new_fd);
    fprintf(stderr, "#   files raw opened after call: %ld\n", files_raw_opened);
    fprintf(stderr, "# RawOpenW end of call\n");
#endif // FILE_WRAPER_LOG_ON

    return new_fd;
}

int RawCloseW(int fd) {
#if defined (FILE_WRAPER_LOG_ON)
    fprintf(stderr, "# RawCloseW call:\n");
    fprintf(stderr, "#   [ file descriprtor]: %d\n", fd);
    fprintf(stderr, "#   files raw opened before call: %ld\n", files_raw_opened);
#endif // FILE_WRAPER_LOG_ON

    int err = close(fd);

#if defined(FILE_WRAPER_LOG_ON)
    if (err != 0) {
        fprintf(stderr, "#   BAD RAW CLOSE: %s\n", strerror(errno));
    } else {
        files_raw_opened--;
        fprintf(stderr, "#   file raw closed succesfuly\n");
    }
    fprintf(stderr, "#   files raw opened after call: %ld\n", files_raw_opened);
    fprintf(stderr, "# RawCloseW end of call\n");
#endif // FILE_WRAPER_LOG_ON

    return err;
}

#endif // __linux__
