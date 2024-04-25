#include "file_wraper.h"

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "my_assert.h"

#if defined (DEBUG)
static int64_t files_opened = 0;
#endif // DEBUG

FILE* FOpenW(const char* file_name, const char* modes) {
    ASSERT(file_name != NULL);
    ASSERT(modes != NULL);

#if defined (DEBUG)
    fprintf(stderr, "# FOpenW call:\n");
    fprintf(stderr, "#   [ filename ]: %s [ modes ]: %s\n", file_name, modes);
    fprintf(stderr, "#   files opened before call: %ld\n", files_opened);
#endif // DEBUG

    FILE* new_file = fopen(file_name, modes);

#if defined(DEBUG)
    if (new_file == NULL) {
        fprintf(stderr, "#   BAD FILE OPEN: %s\n", strerror(errno));
    } else {
        files_opened++;
        fprintf(stderr, "#   file opened succesfuly\n");
    }
    fprintf(stderr, "#   [ FILE* ]: %p\n", new_file);
    fprintf(stderr, "#   files opened after call: %ld\n", files_opened);
    fprintf(stderr, "# FOpenW end of call\n");
#endif // DEBUG

    return new_file;
}

int FCloseW(FILE* stream) {
    ASSERT(stream != NULL);

#if defined (DEBUG)
    fprintf(stderr, "# FCloseW call:\n");
    fprintf(stderr, "#   [ FILE* ]: %p\n", stream);
    fprintf(stderr, "#   files opened before call: %ld\n", files_opened);
#endif // DEBUG

    int err = fclose(stream);

#if defined(DEBUG)
    if (err != 0) {
        fprintf(stderr, "#   BAD FILE CLOSE: %s\n", strerror(errno));
    } else {
        files_opened--;
        fprintf(stderr, "#   file closed succesfuly\n");
    }
    fprintf(stderr, "#   files opened after call: %ld\n", files_opened);
    fprintf(stderr, "# FCloseW end of call\n");
#endif // DEBUG

    return err;
}

// linux specific --------------------------------------------------------------

#if defined(__linux__)
#include <fcntl.h>
#include <unistd.h>

#if defined(DEBUG)
static int64_t files_raw_opened = 0;
#endif // DEBUG

// static const char* FlagsToStr(int flags); //FIXME

int RawOpenW(const char* file_name, int flags) {
    ASSERT(file_name != NULL);

#if defined (DEBUG)
    fprintf(stderr, "# RawOpenW call:\n");
    fprintf(stderr, "#   [ filename ]: %s [ flags ]: %d\n", file_name, flags);
    fprintf(stderr, "#   files raw opened before call: %ld\n", files_raw_opened);
#endif // DEBUG

    int new_fd = open(file_name, flags);

#if defined(DEBUG)
    if (new_fd == -1) {
        fprintf(stderr, "#   BAD RAW OPEN: %s\n", strerror(errno));
    } else {
        files_raw_opened++;
        fprintf(stderr, "#   file raw opened succesfuly\n");
    }
    fprintf(stderr, "#   [ file desriptor ]: %d\n", new_fd);
    fprintf(stderr, "#   files raw opened after call: %ld\n", files_raw_opened);
    fprintf(stderr, "# RawOpenW end of call\n");
#endif // DEBUG

    return new_fd;
}

int RawCloseW(int fd) {
#if defined (DEBUG)
    fprintf(stderr, "# RawCloseW call:\n");
    fprintf(stderr, "#   [ file descriprtor]: %d\n", fd);
    fprintf(stderr, "#   files raw opened before call: %ld\n", files_raw_opened);
#endif // DEBUG

    int err = close(fd);

#if defined(DEBUG)
    if (err != 0) {
        fprintf(stderr, "#   BAD RAW CLOSE: %s\n", strerror(errno));
    } else {
        files_raw_opened--;
        fprintf(stderr, "#   file raw closed succesfuly\n");
    } 
    fprintf(stderr, "#   files raw opened after call: %ld\n", files_raw_opened);
    fprintf(stderr, "# RawCloseW end of call\n");
#endif // DEBUG

    return err;
}

#endif // __linux__