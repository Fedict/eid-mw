AC_PREREQ([2.65])
AC_INIT([libbeiddialogs], [0.0.1], [support@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/langutil.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
AC_GNU_SOURCE
LT_INIT([win32-dll])

AC_LIBTOOL_WIN32_DLL

# Checks for programs (1)
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O

AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

AM_GNU_GETTEXT_VERSION([0.18])
AM_GNU_GETTEXT

AC_SUBST(LOCALEDIR)

############################################################################
# Test for platform and set corresponding AM_CONDITIONALs for Makefile.am  #
############################################################################

linux="no"
osx="no"
win32="no"
qt="no"

AS_CASE([$host],
  [*-*-linux*],[linux=true gtk=true],
  [*-*-darwin*],[osx=true qt=true],
  [*-mingw*|*-winnt*],[win32=true])

AM_CONDITIONAL(LINUX,test x$linux = xtrue)
AM_CONDITIONAL(OSX,test x$osx = xtrue)
AM_CONDITIONAL(WIN32,test x$win32 = xtrue)
AM_CONDITIONAL(QT,test x$qt = xtrue)
AM_CONDITIONAL(GTK,test x$gtk = xtrue)

###########################################################################
####### read user-specific dependecy requests from --with directives ######
###########################################################################

# if qt dialogs, enable checking for Qt
AS_VAR_IF([qt],[true],
[
	AC_MSG_CHECKING([user-specified Qt installation])
	AC_ARG_WITH([pcsclite],
	  AC_HELP_STRING([--with-qt=DIR],[specific Qt install location]),
	  [],
	  [with_qt=no])
	AC_MSG_RESULT([$with_qt])

	AS_VAR_IF([with_qt],[no],
	[
		PKG_CHECK_MODULES([QTGUI], [QtGui >= 4.5.0],
		[
			CPPFLAGS="$QTGUI_CFLAGS $CPPFLAGS"
			LDFLAGS="$QTGUI_LIBS $CPPFLAGS"
		])
	],
	[
	  AS_VAR_IF([win32],[no],[
	    CPPFLAGS="-I$with_qt/include $CPPFLAGS"
	    LDFLAGS="-L$with_qt/lib $LDFLAGS"
	  ],[
	    AC_MSG_WARN([--with-qt specified on MS Windows host, ignoring])
	  ])
	])
])

# if gtk dialogs, enable checking for GTK2
AS_VAR_IF([gtk],[true],
[

	AC_MSG_CHECKING([user-specified GTK installation])
	AC_ARG_WITH([gtk],
	  AC_HELP_STRING([--with-gtk=DIR],[specific GTK install location]),
	  [],
	  [with_gtk=no])
	AC_MSG_RESULT([$with_gtk])

	AS_VAR_IF([with_gtk],[no],
	[
    	PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.10.0],
		[
			CPPFLAGS="$GTK_CFLAGS $CPPFLAGS"
            LDFLAGS="$GTK_LIBS $CPPFLAGS"
 		],
		[
        	AC_MSG_RESULT(no)
        	AC_MSG_ERROR([You must have GTK+ 2.10.0 or newer development headers installed.])
		])
	],
	[
		AS_VAR_IF([linux],[true],
		[
	    	CPPFLAGS="-I$with_gtk/include $CPPFLAGS"
	    	LDFLAGS="-L$with_gtk/lib $LDFLAGS"
	  	],
		[
	    	AC_MSG_WARN([--with-gtk specified on non-GNU/Linux host, ignoring])
	  	])
	])
])


##################################################################
####### check for Qt build utilities, libraries and headers ######
##################################################################

AS_VAR_IF([qt],[true],
[
	AC_CHECK_PROGS(RCC,[rcc],[missing])
	AC_CHECK_PROGS(UIC,[uic-qt4 uic],[missing])
	AC_CHECK_PROGS(MOC,[moc-qt4 moc],[missing])
	AS_VAR_IF([RCC],[missing],AC_MSG_ERROR([Qt4 Resource Compiler (rcc) is required.]),AC_SUBST(RCC))
	AS_VAR_IF([UIC],[missing],AC_MSG_ERROR([Qt4 User Interface Compiler (uic or uic-qt4) is required.]),AC_SUBST(UIC))
	AS_VAR_IF([MOC],[missing],AC_MSG_ERROR([Qt4 Meta Object Compiler (moc or moc-qt4) is required.]),AC_SUBST(MOC))
])

########### libraries ##################
AS_VAR_IF([qt],[true],  [AC_CHECK_LIB([ssl],[SSL_get_version],[],AC_MSG_ERROR([OpenSSL Library is required.]))])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])
AS_VAR_IF([qt],[true],[AC_CHECK_HEADER([openssl/opensslconf.h],[],AC_MSG_ERROR([OpenSSL headers are required.]))])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([strerror vasprintf])

AC_CONFIG_FILES([Makefile intl/Makefile po/Makefile.in])
AC_OUTPUT
