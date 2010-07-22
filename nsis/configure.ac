AC_PREREQ([2.65])
AC_INIT([eid-mw-nsis-installer], m4_esyscmd([echo -n 3.99.M1-`svnversion -n` | tr -d '\n' | tr -d '\r']), [servicedesk@fedict.be])
AM_INIT_AUTOMAKE([foreign])

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
	AC_CHECK_PROGS(MAKENSIS,[makensis],[missing])
	AS_VAR_IF([MAKENSIS],[missing],AC_MSG_WARN([makensis is missing. You won't be able to build the MS-Windows installers.]),AC_SUBST(JAVAC))
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
