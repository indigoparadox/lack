
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

struct socket* listener = NULL;

static int __init lack_init( void ) {
   int res = 0;
 
   printk( KERN_INFO "lack: opening socket...\n" );
   res = sock_create_kern(
      &init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &listener );

   return 0;
}

static void __exit lack_exit( void ) {
   if( NULL == listener ) {
      printk( KERN_WARNING "lack: socket not found...\n" );
      return;
   }
   if( NULL == listener->ops ) {
      printk( KERN_WARNING "lack: socket ops not found...\n" );
      return;
   }
   printk( KERN_INFO "lack: closing socket...\n" );
   listener->ops->release( listener );
   listener = NULL;
}

module_init( lack_init );
module_exit( lack_exit );

