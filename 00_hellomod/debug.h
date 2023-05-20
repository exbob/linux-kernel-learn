#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef DEBUG
    #define pr_fmt(fmt) KBUILD_MODNAME " [%s:%d - %s]: " fmt, __FILE__, __LINE__, __FUNCTION__
#else
    #define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#endif

#include <linux/printk.h>

#endif
