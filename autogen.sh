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

if automake-1.10 --version < /dev/null > /dev/null 2>&1 ; then
    AUTOMAKE=automake-1.10
    ACLOCAL=aclocal-1.10
elif automake-1.9 --version < /dev/null > /dev/null 2>&1 ; then
    AUTOMAKE=automake-1.9
    ACLOCAL=aclocal-1.9
elif automake-1.8 --version < /dev/null > /dev/null 2>&1 ; then
    AUTOMAKE=automake-1.8
    ACLOCAL=aclocal-1.8
elif automake-1.7 --version < /dev/null > /dev/null 2>&1 ; then
    AUTOMAKE=automake-1.7
    ACLOCAL=aclocal-1.7
else
        echo
        echo "You must have automake 1.7.x installed to compile $PROJECT."
        echo "Install the appropriate package for your distribution,"
        echo "or get the source tarball at http://ftp.gnu.org/gnu/automake/"
        DIE=1
fi

(libtool --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have libtool installed to compile GTK+."
	echo "Get http://ftp.gnu.org/gnu/libtool/libtool-1.5.10.tar.gz"
	echo "(or a newer version if it is available)"
	DIE=1
}
if test "$DIE" -eq 1; then
	exit 1
fi

if test -z "$*"; then
	echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

libtoolize --force --copy

$ACLOCAL $ACLOCAL_FLAGS
autoconf
$AUTOMAKE --add-missing
cd $THEDIR

$srcdir/configure --enable-maintainer-mode "$@"

echo 
echo "Now type 'make' to compile themes"
