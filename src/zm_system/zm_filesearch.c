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
