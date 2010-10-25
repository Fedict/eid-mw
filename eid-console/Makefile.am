EXTRA_DIST = pom.xml eid-console-package eid-console-runtime eid-console-webapp

all: pom.xml eid-console-package eid-console-runtime eid-console-webapp
	cd $(srcdir) ; chmod -R u+w eid-console-package eid-console-runtime eid-console-webapp
	cd $(srcdir) ; $(MVN) -e install

clean-local:
	cd $(srcdir) ; $(MVN) clean


