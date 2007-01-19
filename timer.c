/* Westermo OnTime AB - Timer Implementation.
 *
 * Copyright (C) 2006 Joachim Nilsson <jocke@vmlinux.org>
 *
 * This is a free implementation of the legendary paper "Implementing Software
 * Timers" by Don Libes.  It originally appeared in the November 1990 "C User's
 * Journal", reprinted as Chapter 35 of "Obuscated C and other Mysteries", John
 * Wiley & Sons, 1993, ISBN 0-471-57805-3.  Also available online from the
 * author's home page http://www.kohala.com/start/libes.timers.txt
 *
 * Nota bene: The interrupt disable/enable routines are simulated using
 * sigprocmask() and pthread_mutex_lock() with a mutex initialized in
 * timers_init().
 *
 * The time_now, time_timer_set variables and all operations on them have been
 * adapted to using the UNIX gettimeofday() & c:o system calls.
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

/* Periodic timers now added.
 * TODO: Test timers, both one-shot and periodic, with pthreads.
 */

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>             /* All the UNIX signal definitions */
#include <stdio.h>
#include <string.h>             /* memset */
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>             /*  */

#define TIMEVAL_MAX {INT_MAX, INT_MAX}
#define TIMEVAL_MIN {0, 0} 
typedef struct timeval timeval_t;

struct timer
{
      int inuse, periodic;       /* 1 if in use, dito if it is a periodic timer */
      timeval_t time;            /* relative time to wait */
      timeval_t period;          /* Period time for periodic timers */
      void (*callback)(void *);  /* called on timeout */
      void *arg;                 /* optional argument passed to callback */
} ;

static int max_timers = 0;                  /* Max timers, set by timers_init() */
static struct timer *timers;                /* set of timers */
static struct timer *timer_next = NULL;     /* timer we expect to run down next */
static timeval_t time_timer_set;            /* time when physical timer was set */

static void disable_interrupts (void)
{
   sigset_t s;

   sigemptyset (&s); 
   sigaddset (&s, SIGALRM);

   if (sigprocmask (SIG_BLOCK, &s, NULL) == -1)
   {
      perror("Could not block signal SIGALRM");
   }
}

static void enable_interrupts (void)
{
   sigset_t s;

   sigemptyset (&s); 
   sigaddset (&s, SIGALRM);

   if (sigprocmask (SIG_UNBLOCK, &s, NULL) == -1)
   {
      perror("Could not unblock signal SIGALRM");
   }
}

/* Implemented using POSIX setitimer() */
static void start_physical_timer (timeval_t start)
{
   struct itimerval val;
   struct timeval now;

   memset (&val, 0, sizeof (struct itimerval));
   val.it_value = start;

   /* ITIMER_REAL delivers SIGALRM on expiration. */
   if (setitimer (ITIMER_REAL, &val, NULL))
   {
      perror ("Failed setting itimer");
      return;
   }
}



/* subtract time from all timers, enabling any that run out along the way */
static void update_timers (timeval_t elapsed)
{
   int do_restart = 0;
   struct timer *t;
   static struct timer timer_last = {
      0,                           /* in use */
      0,                           /* non-periodic */
      TIMEVAL_MAX,                 /* time */
      TIMEVAL_MIN,                 /* period */
      NULL                         /* event pointer */
   };
  restart:
   timer_next = &timer_last;

   for (t = timers; t < &timers[max_timers]; t++)
   {
      if (t->inuse)
      {
         if (timercmp (&elapsed, &t->time, <)) /* unexpired */
         {
            timersub (&t->time, &elapsed, &t->time);
            if (timercmp (&t->time, &timer_next->time, <))
            {
               timer_next = t;
            }
         }
         else                   /* expired */
         {
            /* Ah, t has expired, call user defined callback */
            t->callback (t->arg);

            /* Free timer *after* callback has completed! */
            if (t->periodic)
            {
               t->time = t->period;
               do_restart = 1;
            }
            else
            {
               t->inuse = 0;    /* remove timer */
            }
         }
      }
   }

   /* Hack: when perodic timers expire */
   if (do_restart)
   {
      do_restart = 0;
      elapsed.tv_sec = 0;
      elapsed.tv_usec = 0;

      goto restart;
   }

   /* reset timer_next if no timers are found */
   if (!timer_next->inuse)
   {
      timer_next = 0;
   }
}


static void timer_interrupt_handler (int dummy)
{
   timeval_t elapsed, now;
   
   gettimeofday (&now, NULL);
   timersub (&now, &time_timer_set, &elapsed);
   update_timers (elapsed);

   /* start physical timer for next shortest time if one exists */
   if (timer_next)
   {
      start_physical_timer (timer_next->time);
      time_timer_set = now;
   }
}



/**
 * timers_init - Initialize linked in application timers.
 * @max: Max number of timers available to application.
 *
 * Initialize the application timers.  Setup @max amount of software timers
 * for the application.
 *
 * Return value: 0 on OK, 1 on error (out of memory).
 */
int timers_init (int max)
{
   struct sigaction sig;

   if (max < 1)
   {
      errno = EINVAL;           /* Needs to be at least one timer. */
      return 1;
   }

   timers = calloc (max_timers, sizeof (struct timer));
   if (!timers)
   {
      return 1;
   }

   /* Setup SIGALRM handler for periodic update_timers() function. */
   sig.sa_handler = timer_interrupt_handler;
   sigemptyset (&sig.sa_mask);
   sig.sa_flags = 0;
   if (sigaction (SIGALRM, &sig, NULL) == -1)
   {
      return 1;
   }

   /* OK, we're all setup OK here. */
   max_timers = max;
}

struct timer *__timer_declare (int periodic, unsigned int time, void (*callback)(void *), void *arg)
{
   /* All local variables in this API need to be on stack for thread safety. */
   struct timer *t;
   timeval_t elapsed, now, new, t1, t2;

   /* We only need to verify max_timers here, it is set by timers_init() when
    * allocating the timers, so the only possible entry point is here. */
   assert (max_timers > 0);
   /* Verify the callback. */
   assert (callback != NULL);

   disable_interrupts ();

   /* Find a new/free one */
   for (t = timers; t < &timers[max_timers]; t++)
   {
      if (!t->inuse)
         break;
   }
      
   /* out of timers? */
   if (t == &timers[max_timers])
   {
      enable_interrupts ();
      return NULL;
   }
      
   /* Setup new timer, do adjust the time base. */
   if (time / 1000)
   {
      new.tv_sec = time / 1000;
      new.tv_usec = (time % 1000) * 1000;
   }
   else
   {
      new.tv_sec = 0;
      new.tv_usec = time * 1000;
   }
      
   /* install new timer */
   t->callback = callback;
   t->arg = arg;
   t->time = new;
   t->inuse = 1;
   t->periodic = periodic;
   t->period = new;

   /* What time is Love? */
   gettimeofday (&now, NULL);

   if (!timer_next)
   {
      /* no timers set at all, so this is shortest */
      start_physical_timer ((timer_next = t)->time);
      time_timer_set = now;
   }
   else 
   {
      timeradd (&now, &new, &t1);
      timeradd (&timer_next->time, &time_timer_set, &t2);

      if (timercmp (&t1, &t2, <))
      {
         /* new timer is shorter than current one, so */
         timersub (&now, &time_timer_set, &elapsed);
         update_timers (elapsed);
         start_physical_timer ((timer_next = t)->time);
         time_timer_set = now;
      }
      else
      {
         /* new timer is longer, than current one */
      }
   }
   enable_interrupts ();

   return t;
}

/**
 * timer_declare_periodic - Create a new periodic timer.
 * @period: Period time in milliseconds
 * @callback: Pointer to simple callback.
 * @arg: Argument to send to @callback, can be %NULL.
 *
 * Creates a new periodic timer which will expire each @period milliseconds.
 * Upon expiration the callback function @callback is called with the argument
 * @arg.  If the timer is cancelled using timer_undeclare() before the timer has
 * expired the callback will not be called.  Like any callback it should be
 * short and do nothing more than is required.
 *
 * Note: The original article by Don Libes did not have the concept of
 * registering a callback for each timer.  Instead a scheduler/user given
 * pointer to an "event" was used, this was set to %TRUE when the timer expired.
 * This implementation deviates due to project implementation requirements.  It
 * is important to know that this means that the callback will be run in soft
 * interrupt context, i.e. in the signal handler for SIGALRM and thus "steal"
 * time from the other timers.  Hence, the advice to keep the callback execution
 * time short.
 *
 * Return value: Timer id, used for e.g. timer_undelcare().
 */
struct timer *timer_declare_periodic (unsigned int period, void (*callback)(void *), void *arg)
{
   return __timer_declare (1, period, callback, arg);
}

/**
 * timer_declare - Create a new timer.
 * @time: Time in milliseconds
 * @callback: Pointer to simple callback.
 * @arg: Argument to send to @callback, can be %NULL.
 *
 * Creates a new timer which will expire after @time milliseconds.  Upon
 * expiration the callback function @callback will be called with the argument
 * @arg.  If the timer is cancelled using timer_undeclare() before the timer has
 * expired the callback will not be called.  Like any callback it should be
 * short and do nothing more than is required.
 *
 * Note: The original article by Don Libes did not have the concept of
 * registering a callback for each timer.  Instead a scheduler/user given
 * pointer to an "event" was used, this was set to %TRUE when the timer expired.
 * This implementation deviates due to project implementation requirements.  It
 * is important to know that this means that the callback will be run in soft
 * interrupt context, i.e. in the signal handler for SIGALRM and thus "steal"
 * time from the other timers.  Hence, the advice to keep the callback execution
 * time short.
 *
 * Return value: Timer id, used for e.g. timer_undelcare().
 */
struct timer *timer_declare (unsigned int time, void (*callback)(void *), void *arg)
{
   return __timer_declare (0, time, callback, arg);
}


void timer_undeclare (struct timer *t)
{
   timeval_t elapsed, now;      /* Must be on stack to be thread safe! */

   disable_interrupts ();
   if (!t->inuse)
   {
      enable_interrupts ();
      return;
   }

   t->inuse = 0;

   /* check if we were waiting on this one */
   if (t == timer_next)
   {
      gettimeofday (&now, NULL);
      /* Time since last timer start. */
      timersub (&now, &time_timer_set, &elapsed);
      update_timers (elapsed);
      if (timer_next)
      {
         start_physical_timer (timer_next->time);
         time_timer_set = now;
      }
   }
   enable_interrupts ();
}


/******************************* UNIT TEST CODE *******************************/
#ifdef UNITTEST
static void callback (void *arg)
{
   char *text = arg;

   printf ("callback() - %s", text);
   fflush (stdout);
}

/* TODO: XXX need to rewrite this unit test to not print anything on screen but
 * just return 0 or 1. Possibly use Jakob's OK macro.  I'm only using the
 * current setup for debugging the code. */
int main (void)
{
   int i;
   struct timer *t;
   time_t start;
   time_t now;

   /* Declare 4 timers for this application. */
   timers_init (6);

   /* Record actual starting time. */
   time (&start);

   /* Declaring in reversed execution order to catch inversion bugs. */
   timer_declare (100000, callback, "100 seconds");
   timer_declare (70000, callback, "70 seconds");
   timer_declare (30000, callback, "30 seconds");
   timer_declare_periodic (3000, callback, " > 3 sek period < ");
   t = timer_declare (2000, callback, "2 seconds");
   if (timer_declare (1000, callback, "1 second"))
   {
      return 1;
   }
   /* Withdraw 2 second timer... */
   timer_undeclare (t);

   printf ("Waiting ... (%lu) %s==================================================",
           start, "tok\n"); //ctime (&start));
   fflush (stdout);
   for (i = 0; i < 105; i++)
   {
      time (&now);
      printf ("\n%d. (%lu) ", i + 1, now);
//      printf ("\n%d. ", i + 1);
      fflush (stdout);
      sleep (1);
   }

   printf ("\nLeaving...\n");
   fflush (stdout);

   return 0;
}
#endif /* UNITTEST */


/**
 * Local Variables:
 * compile-command: "gcc -O2 -DUNITTEST -o timer-test timer.c" 
 *  version-control: t
 *  kept-new-versions: 2
 *  c-file-style: "ellemtel"
 * End:
 */
