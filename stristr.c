/**
 * stristr - Case insensitive strstr()
 * @haystack: Where we will search for our @needle
 * @needle:   Search pattern.
 *
 * Description:
 * This function is an ANSI version of strstr() with case insensitivity.
 *
 * It is a commodity funciton found on the web, cut'd, 'n', pasted..
 * URL: http://www.brokersys.com/snippets/STRISTR.C
 *
 * Hereby donated to public domain.
 *
 * Returns:  char *pointer if needle is found in haystack, otherwise NULL.
 *
 * Rev History:  01/20/05  Joachim Nilsson   Cleanups
 *               07/04/95  Bob Stout         ANSI-fy
 *               02/03/94  Fred Cole         Original
 */

static inline char *
stristr (const char *haystack, const char *needle)
{
   char *pptr  = (char *) needle;   /* Pattern to search for    */
   char *start = (char *) haystack; /* Start with a bowl of hay */
   char *sptr;                      /* Substring pointer        */
   int   slen  = strlen (haystack); /* Total size of haystack   */
   int   plen  = strlen (needle);   /* Length of our needle     */

   /* while string length not shorter than pattern length */
   for (; slen >= plen; start++, slen--)
   {
      /* find start of pattern in string */
      while (toupper (*start) != toupper (*needle))
      {
         start++;
         slen--;
         /* if pattern longer than string */
         if (slen < plen)
         {
            return NULL;
         }
      }

      sptr = start;
      pptr = (char *) needle;
      while (toupper (*sptr) == toupper (*pptr))
      {
         sptr++;
         pptr++;
         /* if end of pattern then pattern was found */
         if ('\0' == *pptr)
         {
            return start;
         }
      }
   }

   return NULL;
}
