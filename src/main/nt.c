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
#define MAX_SECRET_LEN  24

char *pnt_hi = "PNT_PEER_HELLO";
char *pnt_ack = "PNT_PEER_ACK";

struct errep *pnt_traverse(struct in_addr addr, char *pass, float millis, struct std_conn *res)
{
	struct errep *err;
	char *fnname = "pnt_traverse()";
	socket_t sock;
	struct sockaddr_in tobind, reply, dest;
	socklen_t siz = sizeof(struct sockaddr);
	struct timespec sleeptime;
	word portnum;
	char hi_msg[strlen(pnt_hi) + strlen(pass) + 1], ack_msg[strlen(pnt_ack) + strlen(pass) + 1], *buf;
        int buflen;
	#ifdef DEBUG
		char tempbuf[24];
	#endif

        if (!pass || strlen(pass) > MAX_SECRET_LEN) {
                ERREP(err, fnname, "secret phrase was either NULL or too large");
                return err;
        }
        buflen = sizeof(hi_msg) > sizeof(ack_msg) ? sizeof(hi_msg) : sizeof(ack_msg);
        if ((buf = malloc(sizeof(char) * buflen)) == NULL) {
                ERREP(err, fnname, "could not allocate memory for buffer");
                return err;
        }
	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = millis * NANOSEC;
        snprintf(hi_msg, sizeof(hi_msg), "%s:%s", pnt_hi, pass);
        snprintf(ack_msg, sizeof(ack_msg), "%s:%s", pnt_ack, pass);
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
	//core connection-negotiation loop
	while (1) {
		portnum = 1024;
		while (portnum) {
			if (nanosleep(&sleeptime, NULL) == -1) {
				ERREP(err, fnname, "error having a snooze");
				return err;
			}
			dest.sin_port = htons(portnum++);
			if (sendto(sock, hi_msg, sizeof(hi_msg), 0, (struct sockaddr *) &dest, siz) == -1) {
				ERREP(err, fnname, "error sending hi message to our peer");
				return err;
			}
			#ifdef DEBUG
				fprintf(stdout, "Sent hello to %s:%d\n", inet_ntop(AF_INET, &dest.sin_addr, tempbuf, sizeof(tempbuf)), ntohs(dest.sin_port));
			#endif
		}
		if (recvfrom(sock, buf, buflen, MSG_DONTWAIT, (struct sockaddr *) &reply, &siz) == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				continue;
			} else {
				ERREP(err, fnname, "error recovering hi message from peer");
				return err;
			}
		}
		if (strcmp(hi_msg, buf) == 0) {
			if (sendto(sock, ack_msg, sizeof(ack_msg), 0, (struct sockaddr *) &reply, siz) == -1) {
				ERREP(err, fnname, "error sending ACK message to our peer");
				return err;
			}
			#ifdef DEBUG
				fprintf(stdout, "Sent ACK to %s:%d\n", inet_ntop(AF_INET, &reply.sin_addr, tempbuf, sizeof(tempbuf)), ntohs(reply.sin_port));
			#endif
			if (recvfrom(sock, buf, buflen, 0, NULL, NULL) == -1) {
				ERREP(err, fnname, "error recovering ACK message from peer");
				return err;
			}
			if (strcmp(ack_msg, buf) == 0)
				break;
		} else if (strcmp(ack_msg, buf) == 0) {
			if (sendto(sock, ack_msg, sizeof(ack_msg), 0, (struct sockaddr *) &reply, siz) == -1) {
				ERREP(err, fnname, "error sending ACK message to our peer");
				return err;
			}
			break;
		}
	}
	printf("%s\n", buf);
        free(buf);
	res -> sock = sock;
        memcpy(&res -> address, &reply, sizeof(res -> address));
	ERREP(err, fnname, NULL);
	return err;
}

//suitable for passing to pthread_create
void *pnt_keepalive(void *std_conn)
{
        struct errep *err;
        char *fnname = "pnt_keepalive()";
        struct timespec sleeptime;
        struct std_conn *conn = (struct std_conn *) std_conn;

        sleeptime.tv_sec = 4;
	sleeptime.tv_nsec = 0;
        while (1) {
                if (nanosleep(&sleeptime, NULL) == -1) {
                        ERREP(err, fnname, "error sleeping keepalive function");
                        ptools_format_errors(err);
                        return;
                }
                if ((err = pnt_sendmsg(*conn)) -> msg != NULL) {
                        ERREP(err -> next, fnname, "error sending keepalive message to peer");
                        ptools_format_errors(err);
                        return;
                }
        }
}