
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
