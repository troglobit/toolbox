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

#include <stdio.h>
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

int main (void)
{
   int i, rows, cols, col = 1, row = 1, startrow = 2;
   struct winsize win = {.ws_col = 80, .ws_row = 24};
   
   ioctl (2, TIOCGWINSZ, &win);
   rows = win.ws_row - 4;
   cols = win.ws_col;

   if (cols < 20)
   {
      printf ("Sorry, too small (not wide enough) display. Needs at least 20 chars\n");
      return 1;
   }

   cls();
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

      if (i <= 32)
      {
         printxy (col, row++, "%3d  0x%02x  %-3s %-3s\n", i, i, ascii[i].seq, ascii[i].abbr);
      }
      else if (i == 127)
      {
         printxy (col, row++, "%3d  0x%02x  %-3s %-3s\n", i, i, "^?", "DEL");
      }
      else
      {
         printxy (col, row++, "%3d  0x%02x  %-3s %-3c\n", i, i, "", i);
      }
   }
   gotoxy (1, win.ws_row);

   return 0;
}

/**
 * Local Variables:
 *  version-control: t
 *  c-file-style: "ellemtel"
 * End:
 */

