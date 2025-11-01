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
 *
 * int  init_system(void)              ::  Initialise clocks and freetypr
 * int  nint(double d)                 ::  Nearest integer
 * void  str2lc(char *string)          ::  Convert string to lower case
 * char *zr_to_upper(char *string)     :: Return tring as upper case
 * char *zr_to_lower(char *string)     :: Return as lower case
 *
 * int  zr_filename_MS2L(char *fname)  :: replace '\' and '\\' by '/'
 * char *convert_dir_markers(char *string1) ::  Convert '\\' to '/'
 * char *strip_dot_dirs(char* file_1)  :: Convert '/./' and '/A/../' to '/'
 *
 * char *strip_quotes(char *string1)   :: Remove quotes around string
 * void  zr_str2lcnqne(char *string)   :: Convert string to lower case without
 *                                     :: quotes and file extension
 *
 * int strcmp_ic(const char *s1,
 *                     const char *s2) ::  Compare two string ignoring case
 * int strncmp_ic(const char *s1,
 *            const char *s2, int nn)  :: Ditto
 * int  strcmp_nq(char *s1a, char *s2a)   :: Compare strings with quotes removed
 * int  strcmp_nqic(char *s1a, char *s2a) :: Ditto ignoring case
 *
 * char *zr_basename(char *fname)      :: filename with directories removed
 * char *zr_basename2(char *fname)     :: ... and without extension
 * char *zr_extension(char *fname)     :: return extension
 * char *zr_extension_lc(char *fname)  :: ... as lower case
 * char *zr_corename(char *fname)      :: filename without extension
 * char *zr_parentdir(char *fname)     :: Return parent directory A/B/C -> B
 * char *zr_full_parentdir(char *fname):: Return full parent name A/B/C -> A/B
 *
 * int find_msstyle_file(char **pname) :: Find MS style file
 * int zr_find_msfile2(char **pname)   :: Ditto modifying *pname
 * int zr_find_msdir(char **pname)     :: Ditto + save directory in dir_master
 *
 * int calls_per_second()              :: Count number of calls per second
 *
 * enum MatrixType check_matrix4x3(Matrix4x3 *m) :: Matrix utility routine
 * int msts4x3_to_opengl4x4(float m[16], Matrix4x3 *m1) :: Ditto
 * int  zrRotateAxyz( double ang, double  ax, double  ay, double  az,
 *                                double   x, double   y, double   z,
 *                               double *rx, double *ry, double *rz)
 *
 * FILE *gopen(char* fname, char *ftype) :: Keep count of open files
 * FILE *gcaseopen(char* fname, char *ftype) :: Ditto
 * int gclose(FILE *f)                   :: Ditto
 *
 * void  zr_clock_gettime(struct timespec clock[4]) ::  Get time
 * double interpolate_array(int nn, double xx, double *xa, double *ya)
 *                                                  :: Sound Utility
 *
 * int close_system(void)                 ::  Shut down smoothly
 *
 *==============================================================================
 *
 */



/*
 *   Function to convert string to lower case
 */
void str2lc(char *string){

  unsigned int i ;

      if(!string) return ;
      for(i=0;i<strlen(string);i++)string[i]=tolower((unsigned char)string[i]) ;
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

  unsigned int i, l1, l2, n ;
  char         c1, c2 ;

     l1 = strlen(s1) ;
     l2 = strlen(s2) ;
     n  = nn ;
     if( (l1 < n) || (l2 < n) ) return 1 ;

     for(i=0;i<n;i++){
       c1 = s1[i]  ;
       c2 = s2[i]  ;
       c1 = tolower(c1) ;
       c2 = tolower(c2) ;
       if(c1 != c2) return 1 ;
     }
     return 0 ;
}

/*
 *  Routine to return a new string with '\\' replaced by '/',
 *  i.e. convert from MSTS directory seperator to everyone else.
 *  Note: this frees the input string
 */

char *convert_dir_markers(char *string1){

  unsigned int n, i, j ;
  int          c ;
  char         *string2 ;

      n = strlen(string1) ;
      string2 = (char *)malloc(n+1) ;
      for(i=j=0; i<n; i++){
        if(string1[i] != '\\' || i == n-1){
          string2[j++] = string1[i] ;
          continue ;
        }
        if(string1[i+1] == '\\'){
          string2[j++] = '/' ;
        }
      }
      string2[j] = '\0' ;
      free(string1) ;
      return string2 ;
}

/*
 *  Routine to return a new string with start and end quotes removed.
 *  Note: this frees the input string
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
 *  Routine to return a new string with quotes '"' removed.
 *  and "/n" replaced by newline.
 */

char *convert_print_style_string(char *string1){

  unsigned int nn, i, j ;
  char         c, *string2 ;

      if(!string1) return NULL ;

      nn = strlen(string1) ;
      string2 = (char *) malloc(nn+1) ;
      for(i=0,j=0;i<nn;i++,j++){
        c = string1[i] ;
        if(c == '"'){j--; continue ;}
        if(c == '\\'){
          i++ ;
          c = string1[i] ;
          if(c == 'n'){
            string2[j] = '\n' ;
            if(i<nn-2 && string1[i+1] == '\\'
                      && string1[i+2] == 'n'){
              i = i + 2 ;
            }
          }else{
            string2[j++] = '\\' ;
            string2[j]   = c    ;
          }
        }else
         string2[j] = c ;
      }
      string2[j] = '\0' ;
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
/*
 *  Return lower case version of extension
 */
char *zr_extension_lc(char *fname){

  char *pz ;

      pz = zr_extension(fname) ;
      str2lc(pz) ;
      return pz ;         // malloc
}

/**
 *  char *zr_corename(char *fname)
 *
 *  Routine to return copy of filename without extension
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
      *pz = '\0'    ;

      return string ;
}



/*
 *   Locate files and directories with matching Microsoft style
 *   pathnames.
 *
 *   Given a full pathname, the routine it tries to find an existing
 *   file or directory with the same pathname.  If that does not
 *   exist, it call itself to repeat the check on the parent directory.
 *   If a matching parent is found it searches for a file in the current
 *   directory whose name matches when the case of each character
 *   is ignored.
 *
 *   When successful it replaces the initial pathname with the
 *   matching pathname and returns zero.
 */

int find_msstyle_file(char **pname) {

         return zr_find_msfile2(pname) ;
}

int zr_find_msfile2(char **pname){

  unsigned int  i, k, l, n, m, ok, iret  ;
  unsigned int  ip = 0  ;          // Debug
  char *base   = NULL,             //  File name in last directory
      *parent = NULL,             //  Full pathname of parent directory
      *string = NULL,
      *pname2 = NULL   ;          //  Revised name
  DIR  *dp          ;
  struct dirent *di ;
  struct stat   sb  ;

char *my_name = "zr_find_msfile2" ;
/*
 *  Use stat() to check if file exists
 */
      if(ip){
        printf("\n  Enter routine %s.\n", my_name) ;
        printf("      %s.  Search for = %s\n", my_name, *pname) ;
      }
      iret = stat(*pname, &sb) ;
      if(!iret){
        if(ip)printf("      %s.  File/Directory  exists!\n", my_name) ;
        return 0      ;    //  File or directory exists
      }
/*
 *  If path not recognised:
 *    1. Clean up pathname
 */
//      zr_filename_MS2L(pname) ;
/*
 *    2. Check parent exists
 */
      parent = zr_full_parentdir(*pname) ;  // malloc parent
      if(ip){
        printf("    File/Directory does not exist\n") ;
        printf("    Parent = %s\n",parent) ;
      }
/*
 *    3. Handle special cases and check that parent exists
 */

      if(parent==NULL){
        parent = strdup(".") ;
      }else if(!strcmp(parent,"/")){
        *parent = '\0'       ;
      }else{
//        iret = zr_find_msfile2(&parent) ;
        iret = zr_find_msdir(&parent) ;
        if(iret){
          printf("    Parent does not exist\n") ;
          printf("    Parent = %s\n",parent) ;
          free(parent) ;
          return 1 ;  // Parent does not exist
        }
        if(ip)printf("  ZZ pname  :: address %p\n",
                        (void *)&parent)  ;
        if(ip)printf("  ZZ pname  :: address %p, value %p\n",
                        (void *)&parent,  (void *)parent)  ;
        if(ip)printf("  ZZ pname  :: address %p, value %p, string = %s\n",
                        (void *)&parent,  (void *)parent, parent)  ;
      }
      if(ip){
        printf("\n    %s Parent found = %s\n",my_name,parent) ;
        fflush(NULL) ;
      }
/*
 *    4.  Regenerate filename (pnams2) from parent and base
 */
      base = zr_basename(*pname)        ;  // malloc
      if(ip)printf("    base = %s\n",base) ;

      l = strlen(*pname) ;
      n = strlen(parent) ;
      m = strlen(base)   ;

      k = m + n + 1  ;
      if(parent[n-1]!='/')k = k+1 ;

      pname2 = malloc(k*sizeof(char)) ;

      strcpy(pname2,parent) ;
      if(parent[n-1]!='/')strcat(pname2,"/")    ;
      strcat(pname2,base)   ;
      if(ip)printf("    New pname = %s\n",pname2) ;
/*
 *    5.  Check if new filename  exists
 */
      iret = stat(pname2,&sb) ;
/*
 *  File found.  Copy pname2 to pname if memory available
 *               Otherwise malloc space for new pname.
 */
      if(!iret){
        free(parent) ;
        free(base)   ;
        free(string) ;
        if(l >= n){
          strcpy(*pname,pname2) ;
          free(pname2) ;
        }else{
          free(*pname) ;
          *pname = pname2 ;
        }
        if(ip)printf("     File/Directory  exists!\n") ;
        return 0     ;    //  File or directory exists
      }
/*
 *    5.  Check parent directory for a match when all characters
 *        are changed to lower case
 */
      if(ip){
        printf("     File/Directory  does not exist\n") ;
        printf("     Search for matching item  %s\n",base) ;
        printf("                 in directory  %s\n",parent) ;
      }
      m = strlen(base) ;
      for(i=0;i<m;i++) base[i]=tolower(base[i]) ;

      dp = opendir(parent) ;
      ok = 0 ;
      while((di=readdir(dp))!= NULL){
        free(string) ;
        string = strdup(di->d_name) ;
        for(i=0;i<strlen(string);i++) string[i] = tolower(string[i]) ;
//        printf("  base name = %s, file found = %s\n",base,string) ;
        ok = !strcmp(string,base) ;
        if(ok)break ;
      }
/*
 *    File not found.
 */
      if(!ok){
        closedir(dp) ;
        free(parent) ;
        free(base)   ;
        free(string) ;
        free(pname2) ;
        if(ip)printf("     Routine %s.  File not found during directory search\n", my_name) ;
        return 1     ;  // File does not exist
      }
/*
 *    File found.
 *    Generate full filename
 */
      free(base) ;
      base = strdup(di->d_name) ;
      closedir(dp) ;

      n = strlen(parent) ;
      m = strlen(base)   ;

      if(parent[n-1]!='/')n = n+1 ;
      n = n+m+1;
      free(pname2) ;
      pname2 = malloc(n*sizeof(char)) ;

      strcpy(pname2,parent) ;
      if(parent[n-1]!='/')strcat(pname2,"/")    ;
      strcat(pname2,base)   ;
      if(ip)printf("  Routine %s. File found.  File = %s\n",
                                                     my_name,pname2) ;
/*
 *  File found.  Copy pname2 to pname if memory available
 *               Otherwise malloc space for new pname.
 *               This will cause an arror if pname is not a malloc variable
 */
      if(ip)printf("  Routine %s, l= %i, %i, n= %i %i\n",
                        my_name,l,(int)strlen(*pname),n,(int)strlen(pname2)) ;
      if(ip)printf("  AA pname  :: address %p, value %p\n",(void *)&pname,  (void *)pname)  ;
      if(ip)printf("  AA pname2 :: address %p, value %p\n",(void *)&pname2, (void *)pname2) ;
      if(l >= n){
        strcpy(*pname,pname2) ;
        free(pname2) ;
        if(ip)printf("  BB pname  :: address %p, value %p\n",(void *)&pname,  (void *)pname)  ;
        if(ip)printf("  BB pname2 :: address %p\n",(void *)&pname2) ;
      }else{
        free(*pname) ;
        *pname = pname2 ;
      }
      if(ip)printf("  CC pname  :: address %p, value %p, string = %s\n",
                      (void *)&pname,  (void *)pname, *pname)  ;
      free(parent) ;
      free(base)   ;
      free(string) ;

      if(ip)printf("\n  Exit routine %s.\n",my_name) ;
      return 0  ;
}

/*
 *  Routine to find MS style directories
 *  MS ignores character case when specifying file and
 *  directory names.
 *  This routine searches for a directory which matches
 *  *pname after allowing for differences in case.
 */

int zr_find_msdir(char **pname){

  int     ip = 0 ;
  uint    i ;
  int     k, l, n, iret, ok ;
  BTree   *btree ;
  char    *base     = NULL ;
  char    *parent   = NULL ;
  char    *new_dir  = NULL ;
  char    *local_dir= NULL ;
  char    *lc_name  = NULL ;
  char    *dname    = *pname ;
  DIR     *dp       ;
  struct dirent *di ;
  struct stat   sb  ;

  char    *my_name = "zr_find_msdir" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
 *  Simplify and convert to lc
 */
      if(ip) printf("  AA %s :: dname %p :: lc_name %p\n",
             my_name, (void *)dname, (void *)lc_name) ;
      lc_name = zr_filename(dname) ;  //  Local copy
/*
 *  Check to see if this lc_name is already known
 */
      if(ip) printf("  AA1 %s :: dname %p :: lc_name %p\n",
             my_name, (void *)dname, (void *)lc_name) ;
      if(ip) printf("  AA1 %s :: dname %s :: lc_name %s\n",
             my_name, dname, lc_name) ;
      btree = find_btree(dir_master, lc_name) ;
      if(btree && btree->data){
        free(*pname) ;
        *pname = strdup((char *)btree->data) ;
        if(ip){
          printf("  BB   btree->index = %s, *pname = %p  ::%s::%s::\n",
          btree->index, (void *)*pname, *pname, (char *)btree->data) ;
          print_btree(dir_master, 0, "0") ;
        }
      }
      if(ip) printf("  CC\n") ;
/*
 *  Does this directory exist
 */
      iret = stat(*pname, &sb) ;
      if(!iret){
        if(ip) printf("  AA2 %s :: dname %p :: lc_name %p\n",
               my_name, (void *)dname, (void *)lc_name) ;
        if(ip)printf("      %s.  File/Directory  exists!\n", my_name) ;
        free(lc_name) ;
        if(ip)printf("      %s.  File/Directory  exists!\n", my_name) ;
        return 0      ;    //  File or directory exists
      }
/*
 *  First ensure parent directory exists
 */
      parent = zr_full_parentdir(dname) ;
      if(ip) printf("  DD =  %s\n",parent) ;

      if(parent==NULL){
        parent = strdup(".") ;
      }else if(!strcmp(parent,"/")){
        *parent = '\0'       ;
      }else{
        if(ip) printf("  GG %s  parent =  %s\n",my_name, parent) ;
        iret = zr_find_msdir(&parent) ;
        if(ip) printf("  GG1 %s  parent =  %s\n",my_name, parent) ;
        if(iret){
          if(ip){
            printf("    Parent does not exist\n") ;
            printf("    Parent = %s\n",parent) ;
          }
          free(parent) ;
          free(lc_name) ;
          if(ip) printf("  HH =  %s\n",parent) ;
          return 1 ;  // Parent does not exist
        }
      }
/*
 *  Parent found
 *  Generate new directory as "parent"/"base"
 */
      if(ip) printf("  II =  %s\n",parent) ;
      base = zr_basename(dname) ;
      l = strlen(dname)  ;
      n = strlen(parent) ;
      new_dir = make_filename_3(parent,
                               (parent[n-1]!='/')?"/":"",base) ;
      if(ip) printf("  JJ =  %s\n",parent) ;
/*
 *  Does the new directory exists
 */
      iret = stat(new_dir,&sb) ;    //  0 = found
      if(ip) printf("  KK =  %i %s\n",iret, new_dir) ;
/*
 * If not loop through parent directory looking for
 *    case independet matches
 */
      if(iret){
        for(i=0; i<strlen(base); i++)base[i] = tolower(base[i]) ;
        dp = opendir(parent) ;
        ok = 0 ;
        while((di=readdir(dp))!= NULL){
          local_dir = strdup(di->d_name) ;
          if(ip) printf("  MM  %s\n",local_dir) ;
          ok = !strcmp_ic(local_dir,base) ;
          if(ok)break ;
          free(local_dir) ;
        }
        if(ip) printf("  PP\n") ;
        if(!ok){
          closedir(dp) ;
          free(parent) ;
          free(base)   ;
          free(new_dir) ;
          free(lc_name) ;
          if(ip)printf("     Routine %s, DD File not found\n", my_name) ;
          return 1     ;  // File does not exist
        }
        new_dir = make_filename_3(parent,
                               (parent[n-1]!='/')?"/":"",local_dir) ;
        free(local_dir) ;
        closedir(dp) ;
      }
/*
 * Directory new_dir found concaternate with parent
 */
      if(ip) printf("  QQ  new_dir = %s\n",new_dir) ;
      if(ip)printf("  Routine %s, RR parent   = %s\n", my_name, parent) ;
      if(ip)printf("  Routine %s, RR new_dir  = %s\n", my_name, new_dir) ;
      free(base) ;
      free(parent) ;
/*
 *  File found.
 */
      if(l >= n){
        if(ip)printf("  SS\n") ;
        strcpy(*pname,new_dir) ;
      }else{
        if(ip)printf("  TT\n") ;
        free(*pname) ;
        *pname = new_dir ;
      }
      if(ip)printf("  UU\n") ;
      if(ip)printf("  Routine %s, ZZ new_dir = %s\n", my_name, new_dir) ;
      dir_master = insert_node(dir_master, lc_name, new_dir) ;
      if(ip)print_btree(dir_master,0,"0") ;

      return 0 ;
}

/**
 *  char *zr_filename_MS2L(char *fname)
 *
 *  Routine to replace '\' and '\''\' by '/' in filenames.
 */

int zr_filename_MS2L(char *fname){

  int  i, j, n  ;

      n = strlen(fname) ;
      for(i=0,j=0;i<n;i++,j++){
        if(fname[i] == '\\'){                   // Convert '\\' to '/'
          fname[j] = '/' ;
          if(j > 0 && fname[j-1] == '/')j-- ;   // Convert "//" to "/"
        }else{
          fname[j] = fname[i] ;
        }
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

char *zr_to_lower(char *string){

 int i, n ;
 static char *lower = NULL;

     if(lower)free(lower);
     lower = NULL ;
     n = strlen(string) ;
     lower = (char *)malloc(n+1) ;
     for(i=0;i<n;i++)lower[i]=tolower(string[i]);
     lower[n]='\0' ;

     return lower ;
}

/*
 *  Routine to strip '.' and '..' directory references in a string.
 *  Given a pointer to a string 'file_1' of the form
 *  "/A/./B/../C" it frees 'file_1' and returns "A/C"
 *  If the string contains no such directories it returns file_1.
 */

char *strip_dot_dirs(char* file_1){

  int   ip = 0 ;
  int   i, j,
        n,                 // Length of string
        m ;                // Number of sub strings
  int   top_dir ,
        end_dir ;
  char *file_2 ;
  char *file_3 ;
  char *p1, *p2 ;
  char **pa    ;           //  pointer to array of sub-strings

  char *my_name = "strip_dot_dirs" ;

      if(ip)printf("  Enter %s\n",my_name) ;
/*
 *  Return if file_1 is NULL or it does not contain "./".
 */
      if(!file_1 || !strstr(file_1,"./"))return file_1 ;
/*
 *  Duplicate file_1 for processing.
 */
      file_2 = strdup(file_1) ;
      n = strlen(file_2) ;
/*
 *  Set top_dir/end_dir flags if the string starts/ends with a '/'.
 */
      p1 = file_2 ;
      top_dir = p1[0] == '/' ;
      end_dir = p1[n-1] == '/' ;
      if(ip)printf(" n = %i,  p1[n] = :%c:, p[n-1] = :%c: \n",n,p1[n],p1[n-1]) ;
      if(top_dir) p1++ ;
/*
 * Count number of sub-strings
 */
      p2 = strtok(p1,"/") ;
      for(m=1;p2!=NULL;m++){
        p2 = strtok(NULL,"/") ;
        if(ip)printf(" m = %i,  string = %s\n",m,p2) ;
        if(!p2)break ;
      }
      if(ip)printf("  n = %i, m = %i\n",n,m) ;
/*
 *  Create array of pointers
 */
      pa = (char **)malloc((m+10)*sizeof(char *)) ;
/*
 *  Reset string and pointer
 */
      free(file_2) ;
      file_2 = strdup(file_1);
      p1 = file_2 ;
      if(top_dir)p1++ ;
/*
 *  Fill array
 */
      pa[0] = strtok(p1,"/") ;
      if(ip)printf(" m = %i,  pa[0] = %s\n",m,pa[0]) ;
      for(i=1;i<m;i++){
        pa[i] = strtok(NULL,"/") ;
        if(ip)printf(" m = %i,  pa[i] = %s\n",m,pa[i]) ;
      }
      if(ip)printf("  n = %i, m = %i\n",n,m) ;
/*
 *  Edit pointers
 */
      for(i=1;i<m;i++){
        if(!strcmp(pa[i],".")) pa[i] = NULL ;
        if(!strcmp(pa[i],"..")){
          if(ip)printf(" ZZ1  i = %i, pa[i] = %s\n",i,pa[i]) ;
          for(j=i-1;j>=0;j--){
            if(ip)printf(" ZZ2  j = %i, pa[j] = %s\n",j,pa[j]) ;
            if(pa[j] != NULL && strcmp(pa[j],"..") ){
              pa[i] = NULL ;
              pa[j] = NULL ;
              if(ip)printf("  Set %i and %i to NULL\n",i,j) ;
              break ;
            }
          }
        }
      }
/*
 *  Find size of remaining terms
 */
      n = top_dir ;
      for(i=0;i<m;i++){
        if(pa[i])n = n + 1 + strlen(pa[i]) ;
        if(ip)printf(" i = %i, n = %i, pa[i] = %s\n",i,n,pa[i]) ;
      }
      if(end_dir) n++ ;
/*
 *  Allocate memory
 */
      file_3 = (char *)malloc((n+1)*sizeof(char)) ;
/*
 *  Create new filename
 */
      strcpy(file_3,top_dir ? "/" : "") ;
      if(ip)printf(" i = %i, n = %i, file_3 = %s\n",0,n,file_3) ;
      for(i=0;i<m;i++){
        if(pa[i]){
          if(ip)printf(" AA file_3 = %s\n",file_3) ;
          if(ip)printf(" AA i = %i, file_3 = :%s:,  pa[i] = :%s:\n",i,file_3,pa[i]) ;
          strcat(file_3,pa[i]) ;
          if(ip)printf(" BB file_3 = %s\n",file_3) ;
          strcat(file_3,"/") ;
          if(ip)printf(" CC file_3 = %s\n",file_3) ;
        }
        if(ip)printf(" i = %i, n = %i, file_2 = %s\n",i,n,file_3) ;
      }
/*
 * Deal with final '/'.
 */
      if(!end_dir)file_3[n-1] = '\0' ;
      if(ip)printf("  file_3 = %s\n",file_3) ;
      free(pa)      ;
      free(file_2)  ;
      if(ip)printf("  Exit %s\n",my_name) ;
      return file_3 ;
}

/*
 *  Routine to convert a potential filename to standare lower case form.
 *  This removes quotes
 *       changes "\\" and '\' to '/'
 *       changes "/./" to '/'
 *       changes "A/B/../" to A/
 */


char *zr_filename(char *fname){

  char *string1 ;

      string1 = strdup(fname) ;  //  Make local copy
      str2lc(string1) ;          //  Make lower case
      string1 = strip_quotes(string1) ;        // string1 freed
      string1 = convert_dir_markers(string1) ; // string2 freed
      string1 = strip_dot_dirs(string1) ;

      return string1 ;
}
