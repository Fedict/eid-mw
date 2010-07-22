ACLOCAL_AMFLAGS = -I m4

if WIN32
EXTRA_SUBDIRS=minidriver
endif

SUBDIRS=common dialogs cardlayer pkcs11 xpi $(EXTRA_SUBDIRS)
DIST_SUBDIRS=common dialogs cardlayer pkcs11 xpi nsis $(EXTRA_SUBDIRS)

xpipackage:
	$(MAKE) -C xpi xpipackage
