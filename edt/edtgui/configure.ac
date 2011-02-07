AC_PREREQ([2.61])
AC_INIT([edtgui], [4.0.0], [servicedesk@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([qtsingleapplication.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

#  Checks for libraries.
###### eid-mw ##########

AC_MSG_CHECKING([eid-mw source directory])
AC_ARG_WITH(eid-mw, AC_HELP_STRING([--with-eid-mw=DIR],
                [eid-mw source directory location]))
CPPFLAGS="-I$with_eid_mw/common -I$with_eid_mw/tokend -I$with_eid_mw/pkcs11 -I$with_eid_mw/csp -I$with_eid_mw/eidlib -I$with_eid_mw/applayer -I$with_eid_mw/minidriver -I$with_eid_mw/cardlayer $CPPFLAGS"
LDFLAGS="-L$with_eid_mw/_binaries35/release $LDFLAGS"
AC_MSG_RESULT([$with_eid_mw])


###### QT includes and libs ##########

AC_MSG_CHECKING([Qt installation])
AC_ARG_WITH(qt, AC_HELP_STRING([--with-qt=DIR],
                [Qt install location]))
CPPFLAGS="-I$with_qt/include $CPPFLAGS"
LDFLAGS="-L$with_qt/lib $LDFLAGS"
AC_MSG_RESULT([$with_qt])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
