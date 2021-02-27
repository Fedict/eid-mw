# GNU Makefile for building ReSSL
#
LIBRESSL_VERSION=3.3.1
TP_DIR=$(shell pwd)
HOST_ARCH = $(shell uname -m)
ARCHS = x86_64 arm64 # i386 ppc ppc64
PREFIX = /usr/local

# Flags and version
MACVERSION = 10.9
CFLAGS = -Os -mmacosx-version-min=$(MACVERSION)
CXXFLAGS = -Os -mmacosx-version-min=$(MACVERSION)
LDFLAGS = -mmacosx-version-min=$(MACVERSION)

all: build

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.cxx
	$(CC) $(CXXFLAGS) -o $@ $<

%.o: %.cpp
	$(CC) $(CXXFLAGS) -o $@ $<

build: libressl-$(LIBRESSL_VERSION).tar.gz libressl-$(LIBRESSL_VERSION) libressl configured libressl/include/openssl/opensslconf.h make_universal

make_universal-build:
	mkdir -p $@

install:
	ditto libressl/libcrypto.46.dylib $(PREFIX)/lib
	ditto libressl/libssl.48.dylib $(PREFIX)/lib
	ditto libressl/libtls.20.dylib $(PREFIX)/lib
	ditto libressl/*.a $(PREFIX)/lib
	ditto libressl/include/* $(PREFIX)/include
	cd "$(PREFIX)/lib"
	ln -sf libcrypto.46.dylib libcrypto.dylib
	ln -sf libssl.48.dylib libssl.dylib
	ln -sf libtls.20.dylib libtls.dylib

make_universal:
	# Static Libraries
	lipo -create -output libressl/libcrypto.a libressl-x86_64/crypto/.libs/libcrypto.a libressl-arm64/crypto/.libs/libcrypto.a # libressl-i386/crypto/.libs/libcrypto.a libressl-ppc/crypto/.libs/libcrypto.a libressl-ppc64/crypto/.libs/libcrypto.a
	lipo -create -output libressl/libcompat.a libressl-x86_64/crypto/.libs/libcompat.a libressl-arm64/crypto/.libs/libcompat.a # libressl-i386/crypto/.libs/libcompat.a libressl-ppc/crypto/.libs/libcompat.a libressl-ppc64/crypto/.libs/libcompat.a
	lipo -create -output libressl/libcompatnoopt.a libressl-x86_64/crypto/.libs/libcompatnoopt.a libressl-arm64/crypto/.libs/libcompatnoopt.a # libressl-i386/crypto/.libs/libcompatnoopt.a libressl-ppc/crypto/.libs/libcompatnoopt.a libressl-ppc64/crypto/.libs/libcompatnoopt.a
	lipo -create -output libressl/libssl.a libressl-x86_64/ssl/.libs/libssl.a libressl-arm64/ssl/.libs/libssl.a # libressl-i386/ssl/.libs/libssl.a libressl-ppc/ssl/.libs/libssl.a libressl-ppc64/ssl/.libs/libssl.a
	lipo -create -output libressl/libssl.a libressl-x86_64/tls/.libs/libtls.a libressl-arm64/tls/.libs/libtls.a # libressl-i386/tls/.libs/libtls.a libressl-ppc/tls/.libs/libtls.a libressl-ppc64/tls/.libs/libtls.a
	# Applications
	lipo -create -output libressl/openssl libressl-x86_64/apps/openssl/.libs/openssl libressl-arm64/apps/openssl/.libs/openssl # libressl-i386/apps/openssl/.libs/openssl libressl-ppc/apps/openssl/.libs/openssl libressl-ppc64/apps/openssl/.libs/openssl
#	lipo -create -output libressl/nc libressl-x86_64/apps/nc/.libs/nc libressl-arm64/apps/nc/.libs/nc # libressl-i386/apps/nc/.libs/nc libressl-ppc/apps/nc/.libs/nc libressl-ppc64/apps/nc/.libs/nc
	lipo -create -output libressl/ocspcheck libressl-x86_64/apps/ocspcheck/.libs/ocspcheck  libressl-arm64/apps/ocspcheck/.libs/ocspcheck # libressl-i386/apps/ocspcheck/.libs/ocspcheck libressl-ppc/apps/ocspcheck/.libs/ocspcheck libressl-ppc64/apps/ocspcheck/.libs/ocspcheck
	# Dynamic Libraries
	lipo -create -output libressl/libcrypto.46.dylib libressl-x86_64/crypto/.libs/libcrypto.46.dylib libressl-arm64/crypto/.libs/libcrypto.46.dylib # libressl-i386/crypto/.libs/libcrypto.46.dylib libressl-ppc/crypto/.libs/libcrypto.46.dylib libressl-ppc64/crypto/.libs/libcrypto.46.dylib
	lipo -create -output libressl/libssl.48.dylib libressl-x86_64/ssl/.libs/libssl.48.dylib libressl-arm64/ssl/.libs/libssl.48.dylib # libressl-i386/ssl/.libs/libssl.48.dylib libressl-ppc/ssl/.libs/libssl.48.dylib libressl-ppc64/ssl/.libs/libssl.48.dylib
	lipo -create -output libressl/libtls.20.dylib libressl-x86_64/tls/.libs/libtls.20.dylib libressl-arm64/tls/.libs/libtls.20.dylib # libressl-i386/tls/.libs/libtls.20.dylib libressl-ppc/tls/.libs/libtls.20.dylib libressl-ppc64/tls/.libs/libtls.20.dylib
	# Symbolic Links
	cd libressl
	ln -sf libssl.48.dylib libssl.dylib 
	ln -sf libcrypto.46.dylib libcrypto.46.dylib
	ln -sf libtls.20.dylib libtls.dylib

clean-builds:
	rm -rf $(CONFIGURED) libressl libressl-$(LIBRESSL_VERSION) libressl-arm64 libressl-x86_64
clean:
	rm -rf libressl-$(LIBRESSL_VERSION).tar.gz 

configured: libressl/configure libressl-x86_64 libressl-arm64 # libressl-i386 libressl-ppc libressl-ppc64
	cd libressl-x86_64; CFLAGS="-arch x86_64 -Os" CXXFLAGS="-arch x86_64 -Os" LDFLAGS="-arch x86_64" ../libressl/configure --disable-dependency-tracking --host=x86_64-apple-darwin --target=x86_64-apple-darwin --build=$(HOST_ARCH)-apple-darwin darwin && cd ..
	cd libressl-arm64; CFLAGS="-arch arm64 -Os" CXXFLAGS="-arch arm64 -Os" LDFLAGS="-arch arm64" ../libressl/configure --disable-dependency-tracking --host=arm64-apple-darwin --target=arm64-apple-darwin --build=$(HOST_ARCH)-apple-darwin darwin && cd ..
#	cd libressl-i386 && CFLAGS="-arch i386 -isysroot "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk" -Os" CXXFLAGS="-arch i386 -isysroot "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk" -Os" LDFLAGS="-arch i386" ../libressl/configure --disable-dependency-tracking --host=i686-apple-darwin --target=i686-apple-darwin --build=$(HOST_ARCH)-apple-darwin darwin && cd ..
#	cd libressl-ppc; CFLAGS="-arch ppc -isysroot "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.5.sdk -Os" CXXFLAGS="-arch ppc -isysroot "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.5.sdk -Os" LDFLAGS="-arch ppc" ../libressl/configure --disable-dependency-tracking --host=powerpc-apple-darwin --target=powerpc-apple-darwin --build=$(HOST_ARCH)-apple-darwin darwin && cd ..
#	cd libressl-ppc64 && CFLAGS="-arch ppc64 -isysroot "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk -Os" CXXFLAGS="-arch ppc64 -isysroot "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk -Os" LDFLAGS="-arch ppc64" ../libressl/configure --disable-dependency-tracking --host=powerpc64-apple-darwin --target=powerpc64-apple-darwin --build=$(HOST_ARCH)-apple-darwin darwin && cd ..
	$(MAKE) -C libressl-x86_64
	$(MAKE) -C libressl-arm64
#	$(MAKE) -C libressl-i386
#	$(MAKE) -C libressl-ppc
#	$(MAKE) -C libressl-ppc64
	touch configured

libressl/configure: libressl

libressl-%: libressl
	mkdir -p $@

# LibreSSL needs this file, but it just so happens that it's the same for
# both x86 and arm64, so...
libressl/include/openssl/opensslconf.h: configured
	if [ -nf "libressl-arm64/include/openssl/opensslconf.h" ]; then ditto libressl-$(HOST_ARCH)/include/openssl/opensslconf.h libressl-arm64/include/openssl/opensslconf.h; fi
	if [ -nf "libressl-x86_64/include/openssl/opensslconf.h" ]; then ditto libressl-$(HOST_ARCH)/include/openssl/opensslconf.h libressl-x86_64/include/openssl/opensslconf.h; fi
	if [ -nf "libressl-i386/include/openssl/opensslconf.h" ]; then ditto libressl-$(HOST_ARCH)/include/openssl/opensslconf.h libressl-i386/include/openssl/opensslconf.h; fi
	if [ -nf "libressl-ppc/include/openssl/opensslconf.h" ]; then ditto libressl-$(HOST_ARCH)/include/openssl/opensslconf.h libressl-ppc/include/openssl/opensslconf.h; fi
	if [ -nf "libressl-ppc64/include/openssl/opensslconf.h" ]; then ditto libressl-$(HOST_ARCH)/include/openssl/opensslconf.h libressl-ppc64/include/openssl/opensslconf.h; fi

libressl/libcrypto.a: libressl-x86_64/crypto/.libs/libcrypto.a libressl-arm64/crypto/.libs/libcrypto.a # libressl-i386/crypto/.libs/libcrypto.a libressl-ppc/crypto/.libs/libcrypto.a libressl-ppc64/crypto/.libs/libcrypto.a
	lipo -create -output $@ $^

libressl/lib%.a: libressl-x86_64/crypto/.libs/lib%.a libressl-arm64/crypto/.libs/lib%.a # libressl-i386/crypto/.libs/lib%.a libressl-ppc/crypto/.libs/lib%.a libressl-ppc64/crypto/.libs/lib%.a
	lipo -create -output $@ $^

libressl/lib%.dylib: libressl-x86_64/crypto/.libs/lib%.dylib libressl-arm64/crypto/.libs/lib%.dylib # libressl-i386/crypto/.libs/lib%.dylib libressl-ppc/crypto/.libs/lib%.dylib libressl-ppc64/crypto/.libs/lib%.dylib
	lipo -create -output $@ $^

libressl-x86_64/lib%.a: configured
	$(MAKE) -C libressl-x86_64

libressl-arm64/lib%.a: configured
	$(MAKE) -C libressl-arm64

libressl-i386/lib%.a: configured
	$(MAKE) -C libressl-i386

libressl-ppc/lib%.a: configured
	$(MAKE) -C libressl-ppc

libressl-ppc64/lib%.a: configured
	$(MAKE) -C libressl-ppc64

libressl: libressl-$(LIBRESSL_VERSION)
	ln -sf libressl-$(LIBRESSL_VERSION) libressl

libressl-$(LIBRESSL_VERSION): libressl-$(LIBRESSL_VERSION).tar.gz
	tar xvf $<
	cd libressl-$(LIBRESSL_VERSION) && patch -p1 < $(TP_DIR)/ssl-apple.diff && cd ..

libressl-$(LIBRESSL_VERSION).tar.gz:
	echo "Building LibreSSL $(LIBRESSL_VERSION)"
	curl -O https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-$(LIBRESSL_VERSION).tar.gz
	shasum -a 256 -c libressl-$(LIBRESSL_VERSION).tar.gz.sha256

.SECONDARY: configured
