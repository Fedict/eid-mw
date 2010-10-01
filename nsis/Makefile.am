all : nsis

.PHONY: nsis
nsis:

# make sure the dist dirs exist
	mkdir -p src/dist/win32/usr/local/bin
	mkdir -p src/dist/win64/usr/local/bin

# Copy current mingw (gcc) 32-bit runtime libs
	cp "`dirname \`which i686-w64-mingw32-gcc\``/libgcc_s_sjlj-1.dll" src/dist/win32/usr/local/bin
	cp "`dirname \`which i686-w64-mingw32-gcc\``/libstdc++-6.dll"     src/dist/win32/usr/local/bin

# Copy current mingw (gcc) 64-bit runtime libs
	cp "`dirname \`which x86_64-w64-mingw32-gcc\``/w64gcc_s_sjlj-1.dll" src/dist/win64/usr/local/bin
	cp "`dirname \`which x86_64-w64-mingw32-gcc\``/libstdc++-6.dll"     src/dist/win64/usr/local/bin

	$(MAKENSIS) src/eid-mw.nsi

clean:
	rm -f src/*.exe
	rm -Rf src/dist/win32/*
	rm -Rf src/dist/win64/*
