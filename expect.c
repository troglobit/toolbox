/* Poor mans telnet expect in C. 
 * 
 * Copyright (c) 2010  Joachim Nilsson <joachim.nilsson@member.fsf.org>
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
#include <sys/socket.h>
#include <unistd.h>

/* Note: addr must be in network byte order, so htonl() it if needed! */
char *sys_telnet_expect (int addr, short port, char *script[])
{
   int i, sd;
   char *ptr;
   ssize_t bytes;
   static char buf[BUFSIZ];
   struct sockaddr s;
   struct sockaddr_in *sin;

   sd = socket (PF_INET, SOCK_STREAM, 0);
   if (-1 == sd)
   {
      //perror ("Failed creating socket");
      return NULL;
   }

   sin = (struct sockaddr_in *)&s;
   sin->sin_family = AF_INET;
   sin->sin_port = htons (port);
   sin->sin_addr.s_addr = addr;
   if (-1 == connect (sd, &s, sizeof (s)))
   {
      //perror ("Cannot connect to telnet daemon");
      return NULL;
   }

   for (i = 0; script[i]; i++)
   {
      if (read (sd, buf, sizeof(buf)) <= 0)
      {
         //perror ("no more data");
         return NULL;
      }

      if (!strstr (buf, script[i++]))
      {
         //perror ("no match");
         errno = ENOMSG;
         return NULL;
      }
      if (write (sd, script[i], strlen (script[i])) <= 0)
      {
         //perror ("cannot send");
         return NULL;
      }
   }
   bytes = read (sd, buf, sizeof(buf));
   if (bytes <= 0)
   {
      //perror ("no result");
      return NULL;
   }
   close (sd);
   buf[bytes] = 0;

   /* Skip last line -- only the returning prompt. */
   ptr = strrchr (buf, '\n');
   if (ptr) *ptr = 0;
   /* Skip first line -- only an echo of the last command */
   ptr = strchr (buf, '\n');
   if (ptr)
   {
      *ptr = 0;
      ptr++;
   }
   else
   {
      ptr = buf;
   }
      
   return ptr;
}


/**
 * Local Variables:
 *  version-control: t
 *  c-file-style: "ellemtel"
 * End:
 */
