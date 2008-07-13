/* Unit test macros
 *
 * Copyright (C) 2005-2007  Jakob Eriksson <jakob.eriksson@vmlinux.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __UNITTEST_H__
#define __UNITTEST_H__
#ifdef UNITTEST

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <libgen.h>

/* Not exactly the same. :-/ */
#define strlcat strncat
#define strlcpy strncpy

// ANSI: \033[1;32mOK!\033[0;39m
static int runfunc (int (*a) (void), char *funcname, char *file)
{
   int retval;

   printf ("Begin %40s () in %s\t ... ", funcname, rindex (file, '/') ? : file);
   retval = a ();
   if (!retval)
   {
      printf ("OK!\n\n");
   }
   else
   {
      printf ("ERROR.\n");
      _exit (1);
   }

   return retval;
}

static int xa22 = -1;

// ANSI: \033[1;31mfailed\033[0;39m
#define ok(arg)                                                         \
{                                                                       \
   xa22 = (arg);                                                        \
   if (!xa22)                                                           \
   {                                                                    \
      printf ("\n%s:%d: FAILED ", __FILE__, __LINE__);                  \
      printf ("%40s = %d    \n", #arg, xa22);                           \
      fflush (NULL);                                                    \
      return 1;                                                         \
   }                                                                    \
}
#define run(fun)                                \
   runfunc((fun), #fun, __FILE__)

#endif /* UNITTEST */
#endif /* __UNITTEST_H__ */
