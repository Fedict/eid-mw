ACLOCAL_AMFLAGS         				= -I m4
COMMON_CXXFLAGS							= -DEIDMW_CAL_EXPORT -DCAL_BEID -DCARDPLUGIN_IN_CAL -DBEID_35 -DNDEBUG -DBEID_OLD_PINPAD -DLTC_NO_ASM 
if NO_DIALOGS
COMMON_LIBADD     						= ../common/libbeidcommon.la
MAYBE_CPPFLAGS_DIALOGS                  = -DNO_DIALOGS
else
COMMON_LIBADD     						= ../common/libbeidcommon.la ../dialogs/libbeiddialogs.la
endif
COMMON_CPPFLAGS							= $(MAYBE_CPPFLAGS_DIALOGS)
noinst_HEADERS							= old/scr.h old/pinpadliboldbeid.h src/cardlayer.h src/pkcs15parser.h src/internalconst.h src/card.h src/cardfactory.h src/pkcs15.h src/cache.h src/context.h src/cardpluginbeid/beidp15correction.h src/cardpluginbeid/beidcard.h src/cardreaderinfo.h src/pinpadlib.h src/threadpool.h src/pcsc.h src/cardlayerconst.h src/cardpluginsis/siscard.h src/cardpluginsis/sispluginhandler.h src/cardpluginsis/sisplugin1.h src/pinpad2.h src/pinpad.h src/p15objects.h src/p15correction.h src/readersinfo.h src/pkicard.h src/unknowncard.h src/reader.h config.h

lib_LTLIBRARIES    						= libbeidcardlayer.la
libbeidcardlayer_la_SOURCES  			= ../common/src/mw_util.cpp src/cache.cpp src/card.cpp src/cardfactory.cpp src/cardlayer.cpp src/cardreaderinfo.cpp src/context.cpp src/p15correction.cpp src/pcsc.cpp src/pinpad.cpp src/pinpadlib.cpp src/pkcs15.cpp src/pkcs15parser.cpp src/pkicard.cpp src/reader.cpp src/readersinfo.cpp src/threadpool.cpp src/unknowncard.cpp src/cardpluginbeid/beidcard.cpp src/cardpluginbeid/beidp15correction.cpp src/cardpluginsis/siscard.cpp src/cardpluginsis/sispluginhandler.cpp
libbeidcardlayer_la_CPPFLAGS   			= $(COMMON_CPPFLAGS) -I$(srcdir)/../common/src -I$(srcdir)/src/cardpluginbeid -I$(srcdir)/src/cardpluginsis -I$(srcdir)/src/cardpluginsis_acr38u
libbeidcardlayer_la_LDFLAGS				= -avoid-version
libbeidcardlayer_la_LIBADD				= $(COMMON_LIBADD)
libbeidcardlayer_la_CXXFLAGS			= $(COMMON_CXXFLAGS)
