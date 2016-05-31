ACLOCAL_AMFLAGS = -I scripts/m4
EXTRA_DIST = scripts/build-aux/config.rpath  debian build-debian.sh rpm doc

SUBDIRS=cardcomm/pkcs11/src doc/sdk/include/rsaref220 plugins_tools/util tests/unit plugins_tools/xpi plugins_tools/aboutmw/gtk plugins_tools/eid-viewer

xpipackage:
	$(MAKE) -C plugins_tools/xpi xpipackage
