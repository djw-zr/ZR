
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

void glutBitmapCharacter( void* fontID, int character ){
  SFG_Font  *font ;
  const GLubyte* face;
  char  *my_name = "glutBitmapCharacter (zr version)" ;

      if(NULL==fontID ){
        printf(" Routine %s,  Parameter fontID = %p\n",my_name,fontID) ;
        exit(1) ;
      }
      font = fontID ;
      if(NULL==font->bitmap ){
//        printf(" Routine %s,  Bitmap NULL\n",my_name) ;
//        printf("              bitmap = %p\n",(void *)font->bitmap) ;
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
