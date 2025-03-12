add_library(lib_code
    STATIC
        source/alloc_wraper.c
        source/bin_file.c
        source/darray.c
        source/file_wraper.c
        source/list.c
        source/my_assert.c
        source/recalloc.c
        source/utils.c
)

target_include_directories(lib_code
    PUBLIC
        include
)
