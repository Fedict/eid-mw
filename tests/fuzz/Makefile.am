if FUZZING
TESTS = fuzz_id
endif
check_PROGRAMS = $(TESTS)
AM_CFLAGS = -I$(top_srcdir)/doc/sdk/include/rsaref220 -I$(top_srcdir)/cardcomm/pkcs11/src -fsanitize=fuzzer -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION

fuzz_id_SOURCES = fuzz.c
fuzz_id_CFLAGS = $(AM_CFLAGS) -DFUZZ_OBJ="id" -DFUZZ_FILE="3F00DF014031"
fuzz_id_LDADD = $(top_builddir)/cardcomm/pkcs11/src/libbeidpkcs11.la
