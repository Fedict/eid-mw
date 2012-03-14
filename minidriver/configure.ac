AC_PREREQ([2.61])
AC_INIT([beidmdrv],[4.0.2] [servicedesk@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/CardAndContProp.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST

# Checks for programs.
LT_INIT([win32-dll])
AC_PROG_CXX
AC_PROG_CC
LT_PROG_RC 

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([malloc.h stdlib.h string.h winscard.h])

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


# Check for 32-64 bit (ty opensc)
AC_CHECK_SIZEOF(void *)
if test "${ac_cv_sizeof_void_p}" = 8; then
	LIBRARY_BITNESS="64"
else
	LIBRARY_BITNESS="32"
fi

AC_SUBST([CFLAGS])
AC_SUBST([DEBUG])
AC_SUBST([DEBUG_POSTFIX])
AC_SUBST([LIBRARY_BITNESS])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
