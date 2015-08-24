/* Attempt at function overloading in C, inspired by
 * https://gustedt.wordpress.com/2010/06/03/default-arguments-for-c99/
 *
 * The following magic creates two functions with the same name and
 * different input arguments:
 *
 *     void print(char *str);
 *     void print(char *str, FILE *fp);
 *
 * Enjoy this example under the public domain, worldwide through the
 * CC0 1.0 Universal public domain dedication.
 */

#include <stdio.h>

#define            _ARG2(_0, _1, _2, ...) _2
#define NARG2(...) _ARG2(__VA_ARGS__, 2, 1, 0)

#define  _PRINT_ARGS_1(fn, str)     str, fn ## _default_fp()
#define  _PRINT_ARGS_2(fn, str, fp) str, fp
#define __PRINT_ARGS(fn, N, ...)       _PRINT_ARGS_ ## N (fn,              __VA_ARGS__)
#define  _PRINT_ARGS(fn, N, ...)      __PRINT_ARGS(fn, N,                  __VA_ARGS__)
#define   PRINT_ARGS(fn,    ...)    fn(_PRINT_ARGS(fn, NARG2(__VA_ARGS__), __VA_ARGS__))

#define print(...) PRINT_ARGS(out, __VA_ARGS__)

static FILE *out_default_fp(void)     { return stdout; }
static void  out(char *str, FILE *fp) { (void)fputs((const char *)str, fp); }

int main(void)
{
	print("Hello\n");
	print("Hello\n", stderr);

	return 0;
}

/**
 * Local Variables:
 *  compile-command: "gcc -W -Wall -o overload overload.c; ./overload; rm overload"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
