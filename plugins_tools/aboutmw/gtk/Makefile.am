bin_PROGRAMS = about-eid-mw

about_eid_mw_SOURCES = about-main.c gettext.h
about_eid_mw_LDFLAGS = @GTK_LIBS@
about_eid_mw_CFLAGS = @GTK_CFLAGS@ -DLIBDIR='"$(libdir)"' -DDATAROOTDIR='"$(datarootdir)"' @FUZZING@

BUILT_SOURCES = about_glade.h now.h
EXTRA_DIST = about.glade gen.sh

SUBDIRS = po

about_glade.h: about.glade
	$(srcdir)/gen.sh ABOUT_GLADE_H ABOUT_GLADE_STRING < $^ > $@

timestamp="$(shell if test -n "$$SOURCE_DATE_EPOCH"; \
                      then \
                        date -u --date="@$$SOURCE_DATE_EPOCH" "+%Y-%m-%d" 2>/dev/null || date -u -r "$$SOURCE_DATE_EPOCH" "+%Y-%m-%d" 2>/dev/null; \
                      else \
                        date -u "+%Y-%m-%d"; \
                      fi)"

now:
now.h: now
	echo $(timestamp) | $(srcdir)/gen.sh EID_NOW_H EID_NOW_STRING > $@

CLEANFILES = now.h about_glade.h
