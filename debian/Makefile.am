noinst_headers = changelog

all: clean changelog build

build:
	cd .. ; $(BUILDPACKAGE)

changelog: changelog.tmpl
	$(SED) -e "s|_SVN_REVISION_|$$(cat ../svn_revision)|" < changelog.tmpl > changelog

clean:
	rm -f changelog
