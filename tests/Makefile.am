TESTS = init_finalize fork_init double_init getinfo funclist slotlist slotinfo tkinfo slotevent
check_PROGRAMS = $(TESTS)

AM_CFLAGS = -I$(top_srcdir)/sdk/Pkcs11/include/rsaref220
COMMON_SRCS = testlib.h testlib.c

init_finalize_SOURCES = init_finalize.c $(COMMON_SRCS)
init_finalize_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

getinfo_SOURCES = getinfo.c $(COMMON_SRCS)
getinfo_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

funclist_SOURCES = funclist.c $(COMMON_SRCS)
funclist_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

slotlist_SOURCES = slotlist.c $(COMMON_SRCS)
slotlist_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

slotinfo_SOURCES = slotinfo.c $(COMMON_SRCS)
slotinfo_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

tkinfo_SOURCES = tkinfo.c $(COMMON_SRCS)
tkinfo_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

double_init_SOURCES = double_init.c $(COMMON_SRCS)
double_init_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

fork_init_SOURCES = fork_init.c $(COMMON_SRCS)
fork_init_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

slotevent_SOURCES = slotevent.c $(COMMON_SRCS)
slotevent_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la
