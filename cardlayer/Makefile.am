ACLOCAL_AMFLAGS         				= -I m4
COMMON_CXXFLAGS							= -DEIDMW_CAL_EXPORT -DCAL_BEID -DCARDPLUGIN_IN_CAL -DBEID_35 -DNDEBUG -DBEID_OLD_PINPAD -DLTC_NO_ASM # -DPKCS11_V2_20 -DPKCS11_FF
EXTRA_DIST          					= VS_2010 VS_2005
if NO_DIALOGS
COMMON_LIBADD     						= ../common/libbeidcommon.la
MAYBE_CPPFLAGS_DIALOGS                  = -DNO_DIALOGS
else
COMMON_LIBADD     						= ../common/libbeidcommon.la ../dialogs/libbeiddialogs.la
endif
COMMON_CPPFLAGS							= $(MAYBE_CPPFLAGS_DIALOGS)
noinst_HEADERS							= old/scr.h old/pinpadliboldbeid.h src/cardlayer.h src/pkcs15parser.h src/internalconst.h src/card.h src/cardfactory.h src/pkcs15.h src/cache.h src/context.h src/cardpluginbeid/beidp15correction.h src/cardpluginbeid/beidcard.h src/cardreaderinfo.h src/pinpadlib.h src/threadpool.h src/pcsc.h src/cardlayerconst.h src/cardpluginsis/siscard.h src/cardpluginsis/sispluginhandler.h src/cardpluginsis/sisplugin1.h src/pinpad2.h src/pinpad.h src/p15objects.h src/p15correction.h src/readersinfo.h src/pkicard.h src/unknowncard.h src/reader.h config.h

lib_LTLIBRARIES    						= libbeidcardlayer.la libcardpluginbeid.la libcardpluginsis.la libcardpluginsis_acr38u.la

if LINUX

libbeidcardlayer_la_SOURCES  			= ../common/src/mw_util.cpp src/cache.cpp src/card.cpp src/cardfactory.cpp src/cardlayer.cpp src/cardreaderinfo.cpp src/context.cpp src/p15correction.cpp src/pcsc.cpp src/pinpad.cpp src/pinpadlib.cpp src/pkcs15.cpp src/pkcs15parser.cpp src/pkicard.cpp src/reader.cpp src/readersinfo.cpp src/threadpool.cpp src/unknowncard.cpp src/cardpluginbeid/beidcard.cpp src/cardpluginbeid/beidp15correction.cpp src/cardpluginsis/siscard.cpp src/cardpluginsis/sispluginhandler.cpp
libbeidcardlayer_la_CPPFLAGS   			= $(COMMON_CPPFLAGS) -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginbeid -I$(srcdir)/src/cardpluginsis -I$(srcdir)/src/cardpluginsis_acr38u
libbeidcardlayer_la_LDFLAGS				= -avoid-version
libbeidcardlayer_la_LIBADD				= $(COMMON_LIBADD)
libbeidcardlayer_la_CXXFLAGS			= $(COMMON_CXXFLAGS)

libcardpluginbeid_la_SOURCES  			= ../common/src/mw_util.cpp src/cardpluginbeid/beidcard.cpp src/cardpluginbeid/beidp15correction.cpp src/cache.cpp src/card.cpp src/pkicard.cpp
libcardpluginbeid_la_CPPFLAGS  			= $(COMMON_CPPFLAGS) -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginbeid 
libcardpluginbeid_la_LDFLAGS			= -avoid-version
libcardpluginbeid_la_LIBADD             = $(COMMON_LIBADD) libbeidcardlayer.la
libcardpluginbeid_la_CXXFLAGS           = $(COMMON_CXXFLAGS)

libcardpluginsis_la_SOURCES  			= ../common/src/mw_util.cpp src/cardpluginsis/siscard.cpp src/cache.cpp src/card.cpp
libcardpluginsis_la_CPPFLAGS  			= $(COMMON_CPPFLAGS) -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginsis 
libcardpluginsis_la_LDFLAGS				= -avoid-version
libcardpluginsis_la_LIBADD              = $(COMMON_LIBADD) libbeidcardlayer.la
libcardpluginsis_la_CXXFLAGS            = $(COMMON_CXXFLAGS)

libcardpluginsis_acr38u_la_SOURCES  	= ../common/src/mw_util.cpp src/cardpluginsis_acr38u/siscard.cpp 
libcardpluginsis_acr38u_la_CPPFLAGS  	= $(COMMON_CPPFLAGS) -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginsis_acr38u
libcardpluginsis_acr38u_la_LDFLAGS		= -avoid-version
libcardpluginsis_acr38u_la_CXXFLAGS  	= $(COMMON_CXXFLAGS) -DCARDPLUGINSIS_EXPORTS -DEIDMW_CMN_EXPORT
libcardpluginsis_acr38u_la_LIBADD       = $(COMMON_LIBADD) libbeidcardlayer.la

endif

###############################################################################################################################################################################

if OSX

libbeidcardlayer_la_SOURCES  			= ../common/src/mw_util.cpp src/cache.cpp src/card.cpp src/cardfactory.cpp src/cardlayer.cpp src/cardreaderinfo.cpp src/context.cpp src/p15correction.cpp src/pcsc.cpp src/pinpad.cpp src/pinpadlib.cpp src/pkcs15.cpp src/pkcs15parser.cpp src/pkicard.cpp src/reader.cpp src/readersinfo.cpp src/threadpool.cpp src/unknowncard.cpp src/cardpluginbeid/beidcard.cpp src/cardpluginbeid/beidp15correction.cpp src/cardpluginsis/siscard.cpp src/cardpluginsis/sispluginhandler.cpp
libbeidcardlayer_la_CPPFLAGS   			= $(COMMON_CPPFLAGS) -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginbeid -I$(srcdir)/src/cardpluginsis -I$(srcdir)/src/cardpluginsis_acr38u -I/System/Library/Frameworks/PCSC.framework/Headers/
libbeidcardlayer_la_LDFLAGS				= -avoid-version
libbeidcardlayer_la_CXXFLAGS   			= $(COMMON_CXXFLAGS) -framework PCSC
libbeidcardlayer_la_LIBADD       		= $(COMMON_LIBADD)

libcardpluginbeid_la_SOURCES  			= ../common/src/mw_util.cpp src/cardpluginbeid/beidcard.cpp src/cardpluginbeid/beidp15correction.cpp src/cache.cpp src/card.cpp src/pkicard.cpp
libcardpluginbeid_la_CPPFLAGS  			= -I$(srcdir)/src -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginbeid -I/System/Library/Frameworks/PCSC.framework/Headers/
libcardpluginbeid_la_LDFLAGS			= -avoid-version
libcardpluginbeid_la_CXXFLAGS  			= $(COMMON_CXXFLAGS) -framework PCSC
libcardpluginbeid_la_LIBADD             = $(COMMON_LIBADD) libbeidcardlayer.la

libcardpluginsis_la_SOURCES  			= ../common/src/mw_util.cpp src/cardpluginsis/siscard.cpp src/cache.cpp src/card.cpp
libcardpluginsis_la_CPPFLAGS  			= -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginsis -I/System/Library/Frameworks/PCSC.framework/Headers/
libcardpluginsis_la_LDFLAGS				= -avoid-version
libcardpluginsis_la_CXXFLAGS  			= $(COMMON_CXXFLAGS) -framework PCSC
libcardpluginsis_la_LIBADD              = $(COMMON_LIBADD) libbeidcardlayer.la

libcardpluginsis_acr38u_la_SOURCES  	= ../common/src/mw_util.cpp src/cardpluginsis_acr38u/siscard.cpp 
libcardpluginsis_acr38u_la_CPPFLAGS  	= $(COMMON_CPPFLAGS) -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginsis_acr38u -I/System/Library/Frameworks/PCSC.framework/Headers/
libcardpluginsis_acr38u_la_LDFLAGS		= -avoid-version
libcardpluginsis_acr38u_la_CXXFLAGS  	= $(COMMON_CXXFLAGS) -DCARDPLUGINSIS_EXPORTS -DEIDMW_CMN_EXPORT -framework PCSC
libcardpluginsis_acr38u_la_LIBADD       = $(COMMON_LIBADD) libbeidcardlayer.la

endif

##############################################################################################################################################################################

if WIN32



libbeidcardlayer_la_SOURCES  			= ../common/src/mw_util.cpp src/cache.cpp src/card.cpp src/cardfactory.cpp src/cardlayer.cpp src/cardreaderinfo.cpp src/context.cpp src/p15correction.cpp src/pcsc.cpp src/pinpad.cpp src/pinpadlib.cpp src/pkcs15.cpp src/pkcs15parser.cpp src/pkicard.cpp src/reader.cpp src/readersinfo.cpp src/threadpool.cpp src/unknowncard.cpp old/pinpadliboldbeid.cpp src/cardpluginbeid/beidcard.cpp src/cardpluginbeid/beidp15correction.cpp src/cardpluginsis/siscard.cpp src/cardpluginsis/sispluginhandler.cpp
libbeidcardlayer_la_CPPFLAGS   			= $(COMMON_CPPFLAGS) -I$(srcdir)/../common -I$(srcdir)/src/cardpluginbeid -I$(srcdir)/src/cardpluginsis -I$(srcdir)/src/cardpluginsis_acr38u
libbeidcardlayer_la_LDFLAGS    			= -lwinscard -no-undefined -shared -static-libstdc++ -static-libgcc -avoid-version
libbeidcardlayer_la_CXXFLAGS   			= $(COMMON_CXXFLAGS) -municode -DMINGW_HAS_SECURE_API 
libbeidcardlayer_la_LIBADD              = $(COMMON_LIBADD)

libcardpluginbeid_la_SOURCES  			= ../common/src/mw_util.cpp src/cardpluginbeid/beidcard.cpp src/cardpluginbeid/beidp15correction.cpp src/cache.cpp src/card.cpp src/pkicard.cpp
libcardpluginbeid_la_CPPFLAGS  			= $(COMMON_CPPFLAGS) -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginbeid 
libcardpluginbeid_la_LDFLAGS   			= -no-undefined -shared -static-libstdc++ -static-libgcc -avoid-version
libcardpluginbeid_la_CXXFLAGS  			= $(COMMON_CXXFLAGS) -municode -DMINGW_HAS_SECURE_API
libcardpluginbeid_la_LIBADD             = $(COMMON_LIBADD) libbeidcardlayer.la

libcardpluginsis_la_SOURCES  			= ../common/src/mw_util.cpp src/cardpluginsis/siscard.cpp src/cache.cpp src/card.cpp
libcardpluginsis_la_CPPFLAGS  			= $(COMMON_CPPFLAGS) -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginsis 
libcardpluginsis_la_LDFLAGS   			= -no-undefined -static-libstdc++ -static-libgcc -shared -avoid-version
libcardpluginsis_la_CXXFLAGS  			= $(COMMON_CXXFLAGS) -municode -DMINGW_HAS_SECURE_API
libcardpluginsis_la_LIBADD              = $(COMMON_LIBADD) libbeidcardlayer.la

libcardpluginsis_acr38u_la_SOURCES  	= ../common/src/mw_util.cpp src/cardpluginsis_acr38u/siscard.cpp 
libcardpluginsis_acr38u_la_CPPFLAGS  	= $(COMMON_CPPFLAGS) -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginsis_acr38u
libcardpluginsis_acr38u_la_LDFLAGS   	= -no-undefined -shared -static-libstdc++ -static-libgcc -avoid-version
libcardpluginsis_acr38u_la_CXXFLAGS  	= $(COMMON_CXXFLAGS) -municode -DMINGW_HAS_SECURE_API -DCARDPLUGINSIS_EXPORTS -DEIDMW_CMN_EXPORT
libcardpluginsis_acr38u_la_LIBADD       = $(COMMON_LIBADD) libbeidcardlayer.la

# add dll to all-local to include test for dll creation
# libtool will not fail if it is unable to create a dll (eg because of missing libs)
all-local: .libs/libbeidcardlayer.dll .libs/libcardpluginbeid.dll .libs/libcardpluginsis.dll .libs/libcardpluginsis_acr38u.dll

# simple test if dlls exist	
.libs/libbeidcardlayer.dll: libbeidcardlayer.la
	test -e $@

.libs/libcardpluginbeid.dll: libcardpluginbeid.la
	test -e $@

.libs/libcardpluginsis.dll: libcardpluginsis.la
	test -e $@

.libs/libcardpluginsis_acr38u.dll: libcardpluginsis_acr38u.la
	test -e $@
endif
