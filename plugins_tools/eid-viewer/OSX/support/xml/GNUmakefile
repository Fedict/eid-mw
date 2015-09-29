XML2_VERSION=2.9.2

build: builddir/libxml2.la

clean:
	rm -rf config.stamp libxml2 libxml2-$(XML2_VERSION)* builddir

libxml2-$(XML2_VERSION).tar.gz:
	curl -O ftp://xmlsoft.org/libxml2/libxml2-$(XML2_VERSION).tar.gz

libxml2-$(XML2_VERSION): libxml2-$(XML2_VERSION).tar.gz
	tar xvzf libxml2-$(XML2_VERSION).tar.gz

builddir/config.status: builddir
	cd builddir; ../libxml2-$(XML2_VERSION)/configure

builddir: libxml2-$(XML2_VERSION)
	rm -rf builddir
	mkdir builddir

builddir/libxml2.la: builddir/config.status
	$(MAKE) -C builddir

.SECONDARY: builddir/config.status
