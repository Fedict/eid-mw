ACLOCAL_AMFLAGS         = -I m4

noinst_LIBRARIES = libwinscard@LIBRARY_BITNESS@.a libcrypt32@LIBRARY_BITNESS@.a libcomctl32@LIBRARY_BITNESS@.a

lib_LTLIBRARIES = libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@.la
libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@_la_SOURCES = $(srcdir)/src/CardAndContProp.c $(srcdir)/src/CardCap.c $(srcdir)/src/CardPinOps.c \
	$(srcdir)/src/CryptoOps.c $(srcdir)/src/DllMain.c $(srcdir)/src/InitAndDeconstruct.c \
	$(srcdir)/src/KeyContainer.c $(srcdir)/src/ListMgr.c $(srcdir)/src/Log.c $(srcdir)/src/PubDataOps.c \
	$(srcdir)/src/SmartCard.c $(srcdir)/src/Util.c $(srcdir)/src/ExternalPinUI.c $(srcdir)/src/res/beidmdrv@LIBRARY_BITNESS@.rc
libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@_la_LDFLAGS = -no-undefined  -lwinscard@LIBRARY_BITNESS@ -lcrypt32@LIBRARY_BITNESS@ \
	-lole32 -lcomctl32@LIBRARY_BITNESS@ -L. \
	-export-symbols "$(srcdir)/src/inc/beidmdrv.exports" \
	-avoid-version 
libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@_la_CPPFLAGS = -I$(srcdir)/src/inc

libwinscard@LIBRARY_BITNESS@.a: $(srcdir)/src/imports/winscard-@LIBRARY_BITNESS@.def
	@DLLTOOL@ --kill-at --output-lib $@ --input-def $< --dllname=winscard.dll
	
libcrypt32@LIBRARY_BITNESS@.a: $(srcdir)/src/imports/crypt32-@LIBRARY_BITNESS@.def
	@DLLTOOL@ --kill-at --output-lib $@ --input-def $< --dllname=crypt32.dll

libcomctl32@LIBRARY_BITNESS@.a: $(srcdir)/src/imports/comctl32-@LIBRARY_BITNESS@.def
	@DLLTOOL@ --kill-at --output-lib $@ --input-def $< --dllname=comctl32.dll
	
.rc.lo:
	$(LIBTOOL) --mode=compile --tag=RC $(RC) -i "$<" -o "$@"
