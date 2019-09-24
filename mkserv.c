
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/net_namespace.h>
#include <linux/mutex.h>
#include <linux/kthread.h>

#include "mkserv.h"

MODULE_LICENSE( "GPL");
MODULE_AUTHOR( "indigoparadox" );
MODULE_DESCRIPTION( "A common TCP server framework for the kernel." );
MODULE_VERSION( "0.19.9" );

EXPORT_SYMBOL( mkserv_listen );
EXPORT_SYMBOL( mkserv_shutdown );

struct mkservice testsvc = { 0 };

int mkserv_listen( struct mkservice* service, int port ) {
   int res = 0;
   struct sockaddr_in listen_addr = { 0 };

   DECLARE_WAIT_QUEUE_HEAD( wq );

   allow_signal( SIGKILL | SIGTERM );

   /* Create the listener socket. */
   res = sock_create( AF_INET, SOCK_STREAM, IPPROTO_TCP, &(service->listener) );

   if( 0 > res ) {
      printk( KERN_ERR "unable to open mkserv socket\n" );
      goto cleanup;
   }

   /* Bind the listener to the given address. */
   listen_addr.sin_addr.s_addr = INADDR_ANY;
   listen_addr.sin_family = AF_INET;
   listen_addr.sin_port = htons( port );

   res = service->listener->ops->bind( service->listener,
      (struct sockaddr*)&listen_addr, sizeof( listen_addr ) );

   if( 0 > res ) {
      printk( KERN_ERR "unable to bind socket to port %d\n", port );
      mkserv_shutdown( service );
      goto cleanup;
   }

   /* Start listening. */
   res = service->listener->ops->listen( service->listener, MKSERV_BACKLOG );

   if( 0 > res ) {
      printk( KERN_ERR "unable to listen\n" );
      mkserv_shutdown( service );
      goto cleanup;
   }

   while( 1 ) {
      wait_event_timeout( wq, 0, 3 * HZ );

      if( kthread_should_stop() ) {
         res = 1;
         printk( KERN_INFO "stopping listen thread...\n" );
         goto cleanup;
      }

      if( signal_pending( current ) ) {
         res = 1;
         goto cleanup;
      }
   }

cleanup:

   return res;
}

int mkserv_shutdown( struct mkservice* service ) {
   if( NULL != service->listener ) {
      sock_release( service->listener );
      service->listener = NULL;
   }

   return 0;
}

static int __init mkserv_init( void ) {
/*
   int res = 0;
 
   printk( KERN_INFO "mkserv: opening socket...\n" );
   res = sock_create_kern(
      &init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &listener );
*/

   mkserv_listen( &testsvc, 4567 );

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

   mkserv_shutdown( &testsvc );
}

module_init( mkserv_init );
module_exit( mkserv_exit );

