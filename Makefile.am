ACLOCAL_AMFLAGS = -I scripts/m4
EXTRA_DIST = debian build-debian.sh rpm svn_revision doc

BUILT_SOURCES=svn_revision

SUBDIRS=cardcomm/pkcs11/src tests/unit plugins_tools/xpi

svn_revision:
	svnversion > svn_revision

xpipackage:
	$(MAKE) -C plugins_tools/xpi xpipackage

clean-local:
	rm -rf svn_revision
