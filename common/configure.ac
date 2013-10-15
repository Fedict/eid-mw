AC_PREREQ([2.61])
AC_INIT([libbeidcommon],[4.0.6],[servicedesk@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/mwexception.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

LT_INIT
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

###########################################################################
####### read user-specific dependecy requests from --with directives ######
###########################################################################

AC_MSG_CHECKING([user-specified pscs-lite installation])
AC_ARG_WITH([pcsclite],
AC_HELP_STRING([--with-pcsclite=DIR],[specific pcsc-lite install location]),
  [],
  [with_pcsclite=no])
AC_MSG_RESULT([$with_pcsclite])

AS_VAR_IF([with_pcsclite],[no],
[
	PKG_CHECK_MODULES([PCSC],[libpcsclite >= 1.4.4],
   	[
        CPPFLAGS="$PCSC_CFLAGS $CPPFLAGS"
        LDFLAGS="$PCSC_LIBS $CPPFLAGS"
	])
])

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
