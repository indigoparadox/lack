
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/net_namespace.h>
#include <linux/mutex.h>

#include "mkserv.h"

MODULE_LICENSE( "GPL");
MODULE_AUTHOR( "indigoparadox" );
MODULE_DESCRIPTION( "A common TCP server framework for the kernel." );
MODULE_VERSION( "0.19.9" );

EXPORT_SYMBOL( mkserv_listen );

int mkserv_listen( struct socket** listen_sock_ptr, int port ) {
   int res = 0;
   struct sockaddr_in listen_addr = { 0 };

   DECLARE_WAIT_QUEUE_HEAD( wq );

   allow_signal( SIGKILL | SIGTERM );

   /* Create the listener socket. */
   res = sock_create_kern(
      &init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, listen_sock_ptr );

   if( 0 > res ) {
      printk( KERN_ERR "unable to open mkserv socket\n" );
      goto cleanup;
   }

   /* Bind the listener to the given address. */
   listen_addr.sin_addr.s_addr = INADDR_ANY;
   listen_addr.sin_family = AF_INET;
   listen_addr.sin_port = htons( port );

   res = (*listen_sock_ptr)->ops->bind( (*listen_sock_ptr),
      (struct sockaddr*)&listen_addr, sizeof( listen_addr ) );

   if( 0 > res ) {
      printk( KERN_ERR "unable to bind socket to port %d\n", port );
      goto cleanup;
   }

cleanup:

   return res;
}

static int __init mkserv_init( void ) {
/*
   int res = 0;
 
   printk( KERN_INFO "mkserv: opening socket...\n" );
   res = sock_create_kern(
      &init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &listener );
*/

   return 0;
}

static void __exit mkserv_exit( void ) {
   /*if( NULL == listener ) {
      printk( KERN_WARNING "mkserv: socket not found...\n" );
      return;
   }
   if( NULL == listener->ops ) {
      printk( KERN_WARNING "mkserv: socket ops not found...\n" );
      return;
   }
   printk( KERN_INFO "mkserv: closing socket...\n" );
   listener->ops->release( listener );
   listener = NULL;*/
}

module_init( mkserv_init );
module_exit( mkserv_exit );

