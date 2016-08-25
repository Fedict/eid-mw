TESTS = init_finalize wrong_init fork_init double_init getinfo funclist slotlist slotinfo tkinfo slotevent mechlist mechinfo sessions sessions_nocard sessioninfo login login_state nonsensible objects readdata readdata_sequence digest threads sign sign_state ordering
if JPEG
TESTS += decode_photo
endif
check_PROGRAMS = $(TESTS)

CLEANFILES=foto.jpg

COMMON_LIB = $(top_builddir)/cardcomm/pkcs11/src/libbeidpkcs11.la libtestlib.la
noinst_LTLIBRARIES = libtestlib.la
libtestlib_la_SOURCES = testlib.h testlib.c

AM_CFLAGS = -I$(top_srcdir)/doc/sdk/include/rsaref220 -I$(top_srcdir)/plugins_tools/util
if NO_DIALOGS
AM_CFLAGS += -DNO_DIALOGS
endif
COMMON_SRCS = testlib.h testlib.c

init_finalize_SOURCES = init_finalize.c $(COMMON_SRCS)
init_finalize_LDADD = $(COMMON_LIB)

getinfo_SOURCES = getinfo.c $(COMMON_SRCS)
getinfo_LDADD = $(COMMON_LIB)

funclist_SOURCES = funclist.c $(COMMON_SRCS)
funclist_LDADD = $(COMMON_LIB)

slotlist_SOURCES = slotlist.c $(COMMON_SRCS)
slotlist_LDADD = $(COMMON_LIB)

slotinfo_SOURCES = slotinfo.c $(COMMON_SRCS)
slotinfo_LDADD = $(COMMON_LIB)

tkinfo_SOURCES = tkinfo.c $(COMMON_SRCS)
tkinfo_LDADD = $(COMMON_LIB)

double_init_SOURCES = double_init.c $(COMMON_SRCS)
double_init_LDADD = $(COMMON_LIB)

fork_init_SOURCES = fork_init.c $(COMMON_SRCS)
fork_init_LDADD = $(COMMON_LIB)

slotevent_SOURCES = slotevent.c $(COMMON_SRCS)
slotevent_LDADD = $(COMMON_LIB)

mechlist_SOURCES = mechlist.c $(COMMON_SRCS)
mechlist_LDADD = $(COMMON_LIB)

mechinfo_SOURCES = mechinfo.c $(COMMON_SRCS)
mechinfo_LDADD = $(COMMON_LIB)

sessions_SOURCES = sessions.c $(COMMON_SRCS)
sessions_LDADD = $(COMMON_LIB)

sessions_nocard_SOURCES = sessions_nocard.c $(COMMON_SRCS)
sessions_nocard_LDADD = $(COMMON_LIB)

sessioninfo_SOURCES = sessioninfo.c $(COMMON_SRCS)
sessioninfo_LDADD = $(COMMON_LIB)

login_SOURCES = login.c $(COMMON_SRCS)
login_LDADD = $(COMMON_LIB)

nonsensible_SOURCES = nonsensible.c $(COMMON_SRCS)
nonsensible_LDADD = $(COMMON_LIB)

login_state_SOURCES = login_state.c $(COMMON_SRCS)
login_state_LDADD = $(COMMON_LIB)

objects_SOURCES = objects.c $(COMMON_SRCS)
objects_LDADD = $(COMMON_LIB)

readdata_SOURCES = readdata.c $(COMMON_SRCS)
readdata_LDADD = $(COMMON_LIB) $(top_builddir)/plugins_tools/util/liblabels.la

digest_SOURCES = digest.c $(COMMON_SRCS)
digest_LDADD = $(COMMON_LIB)

readdata_sequence_SOURCES = readdata_sequence.c $(COMMON_SRCS)
readdata_sequence_LDADD = $(COMMON_LIB)

decode_photo_SOURCES = decode_photo.c $(COMMON_SRCS)
decode_photo_LDADD = $(COMMON_LIB) @JPEG_LIBS@

threads_SOURCES = threads.c $(COMMON_SRCS)
threads_LDADD = $(COMMON_LIB)

sign_SOURCES = sign.c $(COMMON_SRCS)
sign_CFLAGS = @SSL_CFLAGS@ $(AM_CFLAGS)
sign_LDADD = $(COMMON_LIB) @SSL_LIBS@

ordering_SOURCES = ordering.c $(COMMON_SRCS)
ordering_LDADD = $(COMMON_LIB)

sign_state_SOURCES = sign_state.c $(COMMON_SRCS)
sign_state_LDADD = $(COMMON_LIB)

wrong_init_SOURCES = wrong_init.c $(COMMON_SRCS)
wrong_init_LDADD = $(COMMON_LIB)
