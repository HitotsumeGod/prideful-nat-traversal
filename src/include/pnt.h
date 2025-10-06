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
	struct sockaddr_in	address;
	//TODO add more state
};

extern struct errep *pnt_traverse_nat(struct sockaddr_in *address, struct std_conn **results);

#endif //__PNT_H___
