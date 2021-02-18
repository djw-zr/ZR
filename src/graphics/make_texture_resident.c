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
int make_texture_resident(TextureNode *tnode){
int   ip = 0 ;
int   width  ;
int   height ;
int   size   ;
int   i      ;
char  string[256] ;

//      ip = !strcmp(tnode->name,"road") ;

      if(ip)printf("   Enter make_texture_resident\n") ;
//      printf("  tnode  = %p\n",(void *)tnode) ;
//      printf("  name   = %s\n",tnode->name) ;
//      printf("  needed = %i\n",tnode->needed) ;
      if(0 == tnode->needed) return 0 ;

//  Generate a new reference number

      if(ip)printf("  Old ref number = %i\n",tnode->gl_tex_ref_no) ;
      glGenTextures(1,&tnode->gl_tex_ref_no) ;
      if(ip)printf("  New ref number = %i\n",tnode->gl_tex_ref_no) ;
      check_glerror();

//  Make it an active 2D texture

      glBindTexture(GL_TEXTURE_2D, tnode->gl_tex_ref_no);
      if(ip){
        printf(" name           = %s\n",tnode->name);
        printf(" width, height  = %3i  %3i\n",tnode->width, tnode->height);
        printf(" surface_format = %3x  %3i\n",tnode->surface_format,
                                           tnode->surface_format);
        printf(" gl_tex_ref_no  = %i\n",tnode->gl_tex_ref_no);
      }

//  Specify Properites

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                     GL_NEAREST_MIPMAP_NEAREST) ;
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;
      width  = tnode->width  ;
      height = tnode->height ;

//  Load mipmaps
//  Note size with format 0x12 (https://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/)

      for(i=0;i<tnode->n_textures;i++){
        if(ip)printf(" Load mipmap %i,  format = %2x, texture[i] = %p\n",
                       i, tnode->surface_format, tnode->texture[i]);
        if(0x12 == tnode->surface_format){
          size = ((width+3)/4)*((height+3)/4)*8 ;
          if(ip)printf("   width = %i, height = %i, size = %i\n",
                         width, height, size ) ;
          glCompressedTexImage2D(GL_TEXTURE_2D, i,
                    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, width, height,
                    0, size, tnode->texture[i]);
        }else if(0x0e == tnode->surface_format){
          glTexImage2D(GL_TEXTURE_2D, i, GL_RGB, width, height,
                  0, GL_RGB, GL_UNSIGNED_BYTE, tnode->texture[i]);
        }else{
          glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, width, height,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, tnode->texture[i]);
        }
        sprintf(string," format = %i, i = %i",tnode->surface_format,i);
        check_glerror2(string);
        width = width/2 ; height = height/2 ;
      }
/*
 *  Check texture is installed
 */
      i = glIsTexture(tnode->gl_tex_ref_no);
      if(GL_FALSE == i)printf("  Installation of texture failed\n");

      return 0 ;
}
