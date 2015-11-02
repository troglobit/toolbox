/* Simple multicast bus implementation
 *
 * Copyright (c) 2015  Joachim Nilsson <troglobit@gmail.com>
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
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <libuev/uev.h>

#define USE_LOOPBACK
#ifdef USE_LOOPBACK
# define MC_GROUP_DEFAULT "224.0.0.254"
# define MC_PORT_DEFAULT  1234
# define MC_TTL_DEFAULT   0
#else
# define MC_GROUP_DEFAULT "239.255.255.1"
# define MC_PORT_DEFAULT  10000
# define MC_TTL_DEFAULT   0
#endif
#define UNUSED(arg) arg __attribute__ ((unused))

struct sockaddr_in sa;
struct mbus_arg {
	int sd;
	FILE *fp;
};


static int socket_init(char *group, int port, int ttl)
{
	int sd, ret, flag = 1;
	struct ip_mreq imr;

	sd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
	if (sd < 0) {
		perror("Failed creating mbus socket");
		return -1;
	}

	ret = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
	if (ret < 0) {
		perror("Failed setsockopt()");
		close(sd);
		return -1;
	}

	memset(&sa, 0, sizeof(sa));
	sa.sin_family      = AF_INET;
	sa.sin_addr.s_addr = inet_addr(group);
	sa.sin_port        = htons(port);
	ret = bind(sd, (struct sockaddr *)&sa, sizeof(sa));
	if (ret < 0) {
		perror("Failed bind()");
		close(sd);
		return -1;
	}

	imr.imr_multiaddr.s_addr = inet_addr(group);
#ifdef USE_LOOPBACK
	imr.imr_interface.s_addr = htonl(INADDR_LOOPBACK);
#else
	imr.imr_interface.s_addr = htonl(INADDR_ANY);
#endif
	ret = setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr));
	if (ret < 0) {
		perror("Failed joining bus ");
		close(sd);
		return -1;
	}

	ret = setsockopt(sd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
	if (ret < 0) {
		perror("Failed setting socket TTL");
		close(sd);
		return -1;
	}
#ifdef USE_LOOPBACK
	struct in_addr addr;
	addr.s_addr = htonl(INADDR_LOOPBACK);
	ret = setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, &addr, sizeof(addr));
	if (ret < 0) {
		perror("Failed setting outbound multicast interface");
		close(sd);
		return -1;
	}
#endif
	int loop = 0;
	ret = setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
	if (ret < 0) {
		perror("Failed disabling multicast loop on socket");
		close(sd);
		return -1;
	}

	int all = 0;
	ret = setsockopt(sd, IPPROTO_IP, IP_MULTICAST_ALL, &all, sizeof(all));
	if (ret < 0) {
		perror("Failed disabling multicast all on socket");
		close(sd);
		return -1;
	}

	return sd;
}

static void read_socket(uev_t *w, void UNUSED(*arg), int UNUSED(events))
{
	char buf[512];
	size_t len;

	memset(buf, 0, sizeof(buf));
	len = recvfrom(w->fd, buf, sizeof(buf) - 1, 0, NULL, 0);
	if (len < 0) {
		perror("Failed recvfrom()");
		uev_exit(w->ctx);
		return;
	}

	buf[len + 1] = 0;
	puts(buf);
}

static void read_stdin(uev_t *w, void *arg, int UNUSED(events))
{
	char buf[512];
	size_t len;
	struct mbus_arg *marg = (struct mbus_arg *)arg;

	if (!fgets(buf, sizeof(buf), marg->fp)) {
		uev_exit(w->ctx);
		return;
	}

	len = sendto(marg->sd, buf, strlen(buf) - 1, 0, (struct sockaddr *)&sa, sizeof(sa));
	if (len != strlen(buf) - 1) {
		perror("Failed sendto()");
		uev_exit(w->ctx);
		return;
	}
}

/* This example implementation uses libuEv, so you need to
 * build that first and link to it using the Makefile. */
int main(void)
{
	int sd;
	char buf[512];
	size_t len;
	struct mbus_arg arg;
        uev_t     stdio_watcher, socket_watcher;
        uev_ctx_t ctx;
	
	sd = socket_init(MC_GROUP_DEFAULT, MC_PORT_DEFAULT, MC_TTL_DEFAULT);
	if (sd < 0)
		return 1;

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	arg.fp = stdin;
	arg.sd = sd;

	uev_init(&ctx);
        uev_io_init(&ctx, &stdio_watcher, read_stdin, &arg, STDIN_FILENO, UEV_READ);
        uev_io_init(&ctx, &socket_watcher, read_socket, &arg, sd, UEV_READ);

	return uev_run(&ctx, 0);
}

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
