/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  system_alt.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   System utility functions used by ZR
 *
 *   This file contains alternative versions of C library (glibc) routines
 *   which are not supported in all versions of C.
 *
 *==============================================================================
 */
#if defined MinGW

#include <sys/stat.h>

/*
 *   Minimal replacement for routine 'realpath'.
 *   The version makes no changes to the filename.
 */
#if 1
char *realpath1(const char *path, char *resolved_path){

  char my_name[] = "REALPATH" ;
  char *new_path ;
  int  n ;
//  If using MinGW under linux use the real 'realpath()' to handle
//  the /sys directory pointers
#ifndef WINDOWS
      if(!strncmp(path,"/sys",4)){
        return realpath(path,resolved_path) ;
      }
#endif
//
      new_path = strdup(path) ;
      if(resolved_path == NULL){
        return new_path ;
      }
      n = strlen(resolved_path) -1;
      strncpy(resolved_path,new_path,n) ;
      resolved_path[n] = '\0' ;
      free(new_path) ;
      return resolved_path ;
}
#endif
/*
 *   Routines using struct dirent.
 *     Defined in /usr/include/bits/dirent.h
 *   struct dirent{
 *     int                d_ino       ;  // May be int64 on ...
 *     int                d_off       ;  //     ... some systems
 *     unsigned short int d_reclen    ;
 *     unsigned char      d_type      ;
 *     char               d_name[256] ;
 *     }
 */

/*
 *  Replacement for alphasort
 */
static int alphasort1(const void *p1, const void *p2){
/*
 * The actual arguments to this function are "pointers to
 *  dirent structures", but strcmp(3) arguments are "pointers
 *  to char", hence the following
 */
char *my_name = "alphasort (zr version)";
struct dirent  **d1, **d2 ;

      d1 =  (struct dirent **) p1 ;
      d2 =  (struct dirent **) p2 ;

      return strcmp((*d1)->d_name, (*d2)->d_name) ;
}

/*
 *   Replacement for scandir
 *   dir_name  :: pointer to name of directory
 *   namelist  :: pointer to array of pointers to filenames
 *   filter    :: filter applied to each dirent structure
 *   compar    :: compare (file names) in two dirent structures
 *
 *   This version sorts the list useing routine alphasort1 (above)
 *        instead of the routine referenced by 'compar'.
 */
//  GCC pragmas to ignore unused parameter in this section only
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

int scandir(const char *dir_name, struct dirent ***namelist,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **)){

#pragma GCC diagnostic pop

int  n, i ;
int  ip = 0    ;    // DEBUG

struct dirnode{               //  Linked list of dirent
   struct dirnode *next     ;
   struct dirent  dir_entry ;
} ;

DIR             *root_dir     ;  // Pointer to a directory (DIR) structure
struct dirent   *dir_point    ;  // Pointer to structure dirent
struct dirent  **dir_array    ;  // Pointer to array of pointers
struct stat      sb           ;

struct dirnode  *d_node_first ;  // Pointer to d_node structure
struct dirnode  *d_node_last  ;
struct dirnode  *d_node       ;
struct dirnode  *d_node_array ;  // Pointer to array of pointers
char my_name[] = "scandir (zr version)" ;

      if(ip)printf("  Enter %s\n",my_name );
      d_node_first = NULL ;
      d_node_last  = NULL ;
/*
 *  1.  Open directory :: [errno defined in error.h]
 */
      if(ip)printf("  Open dir %s\n",dir_name );

      root_dir = opendir(dir_name) ;
      if(root_dir == NULL){
        return errno ;     //  Return (non-zero) error from opendir
      }
/*
 *  2.  Scan directory looking for regular files,  Each file that
 *      passes 'filter' is added to a linked list and the list
 *      length counted.
 *
 *     Windows probably needs _stat, a full pathname, _S_IFMMT and _S_IFREG.
 *             of which the full pathname is most likely.
 *     Directory entries . and .. should be skipped.
 */
      n = 0 ;
      for(;;){
        dir_point = readdir(root_dir) ;
        if(dir_point == NULL) break   ;            //  End of directory entries

        if(ip)printf(" Routine %s, readdir returns file: %s\n",my_name,dir_point->d_name) ;
/*
 *  Windows MinGW does not save he full path name in d_name
 *  The folllowing code corrects this
 */
        {
  int   n1, n2 ;
  char *full_name ;
          n1 = strlen(dir_name);
          n2 = strlen(dir_point->d_name) ;
          full_name = (char *)malloc(n1+n2+2) ;
          strcpy(full_name,dir_name) ;
          if(dir_name[n1-1]!='/')strcat(full_name,"/") ;
          strcat(full_name,dir_point->d_name) ;
          if(ip)printf(" Routine %s, readdir full_name   : %s\n",my_name,full_name) ;
/*
 *  Check for file - skip mising files
 */

          if(stat(full_name,&sb) == -1){
            printf("Routine scandir ::File = %s\n",full_name) ;
            perror("Routine scandir : call to stat()");
            free(full_name) ;
            continue ;
//          exit(EXIT_FAILURE) ;
          }
          free(full_name) ;
        }

        if((sb.st_mode & S_IFMT) != S_IFREG)continue ; // Not a regular file

//        if(dir_point->d_type != DT_REG)continue ;  //  Not a regular file
        if(filter != NULL){
          if(filter(dir_point) == 0) continue   ;  //  Does not match filter
        }

//  Dummy commmand involving dir_point to sidestep (possible)
//                                                 out of bounds compiler error

        if(0 == strlen(dir_point->d_name))continue ;

//  Crete a new linked list node and fill from structure dirent

        d_node = (struct dirnode *)malloc(sizeof(struct dirnode));
        d_node->dir_entry = *dir_point  ;          //  Out of bounds error (valgrind)

//  Set pointers connecting linked list and count number of items

        if(d_node_first == NULL){
          d_node_first = d_node ;
        }else{
          d_node_last->next = d_node ;
        }
        d_node_last = d_node ;
        d_node->next = NULL ;
        n++ ;
      }
      closedir(root_dir) ;
/*
 *  3.  Generate simple array of dirent structures from linked list
 */

//  Allocate space for an array of ponters to dirent structures

      dir_array = (struct dirent **)malloc(n*sizeof(struct dirent *)) ;

//  Loop over linked list

      n = 0 ;
      d_node_last = NULL ;
      for(d_node = d_node_first; d_node != NULL ; d_node=d_node->next){

//  Allocate space for a new dirent entry and copy data across

        dir_array[n] = (struct dirent *)malloc(sizeof(struct dirent)) ;
        *(dir_array[n]) = d_node->dir_entry  ;                 //  Copy

//  Free linked list storage

        free(d_node_last)       ;
        d_node_last = d_node    ;
        n++ ;
      }
      free(d_node_last) ;
/*
 *  4.  Sort array of structures using alphasort1
 */
      qsort(dir_array, n, sizeof(struct dirent *), alphasort1) ;
/*
 *  Debug printing on exit
 */
      if(ip){
        for(i=0;i<n;i++)
          printf(" %s  name = %i,  %s\n",my_name,i,dir_array[i]->d_name) ;
        printf("  Exit  %s\n",my_name );
      }
      *namelist = dir_array ;
      return n ;        //  Number of names in list
}
#endif
