ACLOCAL_AMFLAGS         = -I m4
COMMON_LIBADD   		= ../common/libbeidcommon.la 
COMMON_LDADD			= $(COMMON_LIBADD)
COMMON_SOURCES			= $(srcdir)/../common/src/mw_util.cpp src/langutil.cpp
noinst_HEADERS			= src/langutil.h src/language.h src/dialogswin32/dlgwndpinpadinfo.h src/dialogswin32/dlgwndaskaccess.h src/dialogswin32/dlgwndmodal.h src/dialogswin32/dlgwndbase.h src/dialogswin32/stdafx.h src/dialogswin32/resource.h src/dialogswin32/dlgwndaskpins.h src/dialogswin32/dlgwndbadpin.h src/dialogswin32/dlgwndaskpin.h src/dialogsqtsrv/dlgwndpinpadinfo.h src/dialogsqtsrv/ui_dlgwndaskpin.h src/dialogsqtsrv/dlgwndaskaccess.h src/dialogsqtsrv/ui_dlgwndmodal.h src/dialogsqtsrv/dlgwndmodal.h src/dialogsqtsrv/dlgwndbase.h src/dialogsqtsrv/ui_dlgwndbadpin.h src/dialogsqtsrv/ui_dlgwndaskaccess.h src/dialogsqtsrv/ui_dlgwndaskpins.h src/dialogsqtsrv/dlgwndaskpins.h src/dialogsqtsrv/ui_dlgwndpinpadinfo.h src/dialogsqtsrv/dlgwndbadpin.h src/dialogsqtsrv/dlgwndaskpin.h src/sharedmem.h src/dialogs.h src/dialogsqtsrv/dlgs.qrc src/dialogsqtsrv/*.ui src/dialogsqtsrv/qt.conf src/dialogsqtsrv/resource/ICO_CARD_DIGSIG_128x128.png src/dialogsqtsrv/resource/ICO_CARD_EID_PLAIN_16x16.png src/dialogsqtsrv/resource/ICO_CARD_INFO_64x64.png src/dialogsqtsrv/resource/ICO_CARD_NOK_64x64.png src/dialogsqtsrv/resource/ICO_CARD_PIN_128x128.png src/dialogsqtsrv/resource/ICO_CARD_QUESTION_64x64.png src/dialogsqtsrv/resource/ICO_CARD_WARNING_64x64.png src/dialogsqtsrv/resource/KeyPadButton0.png src/dialogsqtsrv/resource/KeyPadButton1.png src/dialogsqtsrv/resource/KeyPadButton2.png src/dialogsqtsrv/resource/KeyPadButton3.png src/dialogsqtsrv/resource/KeyPadButton4.png src/dialogsqtsrv/resource/KeyPadButton5.png src/dialogsqtsrv/resource/KeyPadButton6.png src/dialogsqtsrv/resource/KeyPadButton7.png src/dialogsqtsrv/resource/KeyPadButton8.png src/dialogsqtsrv/resource/KeyPadButton9.png src/dialogsqtsrv/resource/KeyPadButtonCE.png src/dialogsqtsrv/resource/KeyPadButton.png src/dialogsqtsrv/resource/PINdisplay.png src/dialogswin32/images/ICO_BELPIC.ico src/dialogswin32/images/ICO_CARD_INFO_64x64.bmp src/dialogswin32/images/ICO_CARD_NOK_64x64.bmp src/dialogswin32/images/ICO_CARD_PIN_128x128.bmp src/dialogswin32/images/ICO_CARD_QUESTION_64x64.bmp src/dialogswin32/images/ICO_CARD_SIGN_128x128.bmp src/dialogswin32/images/ICO_CARD_WARNING_64x64.bmp src/dialogswin32/images/KeyPadButton0.bmp src/dialogswin32/images/KeyPadButton1.bmp src/dialogswin32/images/KeyPadButton2.bmp src/dialogswin32/images/KeyPadButton3.bmp src/dialogswin32/images/KeyPadButton4.bmp src/dialogswin32/images/KeyPadButton5.bmp src/dialogswin32/images/KeyPadButton6.bmp src/dialogswin32/images/KeyPadButton7.bmp src/dialogswin32/images/KeyPadButton8.bmp src/dialogswin32/images/KeyPadButton9.bmp src/dialogswin32/images/KeyPadButton.bmp src/dialogswin32/images/KeyPadButtonCE.bmp

lib_LTLIBRARIES    	 	= libbeiddialogs.la

if LINUX

#
libbeiddialogs_la_SOURCES  	 = $(COMMON_SOURCES) src/dialogsqt/dlgs.cpp src/sharedmem.cpp
libbeiddialogs_la_CPPFLAGS   = -I$(srcdir)/src/dialogsqt -I$(srcdir)/../common/src -I$(srcdir)/src $(QT_INCS) $(OPENSSL_INCS)
libbeiddialogs_la_LDFLAGS	 = $(QT_LIBS) $(OPENSSL_LIBS)-avoid-version
libbeiddialogs_la_CXXFLAGS	 = -finput-charset=cp1252 -fno-strict-aliasing
libbeiddialogs_la_LIBADD     = $(COMMON_LIBADD)

bin_PROGRAMS			 	= beiddialogsQTsrv
beiddialogsQTsrv_SOURCES   	= $(COMMON_SOURCES) src/dialogsqt/dlgs.cpp src/dialogsqtsrv/dlgwndaskaccess.cpp src/dialogsqtsrv/dlgwndaskpin.cpp src/dialogsqtsrv/dlgwndaskpins.cpp src/dialogsqtsrv/dlgwndbadpin.cpp src/dialogsqtsrv/dlgwndbase.cpp src/dialogsqtsrv/dlgwndmodal.cpp src/dialogsqtsrv/dlgwndpinpadinfo.cpp src/dialogsqtsrv/main.cpp src/dialogsqtsrv/moc_dlgwndbase.cpp src/dialogsqtsrv/moc_dlgwndmodal.cpp src/dialogsqtsrv/moc_dlgwndaskaccess.cpp src/dialogsqtsrv/moc_dlgwndaskpin.cpp src/dialogsqtsrv/moc_dlgwndaskpins.cpp src/dialogsqtsrv/moc_dlgwndbadpin.cpp src/dialogsqtsrv/moc_dlgwndpinpadinfo.cpp src/sharedmem.cpp
beiddialogsQTsrv_CPPFLAGS   = -I$(srcdir)/dialogsqt -I$(srcdir)/../common/src -I$(srcdir)/src $(QT_INCS) $(OPENSSL_INCS)
beiddialogsQTsrv_LDFLAGS    = $(QT_LIBS) $(OPENSSL_LIBS) -lpthread -lQtCore -lQtGui -avoid-version
beiddialogsQTsrv_CXXFLAGS   = -finput-charset=cp1252 -fno-strict-aliasing
beiddialogsQTsrv_LDADD      = $(COMMON_LDADD)
#
endif

###################################################################################################################################################################################

if OSX
#
libbeiddialogs_la_SOURCES  	 = $(COMMON_SOURCES) src/dialogsqt/dlgs.cpp src/sharedmem.cpp
libbeiddialogs_la_CPPFLAGS   = -I$(srcdir)/src/dialogsqt -I$(srcdir)/../common/src -I$(srcdir)/src
libbeiddialogs_la_LDFLAGS	 = -avoid-version
libbeiddialogs_la_CXXFLAGS	 = -finput-charset=cp1252 -fno-strict-aliasing 
libbeiddialogs_la_LIBADD     = $(COMMON_LIBADD) 

bin_PROGRAMS			 	= beiddialogsQTsrv
beiddialogsQTsrv_SOURCES   	= $(COMMON_SOURCES) src/dialogsqt/dlgs.cpp src/dialogsqtsrv/dlgwndaskaccess.cpp src/dialogsqtsrv/dlgwndaskpin.cpp src/dialogsqtsrv/dlgwndaskpins.cpp src/dialogsqtsrv/dlgwndbadpin.cpp src/dialogsqtsrv/dlgwndbase.cpp src/dialogsqtsrv/dlgwndmodal.cpp src/dialogsqtsrv/dlgwndpinpadinfo.cpp src/dialogsqtsrv/main.cpp src/dialogsqtsrv/moc_dlgwndbase.cpp src/dialogsqtsrv/moc_dlgwndmodal.cpp src/dialogsqtsrv/moc_dlgwndaskaccess.cpp src/dialogsqtsrv/moc_dlgwndaskpin.cpp src/dialogsqtsrv/moc_dlgwndaskpins.cpp src/dialogsqtsrv/moc_dlgwndbadpin.cpp src/dialogsqtsrv/moc_dlgwndpinpadinfo.cpp src/sharedmem.cpp
beiddialogsQTsrv_CPPFLAGS   = -I$(srcdir)/dialogsqt -I$(srcdir)/../common/src -I$(srcdir)/src
beiddialogsQTsrv_LDFLAGS    = -lpthread -avoid-version
beiddialogsQTsrv_CXXFLAGS   = -finput-charset=cp1252 -fno-strict-aliasing -framework QtCore -framework QtGui 
beiddialogsQTsrv_LDADD      = $(COMMON_LDADD)
#
endif

###############################################################################################################################################

if WIN32
#
libbeiddialogs_la_SOURCES  	 = $(COMMON_SOURCES) src/dialogswin32/dlgswin32.cpp src/dialogswin32/dlgwndaskaccess.cpp src/dialogswin32/dlgwndaskpin.cpp src/dialogswin32/dlgwndaskpins.cpp src/dialogswin32/dlgwndbadpin.cpp src/dialogswin32/dlgwndbase.cpp src/dialogswin32/dlgwndmodal.cpp src/dialogswin32/dlgwndpinpadinfo.cpp src/dialogswin32/stdafx.cpp
libbeiddialogs_la_CPPFLAGS   = -I$(srcdir)/src/dialogswin32 -I$(srcdir)/../common/src
libbeiddialogs_la_LDFLAGS	 = -lpsapi -lgdi32 -no-undefined -shared -static-libstdc++ -static-libgcc -avoid-version
libbeiddialogs_la_CXXFLAGS	 = -municode -DMINGW_HAS_SECURE_API -DDLGSWIN32_EXPORTS
libbeiddialogs_la_LIBADD     = $(COMMON_LIBADD)
#
endif


if QT
#
MOC_CPPFLAGS=$(dialogsqtsrv_CPPFLAGS)
BUILT_SOURCES = qrc_dlgs.cpp ui_dlgwndaskaccess.h ui_dlgwndaskpins.h ui_dlgwndaskpin.h ui_dlgwndbadpin.h ui_dlgwndmodal.h ui_dlgwndpinpadinfo.h moc_dlgwndbase.cpp moc_dlgwndmodal.cpp moc_dlgwndaskaccess.cpp moc_dlgwndaskpin.cpp moc_dlgwndaskpins.cpp moc_dlgwndbadpin.cpp moc_dlgwndpinpadinfo.cpp
qrc_dlgs.cpp: src/dialogsqtsrv/dlgs.qrc
	$(RCC) -name src/dialogsqtsrv/dialogsqtsrv src/dialogsqtsrv/dlgs.qrc -o src/dialogsqtsrv/qrc_dlgs.cpp
ui_dlgwndaskaccess.h : src/dialogsqtsrv/dlgwndaskaccess.ui
	$(UIC) src/dialogsqtsrv/dlgwndaskaccess.ui -o src/dialogsqtsrv/ui_dlgwndaskaccess.h
ui_dlgwndaskpins.h : src/dialogsqtsrv/dlgwndaskpins.ui
	$(UIC) src/dialogsqtsrv/dlgwndaskpins.ui -o src/dialogsqtsrv/ui_dlgwndaskpins.h
ui_dlgwndaskpin.h : src/dialogsqtsrv/dlgwndaskpin.ui
	$(UIC) src/dialogsqtsrv/dlgwndaskpin.ui -o src/dialogsqtsrv/ui_dlgwndaskpin.h
ui_dlgwndbadpin.h : src/dialogsqtsrv/dlgwndbadpin.ui
	$(UIC) src/dialogsqtsrv/dlgwndbadpin.ui -o src/dialogsqtsrv/ui_dlgwndbadpin.h
ui_dlgwndmodal.h : src/dialogsqtsrv/dlgwndmodal.ui
	$(UIC) src/dialogsqtsrv/dlgwndmodal.ui -o src/dialogsqtsrv/ui_dlgwndmodal.h
ui_dlgwndpinpadinfo.h : src/dialogsqtsrv/dlgwndpinpadinfo.ui
	$(UIC) src/dialogsqtsrv/dlgwndpinpadinfo.ui -o src/dialogsqtsrv/ui_dlgwndpinpadinfo.h
moc_dlgwndbase.cpp: src/dialogsqtsrv/dlgwndbase.h
	$(MOC) -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED $(MOCCPPFLAGS) src/dialogsqtsrv/dlgwndbase.h -o src/dialogsqtsrv/moc_dlgwndbase.cpp
moc_dlgwndmodal.cpp: src/dialogsqtsrv/dlgwndmodal.h
	$(MOC) -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED $(MOCCPPFLAGS) src/dialogsqtsrv/dlgwndmodal.h -o src/dialogsqtsrv/moc_dlgwndmodal.cpp
moc_dlgwndaskaccess.cpp: src/dialogsqtsrv/dlgwndaskaccess.h
	$(MOC) -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED $(MOCCPPFLAGS) src/dialogsqtsrv/dlgwndaskaccess.h -o src/dialogsqtsrv/moc_dlgwndaskaccess.cpp
moc_dlgwndaskpin.cpp: src/dialogsqtsrv/dlgwndaskpin.h
	$(MOC) -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED $(MOCCPPFLAGS) src/dialogsqtsrv/dlgwndaskpin.h -o src/dialogsqtsrv/moc_dlgwndaskpin.cpp
moc_dlgwndaskpins.cpp: src/dialogsqtsrv/dlgwndaskpins.h
	$(MOC) -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED $(MOCCPPFLAGS) src/dialogsqtsrv/dlgwndaskpins.h -o src/dialogsqtsrv/moc_dlgwndaskpins.cpp
moc_dlgwndbadpin.cpp: src/dialogsqtsrv/dlgwndbadpin.h
	$(MOC) -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED $(MOCCPPFLAGS) src/dialogsqtsrv/dlgwndbadpin.h -o src/dialogsqtsrv/moc_dlgwndbadpin.cpp
moc_dlgwndpinpadinfo.cpp: src/dialogsqtsrv/dlgwndpinpadinfo.h
	$(MOC) -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED $(MOCCPPFLAGS) src/dialogsqtsrv/dlgwndpinpadinfo.h -o src/dialogsqtsrv/moc_dlgwndpinpadinfo.cpp
CLEANFILES = $(BUILT_SOURCES)
#
endif
