/* Simple test of libev capabilities, with a focus on timers.
 *
 * Copyright (C) 2009  Joachim Nilsson <troglobit@gmail.com>
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

#include <ev.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>             /* For abort() */

struct ev_loop *loop = NULL;
static ev_async wakeup;

/* On 64 bit systems a (void *) is 64 bit, while an int is 32, but
 * on 32 bit systems a (void *) is 32 bit and the int is 32. Let's
 * use a dedicated struct for passing data instead of trying to type
 * cast the integer directly. */
struct my_data
{
   int data;
};

static void my_sleep (int sec)
{
   struct timespec req;
   struct timespec rem;

   req.tv_sec = sec;
   req.tv_nsec = 0;
   while (nanosleep (&req, &rem))
   {
      fprintf (stderr, "%s: Interrupted while sleeping, retrying.\n", __FUNCTION__);
      req = rem;
   }
}

/* This thread is a very simplistic implementation to keep track of and check if the
 * timers, those under evaluation, work as expected.  I.e. that they don't diverge
 * over time.  Admittedly a better checker_task() would be desirable. --Jocke */
void *checker_task (void *p __attribute__ ((unused)))
{
   int i;
   time_t start;
   time_t now;

   /* Record actual starting time. */
   time (&start);

   for (i = 0; i < 105; i++)
   {
      time (&now);
      fprintf (stderr, "%s()   - %s", __FUNCTION__, ctime (&now));
      my_sleep (1);
   }
   fprintf (stderr, "Total  - %f sec\n", difftime (now, start));

   ev_async_send (EV_DEFAULT, &wakeup);

   return NULL;
}

void *competing_task (void *p __attribute__ ((unused)))
{
   while (1)
   {
      fprintf (stderr, "%s() - Sleeping 5 sec\n", __FUNCTION__);
      my_sleep (5);
   }

   return NULL;
}

static void timer_cb (struct ev_loop *loop __attribute__ ((unused)), ev_timer *w, int revents __attribute__ ((unused)))
{
   struct my_data *arg = (struct my_data *)(w->data);

   fprintf (stderr, "%s()       - Timeout %d sec\n", __FUNCTION__, arg->data);
}

static void sigint_cb (struct ev_loop *loop, ev_signal *w __attribute__ ((unused)), int revents __attribute__ ((unused)))
{
   ev_unloop (loop, EVUNLOOP_ALL);
}

static void wakeup_cb (struct ev_loop *loop, ev_async *w __attribute__ ((unused)), int revents __attribute__ ((unused)))
{
   fprintf (stderr, "Got async event\n");
   ev_unloop (loop, EVUNLOOP_ALL);
}

int main (void)
{
   pthread_t id;
   ev_signal signal_watcher;
   ev_timer timer100, timer70, timer30, timer12; //, timer2, timer1;
   struct my_data md100 = { 100 }, md70 = { 70 }, md30 = { 30 }, md12 = { 12 };

//   fprintf (stderr, "sizeof(int):%lu, sizeof(long):%lu, sizeof(long long):%lu, sizeof(void *): %lu\n",
//            sizeof(int), sizeof(long), sizeof(long long), sizeof(void *));
   loop = ev_default_loop (0);

   ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
   ev_signal_start (loop, &signal_watcher);

   ev_async_init (&wakeup, wakeup_cb);
   ev_async_start (loop, &wakeup);

   /* Test for pthread safey */
   pthread_create (&id, NULL, checker_task, NULL);
   pthread_detach (id);
   pthread_create (&id, NULL, competing_task, NULL);
   pthread_detach (id);

   /* Declaring in reversed execution order to catch inversion bugs. */
   timer100.data = (void *)&md100;
   ev_timer_init (&timer100, timer_cb, 100.0, 0.0);
   ev_timer_start (loop, &timer100);

   timer70.data = (void *)&md70;
   ev_timer_init (&timer70, timer_cb, 70.0, 0.0);
   ev_timer_start (loop, &timer70);

   timer30.data = (void *)&md30;
   ev_timer_init (&timer30, timer_cb, 30.0, 0.0);
   ev_timer_start (loop, &timer30);

   timer12.data = (void *)&md12;
   ev_timer_init (&timer12, timer_cb, 12.0, 12.0);
   ev_timer_start (loop, &timer12);

#if 0
   t = timer_declare (2000, callback, "2 seconds");
   if (timer_declare (1000, callback, "1 second"))
   {
      return 1;
   }
   /* Withdraw 2 second timer... */
   timer_undeclare (t);
#endif

   ev_loop (loop, 0);

   fprintf (stderr, "Leaving...\n");

   return 0;
}

/**
 * Local Variables:
 * compile-command: "gcc -g -Wall -o event-demo event-demo.c -lpthread -lev" 
 *  version-control: t
 *  kept-new-versions: 2
 *  c-file-style: "ellemtel"
 * End:
 */
