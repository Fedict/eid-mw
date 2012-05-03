AC_PREREQ([2.61])
AC_INIT([eid-mw-dmg-installer],[4.0.4],[servicedesk@fedict.be])
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

AS_VAR_IF([osx],[true],
[
AC_CHECK_PROGS(PACKAGEMAKER,[PackageMaker],[missing])
AS_VAR_IF([PACKAGEMAKER],[missing],AC_MSG_ERROR([PackageMaker is missing. You won't be able to build packages.]),AC_SUBST(PACKAGEMAKER))
AC_CHECK_PROGS(LIPO,[lipo],[missing])
AS_VAR_IF([LIPO],[missing],AC_MSG_ERROR([lipo is missing. You won't be able to build multi-platform packages.]),AC_SUBST(LIPO))
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
