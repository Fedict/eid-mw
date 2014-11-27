bin_PROGRAMS = about-eid-mw

about_eid_mw_SOURCES = about-main.c
about_eid_mw_LDFLAGS = @GTK_LIBS@
about_eid_mw_CFLAGS = @GTK_CFLAGS@

BUILT_SOURCES = about_glade.h
EXTRA_DIST = genabout.sh

about_glade.h: about.glade
	./genabout.sh < $^ > $@
