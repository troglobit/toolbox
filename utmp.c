/* Investigate UNIX accounting (utmp/wtmp) db
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

#include <err.h>
#include <pwd.h>
#include <paths.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>

static int parse(char *file)
{
	time_t sec;
	struct utmp *ut;
	struct tm *sectm;

	int pid;
	char id[sizeof(ut->ut_id)], user[sizeof(ut->ut_user)], when[80];

	printf("%s =============================================================================================\n", file);
	utmpname(file);

	setutent();
	while ((ut = getutent())) {
		memset(id, 0, sizeof(id));
		strncpy(id, ut->ut_id, sizeof(ut->ut_id));

		memset(user, 0, sizeof(user));
		strncpy(user, ut->ut_user, sizeof(ut->ut_user));

		sec = ut->ut_tv.tv_sec;
		sectm = localtime(&sec);
		strftime(when, sizeof(when), "%F %T", sectm);

		pid = ut->ut_pid;

		switch (ut->ut_type) {
		case EMPTY:
			break;

		case RUN_LVL:
			//printf("Runlevel %c, oldlevel %c\n", pid % 0xFF, pid / 0xFF ?: 'N');
			break;

		case BOOT_TIME:
			//printf("reboot %s\n", when);
			break;

		case NEW_TIME:
			//printf("New time\n");
			break;

		case OLD_TIME:
			//printf("Old time\n");
			break;

		case INIT_PROCESS:
			//printf("Init process %d line %s id %s user %s host %s\n", ut->ut_pid, ut->ut_line, id, ut->ut_user, ut->ut_host);
			break;

		case LOGIN_PROCESS:
			//printf("Login process %d line %s id %s user %s host %s\n", ut->ut_pid, ut->ut_line, id, ut->ut_user, ut->ut_host);
			break;

		case USER_PROCESS:
			//printf("User process %d line %s id %s user %s host %s\n", ut->ut_pid, ut->ut_line, id, ut->ut_user, ut->ut_host);
			break;

		case DEAD_PROCESS:
			//printf("Dead process\n");
			break;

		case ACCOUNTING:
			//printf("Accounting (unused)\n");
			break;

		default:
			//printf("Unknown\n");
			break;
		}

		printf("[%d] [%05d] [%-4.4s] [%-8.8s] [%-12.12s] [%-20.20s] [%-15.15s] [%-19.19s]\n",
		       ut->ut_type, pid, id, user, ut->ut_line, ut->ut_host, "0.0.0.0", when);
	}
	endutent();

	return 0;
}

int main(void)
{
	parse(_PATH_UTMP);
	parse(_PATH_WTMP);

	return 0;
}
