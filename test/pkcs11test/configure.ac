AC_PREREQ([2.65])
AC_INIT([pkcs11tests], [0.0.1], [servicedesk@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/open-close-session.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
LT_INIT()

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

linux="no"
osx="no"
win32="no"

case "${host}" in

    *-*-linux*)
        linux=true
        ;;  

    *-*-darwin*)
        osx=true
        ;;

    *-mingw*|*-winnt*)
        win32=true
        ;;
esac

AM_CONDITIONAL(LINUX,test x$linux = xtrue)
AM_CONDITIONAL(OSX,test x$osx = xtrue)
AM_CONDITIONAL(WIN32,test x$win32 = xtrue)


########### libraries ##################
# on Windows, check for msvcr
if test "${win32}" = "yes"; then
    AC_CHECK_LIB([msvcr], [main])
fi

# on GNU/Linux and OSX, enable --with-opensc-headers
if test "${win32}" = "no"; then
    AC_MSG_CHECKING([opensc headers])
    AC_ARG_WITH(opensc-headers, AC_HELP_STRING([--with-opensc-headers=DIR],
                    [opensc headers install location]))
    OPENSC_INCS="-I$with_opensc_headers/"
    AC_MSG_RESULT([$with_opensc_headers])
fi

AC_SUBST([OPENSC_INCS])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset mkdir strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
