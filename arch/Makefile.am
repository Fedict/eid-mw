noinst_HEADERS = PKGBUILD

all: 
	makepkg -s

PKGBUILD: PKGBUILD.tmpl
	$(SED) -e "s|_SVN_REVISION_|$$(cat ../svn_revision)|" < PKGBUILD.tmpl > PKGBUILD

clean:
	rm -f PKGBUILD
