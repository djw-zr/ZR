/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  make_texture_resident.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routine to make a texture resident
 *
 *==============================================================================
 */
int make_texture_resident(TextureNode *tx_node){
int   ip = 0 ;
int   width, iret ;
int   height ;
int   size   ;
int   i      ;
char  string[256] ;
char *my_name = "make_texture_resident" ;

//      ip = !strcmp(tx_node->name,"brake3") ;
//      ip = (tx_node == land_texture) ;

      if(ip)printf("  Enter make_texture_resident\n") ;
//      printf("  tx_node  = %p\n",(void *)tx_node) ;
//      printf("  name   = %s\n",tx_node->name) ;
//      printf("  needed = %i\n",tx_node->needed) ;
      if(0 == tx_node->needed) return 0 ;

//  Generate a new reference number

      if(ip)printf("    Old ref number = %i\n",tx_node->gl_tex_ref_no) ;
      if(0 != tx_node->gl_tex_ref_no){
        printf("  Routine %s.  Texture %s already is already bound to index %i\n",
                   my_name, tx_node->name, tx_node->gl_tex_ref_no) ;
        return 0 ;
      }
      glGenTextures(1,&tx_node->gl_tex_ref_no) ;
      if(ip)printf("    New ref number = %i\n",tx_node->gl_tex_ref_no) ;
      iret = check_glerror();
      if(ip)printf("    iret = %i\n",iret) ;

//  Make it an active 2D texture

      glBindTexture(GL_TEXTURE_2D, tx_node->gl_tex_ref_no);
      if(ip){
        printf("    name           = %s\n",tx_node->name);
        printf("    width, height  = %3i  %3i\n",tx_node->width, tx_node->height);
        printf("    surface_format = %3x  %3i\n",tx_node->surface_format,
                                           tx_node->surface_format);
        printf("    gl_tex_ref_no  = %i\n",tx_node->gl_tex_ref_no);
      }

//  Specify Properites

      width  = tx_node->width  ;
      height = tx_node->height ;

      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR) ;
//      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST) ;
      if(GL_EXT_texture_filter_anisotropic){
GLfloat anisot ;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.0) ;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisot) ;
        if(anisot>4.0) anisot = 4.0 ;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisot) ;
      }else{
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.4) ;
      }

//  Load mipmaps
//  Note size with format 0x12 (https://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/)
//  To uncompress see: www.gamedev.net/forums/topic616454-dxt1-decompression

      for(i=0;i<tx_node->n_textures;i++){
        if(ip)printf("    Load mipmap %i,  format = %2x, texture[i] = %p\n",
                       i, tx_node->surface_format, tx_node->texture[i]);
        if(0x12 == tx_node->surface_format){
          size = ((width+3)/4)*((height+3)/4)*8 ;
          if(ip)printf("     width = %i, height = %i, size = %i\n",
                         width, height, size ) ;
#ifndef WINDOWS
          glCompressedTexImage2D(GL_TEXTURE_2D, i,
                     GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, width, height,
                     0, size, tx_node->texture[i]);
#endif
        }else if(0x0e == tx_node->surface_format){
          glTexImage2D(GL_TEXTURE_2D, i, GL_RGB, width, height,
                  0, GL_RGB, GL_UNSIGNED_BYTE, tx_node->texture[i]);
        }else{
          if(ip)printf("    Mipmap = %i,  width = %i, height = %i\n",
                         i, width, height ) ;
          glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, width, height,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, tx_node->texture[i]);
        }
        sprintf(string,"      format = %i, i = %i",tx_node->surface_format,i);
        check_glerror2(string);
        width = width/2 ; height = height/2 ;
      }
/*
 *  Check texture is installed
 */
      i = glIsTexture(tx_node->gl_tex_ref_no);
      if(GL_FALSE == i)
        printf("  ++++++++++  Installation of texture %s failed.\n",tx_node->name);

      return 0 ;
}
