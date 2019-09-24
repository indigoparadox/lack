
#include "mkserv.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/net_namespace.h>

MODULE_LICENSE( "GPL");
MODULE_AUTHOR( "indigoparadox" );
MODULE_DESCRIPTION( "A lacking web admin tool for the kernel." );
MODULE_VERSION( "0.19.9" );

int lack_handler( struct mkservice* service );

struct mkservice lack_listen = {
   "lack", lack_handler, 0, 0 };

int lack_handler( struct mkservice* service ) {

   return 0;
}

static int __init lack_init( void ) {
 
   printk( KERN_INFO "lack: starting up...\n" );

   mkserv_listen( &lack_listen, 8045 );

   return 0;
}

static void __exit lack_exit( void ) {
   mkserv_shutdown( &lack_listen );
}

module_init( lack_init );
module_exit( lack_exit );

