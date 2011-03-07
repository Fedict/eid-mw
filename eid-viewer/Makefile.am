EXTRA_DIST = pom.xml eid-viewer-package eid-viewer-lib eid-viewer-gui

####################################################################################################################

if UNIXEXE
all: pom.xml eid-viewer-package eid-viewer-lib eid-viewer-gui
	cd $(srcdir) ; chmod -R u+w eid-viewer-package eid-viewer-lib eid-viewer-gui
	cd $(srcdir) ; $(MVN) -Punixpackage -e -Dmaven.test.skip=true clean install

install-data-local:
	cd $(srcdir) ; cp -r eid-viewer-package/target/package-4.0.0/eid-viewer $(DESTDIR)$(datadir)
	ln -sf $(datadir)/eid-viewer/eid-viewer.sh $(DESTDIR)$(bindir)/eid-viewer

uninstall-local:
	rm -Rf $(DESTDIR)$(bindir)/eid-viewer
	rm -Rf $(DESTDIR)$(datadir)/eid-viewer
endif

####################################################################################################################

if WIN32EXE
all: pom.xml eid-viewer-package eid-viewer-lib eid-viewer-gui
	cd $(srcdir) ; chmod -R u+w eid-viewer-package eid-viewer-lib eid-viewer-gui
	cd $(srcdir) ; $(MVN) -Pwin32package -e -Dmaven.test.skip=true clean install
endif

####################################################################################################################

clean-local:
	cd $(srcdir) ; $(MVN) clean

