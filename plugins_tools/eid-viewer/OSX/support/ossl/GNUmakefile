OSSL_VERSION=1.0.2d

build: openssl/libssl.dylib openssl/libcrypto.dylib

clean:
	rm -rf configured openssl openssl-$(OSSL_VERSION)*

configured: openssl
	cd openssl; ./Configure darwin64-x86_64-cc -shared
	touch configured

openssl/lib%.dylib: configured
	$(MAKE) -C openssl $(patsubst openssl/%.dylib,build_%,$@)

openssl: openssl-$(OSSL_VERSION)
	ln -sf openssl-$(OSSL_VERSION) openssl

openssl-$(OSSL_VERSION): openssl-$(OSSL_VERSION).tar.gz
	tar xvf $<

openssl-$(OSSL_VERSION).tar.gz:
	rm -f openssl-$(OSSL_VERSION).tar.gz*
	curl -O https://www.openssl.org/source/openssl-$(OSSL_VERSION).tar.gz
	curl -O https://www.openssl.org/source/openssl-$(OSSL_VERSION).tar.gz.sha256
	sed -i '' -e "s/\$$/  openssl-$(OSSL_VERSION).tar.gz/" openssl-$(OSSL_VERSION).tar.gz.sha256
	shasum -a 256 -c openssl-$(OSSL_VERSION).tar.gz.sha256

.SECONDARY: configured
