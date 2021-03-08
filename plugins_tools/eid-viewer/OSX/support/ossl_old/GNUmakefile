OSSL_VERSION=1.1.1i
TP_DIR=../../../../../../ThirdParty

build: openssl/libcrypto.a openssl/include/openssl/opensslconf.h

clean-builds:
	rm -rf $(CONFIGURED) openssl openssl-$(OSSL_VERSION) openssl-arm64 openssl-x86_64
clean:
	rm -rf openssl-$(OSSL_VERSION).tar.gz 

configured: openssl/Configure openssl-x86_64 openssl-arm64
	cd openssl-x86_64; ../openssl/Configure no-shared no-asm -mmacosx-version-min=10.9 darwin64-x86_64-cc
	cd openssl-arm64; ../openssl/Configure no-shared no-asm -mmacosx-version-min=11 darwin64-arm64-cc
	$(MAKE) -C openssl-x86_64 depend
	$(MAKE) -C openssl-arm64 depend
	touch configured

openssl/Configure: openssl

openssl-%: openssl
	mkdir -p $@

# OpenSSL needs this file, but it just so happens that it's the same for
# both x86 and arm64, so...
openssl/include/openssl/opensslconf.h: configured
	diff -q openssl-x86_64/include/openssl/opensslconf.h openssl-arm64/include/openssl/opensslconf.h
	cp -a openssl-x86_64/include/openssl/opensslconf.h openssl/include/openssl/opensslconf.h

openssl/lib%.a: openssl-x86_64/lib%.a openssl-arm64/lib%.a
	lipo -create -output $@ $^

openssl-x86_64/lib%.a: configured
	$(MAKE) -C openssl-x86_64

openssl-arm64/lib%.a: configured
	$(MAKE) -C openssl-arm64

openssl: openssl-$(OSSL_VERSION)
	ln -sf openssl-$(OSSL_VERSION) openssl

openssl-$(OSSL_VERSION): openssl-$(OSSL_VERSION).tar.gz
	tar xvf $<

openssl-$(OSSL_VERSION).tar.gz:
	rm -f openssl-$(OSSL_VERSION).tar.gz
	if [ -f $(TP_DIR)/openssl-$(OSSL_VERSION).tar.gz ]; then \
		cp $(TP_DIR)/openssl-$(OSSL_VERSION).tar.gz . ; \
	else \
		curl -O https://www.openssl.org/source/openssl-$(OSSL_VERSION).tar.gz; \
		shasum -a 256 -c openssl-$(OSSL_VERSION).tar.gz.sha256; \
	fi

.SECONDARY: configured
