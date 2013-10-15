AC_PREREQ([2.61])
AC_INIT([eid-mw],[4.0.6],[servicedesk@fedict.be])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([common])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

LT_INIT
AM_INIT_AUTOMAKE([subdir-objects foreign tar-pax dist-zip])

# Checks for programs.
AC_PROG_CXX([g++])
AC_PROG_CPP
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S

AS_VAR_IF([GXX],[yes],[],AC_MSG_ERROR([GNU C++ compiler (g++) is required.]))

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h float.h limits.h netinet/in.h stdlib.h string.h sys/file.h sys/ioctl.h sys/time.h unistd.h malloc.h memory.h sys/timeb.h])

###### pcsc-lite ##########
AC_MSG_CHECKING([pcsc-lite installation])
AC_ARG_WITH(pcsclite, AC_HELP_STRING([--with-pcsclite=DIR],
                    [pcsc-lite install location]))
AC_MSG_RESULT([$with_pcsclite])

AS_VAR_IF([with_pcsclite],[no],
[
	PKG_CHECK_MODULES([PCSC],[libpcsclite >= 1.4.4],
   	[
        CPPFLAGS="$PCSC_CFLAGS $CPPFLAGS"
        LDFLAGS="$PCSC_LIBS $CPPFLAGS"
	])
])

###########################################################################
####### read user-specific requests from --enable directives         ######
###########################################################################
AC_MSG_CHECKING(whether to enable dialogs)
default="yes"
NO_DIALOGS=
AC_ARG_ENABLE(dialogs, [  --enable-dialogs=[yes/no] turn off dialogs
                               [default=$default]],, enable_dialogs=$default)
if test "x$enable_dialogs" = "xno"; then
AC_MSG_RESULT(no)
else
AC_MSG_RESULT(yes)
fi
AM_CONDITIONAL([NO_DIALOGS],[test x$enable_dialogs = xno])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gethostbyname socket strerror ftime memchr memset mkdir select strchr strdup strerror strncasecmp strrchr strstr strtol strtoul vasprintf])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_SUBDIRS([common cardlayer pkcs11 xpi])

if test "x$enable_dialogs" = "xyes"; then
	AC_CONFIG_SUBDIRS([dialogs])
fi

AC_OUTPUT
