
#ifndef MKSERV_H
#define MKSERV_H

#define MKSERV_BACKLOG 16

struct mkservice;

typedef int (*mkserv_handler)( struct mkservice* );

struct mkservice {
   const char name[20];
   mkserv_handler handle_func;

   struct socket* listen_sock;
   struct task_struct* accept_thd;
   int running;
};

int mkserv_listen( struct mkservice* service, int port );
int mkserv_shutdown( struct mkservice* service );

#endif /* MKSERV_H */

