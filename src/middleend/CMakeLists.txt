add_library(middleend
    STATIC
        source/const_prop_pass.c
        source/middleend.c
)

target_include_directories(middleend
    PUBLIC
        include
)

target_link_libraries(middleend
    PUBLIC  
        lib_code
        shared
)
