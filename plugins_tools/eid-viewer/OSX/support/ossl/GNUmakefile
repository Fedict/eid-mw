OSSL_VERSION=1.0.2g
TP_DIR=../../../../../../ThirdParty

build: openssl/libssl.dylib openssl/libcrypto.dylib

clean:
	rm -rf configured openssl openssl-$(OSSL_VERSION)*

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
	rm -f openssl-$(OSSL_VERSION).tar.gz*
	if [ -f $(TP_DIR)/openssl-$(OSSL_VERSION).tar.gz ]; then \
		cp $(TP_DIR)/openssl-$(OSSL_VERSION).tar.gz . ; \
	else \
		curl -O https://www.openssl.org/source/openssl-$(OSSL_VERSION).tar.gz; \
		curl -O https://www.openssl.org/source/openssl-$(OSSL_VERSION).tar.gz.sha256; \
		sed -i '' -e "s/\$$/  openssl-$(OSSL_VERSION).tar.gz/" openssl-$(OSSL_VERSION).tar.gz.sha256; \
		shasum -a 256 -c openssl-$(OSSL_VERSION).tar.gz.sha256; \
	fi

.SECONDARY: configured
