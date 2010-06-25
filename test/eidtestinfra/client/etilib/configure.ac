AC_PREREQ([2.65])
AC_INIT([etilib], [3.99.0], [servicedesk@fedict.be])
AC_CONFIG_SRCDIR([src/virtualcardaccess.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

LT_INIT([win32-dll])
LT_PREREQ([2.2.7c])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

############################################################################
# Test for platform and set corresponding AM_CONDITIONALs for Makefile.am  #
############################################################################

linux="no"
osx="no"
win32="no"

AS_CASE([$host],
  [*-*-linux*],[linux=true],
  [*-*-darwin*],[osx=true],
  [*-mingw*|*-winnt*],[win32=true])

AM_CONDITIONAL([LINUX],[test x$linux = xtrue])
AM_CONDITIONAL([OSX],[test x$osx = xtrue])
AM_CONDITIONAL([WIN32],[test x$win32 = xtrue])

# Checks for libraries
AS_VAR_IF([win32],[true],[AC_CHECK_LIB([msvcrt],[_getpid])])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([strchr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

