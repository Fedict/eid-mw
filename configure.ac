AC_PREREQ([2.63])
AC_INIT([eid-mw],[4.0.0],[servicedesk@fedict.be])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([common])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

LT_INIT
LT_PREREQ([2.2.7c],
      [LT_PREREQ([2.2.6])],
      [AC_MSG_WARN([Libtool Version 2.2.7c is required to build for Windows])]
      )


AM_INIT_AUTOMAKE([subdir-objects foreign tar-pax])

# Checks for programs.
AC_PROG_CXX([g++])
AC_PROG_CPP
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S

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

AS_VAR_IF([GXX],[yes],[],AC_MSG_ERROR([GNU C++ compiler (g++) is required.]))

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h float.h limits.h netinet/in.h stdlib.h string.h sys/file.h sys/ioctl.h sys/time.h unistd.h malloc.h memory.h sys/timeb.h])

###### QT includes and libs ##########
AC_MSG_CHECKING([Qt installation])
AC_ARG_WITH(qt, AC_HELP_STRING([--with-qt=DIR],
                [Qt install location]))
AC_MSG_RESULT([$with_qt])

###### pcsc-lite ##########
AC_MSG_CHECKING([pcsc-lite installation])
AC_ARG_WITH(pcsclite, AC_HELP_STRING([--with-pcsclite=DIR],
                    [pcsc-lite install location]))
AC_MSG_RESULT([$with_pcsclite])

###### openssl ##########
AC_MSG_CHECKING([openssl installation])
AC_ARG_WITH(openssl, AC_HELP_STRING([--with-openssl=DIR],
                    [openssl install location]))
AC_MSG_RESULT([$with_openssl])

# on Windows, check for msvcr
AS_VAR_IF([win32],[true],[AC_CHECK_LIB([msvcr],[getpid])])

###########################################################################
####### read user-specific requests from --enable directives         ######
###########################################################################
dnl this macro is used to get the arguments supplied
dnl to the configure script (./configure --enable-dialogs)
dnl Check if we have enable dialogs support.
AC_MSG_CHECKING(whether to enable dialogs)
default="yes"
NO_DIALOGS=
AC_ARG_ENABLE(dialogs, [  --enable-dialogs=[yes/no] turn off dialogs
                               [default=$default]],, enable_dialogs=$default)
if test "x$enable_dialogs" = "xno"; then
AC_MSG_RESULT(no)
else
AC_MSG_RESULT(yes)
fi
AM_CONDITIONAL([NO_DIALOGS],[test x$enable_dialogs = xno])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gethostbyname socket strerror ftime memchr memset mkdir select strchr strdup strerror strncasecmp strrchr strstr strtol strtoul vasprintf])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_SUBDIRS([common cardlayer pkcs11 xpi eid-viewer])

if test "x$enable_dialogs" = "xyes"; then
	AC_CONFIG_SUBDIRS([dialogs])
fi

if test "x$osx" = "xtrue" ; then
	AC_CONFIG_SUBDIRS([dmg])
fi

if test "x$win32" = "xtrue" ; then
	AC_CONFIG_SUBDIRS([minidriver sccertprop])
fi

AC_OUTPUT
