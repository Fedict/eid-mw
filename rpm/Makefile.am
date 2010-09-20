noinst_HEADERS = eid-mw.spec

rpm: 
	$(RPMDEVSETUPTREE)
	$(WGET) -P=$$HOME/rpmbuild/SOURCES http://eidmw.yourict.net/dist/sources/eid-mw-4.0.0.tar.gz
	$(RPMBUILD) -ba eid-mw.spec
