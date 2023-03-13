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
 *   The following freeglut definitions are provided for use in systems where
 *   they are neneded by ZR and suitable alternatives are not provided.  They
 *   are based on freeglut source files available from
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

/*
 * Defined in fg_internal.h
 */
 struct tag_SFG_Font{
  const char    *name     ;
  const int     num_chars ;
  const int     height    ;
  const GLubyte **bitmap  ;
  const GLfloat xorig     ;
  const GLfloat yorig     ;
}  ;
typedef struct tag_SFG_Font SFG_Font;

typedef struct tagT_Font T_Font;
 struct tagT_Font{
 const GLubyte *bitmap  ;
}  ;

/*
 *  Copied from glutbitmap.h
 */

typedef struct {
  const GLsizei Width;
  const GLsizei Height;
  const GLfloat xorig;
  const GLfloat yorig;
  const GLfloat advance;
  const GLubyte *bitmap;
} BitmapCharRec, *BitmapCharPtr;

typedef struct {
  const char *name;
  const int num_chars;
  const int first;
  const BitmapCharRec * const *ch;
} BitmapFontRec, *BitmapFontPtr;

typedef void *GLUTbitmapFont;

#include "zr_font_data.h"

/*
 *   Copied from freeglut_std.h
 */

    extern void* glutStrokeRoman;
    extern void* glutStrokeMonoRoman;
    extern void* glutBitmap9By15;
    extern void* glutBitmap8By13;
    extern void* glutBitmapTimesRoman10;
    extern void* glutBitmapTimesRoman24;
    extern void* glutBitmapHelvetica10;
    extern void* glutBitmapHelvetica12;
    extern void* glutBitmapHelvetica18;

    /*
     * Those pointers will be used by following definitions:
     */
#if 0
#   define  GLUT_STROKE_ROMAN               ((void *) &glutStrokeRoman)
#   define  GLUT_STROKE_MONO_ROMAN          ((void *) &glutStrokeMonoRoman)
#   define  GLUT_BITMAP_9_BY_15             ((void *) &glutBitmap9By15)
#   define  GLUT_BITMAP_8_BY_13             ((void *) &glutBitmap8By13)
#   define  GLUT_BITMAP_TIMES_ROMAN_10      ((void *) &glutBitmapTimesRoman10)
#   define  GLUT_BITMAP_TIMES_ROMAN_24      ((void *) &glutBitmapTimesRoman24)
#   define  GLUT_BITMAP_HELVETICA_10        ((void *) &glutBitmapHelvetica10)
#   define  GLUT_BITMAP_HELVETICA_12        ((void *) &glutBitmapHelvetica12)
#   define  GLUT_BITMAP_HELVETICA_18        ((void *) &glutBitmapHelvetica18)
#else
#   define  GLUT_BITMAP_9_BY_15             ((void *) &fgFontFixed9x15)
#   define  GLUT_BITMAP_8_BY_13             ((void *) &fgFontFixed8x13)
#   define  GLUT_BITMAP_TIMES_ROMAN_10      ((void *) &fgFontTimesRoman10)
#   define  GLUT_BITMAP_TIMES_ROMAN_24      ((void *) &fgFontTimesRoman24)
#   define  GLUT_BITMAP_HELVETICA_10        ((void *) &fgFontHelvetica10)
#   define  GLUT_BITMAP_HELVETICA_12        ((void *) &fgFontHelvetica12)
#   define  GLUT_BITMAP_HELVETICA_18        ((void *) &fgFontHelvetica18)
#endif
