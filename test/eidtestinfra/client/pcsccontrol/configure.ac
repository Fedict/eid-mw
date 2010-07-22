AC_PREREQ([2.65])
AC_INIT([pcsccontrol], m4_esyscmd([echo -n 3.99.M1-`svnversion -n` | tr -d '\n' | tr -d '\r']), [servicedesk@fedict.be])
AM_INIT_AUTOMAKE([foreign])

AC_CHECK_PROGS(JAVAC,[javac],[missing])
AS_VAR_IF([JAVAC],[missing],AC_MSG_ERROR([Java compiler is required.]),AC_SUBST(JAVAC))

AC_CHECK_PROGS(ANT,[ant],[missing])
AS_VAR_IF([ANT],[missing],AC_MSG_ERROR([ant is required.]),AC_SUBST(ANT))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
