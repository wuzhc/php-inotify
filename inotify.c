/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_inotify.h"

/* If you declare any globals in php_inotify.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(inotify)
*/

/* True global resources - no need for thread safety here */
static int le_inotify;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("inotify.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_inotify_globals, inotify_globals)
    STD_PHP_INI_ENTRY("inotify.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_inotify_globals, inotify_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_inotify_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_inotify_compiled)
{
    char *arg = NULL;
    size_t arg_len, len;
    zend_string *strg;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE)
    {
        return;
    }

    strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "inotify", arg);

    RETURN_STR(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_inotify_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_inotify_init_globals(zend_inotify_globals *inotify_globals)
{
  inotify_globals->global_value = 0;
  inotify_globals->global_string = NULL;
}
*/
/* }}} */

zend_class_entry inotify_ce;
zend_class_entry *inotify_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_watch, 0, 0, 1)
ZEND_ARG_INFO(0, targets) // zend_internal_arg_info宏定义
ZEND_END_ARG_INFO()

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

PHP_METHOD(inotify, __construct)
{
    int fd;
    fd = inotify_init();
    if (fd == -1)
    {
        return ;
    }
    zend_update_property_long(inotify_ce_ptr, getThis(), ZEND_STRL("fd"), fd);
}

PHP_METHOD(inotify, __destruct)
{

}

PHP_METHOD(inotify, addWatch)
{
    int i, fd, wd;
    zend_array *targets;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &targets) == FAILURE)
    {
        return ;
    }

    zval *self = getThis(); // getThis返回一个zval指针
    zval *fd_ptr;
    zval rv;
    fd_ptr = zend_read_property(Z_OBJCE_P(self), self, ZEND_STRL("fd"), 0 TSRMLS_CC, &rv);
    fd = Z_LVAL_P(fd_ptr);

    for (int i = 0; i < targets->nNumUsed; ++i)
    {
        wd = inotify_add_watch(fd,  Z_STRVAL(targets->arData[i].val), IN_ALL_EVENTS);
        if ( wd == -1)
        {
            return;
        }
        else
        {
            php_printf("watching %s using wd %d \n", Z_STRVAL(targets->arData[i].val), wd);
        }
    }


}

PHP_METHOD(inotify, run)
{
    int fd;
    char *p;
    ssize_t numRead;
    char buf[BUF_LEN];
    struct inotify_event *event;

    zval *self = getThis(); // getThis返回一个zval指针
    zval *fd_ptr;
    zval rv;
    fd_ptr = zend_read_property(Z_OBJCE_P(self), self, ZEND_STRL("fd"), 0 TSRMLS_CC, &rv);
    fd = Z_LVAL_P(fd_ptr);

    for (;;)
    {
        numRead = read(fd, buf, BUF_LEN);
        if (numRead == 0)
        {
            php_printf("read from inotify fd returned 0 \n");
            return ;
        }
        if (numRead == -1)
        {
            php_printf("read failed\n");
            return ;
        }

        php_printf("read %ld bytes from inotify fd\n", (long)numRead);
        for (p = buf; p < buf + numRead;)
        {
            event = (struct inotify_event *)p;
            displayInotifyEvent(event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }


}

void displayInotifyEvent(struct inotify_event *i)
{
    php_printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        php_printf("cookie =%4d; ", i->cookie);

    php_printf("mask = ");
    if (i->mask & IN_ACCESS)        php_printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        php_printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) php_printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   php_printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        php_printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        php_printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   php_printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       php_printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         php_printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        php_printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     php_printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    php_printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      php_printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          php_printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    php_printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       php_printf("IN_UNMOUNT ");
    php_printf("\n");

    if (i->len > 0)
        php_printf("        name = %s\n", i->name);
}

const zend_function_entry inotify_methods[] =
{
    PHP_ME(inotify, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(inotify, addWatch, arginfo_add_watch, ZEND_ACC_PUBLIC)
    PHP_ME(inotify, run, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(inotify, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_FE_END  /* Must be the last line in inotify_functions[] */
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(inotify)
{
    /* If you have INI entries, uncomment these lines
    REGISTER_INI_ENTRIES();
    */
    INIT_CLASS_ENTRY(inotify_ce, "inotify", inotify_methods); // 类名和类方法
    inotify_ce_ptr = zend_register_internal_class(&inotify_ce TSRMLS_CC); // 注册
    zend_declare_property_null(inotify_ce_ptr, ZEND_STRL("fd"), ZEND_ACC_PUBLIC TSRMLS_CC);
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(inotify)
{
    /* uncomment this line if you have INI entries
    UNREGISTER_INI_ENTRIES();
    */
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(inotify)
{
#if defined(COMPILE_DL_INOTIFY) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(inotify)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(inotify)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "inotify support", "enabled");
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
    */
}
/* }}} */

/* {{{ inotify_functions[]
 *
 * Every user visible function must have an entry in inotify_functions[].
 */
const zend_function_entry inotify_functions[] =
{
    PHP_FE(confirm_inotify_compiled,  NULL)   /* For testing, remove later. */
    PHP_FE_END  /* Must be the last line in inotify_functions[] */
};
/* }}} */

/* {{{ inotify_module_entry
 */
zend_module_entry inotify_module_entry =
{
    STANDARD_MODULE_HEADER,
    "inotify",
    inotify_functions,
    PHP_MINIT(inotify),
    PHP_MSHUTDOWN(inotify),
    PHP_RINIT(inotify),   /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(inotify), /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(inotify),
    PHP_INOTIFY_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_INOTIFY
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(inotify)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
