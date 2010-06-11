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

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

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

AS_VAR_IF([qt],[true],
[
	AC_CHECK_PROGS(RCC,[rcc],[missing])
	AC_CHECK_PROGS(UIC,[uic uic-qt4],[missing])
	AC_CHECK_PROGS(MOC,[moc moc-qt4],[missing])
	AS_VAR_IF([RCC],[missing],AC_MSG_ERROR([Qt4 Resource Compiler (rcc) is required.]),AC_SUBST(RCC))
	AS_VAR_IF([UIC],[missing],AC_MSG_ERROR([Qt4 User Interface Compiler (uic or uic-qt4) is required.]),AC_SUBST(UIC))
	AS_VAR_IF([MOC],[missing],AC_MSG_ERROR([Qt4 Meta Object Compiler (moc or moc-qt4) is required.]),AC_SUBST(MOC))
])

########### libraries ##################
# on Windows, check for msvcr
if test "${win32}" = "yes"; then
    AC_CHECK_LIB([msvcr], [main])
fi

# on GNU/Linux and OSX, enable --with-qt and --with-openssl
if test "${win32}" = "no"; then
	AC_MSG_CHECKING([Qt installation])
	AC_ARG_WITH(qt, AC_HELP_STRING([--with-qt=DIR],
	                [Qt install location]),,with_qt=/usr)
	QT_INCS="-I$with_qt/include"
	QT_LIBS="-L$with_qt/lib"
	QT_BINDIR="$with_qt/bin"
	AC_MSG_RESULT([$with_qt])

###### openssl ##########
AC_MSG_CHECKING([openssl installation])
AC_ARG_WITH(openssl, AC_HELP_STRING([--with-openssl=DIR],
                    [openssl install location]))
	OPENSSL_INCS="-I$with_openssl/include"
	OPENSSL_LIBS="-L$with_openssl/lib"
AC_MSG_RESULT([$with_openssl])

fi

AC_SUBST([QT_INCS])
AC_SUBST([QT_LIBS])
AC_SUBST([OPENSSL_INCS])
AC_SUBST([OPENSSL_LIBS])
AC_SUBST([QT_BINDIR])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([strerror vasprintf])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
