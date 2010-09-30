AC_PREREQ([2.63])
AC_INIT([sccertprop], [4.0.0], [servicedesk@fedict.be])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

LT_INIT
LT_PREREQ([2.2.7c],
      [LT_PREREQ([2.2.6])],
      [AC_MSG_WARN([Libtool Version 2.2.7c is required])]
      )

AM_INIT_AUTOMAKE([-Wall -Werror])

# Checks for programs.
AC_PROG_CXX

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
	         CXXFLAGS="$CXXFLAGS -g -D_DEBUG -Wall -O0"
	         DEBUG=-g
	         DEBUG_POSTFIX=_debug;
AC_MSG_RESULT(yes)
else
AC_MSG_RESULT(no)
fi

AC_SUBST([CXXFLAGS])
AC_SUBST([DEBUG])
AC_SUBST([DEBUG_POSTFIX])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
