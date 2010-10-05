noinst_HEADERS = eid-mw.spec

all: 
	$(RPMDEVSETUPTREE)
	cp ../../eid-mw-4.0.0-latest.tar.gz $$HOME/rpmbuild/SOURCES/eid-mw-4.0.0.tar.gz
	QA_RPATHS=$$[ 0x0001|0x0010 ] $(RPMBUILD) -bb eid-mw.spec

eid-mw.spec: eid-mw.spec.tmpl
	$(SED) -e "s|_SVN_REVISION_|$$(cat ../svn_revision)|" < eid-mw.spec.tmpl > eid-mw.spec

clean:
	rm -f eid-mw.spec
