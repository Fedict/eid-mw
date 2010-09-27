AC_PREREQ([2.65])
AC_INIT([eid-mw-deb-installer],[4.0.0],[servicedesk@fedict.be])
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
	AC_CHECK_PROGS(BUILDPACKAGE,[dpkg-buildpackage],[missing])
	AS_VAR_IF([SED],[missing],AC_MSG_ERROR([the sed command is required. Please install sed.]),AC_SUBST(SED))
	AS_VAR_IF([BUILDPACKAGE],[missing],AC_MSG_ERROR([the dpkg-buildpackage command is required. Please install build-essentials.]),AC_SUBST(BUILDPACKAGE))
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
