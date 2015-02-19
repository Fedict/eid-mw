AC_PREREQ([2.61])
AC_INIT([eid-mw],
  m4_esyscmd_s(echo "4.1.3-"$(git describe --dirty|sed -e 's/-/./g')),
  [servicedesk@fedict.be],,
  [http://eid.belgium.be])

AC_CONFIG_AUX_DIR([scripts/build-aux])
AC_CONFIG_SRCDIR([cardcomm])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([scripts/m4])
AC_CANONICAL_HOST

LT_INIT
AM_INIT_AUTOMAKE([subdir-objects foreign tar-pax dist-zip serial-tests])

# Checks for programs.
AC_PROG_CXX([g++])
AC_PROG_CPP
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S

AS_VAR_IF([GXX],[yes],[],AC_MSG_ERROR([GNU C++ compiler (g++) is required.]))

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h float.h limits.h netinet/in.h stdlib.h string.h sys/file.h sys/ioctl.h sys/time.h unistd.h malloc.h memory.h sys/timeb.h termios.h])

AM_GNU_GETTEXT([external])

###### pcsc-lite ##########
AC_MSG_CHECKING([custom pcsc-lite installation])
AC_ARG_WITH(pcsclite, AC_HELP_STRING([--with-pcsclite=DIR],
                    [pcsc-lite install location]),
		    [],
		    [with_pcsclite=default])
AC_MSG_RESULT([$with_pcsclite])

AS_VAR_IF([with_pcsclite],[default],
[
	PKG_CHECK_MODULES([PCSC],[libpcsclite >= 1.4.4],
   	[
        CPPFLAGS="$PCSC_CFLAGS $CPPFLAGS"
        LDFLAGS="$PCSC_LIBS $CPPFLAGS"
	])
])

AC_MSG_CHECKING([custom gtk installation])
AC_ARG_WITH(gtk, AC_HELP_STRING([--with-gtk=DIR],
		[gtk install location]),
		[],
		[with_gtk=default])

AS_VAR_IF([with_gtk],[default],
[
	PKG_CHECK_MODULES([GTK],[gtk+-2.0 >= 2.10.0],,
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([you must have GTK+ 2.10.0 or newer development headers installed.]))
])

###########################################################################
####### read user-specific requests from --enable directives         ######
###########################################################################
AC_MSG_CHECKING(whether to enable dialogs)
default="yes"
NO_DIALOGS=
AC_ARG_ENABLE(dialogs, [  --enable-dialogs=[yes/no] turn off dialogs
                               [default=$default]],, enable_dialogs=$default)
if test "x$enable_dialogs" = "xno"; then
AC_MSG_RESULT(no)
else
AC_MSG_RESULT(yes)
fi
AM_CONDITIONAL([NO_DIALOGS],[test x$enable_dialogs = xno])

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
AC_CHECK_FUNCS([gethostbyname socket strerror ftime memchr memset mkdir select strchr strdup strerror strncasecmp strrchr strstr strtol strtoul vasprintf])

# we only need libjpeg for the test suite, so don't worry if it's not available...
AC_CHECK_LIB(jpeg, jpeg_mem_src, [HAVE_JPEGLIB=1; JPEG_LIBS='-ljpeg'], [HAVE_JPEGLIB=0])
AM_CONDITIONAL([JPEG], [test x$HAVE_JPEGLIB = x1])
AC_SUBST([JPEG_LIBS])
AC_DEFINE_UNQUOTED([HAVE_JPEGLIB], $HAVE_JPEGLIB, [Define to 1 if you have libjpeg installed])

# we only need libssl for the test suite, so don't worry if it's not available...
PKG_CHECK_MODULES(SSL, "openssl", [HAVE_OPENSSL=1], [HAVE_OPENSSL=0])
AC_DEFINE_UNQUOTED([HAVE_OPENSSL], $HAVE_OPENSSL, [Define to 1 if you have OpenSSL installed])

AC_CONFIG_FILES([Makefile cardcomm/pkcs11/src/Makefile tests/unit/Makefile plugins_tools/aboutmw/gtk/Makefile plugins_tools/aboutmw/gtk/about-eid-mw.desktop.sh plugins_tools/aboutmw/gtk/po/Makefile.in])
AC_CONFIG_SUBDIRS([plugins_tools/xpi])

AC_OUTPUT
