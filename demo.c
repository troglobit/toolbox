/* Demo for TroglOS
 *                    ______
 *        .-------.  /\     \
 *       /   o   /| /o \  o  \
 *      /_______/o|/   o\_____\
 *      | o     | |\o   /o    /
 *      |   o   |o/ \ o/  o  /
 * jgs  |     o |/   \/____o/
 *      '-------'                   
 */

#include <ctype.h>
#include <poll.h>
#include <stdlib.h>		/* atexit(), rand() */
#include <stdio.h>
#include <string.h>		/* strlen() */
#include <termios.h>
#include <unistd.h>		/* usleep() */

#define msleep(msec) usleep(msec * 1000)
#define DRAW_DELAY   10000
#define SCREEN_WIDTH 132
#include "../libite/conio.h"

const char *logo[] = {
	"                                                               ______\n",
	"                          __                       .-------.  /\\     \\\n",
	"                         |  |                     /   o   /| /o \\  o  \\\n",
	" _______                 |  |_______ _______     /_______/o|/   o\\_____\\\n",
	"|       .----.-----.-----|  |   _   |   _   |    | o     | |\\o   /o    /\n",
	"|\e[1m.\e[0m|   | |   _|  _  |  _  |  |\e[1m.\e[0m  |   |   1___|    |   o   |o/ \\ o/  o  /\n",
	"'-|\e[1m.\e[0m  |-|__| |_____|___  |__|\e[1m.\e[0m  |   |____   |    |     o |/   \\/____o/\n",
	"  |\e[1m:\e[0m  |            |_____|  |\e[1m:\e[0m  1   |\e[1m:\e[0m  1   |    '-------'\n",
	"  |\e[1m::.\e[0m| Chaos Rel.          |\e[1m::.. .\e[0m |\e[1m::.. .\e[0m |    Troglobit Software Inc.\n",
	"  '---'                     '-------'-------'    \e[4mhttp://troglobit.com\e[0m\n",
	NULL
};

int screen_rows  = 24;
int screen_cols  = 80;
int screen_colo  = 1;
int screen_rowo  = 1;
int logo_cols    = 0;
int logo_rows    = 0;

static int lenparse(const char *line)
{
	int len = 0, esc = 0;

	for (int i = 0; line[i]; i++) {
		if (line[i] == '\n')
			break;

		if (esc || line[i] == '\e') {
			esc = 1;

			if (isalpha(line[i]))
				esc = 0;
		} else
			len++;
	}

	return len;
}

int logo_init(int center)
{
	int i, len;

	for (i = 0; logo[i]; i++) {
		len = lenparse(logo[i]);

		if (len > logo_cols)
			logo_cols = len;
	}

	if (i > logo_rows)
		logo_rows = i;

	if (screen_cols < logo_cols)
		return 1;

	if (center) {
		screen_rowo = (screen_rows - logo_rows) / 2;
		screen_colo = (screen_cols - logo_cols) / 2;
	}

	return 0;
}

static void cursor(int x, int y)
{
	gotoxy(x, y);
	fputs("   \e[7m   \e[0m   ", stdout);
}

int logo_show(int rows, int cols, int center)
{
	int l = 0;
	int x, y, i = 0, j;

	if (logo_init(center))
		return 1;

	hidecursor();
	clrscr();
	for (y = screen_rowo; y < screen_rows && logo[l]; y++) {
	     for (j = 0; j < DRAW_DELAY; j++) {
		     x = (rand() % logo_cols) + screen_colo;
		     if (x > 5)
			     x -= 5;

		     cursor(x, y);
	     }

	     gotoxy(screen_colo, y);
	     delline();
	     if (logo[l])
		     puts(logo[l++]);
	}
	showcursor();

	return 0;
}

char *svc[] = {
	"Populating device tree",
	"Loading plugins",
	"Loading configuration",
	"Mounting filesystems",
	"Restoring system clock (UTC) from RTC",
	"Initializing random number generator",
	"Starting System log daemon",
	"Starting Kernel log daemon",
	"Bringing up interface lo",
	"Bringing up interface eth0",
	"Starting LLDP daemon (IEEE 802.1ab)",
	"Starting Mini snmpd",
	"Starting UDP port redirector",
	"Starting FTP daemon",
	"Starting TFTP daemon",
	NULL
};

const char *result[] = {
	" \e[1m[ OK ]\e[0m",
	" \e[7m[FAIL]\e[0m",
	" \e[7m[WARN]\e[0m"
};

static const int random_result(void)
{
	return rand() % 2;
}

static void services(void)
{
	int i;

	for (i = 0; svc[i]; i++) {
		char buf[512];
		int j, num, len = logo_cols - (int)strlen(svc[i]) - 8;
		int res = random_result();

		fputs("\r", stderr);
		delline();

		memset(buf, 0, sizeof(buf));
		num = snprintf(buf, sizeof(buf), "\r%*s%s ", screen_colo - 1, "", svc[i]);
		for (j = 0; j < len; j++)
			buf[num++] = '.';
		strcat(buf, result[res]);
		if (res)
			strcat(buf, "\n");
		fputs(buf, stderr);

		usleep(200000);
	}
}

void login(void)
{
	int i = 0;
	char *getty[] = {
		"TroglOS Linux 1.0-rc1 chaos ttyAMA0\n",
		"chaos login: \e[7m \e[0m\n",
		"\n",
		NULL
	};

	while (getty[i])
		fprintf(stderr, "%*s%s", screen_colo - 1, "", getty[i++]);
}

int main(int argc, char *argv[])
{
	int center = 0;

	if (argc > 1)
		center = atoi(argv[1]);

	initscr(&screen_rows, &screen_cols);

	if (logo_show(screen_rows, screen_cols, center)) {
		printf("    Logo %d x %d\n", logo_cols, logo_rows);
		printf("Terminal %d x %d\n", screen_cols, screen_rows);
		printf("Terminal too small, falling back to traditional progress startup.");
		return 1;
	}
		
	services();
	fputs("\r\033[2K\n", stderr);
	login();

	return 0;
}

/**
 * Local Variables:
 *  compile-command: "gcc -o demo demo.c"
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
