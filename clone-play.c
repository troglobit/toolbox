/* Playing with clone(2)
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

static int callback(void *arg)
{
	arg_t *data = (arg_t *)arg;
	arg_t *foo  = (arg_t *)NULL;

	printf("child: got magic %d and ptr %p, stack-top %p\n",
	       data->magic, data->ptr, &data + sizeof(arg_t) + sizeof(void *));

	/* With CLONE_VM the child and parent share heap, but the below
	 * segfault cannot be handled */
	data->magic = 7331;
	data->ptr   = &data;

	/* Cause segfault! */
#if DO_SEGFAULT == 1
	foo->ptr   = data->ptr;
	printf("child: segfaulting failed!\n");
#endif

        return 42;
}

static void sigsegv_cb(int signo)
{
	warnx("Child caused segfault.");
}

int main(int argc, char *argv[])
{
	int   status = 0;
        char *stack, *stack_top;
        pid_t pid;

	signal(SIGSEGV, sigsegv_cb);

        stack = malloc(PAGE_SIZE);
        if (!stack)
                err(1, "malloc");
        stack_top = stack + PAGE_SIZE; /* Assume stack grows downward */

        pid = clone(callback, stack_top, SIGCHLD, &arg);
        if (-1 == pid)
                err(1, "clone");

	free(stack);
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
 *  compile-command: "make clone-play && ./clone-play"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
