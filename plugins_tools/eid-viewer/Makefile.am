bin_PROGRAMS = eid-viewer

eid_viewer_SOURCES = \
	oslayer.h \
	oslayer.c \
	gettext.h \
	backend.c \
	backend.h \
	gtk/main.c \
	gtk/viewer_glade.h \
	gtk/certs.h \
	gtk/certs.c \
	gtk/gtkui.c \
	gtk/gtkui.h \
	gtk/gtk_globals.h \
	gtk/thread.h \
	gtk/thread.c \
	gtk/photo.h \
	gtk/photo.c \
	gtk/verify.h \
	gtk/verify.c \
	cache.h \
	cache/cache.cpp \
	conversions/hexnumconv.cpp \
	conversions/hexnumconv.h \
	conversions/convert.cpp \
	conversions/convertor.cpp \
	conversions/convertor.h \
	conversions/convworker.h \
	conversions/convworker.cpp \
	conversions/dateconv.h \
	conversions/dateconv.cpp \
	conversions/doctypeconv.h \
	conversions/doctypeconv.cpp \
	conversions/natnumconv.cpp \
	conversions/natnumconv.h \
	conversions/specconv.h \
	conversions/specconv.cpp \
	conversions.h \
	state.c \
	state.h \
	p11.h \
	p11.c
eid_viewer_LDADD = ../../cardcomm/pkcs11/src/libbeidpkcs11.la @GTK_LIBS@ $(top_builddir)/tests/unit/liblabels.a @SSL_LIBS@
AM_CFLAGS = @GTK_CFLAGS@ -I. -Igtk -DDATAROOTDIR='"$(datarootdir)"' -I$(top_srcdir)/doc/sdk/include/rsaref220 -I$(top_srcdir)/tests/unit @SSL_CFLAGS@
AM_CXXFLAGS = $(AM_CFLAGS) 
SUBDIRS = gtk/po test

BUILT_SOURCES = gtk/viewer_glade.h
EXTRA_DIST = gtk/viewer.glade

gtk/viewer_glade.h: gtk/viewer.glade
	$(top_srcdir)/plugins_tools/aboutmw/gtk/gen.sh VIEWER_GLADE_H VIEWER_GLADE_STRING < $^ > $@
