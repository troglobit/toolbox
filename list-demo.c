/* Simple demo of BSD sys/queue.h LIST API.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/queue.h>
#include "queue.h"

#define MAX_PEERS 10
#define MALLOC(p,sz)							\
	p = malloc(sz);							\
	if (!p)								\
		errx(1, "Failed allocating LIST peer: %s\n", strerror(errno));

typedef struct peer {
	LIST_ENTRY(peer) link;
	int data;
} peer_t;

typedef struct {
	LIST_HEAD(peer_list, peer) peers;
} torrent_t;


int main (void)
{
	int i;
	peer_t *entry, *tmp;
	torrent_t torrent;

	printf("Creating LIST list...\n");
	LIST_INIT(&torrent.peers);

	for (i = 0; i < MAX_PEERS; i++) {
		MALLOC(entry, sizeof(*entry));
		entry->data = i + 1;
		printf("  Creating entry %d\n", i + 1);
		LIST_INSERT_HEAD(&torrent.peers, entry, link);
	}

	printf("Linked list created. Iterating with foreach():\n");

	i = 0;
	LIST_FOREACH(entry, &torrent.peers, link) {
		printf("  Entry %d => data:%d\n", i++, entry->data);
	}

	i = 0;
#ifdef LIST_FOREACH_SAFE /* Actual BSD systems with working sys/queue.h */
	printf("Removing all entries, cleaning up using LIST_FOREACH_SAFE() ...\n");
	LIST_FOREACH_SAFE(entry, &torrent.peers, link, tmp) {
		LIST_REMOVE(entry, link);
		printf("  Entry %d => data:%d\n", i++, entry->data);
		free(entry);
	}
#else
	printf("Removing all entries, cleaning up using !LIST_EMPTY() ...\n");
	while (!LIST_EMPTY(&torrent.peers)) {
		entry = LIST_FIRST(&torrent.peers);
		LIST_REMOVE(entry, link);
		printf("  Entry %d => data:%d\n", i++, entry->data);
		free(entry);
	}
#endif
	printf("Done, exiting.\n");

	return 0;
}

/**
 * Local Variables:
 *  compile-command: "make list-demo && ./list-demo"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
