/* Simple demo of BSD sys/queue.h linked lists API.
 *
 * Copyright (c) 2012  Joachim Nilsson <troglobit@gmail.com>
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
#include <sys/queue.h>

#define MAX_NODES 10
#define MALLOC(p,sz) p = malloc(sz); if (!p) errx(1, "Failed allocating RAM: %s\n", strerror(errno));
typedef struct node {
	LIST_ENTRY(node) link;
	int data;
} node_t;

LIST_HEAD(, node) node_list = LIST_HEAD_INITIALIZER();

int main (void)
{
	int i;
	node_t *entry, *tmp;

//	LIST_INIT(&node_list);

	printf("Creating linked list...\n");

	for (i = 0; i < MAX_NODES; i++) {
		MALLOC(entry, sizeof(*entry));
		entry->data = i + 1;
		printf("  Creating entry %d\n", i);
		LIST_INSERT_HEAD(&node_list, entry, link);
	}

	printf("Linked list created. Iterating with foreach():\n");

	i = 0;
	LIST_FOREACH(entry, &node_list, link) {
		printf("  Entry %d => data:%d\n", i++, entry->data);
	}

	printf("Removing all entries, cleaning up...\n");

	i = 0;
#if LIST_FOREACH_SAFE  /* Actual BSD systems with working sys/queue.h */
	LIST_FOREACH_SAFE(entry, &node_list, link, tmp) {
	while (node_list.lh_first != NULL) {
		LIST_REMOVE(entry, link);
		printf("  Entry %d => data:%d\n", i++, entry->data);
		free(entry);
	}
#else
	while (!LIST_EMPTY(&node_list)) {
		entry = LIST_FIRST(&node_list);
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
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "bsd"
 * End:
 */
