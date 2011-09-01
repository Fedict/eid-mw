ACLOCAL_AMFLAGS         = -I m4
noinst_HEADERS			= src/langutil.h src/language.h src/dialogswin32/dlgwndpinpadinfo.h src/dialogswin32/dlgwndaskaccess.h src/dialogswin32/dlgwndmodal.h src/dialogswin32/dlgwndbase.h src/dialogswin32/stdafx.h src/dialogswin32/resource.h src/dialogswin32/dlgwndaskpins.h src/dialogswin32/dlgwndbadpin.h src/dialogswin32/dlgwndaskpin.h src/dialogsqtsrv/dlgwndpinpadinfo.h src/dialogsqtsrv/dlgwndaskaccess.h src/dialogsqtsrv/dlgwndmodal.h src/dialogsqtsrv/dlgwndbase.h src/dialogsqtsrv/dlgwndaskpins.h src/dialogsqtsrv/dlgwndbadpin.h src/dialogsqtsrv/dlgwndaskpin.h src/sharedmem.h src/dialogs.h src/dialogsqtsrv/dlgs.qrc src/dialogsqtsrv/dlgwndaskaccess.ui src/dialogsqtsrv/dlgwndaskpins.ui src/dialogsqtsrv/dlgwndaskpin.ui src/dialogsqtsrv/dlgwndbadpin.ui src/dialogsqtsrv/dlgwndmodal.ui src/dialogsqtsrv/dlgwndpinpadinfo.ui src/dialogsqtsrv/qt.conf src/dialogsqtsrv/resource/ICO_CARD_DIGSIG_128x128.png src/dialogsqtsrv/resource/ICO_CARD_EID_PLAIN_16x16.png src/dialogsqtsrv/resource/ICO_CARD_INFO_64x64.png src/dialogsqtsrv/resource/ICO_CARD_NOK_64x64.png src/dialogsqtsrv/resource/ICO_CARD_PIN_128x128.png src/dialogsqtsrv/resource/ICO_CARD_QUESTION_64x64.png src/dialogsqtsrv/resource/ICO_CARD_WARNING_64x64.png src/dialogsqtsrv/resource/KeyPadButton0.png src/dialogsqtsrv/resource/KeyPadButton1.png src/dialogsqtsrv/resource/KeyPadButton2.png src/dialogsqtsrv/resource/KeyPadButton3.png src/dialogsqtsrv/resource/KeyPadButton4.png src/dialogsqtsrv/resource/KeyPadButton5.png src/dialogsqtsrv/resource/KeyPadButton6.png src/dialogsqtsrv/resource/KeyPadButton7.png src/dialogsqtsrv/resource/KeyPadButton8.png src/dialogsqtsrv/resource/KeyPadButton9.png src/dialogsqtsrv/resource/KeyPadButtonCE.png src/dialogsqtsrv/resource/KeyPadButton.png src/dialogsqtsrv/resource/PINdisplay.png src/dialogswin32/images/ICO_BELPIC.ico src/dialogswin32/images/ICO_CARD_INFO_64x64.bmp src/dialogswin32/images/ICO_CARD_NOK_64x64.bmp src/dialogswin32/images/ICO_CARD_PIN_128x128.bmp src/dialogswin32/images/ICO_CARD_QUESTION_64x64.bmp src/dialogswin32/images/ICO_CARD_SIGN_128x128.bmp src/dialogswin32/images/ICO_CARD_WARNING_64x64.bmp src/dialogswin32/images/KeyPadButton0.bmp src/dialogswin32/images/KeyPadButton1.bmp src/dialogswin32/images/KeyPadButton2.bmp src/dialogswin32/images/KeyPadButton3.bmp src/dialogswin32/images/KeyPadButton4.bmp src/dialogswin32/images/KeyPadButton5.bmp src/dialogswin32/images/KeyPadButton6.bmp src/dialogswin32/images/KeyPadButton7.bmp src/dialogswin32/images/KeyPadButton8.bmp src/dialogswin32/images/KeyPadButton9.bmp src/dialogswin32/images/KeyPadButton.bmp src/dialogswin32/images/KeyPadButtonCE.bmp src/dialogsgtk/beid-i18n.h src/dialogsgtk/single_dialog.h
EXTRA_DIST          	= VS_2010 VS_2005
lib_LTLIBRARIES    		= libbeiddialogs.la

dist-hook:
	cp -p $(srcdir)/bin2c $(distdir)/bin2c

if LINUX
#

BUILT_SOURCES 					= default-beid-askpin.cpp default-beid-askaccess.cpp default-beid-badpin.cpp default-beid-changepin.cpp default-beid-spr-askpin.cpp default-beid-spr-changepin.cpp default-beid-spr-close.cpp
CLEANFILES 						= $(BUILT_SOURCES)

libbeiddialogs_la_SOURCES  	 	= src/dialogsgtk/single_dialog.c src/dialogsgtk/dlgs_gtk.cpp
libbeiddialogs_la_CPPFLAGS   	= -I$(srcdir)/src/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src -DSDIALOG_ID=beid-spr -DDEBUG
libbeiddialogs_la_LDFLAGS	 	= -avoid-version -g 
libbeiddialogs_la_CXXFLAGS	 	= -fno-strict-aliasing -ggdb -g
libbeiddialogs_la_LIBADD     	= ../common/libbeidcommon.la
#
noinst_PROGRAMS		   			= beid-askpin beid-changepin beid-badpin beid-askaccess beid-spr-askpin beid-spr-changepin beid-spr-close 

beid_askpin_SOURCES    			= src/dialogsgtk/beid-askpin.c
beid_askpin_CPPFLAGS   			= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src 
beid_askpin_LDFLAGS    			= -avoid-version
beid_askpin_LDADD      			= ../common/libbeidcommon.la

beid_changepin_SOURCES   		= src/dialogsgtk/beid-changepin.c
beid_changepin_CPPFLAGS   		= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src
beid_changepin_LDFLAGS   		= -avoid-version
beid_changepin_LDADD     		= ../common/libbeidcommon.la

beid_badpin_SOURCES   			= src/dialogsgtk/beid-badpin.c
beid_badpin_CPPFLAGS  			= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src
beid_badpin_LDFLAGS   			= -avoid-version
beid_badpin_LDADD     			= ../common/libbeidcommon.la

beid_askaccess_SOURCES   		= src/dialogsgtk/beid-askaccess.c
beid_askaccess_CPPFLAGS  		= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src
beid_askaccess_LDFLAGS   		= -avoid-version
beid_askaccess_LDADD     		= ../common/libbeidcommon.la

beid_spr_askpin_SOURCES    		= src/dialogsgtk/single_dialog.c src/dialogsgtk/beid-spr-askpin.c
beid_spr_askpin_CPPFLAGS   		= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src -DSDIALOG_ID=beid-spr
beid_spr_askpin_LDFLAGS    		= -avoid-version
beid_spr_askpin_LDADD      		= ../common/libbeidcommon.la

beid_spr_changepin_SOURCES  	= src/dialogsgtk/single_dialog.c src/dialogsgtk/beid-spr-changepin.c	
beid_spr_changepin_CPPFLAGS 	= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src -DSDIALOG_ID=beid-spr
beid_spr_changepin_LDFLAGS  	= -avoid-version
beid_spr_changepin_LDADD    	= ../common/libbeidcommon.la

beid_spr_close_SOURCES   		= src/dialogsgtk/single_dialog.c src/dialogsgtk/beid-spr-close.c
beid_spr_close_CPPFLAGS  		= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src -DSDIALOG_ID=beid-spr
beid_spr_close_LDFLAGS   		= -avoid-version
beid_spr_close_LDADD     		= ../common/libbeidcommon.la


default-beid-%.cpp: beid-%
	$(srcdir)/bin2c .libs/$< default_ > $@

endif

###################################################################################################################################################################################

if OSX
#
libbeiddialogs_la_SOURCES  	 = src/dialogsqt/dlgs.cpp src/sharedmem.cpp
libbeiddialogs_la_CPPFLAGS   = -I$(srcdir)/src/dialogsqt -I$(srcdir)/../common/src -I$(srcdir)/src
libbeiddialogs_la_LDFLAGS	 = -avoid-version
libbeiddialogs_la_CXXFLAGS	 = -finput-charset=cp1252 -fno-strict-aliasing 
libbeiddialogs_la_LIBADD     = ../common/libbeidcommon.la

#bin_PROGRAMS			 		= beiddialogsQTsrv
#beiddialogsQTsrv_SOURCES   		= src/dialogsqt/dlgs.cpp src/langutil.cpp src/dialogsqtsrv/dlgwndaskaccess.cpp src/dialogsqtsrv/dlgwndaskpin.cpp src/dialogsqtsrv/dlgwndaskpins.cpp src/dialogsqtsrv/dlgwndbadpin.cpp src/dialogsqtsrv/dlgwndbase.cpp src/dialogsqtsrv/dlgwndmodal.cpp src/dialogsqtsrv/dlgwndpinpadinfo.cpp src/dialogsqtsrv/main.cpp src/sharedmem.cpp 
#nodist_beiddialogsQTsrv_SOURCES = src/dialogsqtsrv/moc_dlgwndbase.cpp src/dialogsqtsrv/moc_dlgwndmodal.cpp src/dialogsqtsrv/moc_dlgwndaskaccess.cpp src/dialogsqtsrv/moc_dlgwndaskpin.cpp src/dialogsqtsrv/moc_dlgwndaskpins.cpp src/dialogsqtsrv/moc_dlgwndbadpin.cpp src/dialogsqtsrv/moc_dlgwndpinpadinfo.cpp
#beiddialogsQTsrv_CPPFLAGS   	= -I$(srcdir)/dialogsqt -I$(srcdir)/../common/src -I$(srcdir)/src
#beiddialogsQTsrv_LDFLAGS    	= -lpthread -avoid-version
#beiddialogsQTsrv_CXXFLAGS   	= -finput-charset=cp1252 -fno-strict-aliasing -framework QtCore -framework QtGui 
#beiddialogsQTsrv_LDADD      	= ../common/libbeidcommon.la
#

bin_PROGRAMS					= beid-askpin beid-changepin beid-badpin beid-askaccess beid-spr-askpin beid-spr-changepin

beid-askpin: src/dialogscocoa/beid-askpin/beid-askpin.m
	cd src/dialogscocoa/beid-askpin && rm -Rf build/* && $(XCODEBUILD)

beid-changepin: src/dialogscocoa/beid-changepin/beid-changepin.m
	cd src/dialogscocoa/beid-changepin && rm -Rf build/* && $(XCODEBUILD)

beid-badpin: src/dialogscocoa/beid-badpin/beid-badpin.m
	cd src/dialogscocoa/beid-badpin && rm -Rf build/* && $(XCODEBUILD)

beid-askaccess: src/dialogscocoa/beid-askaccess/beid-askaccess.m
	cd src/dialogscocoa/beid-askaccess && rm -Rf build/* && $(XCODEBUILD)

beid-spr-askpin: src/dialogscocoa/beid-spr-askpin/beid-spr-askpin.m
	cd src/dialogscocoa/beid-spr-askpin && rm -Rf build/* && $(XCODEBUILD)

beid-spr-changepin: src/dialogscocoa/beid-spr-changepin/beid-spr-changepin.m
	cd src/dialogscocoa/beid-spr-changepin && rm -Rf build/* && $(XCODEBUILD)

endif

###############################################################################################################################################

if WIN32
#
libbeiddialogs_la_SOURCES  	 =  $(srcdir)/../common/src/mw_util.cpp src/langutil.cpp src/dialogswin32/dlgswin32.cpp src/dialogswin32/dlgwndaskaccess.cpp src/dialogswin32/dlgwndaskpin.cpp src/dialogswin32/dlgwndaskpins.cpp src/dialogswin32/dlgwndbadpin.cpp src/dialogswin32/dlgwndbase.cpp src/dialogswin32/dlgwndmodal.cpp src/dialogswin32/dlgwndpinpadinfo.cpp src/dialogswin32/stdafx.cpp
libbeiddialogs_la_CPPFLAGS   = -I$(srcdir)/src/dialogswin32 -I$(srcdir)/../common/src
libbeiddialogs_la_LDFLAGS	 = -lpsapi -lgdi32 -no-undefined -shared -static-libstdc++ -static-libgcc -avoid-version
libbeiddialogs_la_CXXFLAGS	 = -municode -DMINGW_HAS_SECURE_API -DDLGSWIN32_EXPORTS
libbeiddialogs_la_LIBADD     = ../common/libbeidcommon.la
#
endif

