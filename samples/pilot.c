#include "pnt.h"
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	struct errep *err;
	struct in_addr addr;
	struct std_conn conn;

	if (argc == 1) {
		fprintf(stderr, "Please provide an IP address.\n");
		return -1;
	}
	if (inet_pton(AF_INET, argv[1], &addr) != 1) {
		perror("inet_pton() err");
		return -1;
	}
	if ((err = pnt_traverse(addr, 0, &conn)) -> msg != NULL) {
		fprintf(stderr, "%s", ptools_format_errors(err));
		return -1;
	}
	return 0;
}
