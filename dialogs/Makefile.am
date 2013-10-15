ACLOCAL_AMFLAGS         = -I m4
noinst_HEADERS			= src/langutil.h src/language.h src/dialogsqtsrv/dlgwndpinpadinfo.h src/dialogsqtsrv/dlgwndaskaccess.h src/dialogsqtsrv/dlgwndmodal.h src/dialogsqtsrv/dlgwndbase.h src/dialogsqtsrv/dlgwndaskpins.h src/dialogsqtsrv/dlgwndbadpin.h src/dialogsqtsrv/dlgwndaskpin.h src/sharedmem.h src/dialogs.h src/dialogsqtsrv/dlgs.qrc src/dialogsqtsrv/dlgwndaskaccess.ui src/dialogsqtsrv/dlgwndaskpins.ui src/dialogsqtsrv/dlgwndaskpin.ui src/dialogsqtsrv/dlgwndbadpin.ui src/dialogsqtsrv/dlgwndmodal.ui src/dialogsqtsrv/dlgwndpinpadinfo.ui src/dialogsqtsrv/qt.conf src/dialogsqtsrv/resource/ICO_CARD_DIGSIG_128x128.png src/dialogsqtsrv/resource/ICO_CARD_EID_PLAIN_16x16.png src/dialogsqtsrv/resource/ICO_CARD_INFO_64x64.png src/dialogsqtsrv/resource/ICO_CARD_NOK_64x64.png src/dialogsqtsrv/resource/ICO_CARD_PIN_128x128.png src/dialogsqtsrv/resource/ICO_CARD_QUESTION_64x64.png src/dialogsqtsrv/resource/ICO_CARD_WARNING_64x64.png src/dialogsqtsrv/resource/KeyPadButton0.png src/dialogsqtsrv/resource/KeyPadButton1.png src/dialogsqtsrv/resource/KeyPadButton2.png src/dialogsqtsrv/resource/KeyPadButton3.png src/dialogsqtsrv/resource/KeyPadButton4.png src/dialogsqtsrv/resource/KeyPadButton5.png src/dialogsqtsrv/resource/KeyPadButton6.png src/dialogsqtsrv/resource/KeyPadButton7.png src/dialogsqtsrv/resource/KeyPadButton8.png src/dialogsqtsrv/resource/KeyPadButton9.png src/dialogsqtsrv/resource/KeyPadButtonCE.png src/dialogsqtsrv/resource/KeyPadButton.png src/dialogsqtsrv/resource/PINdisplay.png src/dialogsgtk/beid-i18n.h src/dialogsgtk/single_dialog.h src/dialogsgtk/parent.h
lib_LTLIBRARIES    		= libbeiddialogs.la

if DEBUG
DEFINE_DEBUG = -DDEBUG
FLAG_DEBUG   = -ggdb
else
DEFINE_DEBUG =
FLAG_DEBUG   =
endif

libbeiddialogs_la_SOURCES  	 	= src/dialogsgtk/single_dialog.c src/dialogsgtk/dlgs_gtk.cpp
libbeiddialogs_la_CPPFLAGS   	= -I$(srcdir)/src/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src $(DEFINE_DEBUG) -DBEID_ASKPIN_DIALOG=$(libexecdir)/beid-askpin -DBEID_CHANGEPIN_DIALOG=$(libexecdir)/beid-changepin -DBEID_BADPIN_DIALOG=$(libexecdir)/beid-badpin -DBEID_ASKACCESS_DIALOG=$(libexecdir)/beid-askaccess -DBEID_SPR_ASKPIN_DIALOG=$(libexecdir)/beid-spr-askpin -DBEID_SPR_CHANGEPIN_DIALOG=$(libexecdir)/beid-spr-changepin
libbeiddialogs_la_LDFLAGS	 	= -avoid-version -g 
libbeiddialogs_la_CXXFLAGS	 	= -fno-strict-aliasing $(FLAG_DEBUG)
libbeiddialogs_la_LIBADD     	= ../common/libbeidcommon.la
#
libexec_PROGRAMS		 		= beid-askpin beid-changepin beid-badpin beid-askaccess beid-spr-askpin beid-spr-changepin

beid_askpin_SOURCES    			= src/dialogsgtk/parent.c src/dialogsgtk/beid-askpin.c
beid_askpin_CPPFLAGS   			= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src 
beid_askpin_LDFLAGS    			= -avoid-version
beid_askpin_LDADD      			= ../common/libbeidcommon.la

beid_changepin_SOURCES   		= src/dialogsgtk/parent.c src/dialogsgtk/beid-changepin.c
beid_changepin_CPPFLAGS   		= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src
beid_changepin_LDFLAGS   		= -avoid-version
beid_changepin_LDADD     		= ../common/libbeidcommon.la

beid_badpin_SOURCES   			= src/dialogsgtk/parent.c src/dialogsgtk/beid-badpin.c
beid_badpin_CPPFLAGS  			= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src
beid_badpin_LDFLAGS   			= -avoid-version
beid_badpin_LDADD     			= ../common/libbeidcommon.la

beid_askaccess_SOURCES   		= src/dialogsgtk/parent.c src/dialogsgtk/beid-askaccess.c
beid_askaccess_CPPFLAGS  		= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src
beid_askaccess_LDFLAGS   		= -avoid-version
beid_askaccess_LDADD     		= ../common/libbeidcommon.la

beid_spr_askpin_SOURCES    		= src/dialogsgtk/parent.c src/dialogsgtk/single_dialog.c src/dialogsgtk/beid-spr-askpin.c
beid_spr_askpin_CPPFLAGS   		= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src
beid_spr_askpin_LDFLAGS    		= -avoid-version
beid_spr_askpin_LDADD      		= ../common/libbeidcommon.la

beid_spr_changepin_SOURCES  	= src/dialogsgtk/parent.c src/dialogsgtk/single_dialog.c src/dialogsgtk/beid-spr-changepin.c	
beid_spr_changepin_CPPFLAGS 	= -I$(srcdir)/dialogsgtk -I$(srcdir)/../common/src -I$(srcdir)/src
beid_spr_changepin_LDFLAGS  	= -avoid-version
beid_spr_changepin_LDADD    	= ../common/libbeidcommon.la
