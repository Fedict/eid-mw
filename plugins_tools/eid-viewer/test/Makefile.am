TESTS = cardevent deserialize serialize
check_PROGRAMS = $(TESTS)

COMMON_LIB=$(top_builddir)/cardcomm/pkcs11/src/libbeidpkcs11.la $(top_builddir)/tests/unit/libtestlib.a
COMMON_SRCS = ../backend.c ../oslayer.h common.h common.c
AM_CFLAGS = -I$(top_srcdir)/tests/unit -I$(srcdir)/..

cardevent_SOURCES = cardevent.c $(COMMON_SRCS)
cardevent_CPPFLAGS = -I$(top_srcdir)/tests/unit -I$(top_srcdir)/doc/sdk/include/rsaref220
cardevent_LDADD = $(COMMON_LIB)

deserialize_SOURCES = deserialize.c $(COMMON_SRCS)
deserialize_CPPFLAGS = -I$(top_srcdir)/tests/unit -I$(top_srcdir)/doc/sdk/include/rsaref220
deserialize_LDADD = $(COMMON_LIB)

serialize_SOURCES = serialize.c $(COMMON_SRCS)
serialize_CPPFLAGS = -I$(top_srcdir)/tests/unit -I$(top_srcdir)/doc/sdk/include/rsaref220
serialize_LDADD = $(COMMON_LIB)
