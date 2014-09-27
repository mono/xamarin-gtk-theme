#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.
DIE=0

THEDIR=`pwd`
cd $srcdir

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have autoconf installed to compile GTK+."
	echo "Download the appropriate package for your distribution,"
	echo "or get the source tarball at http://ftp.gnu.org/gnu/autoconf/"
	DIE=1
}

if automake --version < /dev/null > /dev/null 2>&1 ; then
	AUTOMAKE=automake
	ACLOCAL=aclocal
else
	echo
	echo "You must have automake 1.7.x installed to compile $PROJECT."
	echo "Install the appropriate package for your distribution,"
	echo "or get the source tarball at http://ftp.gnu.org/gnu/automake/"
	DIE=1
fi

if libtool --version > /dev/null 2> /dev/null ; then
   LIBTOOL=libtool
elif glibtool --version > /dev/null 2> /dev/null ; then
   LIBTOOL=glibtool
else
   echo "libtool missing"
   DIE=1
fi

if libtoolize --version > /dev/null 2> /dev/null ; then
   LIBTOOLIZE=libtoolize
elif glibtoolize --version > /dev/null 2> /dev/null ; then
   LIBTOOLIZE=glibtoolize
else
   echo "libtoolize missing"
   DIE=1
fi

($LIBTOOL --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have libtool installed to compile murrine."
	echo "Get http://ftp.gnu.org/gnu/libtool/libtool-1.5.10.tar.gz"
	echo "(or a newer version if it is available)"
	DIE=1
}

(intltoolize --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have intltool installed to compile murrine."
	DIE=1
}
if test "$DIE" -eq 1; then
	exit 1
fi

if test -z "$*"; then
	echo "I am going to run ./configure with no arguments - if you wish "
	echo "to pass any to it, please specify them on the $0 command line."
fi

$ACLOCAL $ACLOCAL_FLAGS

$LIBTOOLIZE --force --copy
intltoolize --force --copy --automake

autoheader

$AUTOMAKE --add-missing
autoconf

cd $THEDIR

$srcdir/configure "$@"
