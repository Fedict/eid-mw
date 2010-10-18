# Auto-makefile for the eID Console.

all : eid-console

.PHONY: eid-console
eid-console:
	$(MVN) -e install

install:
	install -d $(DESTDIR)$(bindir)/
	install -d $(DESTDIR)$(datadir)/

if LINUX
	cd eid-console-package/target ; tar xvfz eid-console*.tar.gz ; rm eid-console/*.exe
	cp -r eid-console-package/target/eid-console $(DESTDIR)$(datadir)/
endif

if OSX
	cd eid-console-package/target ; tar xvfz eid-console*.tar.gz ; rm eid-console/*.exe
	cp -r eid-console-package/target/eid-console $(DESTDIR)$(datadir)/
endif

if WIN32
	cd eid-console-package/target ; tar xvfz eid-console*.tar.gz ; rm eid-console/*.sh
	cp -r eid-console-package/target/eid-console $(DESTDIR)$(datadir)/
endif


clean-local:
	$(MVN) clean

