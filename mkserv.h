
#ifndef MKSERV_H
#define MKSERV_H

#define MKSERV_BACKLOG 16

struct mkservice {
   struct socket* listener;
};

int mkserv_listen( struct mkservice* service, int port );
int mkserv_shutdown( struct mkservice* service );

#endif /* MKSERV_H */

