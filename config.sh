rm -f aclocal.m4
autoheader
aclocal
automake -c -a
autoupdate
autoconf
