add_library(frontend
    STATIC
        source/ast_builder.c
        source/ast_dump.c
        source/ast_to_ir.c
        source/frontend.c
        source/lexer.c
        source/tree.c
)

target_include_directories(frontend
    PUBLIC
        include
)

target_link_libraries(frontend
    PUBLIC  
        lib_code
        shared
)
