AC_PREREQ([2.61])
AC_INIT([xpi], [4.0.6], [servicedesk@fedict.be])
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

AC_SUBST([ZIP])
AC_SUBST([DEBUG])
AC_SUBST([DEBUG_POSTFIX])
AC_SUBST([mozext])

# Checks for library functions.

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_OUTPUT
