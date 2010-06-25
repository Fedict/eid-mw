AC_PREREQ([2.65])
AC_INIT([eid-mw], m4_esyscmd([echo -n 3.99.`svnversion -n` | tr -d '\n' | tr -d '\r']), [servicedesk@fedict.be])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([pcscproxy])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

LT_INIT
LT_PREREQ([2.2.7c])

AM_INIT_AUTOMAKE([subdir-objects foreign])

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

# on Windows, check for msvcr
AS_VAR_IF([win32],[true],[AC_CHECK_LIB([msvcr],[getpid])])

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

AC_CONFIG_SUBDIRS([etilib
                   pcscproxy
				   pcsccontrol])
AC_OUTPUT
