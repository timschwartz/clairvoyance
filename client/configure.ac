#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([clairvoyance-client], [0.1], [tim@metaverse.systems])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.
AC_CHECK_HEADER([json-c/json.h],
    [AC_DEFINE([HAVE_LIBJSON_C], [1], [Define if libjson-c exists.])
     LIBS="-ljson-c $LIBS"],
    [],
    [#include <json-c/json.h>])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT(Makefile src/Makefile)
