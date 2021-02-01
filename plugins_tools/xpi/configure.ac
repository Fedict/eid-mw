AC_PREREQ([2.61])
AC_INIT([xpi], [4.1.18], [servicedesk@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/content/belgiumeidpkcs11.js])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST

# Checks for programs.
# Check for zip
AC_PATH_PROG([ZIP], [zip])
AC_PROG_SED

# Checks for libraries.

dnl this macro is used to get the arguments supplied
dnl to the configure script (./configure --enable-debug)
dnl Check if we have enable debug support.
AC_MSG_CHECKING(whether to enable debugging)
default="no"
DEBUG=
DEBUG_POSTFIX=
AC_ARG_ENABLE(debug, [  --enable-debug=[no/yes] turn on debugging
	                       [default=$default]],, enable_debug=$default)
if test "x$enable_debug" = "xyes"; then
	         CFLAGS="$CFLAGS -g -D_DEBUG -Wall -O0"
	         DEBUG=-g
	         DEBUG_POSTFIX=_debug;
AC_MSG_RESULT(yes)
else
AC_MSG_RESULT(no)
fi

AC_ARG_WITH([mozext],
	  AS_HELP_STRING([--with-mozext=DIR],[Mozilla extension directory]),
	  [],
	  [with_mozext=no])

AS_IF(
	[test "x$with_mozext" = "xno"],
	[mozext]=[${datadir}/mozilla/extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}],
	[[mozext]=[$with_mozext]]
)

AC_ARG_ENABLE([webextension],
	AS_HELP_STRING([--enable-webextension],[Mozilla addon API style to use: enable for WebExtension, disable for older XPCOM add-on]),
	[],
	[enable_webextension=no])
AS_IF(
	[test "x$enable_webextension" = "xno"],
	[we]=[yes],
	[[we]=[$enable_webextension]]
)
AM_CONDITIONAL([WEBEXT],[test "$enable_webextension" = yes])

AC_ARG_ENABLE(signed, AS_HELP_STRING([--disable-signed], [Don't use the (pre-)signed version of the extension, instead build it from source. Required for Firefox >= 43 (default on)]),[],[enable_signed=yes])
AM_CONDITIONAL([SIGNED], [test "$enable_signed" = yes])

AC_SUBST([ZIP])
AC_SUBST([DEBUG])
AC_SUBST([DEBUG_POSTFIX])
AC_SUBST([mozext])

# Checks for library functions.

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	signed-build/Makefile
])
AC_OUTPUT
