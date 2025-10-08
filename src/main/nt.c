#include "pnt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define NANOSEC		1000000
#define PNT_BINDPORT 	8668
#define KEYSTR		"PLEASE STOP LYING"

struct errep *pnt_traverse(struct in_addr addr, float millis, struct std_conn *res)
{
	struct errep *err;
	char *fnname = "pnt_traverse()";
	socket_t sock;
	struct sockaddr_in tobind, reply, dest;
	socklen_t siz = sizeof(struct sockaddr);
	struct timespec sleeptime;
	int klen = strlen(KEYSTR);
	word portnum;
	char buf[klen], tempbuf[24];

	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = millis * NANOSEC;
	memset(&tobind, 0, sizeof(struct sockaddr_in));
	tobind.sin_family = AF_INET;
	tobind.sin_port = htons(PNT_BINDPORT);
	dest.sin_family = AF_INET;
	memcpy(&dest.sin_addr, &addr, sizeof(dest.sin_addr));
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		ERREP(err, fnname, "socket for nat traversal could not be created");
		return err;
	}
	if (bind(sock, (struct sockaddr *) &tobind, sizeof(struct sockaddr)) == -1) {
		ERREP(err, fnname, "error binding our socket to the designated bindport");
		return err;
	}
	while (1) {
		portnum = 1024;
		while (portnum) {
			if (nanosleep(&sleeptime, NULL) == -1) {
				ERREP(err, fnname, "error having a snooze");
				return err;
			}
			dest.sin_port = htons(portnum++);
			if (sendto(sock, KEYSTR, klen, 0, (struct sockaddr *) &dest, siz) < klen) {
				ERREP(err, fnname, "error sending a message to our peer");
				return err;
			}
			fprintf(stdout, "Ping on its way to %s:%d\n", inet_ntop(AF_INET, &dest.sin_addr, tempbuf, sizeof(tempbuf)), ntohs(dest.sin_port));
		}
		if (recvfrom(sock, buf, sizeof(buf), MSG_DONTWAIT, (struct sockaddr *) &reply, &siz) == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				continue;
			} else {
				ERREP(err, fnname, "error recovering message from peer");
				return err;
			}
		}
		if (strcmp(KEYSTR, buf) == 0)
			break;
	}
	printf("%s\n", buf);
	res -> sock = sock;
        memcpy(&res -> address, &reply, sizeof(res -> address));
	ERREP(err, fnname, NULL);
	return err;
}
