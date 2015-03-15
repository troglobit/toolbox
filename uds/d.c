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
#include <stdio.h>
#include <sys/socket.h>

#include "api.h"

static int id = 1337;
extern int api_init(int server);

int main(void)
{
	int sd = api_init(1);

	if (-1 == sd)
		err(1, "Failed starting server");

	while (1) {
		int     c = accept(sd, NULL, NULL);
		api_t   a;
		ssize_t num;

		printf("D: New client connecting!\n");
		num = read(c, &a, sizeof(a));
		if (num <= 0) {
			close(c);
			if (num < 0)
				err(1, "Failed reading client request");
		
			continue;
		}

		printf("D: Client sent cmd %d\n", a.cmd);
		switch (a.cmd) {
		case API_SUBSCRIBE_CMD:
			printf("D: Hello %s, pid %d.  Registering ...\n", a.label, a.pid);
			a.id       = id++;
			a.next_ack = 42;
			break;

		case API_UNSUBSCRIBE_CMD:
			printf("D: Goodbye pid %d ...\n", a.pid);
			break;

		case API_KICK_CMD:
			printf("D: How do you do id:%d, pid %d?  ACK should be %d, is %d ...\n",
			       a.id, a.pid, a.next_ack, a.ack);
			a.next_ack = a.ack + 2;
			break;

		default:
			printf("D: Invalid command.\n");
			break;
		}

		if (write(c, &a, sizeof(a)) != sizeof(a))
			warn("Failed sending reply to client");

		close(c);
		printf("D: Preparing for next client ...\n");
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
