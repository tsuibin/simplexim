#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([dp-xims], [0.1], [hukang@linuxdeepin.com])

AC_CONFIG_SRCDIR([dp-xims.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])
AC_GNU_SOURCE

LT_INIT


# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([langinfo.h limits.h locale.h stdlib.h string.h sys/param.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for packages.
PKG_CHECK_MODULES(GLIB, [glib-2.0])
PKG_CHECK_MODULES(GOBJECT, [gobject-2.0])
PKG_CHECK_MODULES(GTK3, [gtk+-3.0])
PKG_CHECK_MODULES(GTK3X11, [gtk+-x11-3.0])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit memmove memset])

AC_CONFIG_FILES([
Makefile
IMdkit/Makefile
])

AC_OUTPUT
