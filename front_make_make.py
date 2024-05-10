import def_make_make

front_make_release_str = ""
front_make_debug_str = ""

front_flags = """
EXE = tlang_front
EXE_ARGS =
EXE_LOG = 2>log
"""

include = "\nINCLUDE = -Isrc/frontend/include/ -Isrc/lib_code/include/\n"

c_files = "src/lib_code/source/utils.c src/lib_code/source/racalloc.c src/lib_code/source/my_assert.c src/lib_code/source/darray.c src/lib_code/source/bin_file.c src/lib_code/source/file_wraper.c src/lib_code/source/tree.c src/frontend/source/main.c src/frontend/source/frontend.c src/frontend/source/lexer.c src/frontend/source/ast_builder.c src/frontend/source/ast_dump.c src/lib_code/source/alloc_wraper.c"
c_objects = "src/lib_code/build/utils.o src/lib_code/build/racalloc.o src/lib_code/build/my_assert.o src/lib_code/build/darray.o src/lib_code/build/bin_file.o src/lib_code/build/file_wraper.o src/lib_code/build/tree.o src/frontend/build/main.o src/frontend/build/frontend.o src/frontend/build/lexer.o src/frontend/build/ast_builder.o src/frontend/build/ast_dump.o src/lib_code/build/alloc_wraper.o"

debug_target = """debug_front: all
all: {}
\t@$(CC) {} -o $(EXE) $(LINK_FLAGS_DEBUG)\n""".format(c_objects, c_objects)

release_target = """release_front: all
all: {}
\t@$(CC) {} -o $(EXE) $(LINK_FLAGS_RELEASE)""".format(c_objects, c_objects)
# ------------------------------------------------------------------------------

# objects for debug: -----------------------------------------------------------

c_obj_debug = ''
c_sources_splited = c_files.split()
c_objects_splited = c_objects.split()

c_ns_files = len(c_sources_splited)
c_no_files = len(c_objects_splited)

if (c_no_files != c_ns_files):
    print("fuck")
    exit(0)

for i in range(c_no_files):
    c_obj_debug += "{}: {}\n\t@$(CC) -c $^ $(DEBUG_FLAGS) $(INCLUDE) -o $@\n\n".format(c_objects_splited[i], c_sources_splited[i])

# objects for release: ---------------------------------------------------------

c_obj_release = ''
c_sources_splited = c_files.split()
c_objects_splited = c_objects.split()

c_ns_files = len(c_sources_splited)
c_no_files = len(c_objects_splited)

if (c_no_files != c_ns_files):
    print("fuck")
    exit(0)

for i in range(c_no_files):
    c_obj_release += "{}: {}\n\t@$(CC) -c $^ $(RELEASE_FLAGS) $(INCLUDE) -o $@\n\n".format(c_objects_splited[i], c_sources_splited[i])

front_make_release_str += front_flags + def_make_make.def_frags + include + def_make_make.sep + release_target + def_make_make.sep + c_obj_release
front_make_debug_str += front_flags + def_make_make.def_frags + include + def_make_make.sep + debug_target + def_make_make.sep + c_obj_debug
