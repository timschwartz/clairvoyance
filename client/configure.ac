#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([clairvoyance-client], [0.1], [tim@metaverse.systems])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.
AC_CHECK_HEADER([winsock2.h],
    [AC_DEFINE([HAVE_LIBWS2_32], [1], [Define if libws2_32 exists.])
     LIBS="-lws2_32 -lgdi32 -lwldap32 $LIBS"
     AM_CONDITIONAL([WIN32], true)],
    [AM_CONDITIONAL([WIN32], false)],
    [#include <windows.h>])

AC_CHECK_HEADER([json-c/json.h],
    [AC_DEFINE([HAVE_LIBJSON_C], [1], [Define if libjson-c exists.])
     LIBS="-ljson-c $LIBS"],
    [],
    [#include <json-c/json.h>])

PKG_CHECK_MODULES([SSL], openssl)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT(Makefile src/Makefile)
