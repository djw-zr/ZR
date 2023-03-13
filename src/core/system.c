/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  system.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   System utility functions used by ZR
 *
 *==============================================================================
 */

/*
 *   Initialise system variables and those not initialised elsewhere
 */
int init_system(void){
int i, j ;
/*
 *  Initialise clocks
 */
      for(j=0;j<5;j++){
        for(i=0;i<4;i++)
          zr_clock_time[i][j] = 0.0 ;
      }
/*
 * Initialise truetype/freetype
 */
#ifdef zr_freetype
       freetype_init() ;
#endif

      return 0 ;
}

int   nint(double d){

int i ;

     if(d >= 0.0){
       i = d + 0.5 ;
       return i ;
     }
     i = 0.5 - d ;
     return -i ;
}

/*
 *   Function to convert string to lower case
 */
void str2lc(char *string){

unsigned int i ;

      if(!string) return ;
      for(i=0;i<strlen(string);i++)string[i]=tolower(string[i]) ;
      return ;
}

/**
 *   Function to compare two strings ignoring case.
 *   Returns 0 if the strings match and 1 if the comparison fails.
 */
int strcmp_ic(const char *s1, const char *s2){

unsigned int i, l1  ;
char         c1, c2 ;

     l1 = strlen(s1) ;
     if(strlen(s2) != l1) return 1 ;   //  No match - lengths different
     for(i=0;i<l1;i++){
       c1 = s1[i]  ;
       c2 = s2[i]  ;
       c1 = tolower(c1) ;
       c2 = tolower(c2) ;
       if(c1 != c2) return 1 ;
     }
     return 0 ;
}

int strncmp_ic(const char *s1, const char *s2, int nn){

unsigned int i, l1, l2  ;
char         c1, c2 ;

     l1 = strlen(s1) ;
     l2 = strlen(s2) ;
     if(l1 != l2 && ((int)l1 < nn || (int)l2 < nn) ) return 1 ;
     if(nn<(int)l1) l1 = nn ;

     for(i=0;i<l1;i++){
       c1 = s1[i]  ;
       c2 = s2[i]  ;
       c1 = tolower(c1) ;
       c2 = tolower(c2) ;
       if(c1 != c2) return 1 ;
     }
     return 0 ;
}

/*
 *  Routine to return a new string with start and end quotes removed.
 */

char *strip_quotes(char *string1){

unsigned int n, i ;
char         *string2 ;

      n = strlen(string1) ;
      if(string1[0] != '"' || string1[n-1] != '"') return string1 ;
      string2 =strndup(&string1[1],n-2) ;
      free(string1) ;
      return string2 ;
}

/*
 *  Routine to compare strings with start and end quotes removed.
 */

int strcmp_nq(char *s1a, char *s2a){

int  iret ;
char *s1 = strdup(s1a) ;
char *s2 = strdup(s2a) ;

      s1 = strip_quotes(s1);
      s2 = strip_quotes(s2);
      iret = strcmp(s1,s2) ;
      free(s1) ;
      free(s2) ;
      return iret ;
}
/*
 *  Routine to compare strings with start and end quotes removed.
 */

int strcmp_nqic(char *s1a, char *s2a){

int  iret ;
char *s1 = strdup(s1a) ;
char *s2 = strdup(s2a) ;

      s1 = strip_quotes(s1);
      s2 = strip_quotes(s2);
      iret = strcmp_ic(s1,s2) ;
      free(s1) ;
      free(s2) ;
      return iret ;
}

/*
 * Routine to strip quotes and any extension from a filename
 * and convert to lower case.
 *
 * Designed to convert names of textures and other MSTS strings
 * to a consistent format
 */
void  zr_str2lcnqne(char *string){

  int i, j, n ;
      n = strlen(string) ;
      for(i=0,j=0;i<n;i++){
        if(string[i] == '"')  ;
        else if(string[i] == '.'){
          break ;
        }else{
          string[j++] = tolower(string[i]) ;
        }
      }
      string[j] = '\0' ;
      return ;
}

/**
 *  char *zr_basename(char *fname)
 *
 *  Returns a copy of fname with all directories removed
 *
 *  zr version of basename used because of the differences between
 *  versions of the standard basename routine.  This always returns
 *  a string but it can be the null string "".
 *  The string is created using malloc.  The space should be freed
 *  when/if no longer needed.
 */

char *zr_basename(char *fname){

char  *pz ;
      if(fname == NULL) return strdup("") ;
      pz = strrchr(fname,'/') ;
      if(pz == NULL) return strdup(fname) ;
      pz++ ;
      return strdup(pz) ;         // malloc
}

/**
 *  char *zr_basename2(char *fname)
 *
 *  Returns a copy of fname without the extension and with all directories
 *  removed
 *
 *  zr version of basename used because of the differences between
 *  versions of the standard basename routine.  This always returns
 *  a string but it can be the null string "".
 *  The string is created using malloc.  The space should be freed
 *  when/if no longer needed.
 */

char *zr_basename2(char *fname){

char  *pz, *string1, *string2 ;
      if(fname == NULL) return strdup("") ;
      pz = strrchr(fname,'/') ;
      if(pz == NULL){
        string1 = strdup(fname) ;
      }else{
        pz++ ;
        string1 = strdup(pz) ;
      }
      pz = strrchr(string1,'.')  ;
      if(pz != NULL) *pz = '\0' ;
      string2 = strdup(string1) ;
      free(string1) ;
      return string2 ;         // malloc
}


/**
 *  char *zr_extension(char *fname)
 *
 *  Routine to return copy of filename extension  i.e. jpg
 */

char *zr_extension(char *fname){

char *pz ;
      if(fname == NULL) return strdup("") ;
      pz = strrchr(fname,'.') ;
      if(pz == NULL) return strdup("") ;
      pz++ ;
      return strdup(pz) ;         // malloc
}

/**
 *  char *zr_corename(char *fname)
 *
 *  Routine to return copy of file fname without extension
 */

char *zr_corename(char *fname){

int  n   ;
char *pz, *corename ;
      if(fname == NULL) return strdup("")   ;   // malloc
      pz = strrchr(fname,'.')               ;
      if(pz == NULL) return strdup(fname)   ;   // malloc
      n = strlen(fname) - strlen(pz)        ;
//      printf(" zr_corename  strlen(fname) = %i\n",(int)strlen(fname)) ;
//      printf(" zr_corename  strlen(pz)    = %i\n",(int)strlen(pz)) ;
      corename = (char *)malloc((n+1)*sizeof(char)) ;
      strncpy(corename,fname,n) ;
      corename[n] = '\0' ;
//      return strndup(fname, n) ;
      return corename ;
}

/**
 *  char *zr_parentdir(char *fname)
 *
 *  Routine to return name of parent directory
 *    If fname is "A/B/C", returns "B".
 *    If fname is NULL or "C" returns NULL.
 */

char *zr_parentdir(char *fname){

  char *string1, *string2 ;
  char *pz ;

      if(fname == NULL) return strdup("")   ;
      string1 = strdup(fname) ;                 //malloc
      pz = strrchr(string1,'/') ;
      if(pz==NULL){free(string1); return NULL ;}

      *pz = '\0' ;
      pz = strrchr(string1,'/') ;
      if(pz==NULL){free(string1); return NULL ;}
      pz++ ;
      string2 = strdup(pz) ;                    // malloc
//      printf(" zr_parentdir : string1 = %s\n",string1) ;
//      printf(" zr_parentdir : string2 = %s\n",string2) ;
      free(string1) ;
      return string2 ;
}

/**
 *  char *zr_full_parentdir(char *fname)
 *
 *  Routine to return full name of parent directory
 *    If fname is "/A/B/C", returns "/A/B".
 *    If fname is "/A", returns "/".
 *    If fname is "A" returns NULL
 */

char *zr_full_parentdir(char *fname){

  char *string ;
  char *pz ;

      if(fname == NULL) return NULL  ;

      string = strdup(fname)   ;                 //malloc
      pz = strrchr(string,'/') ;
// If '/' is not found
      if(pz==NULL){free(string); return NULL ;}  // '/' not found
// If fname is in top directory
      if(pz==string){
        free(string) ;
        string = strdup("/") ;
        return string ;
      }
      *pz = '\0'     ;
      return string ;
}



/**
 *  char *zr_find_msfile(char *fname)
 *
 *  In Microsoft style filenames, either uppercase or lowercase characters
 *  may be used to represent the same filename.  This routine first checks
 *  to see if a file exists.  If the file does not exist it checks for the
 *  parent directory and then searches the current directory for a matching
 *  filename, ignoring differences between upped and lower case.
 *
 */

#if 0
char *zr_find_msfile(char *fname){

unsigned int  i, j, n, m, ok  ;
unsigned int  ip = 0          ;  // Debug
char *base,             //  File name in last directory
     *parent,           //  Full pathname of parent directory
     *string      ;
DIR  *dp          ;
FILE *fp          ;
struct dirent *di ;
char *my_name = "zr_find_msfile" ;
/*
 *  Check if file exists
 */
      if(ip)printf(" Enter routine %s, File = %s\n",my_name,fname) ;
      fp = fopen(fname,"r") ;
      if(fp!=NULL){  // File exists
        fclose(fp) ;
        return strdup(fname)   ;
      }
/*
 *  If not search for parent  [Use "stat(filename)" to check it exists]
 */
      parent = zr_full_parentdir(fname) ;  // malloc
      if(ip)printf(" Routine %s, AA parent = %s\n",my_name,parent) ;
      if(parent==NULL){
        parent = strdup(".") ;
      }else if(!strcmp(parent,"/")){
        *parent = '\0'       ;
      }else{
        fp = fopen(parent,"r") ;
        if(fp == NULL){
          string = zr_find_msfile(parent) ;
          free(parent) ;
          if(string == NULL) return NULL ; // Error return
          parent = string ;
        }else{
          fclose(fp) ;
        }
      }
      if(ip)printf(" Routine %s, BB parent = %s\n",my_name,parent) ;
/*
 * Check again for file
 */
      base = zr_basename(fname)        ;  // malloc
      n      = strlen(base) + strlen(parent) + 2 ;
      string = (char *)malloc(n*sizeof(char))    ;
      strcpy(string,parent) ;
      strcat(string,"/")    ;
      strcat(string,base)   ;
      if(ip)printf(" Routine %s, CC file = %s\n",my_name,string) ;
      fp = fopen(string,"r") ;
      if(fp!=NULL){  // Fle exists
        fclose(fp)   ;
        free(parent) ;
        free(base)   ;
        return string   ;
      }
/*
 *  Search parent directory for matching file
 */
      n = strlen(base) ;
      for(i=0;i<n;i++) base[i]=tolower(base[i]) ;

      dp = opendir(parent) ;
      ok = 0 ;
      while((di=readdir(dp))!= NULL){
        free(string) ;
        string = strdup(di->d_name) ;
        for(i=0;i<strlen(string);i++) string[i] = tolower(string[i]) ;
        ok = !strcmp(string,base) ;
        if(ok)break ;
      }
      free(string) ;
      string = strdup(di->d_name) ;
      closedir(dp) ;
/*
 *  Success ??
 */
      if(ok){
        if(ip)printf("  Routine %s, AA File found.  File = %s\n",
                                                     my_name,string) ;
        free(base)    ;
        base = string ;
        n    = strlen(base) + strlen(parent) + 2 ;
        string = (char *)malloc(n*sizeof(char))  ;
        strcpy(string,parent) ;
        strcat(string,"/")    ;
        strcat(string,base)   ;
        if(ip)printf("  Routine %s, BB File found.  File = %s\n",
                                                     my_name,string) ;
      }else{
        if(ip)printf("  Routine %s, File not found.  File = %s\n",
                                                     my_name,fname) ;
        free(string)  ;
        string = NULL ;
      }
      free(base)   ;
      free(parent) ;
      return string  ;
}
#endif

/*
 *   Second version of zr_find_msfile.  Given a filename it tries to find
 *   an existing file or directory with the same name.  If that does not
 *   exists it first checks that the parent directory exists and then
 *   looks for a matching name in the current directory.
 */

int zr_find_msfile2(char *fname){

unsigned int  i, n, ok, iret  ;
unsigned int  ip = 0          ;  // Debug
char *base   = NULL,             //  File name in last directory
     *parent = NULL,             //  Full pathname of parent directory
     *string = NULL     ;
DIR  *dp          ;
struct dirent *di ;
struct stat   sb  ;

char *my_name = "zr_find_msfile2" ;
/*
 *  Check if file exists
 */
      if(ip)printf("\n  Enter routine %s.     Search for = %s\n",my_name,fname) ;
      fflush(NULL) ;
      iret = stat(fname, &sb) ;
      if(!iret){
        if(ip)printf("     File/Directory  exists!\n") ;
        return 0      ;    //  File or directory exists
      }
/*
 *  Check parent exists  [Use "stat(filename)" to check it exists]
 */
      parent = zr_full_parentdir(fname) ;  // malloc parent
      if(ip){
        printf("    File/Directory does not exist\n") ;
        printf("    Parent = %s\n",parent) ;
      }
      if(parent==NULL){
        parent = strdup(".") ;
      }else if(!strcmp(parent,"/")){
        *parent = '\0'       ;
      }else{
        iret = zr_find_msfile2(parent) ;
        if(iret){
          free(parent) ;
          return 1 ;  // Parent does not exist
        }
      }
      if(ip){
        printf("\n    Parent found = %s\n",parent) ;
        fflush(NULL) ;
      }
/*
 * Regenerate filename
 */
      base = zr_basename(fname)        ;  // malloc
      n = strlen(parent)   ;
      strcpy(fname,parent) ;
      if(parent[n-1]!='/')strcat(fname,"/")    ;
      strcat(fname,base)   ;
      if(ip)printf("    New fname = %s\n",fname) ;
/*
 * Check again for file
 */
      iret = stat(fname,&sb) ;
      if(!iret){
        free(parent) ;
        free(base)   ;
        free(string) ;
        if(ip)printf("     File/Directory  exists!\n") ;
        return 0     ;    //  File or directory exists
      }
/*
 *  Search parent directory for matching file
 */
      if(ip){
        printf("     File/Directory  does not exist\n") ;
        printf("     Search for matching item  %s\n",base) ;
        printf("                 in directory  %s\n",parent) ;
      }
      n = strlen(base) ;
      for(i=0;i<n;i++) base[i]=tolower(base[i]) ;

      dp = opendir(parent) ;
      ok = 0 ;
      while((di=readdir(dp))!= NULL){
        free(string) ;
        string = strdup(di->d_name) ;
        for(i=0;i<strlen(string);i++) string[i] = tolower(string[i]) ;
        ok = !strcmp(string,base) ;
        if(ok)break ;
      }
/*
 *  Failure ??
 */
      if(!ok){
        closedir(dp) ;
        free(parent) ;
        free(base)   ;
        free(string) ;
        if(ip)printf("     Routine %s, DD File not found\n", my_name) ;
        return 1     ;  // File does not exist
      }
/*
 *  Success
 */
      free(base) ;
      base = strdup(di->d_name) ;
      closedir(dp) ;
      n = strlen(parent)   ;
      strcpy(fname,parent) ;
      if(parent[n-1]!='/')strcat(fname,"/")    ;
      strcat(fname,base)   ;
      if(ip)printf("  Routine %s, DD File found.  File = %s\n",
                                                     my_name,fname) ;
      free(parent) ;
      free(base)   ;
      free(string) ;

      return 0  ;
}



/**
 *  char *zr_filename_MS2L(char *fname)
 *
 *  Routine to replace '\' by '/' in filenames and to correct case of extension.
 *  This is because files referred to as name.s may be name.S etc.
 */

int zr_filename_MS2L(char *fname){

int  i, j, n  ;

      n = strlen(fname) ;
      for(i=0,j=0;i<n;i++,j++){
        if(fname[i] == '\\')fname[j] = '/' ;
        if(fname[i] == '/' && fname[i+1] == '/')i++ ;   // Convert "//" to "/"
      }
      fname[j] = '\0' ;
      return 0 ;
}

char *zr_to_upper(char *string){

 int i, n ;
 static char *upper = NULL;

     if(upper)free(upper);
     upper = NULL ;
     n = strlen(string) ;
     upper = (char *)malloc(n+1) ;
     for(i=0;i<n;i++)upper[i]=toupper(string[i]);
     upper[n]='\0' ;
     return upper ;
}

//#ifdef MinGW
#if 0
/*
 *   Routines included for systems, such as MinGW, where the corresponding
 *   glibc (gnu c library) routine is missing.
 *
 *   These are usually simplified versions of the gnu routines
 */

/*
 *   Replacement for routine 'realpath'.
 *   The version makes no changes to the filename.
 */

char *realpath(const char *path, char *resolved_path){

char my_name[] = "REALPATH" ;

//      printf("  Enter %s\n",my_name );
      resolved_path = strdup(path) ;
      return resolved_path ;
}

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
 */

int scandir(const char *dir_name, struct dirent ***namelist,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **)){

int  n, i ;
int  ip = 0    ;    // DEBUG
DIR  *root_dir ;    // Pointer to a directory (DIR) structure

struct dirent  *dir_point ;   // Pointer to structure dirent
struct dirent **dir_array ;   // Pointer to array of pointers

struct dirnode{               //  Linked list of dirent
   struct dirnode *next     ;
   struct dirent  dir_entry ;
} ;

struct dirnode *d_node_first ;
struct dirnode *d_node_last  ;
struct dirnode *d_node ;
struct dirnode *d_node_array ;
char my_name[] = "scandir (zr version)" ;

      if(ip)printf("  Enter %s\n",my_name );
      d_node_first = NULL ;
      d_node_last  = NULL ;
/*
 *  1.  Open directory :: [errno defined in error.h]
 */
      root_dir = opendir(dir_name) ;
      if(root_dir == NULL){
        return errno ;     //  Return (non-zero) error from opendir
      }
/*
 *  2.  Scan directory looking for regular files,  Each file that
 *      passes 'filter' is added to a linked list and the list
 *      length counted.
 */
      n = 0 ;
      for(;;){
        dir_point = readdir(root_dir) ;
        if(dir_point == NULL) break   ;            //  End of directory entries

        if(dir_point->d_type != DT_REG)continue ;  //  Not a regular file
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
 *  4.  Sort array of structures using alphasort
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
      return n ;
}
#endif



/**
 *
 * Function to return an estimate of the number of times it is
 * called each second - averaged over the last 5 seconds.
 * It is used to estimate the frames per second
 */

int calls_per_second(){

int  fps, i, time_now ;
static int icount = 0 ;
static int lasta  = -1 ;
static int time_last = 0 ;
static int icounta[] = {0, 0, 0, 0, 0} ;

      icount++ ;
      time_now = time(NULL);
      if(time_last == 0)time_last = time_now ;
      if(time_last != time_now){
        lasta++ ;
        if(lasta>=5)lasta = 0 ;
        icounta[lasta] = icount ;
        icount = 0 ;
        time_last = time_now ;
        fps = 0 ;
        for(i=0;i<5;i++) fps = fps + icounta[i];
        return fps/5;
      }
      return 0 ;
}

/**
 *
 * Function to check the properties of a MSTS 4x3 matrix.
 * It returns 0 if it is a unit diagonal matrix,
 *            1 if it represents translation only,
 *            2 if it represents rotation and/or stretching,
 *            3 if it represents both.
 */
enum MatrixType check_matrix4x3(Matrix4x3 *m){

int k = 0 ;

      if(m->AX != 1.0 || m->AY != 0.0 || m->AZ != 0.0 ||
         m->BX != 0.0 || m->BY != 1.0 || m->BZ != 0.0 ||
         m->CX != 0.0 || m->CY != 0.0 || m->CZ != 1.0) k = 2  ;
      if(m->DX != 0.0 || m->DY != 0.0 || m->DZ != 0.0) k++    ;
      switch (k){
        case 0: return MAT_UNIT      ;
        case 1: return MAT_TRANSLATE ;
        case 2: return MAT_ROTATE    ;
      }
      return MAT_GENERAL ;
}

/**
 *
 * Function to convert MSTS 4x3 transformation matrix
 *          to an OpenGl vector representation of a 4x4 matrix
 */

int msts4x3_to_opengl4x4(float m[16], Matrix4x3 *m1){

      m[ 0] = m1->AX ;
      m[ 1] = m1->AY ;
      m[ 2] = m1->AZ ;
      m[ 3] = 0.0    ;
      m[ 4] = m1->BX ;
      m[ 5] = m1->BY ;
      m[ 6] = m1->BZ ;
      m[ 7] = 0.0    ;
      m[ 8] = m1->CX ;
      m[ 9] = m1->CY ;
      m[10] = m1->CZ ;
      m[11] = 0.0    ;
      m[12] = m1->DX ;
      m[13] = m1->DY ;
      m[14] = m1->DZ ;
      m[15] = 1.0    ;
      return 0 ;
}

/*
 *  Function to mimic the OpenGL function glRotate.
 *  This uses the transformation recipe given in file
 *    README.ROTATION_MATRIX2
 *  Input
 *    ang         :  Rotation angle (degrees)
 *    ax, ay, az  :  Unit vector specifying axis of rotation
 *    x, y, z     :  Vector to be rotated
 *  Output
 *    rx, ry, rz  :  Rotated vector
 */

int  zrRotateAxyz( double ang, double  ax, double  ay, double  az,
                               double   x, double   y, double   z,
                               double *rx, double *ry, double *rz){

double cc, ss, c1 ;

      cc = cos(ang*radian) ;  c1 = 1.0 - cc ;
      ss = sin(ang*radian) ;

      *rx = (ax*ax*c1 +    cc)*x + (ax*ay*c1 - az*ss)*y + (ax*az*c1 + ay*ss)*z ;
      *ry = (ax*ay*c1 + az*ss)*x + (ay*ay*c1 +    cc)*y + (ay*az*c1 - ax*ss)*z ;
      *rz = (ax*az*c1 - ay*ss)*x + (ay*az*c1 + ax*ss)*y + (az*az*c1 +    cc)*z ;

      return 0 ;
}

/*
 *  Routines used to keep track of open files
 *  Not needed once program fully debugged - if ever!
 */

FILE *gopen(char* fname, char *ftype){

FILE *f ;
      f = fopen(fname,ftype) ;
      if(f != NULL)
        n_open_files++ ;
      return f ;
}

int gclose(FILE *f){

int  iret ;
      iret = fclose(f) ;
      if(0 == iret)
        n_open_files-- ;
      else
        printf(" Error : fclose returns error %i\n",iret);
      return iret ;
}

void  zr_clock_gettime(struct timespec clock[4]){

#ifdef MinGW
      clock_gettime(CLOCK_REALTIME,           &clock[0] );
      clock_gettime(CLOCK_MONOTONIC,          &clock[1] );
#else
      clock_gettime(CLOCK_REALTIME_COARSE,    &clock[0] );
      clock_gettime(CLOCK_MONOTONIC_COARSE,   &clock[1] );
#endif
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &clock[2] );
      clock_gettime(CLOCK_THREAD_CPUTIME_ID,  &clock[3] );

      return ;
}
