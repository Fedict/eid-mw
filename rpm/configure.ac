AC_PREREQ([2.61])
AC_INIT([eid-mw-rpm-installer],[4.0.0],[servicedesk@fedict.be])
AM_INIT_AUTOMAKE([foreign])
AC_CANONICAL_HOST

linux="no"
osx="no"
win32="no"
rpm_based="no"

AS_CASE([$host],
  [*-*-linux*],[linux=true],
  [*-*-darwin*],[osx=true],
  [*-mingw*|*-winnt*],[win32=true])

AS_VAR_IF([linux],[true],
[
	AC_CHECK_PROGS(RPM,[rpm],[NOT_RPM_BASED])
	AS_VAR_IF([RPM],[NOT_RPM_BASED],AC_MSG_NOTICE([Non-RPM-Based distro. Not Configuring RPM build.]),rpm_based=true)
	AM_CONDITIONAL([RPM_BASED_DISTRO],[test x$rpm_based = xtrue])
])

AS_VAR_IF([rpm_based],[true],
[
	AC_CHECK_PROGS(SED,[sed],[missing])
	AC_CHECK_PROGS(RPMBUILD,[rpmbuild],[missing])
	AS_VAR_IF([SED],[missing],AC_MSG_ERROR([the sed command is required. Please install sed.]),AC_SUBST(SED))
	AS_VAR_IF([RPMBUILD],[missing],AC_MSG_ERROR([the rpmbuild command is required. Please install rpmdevtools]),AC_SUBST(RPMBUILD))
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
