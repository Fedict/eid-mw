XPI_NAME=belgiumeid.xpi

#noinst_DATA = $(XPI_NAME)

CLEANFILES = $(XPI_NAME) platform/Linux/defaults/preferences/belgiumeid.js

xpi_files = $(dist_content_DATA) \
			$(dist_defaults_preferences_DATA) \
			$(dist_locale_enus_DATA) \
			$(dist_locale_frfr_DATA) \
			$(dist_locale_nlnl_DATA) \
			$(dist_locale_dede_DATA) \
			$(dist_platform_darwin_defaults_preferences_DATA) \
			$(dist_platform_linux_defaults_preferences_DATA) \
			$(dist_platform_winnt_defaults_preferences_DATA) \
			$(dist_skin_DATA) \
			$(dist_root_DATA)

dist_extensionroot_DATA = chrome.manifest \
			icon.png \
			install.rdf

dist_content_DATA = content/belgiumeidpkcs11.js \
			content/belgiumeidpkcs11.xul \
			content/belgiumeidprompt.js \
			content/belgiumeidprompt.xul \
			content/util.js

dist_defaults_preferences_DATA = defaults/preferences/belgiumeid.js

dist_locale_enus_DATA = locale/en-US/belgiumeid.properties

dist_locale_frfr_DATA = locale/fr-FR/belgiumeid.properties

dist_locale_nlnl_DATA = locale/nl-NL/belgiumeid.properties

dist_locale_dede_DATA = locale/de-DE/belgiumeid.properties

dist_platform_darwin_defaults_preferences_DATA = platform/Darwin/defaults/preferences/belgiumeid.js

dist_platform_linux_defaults_preferences_DATA = platform/Linux/defaults/preferences/belgiumeid.js

dist_platform_winnt_defaults_preferences_DATA = platform/WINNT/defaults/preferences/belgiumeid.js

dist_skin_DATA = skin/icon.png \
			skin/icon40x40.png \
			skin/overlay.css

extensionrootdir=$(mozext)/$(XPI_ID)
contentdir=$(extensionrootdir)/content
defaults_preferencesdir=$(extensionrootdir)/defaults/preferences
locale_enusdir=$(extensionrootdir)/locale/en-US
locale_frfrdir=$(extensionrootdir)/locale/fr-FR
locale_nlnldir=$(extensionrootdir)/locale/nl-NL
locale_dededir=$(extensionrootdir)/locale/de-DE
platform_darwin_defaults_preferencesdir=$(extensionrootdir)/platform/Darwin/defaults/preferences
platform_linux_defaults_preferencesdir=$(extensionrootdir)/platform/Linux/defaults/preferences
platform_winnt_defaults_preferencesdir=$(extensionrootdir)/platform/WINNT/defaults/preferences
skindir=$(extensionrootdir)/skin

xpipackage: $(xpi_files)
#	 Create a temporary directory $tmp in $TMPDIR (default /tmp).
#	 Use mktemp if possible; otherwise fall back on mkdir,
#	 with $RANDOM to make collisions less likely.
	{ \
		tmp=` \
			(umask 077 && mktemp -t -d "xpiXXXXXX") 2>/dev/null \
		` && \
		test -n "$$tmp" && test -d "$$tmp"; \
	} || { \
		tmp=$$TMPDIR/xpi$$RANDOM; \
		umask 077 && $(mkinstalldirs) "$$tmp"; \
	} || exit $$?; \
	$(MAKE) DESTDIR=$$tmp mozext= install; \
	cd $$tmp; \
	@ZIP@ -rq $(abs_builddir)/belgiumeid.xpi .; \
	cd -; \
	rm -rf $$tmp;

EXTRA_DIST=platform/Linux/defaults/preferences/belgiumeid.js.in

# should be updated every time - there should be a better way...
.PHONY: platform/Linux/defaults/preferences/belgiumeid.js
BUILT_SOURCES=platform/Linux/defaults/preferences/belgiumeid.js


if LINUX
belgiumeid.js: belgiumeid.js.in
	$(SED) "s|__libdir__|@libdir@|g" $< > $@
else
# on other platforms libdir is unknown for Linux
# we default to /usr/local/lib
belgiumeid.js: belgiumeid.js.in
	$(SED) "s|__libdir__|/usr/local/lib|g" $< > $@
endif
