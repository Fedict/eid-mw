AC_PREREQ([2.65])
AC_INIT([eid-mw-nsis-installer], [0.0.1], [servicedesk@fedict.be])
AM_INIT_AUTOMAKE([foreign])

AC_CHECK_PROGS(MAKENSIS,[makensis],[missing])
AS_VAR_IF([MAKENSIS],[missing],AC_MSG_ERROR([makensis is required]),AC_SUBST(JAVAC))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
