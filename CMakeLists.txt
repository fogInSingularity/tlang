cmake_minimum_required(VERSION 3.20)
project(tlang C)

include(defaults.cmake)

add_subdirectory(src/frontend)
add_subdirectory(src/lib_code)
add_subdirectory(src/shared)
add_subdirectory(src/backend_general)
add_subdirectory(src/x86_64_backend)
add_subdirectory(src/middleend)

add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        frontend
        lib_code
        shared
        backend_general
        x86_64_backend
        middleend
        tlang-defaults
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        m # -lm
)

set(AST_DOT_FILE "dump_tree.dot" CACHE STRING "AST dot file")
add_custom_target(dot_compile
  COMMAND dot ${AST_DOT_FILE} -Tpng -o dump_tree.png
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Generating PNG from ${AST_DOT_FILE}"
)
