AC_PREREQ([2.65])
AC_INIT([eid-mw], m4_esyscmd([echo -n 3.99.`svnversion -n`]), [servicedesk@fedict.be])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([common])
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


# Checks for libraries.
# FIXME: Replace `main' with a function in `-lbeidcardlayer':
AC_CHECK_LIB([beidcardlayer], [main])
# FIXME: Replace `main' with a function in `-lbeidcommon':
AC_CHECK_LIB([beidcommon], [main])
# FIXME: Replace `main' with a function in `-lbeidlib':
AC_CHECK_LIB([beidlib], [main])
# FIXME: Replace `main' with a function in `-lbeidlibC':
AC_CHECK_LIB([beidlibC], [main])
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lpcsclite':
AC_CHECK_LIB([pcsclite], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h float.h limits.h netinet/in.h stdlib.h string.h sys/file.h sys/ioctl.h sys/time.h unistd.h malloc.h memory.h sys/timeb.h])

###### QT includes and libs ##########
AC_MSG_CHECKING([Qt installation])
AC_ARG_WITH(qt, AC_HELP_STRING([--with-qt=DIR],
                [Qt install location]))
AC_MSG_RESULT([$with_qt])

###### pcsc-lite ##########
AC_MSG_CHECKING([pcsc-lite installation])
AC_ARG_WITH(pcsclite, AC_HELP_STRING([--with-pcsclite=DIR],
                    [pcsc-lite install location]))
AC_MSG_RESULT([$with_pcsclite])

###### openssl ##########
AC_MSG_CHECKING([openssl installation])
AC_ARG_WITH(openssl, AC_HELP_STRING([--with-openssl=DIR],
                    [openssl install location]))
AC_MSG_RESULT([$with_openssl])

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

AC_CONFIG_SUBDIRS([common
                   dialogs
				   cardlayer
                   pkcs11])
AC_OUTPUT
