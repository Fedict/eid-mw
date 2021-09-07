# The eject_card must occur first. It's a workaround for the fact that the
# machine on which the automated test suite runs (with a robot to eject the
# card and/or the USB device) sometimes puts the card reader in an
# unrecoverable sleep, which then causes the full test suite to fail. By
# ejecting both the card and the reader, we ensure that the reader is
# definitely not in unrecoverable sleep, and the first test that needs the
# reader will just insert the card and/or the reader if necessary.
TESTS =				\
	eject_card		\
	bad_login		\
	cardcom_common		\
	digest			\
	double_init		\
	exception_data		\
	fork_init		\
	funclist		\
	getinfo			\
	init_finalize		\
	login			\
	login_state		\
	mechinfo		\
	mechlist		\
	nonsensible		\
	objects			\
	ordering		\
	readdata		\
	readdata_limited	\
	readdata_sequence	\
	sessioninfo		\
	sessions		\
	sessions_nocard		\
	sign			\
	sign_state		\
	#slotevent		\
	slotinfo		\
	slotlist		\
	threads			\
	tkinfo			\
	wrong_init

if !NO_DIALOGS
TESTS += dialogs
endif
if JPEG
TESTS += decode_photo
endif
check_PROGRAMS = $(TESTS)

CLEANFILES=foto.jpg junit.xml

P11_LIB = libtestlib.la $(top_builddir)/cardcomm/pkcs11/src/libbeidpkcs11.la
COMMON_LIB = $(top_builddir)/cardcomm/pkcs11/src/libbeidcommon.la
DIALOGLIB = $(top_builddir)/cardcomm/pkcs11/src/libdialogs.la
noinst_LTLIBRARIES = libtestlib.la
libtestlib_la_SOURCES = testlib.h testlib.c serial_linux.c serial_io.h

AM_CFLAGS = -I$(top_srcdir)/doc/sdk/include/v240 -I$(top_srcdir)/plugins_tools/util @FUZZING@
if NO_DIALOGS
AM_CFLAGS += -DNO_DIALOGS
endif
AM_CXXFLAGS = $(AM_CFLAGS)

eject_card_SOURCES = eject.c
eject_card_LDADD = $(P11_LIB)

init_finalize_SOURCES = init_finalize.c
init_finalize_LDADD = $(P11_LIB)

getinfo_SOURCES = getinfo.c
getinfo_LDADD = $(P11_LIB)

funclist_SOURCES = funclist.c
funclist_LDADD = $(P11_LIB)

slotlist_SOURCES = slotlist.c
slotlist_LDADD = $(P11_LIB)

slotinfo_SOURCES = slotinfo.c
slotinfo_LDADD = $(P11_LIB)

tkinfo_SOURCES = tkinfo.c
tkinfo_LDADD = $(P11_LIB)

double_init_SOURCES = double_init.c
double_init_LDADD = $(P11_LIB)

fork_init_SOURCES = fork_init.c
fork_init_LDADD = $(P11_LIB)

slotevent_SOURCES = slotevent.c
slotevent_LDADD = $(P11_LIB)

mechlist_SOURCES = mechlist.c
mechlist_LDADD = $(P11_LIB)

mechinfo_SOURCES = mechinfo.c
mechinfo_LDADD = $(P11_LIB)

sessions_SOURCES = sessions.c
sessions_LDADD = $(P11_LIB)

sessions_nocard_SOURCES = sessions_nocard.c
sessions_nocard_LDADD = $(P11_LIB)

sessioninfo_SOURCES = sessioninfo.c
sessioninfo_LDADD = $(P11_LIB)

login_SOURCES = login.c
login_LDADD = $(P11_LIB)

nonsensible_SOURCES = nonsensible.c
nonsensible_LDADD = $(P11_LIB)

login_state_SOURCES = login_state.c
login_state_LDADD = $(P11_LIB)

objects_SOURCES = objects.c
objects_LDADD = $(P11_LIB)

readdata_SOURCES = readdata.c
readdata_LDADD = $(P11_LIB) $(top_builddir)/plugins_tools/util/liblabels.la

readdata_limited_SOURCES = readdata_limited.c
readdata_limited_LDADD = $(P11_LIB) $(top_builddir)/plugins_tools/util/liblabels.la

digest_SOURCES = digest.c
digest_LDADD = $(P11_LIB)

readdata_sequence_SOURCES = readdata_sequence.c
readdata_sequence_LDADD = $(P11_LIB)

decode_photo_SOURCES = decode_photo.c
decode_photo_LDADD = $(P11_LIB) @JPEG_LIBS@
decode_photo_CFLAGS = @JPEG_CFLAGS@ $(AM_CFLAGS)

threads_SOURCES = threads.c
threads_LDADD = $(P11_LIB)

sign_SOURCES = sign.c
sign_CFLAGS = @SSL_CFLAGS@ $(AM_CFLAGS)
sign_LDADD = $(P11_LIB) @SSL_LIBS@

ordering_SOURCES = ordering.c
ordering_LDADD = $(P11_LIB)

sign_state_SOURCES = sign_state.c
sign_state_LDADD = $(P11_LIB)

wrong_init_SOURCES = wrong_init.c
wrong_init_LDADD = $(P11_LIB)

cardcom_common_SOURCES = cardcom_common.cpp
cardcom_common_CXXFLAGS = -I$(top_srcdir)/cardcomm/pkcs11/src $(AM_CXXFLAGS)
cardcom_common_LDADD = $(P11_LIB) $(COMMON_LIB)

bad_login_SOURCES = bad_login.c
bad_login_LDADD = $(P11_LIB)

dialogs_SOURCES = dialogs.cpp
dialogs_CXXFLAGS = -I$(top_srcdir)/cardcomm/pkcs11/src $(AM_CXXFLAGS)
dialogs_LDADD = $(P11_LIB) $(COMMON_LIB) $(DIALOGLIB)

exception_data_SOURCES = exception_data.cpp
exception_data_CXXFLAGS = -I$(top_srcdir)/cardcomm/pkcs11/src $(AM_CXXFLAGS)
exception_data_LDADD = $(P11_LIB) $(COMMON_LIB)
