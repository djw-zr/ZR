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
 *   Note size with format 0x12 (https://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/)
 *   To uncompress see: www.gamedev.net/forums/topic616454-dxt1-decompression
 *
 *   gluBuild2DMipmaps :: If width or height are not an exact power of two the
 *   values are scaled to the nearest power of two and these are used for
 *   starting the Mipmaps.  Thus if width is 640 it will be scaled to 512.
 *
 *==============================================================================
 */
int make_texture_resident(TextureNode *tx_node){
int   ip = 0 ;
int   width, iret ;
int   height ;
int   size   ;
int   n_tex  ;
int   i      ;
int   log_01 ;
char  string[256] ;
GLfloat anisot    ;
char *my_name = "make_texture_resident" ;

      if(ip)printf("  Enter make_texture_resident\n") ;
      if(0 == tx_node->needed) return 0 ;

      width  = tx_node->width  ;
      height = tx_node->height ;
      n_tex  = tx_node->n_textures ;
/*
 * Check if a full set of correctly sized mipmaps are available
 */
      log_01 = (width >= height)
                  ? 0 == (height & (height-1)) && (width  == 2<<(n_tex-1))
                  : 0 == (width  & (width -1)) && (height == 2<<(n_tex-1)) ;

//      ip = !log_01 ;
//      ip = !strcmp(tx_node->name,"FootBrBrank") ;
//      ip = (tx_node == land_texture) ;
      if(ip)printf("============= width = %i, n_textures = %i, log_01 = %i =======\n",
                   width,tx_node->n_textures,log_01) ;
//      printf("  tx_node  = %p\n",(void *)tx_node) ;
//      printf("  name   = %s\n",tx_node->name) ;
//      printf("  needed = %i\n",tx_node->needed) ;

/*
 *  Generate a new reference number
 */
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
/*
 *  Make it an active 2D texture
 */
      glBindTexture(GL_TEXTURE_2D, tx_node->gl_tex_ref_no);
      if(ip){
        printf("    name           = %s\n",tx_node->name);
        printf("    width, height  = %3i  %3i\n",width, height);
        printf("    surface_format = %3x  %3i\n",tx_node->surface_format,
                                                 tx_node->surface_format);
        printf("    n_textures     = %i\n",tx_node->n_textures);
        printf("    gl_tex_ref_no  = %i\n",tx_node->gl_tex_ref_no);
      }
/*
 *   Specify Texture properites
 */
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR) ;
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST) ;
/*
 *  If anisotropic filter supported initialise filter
 */
      if(GL_EXT_texture_filter_anisotropic){
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.0) ;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisot) ;
        if(anisot>4.0) anisot = 4.0 ;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisot) ;
      }else{
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.4) ;
      }
/*
 *  Load set of Mipmaps
 */
      if(log_01){
        for(i=0;i<n_tex;i++){
          if(ip)printf("    Load mipmap %i,  format = %2x, texture[i] = %p\n",
                        i, tx_node->surface_format, tx_node->texture[i]);
          if(0x12 == tx_node->surface_format){
#ifdef WINDOWS
            if(ip)printf("     width = %i, height = %i\n", width, height) ;
            glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, width, height,
                    0, GL_RGBA, GL_UNSIGNED_BYTE, tx_node->texture[i]);
#else
// This tries to store it inernally (within OpenGL) in the special compressed form.
            size = ((width+3)/4)*((height+3)/4)*8 ;
            if(ip)printf("     width = %i, height = %i, size = %i\n",
                          width, height, size ) ;
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
          sprintf(string,"      format = %i, mipmap = %i",tx_node->surface_format,i);
          check_glerror2(string);
          width  = (1 == width) ? 1 : width>>1  ;
          height = (1 == height)? 1 : height>>1 ;
        }
/*
 * or generate a set of Mipmaps.  The routine scales the width and height of the
 * base image to the nearest power of 2 before creating the Mipmaps.
 */
      }else{
        if(ip)printf("    Construct Mipmaps.  Name = %s, format = %2x, texture[0] = %p\n",
                        tx_node->name, tx_node->surface_format, tx_node->texture[0]);
        if(0x12 == tx_node->surface_format){
          size = ((width+3)/4)*((height+3)/4)*8 ;
          if(ip)printf("     width = %i, height = %i, size = %i\n",
                        width, height, size ) ;
          gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height,
                    GL_RGBA, GL_UNSIGNED_BYTE, tx_node->texture[0]);
        }else if(0x0e == tx_node->surface_format){
          gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height,
                  GL_RGB, GL_UNSIGNED_BYTE, tx_node->texture[0]);
        }else{
          if(ip)printf("    Mipmap = 0,  width = %i, height = %i\n",
                        width, height ) ;
          gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height,
                    GL_RGBA, GL_UNSIGNED_BYTE, tx_node->texture[0]);
        }
        sprintf(string,"      format = %i",tx_node->surface_format);
        check_glerror2(string);
      }
/*
 *  Check texture is installed
 */
      i = glIsTexture(tx_node->gl_tex_ref_no);
      if(GL_FALSE == i)
        printf("  ++++++++++  Installation of texture %s failed.\n",tx_node->name);

      return 0 ;
}
