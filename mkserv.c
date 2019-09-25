
#include "mkserv.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/net_namespace.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <net/request_sock.h>
#include <net/inet_connection_sock.h>

MODULE_LICENSE( "GPL");
MODULE_AUTHOR( "indigoparadox" );
MODULE_DESCRIPTION( "A common TCP server framework for the kernel." );
MODULE_VERSION( "0.19.9" );

EXPORT_SYMBOL( mkserv_listen );
EXPORT_SYMBOL( mkserv_shutdown );

int mkserv_accept( void* data ) {
   int res = 0;
   struct mkservice* service = (struct mkservice*)data;
   struct inet_connection_sock* isock = NULL;
   struct socket* new_sock = NULL;

   DECLARE_WAITQUEUE( wq, current );
   
   while( service->running ) {

      if( kthread_should_stop() ) {
         service->running = 0;
         break;
      }

      /* Check the connection queue to see if there are new incoming. */
      if( reqsk_queue_empty( &(isock->icsk_accept_queue) ) ) {
         /* Nothing waiting, so go to sleep. */
         add_wait_queue( &(service->listen_sock->sk->sk_wq->wait), &wq );
         __set_current_state( TASK_INTERRUPTIBLE );
         schedule_timeout( HZ );

         /* Resuming from sleep, try again. */
         __set_current_state( TASK_RUNNING );
         remove_wait_queue( &(service->listen_sock->sk->sk_wq->wait), &wq );
         continue;
      }

      /* Create a socket to accept the connection. */
      res = sock_create( AF_INET, SOCK_STREAM, IPPROTO_TCP, &new_sock );
      if( 0 > res ) {
         printk( KERN_ERR "%s: error creating server socket\n", service->name );
         continue;
      }

      /* Accept the connection to a new socket. */
      printk( KERN_INFO "%s: accepting connection...\n", service->name );
      res = service->listen_sock->ops->accept( service->listen_sock,
         new_sock, O_NONBLOCK );
      if( 0 > res ) {
         printk( KERN_ERR "%s: error accepting connection\n", service->name );
         sock_release( new_sock );
         continue;
      }
   }

   printk( KERN_INFO "mkserv: peacefully stopping accept thread...\n" );

   sock_release( new_sock );
   kfree( new_sock );

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

   /* Start processing thread. */
   service->running = 1;
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

