AC_PREREQ([2.61])
AC_INIT([libbeidpkcs11],[4.0.6], [servicedesk@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/beid_p11.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

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

###########################################################################
####### read user-specific requests from --enable directives         ######
###########################################################################
dnl this macro is used to get the arguments supplied
dnl to the configure script (./configure --enable-dialogs)
dnl Check if we have enable dialogs support.
AC_MSG_CHECKING(whether to enable dialogs)
default="yes"
AC_ARG_ENABLE(dialogs, [  --enable-dialogs=[yes/no] turn off dialogs
                               [default=$default]],, enable_dialogs=$default)
if test "x$enable_dialogs" = "xno"; then
AC_MSG_RESULT(no)
else
AC_MSG_RESULT(yes)
fi
AM_CONDITIONAL([NO_DIALOGS],[test x$enable_dialogs = xno])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
#AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset mkdir strstr vasprintf])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
