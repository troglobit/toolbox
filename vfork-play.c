/* Playing with vfork(2)
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

#define _GNU_SOURCE
#include <err.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#define DO_SEGFAULT 1

typedef struct {
	int magic;
	void *ptr;
} arg_t;

/* Stack is not shared between parent and child, only the heap. */
static arg_t arg = {
	.magic = 1337,
	.ptr = NULL
};

static int callback(arg_t *arg, int event, void *foo)
{
	arg_t *event_arg  = (arg_t *)foo;

	printf("child: got magic %d and ptr %p, with event %d and event arg %p\n",
	       arg->magic, arg->ptr, event, event_arg);

	/* With VFORK_VM the child and parent share heap, but the below
	 * segfault cannot be handled */
	arg->magic = 7331;
	arg->ptr   = &event_arg;

	/* Cause segfault! */
#if DO_SEGFAULT == 1
	event_arg->ptr = event_arg->ptr;
	printf("child: segfaulting failed!\n");
#endif

        return 42;
}

static void sigsegv_cb(int signo)
{
	warnx("Child caused segfault.");
	exit(-1);
}

int main(int argc, char *argv[])
{
	int   status = 0;
        pid_t pid;

	signal(SIGSEGV, sigsegv_cb);

        pid = vfork();
        if (-1 == pid)
                err(1, "vfork");
	if (!pid)
		return callback(&arg, 6137, NULL);
	
        if (waitpid(pid, &status, 0) == -1)
                err(1, "waitpid");

	if (WIFEXITED(status))
		printf("Child exited normally => %d\n", WEXITSTATUS(status));
	else if (WCOREDUMP(status))
		printf("Child crashed!\n");

        return 0;
}

/**
 * Local Variables:
 *  compile-command: "make vfork-play && ./vfork-play"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
