AC_PREREQ([2.61])
AC_INIT([libbeiddialogs],[4.0.0],[support@fedict.be])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/langutil.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST

# Checks for programs (1)
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

############################################################################
# Test for platform and set corresponding AM_CONDITIONALs for Makefile.am  #
############################################################################

linux="no"
osx="no"
win32="no"
gtk="no"

AS_CASE([$host],
  [*-*-linux*],[linux=true gtk=true],
  [*-*-darwin*],[osx=true],
  [*-mingw*|*-winnt*],[win32=true])

AM_CONDITIONAL(LINUX,test x$linux = xtrue)
AM_CONDITIONAL(OSX,test x$osx = xtrue)
AM_CONDITIONAL(WIN32,test x$win32 = xtrue)
AM_CONDITIONAL(GTK,test x$gtk = xtrue)

###########################################################################
####### read user-specific dependecy requests from --with directives ######
###########################################################################

AC_ARG_ENABLE(debug,
[  --enable-debug    Turn on debugging],
[case "${enableval}" in
  yes) debug=true ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
esac],[debug=false])
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)

# if gtk dialogs, enable checking GTK2
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
                # PKG_CHECK_MODULES is possibly undefined
                # as we do not use pkgconfig on Windows and Mac
                m4_ifdef([PKG_CHECK_MODULES],
                [
                        PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.10.0],
                        [
                                CPPFLAGS="$GTK_CFLAGS $CPPFLAGS"
                                LDFLAGS="$GTK_LIBS $CPPFLAGS"
                        ],
                        AC_MSG_RESULT([no])
                        AC_MSG_ERROR([you must have GTK+ 2.10.0 or newer development headers installed.])
                        )
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
####### We need xcodebuild to build the OSX dialogs ##############
##################################################################

AS_VAR_IF([osx],[true],
[
	AC_CHECK_PROGS(XCODEBUILD,[xcodebuild],[missing])
	AS_VAR_IF([XCODEBUILD],[missing],AC_MSG_ERROR([xcodebuild command-line build tool is required.]),AC_SUBST(XCODEBUILD))
])


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
