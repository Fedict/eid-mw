ACLOCAL_AMFLAGS = -I scripts/m4
EXTRA_DIST = debian build-debian.sh rpm doc

SUBDIRS=cardcomm/pkcs11/src tests/unit plugins_tools/xpi plugins_tools/aboutmw/gtk

xpipackage:
	$(MAKE) -C plugins_tools/xpi xpipackage
