/* Simple CRC test
 *
 * Copyright (C) 2005-2014  Joachim Nilsson <troglobit@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

static char *program_name;

unsigned int (*checksum_func)(unsigned char data[], size_t len);

/* Update the CRC for transmitted and received data using
 * the CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1).
 */
unsigned int crc16 (unsigned char data[], size_t len)
{
  int i;
  unsigned int crc = 0;

  for (i = 0; i < len; i++)
  {
    crc = (unsigned char)(crc >> 8) | (crc << 8);
    crc ^= data[i];
    crc ^= (unsigned char)(crc & 0xff) >> 4;
    crc ^= (crc << 8) << 4;
    crc ^= ((crc & 0xff) << 4) << 1;
  }

  return crc;
}

/* Alternative implementation.
 * XXX - Does not work!
 */
unsigned int crc8_alt (unsigned char data[], size_t len)
{
   int i, j;
   unsigned char byte, temp, crc = 0;

   for (i = 0; i < len; i++)
   {
      byte = data[i];

      for (j = 0; j < 8; j++)
      {
         temp = (crc & 0x01) ^ byte;
         /* exclusive or least sig bit of current shift reg with the data bit */

         crc = crc >> 1;
         /* shift one place to the right */

         if (temp==1)
         {
            crc = crc ^ 0x8C; /* CRC ^ binary 1000 1100 */
         }
      }
   }

   return (unsigned int)crc;
}


/**
 * Calculates CRC8 checksum for an array of data.
 * The inner loop is where the CRC8 poly, X^8+X^5+X^4+1, operates.
 * the below crc8_table() implementation has pregenerated per-byte
 * values for a quicker version.
 */
unsigned int crc8 (unsigned char data[], size_t len)
{
   int i, j;
   unsigned char byte, temp, crc = 0;

   for (i = 0; i < len; i++)
   {
      byte = data[i];

      for (j = 8; j > 0; j--)
      {
         temp = ((crc ^ byte) & 0x01);
         if (temp == 0)
         {
            crc >>= 1;
         }
         else
         {
            crc ^= 0x18;
            crc >>= 1;
            crc |= 0x80;
         }
         byte >>= 1;
      }
   }

   return (unsigned int)crc;
}

unsigned int crc8_table (unsigned char data[], size_t len)
{
   unsigned char table[] = {
      0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83,
      0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
      0x9D, 0xC3, 0x21, 0x7F, 0xFC, 0xA2, 0x40, 0x1E,
      0x5F, 0x01, 0xE3, 0xBD, 0x3E, 0x60, 0x82, 0xDC,
      0x23, 0x7D, 0x9F, 0xC1, 0x42, 0x1C, 0xFE, 0xA0,
      0xE1, 0xBF, 0x5D, 0x03, 0x80, 0xDE, 0x3C, 0x62,
      0xBE, 0xE0, 0x02, 0x5C, 0xDF, 0x81, 0x63, 0x3D,
      0x7C, 0x22, 0xC0, 0x9E, 0x1D, 0x43, 0xA1, 0xFF,
      0x46, 0x18, 0xFA, 0xA4, 0x27, 0x79, 0x9B, 0xC5,
      0x84, 0xDA, 0x38, 0x66, 0xE5, 0xBB, 0x59, 0x07,
      0xDB, 0x85, 0x67, 0x39, 0xBA, 0xE4, 0x06, 0x58,
      0x19, 0x47, 0xA5, 0xFB, 0x78, 0x26, 0xC4, 0x9A,
      0x65, 0x3B, 0xD9, 0x87, 0x04, 0x5A, 0xB8, 0xE6,
      0xA7, 0xF9, 0x1B, 0x45, 0xC6, 0x98, 0x7A, 0x24,
      0xF8, 0xA6, 0x44, 0x1A, 0x99, 0xC7, 0x25, 0x7B,
      0x3A, 0x64, 0x86, 0xD8, 0x5B, 0x05, 0xE7, 0xB9,
      0x8C, 0xD2, 0x30, 0x6E, 0xED, 0xB3, 0x51, 0x0F,
      0x4E, 0x10, 0xF2, 0xAC, 0x2F, 0x71, 0x93, 0xCD,
      0x11, 0x4F, 0xAD, 0xF3, 0x70, 0x2E, 0xCC, 0x92,
      0xD3, 0x8D, 0x6F, 0x31, 0xB2, 0xEC, 0x0E, 0x50,
      0xAF, 0xF1, 0x13, 0x4D, 0xCE, 0x90, 0x72, 0x2C,
      0x6D, 0x33, 0xD1, 0x8F, 0x0C, 0x52, 0xB0, 0xEE,
      0x32, 0x6C, 0x8E, 0xD0, 0x53, 0x0D, 0xEF, 0xB1,
      0xF0, 0xAE, 0x4C, 0x12, 0x91, 0xCF, 0x2D, 0x73,
      0xCA, 0x94, 0x76, 0x28, 0xAB, 0xF5, 0x17, 0x49,
      0x08, 0x56, 0xB4, 0xEA, 0x69, 0x37, 0xD5, 0x8B,
      0x57, 0x09, 0xEB, 0xB5, 0x36, 0x68, 0x8A, 0xD4,
      0x95, 0xCB, 0x29, 0x77, 0xF4, 0xAA, 0x48, 0x16,
      0xE9, 0xB7, 0x55, 0x0B, 0x88, 0xD6, 0x34, 0x6A,
      0x2B, 0x75, 0x97, 0xC9, 0x4A, 0x14, 0xF6, 0xA8,
      0x74, 0x2A, 0xC8, 0x96, 0x15, 0x4B, 0xA9, 0xF7,
      0xB6, 0xE8, 0x0A, 0x54, 0xD7, 0x89, 0x6B, 0x35
   };
   int i;
   unsigned char tmp, crc = 0;

   for (i = 0; i < len; i++)
   {
      tmp = crc ^ data[i];
      crc = table[tmp];
   }

   return (unsigned int)crc;
}

unsigned int xor (unsigned char data[], size_t len)
{
  int i;
  unsigned int crc = 0;

  for (i = 0; i < len; i++)
  {
    crc ^= data[i];
  }

  return crc;
}

static void usage (int status)
{
   printf ("Usage: %s [options] file1 .. fileN\n", program_name);
   printf ("Simple CRC and checksum testing tool.\n\
Options:\n\
-?, --help           Display brief help (this).\n\
-V, --verbose        Verbose output.\n\n\
-8, --crc8           Calculate CRC8 for all input files (default).\n\
-q, --crc8-quick     Calculate CRC8 based on table lookup (quicker than regular CRC8).\n\
-a, --crc8-alt       Calculate CRC8 based on an alternative algorithm.\n\
-c, --crc16          Calculate CRC16 for all input files.\n\
-C, --crc32          Calculate CRC32 for all input files.\n\
-x, --xor            Calculate simple XOR checksum for all input files.\n\n\
-h, --hex            Output CRC in hexadecimal (default).\n\
-d, --dec            Output CRC in decimal.\n\
");
   exit (status);
}

int main (int argc, char *argv[])
{
  int result, file = 1, hex = 1;
   int c, i, r, val;
   int option_index;
   char *filename;
   struct stat fs;
   FILE *fp;
   char *buf;

   static struct option long_options[] =
      {
         {"help",     no_argument, 0, '?'},
         {"verbose",  no_argument, 0, 'V'},
         {"crc32", no_argument, 0, 'C'},
         {"crc16", no_argument, 0, 'c'},
         {"crc8", no_argument, 0, '8'},
         {"crc8-quick", no_argument, 0, 'q'},
         {"crc8-alt", no_argument, 0, 'a'},
         {"xor", no_argument, 0, 'x'},
         {"hex", no_argument, 0, 'h'},
         {"dec", no_argument, 0, 'd'},
         /* insert your option here */
         {0, 0, 0, 0}
      };

   /* Set program name before we mess up argv with getopt. */
   program_name = argv[0];

   /* Default checksum function */
   checksum_func = crc8;

   while (1)
   {
      option_index = 0; /* Index in long_options[] */
      /* "W:" in optstring is reserved by POSIX compliant getopt()'s */
      c = getopt_long (argc, argv,
                       "Cc8xq"
                       "hd"
                       "V"       /* Verbose ... */
                       "?",      /* Help */
                       long_options, &option_index);
      if (-1 == c) break;
      switch (c)
      {
         case '8':              /* CRC8 */
            checksum_func = crc8;
            break;

         case 'q':              /* CRC8 */
            checksum_func = crc8_table;
            break;

         case 'a':
            checksum_func = crc8_alt;
            break;

         case 'c':              /* CRC16 */
            checksum_func = crc16;
            break;

         case 'x':              /* XOR */
            checksum_func = xor;
            break;

         case 'h':              /* HEX */
            hex = 1;
            break;

         case 'd':              /* DEC */
            hex = 0;
            break;

         case '?':
         default:
            usage (EXIT_SUCCESS);
            break;
      }
   }

   for (file = optind; file < argc; file++)
   {
     filename = argv[file];
     result = stat (filename, &fs);
     if (result) err (-1, "Cannot read size of %s", filename);

     fp = fopen (filename, "r");
     if (!fp) err (-1, "Failed opening %s", argv[1]);

     buf = (char *)malloc (fs.st_size);
     if (!buf) err (-1, "Failed allocating buffer memory.");

     result = fread (buf, 1, fs.st_size, fp);
     if (result != fs.st_size) err (-1, "Failed reading file.");

     fclose (fp);

     printf (hex ? "0x%02X" : "%u", checksum_func (buf, fs.st_size));
     printf ("\t%s\n", filename);
  }

  return 0;
}

/**
 * Local Variables:
 *  c-file-style: "ellemtel"
 *  indent-tabs-mode: nil
 * End:
 */
