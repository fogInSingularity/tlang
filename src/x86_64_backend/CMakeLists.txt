add_library(x86_64_backend
    STATIC
        source/x86_64_backend.c
)

target_include_directories(x86_64_backend
    PUBLIC
        include
)

target_link_libraries(x86_64_backend
    PUBLIC  
        lib_code
        shared
        backend_general
)
