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

case "${host}" in

    *-*-linux*)
        linux=true
		qt=true
        ;;  

    *-*-darwin*)
        osx=true
		qt=true
        ;;

    *-mingw*|*-winnt*)
        win32=true
        ;;
esac

AM_CONDITIONAL(LINUX,test x$linux = xtrue)
AM_CONDITIONAL(OSX,test x$osx = xtrue)
AM_CONDITIONAL(WIN32,test x$win32 = xtrue)
AM_CONDITIONAL(QT,test x$qt = xtrue)

########### libraries ##################
# on Windows, check for msvcr
if test "${win32}" = "yes"; then
    AC_CHECK_LIB([msvcr], [main])
fi

# on GNU/Linux and OSX, enable --with-qt
if test "${win32}" = "no"; then
	AC_MSG_CHECKING([Qt installation])
	AC_ARG_WITH(qt, AC_HELP_STRING([--with-qt=DIR],
	                [Qt install location]),,with_qt=/usr)
	QT_INCS="-I$with_qt/include"
	QT_LIBS="-L$with_qt/lib"
	QT_BINDIR="$with_qt/bin"
	AC_MSG_RESULT([$with_qt])
fi

AC_SUBST([QT_INCS])
AC_SUBST([QT_LIBS])
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
