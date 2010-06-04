AC_PREREQ([2.65])
AC_INIT([libbeidcommon], [0.0.1], [servicedesk@fedict.be])
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

# Checks for libraries.

# on GNU/Linux and OSX, enable --with-pcsclite

AC_MSG_CHECKING([pcsc-lite installation])
AC_ARG_WITH([pcsclite],
  AC_HELP_STRING([--with-pcsclite=DIR],[pcsc-lite install location]),
  [],
  [with_pcsclite=no])
AC_MSG_RESULT([$with_pcsclite])

AS_VAR_IF([with_pcsclite],[no],[],[
  AS_VAR_IF([win32],[no],[
    CPPFLAGS="-I$with_pcsclite/include $CPPFLAGS"
    LDFLAGS="-L$with_pscslite/lib $LDFLAGS"
  ],[
    AC_MSG_WARN([--with-pcsclite specified on MS Windows host, ignoring])
  ])
])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h float.h limits.h netinet/in.h stdlib.h string.h sys/file.h sys/ioctl.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

#AC_CHECK_TYPES([errno_t], [], [], [[#include <errno.h>]])

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
AC_CHECK_FUNCS([gethostbyname memset mkdir socket strcasecmp strerror vasprintf strcpy_s wcscpy_s])

AC_MSG_NOTICE([**** configured for ${host} ****])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
