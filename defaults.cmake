set(TARGET tlang-defaults)

add_library(${TARGET} INTERFACE)

target_compile_features(${TARGET}
    INTERFACE
        c_std_11
)

target_compile_options(${TARGET}
    INTERFACE
        -Wall
        -Wextra
        -fstack-protector-strong
        -fstrict-overflow

        $<$<CONFIG:Debug>:
            -Og
            -g3
            -ggdb
            # -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -march=native
            -flto
            -DNDEBUG
        >
)

target_link_options(${TARGET}
    INTERFACE
        $<$<CONFIG:Debug>:
            -Og
            -g3
            -ggdb
            # -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -march=native
            -flto
        >
)

# for correct color output
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${TARGET}
        INTERFACE
            -fdiagnostics-color=always
    )
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # to generate compile_commands.json

