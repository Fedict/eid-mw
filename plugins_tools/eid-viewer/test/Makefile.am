TESTS = cardevent deserialize serialize init
check_PROGRAMS = $(TESTS)

COMMON_LIB= $(top_builddir)/tests/unit/libtestlib.la $(top_builddir)/plugins_tools/util/liblabels.la $(top_builddir)/cardcomm/pkcs11/src/libbeidpkcs11.la $(builddir)/../libeidviewer.la
COMMON_SRCS = common.h common.c
AM_CFLAGS = -I$(top_srcdir)/tests/unit -I$(top_srcdir)/plugins_tools/util -I$(srcdir)/.. -I$(srcdir)/../include -I$(top_srcdir)/doc/sdk/include/rsaref220

cardevent_SOURCES = cardevent.c $(COMMON_SRCS)
cardevent_LDADD = $(COMMON_LIB)

deserialize_SOURCES = deserialize.c $(COMMON_SRCS)
deserialize_LDADD = $(COMMON_LIB)

serialize_SOURCES = serialize.c $(COMMON_SRCS)
serialize_LDADD = $(COMMON_LIB)

init_SOURCES = init.c $(COMMON_SRCS)
init_LDADD = $(COMMON_LIB)
