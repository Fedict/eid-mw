EXTRA_DIST = pom.xml eid-console-package eid-console-runtime eid-console-webapp

all: pom.xml eid-console-package eid-console-runtime eid-console-webapp
	[ -d eid-console-package ] || cp -r $(srcdir)/eid-console-package . && chmod -R u+w eid-console-package
	[ -d eid-console-runtime ] || cp -r $(srcdir)/eid-console-runtime . && chmod -R u+w eid-console-runtime
	[ -d eid-console-webapp  ] || cp -r $(srcdir)/eid-console-webapp  . && chmod -R u+w eid-console-webapp
	[ -f pom.xml             ] || cp $(srcdir)/pom.xml                .
	$(MVN) -e install

clean-local:
	$(MVN) clean

distcleancheck_listfiles = find . -type f -exec sh -c 'test -f $(srcdir)/$$1 || echo $$1' sh '{}' ';'


