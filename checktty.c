/* Check if a TTY is there, anyone home? */

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

extern char *__progname;

int main(int argc, char *argv[])
{
	int fd;
	char *dev;
	struct termios c;

	if (argc < 2)
		errx(1, "Usage: %s /dev/TTYNAME", __progname);

	dev = argv[1];
	if (access(dev, F_OK))
	        err(1, "%s", dev);

	fd = open(dev, O_RDONLY);
	if (-1 == fd)
		err(1, "%s", dev);

	if (tcgetattr(fd, &c))
		err(1, "%s", dev);

	return 0;
}

/**
 * Local Variables:
 *  compile-command: "make checktty && ./checktty /dev/ttyS1"
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
