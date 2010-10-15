noinst_HEADERS = eid-mw.spec

all: 
if RPM_BASED_DISTRO
	QA_RPATHS=$$[ 0x0001|0x0010 ] $(RPMBUILD) --sign -bb eid-mw.spec
endif

eid-mw.spec: eid-mw.spec.tmpl
	$(SED) -e "s|_SVN_REVISION_|$$(cat ../svn_revision)|" < eid-mw.spec.tmpl > eid-mw.spec

clean:
	rm -f eid-mw.spec

