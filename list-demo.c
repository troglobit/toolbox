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

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/queue.h> /* Linux/GLIBC version does not have _SAFE() macros! */
#include "queue.h"

#define MAX_PEERS 10
#define MALLOC(p,sz)							\
	p = malloc(sz);							\
	if (!p)								\
		err(1, "Failed allocating LIST peer");

/* *BSD queue.h API use 'struct peer' internally, code in main() use peer_t */
typedef struct peer {
	LIST_ENTRY(peer) link;
	int data;
} peer_t;

/* Simplify initialization, otherwise LIST_INIT() must be used at runtime. */
LIST_HEAD(, peer) peers = LIST_HEAD_INITIALIZER();


int main (void)
{
	int i;
	peer_t *entry, *tmp;

	printf("Creating LIST list...\n");

	for (i = 0; i < MAX_PEERS; i++) {
		MALLOC(entry, sizeof(*entry));
		entry->data = i + 1;
		printf("  Creating entry %d\n", i + 1);
		LIST_INSERT_HEAD(&peers, entry, link);
	}

	printf("Linked list created. Iterating with foreach():\n");

	i = 0;
	LIST_FOREACH(entry, &peers, link) {
		printf("  Entry %d => data:%d\n", i++, entry->data);
	}

	i = 0;

#ifdef LIST_FOREACH_SAFE /* Actual BSD systems with working sys/queue.h */
	printf("Removing all entries, cleaning up using LIST_FOREACH_SAFE() ...\n");
	LIST_FOREACH_SAFE(entry, &peers, link, tmp) {
		LIST_REMOVE(entry, link);
		printf("  Entry %d => data:%d\n", i++, entry->data);
		free(entry);
	}
#else
	printf("Removing all entries, cleaning up using !LIST_EMPTY() ...\n");
	while (!LIST_EMPTY(&peers)) {
		entry = LIST_FIRST(&peers);
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
