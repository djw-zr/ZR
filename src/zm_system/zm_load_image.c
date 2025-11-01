
/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  zm_load_image.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   zm program used to load images into a Gdk_Pixbuf
 *
 *==============================================================================
 */


GdkPixbuf *zm_load_image_from_ace_file(char *ace_file){

  int  ip = 0 ;
  int  n, iret ;
  TextureNode tnode,
            *texture = &tnode ;
  GdkPixbuf  *pixbuf ;
  GBytes     *gbytes ;

  char       *my_name = "zm_load_image_from_ace_file" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      iret = zr_find_msfile2(&ORrouteace) ;
      if(iret){
        printf("  Ace file not found.  \n   %s\n",ORrouteace) ;
        return NULL ;
      }
      if(ip)printf("  Ace file found.\n")    ;
/*
 *  Load ace file into texture
 *  See file *.trk.
 *  This has RouteID, Name, and graphics Graphic (small) and LoadingScreen
 *  There may also be graphic details.ace.
 */

      init_texture_node(texture) ;
      texture->filename = ORrouteace ;
      texture->name     = ORroute ;
      iret = load_texture(texture) ;
      if(iret){
        printf("  Texture file loading failed.\n")    ;
        return NULL ;
      }
      n = texture->width*texture->height*4 ;

      if(ip){
        printf("  Texture file loaded.\n") ;
        printf("  texture = %p\n",  texture) ;
        printf(" *texture = %p\n", *texture) ;
        printf("  texture[0] = %p\n",texture[0]) ;
        printf("  texture[1] = %p\n",texture[1]) ;
                          4 ;
        printf("  width  = %i\n", texture->width) ;
        printf("  height = %i\n", texture->height) ;
        printf("  is_alpha = %i\n", texture->is_alpha) ;
        printf("  is_mask  = %i\n", texture->is_mask) ;
        printf("  alpha_is_mask  = %i\n", texture->alpha_is_mask) ;
        printf("  n_textures     = %i\n", texture->n_textures) ;
        printf("  n      = %i\n", n) ;
      }
      gbytes = g_bytes_new_static(texture->texture[0], n) ;
        pixbuf = gdk_pixbuf_new_from_bytes(
                    gbytes,
                    0,                    //  RGB
                    1,                    //  Alpha channel present
                    8,                    //  8 bits per channel
                    texture->width,
                    texture->height,
                    texture->width*4 ) ;   //  Bytes between start of lines
      if(ip){
        printf("  gbytes = %p\n",(void *) gbytes) ;
        printf("  pixbuf = %p\n",(void *) pixbuf) ;
        printf("  width  = %i\n", texture->width) ;
        printf("  height = %i\n", texture->height) ;
        printf("  delta  = %i\n", texture->width*4) ;
      }

      return pixbuf ;
}
