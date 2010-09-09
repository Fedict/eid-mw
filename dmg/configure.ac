AC_PREREQ([2.65])
AC_INIT([eid-mw-dmg-installer],[4.0.0],[servicedesk@fedict.be])
AM_INIT_AUTOMAKE([foreign])

AC_CHECK_PROGS(PACKAGEMAKER,[PackageMaker],[missing])
AS_VAR_IF([PACKAGEMAKER],[missing],AC_MSG_ERROR([PackageMaker is missing. You won't be able to build packages.]),AC_SUBST(PACKAGEMAKER))
AC_CHECK_PROGS(LIPO,[lipo],[missing])
AS_VAR_IF([LIPO],[missing],AC_MSG_ERROR([lipo is missing. You won't be able to build packages.]),AC_SUBST(LIPO))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
