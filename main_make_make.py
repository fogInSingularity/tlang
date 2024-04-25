# sep_str = "# ------------------------------------------------------------------------------\n"

clean_cmd = "rm src/frontend/build/* src/lib_code/build/*"

main_make_str = """
all:
\t@echo 'available options> release_front debug_front release_mid debug_mid release_back debug_back clean'

release_front:
\tmake -f make_front_debug

debug_front:
\tmake -f make_front_debug

clean:
\t{}

""".format(clean_cmd)

# release_mid:
# \tmake -f make_mid release

# debug_mid:
# \tmake -f make_mid debug

# release_back:
# \tmake -f make_back release

# debug_back:
# \tmake -f make_back debug
