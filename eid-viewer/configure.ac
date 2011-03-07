AC_PREREQ([2.61])
AC_INIT([eid-viewer],[4.0.0],[servicedesk@fedict.be])
AC_CONFIG_SRCDIR([pom.xml])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([1.9 foreign tar-pax])

# Checks for programs.
AC_CHECK_PROGS(JAVAC,[javac],[missing])
AS_VAR_IF([JAVAC],[missing],AC_MSG_ERROR([Java compiler is required.]),AC_SUBST(JAVAC))

AC_CHECK_PROGS(MVN,[mvn],[missing])
AS_VAR_IF([MVN],[missing],AC_MSG_ERROR([Maven2 is required.]),AC_SUBST(MVN))

AC_MSG_CHECKING(whether to enable windows executable wrapper)
default="no"
WIN32EXE=
AC_ARG_ENABLE(win32exe, [  --enable-win32exe=[yes/no] turn on windows exe wrapper 
                               [default=$default]],, enable_win32exe=$default)
if test "x$enable_win32exe" = "xno"; then
AC_MSG_RESULT(no)
else
AC_MSG_RESULT(yes)
fi
AM_CONDITIONAL([WIN32EXE],[test x$enable_win32exe = xyes])
AM_CONDITIONAL([UNIXEXE],[test x$enable_win32exe = xno])

linux="no"
osx="no"
win32="no"

AS_CASE([$host],
  [*-*-linux*],[linux=true],
  [*-*-darwin*],[osx=true],
  [*-mingw*|*-winnt*],[win32=true])

AM_CONDITIONAL([LINUX],[test x$linux = xtrue])
AM_CONDITIONAL([OSX],[test x$osx = xtrue])
AM_CONDITIONAL([WIN32],[test x$win32 = xtrue])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
