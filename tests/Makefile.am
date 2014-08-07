TESTS = init_finalize fork_init double_init getinfo funclist slotlist slotinfo tkinfo slotevent mechlist mechinfo sessions sessions_nocard sessioninfo login
check_PROGRAMS = $(TESTS)

AM_CFLAGS = -I$(top_srcdir)/sdk/Pkcs11/include/rsaref220
COMMON_SRCS = testlib.h testlib.c main.c

init_finalize_SOURCES = init_finalize.c $(COMMON_SRCS)
init_finalize_CPPFLAGS = -DTESTS_TO_RUN=TEST_INIT_FINALIZE
init_finalize_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

getinfo_SOURCES = getinfo.c $(COMMON_SRCS)
getinfo_CPPFLAGS = -DTESTS_TO_RUN=TEST_GETINFO
getinfo_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

funclist_SOURCES = funclist.c $(COMMON_SRCS)
funclist_CPPFLAGS = -DTESTS_TO_RUN=TEST_FUNCLIST
funclist_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

slotlist_SOURCES = slotlist.c $(COMMON_SRCS)
slotlist_CPPFLAGS = -DTESTS_TO_RUN=TEST_SLOTLIST
slotlist_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

slotinfo_SOURCES = slotinfo.c $(COMMON_SRCS)
slotinfo_CPPFLAGS = -DTESTS_TO_RUN=TEST_SLOTINFO
slotinfo_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

tkinfo_SOURCES = tkinfo.c $(COMMON_SRCS)
tkinfo_CPPFLAGS = -DTESTS_TO_RUN=TEST_TKINFO
tkinfo_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

double_init_SOURCES = double_init.c $(COMMON_SRCS)
double_init_CPPFLAGS = -DTESTS_TO_RUN=TEST_DOUBLE_INIT
double_init_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

fork_init_SOURCES = fork_init.c $(COMMON_SRCS)
fork_init_CPPFLAGS = -DTESTS_TO_RUN=TEST_FORK_INIT
fork_init_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

slotevent_SOURCES = slotevent.c $(COMMON_SRCS)
slotevent_CPPFLAGS = -DTESTS_TO_RUN=TEST_SLOTEVENT
slotevent_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

mechlist_SOURCES = mechlist.c $(COMMON_SRCS)
mechlist_CPPFLAGS = -DTESTS_TO_RUN=TEST_MECHLIST
mechlist_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

mechinfo_SOURCES = mechinfo.c $(COMMON_SRCS)
mechinfo_CPPFLAGS = -DTESTS_TO_RUN=TEST_MECHINFO
mechinfo_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

sessions_SOURCES = sessions.c $(COMMON_SRCS)
sessions_CPPFLAGS = -DTESTS_TO_RUN=TEST_SESSIONS
sessions_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

sessions_nocard_SOURCES = sessions_nocard.c $(COMMON_SRCS)
sessions_nocard_CPPFLAGS = -DTESTS_TO_RUN=TEST_SESSIONS_NOCARD
sessions_nocard_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

sessioninfo_SOURCES = sessioninfo.c $(COMMON_SRCS)
sessioninfo_CPPFLAGS = -DTESTS_TO_RUN=TEST_SESSIONINFO
sessioninfo_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la

login_SOURCES = login.c $(COMMON_SRCS)
login_CPPFLAGS = -DTESTS_TO_RUN=TEST_LOGIN
login_LDADD = $(top_builddir)/pkcs11/libbeidpkcs11.la
