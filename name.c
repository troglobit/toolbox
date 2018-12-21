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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


int main(int argc, char *argv[])
{
	struct addrinfo *result, *rp;
	struct addrinfo hints;
	char host[NI_MAXHOST], service[NI_MAXSERV];
	int sd, s;

	if (argc < 2) {
		fprintf(stderr, "Usage: name HOST\n");
		return 1;
	}

	/* Obtain address(es) matching host/port */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo(argv[1], NULL, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return 1;

		rp = result;
		if (rp == NULL) {               /* No address succeeded */
			fprintf(stderr, "No address\n");
			return 1;
		}

		if (getnameinfo(rp->ai_addr, rp->ai_addrlen, host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST)) {
			perror("Failed getnameinfo()");
		} else {
			printf("%s: %s\n", argv[1], host);
		}
		freeaddrinfo(result);           /* No longer needed */

		return 0;
	}

	return 1;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
