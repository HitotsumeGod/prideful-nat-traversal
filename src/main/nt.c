#include "pnt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define PNT_BINDPORT 	8668

struct errep *pnt_traverse_invictus(struct in_addr address, struct std_conn *res)
{
	struct errep *err;
	char *fnname = "pnt_traverse_invictus()";
	socket_t sock;
	struct sockaddr_in dest, tobind, reply;
	char *msg = "PLEASE STOP LYING", buf[strlen(msg)];
	socklen_t siz = sizeof(struct sockaddr);
	
	dest.sin_family = AF_INET;
	dest.sin_port = PNT_BINDPORT;
	memcpy(&dest.sin_addr, &address, sizeof(dest.sin_addr));
	memset(&tobind, 0, sizeof(struct sockaddr_in));
	tobind.sin_family = AF_INET;
	tobind.sin_port = PNT_BINDPORT;
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		ERREP(err, fnname, "socket for nat traversal could not be created");
		return err;
	}
	if (bind(sock, (struct sockaddr *) &tobind, siz) == -1) {
		ERREP(err, fnname, "error binding our socket to the designated bindport");
		return err;
	}
        for (int i = 0; i < 3; i++) {
                if (sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &dest, siz) < strlen(msg)) {
                        ERREP(err, fnname, "error sending a message to our peer");
                        return err;
                }
                if (recvfrom(sock, buf, sizeof(buf), MSG_DONTWAIT, (struct sockaddr *) &reply, &siz) < sizeof(buf)) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                ERREP(err, fnname, "peer did not respond to ping attempts");
                                return err;
                        } else {
                                ERREP(err, fnname, "error recovering message from peer");
                                return err;
                        }
                }
        }
	res -> sock = sock;
        memcpy(&res -> address, &dest, sizeof(res -> address));
	res -> conn_strt = INVICTUS;
	ERREP(err, fnname, NULL);
	return err;
}

struct errep *pnt_traverse_severain(struct in_addr addr, struct std_conn *res)
{
	struct errep *err;
	char *fnname = "pnt_traverse_severain()";
	socket_t sock;
	struct sockaddr_in tobind, reply, dest;
	char *msg = "PLEASE STOP LYING", buf[strlen(msg)], tempbuf[24];
	socklen_t siz = sizeof(struct sockaddr);

	memset(&tobind, 0, sizeof(struct sockaddr_in));
	tobind.sin_family = AF_INET;
	tobind.sin_port = PNT_BINDPORT;
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
		dest.sin_port = 1024;
		while (dest.sin_port) {
			if (sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &dest, siz) < strlen(msg)) {
				ERREP(err, fnname, "error sending a message to our peer");
				return err;
			}
			fprintf(stdout, "Ping on its way to %s:%d\n", inet_ntop(AF_INET, &dest.sin_addr, tempbuf, sizeof(tempbuf)), dest.sin_port);
			dest.sin_port++;
		}
		if (recvfrom(sock, buf, sizeof(buf), MSG_DONTWAIT, (struct sockaddr *) &reply, &siz) < sizeof(buf)) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				continue;
			} else {
				ERREP(err, fnname, "error recovering message from peer");
				return err;
			}
		}
		for (int i = 0; i < sizeof(buf); i++)
			printf("%02X ", buf[i] & 0xFF);
		printf("\n");
		break;
	}
	res -> sock = sock;
        memcpy(&res -> address, &reply, sizeof(res -> address));
        res -> conn_strt = SEVERAIN;
	ERREP(err, fnname, NULL);
	return err;
}
