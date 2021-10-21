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

int   nint(double d){

int    i ;
double dd ;

     if(d >= 0.0){
       i = d + 0.5 ;
       return i ;
     }
     i = 0.5 - d ;
     return -i ;
}



/**
 *   Function to compare two strings ignoring case.
 *   Returns 0 if the strings match and 1 if the comparison fails.
 */
int strcmp_ic(const char *s1, const char *s2){

int  i, l1 ;
char c1, c2 ;

     l1 = strlen(s1) ;
     if(strlen(s2) != l1) return 1 ;
     for(i=0;i<l1;i++){
       c1 = s1[i]  ;
       c2 = s2[i]  ;
       c1 = tolower(c1) ;
       c2 = tolower(c2) ;
       if(c1 != c2) return 1 ;
     }
     return 0 ;
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
char *pz ;
      if(fname == NULL) return strdup("")   ;   // malloc
      pz = strrchr(fname,'.')               ;
      if(pz == NULL) return strdup(fname)   ;   // malloc
      n = strlen(fname) - strlen(pz)        ;
//      printf(" zr_corename  strlen(fname) = %i\n",(int)strlen(fname)) ;
//      printf(" zr_corename  strlen(pz)    = %i\n",(int)strlen(pz)) ;
      return strndup(fname, n) ;
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

char *zr_find_msfile(char *fname){

int  i, j, n, m, ok  ;
int  ip = 0          ;  // Debug
char *base,             //  File name in last directory
     *parent,           //  Full pathname of parent directory
     *string         ;
char *my_name = "zr_find_msfile" ;
DIR  *dp     ;
FILE *fp     ;
struct dirent *di ;
/*
 *  Check if file exists
 */
      if(ip)printf(" Enter routine %s, File = %s\n",my_name,fname) ;
      fp = fopen(fname,"r") ;
      if(fp!=NULL){  // Fle exists
        fclose(fp) ;
        return strdup(fname)   ;
      }
/*
 *  If not search for parent
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


/**
 *  char *zr_filename_MS2L(char *fname)
 *
 *  Routine to replace '\' by '/' in filenames and to correct case of extension.
 *  This is because files referred to as name.s may be name.S etc.
 */

int zr_filename_MS2L(char *fname){

int  i, j, n  ;
int  ip = 0          ;  // Debug
char *my_name = "zr_filename_MS2L" ;

      n = strlen(fname) ;
      for(i=0,j=0;i<n;i++,j++){
        if(fname[i] == '\\')fname[j] = '/' ;
        if(fname[i] == '/' && fname[i+1] == '/')i++ ;   // Convert "//" to "/"
      }
      fname[j] = '\0' ;
      return 0 ;
}



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
 *            1 if it represents translation oonly,
 *            2 if it represents rotation and/or stretching,
 *            3 if it represents both.
 */
int check_matrix4x3(Matrix4x3 *m){

int k = 0 ;

      if(m->AX != 1.0 || m->AY != 0.0 || m->AZ != 0.0 ||
         m->BX != 0.0 || m->BY != 1.0 || m->BZ != 0.0 ||
         m->CX != 0.0 || m->CY != 0.0 || m->CZ != 1.0) k = 2  ;
      if(m->DX != 0.0 || m->DY != 0.0 || m->DZ != 0.0) k++    ;
      return k ;
}

/**
 *
 * Function to convert MSTS 4x3 transformation matrix
 *          to an OpenGl vector representation of a 4x4 matrix
 */

int make_matrix4x4(float m[16], Matrix4x3 *m1){

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
