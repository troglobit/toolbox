/* API
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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "api.h"

extern char *__progname;
 
static int fd_is_valid(int fd)
{
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

/* Hidden API */
int api_init(int server)
{
	int sd;
	struct sockaddr_un sun = {
		.sun_family = AF_UNIX,
		.sun_path   = API_PATH,
	};

	sd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (-1 == sd)
		return -1;

	if (server) {
		remove(API_PATH);

		if (-1 == bind(sd, (struct sockaddr*)&sun, sizeof(sun)))
			goto error;

		if (-1 == listen(sd, 10))
			goto error;
	} else {
		if (connect(sd, (struct sockaddr*)&sun, sizeof(sun)) == -1)
			goto error;
	}

	return sd;

error:
	close(sd);
	return -1;
}

static int api_do(int cmd, int id, char *label, int timeout, int ack, int *next)
{
	int sd;
	api_t a = {
		.cmd     = cmd,
		.pid     = getpid(),
		.timeout = timeout,
	};
	
	sd = api_init(0);
	if (-1 == sd)
		return errno;

		printf("API: Got cmd %d\n", cmd);
	if (API_SUBSCRIBE_CMD == cmd) {
		if (!label || !label[0])
			label = __progname;
		strncpy(a.label, label, sizeof(a.label));
	} else {
		a.id  = id;
		a.ack = ack;
	}

	printf("API: Sending to D ...\n");
	if (write(sd, &a, sizeof(a)) != sizeof(a))
		goto error;

	printf("API: Receiving from D ...\n");
	if (read(sd, &a, sizeof(a)) != sizeof(a))
		goto error;

	*next = a.next_ack;
	printf("API: All OK, next ACK %d!\n", *next);

	close(sd);

	if (API_SUBSCRIBE_CMD == cmd)
		return a.id;

	return 0;
error:
	printf("API: ERROR!\n");
	close(sd);
	return -errno;
}

int api_subscribe(char *label, int timeout, int *next_ack)
{
	return api_do(API_SUBSCRIBE_CMD, -1, label, timeout, -1, next_ack);
}

int api_kick(int id, int timeout, int ack, int *next_ack)
{
	return api_do(API_KICK_CMD, id, NULL, timeout, ack, next_ack);
}

int api_unsubscribe(int id, int ack)
{
	return api_do(API_UNSUBSCRIBE_CMD, id, NULL, -1, ack, &ack);
}


/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
