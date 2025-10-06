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
	enum {
		INVICTUS,
		SEVERAIN
	} conn_strt;
};

/**
 * Traverses a NATted network using the invictus strategy.
 *
 * The invictus strategy exploits the fact that many consumer-grade NAT devices
 * will attempt to create a NAT mapping with the 'outer port' (the port on the
 * NAT device open to the network, from which traffic will flow and be
 * received) having the same number as the 'inner port' (the port from which
 * the traffic originally flowed, located on a host within the NATted subnet).
 * This allows the strategy, when employed by a peer, to predict the NAT
 * mapping that will be present on both its own *and* its peer's NAT devices.
 * Thus, this function attempts to establish communication between the peers
 * via this predicted mapping.
 * @param address the IPv4 address for the NAT device of the target host
 * @param results a double-pointer buffer to which the results of the operation
 * will be passed.
 * @return a struct errep pointer to be used for debugging
 */
extern struct errep *pnt_traverse_invictus(struct in_addr *address, struct std_conn **results);
extern struct errep *pnt_traverse_severain(struct sockaddr_in *address, struct std_conn **results);

#endif //__PNT_H___
