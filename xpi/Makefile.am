ACLOCAL_AMFLAGS         = -I m4

XPI_ID = belgiumeid@eid.belgium.be

if LINUX
XPI_INSTALL_DIR   ?= /usr/share/mozilla/extensions/
endif

if OSX
XPI_INSTALL_DIR   ?= "/Library/Application Support/Mozilla Firefox/Extensions/"
endif

if WIN32
XPI_INSTALL_DIR   ?= "/c/Program Files/Mozilla Firefox/Extensions/"
endif


XPI_FILES = $(srcdir)/src/content/BelgiumEidPKCS11.js \
			$(srcdir)/src/content/firefoxOverlay.xul \
			$(srcdir)/src/content/overlay.js \
			$(srcdir)/src/content/util.js \
			$(srcdir)/src/defaults/preferences/belgiumeid.js \
			$(srcdir)/src/locale/en-us/belgiumeid.dtd \
			$(srcdir)/src/locale/fr-fr/belgiumeid.properties \
			$(srcdir)/src/locale/nl-nl/belgiumeid.properties \
			$(srcdir)/src/platform/darwin/defaults/preferences/belgiumeid.js \
			$(srcdir)/src/platform/linux/defaults/preferences/belgiumeid.js \
			$(srcdir)/src/platform/winnt/defaults/preferences/belgiumeid.js \
			$(srcdir)/src/skin/icon.png \
			$(srcdir)/src/skin/overlay.css \
			$(srcdir)/src/chrome.manifest \
			$(srcdir)/src/icon.png \
			$(srcdir)/src/install.rdf 

dist_data_DATA = belgiumeid.xpi

TMPDIR ?= /tmp

belgiumeid.xpi: $(XPI_FILES)
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
		umask 077 && mkdir "$$tmp"; \
	} || exit $$?; \
	for xpifile in $^; do \
		mkdir -p $$tmp/`dirname $$xpifile`/; \
		cp $$xpifile $$tmp/`dirname $$xpifile`/; \
	done; \
	rm -f $(abs_builddir)/$@; \
	cd $$tmp$(scrdir)/src && \
	@ZIP@ -r $(abs_builddir)/$@ .; \
	rm -rf $$tmp
	 
install-data-local: $(XPI_FILES)
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
		umask 077 && mkdir "$$tmp"; \
	} || exit $$?; \
	for xpifile in $^; do \
		mkdir -p $$tmp/`dirname $$xpifile`/; \
		cp $$xpifile $$tmp/`dirname $$xpifile`/; \
	done; \
	test -z "$(XPI_INSTALL_DIR)$(XPI_ID)" || mkdir -p $(XPI_INSTALL_DIR)$(XPI_ID)/; \
	cp -R $$tmp$(scrdir)/src/* $(XPI_INSTALL_DIR)$(XPI_ID)/;
	
	