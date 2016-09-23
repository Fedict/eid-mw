OSSL_VERSION=1.0.2i
TP_DIR=../../../../../../ThirdParty

build: openssl/libcrypto.dylib

clean:
	rm -rf configured openssl openssl-$(OSSL_VERSION) openssl-$(OSSL_VERSION).tar.gz

configured: openssl/Configure
	cd openssl; ./Configure no-shared no-asm darwin64-x86_64:cc:-mmacosx-version-min=10.7
	$(MAKE) -C openssl depend
	touch configured

openssl/Configure: openssl

openssl/lib%.dylib: configured
	$(MAKE) -C openssl $(patsubst openssl/%.dylib,build_%,$@)

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
