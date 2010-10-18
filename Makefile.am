ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = arch debian rpm svn_revision minidriver sccertprop nsis dmg VS_2010

BUILT_SOURCES=svn_revision

if WIN32
WINDOWS_ONLY=minidriver sccertprop 
endif

if NO_DIALOGS
else
MAYBE_DIALOGS=dialogs
endif

SUBDIRS=common $(MAYBE_DIALOGS) cardlayer pkcs11 xpi eid-console $(WINDOWS_ONLY)
DIST_SUBDIRS=common $(MAYBE_DIALOGS) cardlayer pkcs11 xpi 

svn_revision:
	svnversion > svn_revision

xpipackage:
	$(MAKE) -C xpi xpipackage


