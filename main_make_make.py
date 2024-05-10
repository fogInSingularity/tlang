# sep_str = "# ------------------------------------------------------------------------------\n"

clean_cmd = "rm src/frontend/build/* src/lib_code/build/*"

main_make_str = """
all:
\t@echo 'available options> release_front debug_front release_mid debug_mid release_back debug_back clean'

release_front:
\t@echo 'Compiler $(CC)'
\tmake -f make_front_release CC=$(CC) -j8

debug_front:
\t@echo 'Compiler $(CC)'
\tmake -f make_front_debug CC=$(CC) -j8

clean:
\t{}

dot_compile:
	@dot dump_tree.dot -Tpng -o dump_tree.png
""".format(clean_cmd)

# release_mid:
# \tmake -f make_mid release

# debug_mid:
# \tmake -f make_mid debug

# release_back:
# \tmake -f make_back release

# debug_back:
# \tmake -f make_back debug
