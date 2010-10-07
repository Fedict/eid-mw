AC_PREREQ([2.65])
AC_INIT([eid-mw-arch-package],[4.0.0],[servicedesk@fedict.be])
AM_INIT_AUTOMAKE([foreign])
AC_CANONICAL_HOST

linux="no"
osx="no"
win32="no"

AS_CASE([$host],
  [*-*-linux*],[linux=true],
  [*-*-darwin*],[osx=true],
  [*-mingw*|*-winnt*],[win32=true])

AS_VAR_IF([linux],[true],
[
	AC_CHECK_PROGS(SED,[sed],[missing])
	AS_VAR_IF([SED],[missing],AC_MSG_ERROR([the sed command is required. Please install sed.]),AC_SUBST(SED))

])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
