ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = rpm debian svn_revision minidriver nsis dmg

BUILT_SOURCES=svn_revision

if WIN32
MAYBE_MINIDRIVER=minidriver
endif

if NO_DIALOGS
else
MAYBE_DIALOGS=dialogs
endif

SUBDIRS=common $(MAYBE_DIALOGS) cardlayer pkcs11 xpi $(MAYBE_MINIDRIVER)
DIST_SUBDIRS=common $(MAYBE_DIALOGS) cardlayer pkcs11 xpi 

svn_revision:
	svnversion > svn_revision

xpipackage:
	$(MAKE) -C xpi xpipackage


