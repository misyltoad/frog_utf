mkdir -p test_builds

export GCC_ARGS="-Wall -Wextra -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement -pedantic"

gcc test.c $GCC_ARGS -std=c89 -o test_builds/test_gcc_c89
gcc test.c $GCC_ARGS -std=gnu89 -o test_builds/test_gcc_gnu89
gcc test.c $GCC_ARGS -std=c99 -o test_builds/test_gcc_c99
gcc test.c $GCC_ARGS -std=gnu99 -o test_builds/test_gcc_gnu99
gcc test.c $GCC_ARGS -std=c11 -o test_builds/test_gcc_c11
gcc test.c $GCC_ARGS -std=gnu11 -o test_builds/test_gcc_gnu11
gcc test.c $GCC_ARGS -std=c17 -o test_builds/test_gcc_c17
gcc test.c $GCC_ARGS -std=gnu17 -o test_builds/test_gcc_gnu17

clang test.c $GCC_ARGS -std=c89 -o test_builds/test_clang_c89
clang test.c $GCC_ARGS -std=gnu89 -o test_builds/test_clang_gnu89
clang test.c $GCC_ARGS -std=c99 -o test_builds/test_clang_c99
clang test.c $GCC_ARGS -std=gnu99 -o test_builds/test_clang_gnu99
clang test.c $GCC_ARGS -std=c11 -o test_builds/test_clang_c11
clang test.c $GCC_ARGS -std=gnu11 -o test_builds/test_clang_gnu11
clang test.c $GCC_ARGS -std=c17 -o test_builds/test_clang_c17
clang test.c $GCC_ARGS -std=gnu17 -o test_builds/test_clang_gnu17

tcc test.c -o test_builds/test_tcc
