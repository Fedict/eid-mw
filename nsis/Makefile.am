noinst_HEADERS = eid-mw.nsi

all:

# make sure the dist dirs exist
	mkdir -p src/dist/win32/usr/local/bin
	mkdir -p src/dist/win64/usr/local/bin

# Copy current mingw (gcc) 32-bit runtime libs
	cp "`dirname \`which i686-w64-mingw32-gcc\``/libgcc_s_sjlj-1.dll" src/dist/win32/usr/local/bin
	cp "`dirname \`which i686-w64-mingw32-gcc\``/libstdc++-6.dll"     src/dist/win32/usr/local/bin

# Copy current mingw (gcc) 64-bit runtime libs
	cp "`dirname \`which x86_64-w64-mingw32-gcc\``/libgcc_s_sjlj-1.dll" src/dist/win64/usr/local/bin
	cp "`dirname \`which x86_64-w64-mingw32-gcc\``/libstdc++-6.dll"     src/dist/win64/usr/local/bin

# Run nsis to create installer
	$(MAKENSIS) eid-mw.nsi

eid-mw.nsi: eid-mw.nsi.tmpl
	$(SED) -e "s|_SVN_REVISION_|$$(cat ../svn_revision)|" < eid-mw.nsi.tmpl > eid-mw.nsi

clean:
	rm -f src/*.exe
	rm -Rf src/dist/win32/*
	rm -Rf src/dist/win64/*
	rm -f eid-mw.nsi

