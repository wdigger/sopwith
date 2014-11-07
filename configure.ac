


AC_PREREQ([2.69])
AC_INIT([SDL Sopwith], [1.8.x], [fraggle@gmail.com])
AC_CONFIG_SRCDIR([src/swmain.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign subdir-objects dist-xz])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for libraries.
PKG_CHECK_MODULES([SDL2], [sdl2], [
	echo "Building SDL version."
	SOPWITH_BINS="$SOPWITH_BINS sopwith"
	SOPWITH_SUBDIRS="$SOPWITH_SUBDIRS sdl"
],[
	echo "Not building SDL version."
])

PKG_CHECK_MODULES(GTK, glib-2.0 >= 2.0 gtk+-2.0 >= 2.0, [
	echo "Building Gtk+ version."
	SOPWITH_BINS="$SOPWITH_BINS gtksopwith"
	SOPWITH_SUBDIRS="$SOPWITH_SUBDIRS gtk"
],[
	echo "Not building Gtk+ version."
])


# Checks for header files.
AC_CHECK_HEADER(netinet/ip.h, [
	AC_DEFINE(TCPIP, 1, [TCPIP multiplayer support])
])

AC_SUBST(SOPWITH_BINS)
AC_SUBST(SOPWITH_SUBDIRS)

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/gtk/Makefile
	src/sdl/Makefile
	doc/Makefile
])
AC_OUTPUT

