/************************************************************
* Copyright (C) 2004 Codefidence ltd.
* Author: Gilad Ben-Yossef <gilad@codefidence.com>
* Hacked for write() in 2007 by Jakov
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
* 02111-1307, USA.
*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#define __USE_GNU                // This is needed for the RTLD_NEXT definition
#include <dlfcn.h>


ssize_t write (int fd, const void *buf, size_t count)
{
   ssize_t (*origwrite) (int fd, const void *buf, size_t count);
   char *error;

   origwrite = dlsym (RTLD_NEXT, "write");
   if ((error = dlerror ()) != NULL)
   {
      fprintf (stderr, "%s\n", error);
      exit (1);
   }

   /* You can also call origwrite here with a size of 1 if you want 1 byte files. 
     return origwrite (fd, buf, 1); 
   */

   return count;
}

/*
Compile with:
gcc writehack.c -o writehack.so -ldl -shared

Use with:
LD_PRELOAD=writehack.so ./program
*/
