AC_PREREQ([2.65])
AC_INIT([eid-mw-rpm-installer],[4.0.0],[servicedesk@fedict.be])
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
	AC_CHECK_PROGS(WGET,[wget],[missing])
	AC_CHECK_PROGS(SED,[sed],[missing])
	AC_CHECK_PROGS(RPMBUILD,[rpmbuild],[missing])
	AC_CHECK_PROGS(RPMDEVSETUPTREE,[rpmdev-setuptree],[missing])
	AS_VAR_IF([WGET],[missing],AC_MSG_ERROR([the wget command is required]),AC_SUBST(WGET))
    AS_VAR_IF([SED],[missing],AC_MSG_ERROR([the sed command is required. Please install sed.]),AC_SUBST(SED))
	AS_VAR_IF([RPMBUILD],[missing],AC_MSG_ERROR([the rpmbuild command is required. Please install rpmdevtools]),AC_SUBST(RPMBUILD))
	AS_VAR_IF([RPMDEVSETUPTREE],[missing],AC_MSG_ERROR([the rpmdev-setuptree command is required. Please install rpmdevtools]),AC_SUBST(RPMDEVSETUPTREE))

])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
