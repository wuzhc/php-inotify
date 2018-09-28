dnl $Id$
dnl config.m4 for extension inotify

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(inotify, for inotify support,
dnl Make sure that the comment is aligned:
dnl [  --with-inotify             Include inotify support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(inotify, whether to enable inotify support,
dnl Make sure that the comment is aligned:
[  --enable-inotify           Enable inotify support])

if test "$PHP_INOTIFY" != "no"; then
  dnl Write more examples of tests here...

  dnl # get library FOO build options from pkg-config output
  dnl AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  dnl AC_MSG_CHECKING(for libfoo)
  dnl if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists foo; then
  dnl   if $PKG_CONFIG foo --atleast-version 1.2.3; then
  dnl     LIBFOO_CFLAGS=`$PKG_CONFIG foo --cflags`
  dnl     LIBFOO_LIBDIR=`$PKG_CONFIG foo --libs`
  dnl     LIBFOO_VERSON=`$PKG_CONFIG foo --modversion`
  dnl     AC_MSG_RESULT(from pkgconfig: version $LIBFOO_VERSON)
  dnl   else
  dnl     AC_MSG_ERROR(system libfoo is too old: version 1.2.3 required)
  dnl   fi
  dnl else
  dnl   AC_MSG_ERROR(pkg-config not found)
  dnl fi
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBDIR, INOTIFY_SHARED_LIBADD)
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)

  dnl # --with-inotify -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/inotify.h"  # you most likely want to change this
  dnl if test -r $PHP_INOTIFY/$SEARCH_FOR; then # path given as parameter
  dnl   INOTIFY_DIR=$PHP_INOTIFY
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for inotify files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       INOTIFY_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$INOTIFY_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the inotify distribution])
  dnl fi

  dnl # --with-inotify -> add include path
  dnl PHP_ADD_INCLUDE($INOTIFY_DIR/include)

  dnl # --with-inotify -> check for lib and symbol presence
  dnl LIBNAME=inotify # you may want to change this
  dnl LIBSYMBOL=inotify # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $INOTIFY_DIR/$PHP_LIBDIR, INOTIFY_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_INOTIFYLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong inotify lib version or lib not found])
  dnl ],[
  dnl   -L$INOTIFY_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(INOTIFY_SHARED_LIBADD)

  PHP_NEW_EXTENSION(inotify, inotify.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
