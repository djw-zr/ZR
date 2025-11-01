/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  load_splash_screen.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to load the splash screen - while zr is being set up.
 *
 *==============================================================================
 */

/*
 *==============================================================================
 * Routine to initialise teh splash screen
 *==============================================================================
 */

int load_splash_screen(void){
  int  ip = 1 ;
  int  n ;
  FILE *load_f ;
  char *load_ace ;
  char *my_name = "load_splash_screen" ;

      if(ip)printf("\n  Enter routine %s\n",my_name) ;

      n = strlen(ORroutedir) + 12 ;
      load_ace = (char *)malloc(n) ;
      strcpy(load_ace,ORroutedir) ;
      strcat(load_ace,"/load.ace") ;
      load_f   = fopen(load_ace,"r") ;
      if(NULL == load_f){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  Splash screen file missing\n") ;
        printf("  Fle name should be %s\n",load_ace) ;
        return 1 ;
      }
      fclose(load_f) ;
      printf("  Splash screen file is: %s\n",load_ace) ;
/*
 *   Load texture
 */
      splash_node = (TextureNode *)malloc(sizeof(TextureNode)) ;
      init_texture_node(splash_node) ;
      splash_node->filename = load_ace ;
      load_texture(splash_node) ;
      splash_node->needed   = 1 ;

      printf("  Texture loaded\n") ;

      return 0;
}


