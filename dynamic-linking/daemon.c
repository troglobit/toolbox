/* Test of dlopen() for a plugin based services architecture for finit
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

#include <signal.h>
#include <stdio.h>
#include <unistd.h>		/* sleep() */

#include "plugin.h"

static int running = 1;

static void sigterm_handler(int signo)
{
	printf("Received signal %d, quitting ...", signo);
	running = 0;
}

static void setup_signals(void)
{
	signal(SIGINT,  sigterm_handler);
	signal(SIGTERM, sigterm_handler);
	signal(SIGQUIT, sigterm_handler);
}

int main(void)
{
	printf("Loading plugins...\n");
	load_plugins();

	printf("Hook #1\n");
	run_hooks(HOOK_POST_LOAD);

	printf("Hook #2\n");
	run_hooks(HOOK_PRE_SETUP);

	printf("Setup signals...\n");
	setup_signals();

	printf("Hook #3\n");
	run_hooks(HOOK_PRE_RUNLOOP);

	printf("Entering runloop...\n");
	while (running) {
		run_services();
		sleep(1);
	}

	printf("Exiting...\n");

	printf("Hook #4\n");
	run_hooks(HOOK_EXIT_CLEANUP);

	return 0;
}

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
