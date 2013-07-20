/* Simple demo of BSD sys/queue.h TAILQ API.
 *
 * Copyright (c) 2013  Joachim Nilsson <troglobit@gmail.com>
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
#define MALLOC(p,sz)							\
	p = malloc(sz);							\
	if (!p)								\
		errx(1, "Failed allocating TAILQ node: %s\n", strerror(errno));

typedef struct node {
	TAILQ_ENTRY(node) link;
	int data;
} node_t;

static TAILQ_HEAD(head, node) node_list = TAILQ_HEAD_INITIALIZER(node_list);

int main (void)
{
	int i;
	node_t *entry, *tmp;

	printf("Creating TAILQ list...\n");

	for (i = 0; i < MAX_NODES; i++) {
		MALLOC(entry, sizeof(*entry));
		entry->data = i + 1;
		printf("  Creating entry %d\n", i + 1);
		TAILQ_INSERT_TAIL(&node_list, entry, link);
	}

	printf("Linked list created. Iterating with foreach():\n");

	i = 0;
	TAILQ_FOREACH(entry, &node_list, link) {
		printf("  Entry %d => data:%d\n", i++, entry->data);
	}

	printf("Removing all entries, cleaning up...\n");

	i = 0;
#if TAILQ_FOREACH_SAFE  /* Actual BSD systems with working sys/queue.h */
	TAILQ_FOREACH_SAFE(entry, &node_list, link, tmp) {
		TAILQ_REMOVE(&node_list, entry, link);
		printf("  Entry %d => data:%d\n", i++, entry->data);
		free(entry);
	}
#else
	while (!TAILQ_EMPTY(&node_list)) {
		entry = TAILQ_FIRST(&node_list);
		TAILQ_REMOVE(&node_list, entry, link);
		printf("  Entry %d => data:%d\n", i++, entry->data);
		free(entry);
	}
#endif
	printf("Done, exiting.\n");

	return 0;
}

/**
 * Local Variables:
 *  compile-command: "make tailq-demo && ./tailq-demo"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "bsd"
 * End:
 */
