# sep_str = "# ------------------------------------------------------------------------------\n"

clean_cmd = "rm src/frontend/build/* src/lib_code/build/* src/shared/build/*"

main_make_str = """
all:
\t@echo 'available options> release_front debug_front release_mid debug_mid release_back debug_back clean'

release:
\t@echo 'Compiler $(CC)'
\tmake -f make_release CC=$(CC) -j8

debug:
\t@echo 'Compiler $(CC)'
\tmake -f make_debug CC=$(CC) -j8
clean:
\t{}

dot_compile:
	@dot dump_tree.dot -Tpng -o dump_tree.png
""".format(clean_cmd)
