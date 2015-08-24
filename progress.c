/* Very simple termios based progress bar
 *
 * Copyright (c) 2012, 2013  Joachim Nilsson <troglobit@gmail.com>
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

#include <limits.h>		/* INT_MAX */
#include <stdio.h>
#include <string.h>

#include "conio.h"

#define SPINNER_THROB   ".oOo"
#define SPINNER_PULSAR  ".oO°Oo."
#define SPINNER_ARROW   "v<^>"
#define SPINNER_STAR    ".oO@*"
#define SPINNER_DEFAULT "|/-\\"

static char spinner(char *style)
{
	size_t num;
	static unsigned int i = 0;

	if (!style)
		style = SPINNER_DEFAULT;
	num = strlen(style);

	return style[i++ % num]; /* % Number of states in style */
}

/**
 * progress - Advanced ASCII progress bar with spinner
 * @percent: Start first call with this set to 0, end with 100
 * @max_width: Max width of progress bar, in total characters.
 *
 * This function draws an advanced ASCII progressbar at the current
 * line.  It always start from the first column.
 *
 * The progress bar will hide the cursor if started with @percent 0 and
 * show it again at the end, when called with @percent 100.
 *
 * While being called with the same percentage the spinner will spin,
 * to show the user the process hasn't frozen.
 *
 * If the output TTY cannot interpret control characters, like \r, it is
 * advised to instead used the progress_simple() function.
 */
void progress(int percent, int max_width)
{
	int i, bar;

	/* Adjust for progress bar overhead */
	max_width -= 10;

	if (0 == percent)
		hidecursor();

	fprintf(stderr, "\r%3d%% %c [", percent, spinner(NULL));

	bar = percent * max_width / 100;
	for (i = 0; i < max_width; i++) {
		if (i > bar)
			fputc(' ', stderr);
		else if (i == bar)
			fputc('>', stderr);
		else
			fputc('=', stderr);
	}

	fprintf(stderr, "]");
	if (100 == percent) {
		showcursor();
		fputc('\n', stderr);
	}
}

#ifdef UNITTEST
#include <stdlib.h>		/* atexit() */
#include <unistd.h>		/* usleep() */

#define MAX_WIDTH    80
#define msleep(msec) usleep(msec * 1000)

static void bye(void)
{
	showcursor();
}

int main(void)
{
	int i, percent, block = 0, num = 85;

	atexit(bye);
	hidecursor();

	while (block < num) {
		percent = block * 100 / num;
		for (i = 0; i < 10; i++) {
			progress(percent, MAX_WIDTH);
			msleep(90);
		}
		block++;
		msleep(1);
	}
	progress(100, MAX_WIDTH);

	return 0;
}
#endif	/* UNITTEST */

/**
 * Local Variables:
 *  compile-command: "gcc -g -I../../include -o unittest -DUNITTEST progress.c && ./unittest"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
