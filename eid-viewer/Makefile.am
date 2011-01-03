EXTRA_DIST = pom.xml eid-viewer-package eid-viewer-lib eid-viewer-gui

all: pom.xml eid-viewer-package eid-viewer-lib eid-viewer-gui
	cd $(srcdir) ; chmod -R u+w eid-viewer-package eid-viewer-lib eid-viewer-gui
	cd $(srcdir) ; $(MVN) -e  -Dmaven.test.skip=true clean install

clean-local:
	cd $(srcdir) ; $(MVN) clean

install-data-local:
	cd $(srcdir) ; cp -r eid-viewer-package/target/package-4.0.0/eid-viewer $(DESTDIR)$(datadir)
	ln -sf $(DESTDIR)$(datadir)/eid-viewer/eid-viewer.sh $(DESTDIR)$(bindir)/eid-viewer ; chmod 755 $(DESTDIR)$(bindir)/eid-viewer

