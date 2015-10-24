/* Homebrew tokenizers
 *
 * Copyright 2007, 2008, 2015  Joachim Nilsson <troglobit@gmail.com> 
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

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#ifdef UNITTEST
#include "unittest.h"
#endif
#include "tokenize.h"

/**
 * tokenize - Homebrew tokenizer
 * @str: String to chop up in tiny pieces.
 * @sep: String of possible token separators, e.g. " \t-."
 * @ctx: Pointer to a tokenize_t object to use for book-keeping.
 *
 * This function takes the string @str and chops it up in tokens.
 * Each token is delimited by any character found in @sep. 
 *
 * The @ctx parameter must be supplied.  It is used to keep track of
 * where in the string we last returned a token.
 *
 * The tokenize_t struct is in otn/c.h
 *
 * Returns:
 * The next token in @str or %NULL when no more tokens are available.
 */
char *tokenize (char *str, char *sep, tokenize_t *ctx)
{
   char *token;

   if (!ctx)
   {
      errno = EINVAL;
      return NULL;
   }

   if (!str)
   {
      if (!ctx->next) return NULL;
   }
   else
   {
      ctx->sep = sep;
      ctx->end = str + strlen (str);
      ctx->next = str;
   }

   /* It's legal to call tokenize() with sep:NULL */
   if (!sep)
   {
      sep = ctx->sep;
   }

   /* Find start of token, skip leading separators... */
   token = ctx->next;
   while (index (sep, token[0]))
   {
      token ++;
   }
   
   /* Find end of token */
   ctx->next = strpbrk (token, sep);
   if (ctx->next) 
   {
      /* Mark end of token, for string matchers. */
      *ctx->next = 0;

      /* Remove trailing whitespace */
      while (index (sep, ctx->next[0]))
      {
         ctx->next++;
      }
      if (ctx->next >= ctx->end)
      {
         ctx->next = NULL;
      }
   }

   return token;
}


/**
 * tokenize_simple - Specialized tokenizer, returns first word in a string.
 * @line: Line to split in tokens.
 *
 * This function splits the given string @str in two, but only if
 * there is at least one whitespace character.  A whitespace character
 * can be anything that the standard C library isspace() function
 * recognizes.
 *
 * Use the standard tokenize() function for a more generic
 * implementation.
 *
 * Please note, this function destroys the original string by
 * replacing all whitespace with zeroes.  Similar in nature to the
 * strtok() and strsep() functions.  If this matters to you then
 * write your own, or feed this function a copy of your original.
 *
 * Returns:
 * Upon return this function will have changed @str to point to the
 * reminder of the string and the first token (word) is returned.
 */
char *tokenize_simple (char **line)
{
   char *token;
   char *str;

   if (!line || !*line) 
      return NULL;

   /* Find beginning of token, skip leading whitespace */
   str = *line;
   while (*str && isspace (*str))
      str++;

   /* Find end of token */
   token = str;
   while (*str && !isspace (*str))
      str++;
   if ('\0' != str[0])
      *str++ = 0;           /* Mark end of token, for string matchers. */

   /* Remove trailing whitespace */
   while (*str && isspace (*str))
      str++;

   /* Update line to point at next token, or NULL if end of string. */
   *line = str;
   if ('\0' == str[0])
      *line = NULL;

   return token;
}

/****************************** UNIT TEST ******************************/
#ifdef UNITTEST
char *strings[] = 
{
   "c",
   "configure terminal",
   "conf t",
   "list some files",
   "conf	t  ",
   "  conf t",
   "	confi te	",
   NULL
};


int test_tokenize1 (void)
{
   int i;

   for (i = 0; strings[i]; i++)
   {
      char *line = strdup (strings[i]);
      char *token;
      tokenize_t ctx;

      token = tokenize (line, " \t\r\n", &ctx);
      while (token)
      {
         printf (":%s", token);
         token = tokenize (NULL, NULL, &ctx);
      }
      free (line);
      printf ("\n");
   }

   return 0;
}

int test_tokenize2 (void)
{
   int i;
   char *ports[] = {
      "X2",
      "X1-X2,X3,X5-X7",
      "X31,X5-X12,X42",
      "X31-X52",
      NULL
   };

   for (i = 0; ports[i]; i++)
   {
      char *line = strdup (ports[i]);
      char *token;
      tokenize_t ctx;

      token = tokenize (line, ",", &ctx);
      while (token)
      {
         char *subtoken;
         tokenize_t subctx;

         subtoken = tokenize (token, "-", &subctx);
         printf (":%s", subtoken);
         subtoken = tokenize (NULL, NULL, &subctx);
         if (subtoken)
            printf ("..%s", subtoken);

         token = tokenize (NULL, NULL, &ctx);
      }
      free (line);
      printf ("\n");
   }

   return 0;
}


int test_tokenize_simple1 (void)
{
   int i;

   for (i = 0; strings[i]; i++)
   {
      char *token;
      char *line = strdup (strings[i]);

      while (token = tokenize_simple (&line))
      {
         printf (":%s", token);
      }
      free (line);
      printf ("\n");
   }

   return 0;
}


int main (void)
{
   return 
      run (test_tokenize1) ||
      run (test_tokenize2) ||
      run (test_tokenize_simple1)
      ;
}
#endif /* UNITTEST */
/**
 * Local Variables:
 *  compile-command: "gcc -g -I. -o unittest -DUNITTEST tokenize.c && ./unittest"
 *  version-control: t
 *  kept-new-versions: 2
 *  c-file-style: "ellemtel"
 * End:
 */
