XML2_VERSION=2.9.10
TP_DIR=../../../../../../ThirdParty

build: builddir/libxml2.la libxml2

clean:
	rm -rf config.stamp libxml2 libxml2-$(XML2_VERSION) libxml2-$(XML2_VERSION).tar.gz builddir

libxml2-$(XML2_VERSION).tar.gz:
	if [ -f $(TP_DIR)/libxml2-$(XML2_VERSION).tar.gz ]; then \
		cp $(TP_DIR)/libxml2-$(XML2_VERSION).tar.gz .; \
	else \
		curl -O ftp://xmlsoft.org/libxml2/libxml2-$(XML2_VERSION).tar.gz; \
		shasum -a 256 -c libxml2-$(XML2_VERSION).tar.gz.sha256; \
	fi

libxml2-$(XML2_VERSION)/configure: libxml2-$(XML2_VERSION)
libxml2: libxml2-$(XML2_VERSION)
	ln -sf libxml2-$(XML2_VERSION) libxml2

libxml2-$(XML2_VERSION): libxml2-$(XML2_VERSION).tar.gz
	tar xvzf libxml2-$(XML2_VERSION).tar.gz

builddir/config.status: builddir/.exists
	cd builddir; ../libxml2-$(XML2_VERSION)/configure --disable-shared --enable-static CFLAGS='-mmacosx-version-min=10.7'

builddir/.exists: libxml2-$(XML2_VERSION)
	rm -rf builddir
	mkdir builddir
	touch builddir/.exists

builddir/libxml2.la: builddir/config.status
	$(MAKE) -C builddir

.SECONDARY: builddir/config.status builddir/.exists
