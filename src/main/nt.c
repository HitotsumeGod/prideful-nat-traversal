#include "pnt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define PNT_BINDPORT 	8668

struct errep *pnt_traverse_nat(struct sockaddr_in *dest, struct std_conn **res)
{
	struct errep *err;
	char *fnname = "pnt_traverse_nat()";
	struct std_conn *link;
	int sock;
	struct sockaddr_in us, reply;
	char buf[1], *msg = "PLEASE STOP LYING";
	int msglen = strlen(msg);
	bool received = false;
	socklen_t siz = sizeof(struct sockaddr);

	memset(&us, 0, sizeof(struct sockaddr_in));
	us.sin_family = AF_INET;
	us.sin_port = PNT_BINDPORT;
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		ERREP(err, fnname, "socket for nat traversal could not be created");
		return err;
	}
	if (bind(sock, (struct sockaddr *) &us, sizeof(struct sockaddr)) == -1) {
		ERREP(err, fnname, "error binding our socket to the designated bindport");
		return err;
	}
	while (!received) {
		if (sendto(sock, msg, msglen, 0, (struct sockaddr *) dest, siz) < msglen) {
			ERREP(err, fnname, "error sending a message to our peer");
			return err;
		}
		if (recvfrom(sock, buf, sizeof(buf), MSG_DONTWAIT, (struct sockaddr *) &reply, &siz) < sizeof(buf)) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				dest -> sin_port++;
				continue;
			} else {
				ERREP(err, fnname, "error recovering message from peer");
				return err;
			}
		}
		received = true;
	}
	printf("%s\n", "Received message from peer!");
	close(sock);
	ERREP(err, fnname, NULL);
	return err;
}
