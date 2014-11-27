bin_PROGRAMS = about-eid-mw

about_eid_mw_SOURCES = about-main.c
about_eid_mw_LDFLAGS = @GTK_LIBS@
about_eid_mw_CFLAGS = @GTK_CFLAGS@ -DLIBDIR='"$(libdir)"'

about-eid-mw.desktop: about-eid-mw.desktop.sh
	sh ./about-eid-mw.desktop.sh

desktopdir=@datadir@/applications
desktop_DATA=about-eid-mw.desktop

BUILT_SOURCES = about_glade.h now.h
EXTRA_DIST = genabout.sh about.glade gen.sh

about_glade.h: about.glade
	$(srcdir)/gen.sh ABOUT_GLADE_H ABOUT_GLADE_STRING < $^ > $@

now:
now.h: now
	date +'%F' | $(srcdir)/gen.sh EID_NOW_H EID_NOW_STRING > $@

CLEANFILES = now.h about_glade.h about-eid-mw.desktop
