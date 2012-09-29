/* Very simple termios based progress bar
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

#include <stdio.h>
#include <stdlib.h>		/* atexit() */
#include <string.h>		/* strlen() */
#include <unistd.h>		/* usleep() */
#include "conio.h"

#define MAX_WIDTH    80
#define msleep(msec) usleep(msec * 1000)

xstatic char spinner(void)
{
	static int i = 0;
	char *states = "|/-\\";

	if (i >= strlen(states))
		i = 0;

	return states[i++];
}

progress(int percent, int max_width)
{
	int i, bar = percent * max_width / 100;

	printf("\r%3d%% %c [", percent, spinner());

	for (i = 0; i < max_width; i++) {
		if (i > bar)
			putchar(' ');
		else if (i == bar)
			putchar('>');
		else
			putchar('=');
	}

	printf("]");

}

static void bye(void)
{
	showcursor();
}

int main(int argc, char const *argv[])
{
	int i, loop, percent = 0;

	atexit(bye);
	hidecursor();

	while (percent <= 100) {
		for (i = 0; i < 10; i++) {
			progress(percent, MAX_WIDTH);
			msleep(10);
		}
		percent++;
		msleep(10);
	}
	puts("");

	return 0;
}

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
