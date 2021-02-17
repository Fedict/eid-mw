AC_PREREQ([2.61])
AC_INIT([eid-mw],
  m4_esyscmd_s(scripts/build-aux/genver.sh),
  [servicedesk@fedict.be],,
  [http://eid.belgium.be])

AC_CONFIG_AUX_DIR([scripts/build-aux])
AC_CONFIG_SRCDIR([cardcomm])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([scripts/m4])
AC_CANONICAL_HOST
AM_SILENT_RULES([yes])

LT_INIT
AM_INIT_AUTOMAKE([subdir-objects foreign tar-pax dist-zip serial-tests])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11()
AC_PROG_CPP
AC_PROG_CC_C99
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MKDIR_P
PKG_PROG_PKG_CONFIG

AC_CHECK_PROGS(PLANTUML, plantuml)
AM_CONDITIONAL([PLANTUML], [test x$PLANTUML != x])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h float.h limits.h netinet/in.h stdlib.h string.h sys/file.h sys/ioctl.h sys/time.h unistd.h malloc.h memory.h sys/timeb.h termios.h])

AM_GNU_GETTEXT([external])

AC_ARG_ENABLE([pinentry],AS_HELP_STRING([--disable-pinentry],[When enabled, uses pinentry for dialogs, rather than home-grown dialog boxes (requires libassuan)]),[assuan=$enableval],[assuan=detect])
if test x"$assuan" != x"no"
then
	AM_PATH_LIBASSUAN(2,[ASSUAN=y],[ASSUAN=n])
fi

if test x"$assuan" = x"yes" -a x$ASSUAN = x"n"
then
	AC_MSG_ERROR([assuan-based dialogs requested, but libassuan could not be found])
fi

AM_CONDITIONAL(ASSUAN,[test x"$ASSUAN" = x"y"])
###### pcsc-lite ##########
PKG_CHECK_MODULES([PCSC],[libpcsclite >= 1.4.4],
[
CPPFLAGS="$PCSC_CFLAGS $CPPFLAGS"
LDFLAGS="$PCSC_LIBS $CPPFLAGS"
])

AC_ARG_ENABLE([test-fuzzer],AS_HELP_STRING([--enable-test-fuzzer],[Build a version of eid-mw instrumented for fuzzing (not meant for production use!)]),[fuzzer=$enableval],[fuzzer=no])
AC_MSG_CHECKING([whether to build for fuzzing])
case $fuzzer in
	yes|libfuzz)
		case $CC in
			*clang*)
				AC_MSG_RESULT([yes, with libfuzz])
				AC_SUBST(FUZZING,["-fsanitize=fuzzer-no-link,address -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION"])
				;;
			*)
				AC_MSG_ERROR([Fuzzing with libfuzz only works with clang. Please set CC to clang version 6.0 or higher])
				;;
		esac
		case $CXX in
			*clang++*)
				;;
			*)
				AC_MSG_ERROR([Fuzzing with libfuzz only works with clang. Please set CXX to clang version 6.0 or higher])
				;;
		esac
	;;
	afl)
		AC_MSG_RESULT([yes, with AFL])
		CC=afl-$CC
		CXX=afl-$CXX
		AC_SUBST(FUZZING,["-DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION"])
	;;
	build)
		AC_MSG_RESULT([kinda, build only])
		AC_SUBST(FUZZING, ["-DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION"])
	;;
	*)
		fuzzer=no
		AC_MSG_RESULT([no])
	;;
esac
AM_CONDITIONAL([FUZZING], [test x$fuzzer != xno])
AM_CONDITIONAL([FUZZ_AFL], [test x$fuzzer = xafl])

AC_ARG_WITH([gtkvers],
	[AS_HELP_STRING([--with-gtkvers],[select GTK version to use [default: 3 if available, falls back to 4 if not; note that GTK4 support is incomplete]. --without-gtkvers disables GTK altogether [note: this implies --disable-dialogs].])],
	[gtkvers=$withval],[gtkvers=detect])
have_gtk=no
AC_MSG_CHECKING([for wanted GTK version])
AC_MSG_RESULT($gtkvers)
if test "$gtkvers" = "detect" -o "$gtkvers" = 3
then
	AC_MSG_CHECKING([for gtk3])
	PKG_CHECK_MODULES(GTK, [gtk+-3.0], [have_gtk=3], [have_gtk=no])
fi
if test "$have_gtk" = "no"
then
	if test "$gtkvers" = "detect" -o "$gtkvers" = 4
	then
		AC_MSG_CHECKING([for gtk4])
		PKG_CHECK_MODULES(GTK, [gtk4], [have_gtk=4],[have_gtk=no])
	fi
fi
if test "$have_gtk" = "no"
then
	if test "$gtkvers" = 2
	then
		AC_MSG_ERROR([GTK 2 is no longer supported, sorry])
	fi
fi

AC_DEFINE_UNQUOTED([HAVE_GTK], [$have_gtk], [Set to major version of libgtk])

if test "$have_gtk" = "no"
then
	if test "$gtkvers" != "no"
	then
		AC_MSG_ERROR([At least one version of GTK is recommended. If you do not wish the eID viewer or the PIN dialogs, please use --without-gtkvers.])
	fi
fi

AM_CONDITIONAL(GTK, [test "$have_gtk" != "no"])
if test "$have_gtk" != "no"
then
	PKG_CHECK_MODULES([XML2], [libxml-2.0])
	PKG_CHECK_MODULES([libproxy], [libproxy-1.0])
fi

###########################################################################
####### read user-specific requests from --enable directives         ######
###########################################################################
AC_MSG_CHECKING(whether to enable dialogs)
default="yes"
if test "$have_gtk" = "no"
then
	default=no
fi
NO_DIALOGS=
AC_ARG_ENABLE(dialogs, AS_HELP_STRING([--enable-dialogs],[Turn off dialogs. default: on, unless gtk has been disabled]),, enable_dialogs=$default)
if test "x$enable_dialogs" = "xno"; then
	AC_MSG_RESULT(no)
else
	if test $have_gtk = no; then
		AC_MSG_ERROR([You cannot combine --without-gtkvers and --enable-dialogs, as the dialogs require GTK])
	fi
	AC_MSG_RESULT(yes)
fi
AM_CONDITIONAL([NO_DIALOGS],[test x$enable_dialogs = xno])

PKG_CHECK_MODULES(GIO, [gio-2.0 >= 2.30],[HAVE_GIO=1],[HAVE_GIO=0])
AC_DEFINE_UNQUOTED([HAVE_GIO], [$HAVE_GIO], [Define to 1 if you have a version of libgio >= 2.30])
if test "$HAVE_GIO" -gt 0; then
GLIB_GSETTINGS
fi
AM_CONDITIONAL(HAVE_GIO, [test "$HAVE_GIO" = 1])

AC_DEFINE(PKCS11_V2_20,[],[]))

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_INT32_T
AC_TYPE_UINT8_T
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
saved_CFLAGS=$CFLAGS
export CFLAGS="$CFLAGS -Wno-error"
AC_CHECK_FUNCS([gethostbyname socket strerror memchr memset mkdir select strchr strdup strerror strncasecmp strrchr strstr strtol strtoul vasprintf],,[AC_MSG_ERROR([missing required function])])
CFLAGS=$saved_CFLAGS

# we only need libjpeg for the test suite, so don't worry if it's not available...
AC_CHECK_LIB(jpeg, jpeg_mem_src, [HAVE_JPEGLIB=1; JPEG_LIBS='-ljpeg'], [HAVE_JPEGLIB=0])
AM_CONDITIONAL([JPEG], [test x$HAVE_JPEGLIB = x1])
AC_SUBST([JPEG_LIBS])
AC_DEFINE_UNQUOTED([HAVE_JPEGLIB], $HAVE_JPEGLIB, [Define to 1 if you have libjpeg installed])

PKG_CHECK_MODULES(SSL, "openssl", [HAVE_OPENSSL=1])
PKG_CHECK_VAR(SSL_PREFIX, "openssl", "exec_prefix",, AC_MSG_ERROR([Could not find exec_prefix for openssl]))
AC_DEFINE_UNQUOTED([HAVE_OPENSSL], $HAVE_OPENSSL, [Define to 1 if you have OpenSSL installed])
AC_SUBST(SSL_PREFIX)

PKG_CHECK_MODULES(CURL, "libcurl", [HAVE_CURL=1])
MAINVERSION=$(echo $PACKAGE_VERSION|sed -e 's/-.*$//')
AC_DEFINE_UNQUOTED([MAINVERSION], $MAINVERSION, [do not change this])
AC_SUBST(MAINVERSION)
CRYPTOKIMAJORVERS=$(echo $MAINVERSION|cut -d'.' -f1|sed -e 's/v//')
CRYPTOKIMINORVERS=$(echo $MAINVERSION|cut -d'.' -f2)
AS_IF([test -z "$CRYPTOKIMINORVERS"],[CRYPTOKIMINORVERS=0])
AC_DEFINE_UNQUOTED([CRYPTOKIMAJORVERS], $CRYPTOKIMAJORVERS, [major version of PKCS#11 library])
AC_DEFINE_UNQUOTED([CRYPTOKIMINORVERS], $CRYPTOKIMINORVERS, [minor version of PKCS#11 library])

# need dlopen() for the pincode plugin thing
save_LIBS="$LIBS"
LIBS=""
AC_SEARCH_LIBS([dlopen], [dl],, [AC_MSG_ERROR([I need a dlopen() implementation])])
DL_LIBS="$LIBS"
LIBS="$save_LIBS"
AC_SUBST(DL_LIBS)

p11kitsodir=""
p11kitcfdir=""
PKG_CHECK_MODULES(p11kit, "p11-kit-1", [HAVE_P11KIT=yes], [HAVE_P11KIT=no])
AC_ARG_ENABLE(p11kit, AS_HELP_STRING([--disable-p11kit], [disregard detection result of p11-kit, and do not install support files]), [if test $enableval = no; then HAVE_P11KIT=no; fi])
if test "$HAVE_P11KIT" = yes
then
	PKG_CHECK_VAR([p11kitsodir], "p11-kit-1", "p11_module_path",, [HAVE_P11KIT=0])
	PKG_CHECK_VAR([p11kit_libdir], "p11-kit-1", "libdir",, [HAVE_P11_KIT=0])
	if test ! -z "$p11kit_libdir"; then
		p11kitsodir=$(echo $p11kitsodir|sed -e "s,$p11kit_libdir,\${libdir},g")
	fi
	PKG_CHECK_VAR([p11kitcfdir], "p11-kit-1", "p11_system_config_modules",, [HAVE_P11KIT=0])
	PKG_CHECK_VAR([p11kit_datarootdir], "p11-kit-1", "datarootdir",, [HAVE_P11_KIT=0])
	if test ! -z "$p11kit_datarootdir"; then
		p11kitcfdir=$(echo $p11kitcfdir|sed -e "s,$p11kit_datarootdir,\${datarootdir},g")
	fi
fi
AM_CONDITIONAL([P11KIT], [test "$HAVE_P11KIT" = yes])

pkgconfigdir="\$(libdir)/pkgconfig"
pkcs11hincludedir="\$(includedir)/beid/rsaref220"
AC_SUBST([pkgconfigdir])
AC_SUBST([pkcs11hincludedir])
AC_SUBST([p11kitsodir])
AC_SUBST([p11kitcfdir])

AC_CONFIG_FILES([Makefile
		 cardcomm/pkcs11/src/Makefile
		 cardcomm/pkcs11/src/cardlayer/uml/Makefile
		 cardcomm/pkcs11/src/libbeidpkcs11.pc
		 cardcomm/pkcs11/src/beidpkcs11.json
		 cardcomm/pkcs11/src/beidp11kit.json
		 cardcomm/pkcs11/src/dialogs/dialogsgtk/po/Makefile.in
		 doc/sdk/include/v240/Makefile
		 tests/unit/Makefile
		 tests/fuzz/Makefile
		 plugins_tools/util/Makefile
		 plugins_tools/aboutmw/gtk/Makefile
		 plugins_tools/aboutmw/gtk/po/Makefile.in
		 plugins_tools/chrome_pkcs11/Makefile
		 plugins_tools/eid-viewer/Makefile
		 plugins_tools/eid-viewer/test/Makefile
		 plugins_tools/eid-viewer/gtk/po/Makefile.in
		 plugins_tools/eid-viewer/gtk/eid-viewer.desktop.sh
		 plugins_tools/eid-viewer/uml/Makefile
		 rpm/eid-mw.spec])
AC_CONFIG_SUBDIRS([plugins_tools/xpi])

AC_OUTPUT
