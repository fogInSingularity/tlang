# sep_str = "# ------------------------------------------------------------------------------\n"

clean_cmd = "rm src/frontend/build/* src/lib_code/build/* src/shared/build/* src/backend_general/build/* src/x86_64_backend/build/*"

main_make_str = """
AST_DOT_FILE ?= dump_tree.dot

all:
\t@echo 'available options> release debug clean'

release:
\t@echo 'Compiler $(CC)'
\tmake -f make_release CC=$(CC) -j8

debug:
\t@echo 'Compiler $(CC)'
\tmake -f make_debug CC=$(CC) -j8
clean:
\t{}

dot_compile:
	@dot $(AST_DOT_FILE) -Tpng -o dump_tree.png
""".format(clean_cmd)
