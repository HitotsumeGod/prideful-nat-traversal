#include "pnt.h"
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
	struct errep *err;
	struct in_addr addr;
	struct std_conn conn;

	char *host = "64.66.67.44";
	inet_pton(AF_INET, host, &addr);
	if ((err = pnt_traverse_severain(addr, &conn)) -> msg != NULL) {
		fprintf(stderr, "%s", ptools_format_errors(err));
		return -1;
	}
	return 0;
}
