/* Some neat /proc tools for parsing /proc/<PID>/status 
 * 
 * Copyright (c) 2009  Joachim Nilsson <joachim.nilsson@member.fsf.org>
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

#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUF 80

static char *trim (char *token)
{
   char *start;

   if (!token)
   {
      errno = EINVAL;
      return NULL;
   }

   while (isspace(*token))
      token ++;

   start = token;
   
   while (!isspace(*token))
      token ++;

   *token = 0;

   return start;
}

char *proc_get_property (int pid, char *property)
{
   FILE *fp;
   char *val;
   char file[MAX_BUF];
   char buf[MAX_BUF];

   sprintf (file, "/proc/%d/status", pid);
   fp = fopen (file, "r");
   if (!fp)
   {
      fprintf (stderr, "Failed opening %s for reading: %s\n", file, strerror (errno));
      return NULL;
   }

   while (fgets (buf, MAX_BUF, fp))
   {
      char *ptr = buf;
      char *name = strsep (&ptr, ":");

      val = trim (ptr);
      if (!strcmp (name, property))
      {
//         printf ("DEBUG: %s\t= %s\n", name, val);
         break;
      }
   }

   fclose (fp);

   return strdup (val);
}

/* Returns 0 on error, otherwise the PID of procname */
int proc_find_by_name (char *procname)
{
   int pid = 0;
   DIR *dir;
   struct dirent *entry;

   dir = opendir("/proc");
   if (!dir)
   {
      return 0;
   }

   while ((entry = readdir(dir))) 
   {
      int this;
      char *val;

      if (!isdigit(*entry->d_name))
         continue;

      this = atoi (entry->d_name);
      val = proc_get_property (this, "Name");
      if (val)
      {
         if (!strcmp (val, procname))
         {
            pid = this;
            free (val);
            break;
         }

         free (val);
      }
   }
   closedir(dir);

   return pid;
}

int proc_find_ppid (char *procname)
{
   int pid, ppid = 0;
   char *property;

   pid = proc_find_by_name (procname);
   if (!pid) return 0;

   property = proc_get_property (pid, "PPid");
   if (property)
   {
      ppid = atoi (property);
      free (property);
   }

   return ppid;
}

char *proc_lookup_parent (char *procname)
{
   int ppid = proc_find_ppid (procname);

   if (ppid)
   {
      return proc_get_property (ppid, "Name");
   }

   return NULL;
}

#ifdef UNITTEST
int main (int argc, char *argv[])
{
   char *name;

   if (argc < 2)
   {
      fprintf (stderr, "Invalid number of arguments.\nUsage: %s /proc/<PID>/status\n", argv[0]);
      return 1;
   }

   name = proc_lookup_parent (argv[1]);
   if (!name)
   {
      printf ("Error, failed looking up parent process of %s.\n", argv[1]);
   }
   else
   {
      printf ("%s\n", name);
      free (name);
   }

   return 0;
}
#endif  /* UNITTEST */

/**
 * Local Variables:
 *  compile-command: "cc -DUNITTEST -W -Wall -Werror -o unittest proc.c && ./unittest unittest"
 *  version-control: t
 *  c-file-style: "ellemtel"
 * End:
 */
