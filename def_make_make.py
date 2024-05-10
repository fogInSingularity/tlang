sep = "\n\n# ------------------------------------------------------------------------------\n\n"

def_frags = """WARNINGS = -Wall -Wextra

FLAGS = -std=c2x -fstack-protector-strong -fcheck-new -fstrict-overflow $(WARNINGS)

ASAN_FLAGS = -fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

O_LEVEL = -O2
MARCH = -march=znver1
LIBRARY = -lm # for clang

DEBUG_FLAGS = $(FLAGS) $(ASAN_FLAGS) -Og $(MARCH) -DDEBUG -ggdb -D_FORTIFY_SOURCE=2
RELEASE_FLAGS = $(FLAGS) $(O_LEVEL) $(MARCH) -DNDEBUG -fomit-frame-pointer -g3 -flto

LINK_FLAGS_DEBUG = $(LIBRARY) $(ASAN_FLAGS) -g -DDEBUG -ggdb -D_FORTIFY_SOURCE=2
LINK_FLAGS_RELEASE = $(LIBRARY) -DNDEBUG -g3 -flto

# profiling:
# PROFILE = -fprofile-use
# VEC_FLAGS_GCC = -fopt-info-vec-optimized -fopt-info-vec-missed
# VEC_FLAGS_CLANG = -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize -Rpass-missed=loop-vectorize -fsave-optimization-record
# -Rpass=.* -Rpass-analysis=.* -Rpass-missed=.* -fsave-optimization-record

# find . -type f -name "*.<file_ext>"
# find . -type f -name "*.<file_ext>" -printf "./build/%f\\n" | sort"""
