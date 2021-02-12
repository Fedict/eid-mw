if FUZZING
TESTS = fuzz_id fuzz_add
check_PROGRAMS = $(TESTS)
AM_CFLAGS = -I$(top_srcdir)/doc/sdk/include/rsaref220 -I$(top_srcdir)/cardcomm/pkcs11/src/fuzz -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
if !FUZZ_AFL
AM_CFLAGS += -fsanitize=fuzzer,address
else
AM_CFLAGS += -DFUZZ_AFL
endif

COMMON_LIBS = $(top_builddir)/cardcomm/pkcs11/src/libbeidpkcs11.la $(top_builddir)/tests/unit/libtestlib.la
fuzz_add_SOURCES = fuzz.c
fuzz_add_CFLAGS = $(AM_CFLAGS) -DFUZZ_OBJ="address" -DFUZZ_FILE="3F00DF014033" -I$(top_srcdir)/tests/unit
fuzz_add_LDADD = $(COMMON_LIBS)

fuzz_id_SOURCES = fuzz.c
fuzz_id_CFLAGS = $(AM_CFLAGS) -DFUZZ_OBJ="id" -DFUZZ_FILE="3F00DF014031" -I$(top_srcdir)/tests/unit
fuzz_id_LDADD = $(COMMON_LIBS)
endif
