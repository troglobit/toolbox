/* Simple spinner progress
 *
 * Copyright (c) 2016  Joachim Nilsson <troglobit@gmail.com>
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

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int quiet = 0;
int running = 1;

/* Esc[?25l (lower case L)    - Hide Cursor */
#define hidecursor()          fputs ("\e[?25l", stdout)
/* Esc[?25h (lower case H)    - Show Cursor */
#define showcursor()          fputs ("\e[?25h", stdout)

static void progress(void)
{
//	size_t num = 5;
//	const char *style = "|/-\\";
//	const char *style = ">v<^";
	size_t num = 6;
	const char *style = ".oOOo.";
	static unsigned int i = 0;

	if (quiet)
		return;

//	printf("%u %% %zd = %lu (%u / %zd = %lu)\n", i, num, i % num, i, num, i / num);

	if ((i % num) == 0)
		printf(".");
//	else
		printf("%c", style[i % num]);
	i++;
//	if (i % num)
		printf("\b");

	fflush(stdout);
}

static void quit(int signo __attribute__ ((unused)))
{
	running = 0;
}

int main(void)
{
	struct sigaction sa = {
		.sa_flags = SA_RESTART,
		.sa_handler = quit,
	};

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGHUP, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	hidecursor();
	while (running) {
		progress();
		usleep(100 * 1000);
	}
	showcursor();

	return 0;
}

/**
 * Local Variables:
 *  compile-command: "gcc -g -O2 -o spinner spinner.c && ./spinner"
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
