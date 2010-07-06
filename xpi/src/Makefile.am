XPI_NAME=belgiumeid.xpi

noinst_DATA = $(XPI_NAME)

CLEANFILES = $(XPI_NAME)

xpi_files = $(dist_content_DATA) \
			$(dist_defaults_preferences_DATA) \
			$(dist_locale_enus_DATA) \
			$(dist_locale_frfr_DATA) \
			$(dist_locale_nlnl_DATA) \
			$(dist_platform_darwin_defaults_preferences_DATA) \
			$(dist_platform_linux_defaults_preferences_DATA) \
			$(dist_platform_winnt_defaults_preferences_DATA) \
			$(dist_skin_DATA) \
			$(dist_root_DATA)

dist_extensionroot_DATA = chrome.manifest \
			icon.png \
			install.rdf

dist_content_DATA = content/BelgiumEidPKCS11.js \
			content/firefoxOverlay.xul \
			content/overlay.js \
			content/util.js

dist_defaults_preferences_DATA = defaults/preferences/belgiumeid.js

dist_locale_enus_DATA = locale/en-us/belgiumeid.dtd \
	locale/en-us/belgiumeid.properties

dist_locale_frfr_DATA = locale/fr-fr/belgiumeid.properties

dist_locale_nlnl_DATA = locale/nl-nl/belgiumeid.properties

dist_platform_darwin_defaults_preferences_DATA = platform/darwin/defaults/preferences/belgiumeid.js

dist_platform_linux_defaults_preferences_DATA = platform/linux/defaults/preferences/belgiumeid.js

dist_platform_winnt_defaults_preferences_DATA = platform/winnt/defaults/preferences/belgiumeid.js

dist_skin_DATA = skin/icon.png \
			skin/overlay.css

extensionrootdir=$(mozext)/$(XPI_ID)
contentdir=$(extensionrootdir)/content
defaults_preferencesdir=$(extensionrootdir)/defaults/preferences
locale_enusdir=$(extensionrootdir)/locale/en-us
locale_frfrdir=$(extensionrootdir)/locale/fr-fr
locale_nlnldir=$(extensionrootdir)/locale/nl-nl
platform_darwin_defaults_preferencesdir=$(extensionrootdir)/platform/darwin/defaults/preferences
platform_linux_defaults_preferencesdir=$(extensionrootdir)/platform/linux/defaults/preferences
platform_winnt_defaults_preferencesdir=$(extensionrootdir)/platform/winnt/defaults/preferences
skindir=$(extensionrootdir)/skin

$(XPI_NAME): $(xpi_files)
#	 Create a temporary directory $tmp in $TMPDIR (default /tmp).
#	 Use mktemp if possible; otherwise fall back on mkdir,
#	 with $RANDOM to make collisions less likely.
	{ \
		tmp=` \
			(umask 077 && mktemp -d $(TMPDIR)/xpiXXXXXX) 2>/dev/null \
		` && \
		test -n "$$tmp" && test -d "$$tmp"; \
	} || { \
		tmp=$(TMPDIR)/xpi$$RANDOM; \
		umask 077 && mkdir -p "$$tmp"; \
	} || exit $$?; \
	for xpifile in $^; do \
		mkdir -p $$tmp/`dirname $$xpifile`/; \
		cp $$xpifile $$tmp/`dirname $$xpifile`/; \
	done; \
	rm -f $(abs_builddir)/$@; \
	cd $$tmp && \
	@ZIP@ -r $(abs_builddir)/$@ .; \
	cd $(abs_builddir); \
	rm -rf $$tmp

