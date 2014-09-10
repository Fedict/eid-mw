TESTS = init_finalize fork_init double_init getinfo funclist slotlist slotinfo tkinfo slotevent mechlist mechinfo sessions sessions_nocard sessioninfo login nonsensible objects readdata digest threads sign
if JPEG
TESTS += decode_photo
endif
check_PROGRAMS = $(TESTS)

EXTRA_DIST = main.c

AM_CFLAGS = -I$(top_srcdir)/sdk/Pkcs11/include/rsaref220 -DTESTS_TO_RUN=0
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

mechlist_SOURCES = mechlist.c $(COMMON_SRCS)
mechlist_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

mechinfo_SOURCES = mechinfo.c $(COMMON_SRCS)
mechinfo_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

sessions_SOURCES = sessions.c $(COMMON_SRCS)
sessions_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

sessions_nocard_SOURCES = sessions_nocard.c $(COMMON_SRCS)
sessions_nocard_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

sessioninfo_SOURCES = sessioninfo.c $(COMMON_SRCS)
sessioninfo_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

login_SOURCES = login.c $(COMMON_SRCS)
login_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

nonsensible_SOURCES = nonsensible.c $(COMMON_SRCS)
nonsensible_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

objects_SOURCES = objects.c $(COMMON_SRCS)
objects_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

readdata_SOURCES = readdata.c $(COMMON_SRCS)
readdata_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

digest_SOURCES = digest.c $(COMMON_SRCS)
digest_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

decode_photo_SOURCES = decode_photo.c $(COMMON_SRCS)
decode_photo_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la @JPEG_LIBS@

threads_SOURCES = threads.c $(COMMON_SRCS)
threads_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

sign_SOURCES = sign.c $(COMMON_SRCS)
sign_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la @SSL_LIBS@
