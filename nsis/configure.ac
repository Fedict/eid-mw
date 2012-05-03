AC_PREREQ([2.61])
AC_INIT([eid-mw-nsis-installer],[4.0.4], [servicedesk@fedict.be])
AM_INIT_AUTOMAKE([foreign])
AC_CANONICAL_HOST

linux="no"
osx="no"
win32="no"

AS_CASE([$host],
  [*-*-linux*],[linux=true gtk=true],
  [*-*-darwin*],[osx=true qt=true],
  [*-mingw*|*-winnt*],[win32=true])

AM_CONDITIONAL(LINUX,test x$linux = xtrue)
AM_CONDITIONAL(OSX,test x$osx = xtrue)
AM_CONDITIONAL(WIN32,test x$win32 = xtrue)

AS_VAR_IF([win32],[true],
[
	AC_CHECK_PROGS(SED,[sed],[missing])
	AC_CHECK_PROGS(MAKENSIS,[makensis],[missing])
    	AS_VAR_IF([SED],[missing],AC_MSG_ERROR([the sed command is required. Please install sed.]),AC_SUBST(SED))
	AS_VAR_IF([MAKENSIS],[missing],AC_MSG_ERROR([makensis is required. Please install nsis.]),AC_SUBST(MAKENSIS))
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
