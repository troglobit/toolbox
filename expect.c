/* Poor mans telnet expect in C. 
 * 
 * Copyright (c) 2010, 2012  Joachim Nilsson <troglobit@gmail.com>
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

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#if 0
# define TELL(fmt, args...) fprintf (stderr, fmt "\n", ##args)
#else
# define TELL(fmt, args...)
#endif

int sys_telnet_close (sdbuf_t *ctx)
{
   free (ctx->buf);
   close (ctx->sd);
   free (ctx);

   return 0;
}

/* Open telnet connection to @addr:@port and connect. */
sdbuf_t *sys_telnet_open (int addr, short port)
{
   int saved_errno;
   struct sockaddr s;
   struct sockaddr_in *sin;
   sdbuf_t *ctx = (sdbuf_t *)malloc (sizeof(sdbuf_t));

   if (!ctx)
   {
      //perror("Failed allocating expect ctxect");
      return NULL;
   }

   ctx->buf = (char *)malloc (BUFSIZ);
   if (!ctx->buf)
   {
      //perror("Failed allocating expect buffer");
      free (ctx);
      return NULL;
   }

   ctx->sd = socket (PF_INET, SOCK_STREAM, 0);
   if (-1 == ctx->sd)
   {
      saved_errno = errno;
      //perror ("Failed creating socket");
      free (ctx->buf);
      free (ctx);
      errno = saved_errno;
      return NULL;
   }

   sin = (struct sockaddr_in *)&s;
   sin->sin_family = AF_INET;
   sin->sin_port = port;
   sin->sin_addr.s_addr = addr;
   if (-1 == connect (ctx->sd, &s, sizeof (s)))
   {
      saved_errno = errno;
      //perror ("Cannot connect to telnet daemon");
      sys_telnet_close (ctx);
      errno = saved_errno;

      return NULL;
   }

   return ctx;
}

/* Issue @script sequence on telnet @sd, with optional @output */
int sys_telnet_expect (sdbuf_t *ctx, char *script[], FILE *output)
{
   int i, first, cont, result = 0;
   ssize_t len;

   for (i = 0; script[i]; i++)
   {
      len = read (ctx->sd, ctx->buf, BUFSIZ);
      if (len <= 0)
      {
         //perror ("no more data");
         result = errno;
         break;
      }
      ctx->buf[len] = 0;

      TELL("Got line: '%s' is it '%s'", ctx->buf, script[i]);
      if (!strstr (ctx->buf, script[i++]))
      {
         //perror ("no match");
         result = ENOMSG;
         break;
      }

      TELL("Sending line: '%s'", script[i]);
      if (write (ctx->sd, script[i], strlen (script[i])) <= 0)
      {
         //perror ("cannot send");
         result = errno;
         break;
      }
   }

   cont = 1;
   first = 1;
   while (output && cont)
   {
      char *ptr = ctx->buf, *tmp;

      len = read (ctx->sd, ctx->buf, BUFSIZ);
      if (len == -1)
      {
         if (errno == EINTR)
            continue;

         result = errno;
         break;
      }
      ctx->buf[len] = 0;

      TELL("Read line: '%s'", ctx->buf);

      /* Skip first line -- only an echo of the last command */
      if (first)
      {
         first = 0;
         ptr = strchr (ctx->buf, '\n');
         if (ptr)
            ptr++;
         else
            ptr = ctx->buf;
      }

      TELL("Comparing '%s' with '%s'", ptr, script[i - 2]);

      /* Skip last line -- only the returning prompt. */
      tmp = strstr (ptr, script[i - 2]);
      if (tmp)
      {
         TELL("OK, last line.  Good bye!");
         cont = 0;
         *tmp = 0;

         tmp = strrchr (ptr, '\n');
         if (tmp)
            *++tmp = 0;
      }

      if (output)
         fputs (ptr, output);
   }

   return result;
}

/**
 * sys_telnet_session - Very simple expect-like implementation for telnet.
 * @addr: Must be in network byte order, use htonl().
 * @port: IP port to connect to, use htons().
 * @script: Expect like script of paired 'expect', 'response' strings.
 * @output: A &FILE pointer to a tmpfile() for output from the last command.
 *
 * This is a very simple expect-like implementation for querying and
 * operating daemons remotely over telnet.
 *
 * The @script is a query-response type of list of strings leading up to
 * a final command, which output is then written to the given @output file.
 *
 * Returns:
 * POSIX OK(0), or non-zero with errno set on error.
 */
int sys_telnet_session (int addr, short port, char *script[], FILE *output)
{
   sdbuf_t *sdbuf = sys_telnet_open (addr, port);
   int result = 0;

   if (!sdbuf)
       return errno;

   if (sys_telnet_expect (sdbuf, script, output))
   {
      result = errno;
   }

   sys_telnet_close  (sdbuf);

   return errno = result;
}


/**
 * Local Variables:
 *  version-control: t
 *  c-file-style: "ellemtel"
 * End:
 */
