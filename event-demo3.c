/* Simple test of libev capabilities, with a focus on client/server networking.
 *
 * Copyright (C) 2011  Joachim Nilsson <troglobit@gmail.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <ev.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/fcntl.h>		// fcntl
#include <unistd.h>		// close

EV_P;
ev_io stdin_watcher;
ev_io remote_w;
ev_io send_w;
int remote_fd;
char *line = NULL;
size_t len = 0;

static void
send_cb (EV_P_ ev_io * w, int revents)
{
   if (revents & EV_WRITE)
   {
      puts ("remote ready for writing...");

      if (-1 == send (remote_fd, line, len, 0))
      {
         perror ("echo send");
         exit (EXIT_FAILURE);
      }

      // once the data is sent, stop notifications that
      // data can be sent until there is actually more
      // data to send
      ev_io_stop (EV_A_ & send_w);
      ev_io_set (&send_w, remote_fd, EV_READ);
      ev_io_start (EV_A_ & send_w);
   }
   else if (revents & EV_READ)
   {
      int n;
      char str[100] = ".\0";

      printf ("[r,remote]");
      n = recv (remote_fd, str, 100, 0);
      if (n <= 0)
      {
         if (0 == n)
         {
            // an orderly disconnect
            puts ("orderly disconnect");
            ev_io_stop (EV_A_ & send_w);
            close (remote_fd);
         }
         else if (EAGAIN == errno)
         {
            puts ("should never get in this state with libev");
         }
         else
         {
            perror ("recv");
         }
         return;
      }
      printf ("socket client said: %s", str);

   }
}

static void
stdin_cb (EV_P_ ev_io * w, int revents)
{
   int len2;			// not sure if this is at all useful

   puts ("stdin written to, reading...");
   len2 = getline (&line, &len, stdin);
   ev_io_stop (EV_A_ & send_w);
   ev_io_set (&send_w, remote_fd, EV_READ | EV_WRITE);
   ev_io_start (EV_A_ & send_w);
}

static void
remote_cb (EV_P_ ev_io * w, int revents)
{
   puts ("connected, now watching stdin");
   // Once the connection is established, listen to stdin
   ev_io_start (EV_A_ & stdin_watcher);
   // Once we're connected, that's the end of that
   ev_io_stop (EV_A_ & remote_w);
}

static void connect_to_server (EV_P_ char *sock_path)
{
   int len;
   struct sockaddr_un remote;

   remote_fd = socket (AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
   if (-1 == remote_fd)
   {
      perror ("socket");
      exit (1);
   }

   // this should be initialized before the connect() so
   // that no packets are dropped when initially sent?
   // http://cr.yp.to/docs/connect.html

   // initialize the connect callback so that it starts the stdin asap
   ev_io_init (&remote_w, remote_cb, remote_fd, EV_WRITE);
   ev_io_start (EV_A_ & remote_w);
   // initialize the send callback, but wait to start until there is data to write
   ev_io_init (&send_w, send_cb, remote_fd, EV_READ);
   ev_io_start (EV_A_ & send_w);

   remote.sun_family = AF_UNIX;
   strcpy (remote.sun_path, sock_path);
   len = strlen (remote.sun_path) + sizeof (remote.sun_family);

   if (-1 == connect (remote_fd, (struct sockaddr *) &remote, len))
   {
      perror ("connect");
      //exit(1);
   }
}

int main (void)
{
   loop = EV_DEFAULT;
   // initialise an io watcher, then start it
   // this one will watch for stdin to become readable
   setnonblock (0);
   ev_io_init (&stdin_watcher, stdin_cb, /*STDIN_FILENO */ 0, EV_READ);

   connection_new (EV_A_ "/tmp/libev-echo.sock");

   // now wait for events to arrive
   ev_loop (EV_A_ 0);

   // unloop was called, so exit
   return 0;
}

/**
 * Local Variables:
 * compile-command: "gcc -g -Wall -o event-demo3 event-demo3.c -lpthread -lrt -lev" 
 *  version-control: t
 *  kept-new-versions: 2
 *  c-file-style: "ellemtel"
 * End:
 */
