/*
  Static Malloc

  This is juts like malloc except it lacks sbrk() facilities.

  Instread, it is given a single pre-allocated block of memory
  to work with.

  This code is taked from http://www.panix.com/~mbh/projects.html,
  and is apparently written by Mark B. Hansen.

  Since he didn't specify a license for this code, I cannot go ahead
  and GPL it.  Instead I offer it as free public domain.

  Calin A. Culianu <calin@ajvar.org>
*/


/*
** IDEAS:
**	- keep one link of the list for "sorted by address",
**	  another for for "sorted by size"
**	- mallopt options to turn NDEBUG, DISPLAY, etc. on and off
**	  and to change FIT function
**	  and to switch to functions with no error checking
**	  and to print statistics
**	- return freed memory over to OS if there's enough of it at the
**	  end of the segment
**	- X utility to show fragmentation
**	- specify which file to put output in
**	- output how many times each function has been called
*/
#include "smalloc.h"

#define INT_MAX         ((int)(~0U>>1))
#define MAXINT INT_MAX
#define INT_MIN         (-INT_MAX - 1)
#define UINT_MAX        (~0U)
#define LONG_MAX        ((long)(~0UL>>1))
#define LONG_MIN        (-LONG_MAX - 1)
#define ULONG_MAX       (~0UL)


#define NULL ((void *)0)
#define FALSE 0
#define TRUE 1

#define EMIT(s,x...) ((s) && (s)->printf ? (s)->printf(x) \
                      : (int)0)

#define FIT(s,x) bestfit(s,x) /**/

#define PATTERN(p) ((char)((unsigned int)(p) & 0xff))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define ALIGN(p) (((p) + (sizeof(align_t) - 1)) & ~(sizeof(align_t) - 1))

#define ADDR(n) (((char *)(n)) + sizeof(node))
#define AFTER(n) (ADDR(n) + (n)->rsize)
#define OVERHEAD (sizeof(node))

#define MINNODE (64 * sizeof(align_t))

#define ADJACENT(n1,n2) \
	(((char *)(n1)) + OVERHEAD + (n1)->rsize == (char *)(n2))



typedef double align_t;

union _node {
    struct {
        size_type rsize; /* real size */
        size_type usize; /* user size */
        union _node *next;
    };
    align_t align;
};
typedef union _node node;

enum _list {
    AVAILABLE,
    ALLOCATED
};
typedef enum _list list;
#define SMALLOC_MAGIC 0xf51367d1
struct smalloc_t 
{
    int magic; /* always set to SMALLOC_MAGIC */
    node *available;
    node *allocated;
    unsigned long total;
    unsigned long free;
    unsigned long alloc_len;
    unsigned long avail_len;
    printf_fn_t printf;
};

static node *bestfit(smalloc_t *s, size_type size);
static node *firstfit(smalloc_t *s, size_type size);

/* put a breakpoint here for debugging */
static void debug(void) 
{
    if (0) {
      bestfit(0,0); /* avoid warnings */
      firstfit(0,0);
    }
} /* debug */

static void delete(smalloc_t *s, list l, node *n)
{
    node *c, *p, **head;

    head = (l == ALLOCATED) ? &s->allocated : &s->available;

    c = p = *head;
    while ((c != NULL) && (ADDR(c) > ADDR(n))) {
	p = c;
	c = c->next;
    }

    if (l == ALLOCATED) {
	s->alloc_len--;
    } else {
	s->avail_len--;
    }

    if (c == p) {
	*head = c->next;
    } else {
	p->next = c->next;
    }
} /* delete */


static void insert(smalloc_t *s, list l, node *n)
{
    node *c, *p, **head;

    head = (l == ALLOCATED) ? &s->allocated : &s->available;

    c = p = *head;
    while ((c != NULL) && (ADDR(c) > ADDR(n))) {
      p = c;
      c = c->next;
    }

    if ((l == AVAILABLE) && (*head != NULL) && (c != NULL) && ADJACENT(c,n)) {
      c->rsize += OVERHEAD + n->rsize;
      if (ADJACENT(c,p)) {
            delete(s, AVAILABLE, p);
	    c->rsize += OVERHEAD + p->rsize;
      }
    } else {
      if (l == ALLOCATED) {
	    s->alloc_len++;
      } else {
          s->avail_len++;
      }
      n->next = c;
      if (c == p) {
          *head = n;
      } else {
          p->next = n;
          if ((l == AVAILABLE) && (*head != NULL) && ADJACENT(n,p)) {
              delete(s, AVAILABLE, p);
              n->rsize += OVERHEAD + p->rsize;
          }
      }
    }
} /* insert */


static node *find(node *head, address_t ptr)
{
    node *c;

    c = head;
    while((c != NULL) && (ADDR(c) > (char *)ptr)) {
	c = c->next;
    }

    if ((c == NULL) || (ADDR(c) != (char *)ptr)) {
	return NULL;
    }

    return c;
} /* find */


static node *firstfit(smalloc_t *s, size_type size)
{
    node *c;
    size_type aligned = ALIGN(size);

    c = s->available;
    while ((c != NULL) && (c->rsize < aligned)) {
      c = c->next;
    }

    if (c == NULL) {
        EMIT(s,__FILE__": OUT OF MEMORY!  Requested: %lu  Total Allocated: %lu  Total Free: %lu\n", 
             size, s->total, s->free);
      return NULL;
    } else {
      delete(s, AVAILABLE, c);
    }

    if (c->rsize >= aligned + OVERHEAD + MINNODE) {
      node *n;
      size_type leftover;
      
      leftover = c->rsize - aligned - OVERHEAD;
      c->rsize = aligned;
      
      n = (node *)AFTER(c);
      n->rsize = leftover;
      
      insert(s, AVAILABLE, n);

    }

    c->usize = size;

    insert(s, ALLOCATED, c);

    return c;
} /* firstfit */


static node *bestfit(smalloc_t *s, size_type size)
{
    node *c, *b;
    size_type aligned = ALIGN(size);
    size_type over, best = MAXINT;

    b = NULL;
    c = s->available;
    while (c != NULL) {
	if (c->rsize >= size) {
	    over = c->rsize - size;
	    if ((over < best) || (c == s->available)) {
		best = over;
		b = c;
	    }
	}
	c = c->next;
    }

    if (b == NULL) {
      EMIT(s, __FILE__": OUT OF MEMORY!  Requested: %lu  Total Allocated: %lu  Total Free: %lu\n", 
           size, s->total, s->free);
      return NULL;
    } else {
      delete(s, AVAILABLE, b);
    }

    if (b->rsize >= aligned + OVERHEAD + MINNODE) {
	node *n;
	size_type leftover;
	
	leftover = b->rsize - aligned - OVERHEAD;
	b->rsize = aligned;

	n = (node *)AFTER(b);
	n->rsize = leftover;

	insert(s, AVAILABLE, n);

    }

    b->usize = size;

    insert(s, ALLOCATED, b);

    return b;
} /* bestfit */

/*---------------------------------------------------------------------------*/

static address_t _malloc(smalloc_t *s, size_type size)
{
    node *store;

    if (size == 0) {
        EMIT(s, __FILE__": attempt to allocate 0 bytes\n");
    }
    
    store = FIT(s,size);

    if (store == NULL) {
        EMIT(s, __FILE__": unable to allocate %lu bytes\n", size);
        debug();
        return NULL;
    }

    s->total += size;
    s->free -= store->rsize+OVERHEAD;
    return ADDR(store);
} /* _malloc */

static void _free(smalloc_t *s, address_t ptr)
{
    node *t;

    if (ptr == NULL) {
      EMIT(s, __FILE__": attempt to free NULL\n");
      return;
    }

    t = find(s->allocated, ptr);

    if (t == NULL) {
      EMIT(s, __FILE__": attempt to free unallocated memory (0x%x)\n",
           (unsigned int)ptr);
      debug();
      return;
    }
    s->free += t->rsize+OVERHEAD;
    delete(s, ALLOCATED, t);
    insert(s, AVAILABLE, t);

    s->total -= t->usize;
} /* _free */

static void clear(smalloc_t *s)
{
    s->magic = 0;
    s->available = NULL;
    s->allocated = NULL;
    s->total = 0;
    s->free = 0;
    s->alloc_len = 0;
    s->avail_len = 0;
    s->printf = 0;
}

/*---------------------------------------------------------------------------*/
/*----------------------------- USER INTERFACE: -----------------------------*/
/*---------------------------------------------------------------------------*/


address_t smalloc(smalloc_t *s, size_type size)
{
    return _malloc(s, size);
} /* malloc */


void sfree(smalloc_t *s, address_t ptr)
{
    _free(s, ptr);
} /* free */


void scfree(smalloc_t *s, address_t ptr)
{
    _free(s, ptr);
} /* cfree */

smalloc_t * smalloc_init(address_t memory, size_type size, printf_fn_t pf)
{
    smalloc_t *s = (smalloc_t *)memory;     
    node *n = (node *)(((char *)memory)+sizeof(*s));
    
    if (size < MINNODE) {
        smalloc_t tmp;
        clear(&tmp);
        tmp.printf = pf;
        EMIT(&tmp, __FILE__ ": specified memory chunk is too small!  Please give smalloc_init at least %lu size bytes!\n", (unsigned long)MINNODE);
        return NULL;
    }

    clear(s);
    s->magic = SMALLOC_MAGIC;
    n->rsize = size - OVERHEAD - sizeof(*s);
    n->next = NULL;
    s->available = n;
    s->avail_len = 1;
    s->free = n->rsize;
    s->printf = pf;

    return s;
}

void smalloc_destroy(smalloc_t *s)
{
    clear(s);
}

void smalloc_add_memory_chunk(smalloc_t * s, address_t memory, size_type size)
{
  node *n = (node *)memory;

  if (size < MINNODE) {
      EMIT(s, __FILE__ ": attempt to add a memory chunk that is too small!  Please add memory chunks of at least %lu size bytes!\n", (unsigned long)MINNODE);
      return;
  }

  n->rsize = size - OVERHEAD;
  n->usize = 0;
  s->free += n->rsize;
  
  n->next = NULL;
  insert(s, AVAILABLE, n);
}

/* Returns the number of bytes allocated. */
unsigned long smalloc_get_allocated(smalloc_t *s)
{
    return s->total;
}
/* Returns the number of bytes free. */
extern unsigned long smalloc_get_free(smalloc_t *s)
{
    return s->free;
}
/* Returns the number of contiguous areas that are free. */
extern unsigned smalloc_get_n_free_areas(smalloc_t *s)
{
    return s->avail_len;
}
/* Returns the number of individual areas that are allocated. */
extern unsigned smalloc_get_n_allocated_areas(smalloc_t *s)
{
    return s->alloc_len;
}
