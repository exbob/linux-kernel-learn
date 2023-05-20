#include "debug.h"

#include <linux/init.h>
#include <linux/module.h>

static int __init hellomod_init(void)
{
    pr_info("hellomod init\n");
    pr_debug("Debug information\n");
    
    return 0;
}

static void __exit hellomod_exit(void)
{
    pr_info("hellomod exit\n");
}

module_init(hellomod_init);
module_exit(hellomod_exit);

MODULE_AUTHOR("Bob<gexbob@gmail.com>");
MODULE_DESCRIPTION("Linux kernel module demo");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
