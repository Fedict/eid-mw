AC_PREREQ([2.65])
AC_INIT([libbeiddialogs], [0.0.1], [support@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/langutil.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
LT_INIT([win32-dll])

AC_LIBTOOL_WIN32_DLL

# Checks for programs (1)
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

############################################################################
# Test for platform and set corresponding AM_CONDITIONALs for Makefile.am  #
############################################################################

linux="no"
osx="no"
win32="no"
qt="no"

AS_CASE([$host],
  [*-*-linux*],[linux=true qt=true],
  [*-*-darwin*],[osx=true qt=true],
  [*-mingw*|*-winnt*],[win32=true])

AM_CONDITIONAL(LINUX,test x$linux = xtrue)
AM_CONDITIONAL(OSX,test x$osx = xtrue)
AM_CONDITIONAL(WIN32,test x$win32 = xtrue)
AM_CONDITIONAL(QT,test x$qt = xtrue)

AS_VAR_IF([qt],[true],[PKG_PROG_PKG_CONFIG])

###########################################################################
####### read user-specific dependecy requests from --with directives ######
###########################################################################

AS_VAR_IF([linux],[true],
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

	AC_MSG_CHECKING([user-specified OpenSSL installation])
	AC_ARG_WITH([openssl],
	  AC_HELP_STRING([--with-openssl=DIR],[specific OpenSSL install location]),
	  [],
	  [with_openssl=no])
	AC_MSG_RESULT([$with_openssl])

	AS_VAR_IF([with_openssl],[no],
	[
    	PKG_CHECK_MODULES([OPENSSL], [openssl >= 1.0.0],
    	[
        	CPPFLAGS="$OPENSSL_CFLAGS $CPPFLAGS"
        	LDFLAGS="$OPENSSL_LIBS $CPPFLAGS"
    	])
	],
	[
    	CPPFLAGS="-I$with_openssl/include $CPPFLAGS"
    	LDFLAGS="-L$with_openssl/lib $LDFLAGS"
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

	AS_VAR_IF([linux],[true],
	[
		AC_CHECK_LIB(	 [QtCore],[_ZNK4QUrl4hostEv],[], AC_MSG_ERROR([Qt4 Core Library (libQtCore) is required.]))
		AC_CHECK_LIB(	 [QtGui], [_ZN7QPixmap12defaultDepthEv],[],AC_MSG_ERROR([Qt4 Gui Library (libQtGui) is required.]))
#		AC_CHECK_HEADER([QtCore],[],AC_MSG_ERROR([Qt4 Core Headers (QtCore etc.) are required.]))
#		AC_CHECK_HEADER([QtGui], [],AC_MSG_ERROR([Qt4 Gui Headers (QtGui etc.) are required.]))
	])
])

########### libraries ##################
AS_VAR_IF([win32],[no],  [AC_CHECK_LIB([ssl],[SSL_get_version],[],AC_MSG_ERROR([OpenSSL Library is required.]))])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])
AS_VAR_IF([win32],[no],[AC_CHECK_HEADER([openssl/opensslconf.h],[],AC_MSG_ERROR([OpenSSL headers are required.]))])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([strerror vasprintf])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
