noinst_HEADERS = eid-mw.spec

all: 
	$(RPMDEVSETUPTREE)
	$(WGET) -P $$HOME/rpmbuild/SOURCES http://eidmw.yourict.net/dist/sources/eid-mw-4.0.0.tar.gz
	QA_RPATHS=$$[ 0x0001|0x0010 ] $(RPMBUILD) -bb eid-mw.spec

eid-mw.spec: eid-mw.spec.tmpl
	sed -e "s|_SVN_REVISION_|$$(cat ../svn_revision)|" < eid-mw.spec.tmpl > eid-mw.spec

clean:
	rm -f eid-mw.spec
