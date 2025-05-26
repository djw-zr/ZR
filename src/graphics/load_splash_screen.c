/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  graphics_init.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to initialise graphics and to control the amount of data
 *     held by the graphics card.
 *
 *   If the graphics card is loaded with too many textures and/or display lists
 *     it can result in objects being generated without their required textures.
 *   OpenRails appear to overcome this problem by minimising the amount of data
 *     stored on the graphics card.  It also appears to minimise the data kept
 *     in main memory - probably because some computers are memory limited.
 *   ZR presently assumes that main memory is not a problem, so all shapes and
 *     textures found are loaded into memory ready for use.
 *   Here the loading and culling of data is controlled by routine graphics_reset.
 *     Routine mark_shapes is used to identify which shapes are needed for
 *     the current viewpoint.  Routine mark_textures then identifies which
 *     textures are needed.  Textures and display lists not longer required
 *     are the culled after which any new textures and display lists are
 *     generated and loaded.
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


