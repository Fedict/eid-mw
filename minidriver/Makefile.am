ACLOCAL_AMFLAGS         = -I m4

# add dll to all-local to include test for dll creation
# libtool will not fail if it is unable to create a dll (eg because of missing libs)
all-local: .libs/libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@.dll

lib_LTLIBRARIES = libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@.la
libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@_la_SOURCES = $(srcdir)/src/CardAndContProp.c $(srcdir)/src/CardCap.c $(srcdir)/src/CardPinOps.c \
	$(srcdir)/src/CryptoOps.c $(srcdir)/src/DllMain.c $(srcdir)/src/InitAndDeconstruct.c \
	$(srcdir)/src/KeyContainer.c $(srcdir)/src/Log.c $(srcdir)/src/PubDataOps.c \
	$(srcdir)/src/SmartCard.c $(srcdir)/src/Util.c $(srcdir)/src/ExternalPinUI.c $(srcdir)/src/res/beidmdrv@LIBRARY_BITNESS@.rc

libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@_la_LDFLAGS = -no-undefined -avoid-version \
	--verbose -export-symbols "$(srcdir)/src/res/beidmdrv.def" 

# libcomctl32v6.a is self-built. See dev_env/windows/readme.txt for more information about how to create this lib
libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@_la_LIBADD = -lwinscard -lcrypt32 -lole32 -lcomctl32v6 -L. 

libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@_la_CPPFLAGS = -I$(srcdir)/src/inc

# simple test if dll exists	
.libs/libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@.dll: libbeidmdrv@LIBRARY_BITNESS@@DEBUG_POSTFIX@.la
	test -e $@
.rc.lo:
	$(LIBTOOL) --mode=compile --tag=RC $(RC) -i "$<" -o "$@"
