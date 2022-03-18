OSSL_VERSION=1.1.1n
TP_DIR=../../../../../../ThirdParty

build: openssl-combined/libcrypto.a

clean-builds:
	rm -rf $(CONFIGURED) openssl openssl-combined openssl-$(OSSL_VERSION) openssl-arm64 openssl-x86_64 configured
clean: clean-builds
	rm -rf openssl-$(OSSL_VERSION).tar.gz 

configured: openssl-combined/Configure openssl-x86_64 openssl-arm64
	cd openssl-x86_64; ../openssl-combined/Configure no-shared no-asm -mmacosx-version-min=10.9 darwin64-x86_64-cc
	cd openssl-arm64; ../openssl-combined/Configure no-shared no-asm -mmacosx-version-min=11 darwin64-arm64-cc
	$(MAKE) -C openssl-x86_64 depend
	$(MAKE) -C openssl-arm64 depend
	touch configured

openssl-combined/Configure: openssl-combined

openssl-%: openssl-combined
	mkdir -p $@

openssl-combined/libcrypto.a: openssl-x86_64/libcrypto.a openssl-arm64/libcrypto.a
	rm -f $@
	lipo -create -output $@ $^

openssl-x86_64/libcrypto.a: configured
	$(MAKE) -C openssl-x86_64

openssl-arm64/libcrypto.a: configured
	$(MAKE) -C openssl-arm64

openssl-combined: openssl-$(OSSL_VERSION)
	ln -sf openssl-$(OSSL_VERSION) openssl-combined

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
