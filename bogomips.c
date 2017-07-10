#include <stdio.h>
#include <time.h>

static void delay(unsigned long loops)
{
	while (loops)
		loops--;
}

int main(int argc, const char * argv[])
{
	unsigned long lps = 1;
	unsigned long ticks;

	fprintf(stderr, "Calibrating delay loop ... ");

	while ((lps <<= 1)) {
		ticks = clock();
		delay(lps);
		ticks = clock() - ticks;

		if (ticks >= CLOCKS_PER_SEC) {
			lps = (lps / ticks) * CLOCKS_PER_SEC;
			fprintf(stderr, "%lu.%02lu BogoMips\n", lps / 500000, (lps / 5000) % 100);

			return 0;
		}
	}

	printf("failed\n");
	return 1;
}

/**
 * Local Variables:
 *  compile-command: "make bogomips && ./bogomips"
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
