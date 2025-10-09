#include "pnt.h"
#include <string.h>
#include <sys/socket.h>

struct errep *pnt_sendmsg(void *msg, size_t msglen, struct std_conn conn)
{
        struct errep *err;
        char *fnname = "pnt_sendmsg()";

        if (sendto(conn.socket, msg, msglen, 0, (struct sockaddr *) &conn.address, sizeof(struct sockaddr)) == -1) {
                ERREP(err, fnname, "error sending message to peer");
                return err;
        }
        ERREP(err, fnname, NULL);
        return err;
}

struct errep *pnt_recvmsg(void *msg, size_t msglen, struct std_conn conn)
{
        struct errep *err;
        char *fnname = "pnt_recvmsg()";

        if (recvfrom(conn.socket, msg, msglen, 0, NULL, NULL) == -1) {
                ERREP(err, fnname, "error sending message to peer");
                return err;
        }
        ERREP(err, fnname, NULL);
        return err;
}