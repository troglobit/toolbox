/* Tracer bullet for evaluating Posix mutexes
 * MIT license.
 */

#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define NUM_THREADS 2

/* Thread safe, but only intra-process, mutex operations. */
#define MUTEX_INIT(m)							\
   {									\
      if (pthread_mutex_init (&m, NULL))				\
      {									\
	 fprintf (stderr, "Failed initializing mutex %s, error %d: %s\n", \
		  #m, errno, strerror(errno));				\
	 exit (1);							\
      }									\
      									\
      pthread_mutex_lock (&m);						\
   }

/* Free a mutual exclusion semaphore, previously claimed with
 * MUTEX_TAKE(). Mutexes must have been initialized using MUTEX_INIT()
 * prior to this operation.
 *
 * Releasing a previously released mutex is harmless.
 */
#define MUTEX_GIVE(m)							\
   {									\
      if (pthread_mutex_unlock(&m))					\
      {									\
	 fprintf (stderr, "Failed releasing mutex %s, error %d: %s\n",	\
		  #m, errno, strerror(errno));				\
      }									\
   }
/* Claim a mutual exclusion semaphore, previously initialized using
 * MUTEX_INIT().
 *
 * Claiming an already taken mutex results in deadlock.
 */
#define MUTEX_TAKE(m)							\
   {									\
      if (pthread_mutex_lock(&m))					\
      { 								\
	 fprintf (stderr, "Failed acquiring mutex %s, error %d: %s\n",	\
		  #m, errno, strerror(errno));				\
      }									\
   }

int bignum = 1000;		/* "protected" by mutex */
pthread_mutex_t mut;
pthread_t tid[NUM_THREADS];     /* array of thread IDs */

void *simple (void *parm)
{
   int name = (int)parm;

   /* critical section */
   MUTEX_TAKE(mut);
   bignum++;
   printf ("T%d: I am my own thread, bignum=%d\n", name, bignum);  /* inefficient, but correct */
   MUTEX_GIVE(mut);

   return NULL;
}

int main (int argc, char *argv[])
{
   int i;

   MUTEX_INIT(mut);

   for (i = 0; i < NUM_THREADS; i++)
   {
      pthread_create (&tid[i], NULL, simple, (void *)i);
   }

   bignum++;
   printf ("main: bignum=%d\n", bignum);

   /* OK, unleach threads! */
   printf ("main: Unleaching threads...\n");
   MUTEX_GIVE(mut);

   printf ("main: Waiting for threads to finish..\n");
   for (i = 0; i < NUM_THREADS; i++)
      pthread_join (tid[i], NULL);

   printf ("main: Reporting that all %d threads have terminated\n", i);
   bignum++;
   printf ("main: bignum=%d\n", bignum);

   return 0;
}

/**
 * Local Variables:
 *  compile-command: "LDLIBS=-lpthread make mutex-test && ./mutex-test"
 *  version-control: t
 *  kept-new-versions: 2
 *  c-file-style: "ellemtel"
 * End:
 */
