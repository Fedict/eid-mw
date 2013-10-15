ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = debian build-debian.sh rpm svn_revision misc sdk

BUILT_SOURCES=svn_revision

if NO_DIALOGS
else
MAYBE_DIALOGS=dialogs
endif

SUBDIRS=common $(MAYBE_DIALOGS) cardlayer pkcs11 xpi

svn_revision:
	svnversion > svn_revision

xpipackage:
	$(MAKE) -C xpi xpipackage

clean-local:
	rm -rf svn_revision
