#include "fcaseopen.h"

#if !defined(_WIN32)
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>

/*
 *   Given a file name path, the routine searches for the first
 *   file name that matches when the case of each character is ignored.
 *   If the search is unsuccessful, the routine returns 0.
 *   If successful it returns 1 and 'r' is set to the successful
 *   filename.
 *   On entry r must have the length of path plus 2 bytes
 */

static int casepath(char const *path, char *r){

  size_t l      ;
  size_t rl = 0 ;
  int  last = 0 ;
  int  ip = 0 ;
  char *p ;
  char *c ;
  DIR  *d ;
  struct dirent *e ;
  char *my_name = "casepath" ;

      if(ip){
        printf("\n***********************************************\n") ;
        printf("  Enter routine %s\n",my_name) ;
        printf("        path =  %s\n",path)    ;
        printf("        r    =  %s\n",r)       ;
      }

      l = strlen(path)  ;
      p = alloca(l + 1) ;
      strcpy(p, path)   ;

      if (p[0] == '/'){
          d = opendir("/");
          p = p + 1;
      }else{
          d = opendir(".");
          r[0] = '.';
          r[1] = 0;
          rl = 1;
      }

      c = strsep(&p, "/");
      while (c){
        if(ip)printf("    Directory c = %s\n",c) ;
        if(!d){
          return 0;
        }

        if(last){
          closedir(d);
          return 0;
        }

        r[rl] = '/';
        rl += 1;
        r[rl] = 0;

        e = readdir(d);
        while (e){
          if(ip)printf("      Directory e = %s\n",e->d_name) ;
          if (strcasecmp(c, e->d_name) == 0){
            strcpy(r + rl, e->d_name);
            rl += strlen(e->d_name);

            closedir(d);
            d = opendir(r);
            break;
          }
          e = readdir(d);
        }

        if (!e){
          strcpy(r + rl, c);
          rl += strlen(c);
          last = 1;
        }
        c = strsep(&p, "/");
      }

      if (d) closedir(d);
      return 1;
}
#endif

/*
 *  Given a file name, the routine searches for a file with a name
 *  that matches ignoring character case.  If successful it opens
 *  the file using fopen.
 */

FILE *fcaseopen(char const *path, char const *mode){

int  ip = 0 ;
FILE *fp = fopen(path, mode);
char *r = NULL ;;

      if(ip)printf(" fcaseopen : 1.  initial path = %s, fp = %p\n",path,(void *)fp) ;
#if !defined(_WIN32)
      if (!fp){
        r = alloca(strlen(path) + 2);
        if (casepath(path, r)){
          fp = fopen(r, mode);
          if(ip)printf(" fcaseopen : 2.  final path = %s, fp = %p\n",r,(void *)fp) ;
        }
      }
#endif
      if(ip)printf(" fcaseopen : 3.  fp = %p\n",(void *)fp) ;
      return fp;
}

/*
 *  Given a directory name, the routine searches for a dirextory with
 *  a name that matches ignoring character case.  If successful it switches
 *  to that directory using chdir.
 */

void casechdir(char const *path){

#if !defined(_WIN32)
char *r = alloca(strlen(path) + 2);

      if (casepath(path, r)){
          chdir(r);
      }else{
          errno = ENOENT;
      }
#else
      chdir(path);
#endif
      return ;
}
