XML2_VERSION=2.9.10
TP_DIR=../../../../../../ThirdParty

build: # builddir/.libs/libxml2.a

clean-builds:
	rm -rf config.stamp libxml2 libxml2-$(XML2_VERSION) builddir-x86_64 builddir-arm64 builddir
clean: clean-builds
	rm -rf libxml2-$(XML2_VERSION).tar.gz 

# libxml2-$(XML2_VERSION).tar.gz:
#	if [ -f $(TP_DIR)/libxml2-$(XML2_VERSION).tar.gz ]; then \
#		cp $(TP_DIR)/libxml2-$(XML2_VERSION).tar.gz .; \
#	else \
#		curl -O ftp://xmlsoft.org/libxml2/libxml2-$(XML2_VERSION).tar.gz; \
#		shasum -a 256 -c libxml2-$(XML2_VERSION).tar.gz.sha256; \
#	fi

# libxml2-$(XML2_VERSION)/configure: libxml2-$(XML2_VERSION)
# libxml2-%: libxml2-$(XML2_VERSION)
	mkdir -p $@

# libxml2-$(XML2_VERSION): libxml2-$(XML2_VERSION).tar.gz
#	tar xvzf libxml2-$(XML2_VERSION).tar.gz

builddir-x86_64/config.status: builddir-x86_64/.exists libxml2-$(XML2_VERSION)/configure
	cd builddir-x86_64 && ../libxml2-$(XML2_VERSION)/configure --enable-shared --enable-static CFLAGS='-target x86_64-apple-macos10.9 -mmacosx-version-min=10.9' --host=x86_64-apple-darwin --without-lzma && cd ..

builddir-arm64/config.status: builddir-arm64/.exists libxml2-$(XML2_VERSION)/configure
	cd builddir-arm64 && ../libxml2-$(XML2_VERSION)/configure --enable-shared --enable-static CFLAGS='-target arm64-apple-macos11 -mmacosx-version-min=11' --host=aarch64-apple-darwin --without-lzma && cd ..

builddir-i386/config.status: builddir-i386/.exists libxml2-$(XML2_VERSION)/configure
	cd builddir-i386 && ../libxml2-$(XML2_VERSION)/configure --enable-shared --enable-static CFLAGS='-target arm64-apple-macos11 -mmacosx-version-min=11' --host=i386-apple-darwin --without-lzma && cd ..

builddir-ppc/config.status: builddir-ppc/.exists libxml2-$(XML2_VERSION)/configure
	cd builddir-ppc && ../libxml2-$(XML2_VERSION)/configure --enable-shared --enable-static CFLAGS='-target arm64-apple-macos11 -mmacosx-version-min=11' --host=ppc-apple-darwin --without-lzma && cd ..

builddir-ppc64/config.status: builddir-ppc64/.exists libxml2-$(XML2_VERSION)/configure
	cd builddir-ppc64 && ../libxml2-$(XML2_VERSION)/configure --enable-shared --enable-static CFLAGS='-target arm64-apple-macos11 -mmacosx-version-min=11' --host=ppc64-apple-darwin --without-lzma && cd ..

builddir-%/.exists: libxml2-$(XML2_VERSION)
	rm -rf $(dir $@)
	mkdir $(dir $@)
	touch $@

builddir/.libs/libxml2.a: builddir-x86_64/.libs/libxml2.a builddir-arm64/.libs/libxml2.a
	mkdir -p $(dir $@)
	lipo -create -output $@ $^

builddir-%/.libs/libxml2.a: builddir-%/libxml2.la
builddir-%/libxml2.la: builddir-%/config.status
	$(MAKE) -C $(dir $@)

.SECONDARY: builddir/config.status builddir/.exists
