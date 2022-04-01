/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File texture_alt.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File contains routines for converting compressed textures to uncompressed
 *   RGBA when the OpenGL Compressed texture routines are not available.
 *
 *   For 2x2 and 1x1 Mipmaps the routine uses the 4x4 mipmap average
 *
 *==============================================================================
 */

int  dxt1_2_rgba(unsigned int ia[2], unsigned char *ja) ;

/*
 *   Routine to convert texture data stored in DXT1 format to RGBA byte format
 *
 *   In the ace files with the surface format flag equal to 0X12 each mipmap is
 *   stored as a single vector.  The vector is made up of 8-byte sections, each
 *   of which describes the colours in 4 by 4 set of pixels.
 *
 *   This routines unpacks the formats and replaces each mipmap by the
 *   equivalent data a simple RGBA array with each colour stored in a simgle
 *   byte.  As the lower order bits are redundent a record is kept of the
 *   original surface format (variable surface_format_orig) which can be used
 *   later to specify the internal format to be used by OpenGL.
 */

int convert_texture(TextureNode *tnode){

int           ip = 0,  // DEBUG
              ii, jj,  // indices of pixel block
              iz, jz,  // sub-indices
              w4,
              m, n,
              width  ,
              height ,
              n_textures ;
GLubyte       *t1 = NULL,   //  Initial texture array
              *t2 = NULL,   //  Final texture array
              *tp ;
int      r, g, b, a ;
GLubyte  r0, g0, b0, a0 ;
unsigned int  *uip ;
unsigned char  uc[64],
              *ucp ;
char          my_name[] = "convert_texture" ;

      if(ip)printf("  Enter routine %s  %p\n",my_name,(void *)tnode) ;
      r0 = g0 = b0 = a0 = 0xFF ;

//      ip = strcmp(tnode->name,"Black")==0 ;

      tnode->surface_format_orig = tnode->surface_format ;
      if(0x12 != tnode->surface_format)return 0 ;

      tnode->surface_format = 0 ;

      if(ip)printf("  Convert texture %s\n",tnode->name) ;

      width      = tnode->width  ;
      height     = tnode->height ;
      n_textures = tnode->n_textures ;

      for(m=0;m<n_textures;m++){
        t1 = tnode->texture[m] ;
        if(width>2){
          t2 = (GLubyte *)malloc(width*height*4*sizeof(GLubyte)) ;
        }else{
          t2 = (GLubyte *)malloc(16*sizeof(GLubyte)) ;
        }

        w4 = 4*width ;
        uip = (unsigned int  *)t1 ;

        if(width>2){
          for(jj=0;jj<height;jj=jj+4){
            for(ii=0;ii<width;ii=ii+4,uip=uip+2){
              ucp = uc ;
              dxt1_2_rgba(uip, ucp) ;
              tp = t2 + jj*w4 + ii*4;
              for(jz=0;jz<4;jz++){
                for(iz=0;iz<4;iz++){
                  *tp++ = *ucp++ ;
                  *tp++ = *ucp++ ;
                  *tp++ = *ucp++ ;
                  *tp++ = *ucp++ ;
                }
                tp = tp -16 + w4 ;
              }
            }
          }
/*
 *  Average the 4x4 Mipmap
 */
          if(width == 4){
            r = g = b = a = n = 0 ;
            tp = t2 ;
              for(jz=0;jz<4;jz++){
                for(iz=0;iz<4;iz++){
                  r0 = *tp++  ;
                  g0 = *tp++  ;
                  b0 = *tp++  ;
                  a0 = *tp++  ;
                  if(a0 != 0){
                    r = r0 + r ;
                    g = g0 + g ;
                    b = b0 + b ;
                    n = n + 1  ;
                  }
                }
                tp = tp -16 + w4 ;
              }
              if(n>0){
                r0 = r/n ;
                g0 = g/n ;
                b0 = b/n ;
                a0 = 0xFF ;
              }else{
                r0 = g0 = b0 = a0 = 0xFF ;
              }
          }
        }else{
/*
 *  For 2x2 and 1x1 Mipmaps use the average of the 4x4 Mipmap.
 */
          for(ii=0,tp=t2;ii<4;ii++){
            *tp++ = r0 ;
            *tp++ = g0 ;
            *tp++ = b0 ;
            *tp++ = a0 ;
          }
        }
        free(tnode->texture[m]) ;   //  Replace old texture array
        tnode->texture[m] = t2 ;

        width  = width  >> 1 ;       //  Divide by 2
        height = height >> 1 ;
      }

      if(ip)printf("  Exit  routine %s\n",my_name) ;

      return 0;
}


/*
 * Subroutine to convert packed Dxt1 texture block into rgba bytes
 *
 *  Textures in memory are stored as four bytes per pixel, in which
 *  increasing byte number corresponds to red, green, blue and alpha
 */

int  dxt1_2_rgba(unsigned int ia[2], unsigned char *ja){

  int           i, j ;
  unsigned int  c0, c1, kk, ll ;
  unsigned int  r[4], g[4], b[4], a[4] ;

        c0 = ia[0] & 0xFFFF ;          // First 2 bytes
        c1 = (ia[0]>>16) & 0xFFFF ;    // Next 2 bytes

/*
 *  In the dtx1 format, the first two 16-bit integers contain red, green
 *  and blue values stored in bits 0:4, 5:10 and 11:15.  Here the colours
 *  are extracted and placed in integers with their most significan bit
 *  in bit-7.  Bits 0:7 of the four colours then reflect the original
 *  rgb bytes but with the loss of the low order bits dropped by the dxt1
 *  scheme.
 */
        b[0] = c0 &   0x1f ; b[0] = b[0] << 3 ;
        g[0] = c0 &  0x7e0 ; g[0] = g[0] >> 3 ;
        r[0] = c0 & 0xf800 ; r[0] = r[0] >> 8 ;
        a[0] = 0xffff ;

        b[1] = c1 &   0x1f ; b[1] = b[1] << 3 ;
        g[1] = c1 &  0x7e0 ; g[1] = g[1] >> 3 ;
        r[1] = c1 & 0xf800 ; r[1] = r[1] >> 8 ;
        a[1] = 0xffff ;
/*
 *  Two sets of equations are then used to define a third and fourth colours
 *  used by the dxt1 scheme.  The + 1 in the equations is included to round
 *  up the fraction 2/3, when it occurs, to the next highest integer.
 */
        if(c0>c1){
          r[2] = (r[0]*2 + r[1] + 1)/3 ;
          g[2] = (g[0]*2 + g[1] + 1)/3 ;
          b[2] = (b[0]*2 + b[1] + 1)/3 ;
          a[2] = 0xffff ;
          r[3] = (r[0] + 2*r[1] + 1)/3 ;
          g[3] = (g[0] + 2*g[1] + 1)/3 ;
          b[3] = (b[0] + 2*b[1] + 1)/3 ;
          a[3] = 0xffff ;
        }else{
          r[2] = (r[0] + r[1])/2 ;
          g[2] = (g[0] + g[1])/2 ;
          b[2] = (b[0] + b[1])/2 ;
          a[2] = 0xffff ;
          r[3] = 0 ;               // Transparent - no colour
          g[3] = 0 ;
          b[3] = 0 ;
          a[3] = 0x0000 ;
        }
/*
 *   Finally use the array of 2-bit indices in the second pair of 16-bit
 *   words to define the rgba values of a 4 by 4 block of pixels.
 */
        for(i=0;i<4;i++){
          for(j=0;j<4;j++){
            kk = ia[1] & 0x3 ;     // Extract lowest two bits
            ll = (i*4 + j)*4 ;
            if(1){
            ja[ll  ] = r[kk] ;
            ja[ll+1] = g[kk] ;
            ja[ll+2] = b[kk] ;
            ja[ll+3] = a[kk] ;
            }else{
            ja[ll  ] = a[kk] ;
            ja[ll+1] = b[kk] ;
            ja[ll+2] = g[kk] ;
            ja[ll+3] = r[kk] ;
            }
            ia[1]    = ia[1]>>2 ;  // Shift down two bits
          }
        }
        return 0 ;
}

