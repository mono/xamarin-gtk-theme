AC_INIT([murrine], [0.90.1])
AC_CONFIG_SRCDIR([README])

AM_INIT_AUTOMAKE

dnl Initialize maintainer mode
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Initialize libtool
AM_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL

dnl Initialize intltool
AC_PROG_INTLTOOL([0.37.1])
GETTEXT_PACKAGE=murrine
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT


AC_ARG_ENABLE(animation,
              [  --enable-animation      compile murrine with animation support],
              [animation=$enableval],
              [animation="no"], )
AC_ARG_ENABLE(animationrtl,
              [  --enable-animationrtl   compile murrine with progressbar animation from right to left],
              [animationrtl=$enableval],
              [animationrtl="no"], )
AC_ARG_ENABLE(rgba,
              [  --disable-rgba          compile murrine without rgba support],
              [rgba=$enableval],
              [rgba="yes"], )

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.10.0,,
                  AC_MSG_ERROR([GTK+-2.10 is required to compile murrine]))

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

GTK_VERSION=`$PKG_CONFIG --variable=gtk_binary_version gtk+-2.0`
AC_SUBST(GTK_VERSION)

AC_SUBST(BUILD_ENGINES)
AC_SUBST(BUILD_THEMES)

if test $animation = "yes"; then
	AC_DEFINE_UNQUOTED(HAVE_ANIMATION, 1, [Defines whether to compile with animation support])
fi

if test $animationrtl = "yes"; then
	AC_DEFINE_UNQUOTED(HAVE_ANIMATIONRTL, 1, [Defines whether to compile with progressbar animation from right to left])
fi

if test $rgba = "yes"; then
	AC_DEFINE_UNQUOTED(HAVE_RGBA, 1, [Defines whether to compile with rgba support])
fi

AM_CONFIG_HEADER([src/config.h])

AC_CONFIG_FILES([
Makefile
po/Makefile.in
schema/Makefile
schema/murrine.xml.in
murrine.pc
]) 

AC_OUTPUT

echo
echo "Now type 'make' to compile murrine."
