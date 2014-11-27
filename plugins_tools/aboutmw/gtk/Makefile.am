bin_PROGRAMS = about-eid-mw

about_eid_mw_SOURCES = about-main.c
about_eid_mw_LDFLAGS = @GTK_LIBS@
about_eid_mw_CFLAGS = @GTK_CFLAGS@

BUILT_SOURCES = about_glade.h now.h
EXTRA_DIST = genabout.sh

about_glade.h: about.glade
	./gen.sh ABOUT_GLADE_H ABOUT_GLADE_STRING < $^ > $@

now:
now.h: now
	date +'%F' | ./gen.sh EID_NOW_H EID_NOW_STRING > $@

CLEANFILES = now.h
