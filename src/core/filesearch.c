/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  filesearch.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines which search for files
 *
 *==============================================================================
 */
/*
 *  Routine to find wave files starting with the SMS file
 *  1.  Sear same directory as SMS file
 *  2.  Search same directory as wagon
 *  3.  Search all Trainset directoires
 *  4.  Search Route Sound direntory
 *  5.  Search Top Sound directory
 *  6.  Search MSTS sound directory
 */
#ifdef OPENAL

#include <errno.h>
#include <sys/stat.h>
#include <ftw.h>

int check_nftw_file(const char *fpath,
                  __attribute__((unused))const struct stat *sb,
                  int typeflag, struct FTW *ftwbuf) ;


char *find_wagon_wave_file(RawWagonNode *rw, char *wav_file){

  int  ip = 0 ;
  int  n, iret ;
  char *string1, *string2, *string3 ;
  char *base_name ;
  char *dir ;
  char *my_name = "find_wagon_wave_file" ;

  DIR           *dp ;
  struct dirent *di ;
  struct stat   sb  ;


      if(ip)printf("  Enter routine %s\n",my_name) ;
      if(ip)printf("    Wagon       = %s\n",rw->name) ;
      if(ip)printf("    Wagon File  = %s\n",rw->file) ;

      if(ip)printf("    SMS   File  = %s\n",rw->sms_node->sms_fullfilename) ;
      if(ip)printf("    Sound File  = %s\n",wav_file) ;
/*
 * 1.  Look in directory containing SMS file
 */
      if(ip)printf("\n    %s :: 1. Check directory containing SMS file\n",my_name) ;
      string1 = zr_full_parentdir(rw->sms_node->sms_fullfilename) ;
      string1 = strip_dot_dirs(string1) ;
/*
 *  Check parent exists
 */
      if(ip)printf("    %s : Looking for parent %s\n",my_name,string1) ;
      iret = find_msstyle_file(&string1) ;
/*
 *  Parent found - check for wave_file
 */
      if(iret == 0){
        string2 = make_filename_3(string1,"/",wav_file) ;
        if(ip){
          printf("    %s : Parent found.\n",my_name) ;
          printf("    %s : Looking for wave file %s\n",my_name,string2) ;
        }
        iret = find_msstyle_file(&string2) ;
        if(iret == 0){
          if(ip)printf("    %s : Found wave file       %s\n",my_name,string2) ;
          free(string1) ;
          return string2 ;       // Success
        }
        free(string2)  ;
        string2 = NULL ;
      }
/*
 *  2.  Not in SMS directory so search directory with wagon shape
 */
      if(ip)printf("\n    %s :: 2. Check directory containing wagon shape file\n",my_name) ;
      string1 = zr_full_parentdir(rw->s_file) ;
      string1 = strip_dot_dirs(string1) ;
/*
 *  Check for parent
 */
      if(ip)printf("  %s : Looking for parent %s\n",my_name,string1) ;
      iret = find_msstyle_file(&string1) ;
/*
 *  Parent found - check for wave_file
 */
      if(iret == 0){
        string2 = make_filename_3(string1,"/",wav_file) ;
        if(ip)printf("  %s : Looking for wave file %s\n",my_name,string2) ;
        iret = find_msstyle_file(&string2) ;
        if(iret == 0){
          if(ip)printf("  %s : Found wave file       %s\n",my_name,string2) ;
          free(string2) ;
          return string2 ;       // Success
        }
        free(string2)  ;
        string2 = NULL ;
      }
      free(string2) ;
      string2 = NULL ;
/*
 * 3.  Not in Wagon directory - search all Trainset directories
 */
      base_name = zr_basename(wav_file) ;
      if(ip)printf("\n    %s :: 3. Check all Trainset directories\n",my_name) ;
      string1 = make_filename_3(ORdir,"/","Trains/Trainset/") ;
/*
 *  Check for directory
 */
      if(ip)printf("  %s : Looking for parent %s\n",my_name,string1) ;
      iret = find_msstyle_file(&string1) ;
/*
 *  Directory found - walk structure using nftw
 *  Variable 'nftw_file' (zr.h) is used to specify the file to find (using
 *  a case independent comparison).  If the file is found, 'nftw_file'
 *  conains the absolute pathname of the file.
 */
      if(iret == 0){
        if(ip)printf("  %s : Using nftw to search for wave file %s\n",my_name,base_name) ;
        nftw_file = base_name ;
        iret =nftw(string1, check_nftw_file, 5, 0) ;  //  Look for nftw_file
        if(iret == 1){   //  Success
          if(ip)printf("  %s : Found wave file       %s\n",my_name,nftw_file) ;
          string2 = make_filename_3(string1, "/", nftw_file) ;
          free(nftw_file) ;
          free(string1) ;
          return string2 ;
        }else{
          free(string1) ;
          if(ip)printf("  %s : Routine nftw unable fo find file = %s\n",my_name,base_name) ;
          if(ip)printf("  %s : Routine nftw error code = %i\n",my_name,iret) ;
        }
      }
/*
 *  Failure
 */
      if(ip){
        printf("  Routine %s error\n",my_name) ;
        printf("    Unable to find wagon sound file\n") ;
      }

      return NULL ;
}

/*
 *  Routine called by nftw.
 *  If file nftw_file is found it is replaced by a copy of the full pathname.
 */

int check_nftw_file(const char *fpath,
                  __attribute__((unused))const struct stat *sb,
                  int typeflag, struct FTW *ftwbuf){

  int  ip = 0 ;
  const char *name  ;
  char *my_name = "check_nftw_file" ;
  int  base     = ftwbuf->base ;
  int  level    = ftwbuf->level ;
/*
 *  Return if not a regular file
 */
      if(typeflag != FTW_F) {
        return 0 ;                              //  Continue processing
      }
      name = &fpath[base] ;
      if(strcmp_ic(name,nftw_file)) return 0 ;  //  Continue processing
      nftw_file = strdup(fpath) ;
      if(ip)printf("  Routine %s.  Level = %i. Base = %i. Found file %s\n",
                                   my_name, level, base, fpath) ;
      return 1 ;                                //  File found
}


#endif

char *make_filename_3(char *name1, char *name2, char *name3){

  int  ip = 0 ;
  int  n ;
  char *name ;
  char *my_name = "make_filename_3" ;

      if(ip){
        printf("  Enter routine %s\n",my_name) ;
        printf("    Name1 = %s\n",name1) ;
        printf("    Name2 = %s\n",name2) ;
        printf("    Name3 = %s\n",name3) ;
      }
      n = strlen(name1) + strlen(name2) + strlen(name3) + 1 ;
      name = malloc(n*sizeof(char)) ;
      strcpy(name,name1) ;
      strcat(name,name2) ;
      strcat(name,name3) ;
      if(ip){
        printf("  Exit routine %s\n",my_name) ;
        printf("    Name = %s\n",name) ;
      }
      return name ;
}
char *make_filename_5(char *name1, char *name2, char *name3, char *name4, char *name5){

  int  n ;
  char *name ;

      n = strlen(name1) + strlen(name2) + strlen(name3) + strlen(name4) + strlen(name5) + 1 ;
      name = malloc(n*sizeof(char)) ;
      strcpy(name,name1) ;
      strcat(name,name2) ;
      strcat(name,name3) ;
      strcat(name,name4) ;
      strcat(name,name5) ;
      return name ;
}


