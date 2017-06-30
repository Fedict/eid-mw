bin_PROGRAMS = eid-viewer
lib_LTLIBRARIES = libeidviewer.la

if HAVE_GIO
MAYBE_PREFS = gtk/prefs.c
else
MAYBE_PREFS =
endif

eid_viewer_SOURCES = \
	gtk/main.h \
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
	gtk/print.h \
	gtk/print.c \
	gtk/glib_util.h \
	gtk/logging.h \
	gtk/logging.c \
	gtk/prefs.h \
	gtk/gtk_main.h \
	resources/coat_of_arms_nl.c \
	resources/coat_of_arms_fr.c \
	resources/coat_of_arms_en.c \
	resources/coat_of_arms_de.c \
	resources/logo_128.c \
	resources/certificate_large.c \
	resources/certificate_bad.c \
	resources/certificate_checked.c \
	resources/certificate_warn.c \
	$(MAYBE_PREFS)

libeidviewer_la_SOURCES = \
	oslayer.c \
	certhelpers.c \
	gettext.h \
	backend.c \
	backend.h \
	cache.h \
	cache/cache.cpp \
	conversions/bbannumconv.h \
	conversions/bbannumconv.cpp \
	conversions/genderconv.h \
	conversions/genderconv.cpp \
	conversions/hexnumconv.cpp \
	conversions/hexnumconv.h \
	conversions/hexdecode.h \
	conversions/hexdecode.cpp \
	conversions/convert.cpp \
	conversions/convertor.cpp \
	conversions/convertor.h \
	conversions/convworker.h \
	conversions/convworker.cpp \
	conversions/dateconv.h \
	conversions/dateconv.cpp \
	conversions/dob.h \
	conversions/dob.cpp \
	conversions/doctypeconv.h \
	conversions/doctypeconv.cpp \
	conversions/natnumconv.cpp \
	conversions/natnumconv.h \
	conversions/specconv.h \
	conversions/specconv.cpp \
	conversions/specorgconv.h \
	conversions/specorgconv.cpp \
	conversions/valdate.h \
	conversions/valdate.cpp \
	conversions/xmldoctypeconv.h \
	conversions/xmldoctypeconv.cpp \
	conversions/xmlspecconv.h \
	conversions/xmlspecconv.cpp \
	conversions/xmlspecorgconv.h \
	conversions/xmlspecorgconv.cpp \
	conversions.h \
	cppeidstring.h \
	dataverify.h \
	state.c \
	state.h \
	state-posix.c \
	p11.h \
	p11.c \
	preview.c \
	xml.h \
	xml.c \
	xmlmap.cpp \
	xmlmap.h \
	xsdloc.h \
	xsdloc.c \
	b64/base64dec.h \
	b64/base64dec.c \
	verify.c

eidvincludedir = $(includedir)/eid-viewer

dist_eidvinclude_HEADERS = \
	include/eid-viewer/oslayer.h \
	include/eid-viewer/verify_cert.h \
	include/eid-viewer/certhelpers.h \
	include/eid-viewer/eid-viewer.h \
	include/eid-viewer/macros.h

eid_viewer_LDADD = @GTK_LIBS@ @GIO_LIBS@ @SSL_LIBS@ @XML2_LIBS@ @CURL_LIBS@ @libproxy_LIBS@ $(top_builddir)/plugins_tools/util/liblabels.la libeidviewer.la
libeidviewer_la_LIBADD = $(top_builddir)/plugins_tools/util/liblabels.la $(top_builddir)/cardcomm/pkcs11/src/libbeidpkcs11.la @SSL_LIBS@ @XML2_LIBS@
AM_CFLAGS = @GTK_CFLAGS@ -I. -I$(srcdir)/include -Igtk -DDATAROOTDIR='"$(datarootdir)"' -I$(top_srcdir)/doc/sdk/include/rsaref220 -I$(top_srcdir)/plugins_tools/util @SSL_CFLAGS@ @GIO_CFLAGS@ @XML2_CFLAGS@ @CURL_CFLAGS@ @libproxy_CFLAGS@ -fvisibility=hidden
AM_CXXFLAGS = $(AM_CFLAGS)
SUBDIRS = . gtk/po test

resources/%.c: resources/%.png
	echo -e "#include <gdk-pixbuf/gdk-pixdata.h>\n#if __GNUC__ >= 4\n#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored \"-Wpointer-sign\"\n#endif\n" > $@
	gdk-pixbuf-csource --struct --extern --name `basename $^ .png` $^ >> $@
	echo -e "#pragma GCC diagnostic pop\n" >> $@
resources/%.c: resources/%.jpg
	echo -e "#include <gdk-pixbuf/gdk-pixdata.h>\n#if __GNUC__ >= 4\n#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored \"-Wpointer-sign\"\n#endif\n" > $@
	gdk-pixbuf-csource --struct --extern --name `basename $^ .jpg` $^ >> $@
	echo -e "#pragma GCC diagnostic pop\n" >> $@

BUILT_SOURCES = gtk/viewer_glade.h \
		resources/coat_of_arms_nl.c \
		resources/coat_of_arms_fr.c \
		resources/coat_of_arms_de.c \
		resources/coat_of_arms_en.c \
		resources/certificate_large.c \
		resources/certificate_bad.c \
		resources/certificate_checked.c \
		resources/certificate_warn.c
EXTRA_DIST = gtk/viewer.glade \
	     gtk/eid-viewer.gschema.xml \
	     resources/coat_of_arms_de.jpg \
	     resources/coat_of_arms_en.jpg \
	     resources/coat_of_arms_fr.jpg \
	     resources/coat_of_arms_nl.jpg \
	     resources/logo_16.png \
	     resources/logo_32.png \
	     resources/logo_128.png \
	     resources/logo_256.png \
	     resources/logo_512.png \
	     gtk/eid-viewer.desktop.sh.in \
	     gtk/eid-viewer.png

packagedatadir = $(datarootdir)/$(PACKAGE_NAME)
trustdir = $(packagedatadir)/trustdir
icondir = @datadir@/icons/hicolor
icon16dir = @icondir@/16x16
icon32dir = @icondir@/32x32
icon48dir = @icondir@/48x48
icon128dir = @icondir@/128x128
icon256dir = @icondir@/256x256
icon512dir = @icondir@/512x512

resources/logos/48/eid-viewer.png: gtk/eid-viewer.png
	cp $^ $@

resources/logos/%/eid-viewer.png: resources/logo_%.png
	cp $^ $@

icon16_DATA = resources/logos/16/eid-viewer.png
icon32_DATA = resources/logos/32/eid-viewer.png
icon48_DATA = resources/logos/48/eid-viewer.png
icon128_DATA = resources/logos/128/eid-viewer.png
icon256_DATA = resources/logos/256/eid-viewer.png
icon512_DATA = resources/logos/512/eid-viewer.png

dist_packagedata_DATA = eidv4.xsd
dist_trust_DATA = \
	certs/belgiumrca2.pem \
	certs/belgiumrca3.pem \
	certs/belgiumrca4.pem

install-data-hook:
	$(SSL_PREFIX)/bin/c_rehash $(DESTDIR)/$(trustdir)

gtk/viewer_glade.h: gtk/viewer.glade
	$(top_srcdir)/plugins_tools/aboutmw/gtk/gen.sh VIEWER_GLADE_H VIEWER_GLADE_STRING < $^ > $@

gsettings_SCHEMAS = gtk/eid-viewer.gschema.xml

desktopdir=@datadir@/applications
desktop_DATA=eid-viewer.desktop

eid-viewer.desktop: gtk/eid-viewer.desktop.sh
	sh $(builddir)/gtk/eid-viewer.desktop.sh

CLEANFILES=eid-viewer.desktop

@GSETTINGS_RULES@
