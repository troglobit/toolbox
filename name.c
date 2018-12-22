/* Lookup the IP# from a DNS name - an example of getaddrinfo() et al
 *
 * Copyright (c) 2010  Joachim Nilsson <troglobit@vmlinux.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


int main(int argc, char *argv[])
{
	struct addrinfo *result, *rp;
	struct addrinfo hints;
	char host[NI_MAXHOST];
	int rc;

	if (argc < 2) {
		fprintf(stderr, "Usage: name HOST\n");
		return 1;
	}

	/* Obtain address(es) matching host/port */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow both IPv4 and IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */

	rc = getaddrinfo(argv[1], NULL, &hints, &result);
	if (rc) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
		return 1;
	}

	for (rp = result; rp; rp = rp->ai_next) {
		void *ptr;

		memset(host, 0, sizeof(host));
		switch (rp->ai_family) {
		case AF_INET:
			ptr = &((struct sockaddr_in *)rp->ai_addr)->sin_addr;
			break;

		case AF_INET6:
			ptr = &((struct sockaddr_in6 *)rp->ai_addr)->sin6_addr;
			break;

		default:
			fprintf(stderr, "Unknown address family\n");
			continue;
		}

		inet_ntop(rp->ai_family, ptr, host, rp->ai_addrlen);
		printf("%s sock_type %d\n", host, rp->ai_socktype);
	}

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
