/* POSIX Timers + pipe(2) => Portable select(2):able timers
 *
 * Copyright (c) 2017  Joachim Nilsson <troglobit@gmail.com>
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

#include <signal.h>
#include <string.h>		/* memset() */
#include <stdlib.h>		/* malloc() */
#include <unistd.h>		/* read()/write() */
#include <time.h>
#include "queue.h"

struct timer {
	LIST_ENTRY(timer) link;

	int             period;	/* period time in seconds */
	struct timespec timeout;

	void (*cb)(void *arg);
	void *arg;
};

static timer_t timer;
static int timerfd[2];
static LIST_HEAD(, timer) tl = LIST_HEAD_INITIALIZER();


static int set(struct timer *t, struct timespec *now)
{
	t->timeout.tv_sec  = now->tv_sec + t->period;
	t->timeout.tv_nsec = now->tv_nsec;
}

static int expired(struct timer *t, struct timespec *now)
{
	if (t->timeout.tv_sec  <= now->tv_sec &&
	    t->timeout.tv_nsec <= now->tv_nsec)
		return 1;

	return 0;
}

static struct timer *compare(struct timer *a, struct timer *b)
{
	if (a->timeout.tv_sec <= b->timeout.tv_sec) {
		if (a->timeout.tv_nsec <= b->timeout.tv_nsec)
			return a;

		return b;
	}

	return b;
}

/* Write to pipe to create an event for select() on SIGALRM */
static void sigalarm_handler(int signo)
{
	char buf[] = "!";

	(void)signo;
	if (write(timerfd[1], "!", 1) < 0)
		;
}

/*
 * Register signal pipe and callbacks
 */
int timer_init(void)
{
	struct sigaction sa;

	if (pipe(timerfd))
		return -1;

	sa.sa_handler = sigalarm_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGALRM, &sa, NULL);

	/* NULL means SIGALRM */
	if (timer_create(CLOCK_MONOTONIC, NULL, &timer))
		return -1;

	return 0;
}

/*
 * create periodic timer (seconds)
 */
int timer_add(int period, void (*cb)(void *), void *arg)
{
	struct timer *t;
	struct timespec now;

	if (clock_gettime(CLOCK_MONOTONIC, &now) < 0)
		return -1;

	t = malloc(sizeof(*t));
	if (!t)
		return -1;

	t->period = period;
	t->cb     = cb;
	t->arg    = arg;

	set(t, &now);

	LIST_INSERT_HEAD(&tl, t, link);

	return 0;
}

/*
 * start timers
 */
void timer_start(void)
{
	struct timer *next, *entry;
	struct timespec now;
	struct itimerspec it;

	if (LIST_EMPTY(&tl))
		return;

	clock_gettime(CLOCK_MONOTONIC, &now);
	next = LIST_FIRST(&tl);
	LIST_FOREACH(entry, &tl, link)
		next = compare(next, entry);

	memset(&it, 0, sizeof(it));
	it.it_value.tv_sec  = next->timeout.tv_sec - now.tv_sec;
	it.it_value.tv_nsec = 0;
	timer_settime(timer, 0, &it, NULL);
}

/*
 * callback for activity on pipe
 */
void timer_run(int sd)
{
	char dummy;
	struct timespec now;
	struct timer *entry, *next;

	if (read(sd, &dummy, 1) < 0)
		;

	clock_gettime(CLOCK_MONOTONIC, &now);
	LIST_FOREACH(entry, &tl, link) {
		if (expired(entry, &now)) {
			if (entry->cb)
				entry->cb(entry->arg);
			set(entry, &now);
		}
	}

	timer_start();
}

/***************************************** UNIT TEST **************************************/
#ifdef UNITTEST
#include <stdio.h>
#include <sys/select.h>

static void line(void *arg)
{
	(void)arg;
	fprintf(stderr, "\n");
}

static void hej(void *arg)
{
	(void)arg;
	fprintf(stderr, "Hej!");
}

static void timeout(void *arg)
{
	(void)arg;
	fprintf(stderr, "Timeout!");
	exit(0);
}

int main(void)
{
	int ret;

	timer_init();
	timer_add(1, line, NULL);
	timer_add(3, hej, NULL);
	timer_add(10, timeout, NULL);
	timer_start();

	while (1) {
		int nfds = timerfd[1] + 1;
		fd_set fds;

		FD_ZERO(&fds);
		FD_SET(timerfd[0], &fds);
		ret = select(nfds, &fds, NULL, NULL, NULL);
		if (FD_ISSET(timerfd[0], &fds))
			timer_run(timerfd[0]);
	}
	
	return 0;
}
#endif /* UNITTEST */

/**
 * Local Variables:
 *  compile-command: "CPPFLAGS=-DUNITTEST LDLIBS=-lrt make timers && ./timers"
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
