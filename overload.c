/* Attempt at function overloading in C, inspired by
 * https://gustedt.wordpress.com/2010/06/03/default-arguments-for-c99/
 */

#include <stdio.h>

#define            _ARG2(_0, _1, _2, ...) _2
#define NARG2(...) _ARG2(__VA_ARGS__, 2, 1, 0)

#define  _PRINT_ARGS_1(NAME, a) a, NAME ## _default_fp()
#define  _PRINT_ARGS_2(NAME, a, b) a, b
#define __PRINT_ARGS(NAME, N, ...)      _PRINT_ARGS_ ## N (NAME, __VA_ARGS__)
#define  _PRINT_ARGS(NAME, N, ...)     __PRINT_ARGS(NAME, N, __VA_ARGS__)
#define   PRINT_ARGS(NAME, ...)    NAME(_PRINT_ARGS(NAME, NARG2(__VA_ARGS__), __VA_ARGS__))

#define print(...) PRINT_ARGS(print_to_file, __VA_ARGS__)

static FILE *print_to_file_default_fp(void) { return stdout; }
static void  print_to_file(char *str, FILE *fp)
{
	(void)fputs((const char *)str, fp);
}

int main(void)
{
	print("Hello\n");
	print("Hello\n", stderr);

	return 0;
}

/**
 * Local Variables:
 *  compile-command: "gcc -o overload overload.c; ./overload; rm overload"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
