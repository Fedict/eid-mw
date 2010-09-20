noinst_HEADERS = eid-mw.spec

rpm: 
	$(RPMDEVSETUPTREE)
	cd $$HOME/rpmbuild/SOURCES
	wget http://eidmw.yourict.net/dist/sources/eid-mw-4.0.0.tar.gz
	$(RPMBUILD) -ba eid-mw.spec
