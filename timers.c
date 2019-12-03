/* POSIX Timers + pipe(2) => Portable poll(2):able UNIX timers
 *
 * Copyright (c) 2017-2019  Joachim Nilsson <troglobit@gmail.com>
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
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <string.h>		/* memset() */
#include <stdlib.h>		/* malloc() */
#include <unistd.h>		/* read()/write() */
#include <time.h>
#include "queue.h"

struct timer {
	LIST_ENTRY(timer) tmr_link;

	int      tmr_period;	/* period time in seconds */
	time_t   tmr_timeout;

	void   (*tmr_cb)(void *arg);
	void    *tmr_arg;
};

static LIST_HEAD(, timer) tmr_head = LIST_HEAD_INITIALIZER();

static struct timespec timer_now;
static int timer_fd[2];

/*
 * create periodic timer (seconds)
 */
int timer_add(int period, void (*cb)(void *), void *arg)
{
	struct timer *tmr;

	tmr = calloc(1, sizeof(*tmr));
	if (!tmr)
		return -1;

	tmr->tmr_period = period;
	tmr->tmr_cb     = cb;
	tmr->tmr_arg    = arg;

	LIST_INSERT_HEAD(&tmr_head, tmr, tmr_link);

	return 0;
}

static int __timer_start(void)
{
	struct timer *next, *tmr;
	time_t sec;

	LIST_FOREACH(tmr, &tmr_head, tmr_link) {
		if (tmr->tmr_timeout == 0)
			tmr->tmr_timeout = timer_now.tv_sec + tmr->tmr_period;
	}

	next = LIST_FIRST(&tmr_head);
	LIST_FOREACH(tmr, &tmr_head, tmr_link) {
		if (next->tmr_timeout > tmr->tmr_timeout)
			next = tmr;
	}

	sec = next->tmr_timeout - timer_now.tv_sec;
	if (sec <= 0)
		sec = 1;

	return alarm(sec);
}

/*
 * start timers
 */
int timer_start(void)
{
	if (LIST_EMPTY(&tmr_head))
		return -1;

	if (clock_gettime(CLOCK_MONOTONIC, &timer_now) < 0)
		return -1;

	return __timer_start();
}

/*
 * callback for activity on pipe
 */
void timer_run(int sd)
{
	struct timer *tmr;
	char dummy;

	(void)read(sd, &dummy, 1);

	clock_gettime(CLOCK_MONOTONIC, &timer_now);
	LIST_FOREACH(tmr, &tmr_head, tmr_link) {
		if (tmr->tmr_timeout > timer_now.tv_sec)
			continue;

		if (tmr->tmr_cb)
			tmr->tmr_cb(tmr->tmr_arg);
		tmr->tmr_timeout = 0;
	}

	__timer_start();
}

/*
 * Write to pipe to create an event on SIGALRM
 */
static void sigalarm_handler(int signo)
{
	(void)signo;
	(void)write(timer_fd[1], "!", 1);
}

/*
 * register signal pipe and callback
 */
int timer_init(void (**cb)(int))
{
	struct sigaction sa;

	if (pipe(timer_fd))
		return -1;

	sa.sa_handler = sigalarm_handler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGALRM, &sa, NULL)) {
		close(timer_fd[0]);
		close(timer_fd[1]);
		return -1;
	}

	*cb = timer_run;
	return timer_fd[0];
}

/*
 * deregister signal pipe and callbacks
 */
int timer_exit(int fd)
{
	struct timer *tmr, *tmp;

	if (fd != timer_fd[0])
		return -1;

	close(timer_fd[0]);
	close(timer_fd[1]);

	LIST_FOREACH_SAFE(tmr, &tmr_head, tmr_link, tmp) {
		LIST_REMOVE(tmr, tmr_link);
		free(tmr);
	}

	return 0;
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
	int *forever = (int *)arg;

	fprintf(stderr, "Timeout!");
	*forever = 0;
}

int main(void)
{
	struct pollfd pfd[1];
	void (*cb)(int);
	int forever = 1;
	int pnum;
	int fd;

	fd = timer_init(&cb);
	if (-1 == fd)
		err(1, "Failed initializing timers");

	printf("Starting, timer fd: %d\n", fd);
	timer_add(1, line, NULL);
	timer_add(3, hej, NULL);
	timer_add(11, timeout, &forever);
	timer_start();

	pfd[0].fd = fd;
	pfd[0].events = POLLIN;
	pnum = 1;

	while (forever) {
		int i, rc;

		rc = poll(pfd, 1, -1);
		if (rc == -1) {
			if (EINTR == errno)
				continue;

			err(1, "Failed poll()");
		}

		/* Find active fd and its callback */
		for (i = 0; i < pnum; i++) {
			if (pfd[i].revents & POLLIN)
				cb(pfd[i].fd);
		}
	}

	return timer_exit(fd);
}
#endif /* UNITTEST */

/**
 * Local Variables:
 *  compile-command: "CFLAGS='-pg -W -Wall -Wextra' CPPFLAGS=-DUNITTEST make timers && ./timers"
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
