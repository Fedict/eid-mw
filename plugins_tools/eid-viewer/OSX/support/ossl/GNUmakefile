OSSL_VERSION=1.1.1i
TP_DIR=../../../../../../ThirdParty

ifeq ($(ARCHS),)
ARCHS=x86_64 arm64
endif

BUILDS=$(addprefix openssl-, $(ARCHS))
CRYPTOLIBS=$(addsuffix /libcrypto.a,$(BUILDS))
CONFIGURED=$(addprefix configured-, $(ARCHS))
ARCHCOUNT=$(words $(ARCHS))

build: openssl/libcrypto.a openssl/include/openssl/opensslconf.h
clean-builds:
	rm -rf $(CONFIGURED) openssl openssl-$(OSSL_VERSION) openssl-arm64 openssl-x86_64
clean:
	rm -rf openssl-$(OSSL_VERSION).tar.gz 

configured-x86_64: openssl/Configure openssl-x86_64
	cd openssl-x86_64; ../openssl/Configure no-shared no-asm -mmacos-version-min=10.9 darwin64-x86_64-cc
	$(MAKE) -C openssl-x86_64 depend
	touch $@

configured-arm64: openssl/Configure openssl-arm64
	cd openssl-arm64; ../openssl/Configure no-shared no-asm -mmacosx-version-min=11 darwin64-arm64-cc
	$(MAKE) -C openssl-arm64 depend
	touch $@

configured: $(CONFIGURED)

openssl/Configure: openssl

openssl-%: openssl
	mkdir -p $@

openssl/include/openssl/opensslconf.h: $(CRYPTOLIBS)
# OpenSSL needs this file, but it just so happens that it's the same for
# both x86 and arm64, so...
ifeq ($(ARCHCOUNT),2)
	diff -q openssl-x86_64/include/openssl/opensslconf.h openssl-arm64/include/openssl/opensslconf.h
endif
	cp -a openssl-$(word 1,$(ARCHS))/include/openssl/opensslconf.h openssl/include/openssl/opensslconf.h

openssl/libcrypto.a: $(CRYPTOLIBS)
	lipo -create -output $@ $^

openssl-x86_64/libcrypto.a openssl-x86_64/include/openssl/opensslconf.h: configured-x86_64
	$(MAKE) -C openssl-x86_64

openssl-arm64/libcrypto.a openssl-arm64/include/openssl/opensslconf.h: configured-arm64
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
