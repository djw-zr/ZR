
/*
 *   Rewrite of getline for systems where the routine is missing.
 *   Note:  The routine never adds a terminating null byte.
 */

//#include "file.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <errno.h>
//#include <string.h>


ssize_t getline1(char **lineptr, size_t *n, FILE *fp){

char    *p, *p_end,
        *nlineptr  ;
int     c   ;
size_t  nn  ;  // Size of new line buffer
ssize_t nc  ;  // Number of characters in buffer

//  Allocate memory if required

      if(NULL == *lineptr && 0 == *n){
        *n = 256;
        if(NULL == (*lineptr = malloc(*n))){
          errno = ENOMEM ;
          return -1      ;
        }
      }

//  Check parameters

      if(NULL == *lineptr || 0 == *n){
        errno = EINVAL ;
        return -1      ;
      }

//  Loop over input

      p = *lineptr          ;
      p_end = *lineptr + *n ;
      for (;;) {
        c = fgetc(fp);

//  Process end of input

        if (c == -1) {
          if (feof(fp)) return (p == *lineptr) ? -1 : p - *lineptr;
          else          return -1 ;
        }

//  Check for end of line

        *p++ = c;
        if (c == '\n') {
          *p = '\0'          ;
          return p - *lineptr;
        }

//  Reallocate buffer if it is almost full

        if (p + 2 >= p_end) {
          nc = p - *lineptr ;  //  Number of characters in buffer ;
          nn = *n * 2       ;  //  New soze of buffer

          if (NULL == (nlineptr = realloc(*lineptr, nn))) return -1;

          *lineptr = nlineptr;
          *n = nn;
          p_end = nlineptr + nn ;
          p     = nlineptr + nc ;
        }
      }
}

