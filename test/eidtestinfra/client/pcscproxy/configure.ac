AC_PREREQ([2.61])
AC_INIT([pcscproxy], m4_esyscmd([echo -n 3.99.M1-`svnversion -n` | tr -d '\n' | tr -d '\r']), [servicedesk@fedict.be])
AC_CONFIG_SRCDIR([src/softcard.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

LT_INIT([win32-dll])

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

###########################################################################
####### read user-specific dependecy requests from --with directives ######
###########################################################################

AS_VAR_IF([linux],[true],
[
    AC_MSG_CHECKING([user-specified pscs-lite installation])
    AC_ARG_WITH([pcsclite],
    AC_HELP_STRING([--with-pcsclite=DIR],[specific pcsc-lite install location]),
      [],
      [with_pcsclite=no])
    AC_MSG_RESULT([$with_pcsclite])

    AS_VAR_IF([with_pcsclite],[no],
    [
        PKG_CHECK_MODULES([PCSC], [libpcsclite >= 1.4.102],
        [
            CPPFLAGS="$PCSC_CFLAGS $CPPFLAGS"
            LDFLAGS="$PCSC_LIBS $CPPFLAGS"
        ])
    ],
    [
      AS_VAR_IF([win32],[no],[
        CPPFLAGS="-I$with_pcsclite/include $CPPFLAGS"
        LDFLAGS="-L$with_pcsclite/lib $LDFLAGS"
      ],[
        AC_MSG_WARN([--with-pcsclite specified on MS Windows host, ignoring])
      ])
    ])
])


# Checks for libraries
AS_VAR_IF([win32],[true],[AC_CHECK_LIB([msvcrt],[_getpid])])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([memory.h stddef.h stdlib.h])
AS_VAR_IF([osx],[no],[AC_CHECK_HEADER([winscard.h],[],AC_MSG_ERROR([PCSC headers are required.])]))

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strchr strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
