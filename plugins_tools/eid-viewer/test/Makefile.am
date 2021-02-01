TESTS = cardevent deserialize serialize init pinop states
check_PROGRAMS = $(TESTS)

export EID_XSDLOC = $(srcdir)/../xml/eidv4.xsd

COMMON_LIB= $(top_builddir)/tests/unit/libtestlib.la $(top_builddir)/plugins_tools/util/liblabels.la $(top_builddir)/cardcomm/pkcs11/src/libbeidpkcs11.la $(builddir)/../libeidviewer.la
COMMON_SRCS = common.h common.c testlib2.h
AM_CFLAGS = -I$(top_srcdir)/tests/unit -I$(top_srcdir)/plugins_tools/util -I$(srcdir)/.. -I$(srcdir)/../include -I$(top_srcdir)/doc/sdk/include/rsaref220 @FUZZING@

cardevent_SOURCES = cardevent.c $(COMMON_SRCS)
cardevent_LDADD = $(COMMON_LIB)

deserialize_SOURCES = deserialize.c $(COMMON_SRCS)
deserialize_CPPFLAGS = -DSRCDIR='"$(srcdir)"'
deserialize_LDADD = $(COMMON_LIB)

serialize_SOURCES = serialize.c $(COMMON_SRCS)
serialize_LDADD = $(COMMON_LIB)

init_SOURCES = init.c $(COMMON_SRCS)
init_LDADD = $(COMMON_LIB)

pinop_SOURCES = pinop.c $(COMMON_SRCS)
pinop_LDADD = $(COMMON_LIB)

states_SOURCES = states.c $(COMMON_SRCS)
states_CPPFLAGS = -DSRCDIR='"$(srcdir)"'
states_LDADD = $(COMMON_LIB)

EXTRA_DIST = 67.06.30-296.59.eid
