/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  zr_glut.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   The following freeglut routines are provided for use in systems where
 *   neither the routines or suitable alternatives are provided.  They are
 *   based on freeglut source files available from
 *     https://github.com/FreeGLUTProject
 *   A copyright statement covering the use here of modified freeglut code
 *   follows.  The modified code itself is still licenced under GPL-3.0-or-later.
 *
 *  Freeglut Copyright
 *  ------------------
 *
 *  Freeglut code without an explicit copyright is covered by the following
 *  copyright:
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 *  Permission is hereby granted, free of charge,  to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction,  including without limitation the rights
 *  to use, copy,  modify, merge,  publish, distribute,  sublicense,  and/or sell
 *  copies or substantial portions of the Software.
 *
 *  The above  copyright notice  and this permission notice  shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE  IS PROVIDED "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS OR
 *  IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO THE WARRANTIES  OF MERCHANTABILITY,
 *  FITNESS  FOR  A PARTICULAR PURPOSE  AND NONINFRINGEMENT.  IN  NO EVENT  SHALL
 *  PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM,  DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN  AN ACTION  OF CONTRACT,  TORT OR OTHERWISE,  ARISING FROM,  OUT OF  OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  Except as contained in this notice,  the name of Pawel W. Olszta shall not be
 *  used  in advertising  or otherwise to promote the sale, use or other dealings
 *  in this Software without prior written authorization from Pawel W. Olszta.
 *
 *==============================================================================
 */

#if defined USE_ZRGLUT



void zr_FontInit(){

      fgFontFixed8x13.bitmap    = Fixed8x13_Character_Map    ;
      fgFontFixed9x15.bitmap    = Fixed9x15_Character_Map    ;
      fgFontHelvetica10.bitmap  = Helvetica10_Character_Map  ;
      fgFontHelvetica12.bitmap  = Helvetica12_Character_Map  ;
      fgFontHelvetica18.bitmap  = Helvetica18_Character_Map  ;
      fgFontTimesRoman10.bitmap = TimesRoman10_Character_Map ;
      fgFontTimesRoman24.bitmap = TimesRoman24_Character_Map ;

      return ;
}

/*
 * Draw a bitmap character
 */
void glutBitmapCharacter(void *fontID, int character){
  SFG_Font      *font ;
  const GLubyte *face ;
  char          *my_name = "glutBitmapCharacter (zr version)" ;

      if(NULL==fontID ){
        printf(" Routine %s,  Parameter fontID error\n",my_name) ;
        printf("                        fontID = %p\n",fontID) ;
        printf("              Program stopping ...\n") ;
        exit(1) ;
      }
      font = fontID ;
      if(NULL==font->bitmap ){
        zr_FontInit() ;
        if(NULL==font->bitmap ){
          printf(" Routine %s,  Bitmap not initialised\n",my_name) ;
          printf("              bitmap = %p\n",(void *)font->bitmap) ;
          exit(1) ;
        }
      }
      face = font->bitmap[ character ];

      glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT );
      glPixelStorei( GL_UNPACK_SWAP_BYTES,  GL_FALSE );
      glPixelStorei( GL_UNPACK_LSB_FIRST,   GL_FALSE );
      glPixelStorei( GL_UNPACK_ROW_LENGTH,  0        );
      glPixelStorei( GL_UNPACK_SKIP_ROWS,   0        );
      glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0        );
      glPixelStorei( GL_UNPACK_ALIGNMENT,   1        );
      glBitmap(
          face[ 0 ], font->height,      /* The bitmap's width and height  */
          font->xorig, font->yorig,     /* The origin in the font glyph   */
          ( float )( face[ 0 ] ), 0.0,  /* The raster advance -- inc. x,y */
          ( face + 1 )                  /* The packed bitmap data...      */
      );
      glPopClientAttrib( );
      return ;
}

/*
 * Draw a bitmap string
 */
void glutBitmapString(void *fontID, const unsigned char *string){

  unsigned char c ;
  float         x = 0.0f ;
  SFG_Font      *font ;
  char          *my_name = "glutBitmapString (zr version)" ;

      if(NULL==fontID ){
        printf(" Routine %s,  Parameter fontID error\n",my_name) ;
        printf("                        fontID = %p\n",fontID) ;
        printf("              Program stopping ...\n") ;
        exit(1) ;
      }
      font = fontID ;
      if(NULL==font->bitmap ){
        zr_FontInit() ;
        if(NULL==font->bitmap ){
          printf(" Routine %s,  Bitmap not initialised\n",my_name) ;
          printf("              bitmap = %p\n",(void *)font->bitmap) ;
          exit(1) ;
        }
      }
      if ( !string || ! *string )
          return;

      glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT );
      glPixelStorei( GL_UNPACK_SWAP_BYTES,  GL_FALSE );
      glPixelStorei( GL_UNPACK_LSB_FIRST,   GL_FALSE );
      glPixelStorei( GL_UNPACK_ROW_LENGTH,  0        );
      glPixelStorei( GL_UNPACK_SKIP_ROWS,   0        );
      glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0        );
      glPixelStorei( GL_UNPACK_ALIGNMENT,   1        );
/*
* Step through the string, drawing each character.
* A newline will simply translate the next character's insertion
* point back to the start of the line and down one line.
*/
      while( (c = *string++) ){
        if(c == '\n'){
          glBitmap ( 0, 0, 0, 0, -x, (float) -font->height, NULL );
          x = 0.0f;
        }
/* Not an EOL, draw the bitmap character */
        else{
          const GLubyte* face = font->bitmap[ c ];

          glBitmap(
              face[ 0 ], font->height,     /* Bitmap's width and height    */
              font->xorig, font->yorig,    /* The origin in the font glyph */
              (float)( face[ 0 ] ), 0.0,   /* The raster advance; inc. x,y */
              ( face + 1 )                 /* The packed bitmap data...    */
          );

          x += (float)( face[ 0 ] );
        }
      }

      glPopClientAttrib( );
}


/*
 * Return the width of a string drawn using a bitmap font
 */
int glutBitmapLength(void *fontID, const unsigned char *string ){

  unsigned char c ;
  int           length           = 0 ;
  int           this_line_length = 0 ;
  SFG_Font      *font;
  char          *my_name = "glutBitmapLength (zr version)" ;

      if(NULL==fontID ){
        printf(" Routine %s,  Parameter fontID error\n",my_name) ;
        printf("                        fontID = %p\n",fontID) ;
        printf("              Program stopping ...\n") ;
        exit(1) ;
      }
      font = fontID ;
      if ( !string || ! *string ) return 0;

      while( ( c = *string++) ){
/*
 *  When not an end of line, increment length of line.  The following
 *  loads the first element in each character definition.  This gives
 *  the width of the character.
 */
        if( c != '\n' )
            this_line_length += *( font->bitmap[ c ]);
/*
 *  Otherwise reset the length of the line
 */
        else{
            if( length < this_line_length )
                length = this_line_length;
            this_line_length = 0;
        }
      }
      if ( length < this_line_length )
          length = this_line_length;
      return length;
}


/*
 * Returns the height of a bitmap font
 */
int glutBitmapHeight(void *fontID){

  SFG_Font *font;
  char     *my_name = "glutBitmapHeight (zr version)" ;

      if(NULL==fontID ){
        printf(" Routine %s,  Parameter fontID error\n",my_name) ;
        printf("                        fontID = %p\n",fontID) ;
        printf("              Program stopping ...\n") ;
        exit(1) ;
      }
      font = fontID ;
      return font->height ;
}




/*
 *  The following routines were not included in OpenGL on Windows
 *    so they have been adapted from routines and comment lines
 *    in the mesa file  "ratpos.c" in "Mesa-7.11.1/src/".  As a result
 *    the following copyright notice operates for the rest of this file
 *
 * Mesa 3-D graphics library
 * Version:  6.5.3
 *
 * Copyright (C) 1999-2007  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*  OpenGL implementation of glWindowPos*MESA()
 */
void glWindowPos4ff( GLfloat x, GLfloat y, GLfloat z, GLfloat w ){

int     ip = 0 ;
static int icount = 0;
GLfloat fx, fy;

      if(ip && icount < 100){
        printf(" Using glWindowPos4ff\n") ;
        icount++ ;
      }

      /* Push current matrix mode and viewport attributes */
      glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );

      /* Setup projection parameters */
      glMatrixMode( GL_PROJECTION );
      glPushMatrix();
      glLoadIdentity();
      glMatrixMode( GL_MODELVIEW );
      glPushMatrix();
      glLoadIdentity();

      glDepthRange( z, z );
      glViewport( (int) x - 1, (int) y - 1, 2, 2 );

      /* set the raster (window) position */
      fx = x - (int) x;
      fy = y - (int) y;
      glRasterPos4f( fx, fy, 0.0, w );

      /* restore matrices, viewport and matrix mode */
      glPopMatrix();
      glMatrixMode( GL_PROJECTION );
      glPopMatrix();

      glPopAttrib();
}

void glWindowPos2f(GLfloat x, GLfloat y)
{
      glWindowPos4ff(x, y, 0.0F, 1.0F);
}

#endif
