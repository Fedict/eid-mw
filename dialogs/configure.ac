AC_PREREQ([2.61])
AC_INIT([libbeiddialogs],[4.0.6],[support@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/langutil.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST

# Checks for programs (1)
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

###########################################################################
####### read user-specific dependecy requests from --with directives ######
###########################################################################

AC_ARG_ENABLE(debug,
[  --enable-debug    Turn on debugging],
[case "${enableval}" in
  yes) debug=true ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
esac],[debug=false])
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)

AC_MSG_CHECKING([user-specified GTK installation])
AC_ARG_WITH([gtk],
  AC_HELP_STRING([--with-gtk=DIR],[specific GTK install location]),
  [],
  [with_gtk=no])
AC_MSG_RESULT([$with_gtk])

AS_VAR_IF([with_gtk],[no],
[
	PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.10.0],
    [
     	CPPFLAGS="$GTK_CFLAGS $CPPFLAGS"
        LDFLAGS="$GTK_LIBS $CPPFLAGS"
    ],
   	AC_MSG_RESULT([no])
   	AC_MSG_ERROR([you must have GTK+ 2.10.0 or newer development headers installed.])
    )
])


# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([strerror vasprintf])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
