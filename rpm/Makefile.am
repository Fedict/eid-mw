noinst_HEADERS = eid-mw.spec

all: 
	$(RPMDEVSETUPTREE)
	$(WGET) -P $$HOME/rpmbuild/SOURCES http://eidmw.yourict.net/dist/sources/eid-mw-4.0.0.tar.gz
	$(RPMBUILD) -bb eid-mw.spec
