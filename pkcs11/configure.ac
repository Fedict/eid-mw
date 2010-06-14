AC_PREREQ([2.65])
AC_INIT([libbeidpkcs11], [0.0.1], [servicedesk@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/beid_p11.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
LT_INIT([win32-dll])

AC_LIBTOOL_WIN32_DLL

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
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
		PKG_CHECK_MODULES([PCSC], [libpcsclite >= 1.5.5],
	    [
	        CPPFLAGS="$PCSC_CFLAGS $CPPFLAGS"
	        LDFLAGS="$PCSC_LIBS $CPPFLAGS"
	    ])
	],
	[
	    CPPFLAGS="-I$with_pcsclite/include $CPPFLAGS"
	    LDFLAGS="-L$with_pcsclite/lib $LDFLAGS"
  	])
])

########### libraries ##################
AS_VAR_IF([win32],[true],[AC_CHECK_LIB([msvcrt],[_getpid])])
AS_VAR_IF([win32],[no],  [AC_CHECK_LIB([pcsclite],[SCardConnect])])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])
AS_VAR_IF([osx],[no],[AC_CHECK_HEADER([winscard.h],[],AC_MSG_ERROR([PCSC headers are required.])]))

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
