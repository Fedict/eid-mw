ACLOCAL_AMFLAGS = -I scripts/m4
EXTRA_DIST = debian build-debian.sh rpm svn_revision doc

SUBDIRS=cardcomm/pkcs11/src tests/unit plugins_tools/xpi

xpipackage:
	$(MAKE) -C plugins_tools/xpi xpipackage
