
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE( "GPL");
MODULE_AUTHOR( "indigoparadox" );
MODULE_DESCRIPTION( "A lacking web admin tool for the kernel." );
MODULE_VERSION( "0.19.9" );

static int __init lack_init( void ) {
   return 0;
}
static void __exit lack_exit(void) {
}

module_init( lack_init );
module_exit( lack_exit );

