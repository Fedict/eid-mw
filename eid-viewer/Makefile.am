EXTRA_DIST = pom.xml eid-viewer-package eid-viewer-lib eid-viewer-gui

all: pom.xml eid-viewer-package eid-viewer-lib eid-viewer-gui
	cd $(srcdir) ; chmod -R u+w eid-viewer-package eid-viewer-lib eid-viewer-gui
	cd $(srcdir) ; $(MVN) -e install

clean-local:
	cd $(srcdir) ; $(MVN) clean


