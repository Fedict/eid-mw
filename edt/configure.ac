AC_PREREQ([2.65])
AC_INIT([edt], [3.5.3], [servicedesk@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/secure_helper.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST
LT_INIT

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_SED

# Checks for libraries.

###### eid-mw ##########

AC_MSG_CHECKING([eid-mw source directory])
AC_ARG_WITH(eid-mw, AC_HELP_STRING([--with-eid-mw=DIR],
                [eid-mw source directory location]))
CPPFLAGS="-I$with_eid_mw/common -I$with_eid_mw/tokend -I$with_eid_mw/pkcs11 -I$with_eid_mw/csp -I$with_eid_mw/eidlib -I$with_eid_mw/applayer -I$with_eid_mw/minidriver -I$with_eid_mw/cardlayer $CPPFLAGS"
LDFLAGS="-L$with_eid_mw/_binaries35/release $LDFLAGS"
AC_MSG_RESULT([$with_eid_mw])

########################################################

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_SUBDIRS([edtgui])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT





AC_PREREQ([2.63])
AC_INIT([libbeidpkcs11], m4_esyscmd([echo -n 3.99.M1-`svnversion -n` | tr -d '\n' | tr -d '\r']), [servicedesk@fedict.be])
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
                # PKG_CHECK_MODULES is possibly undefined
                # as we do not use pkgconfig on Windows and Mac
                m4_ifdef([PKG_CHECK_MODULES],
                [
			PKG_CHECK_MODULES([PCSC], [libpcsclite >= 1.5.3],
	    		[
			        CPPFLAGS="$PCSC_CFLAGS $CPPFLAGS"
	        		LDFLAGS="$PCSC_LIBS $CPPFLAGS"
			])
		])
	],
	[
	    CPPFLAGS="-I$with_pcsclite/include $CPPFLAGS"
	    LDFLAGS="-L$with_pcsclite/lib $LDFLAGS"
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
