TESTS = cardcom_common
if JPEG
TESTS += decode_photo
endif
check_PROGRAMS = $(TESTS)

CLEANFILES=foto.jpg

COMMON_LIB = libtestlib.la $(top_builddir)/cardcomm/pkcs11/src/libbeidpkcs11.la
noinst_LTLIBRARIES = libtestlib.la
libtestlib_la_SOURCES = testlib.h testlib.c serial_linux.c serial_io.h

AM_CFLAGS = -I$(top_srcdir)/doc/sdk/include/v240 -I$(top_srcdir)/plugins_tools/util @FUZZING@
if NO_DIALOGS
AM_CFLAGS += -DNO_DIALOGS
endif
AM_CXXFLAGS = $(AM_CFLAGS)

cardcom_common_SOURCES = cardcom_common.cpp
cardcom_common_CXXFLAGS = -I$(top_srcdir)/cardcomm/pkcs11/src $(AM_CXXFLAGS)
cardcom_common_LDADD = $(COMMON_LIB)
