#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_PREREQ([2.69])

AC_INIT([JunkBox_Lib],[1.10.0alpha],[fumi.hax@gmail.com],[JunkBox_Lib])
AC_CONFIG_SRCDIR([Lib/tools.c])
AC_CONFIG_HEADER([config.h])

#AM_INIT_AUTOMAKE([no-define])
AM_INIT_AUTOMAKE([subdir-objects])


AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug], [enable debug mode]), [
echo -n "checking debug mode... "
if test "$enableval" = "yes" ; then
    DEBUG="-DEBUG"
    AC_SUBST(DEBUG)
    echo "yes"
else
    echo "no"
fi
])


# for Berkeley DB
# xLib/fdb_tool.c をコンパイルするか？
AC_ARG_ENABLE(db, AS_HELP_STRING([--enable-db=DIR], [enable DB function in Lib]), [
echo -n "checking enable Berkeley DB function in xLib... "
ENABLE_DB_FNC="$enableval"
if test "$enableval" = "no" ; then
    echo "no"
else
    DB_DIR="$enableval"
    if test "$DB_DIR" = "yes" ; then
        DB_DIR="/usr"
    fi
    echo "yes   sdir = $DB_DIR"
    if test ! -d $DB_DIR ; then
        AC_MSG_ERROR([DB top directory $DB_DIR is not exist.])
        ENABLE_DB_FNC="no"
    else
        DB_INC_DIR="-I${DB_DIR}/include"
        DB_LIB_DIR="-L${DB_DIR}/lib"
        ENABLE_DB="-DENABLE_BERKELEYDB"
        AC_SUBST(DB_INC_DIR)
        AC_SUBST(DB_LIB_DIR)
        AC_SUBST(ENABLE_DB)
        ENABLE_DB_FNC="yes"
    fi
fi
],[
# default
echo "including DB function... no"
ENABLE_DB_FNC="no"
])
AM_CONDITIONAL(ENABLE_DB_FNC, test "$ENABLE_DB_FNC" = "yes")


# for ZLIB
# xLib/gz-tool.c をコンパイルするか？
AC_ARG_ENABLE(zlib, AS_HELP_STRING([--enable-zlib=DIR], [enable ZLIB function in xLib]), [
echo -n "checking enable zlib function in xLib... "
ENABLE_ZLIB_FNC="$enableval"
if test "$enableval" = "no" ; then
    echo "no"
else
    ZLIB_DIR="$enableval"
    if test "$ZLIB_DIR" = "yes" ; then
        ZLIB_DIR="/usr"
    fi
    echo "yes   sdir = $ZLIB_DIR"
    if test ! -d $ZLIB_DIR ; then
        AC_MSG_ERROR([ZLIB top directory $ZLIB_DIR is not exist.])
        ENABLE_ZLIB_FNC="no"
    else
        ZLIB_INC_DIR="-I${ZLIB_DIR}/include"
        ZLIB_LIB_DIR="-L${ZLIB_DIR}/lib"
        ENABLE_ZLIB="-DENABLE_ZLIB"
        AC_SUBST(ZLIB_INC_DIR)
        AC_SUBST(ZLIB_LIB_DIR)
        AC_SUBST(ENABLE_ZLIB)
        ENABLE_ZLIB_FNC="yes"
    fi
fi
],[
# default
echo "checking enable zlib function in xLib... yes"
ZLIB_DIR="/usr"
ZLIB_INC_DIR="-I${ZLIB_DIR}/include"
ZLIB_LIB_DIR="-L${ZLIB_DIR}/lib"
ENABLE_ZLIB="-DENABLE_ZLIB"
AC_SUBST(ZLIB_INC_DIR)
AC_SUBST(ZLIB_LIB_DIR)
AC_SUBST(ENABLE_ZLIB)
ENABLE_ZLIB_FNC="yes"
])
AM_CONDITIONAL(ENABLE_ZLIB_FNC, test "$ENABLE_ZLIB_FNC" = "yes")


# for OpenLDAP
# xLib/ldap_tool.c をコンパイルするか？
AC_ARG_ENABLE(ldap, AS_HELP_STRING([--enable-ldap=DIR], [enable LDAP function in xLib]), [
echo -n "checking enable ldap function in xLib... "
ENABLE_LDAP_FNC="$enableval"
if test "$enableval" = "no" ; then
    echo "no"
else
    LDAP_DIR="$enableval"
    if test "$LDAP_DIR" = "yes" ; then
        LDAP_DIR="/usr"
    fi
    echo "yes   sdir = $LDAP_DIR"
    if test ! -d $LDAP_DIR ; then
        AC_MSG_ERROR([ldap top directory $LDAP_DIR is not exist.])
        ENABLE_LDAP_FNC="no"
    else
        LDAP_INC_DIR="-I${LDAP_DIR}/include"
        LDAP_LIB_DIR="-L${LDAP_DIR}/lib"
        ENABLE_LDAP="-DENABLE_LDAP"
        AC_SUBST(LDAP_INC_DIR)
        AC_SUBST(LDAP_LIB_DIR)
        AC_SUBST(ENABLE_LDAP)
        ENABLE_LDAP_FNC="yes"
    fi
fi
],[
# default
echo "checking enable ldap function in xLib... no"
ENABLE_LDAP_FNC="no"
])
AM_CONDITIONAL(ENABLE_LDAP_FNC, test "$ENABLE_LDAP_FNC" = "yes")


# for OpenSSL
# SSLを含むソースをコンパイルするか？
AC_ARG_ENABLE(ssl, AS_HELP_STRING([--enable-ssl=DIR], [enable SSL function in xLib]), [
echo -n "checking enable SSL function in xLib... "
ENABLE_SSL_FNC="$enableval"
if test "$enableval" = "no" ; then
    echo "no"
else
    SSL_DIR="$enableval"
    if test "$SSL_DIR" = "yes" ; then
        SSL_DIR="/usr"
    fi
    echo "yes   sdir = $SSL_DIR"
    if test ! -d $SSL_DIR ; then
        AC_MSG_ERROR([SSL top directory $SSL_DIR is not exist.])
        ENABLE_SSL_FNC="no"
    else
        SSL_INC_DIR="-I${SSL_DIR}/include"
        SSL_LIB_DIR="-L${SSL_DIR}/lib"
        ENABLE_SSL="-DENABLE_SSL"
        AC_SUBST(SSL_INC_DIR)
        AC_SUBST(SSL_LIB_DIR)
        AC_SUBST(ENABLE_SSL)
        ENABLE_SSL_FNC="yes"
    fi
fi
],[
# default
echo "checking enable SSL function in xLib... no"
ENABLE_SSL_FNC="no"
])
AM_CONDITIONAL(ENABLE_SSL_FNC, test "$ENABLE_SSL_FNC" = "yes")


# for MySQL
# MySQLを含むソースをコンパイルするか？
AC_ARG_ENABLE(mysql, AS_HELP_STRING([--enable-mysql=DIR], [enable MySQL function in xLib]), [
echo -n "checking enable MySQL function in xLib... "
ENABLE_MYSQL_FNC="$enableval"
if test "$enableval" = "no" ; then
    echo "no"
else
    MYSQL_DIR="$enableval"
    if test "$MYSQL_DIR" = "yes" ; then
        MYSQL_DIR="/usr"
    fi
    echo "yes   sdir = $MYSQL_DIR"
    if test ! -d $MYSQL_DIR ; then
        AC_MSG_ERROR([MySQL top directory $MYSQL_DIR is not exist.])
        ENABLE_MYSQL_FNC="no"
    else
        MYSQL_INC_DIR="-I${MYSQL_DIR}/include"
        MYSQL_LIB_DIR="-L${MYSQL_DIR}/lib/mysql"
        ENABLE_MYSQL="-DENABLE_MYSQL"
        AC_SUBST(MYSQL_INC_DIR)
        AC_SUBST(MYSQL_LIB_DIR)
        AC_SUBST(ENABLE_MYSQL)
        ENABLE_MYSQL_FNC="yes"
    fi
fi
],[
# default
echo "checking enable MySQL function in xLib... no"
ENABLE_MYSQL_FNC="no"
])
AM_CONDITIONAL(ENABLE_MYSQL_FNC, test "$ENABLE_MYSQL_FNC" = "yes")


# for libjpeg
# gLib/jpeg_tool.c を含むソースをコンパイルするか？
AC_ARG_ENABLE(jpeg, AS_HELP_STRING([--enable-jpeg=DIR], [enable libjpeg function in gLib]), [
echo -n "checking enable libjpeg function in gLib... "
ENABLE_JPEG_FNC="$enableval"
if test "$enableval" = "no" ; then
    echo "no"
else
    JPEG_DIR="$enableval"
    if test "$JPEG_DIR" = "yes" ; then
        JPEG_DIR="/usr"
    fi
    echo "yes   sdir = $JPEG_DIR"
    if test ! -d $JPEG_DIR ; then
        AC_MSG_ERROR([libjpeg top directory $JPEG_DIR is not exist.])
        ENABLE_JPEG_FNC="no"
    else 
        JPEG_INC_DIR="-I${JPEG_DIR}/include"
        JPEG_LIB_DIR="-L${JPEG_DIR}/lib"
        ENABLE_JPEG="-DENABLE_JPEG"
        AC_SUBST(JPEG_INC_DIR)
        AC_SUBST(JPEG_LIB_DIR)
        AC_SUBST(ENABLE_JPEG)
        ENABLE_JPEG_FNC="yes"
    fi
fi
],[
# default
echo "checking enable libjpeg function in gLib... no"
ENABLE_JPEG_FNC="no"
])
AM_CONDITIONAL(ENABLE_JPEG_FNC, test "$ENABLE_JPEG_FNC" = "yes")


# for libpng
# gLib/png_tool.c を含むソースをコンパイルするか？
AC_ARG_ENABLE(png, AS_HELP_STRING([--enable-png=DIR], [enable libpng function in gLib]), [
echo -n "checking enable libpng function in gLib... "
ENABLE_PNG_FNC="$enableval"
if test "$enableval" = "no" ; then
    echo "no"
else
    PNG_DIR="$enableval"
    if test "$PNG_DIR" = "yes" ; then
        PNG_DIR="/usr"
    fi
    echo "yes   sdir = $PNG_DIR"
    if test ! -d $PNG_DIR ; then
        AC_MSG_ERROR([libpng top directory $PNG_DIR is not exist.])
        ENABLE_PNG_FNC="no"
    else 
        PNG_INC_DIR="-I${PNG_DIR}/include"
        PNG_LIB_DIR="-L${PNG_DIR}/lib"
        ENABLE_PNG="-DENABLE_PNG"
        AC_SUBST(PNG_INC_DIR)
        AC_SUBST(PNG_LIB_DIR)
        AC_SUBST(ENABLE_PNG)
        ENABLE_PNG_FNC="yes"
    fi
fi
],[
# default
echo "checking enable libpng function in gLib... no"
ENABLE_PNG_FNC="no"
])
AM_CONDITIONAL(ENABLE_PNG_FNC, test "$ENABLE_PNG_FNC" = "yes")


# for openjpeg
# gLib/jp2k_tool.c を含むソースをコンパイルするか？
AC_ARG_ENABLE(openjpeg, AS_HELP_STRING([--enable-openjpeg=DIR:VERSION], [enable openjpeg function in gLib]), [
echo -n "checking enable openjpeg function in gLib... "
ENABLE_OPENJPEG_FNC="$enableval"
if test "$enableval" = "no" ; then
    echo "no"
else
    OPENJPEG_DIR=`echo $enableval | awk -F":" '{print $1}'`
    OPENJPEG_VER=`echo $enableval | awk -F":" '{print $2}'`

    # Version
    if test "$OPENJPEG_VER" = "" ; then
        OPENJPEG_VER="2.1"
        OPENJPEG_VER_INT="21"
        OPENJPEG_INC=openjpeg-${OPENJPEG_VER}
    fi
    if test `awk "BEGIN {print $OPENJPEG_VER < 1.4}"` ; then
        OPENJPEG_VER_INT=`echo $OPENJPEG_VER | sed -e "s/\.//"`
        OPENJPEG_INC=""
    fi
    if test `awk "BEGIN {print $OPENJPEG_VER >= 1.4}"` ; then
        OPENJPEG_VER_INT=`echo $OPENJPEG_VER | sed -e "s/\.//"`
        OPENJPEG_INC=openjpeg-${OPENJPEG_VER}
    fi

    # Directory
    if test "$OPENJPEG_DIR" = "" -o "$OPENJPEG_DIR" = "yes" ; then
        OPENJPEG_DIR="/usr"
    fi
    if test ! -d $OPENJPEG_DIR ; then
        AC_MSG_ERROR([openjpeg top directory $OPENJPEG_DIR is not exist.])
        ENABLE_OPENJPEG_FNC="no"
    else
        echo "yes   sdir = $OPENJPEG_DIR, ver = $OPENJPEG_VER"
        OPENJPEG_INC_DIR="-I${OPENJPEG_DIR}/include/${OPENJPEG_INC}"
        OPENJPEG_LIB_DIR="-L${OPENJPEG_DIR}/lib"
        ENABLE_OPENJPEG="-DENABLE_OPENJPEG"
        AC_SUBST(OPENJPEG_INC_DIR)
        AC_SUBST(OPENJPEG_LIB_DIR)
        AC_SUBST(ENABLE_OPENJPEG)
        #
        ENABLE_OPENJPEG_VER="-DOPENJPEG_VER=${OPENJPEG_VER_INT}"
        AC_SUBST(ENABLE_OPENJPEG_VER)
        ENABLE_OPENJPEG_FNC="yes"
    fi
fi
],[
# default
echo "checking enable openjpeg function in gLib... no"
])
AM_CONDITIONAL(ENABLE_OPENJPEG_FNC, test "$ENABLE_OPENJPEG_FNC" = "yes")


# GLIBをコンパイルするか？
AC_ARG_ENABLE(gLib, AS_HELP_STRING([--enable-gLib], [gLib Graphic Library (gLib)]), [
echo -n "checking enable Graphic Library (gLib)... "
ENABLE_GLIB="$enableval"
if test "$enableval" = "no" ; then
    echo "no"
else
    echo "yes"
fi
],[
# default
ENABLE_GLIB="yes"
])
AM_CONDITIONAL(ENABLE_GLIB, test "$ENABLE_GLIB" = "yes")


# X11機能をコンパイルするか？
AC_ARG_ENABLE(X11, AS_HELP_STRING([--enable-X11], [X11  Library (X11)]), [
echo -n "checking enable X11 Window Function (X11)... "
ENABLE_X11="$enableval"
if test "$enableval" = "no" ; then
    echo "no"
else
    echo "yes"
fi
],[
# default
ENABLE_X11="yes"
])
#AM_CONDITIONAL(ENABLE_X11, test "$ENABLE_X11" = "yes")

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_RANLIB


# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcrypt':
AC_CHECK_LIB([crypt], [main])
# FIXME: Replace `main' with a function in `-lcrypto':
AC_CHECK_LIB([crypto], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lnsl':
AC_CHECK_LIB([nsl], [main])
# FIXME: Replace `main' with a function in `-lpam':
AC_CHECK_LIB([pam], [main])
# FIXME: Replace `main' with a function in `-lsocket':
AC_CHECK_LIB([socket], [main])

if test "$ENABLE_DB_FNC" = "yes" ; then
    LDFLAGS="$LDFLAGS $DB_LIB_DIR"
    AC_CHECK_LIB([db-4.2], [main])
fi
if test "$ENABLE_MYSQL_FNC" = "yes" ; then
    LDFLAGS="$LDFLAGS $MYSQL_LIB_DIR"
    AC_CHECK_LIB([mysqlclient], [main])
fi

if test "$ENABLE_ZLIB_FNC" = "yes" ; then
    LDFLAGS="$LDFLAGS $ZLIB_LIB_DIR"
    AC_CHECK_LIB([z], [main])
fi
if test "$ENABLE_LDAP_FNC" = "yes" ; then
    LDFLAGS="$LDFLAGS $LDAP_LIB_DIR"
    AC_CHECK_LIB([ldap], [main])
fi
if test "$ENABLE_SSL_FNC" = "yes" ; then
    LDFLAGS="$LDFLAGS $SSL_LIB_DIR"
    AC_CHECK_LIB([ssl], [main])
fi
if test "$ENABLE_JPEG_FNC" = "yes" ; then
    LDFLAGS="$LDFLAGS $JPEG_LIB_DIR"
    AC_CHECK_LIB([jpeg], [main])
fi
if test "$ENABLE_PNG_FNC" = "yes" ; then
    LDFLAGS="$LDFLAGS $PNG_LIB_DIR"
    AC_CHECK_LIB([png], [main])
fi
if test "$ENABLE_OPENJPEG_FNC" = "yes" ; then
  LDFLAGS="$LDFLAGS $OPENJPEG_LIB_DIR"
  if test "$OPENJPEG_VER_INT" -le "15" ; then
    AC_CHECK_LIB([openjpeg], [main])
  else
    AC_CHECK_LIB([openjp2], [main])
  fi
fi


DISABLE_WIN="no"
ENABLE_WIN="yes"

IS_MINGW=`uname -s|grep MINGW32`
if test "$IS_MINGW" = "" ; then
    DISABLE_WIN="yes"
    ENABLE_WIN="no"
fi

AM_CONDITIONAL(DISABLE_WIN, test "$DISABLE_WIN" = "yes")
AM_CONDITIONAL(ENABLE_WIN,  test "$ENABLE_WIN"  = "yes")


AC_PATH_XTRA
#if test "$X_CFLAGS" != "" -a "$ENABLE_X11" = "yes" -a "$DISABLE_WIN" = "yes" ; then
if test "$ENABLE_X11" = "yes" -a "$DISABLE_WIN" = "yes" ; then
    if test "$X_CFLAGS" = "" ; then
        X_CFLAGS="-I/usr/include/X11"
    fi
    ENABLE_X11="yes"
else
    ENABLE_X11="no"
fi
AM_CONDITIONAL(ENABLE_X11, test "$ENABLE_X11" = "yes")



# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([rpcsvc/ypclnt.h sys/socket.h sys/errno.h locale.h syslog.h crypt.h sys/sockio.h arpa/inet.h fcntl.h netdb.h netinet/in.h shadow.h stdlib.h string.h sys/ioctl.h sys/param.h sys/socket.h time.h sys/time.h unistd.h])

if test "$ENABLE_DB_FNC" = "yes" ; then
    CFLAGS="$CFLAGS $DB_INC_DIR $ENABLE_DB"
    CXXFLAGS="$CFLAGS"
    AC_CHECK_HEADERS([db.h])
fi
if test "$ENABLE_MYSQL_FNC" = "yes" ; then
    CFLAGS="$CFLAGS $MYSQL_INC_DIR $ENABLE_MYSQL"
    CXXFLAGS="$CFLAGS"
    AC_CHECK_HEADERS([mysql/mysql.h])
fi

if test "$ENABLE_ZLIB_FNC" = "yes" ; then
    CFLAGS="$CFLAGS $ZLIB_INC_DIR $ENABLE_ZLIB"
    CXXFLAGS="$CFLAGS"
    AC_CHECK_HEADERS([zlib.h])
fi
if test "$ENABLE_LDAP_FNC" = "yes" ; then
    CFLAGS="$CFLAGS $LDAP_INC_DIR $ENABLE_LDAP"
    CXXFLAGS="$CFLAGS"
    AC_CHECK_HEADERS([ldap.h])
fi
if test "$ENABLE_SSL_FNC" = "yes" ; then
    CFLAGS="$CFLAGS $SSL_INC_DIR $ENABLE_SSL"
    CXXFLAGS="$CFLAGS"
    AC_CHECK_HEADERS([openssl/ssl.h])
fi
if test "$ENABLE_JPEG_FNC" = "yes" ; then
    CFLAGS="$CFLAGS $JPEG_INC_DIR $ENABLE_JPEG"
    CXXFLAGS="$CFLAGS"
    AC_CHECK_HEADERS([jpeglib.h])
fi
if test "$ENABLE_PNG_FNC" = "yes" ; then
    CFLAGS="$CFLAGS $PNG_INC_DIR $ENABLE_PNG"
    CXXFLAGS="$CFLAGS"
    AC_CHECK_HEADERS([png.h])
fi
if test "$ENABLE_OPENJPEG_FNC" = "yes" ; then
    CFLAGS="$CFLAGS $OPENJPEG_INC_DIR $ENABLE_OPENJPEG $ENABLE_OPENJPEG_VER"
    CXXFLAGS="$CFLAGS"
    AC_CHECK_HEADERS([openjpeg.h])
fi


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_HEADER_TIME
AC_STRUCT_TM


# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([localeconv gethostbyaddr gethostbyname getspnam memset select socket sqrt strcasecmp strncasecmp strstr])

AC_CONFIG_FILES([
    Makefile
    Lib/Makefile
    xLib/Makefile
    gLib/Makefile
    C++Lib/Makefile
    C++Lib/BasicLib/Makefile
    C++Lib/GraphLib/Makefile
    C++Lib/MathLib/Makefile
    C++Lib/BREPLib/Makefile
    C++Lib/ExtendLib/Makefile
    C++Lib/MeshLib/Makefile
    Tools/Makefile
])

AC_OUTPUT
AM_PROG_CC_C_O

