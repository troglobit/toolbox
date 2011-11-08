/* Improved version of iroute2 "mroute show" with statistics
 *
 * Copyright (c) 2011  Joachim Nilsson <troglobit@gmail.com>
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
#include <stdlib.h>
#include <string.h>
#include <net/if.h>             /* IFNAMSIZ */

#define MAX_NUM_OUTBOUND_IFACES 8

/* XXX: Yes, strtok() isn't really fine art, and not even thread safe,
 *      but it does its job.  If you want, please refactor, after all
 *      there is a unit test here! :-) --Jocke 2009-03-03
 */
#define TOKEN(xstr)                             \
   token = strtok (xstr, " \t");                \
   if (!token)                                  \
      return NULL

#ifndef strlcpy
#define strlcpy(dst, src, len) { strncpy (dst, src, len - 1); dst[len - 1] = 0; }
#endif

typedef struct
{
   int group;
   int source;
   char iifname[IFNAMSIZ];
   int num_oifs;
   char oifnames[IFNAMSIZ][MAX_NUM_OUTBOUND_IFACES];
   unsigned int packets;
   unsigned long long bytes;
   unsigned long long invalid;
} sys_mroute_t;

static long long __read_number (char *str, int base)
{
   long long number = 0;
   char *endp;

   /* Check if no, or empty, string. */
   if (!str || str[0] == 0)
   {
      errno = EINVAL;
      return -1;
   }

   errno = 0;
   number = strtoull (str, &endp, base);
   if (errno || endp == str)
      return -1;

   return number;
}

/*
  /tmp # cat /proc/net/ip_mr_cache
  Group    Origin   Iif     Pkts    Bytes    Wrong Oifs
  E1010203 C0A80101 0          0        0        0  1:1
*/
static sys_mroute_t *__parse_one_route (char *vif_table[], char *buf, sys_mroute_t *mroute)
{
   int id, num = 0;
   char *token;

   /* Replace \n */
   buf[strlen (buf)] = 0;

   TOKEN(buf);
   mroute->group = __read_number (token, 16);

   TOKEN(NULL);
   mroute->source = __read_number (token, 16);

   TOKEN(NULL);
   id = __read_number (token, 10);
   strlcpy (mroute->iifname, vif_table[id], sizeof(mroute->iifname));

   TOKEN(NULL);
   mroute->packets = __read_number (token, 10);

   TOKEN(NULL);
   mroute->bytes = __read_number (token, 10);

   TOKEN(NULL);
   mroute->invalid = __read_number (token, 10);

   TOKEN(NULL);
   while (token)
   {
      char tmp[10];
      char *ptr;

      strlcpy (tmp, token, sizeof(tmp));
      ptr = strchr (tmp, ':');
      if (!ptr)
         break;
      *ptr = 0;

      id = __read_number (tmp, 10);
      strlcpy (mroute->oifnames[num], vif_table[id], sizeof(mroute->oifnames[num]));

      TOKEN(NULL);
      num++;
   }
   mroute->num_oifs = num;

   return mroute;
}

/*
/tmp # cat /proc/net/ip_mr_vif
Interface      BytesIn  PktsIn  BytesOut PktsOut Flags Local    Remote
 0 vlan1             0       0         0       0 00000 C0A80214 00000000
 1 vlan2             0       0         0       0 00000 0A0A0A02 00000000
 2 vlan3             0       0         0       0 00000 0A0A1401 00000000
*/
static int __build_vif_table (char ***vif_table)
{
   int num = 0, cnt = 0;
   FILE *fp;
   char *dummy;
   char buf[120];

   fp = fopen ("/proc/net/ip_mr_vif", "r");
   if (!fp)
      return -1;

   /* Count number of routes. */
   dummy = fgets (buf, sizeof (buf), fp); /* Skip header. */
   while (dummy && fgets (buf, sizeof (buf), fp))
   {
      /* Skip blank lines */
      if (strlen (buf) < 10)
         continue;

      num++;
   }

   *vif_table = calloc (num, sizeof (char *));
   if (NULL == *vif_table)
   {
      fclose (fp);
      return -1;
   }

   rewind (fp);
   dummy = fgets (buf, sizeof (buf), fp); /* Skip header. */
   while (dummy && fgets (buf, sizeof (buf), fp))
   {
      long long id;
      char *token;

      /* Replace \n */
      buf[strlen (buf)] = 0;

      id = __read_number (strtok (buf, " \t"), 10);
      if (id < 0)
         continue;              /* Skip line */
      if (id >= num)
         continue;              /* Invalid ID? */

      token = strtok (NULL, " \t");
      (*vif_table)[cnt] = strdup (token);
      cnt++;
   }

   fclose (fp);

   return cnt;
}

void __free_vif_table (char **vif_table, int num)
{
   int i;

   for (i = 0; i < num; i++)
   {
      if (vif_table[i])
         free (vif_table[i]);
   }

   free (vif_table);
}


/**
 * sys_mroute_dump - Dump static multicast routing table.
 * @routes: Pointer to a pointer where to return the routing table.
 *
 * This function implements a parser of Linux /proc/net/ip_mr_cache with the aid
 * of the virtual interface mapping in /proc/net/ip_mr_vif.
 *
 * Note, the @routes pointer returned is an allocated list of &sys_mroute_t that must
 * be freed when done.
 *
 * Returns:
 * The number of routes returned in @routes, or -1 on error.
 */
ssize_t sys_mroute_dump (sys_mroute_t **routes)
{
   ssize_t i = 0, cnt = 0, num = 0;
   char buf[120];
   char *dummy;
   char **vif_table = NULL;
   FILE *fp;

   num = __build_vif_table (&vif_table);
   if (!num)
      return -1;

   fp = fopen ("/proc/net/ip_mr_cache", "r");
   if (!fp)
   {
      __free_vif_table (vif_table, num);
      return -1;
   }

   /* Count number of routes. */
   dummy = fgets (buf, sizeof (buf), fp); /* Skip header. */
   while (dummy && fgets (buf, sizeof (buf), fp))
   {
      /* Skip blank lines */
      if (strlen (buf) < 10)
         continue;

      cnt++;
   }

   *routes = calloc (cnt, sizeof (sys_mroute_t));
   if (NULL == *routes)
   {
      __free_vif_table (vif_table, num);
      fclose (fp);
      return -1;
   }

   rewind (fp);
   dummy = fgets (buf, sizeof (buf), fp); /* Skip header. */
   while (dummy && fgets (buf, sizeof (buf), fp))
   {
      sys_mroute_t *mroute = &(*routes)[i];

      if (!__parse_one_route (vif_table, buf, mroute))
         continue;

      i++;
   }

   __free_vif_table (vif_table, num);
   fclose (fp);

   return i;
}


#ifdef UNITTEST
#include <arpa/inet.h>

int main (void)
{
   ssize_t i, j, num;
   sys_mroute_t *routes;

   num = sys_mroute_dump (&routes);
   if (num <= 0)
   {
      printf ("No routes available\n");
      return 1;
   }

   printf ("Group            Source           Inbound          Packets     Bytes       Invalid   Outbound\n");
   printf ("==================================================================================================\n");
   for (i = 0; i < num; i++)
   {
      char group[20], source[20];
      sys_mroute_t *route = &routes[i];

      inet_ntop (AF_INET, &route->group, group, sizeof (group));
      inet_ntop (AF_INET, &route->source, source, sizeof (source));

      printf ("%-15s  %-15s  %-15s  %-10u  %-10Ld  %-10Ld",
              group, source, route->iifname, route->packets, route->bytes, route->invalid);
      for (j = 0; j < route->num_oifs; j++)
         printf ("%s%s", route->oifnames[j], j + 1 < route->num_oifs ? ", " : "\n");
   }
   printf ("==================================================================================================\n");

   free (routes);

   return 0;
}
#endif  /* UNITTEST */

/**
 * Local Variables:
 *  compile-command: "gcc -g -W -Wall -DUNITTEST -I../../include -o mroute-show mroute-show.c && ./mroute-show"
 *  version-control: t
 *  c-file-style: "ellemtel"
 * End:
 */
