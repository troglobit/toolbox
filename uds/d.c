/* Simple-D
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

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>

#include "api.h"

static int id = 1337;
extern int api_init(int server);

static int set_blocking(int fd)
{
	int arg = fcntl(fd, F_GETFL, NULL);

	if (arg == -1)
		return -1;

	return fcntl(fd, F_SETFL, arg & ~O_NONBLOCK);
}

int main(void)
{
	int sd = api_init(1);

	if (-1 == sd)
		err(1, "Failed starting server");
	set_blocking(sd);

	while (1) {
		int     c = accept(sd, NULL, NULL);
		api_t   a;
		ssize_t num;

		if (-1 == c) {
			warn("Failed creating client");
			continue;
		}

		warnx("New client connecting!");
		num = read(c, &a, sizeof(a));
		if (num <= 0) {
			close(c);
			if (num < 0)
				warn("Failed reading client request");
		
			continue;
		}

		warnx("Client sent cmd %d", a.cmd);
		switch (a.cmd) {
		case API_SUBSCRIBE_CMD:
			warnx("Hello %s, registering pid %d.", a.label, a.pid);
			a.id       = id++;
			a.next_ack = 42;
			break;

		case API_UNSUBSCRIBE_CMD:
			warnx("Goodbye pid %d.", a.pid);
			break;

		case API_KICK_CMD:
			warnx("How do you do pid %d(%d)?  ACK should be %d, is %d",
			       a.pid, a.id, a.next_ack, a.ack);
			a.next_ack = a.ack + 2;
			break;

		default:
			warnx("Invalid command %d", a.cmd);
			break;
		}

		if (write(c, &a, sizeof(a)) != sizeof(a))
			warn("Failed sending reply to client");

		close(c);
		warnx("Preparing for next client.");
	}

	return 0;
}

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
