add_library(backend_general
    STATIC
        source/backend.c
)

target_include_directories(backend_general
    PUBLIC
        include
)

target_link_libraries(backend_general
    PUBLIC  
        lib_code
        shared
)
