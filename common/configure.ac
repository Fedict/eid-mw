AC_PREREQ([2.65])
AC_INIT([libbeidcommon], m4_esyscmd([echo -n 3.99.M1-`svnversion -n` | tr -d '\n' | tr -d '\r']), [servicedesk@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/mwexception.h])
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
PKG_PROG_PKG_CONFIG

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

# Checks for headers
AC_CHECK_HEADERS([fcntl.h float.h limits.h netinet/in.h stdlib.h string.h sys/file.h sys/ioctl.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([gethostbyname memset mkdir socket strcasecmp strerror vasprintf strcpy_s wcscpy_s])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
