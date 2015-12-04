TESTS = init_finalize wrong_init fork_init double_init getinfo funclist slotlist slotinfo tkinfo slotevent mechlist mechinfo sessions sessions_nocard sessioninfo login login_state nonsensible objects readdata readdata_sequence digest threads sign sign_state ordering
if JPEG
TESTS += decode_photo
endif
check_PROGRAMS = $(TESTS)

CLEANFILES=foto.jpg

COMMON_LIB = $(top_builddir)/cardcomm/pkcs11/src/libbeidpkcs11.la libtestlib.a
noinst_LIBRARIES = libtestlib.a liblabels.a
libtestlib_a_SOURCES = testlib.h testlib.c
liblabels_a_SOURCES = labels.h labels.c

AM_CFLAGS = -I$(top_srcdir)/doc/sdk/include/rsaref220
if NO_DIALOGS
AM_CFLAGS += -DNO_DIALOGS
endif
COMMON_SRCS = testlib.h testlib.c

init_finalize_SOURCES = init_finalize.c
init_finalize_LDADD = $(COMMON_LIB)

getinfo_SOURCES = getinfo.c
getinfo_LDADD = $(COMMON_LIB)

funclist_SOURCES = funclist.c
funclist_LDADD = $(COMMON_LIB)

slotlist_SOURCES = slotlist.c
slotlist_LDADD = $(COMMON_LIB)

slotinfo_SOURCES = slotinfo.c
slotinfo_LDADD = $(COMMON_LIB)

tkinfo_SOURCES = tkinfo.c
tkinfo_LDADD = $(COMMON_LIB)

double_init_SOURCES = double_init.c
double_init_LDADD = $(COMMON_LIB)

fork_init_SOURCES = fork_init.c
fork_init_LDADD = $(COMMON_LIB)

slotevent_SOURCES = slotevent.c
slotevent_LDADD = $(COMMON_LIB)

mechlist_SOURCES = mechlist.c
mechlist_LDADD = $(COMMON_LIB)

mechinfo_SOURCES = mechinfo.c
mechinfo_LDADD = $(COMMON_LIB)

sessions_SOURCES = sessions.c
sessions_LDADD = $(COMMON_LIB)

sessions_nocard_SOURCES = sessions_nocard.c
sessions_nocard_LDADD = $(COMMON_LIB)

sessioninfo_SOURCES = sessioninfo.c
sessioninfo_LDADD = $(COMMON_LIB)

login_SOURCES = login.c
login_LDADD = $(COMMON_LIB)

nonsensible_SOURCES = nonsensible.c
nonsensible_LDADD = $(COMMON_LIB)

login_state_SOURCES = login_state.c $(COMMON_SRCS)
login_state_LDADD = $(COMMON_LIB)

objects_SOURCES = objects.c
objects_LDADD = $(COMMON_LIB)

readdata_SOURCES = readdata.c
readdata_LDADD = $(COMMON_LIB) liblabels.a

digest_SOURCES = digest.c $(COMMON_SRCS)
digest_LDADD = $(COMMON_LIB)

readdata_sequence_SOURCES = readdata_sequence.c $(COMMON_SRCS)
readdata_sequence_LDADD = $(COMMON_LIB)

decode_photo_SOURCES = decode_photo.c
decode_photo_LDADD = $(COMMON_LIB) @JPEG_LIBS@

threads_SOURCES = threads.c
threads_LDADD = $(COMMON_LIB)

sign_SOURCES = sign.c
sign_CFLAGS = @SSL_CFLAGS@ $(AM_CFLAGS)
sign_LDADD = $(COMMON_LIB) @SSL_LIBS@

ordering_SOURCES = ordering.c
ordering_SOURCES = ordering.c $(COMMON_SRCS)

sign_state_SOURCES = sign_state.c $(COMMON_SRCS)
sign_state_LDADD = $(COMMON_LIB)

wrong_init_SOURCES = wrong_init.c
wrong_init_LDADD = $(COMMON_LIB)
