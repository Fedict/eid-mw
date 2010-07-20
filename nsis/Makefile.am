all : nsis

.PHONY: nsis
nsis:
# Copy current mingw (gcc) runtime libs
	cp "`dirname \`which i686-w64-mingw32-gcc\``/libgcc_s_sjlj-1.dll" src/dist/win32/bin
	cp "`dirname \`which i686-w64-mingw32-gcc\``/libstdc++-6.dll"     src/dist/win32/bin

# FIXME replace with correct 64-bit libs
	cp "`dirname \`which i686-w64-mingw32-gcc\``/libgcc_s_sjlj-1.dll" src/dist/win64/bin
	cp "`dirname \`which i686-w64-mingw32-gcc\``/libstdc++-6.dll"     src/dist/win64/bin

	$(MAKENSIS) src/eid-mw.nsi

clean:
	rm -f src/*.exe
	rm -Rf src/dist/win32/*
	rm -Rf src/dist/win64/*
