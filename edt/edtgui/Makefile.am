ACLOCAL_AMFLAGS = -I m4

BUILT_SOURCES = qrc_edtgui.cpp ui_edtgui.h moc_qtsingleapplication.cpp moc_edtgui.cpp

AM_CPPFLAGS = -I$(srcdir)/../../eidmw/eidlib -I$(srcdir)/../../eidmw/common 
MOCCPPFLAGS = $(AM_CPPFLAGS)

qrc_edtgui.cpp: edtgui.qrc edtgui_nl.qm edtgui_fr.qm resources/wizarrow7_left.png resources/wizarrow_right.png resources/wizarrow7_right.png resources/ICO_CARD_EID_DIAGNOSIS_128x128.png resources/ICO_CARD_EID_TOOLS_16x16.png resources/ICO_CARD_EID_DIGSIG_128x128.png resources/ICO_CARD_EID_TOOLS_128x128.png resources/wizarrow_left.png
	/usr/bin/rcc -name edtgui edtgui.qrc -o qrc_edtgui.cpp

ui_edtgui.h : edtgui.ui
	/usr/bin/uic edtgui.ui -o ui_edtgui.h

moc_qtsingleapplication.cpp: qtsingleapplication.h
	/usr/bin/moc -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED $(MOCCPPFLAGS) qtsingleapplication.h -o moc_qtsingleapplication.cpp

moc_edtgui.cpp: ui_edtgui.h enginethread.h edtgui.h
	/usr/bin/moc -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED $(MOCCPPFLAGS) edtgui.h -o moc_edtgui.cpp

bin_PROGRAMS    = edtgui
edtgui_SOURCES  = edtgui.cpp  enginethread.cpp main.cpp qtsingleapplication.cpp qtsingleapplication_win.cpp moc_edtgui.cpp moc_qtsingleapplication.cpp
edtgui_CPPFLAGS = $(AM_CPPFLAGS) -I.. -I$(top_srcdir)/../diaglib -I$(top_srcdir)/../diagengine -I$(top_srcdir)/../../eidmw/misc/Wix_MW35/beidcleanup/ -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED 
edtgui_LDADD    = ../diaglib/libdiag.la ../diagengine/libdiagengine.la
edtgui_LDFLAGS  = -lQtCore4 -lQtGui4 -lmingw32 -lmsvcr80 -lsetupapi -lcrypt32 -lbeid35cardlayer -lbeid35applayer -lbeid35common -lbeid35CSPlib -lbeid35libCpp 
edtgui_CXXFLAGS = -municode -trigraphs

CLEANFILES = $(BUILT_SOURCES)


