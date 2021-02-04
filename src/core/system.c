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

/**
 *   Function to compare two strings ignoring case.
 *   Returns 0 if the strings match and 1 if the comparison fails.
 */
int strcmp_ic(char *s1, char *s2){

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
 *  char *zr_extension(char *fname)
 *
 *  Routine to return copy of filename extension
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
 *  Routine to return copy of filename without extension
 */

char *zr_corename(char *fname){

int  n   ;
char *pz ;
      if(fname == NULL) return strdup("")   ;
      pz = strrchr(fname,'.')               ;
      if(pz == NULL) return strdup(fname)   ;
      n = strlen(fname) - strlen(pz)        ;
//      printf(" zr_corename  strlen(fname) = %i\n",(int)strlen(fname)) ;
//      printf(" zr_corename  strlen(pz)    = %i\n",(int)strlen(pz)) ;
      return strndup(fname, n) ;
}

/**
 *  char *zr_filename_MS2L(char *fname)
 *
 *  Routine to replace '\' by '/' in filenames
 */

int zr_filename_MS2L(char *fname){

int  i, n   ;

      n = strlen(fname) ;
      for(i=0;i<n;i++){
        if(fname[i] == '\\')fname[i] = '/' ;
      }
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
 *            1 if it represents translation with no rotation,
 *            2 if it represents rotation.
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
