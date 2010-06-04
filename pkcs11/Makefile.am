ACLOCAL_AMFLAGS         	= -I m4
COMMON_CXXFLAGS				= -DBEIDPKCS11_EXPORTS -DBEID_35 -DDEBUG -DLTC_NO_ASM
COMMON_LIBADD   			= ../common/libbeidcommon.la ../dialogs/libbeiddialogs.la ../cardlayer/libbeidcardlayer.la
AM_CPPFLAGS  				= -I$(srcdir)/../common/src -I$(srcdir)/../cardlayer/src -I$(srcdir)/../dialogs/src -I$(srcdir)/src/include/rsaref220

lib_LTLIBRARIES    			= libbeidpkcs11.la 
libbeidpkcs11_la_SOURCES  	= ../common/src/mw_util.cpp src/asn1.c src/cal.cpp src/cert.c src/encrypt.c src/general.c src/log.c src/object.c src/p11.c src/phash.cpp src/session.c src/sign.c src/util.cpp

if LINUX
#
libbeidpkcs11_la_CPPFLAGS  	= $(PCSCLITE_INCS) $(AM_CPPFLAGS)
libbeidpkcs11_la_LDFLAGS	= $(PCSCLITE_LIBS)
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
libbeidpkcs11_la_CPPFLAGS  	= $(AM_CPPFLAGS)
libbeidpkcs11_la_LDFLAGS	= -lwinscard -no-undefined -static-libstdc++ -static-libgcc -avoid-version
libbeidpkcs11_la_CXXFLAGS  	= $(COMMON_CXXFLAGS) -municode -DMINGW_HAS_SECURE_API
libbeidpkcs11_la_LIBADD     = $(COMMON_LIBADD)
#
endif
