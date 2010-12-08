ACLOCAL_AMFLAGS         	= -I m4
COMMON_CXXFLAGS				= -DBEIDPKCS11_EXPORTS -DBEID_35 -DDEBUG -DLTC_NO_ASM
EXTRA_DIST    		        = VS_2010 VS_2005
if NO_DIALOGS
MAYBE_CPPFLAGS_DIALOGS			= -DNO_DIALOGS
else
MAYBE_LIBADD_DIALOGS   			= ../dialogs/libbeiddialogs.la 
MAYBE_CPPFLAGS_DIALOGS			= -I$(srcdir)/../dialogs/src
endif

COMMON_LIBADD   			= ../common/libbeidcommon.la $(MAYBE_LIBADD_DIALOGS) ../cardlayer/libbeidcardlayer.la

COMMON_CPPFLAGS  			= -I$(srcdir)/../common/src -I$(srcdir)/../cardlayer/src $(MAYBE_CPPFLAGS_DIALOGS) -I$(srcdir)/src/include/rsaref220 -I$(srcdir)/../dialogs/src
noinst_HEADERS				= src/p11.h src/phash.h src/cal.h src/beid_p11.h src/cert.h src/include/rsaref210/win32.h src/include/rsaref210/pkcs11f.h src/include/rsaref210/pkcs11t.h src/include/rsaref210/pkcs11.h src/include/rsaref210/unix.h src/include/rsaref220/win32.h src/include/rsaref220/pkcs11f.h src/include/rsaref220/pkcs11t.h src/include/rsaref220/pkcs11.h src/include/rsaref220/unix.h src/asn1.h src/util.h src/log.h config.h src/display.h

lib_LTLIBRARIES    			= libbeidpkcs11.la 
libbeidpkcs11_la_SOURCES  	= ../common/src/mw_util.cpp src/asn1.c src/cal.cpp src/cert.c src/encrypt.c src/general.c src/log.c src/object.c src/p11.c src/phash.cpp src/session.c src/sign.c src/util.cpp src/display.cpp

if LINUX
#
libbeidpkcs11_la_CPPFLAGS  	= $(COMMON_CPPFLAGS)
libbeidpkcs11_la_CXXFLAGS	= $(COMMON_CXXFLAGS)
libbeidpkcs11_la_LIBADD		= $(COMMON_LIBADD)
#
endif

#################################################################################################################################################################################

if OSX
#
libbeidpkcs11_la_CPPFLAGS  	= $(COMMON_CPPFLAGS) -I/System/Library/Frameworks/PCSC.framework/Headers
libbeidpkcs11_la_CXXFLAGS  	= $(COMMON_CXXFLAGS) -framework PCSC
libbeidpkcs11_la_LIBADD     = $(COMMON_LIBADD)
#
endif

###########################################################################################################################3

if WIN32
#
libbeidpkcs11_la_CPPFLAGS  	= $(COMMON_CPPFLAGS)
libbeidpkcs11_la_LDFLAGS	= -lwinscard -no-undefined -static-libstdc++ -static-libgcc -avoid-version
libbeidpkcs11_la_CXXFLAGS  	= $(COMMON_CXXFLAGS) -municode -DMINGW_HAS_SECURE_API
libbeidpkcs11_la_LIBADD     = $(COMMON_LIBADD)
#
endif
