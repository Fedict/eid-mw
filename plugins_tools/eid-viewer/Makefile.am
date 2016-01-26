bin_PROGRAMS = eid-viewer

if HAVE_GIO
MAYBE_PREFS = gtk/prefs.c
else
MAYBE_PREFS =
endif

eid_viewer_SOURCES = \
	oslayer.h \
	oslayer.c \
	certhelpers.h \
	certhelpers.c \
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
	gtk/print.h \
	gtk/print.c \
	gtk/verify.h \
	gtk/verify.c \
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
	$(MAYBE_PREFS) \
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
	conversions/valdate.h \
	conversions/valdate.cpp \
	conversions/xmldoctypeconv.h \
	conversions/xmldoctypeconv.cpp \
	conversions/xmlspecconv.h \
	conversions/xmlspecconv.cpp \
	conversions.h \
	cppeidstring.h \
	soap.c \
	soap.h \
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
	verify_cert.h \
	verify.c

eid_viewer_LDADD = ../../cardcomm/pkcs11/src/libbeidpkcs11.la @GTK_LIBS@ $(top_builddir)/plugins_tools/util/liblabels.a @SSL_LIBS@ @GIO_LIBS@ @XML2_LIBS@ @CURL_LIBS@ @libproxy_LIBS@
AM_CFLAGS = @GTK_CFLAGS@ -I. -Igtk -DDATAROOTDIR='"$(datarootdir)"' -I$(top_srcdir)/doc/sdk/include/rsaref220 -I$(top_srcdir)/plugins_tools/util @SSL_CFLAGS@ @GIO_CFLAGS@ @XML2_CFLAGS@ @CURL_CFLAGS@ @libproxy_CFLAGS@
AM_CXXFLAGS = $(AM_CFLAGS)
SUBDIRS = gtk/po test

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
		resources/coat_of_arms_en.c
EXTRA_DIST = gtk/viewer.glade \
	     gtk/eid-viewer.gschema.xml \
	     resources/coat_of_arms_de.jpg \
	     resources/coat_of_arms_en.jpg \
	     resources/coat_of_arms_fr.jpg \
	     resources/coat_of_arms_nl.jpg \
	     gtk/eid-viewer.desktop.sh.in

packagedatadir = $(datarootdir)/$(PACKAGE_NAME)
trustdir = $(packagedatadir)/trustdir

dist_packagedata_DATA = eidv4.xsd
dist_trust_DATA = \
	certs/belgiumrca.pem \
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
