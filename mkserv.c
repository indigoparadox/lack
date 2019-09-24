
#include "mkserv.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/net_namespace.h>
#include <linux/mutex.h>
#include <linux/kthread.h>

MODULE_LICENSE( "GPL");
MODULE_AUTHOR( "indigoparadox" );
MODULE_DESCRIPTION( "A common TCP server framework for the kernel." );
MODULE_VERSION( "0.19.9" );

EXPORT_SYMBOL( mkserv_listen );
EXPORT_SYMBOL( mkserv_shutdown );

int mkserv_accept( void* data ) {
   int res = 0;
   struct mkservice* service = (struct mkservice*)data;

   DECLARE_WAIT_QUEUE_HEAD( wq );
   
   while( 1 ) {
      wait_event_timeout( wq, 0, 3 * HZ );

      if( kthread_should_stop() ) {
         printk( KERN_INFO "mkserv: peacefully stopping accept thread...\n" );
         res = 0;
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

int mkserv_listen( struct mkservice* service, int port ) {
   int res = 0;
   struct sockaddr_in listen_addr = { 0 };

   allow_signal( SIGKILL | SIGTERM );

   printk( KERN_INFO "mkserv: opening listener on port %d", port );

   /* Create the listener socket. */
   res = sock_create(
      AF_INET, SOCK_STREAM, IPPROTO_TCP, &(service->listen_sock) );

   if( 0 > res ) {
      printk( KERN_ERR "mkserv: unable to open socket\n" );
      goto cleanup;
   }

   /* Bind the listener to the given address. */
   listen_addr.sin_addr.s_addr = INADDR_ANY;
   listen_addr.sin_family = AF_INET;
   listen_addr.sin_port = htons( port );

   res = service->listen_sock->ops->bind( service->listen_sock,
      (struct sockaddr*)&listen_addr, sizeof( listen_addr ) );

   if( 0 > res ) {
      printk( KERN_ERR "mkserv: unable to bind socket to port %d\n", port );
      mkserv_shutdown( service );
      goto cleanup;
   }

   /* Start listening. */
   res = service->listen_sock->ops->listen(
      service->listen_sock, MKSERV_BACKLOG );

   if( 0 > res ) {
      printk( KERN_ERR "mkserv: unable to listen\n" );
      mkserv_shutdown( service );
      goto cleanup;
   }

   service->accept_thd = kthread_run( mkserv_accept, service, service->name );

cleanup:

   return res;
}

int mkserv_shutdown( struct mkservice* service ) {
   int res = 0;

   if( NULL != service->accept_thd ) {
      printk( KERN_INFO "%s: stopping accept thread...\n", service->name );
      res = kthread_stop( service->accept_thd );
      if( res ) {
         printk( KERN_WARNING "%s: problem stopping accept thread\n",
            service->name );
      } else {
         printk( KERN_INFO "%s: stopped accept thread\n", service->name );
      }
   }

   if( NULL != service->listen_sock ) {
      printk( KERN_INFO "%s: closing socket\n", service->name );
      sock_release( service->listen_sock );
      service->listen_sock = NULL;
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

