AC_PREREQ([2.63])
AC_INIT([eid-mw-rpm],[4.0.0],[servicedesk@fedict.be])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([eid-mw.spec])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([subdir-objects foreign])

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
	AC_CHECK_PROGS(RPMBUILD,[rpmbuild],[missing])
	AC_CHECK_PROGS(RPMDEVSETUPTREE,[rpmdev-setuptree],[missing])

	AS_VAR_IF([WGET],[missing],AC_MSG_ERROR([the wget command is required]),AC_SUBST(WGET))
	AS_VAR_IF([RPMBUILD],[missing],AC_MSG_ERROR([the rpmbuild command is required. Please install rpmdevtools]),AC_SUBST(RPMBUILD))
	AS_VAR_IF([RPMDEVSETUPTREE],[missing],AC_MSG_ERROR([the rpmdev-setuptree command is required. Please install rpmdevtools]),AC_SUBST(RPMDEVSETUPTREE))
],
[
	AC_MSG_ERROR([building RPM only makes sense on RPM-based Linux distributions])
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
