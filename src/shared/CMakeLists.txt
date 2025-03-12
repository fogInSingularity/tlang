add_library(shared
    STATIC
        source/flags_parser.c
        source/ir_def_to_str.c
        source/ir_name_table.c
        source/tlang_ir.c
)

target_include_directories(shared
    PUBLIC
        include
)

target_link_libraries(shared
    PUBLIC  
        lib_code
)
