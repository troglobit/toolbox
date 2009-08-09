/* Simple test of libev capabilities, with a focus on POSIX message queues.
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

#include <assert.h>
#include <errno.h>
#include <ev.h>
#include <fcntl.h>              /* For O_* constants */
#include <mqueue.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>             /* For abort() */
#include <string.h>
#include <sys/stat.h>           /* For mode constants */
#include <time.h>               /* For nanosleep() */

#define KEY_TEST_QUEUE "/event-demo2"

struct ev_loop *loop = NULL;

/* On 64 bit systems a (void *) is 64 bit, while an int is 32, but
 * on 32 bit systems a (void *) is 32 bit and the int is 32. Let's
 * use a dedicated struct for passing data instead of trying to type
 * cast the integer directly. */
struct my_data
{
   int data;
};

typedef struct 
{
   int event;
} post_event_t;


int setup_msgqueue (char *queue, mode_t mode)
{
   int qid = mq_open (queue, mode);

   if (-1 == qid)
   {
      struct mq_attr queue_attr;

      perror ("Cannot attach to queue");

      /* Attributes for our queue. Those can be set only during creating. */
      queue_attr.mq_maxmsg = 10; /* max. number of messages in queue at the same time */
      queue_attr.mq_msgsize = sizeof(post_event_t); /* max. message size */

      /* This creates a new queue named "/example_queue_1" and opens it for 
       * sending messages only. If the queue with the same name already exists 
       * function should fail (O_EXCL). The queue file permissions are set rw for owner 
       * and nothing for group/others. Queue limits set to values provided above. */
      qid = mq_open (queue, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, &queue_attr);
      if (-1 == qid)
      {
         perror ("Cannot create queue");
         abort ();
      }
   }

   return qid;
}

/**
 * post_event - Send simple/internal events to qprocessor() 
 * @msg: Per thread unique context, must not be reused by others.
 * @event: Event to post.
 *
 * Only use @block if called from a safe context, i.e. dedicated thread, and NOT
 * in signal/timer callback.
 *
 * It is imperative that the @msg is not reused between different threads!  Otherwise
 * you will overwrite an already posted message while it's pending operation.
 *
 * Returns:
 * POSIX OK (zero) or 1, on error.
 */
int post_event (post_event_t *msg, int event)
{
   static int queue = -1;

   assert (msg);

   if (-1 == queue)
   {
      queue = setup_msgqueue (KEY_TEST_QUEUE, O_WRONLY);
   }

   msg->event = event;

   while (mq_send (queue, (char *)msg, sizeof (post_event_t), 0) == -1)
   {
      if (EINTR == errno)
         continue;

      if (EAGAIN == errno)
      {
         fprintf (stderr, "Failed sending event %d to event_processor(), operation would block: %s\n", event, strerror (errno));
      }
      else
      {
         fprintf (stderr, "Sending %d to event_processor() failed: %s\n", event, strerror (errno));
      }

      return 1;
   }

   return 0;
}

static void my_sleep (int sec)
{
   struct timespec req;
   struct timespec rem;

   req.tv_sec = sec;
   req.tv_nsec = 0;
   while (nanosleep (&req, &rem))
   {
      fprintf (stderr, "%s() Interrupted while sleeping, retrying.\n", __FUNCTION__);
      req = rem;
   }
}

void *competing_task (void *p __attribute__ ((unused)))
{
   post_event_t msg;

   while (1)
   {
      fprintf (stderr, "%s() - Sending message to qprocessor(20)\n", __FUNCTION__);
      post_event (&msg, 20);   
      my_sleep (5);
   }

   return NULL;
}

static void timer_cb (struct ev_loop *loop __attribute__ ((unused)), ev_timer *w __attribute__ ((unused)), int revents __attribute__ ((unused)))
{
   post_event_t msg;
   struct my_data *arg = (struct my_data *)(w->data);

   fprintf (stderr, "%s()       - Got qid:%d from main(). Sending message to qprocessor(10)\n", __FUNCTION__, arg->data);
   post_event (&msg, 10);   
}

static void sigint_cb (struct ev_loop *loop, ev_signal *w __attribute__ ((unused)), int revents __attribute__ ((unused)))
{
   ev_unloop (loop, EVUNLOOP_ALL);
}

void qprocessor (struct ev_loop *loop __attribute__ ((unused)), struct ev_io *w, int revents __attribute__ ((unused)))
{
   post_event_t msg;

   memset (&msg, 0, sizeof (msg)); /* Safety measure. */

   ev_io_stop (loop, w);

   if (-1 == mq_receive (w->fd, (char *)&msg, sizeof (msg), NULL))
   {
      perror ("Nothing there to read");
      ev_io_start (loop, w);
      return;
   }

   ev_io_start (loop, w);
   fprintf (stderr, "%s()     - Recevied event %d\n", __FUNCTION__, msg.event);
}

int main (void)
{
   int qid = setup_msgqueue (KEY_TEST_QUEUE, O_RDWR);
   pthread_t id;
   ev_signal signal_watcher;
   struct ev_io queue_watcher;
   struct ev_timer timer3;
   struct my_data arg = { qid };

   loop = ev_default_loop (0);
   ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
   ev_signal_start (loop, &signal_watcher);

   timer3.data = (void *)&arg;
   ev_timer_init (&timer3, timer_cb, 3.0, 3.0);
   ev_timer_start (loop, &timer3);
   fprintf (stderr, "Passed qid:%d to timer callback\n", arg.data);

   ev_io_init (&queue_watcher, qprocessor, qid, EV_READ);
   ev_io_start (loop, &queue_watcher);

   pthread_create (&id, NULL, competing_task, NULL);
   pthread_detach (id);

   ev_loop (loop, 0);

   fprintf (stderr, "Leaving...\n");

   return 0;
}

/**
 * Local Variables:
 * compile-command: "gcc -g -Wall -o event-demo2 event-demo2.c -lpthread -lrt -lev" 
 *  version-control: t
 *  kept-new-versions: 2
 *  c-file-style: "ellemtel"
 * End:
 */
