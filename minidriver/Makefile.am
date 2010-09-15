ACLOCAL_AMFLAGS         = -I m4

# add dll to all-local to include test for dll creation
# libtool will not fail if it is unable to create a dll (eg because of missing libs)
all-local: .libs/libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@.dll
noinst_LIBRARIES = libwinscard@LIBRARY_BITNESS@.a libcrypt32@LIBRARY_BITNESS@.a libcomctl32@LIBRARY_BITNESS@.a 

lib_LTLIBRARIES = libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@.la
libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@_la_SOURCES = $(srcdir)/src/CardAndContProp.c $(srcdir)/src/CardCap.c $(srcdir)/src/CardPinOps.c \
	$(srcdir)/src/CryptoOps.c $(srcdir)/src/DllMain.c $(srcdir)/src/InitAndDeconstruct.c \
	$(srcdir)/src/KeyContainer.c $(srcdir)/src/ListMgr.c $(srcdir)/src/Log.c $(srcdir)/src/PubDataOps.c \
	$(srcdir)/src/SmartCard.c $(srcdir)/src/Util.c $(srcdir)/src/ExternalPinUI.c $(srcdir)/src/res/beidmdrv@LIBRARY_BITNESS@.rc

libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@_la_LDFLAGS = -no-undefined -avoid-version \
	--verbose -export-symbols "$(srcdir)/src/inc/beidmdrv.exports" 

libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@_la_LIBADD = -lwinscard@LIBRARY_BITNESS@ -lcrypt32@LIBRARY_BITNESS@ \
	-lole32 -lcomctl32@LIBRARY_BITNESS@ -L. 

libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@_la_CPPFLAGS = -I$(srcdir)/src/inc

libwinscard@LIBRARY_BITNESS@.a: $(srcdir)/src/imports/winscard-@LIBRARY_BITNESS@.def
	@DLLTOOL@ --kill-at --output-lib $@ --input-def $< --dllname=winscard.dll
	
libcrypt32@LIBRARY_BITNESS@.a: $(srcdir)/src/imports/crypt32-@LIBRARY_BITNESS@.def
	@DLLTOOL@ --kill-at --output-lib $@ --input-def $< --dllname=crypt32.dll

libcomctl32@LIBRARY_BITNESS@.a: $(srcdir)/src/imports/comctl32-@LIBRARY_BITNESS@.def
	@DLLTOOL@ --kill-at --output-lib $@ --input-def $< --dllname=comctl32.dll

# simple test if dll exists	
.libs/libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@.dll: libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@.la
	test -e $@
.rc.lo:
	$(LIBTOOL) --mode=compile --tag=RC $(RC) -i "$<" -o "$@"
