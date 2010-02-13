/* Small application for printing ASCII codes and symbols in 80x24
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

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

/* Contents of array shamelessly stolen from the Wikipedia article on ASCII
 * control codes, http://en.wikipedia.org/wiki/C0_and_C1_control_codes */
struct ascii_entry {
   char seq[3];
   char abbr[4];
//   char name[20];
//   char desc[80];
} ascii[] = {
	{ "^@", "NUL" },
	{ "^A", "SOH" },
	{ "^B", "STX" },
	{ "^C", "ETX" },
	{ "^D", "EOT" },
	{ "^E", "ENQ" },
	{ "^F", "ACK" },
	{ "^G", "BEL" },
	{ "^H", "BS" },
	{ "^I", "HT" },
	{ "^J", "LF" },
	{ "^K", "VT" },
	{ "^L", "FF" },
	{ "^M", "CR" },
	{ "^N", "SO" },
	{ "^O", "SI" },
	{ "^P", "DLE" },
	{ "^Q", "DC1" },
	{ "^R", "DC2" },
	{ "^S", "DC3" },
	{ "^T", "DC4" },
	{ "^U", "NAK" },
	{ "^V", "SYN" },
	{ "^W", "ETB" },
	{ "^X", "CAN" },
	{ "^Y", "EM" },
	{ "^Z", "SUB" },
	{ "^[", "ESC" },
	{ "^\\", "FS" },
	{ "^]", "GS" },
	{ "^^", "RS" },
	{ "^_", "US" },
        { "",   "SP" },
//        ^? 	DEL       /* 127 */
};

#define cls()                                   \
{                                               \
   printf ("\e[2J");                            \
}
#define gotoxy(x, y)                            \
{                                               \
   printf ("\e[%d;%dH", y, x);                  \
}
#define printxy(x, y, fmt, args...)             \
{                                               \
   gotoxy (x, y);                               \
   printf (fmt, ##args);                        \
}

char *sys_ascii_seq (int code)
{
   if (code <= 32)
   {
      return ascii[code].seq;
   }
   if (code == 127)
   {
      return "^?";
   }

   return "";
}

/* XXX: Not thread safe. */
char *sys_ascii_abbr (int code)
{
   static char abbr[2];

   if (code <= 32)
   {
      return ascii[code].abbr;
   }
   if (code == 127)
   {
      return "DEL";
   }

   if (code > 255)
   {
      errno = EINVAL;
      return "NUL";
   }

   sprintf (abbr, "%c", code);

   return abbr;
}

int sys_ascii_code (char *token)
{
   int code = -1;
   char *ptr;

   /* "The [strtoul()] implementation *may* also set errno to EINVAL in case no
    * conversion was performed (no digits seen, and 0 returned)." -- man strtoul(3).
    * So we also consult the endptr to verify that any conversion was performed. */
   errno = 0;
   code = strtoul (token, &ptr, 10);
   if (errno || ptr == token)
   {
      int i;

      for (i = 0; i < 32; i++)
      {
         if (!strcmp (ascii[i].seq, token) || !strcmp (ascii[i].abbr, token))
         {
            return i;
         }
      }

      errno = EINVAL;
      return -1;                /* Unknown ASCII character/symbol/abbrev. */
   }

   if (code > 255)
   {
      errno = EINVAL;
      return -1;
   }

   return code;
}

int sys_ascii_table (void)
{
   int i, rows, cols, col = 1, row = 1, startrow = 2;
   struct winsize win = {.ws_col = 80, .ws_row = 24};
   
   ioctl (2, TIOCGWINSZ, &win);
   rows = win.ws_row - 4;
   cols = win.ws_col;

   if (cols < 20)
   {
      fprintf (stderr, "Sorry, too small (not wide enough) display. Needs at least 20 chars\n");
      return 1;
   }

   cls ();
   for (i = 0; i <= 127; i++)
   {
      if (i % rows == 0)
      {
         if (col + 38 < cols)
         {
            row = startrow;
            col = i != 0 ? col + 20 : 1;
         }
         else
         {
            col = 1;
         }
         printxy (col, row++, "DEC   HEX SEQ CHAR  \n");
         printxy (col, row++, "=================== \n");
      }

      printxy (col, row++, "%3d  0x%02x  %-3s %-3s\n", i, i, sys_ascii_seq (i), sys_ascii_abbr (i));
   }
   gotoxy (1, win.ws_row);

   return 0;
}

int main (int argc, char *argv[])
{
   if (argc > 1)
   {
      int code = sys_ascii_code (argv[1]);

      if (-1 == code)
      {
         fprintf (stderr, "Cannot find a matching ASCII character for %s: %s\n", argv[1], strerror (errno));
         return 1;
      }

      printf ("Arg: %s => dec:%3d  hex:0x%02x seq:%-3s abbrev:%-3s\n", argv[1], code, code, sys_ascii_seq (code), sys_ascii_abbr (code));

      return 0;
   }

   return sys_ascii_table ();
}

/**
 * Local Variables:
 *  version-control: t
 *  compile-command: "CFLAGS='-g' make ascii"
 *  c-file-style: "ellemtel"
 * End:
 */

