#ifndef __PNT_H__
#define __PNT_H__

#include "ptools.h"
//TODO replace with ptools_unet
#include <netinet/in.h>

/**
 * Struct representing a standard connection between two peers behind NATted
 * networks.
 */
struct std_conn {
	socket_t		sock;
	struct sockaddr_in	address;
};

extern struct errep *pnt_traverse(struct in_addr, float delay, struct std_conn *results);

#endif //__PNT_H___
