 /*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  freetype.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines using freetype routines to display text.  These need the
 *   system header file ft2build.h and the freetype library.  The main
 *   variables are defined in the ZR include file 'zr.h'.
 *
 *   The program tries to load the two Microsoft typefaces, Verdana, with
 *   fixed spacing, and Tahoma which is similar but has variable spacing.
 *   They both have a good reputation for use with vdu displays.  When
 *   using the routine 'render_text_as_greyscale()', I found that sizes
 *   8, 10 and above were good with both typefaces but 6 can only just be
 *   read.
 *
 *   The routine 'render_text()', generates simple bitmaps - with Verdana
 *   and Tahoma the results are not good.
 *
 *==============================================================================
 */
#ifdef zr_freetype

int  freetype_init(){

int   error ;
char  my_name[] = "freetype_init" ;
char  font_file[256] ;

#ifdef NO_SDL_TTF
      return 0;
#endif

      error = FT_Init_FreeType(&ft_lib) ;
      if(error){
        fprintf(stderr, "Routine %s could not inititialise freetype library\n",
                         my_name);
        exit(0) ;
      }
/*
 *   Initialise Tahoma
 */
      strcpy(font_file,ZRfonts);
      strcat(font_file,"/tahoma.ttf") ;

      error = FT_New_Face(ft_lib, font_file, 0, &ft_tahoma) ;
      if(error){
        fprintf(stderr, "Routine %s failed to load Tahoma font %s\n",
                        my_name,font_file);
        exit(0) ;
      }
//  Set default size = 18
      error = FT_Set_Char_Size(
            ft_tahoma,    /* handle to face object           */
            0,       /* char_width in 1/64th of points  */
            18*64,   /* char_height in 1/64th of points */
            96,      /* horizontal device resolution,   */
            96 ) ;    /* vertical device resolution      */
      if(error){
        fprintf(stderr, "Routine %s failed to set font size\n",
                        my_name);
        exit(0) ;
      }
/*
 *   Initialise Verdana
 */
      strcpy(font_file,ZRfonts);
      strcat(font_file,"/verdana.ttf") ;

      error = FT_New_Face(ft_lib, font_file, 0, &ft_verdana) ;
      if(error){
        fprintf(stderr, "Routine %s failed to load Verdana font %s\n",
                        my_name,font_file);
        exit(0) ;
      }
//  Set default size = 18
      error = FT_Set_Char_Size(
            ft_verdana,    /* handle to face object           */
            0,       /* char_width in 1/64th of points  */
            18*64,   /* char_height in 1/64th of points */
            96,      /* horizontal device resolution,   */
            96 ) ;    /* vertical device resolution      */
      if(error){
        fprintf(stderr, "Routine %s failed to set font size\n",
                        my_name);
        exit(0) ;
      }
      return 0 ;
}


//void render_text(const char *text, GLfloat x, GLfloat y, float sx, float sy) {
void render_text(const char *text) {

static int ip  = 0 ;
unsigned int iu, iu1 ;
int          j, k, l, i1, j1, m, error ;
const char *p;

FT_GlyphSlot g  ;
FT_UInt      glyph_index ;
GLuint tex;
GLuint vbo;
char  my_name[]="render_text" ;
char  *b ;
char  c ;
char bitmap[256] ;

      glActiveTexture(GL_TEXTURE0);
      glGenTextures(1, &tex);
      glBindTexture(GL_TEXTURE_2D, tex);
//      glUniform1i(uniform_tex, 0);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

//      glGenBuffers(1, &vbo);
//      glEnableVertexAttribArray(attribute_coord);
//      glBindBuffer(GL_ARRAY_BUFFER, vbo);
//      glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

      for(p = text; *p; p++) {
        glyph_index = FT_Get_Char_Index(ft_tahoma, *p );
        error = FT_Load_Glyph(ft_tahoma, glyph_index, FT_LOAD_DEFAULT );
        if(error)printf(" Load Glyph ERROR %i\n",error) ;
        if(ip)printf(" glyph_index = %i\n",glyph_index );
//        error = FT_Render_Glyph(ft_tahoma->glyph, FT_RENDER_MODE_NORMAL );
        error = FT_Render_Glyph(ft_tahoma->glyph, FT_RENDER_MODE_MONO );
        if(error)printf(" Render Glyph ERROR %i\n",error) ;
//        FT_Load_Char(face,0,FT_LOAD_RENDER) ;
        if(ip){
          printf(" II3 ::%c:: \n",(int) *p) ;
          fflush(NULL) ;
        }

        g = ft_tahoma->glyph ;
        if(ip)printf(" glyph_slot = %p\n",(void *)g );

        if(ip)printf("  %i %i :: %i %i :: %i %i :: %i %i %i : %i\n",
          g->bitmap.width,
          g->bitmap.rows,
          g->bitmap_left,
          g->bitmap_top,
          (int)g->advance.x,
          (int)g->advance.y,
          g->bitmap.pitch,
          g->bitmap.pixel_mode,
          g->bitmap.num_grays,
          g->bitmap.rows - g->bitmap_top ) ;

//  Copy from freetype bitmap buffer to OpenGL bitmap
        b = (char *)g->bitmap.buffer ;
        for(iu=0;iu<g->bitmap.rows;iu++){
          iu1 = g->bitmap.rows - 1 - iu ;
          for(j=0;j<g->bitmap.pitch;j++){
            bitmap[iu1*g->bitmap.pitch + j]
                               = b[iu*g->bitmap.pitch + j] ;
          }
        }

        if(ip){
          b = (char *)g->bitmap.buffer ;
          for(iu=0;iu<g->bitmap.rows;iu++){     // rows - unsigned int
            iu1 = g->bitmap.rows - 1 - iu ;
            for(j=0;j<g->bitmap.pitch;j++){  // pitch - int
              for(k=7;k>-1;k--){
                m = (*b & (1<<k))? 1 : 0 ;
                printf(" %i",m) ;
              }
              b++ ;
            }
            printf("\n") ;
          }


          printf("==============================\n") ;
          b = (char *)bitmap ;
          for(iu=0;iu<g->bitmap.rows;iu++){
            for(j=0;j<g->bitmap.pitch;j++){
              for(k=7;k>-1;k--){
                m = (*b & (1<<k))? 1 : 0 ;
                printf(" %i",m) ;
              }
              b++ ;
            }
//            printf(" :: %x  %x\n",(GLubyte)bitmap[j][0],(GLubyte)bitmap[j][1]) ;
              printf("\n") ;
          }
        }
//  Each row of bitmap is packed into bitmap_pitch bytes
        glPixelStorei(GL_UNPACK_ROW_LENGTH,g->bitmap.pitch*8) ;
//  Plot bitmap - convert (uint) to (int) before any arithmetic
        glBitmap(
          (GLsizei)g->bitmap.width,
          (GLsizei)g->bitmap.rows,
          (GLfloat)(-(int)g->bitmap_left),
          (GLfloat)((int)g->bitmap.rows - (int)g->bitmap_top),
          (GLfloat)(g->advance.x/64.0),
          (GLfloat)(g->advance.y/64.0),
          (const GLubyte *)bitmap) ;
      }

#if 0
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      g->bitmap.width,
      g->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      g->bitmap.buffer
    );
    float x2 = x + g->bitmap_left * sx;
    float y2 = -y - g->bitmap_top * sy;
    float w = g->bitmap.width * sx;
    float h = g->bitmap.rows * sy;

    GLfloat box[4][4] = {
        {x2,     -y2    , 0, 0},
        {x2 + w, -y2    , 1, 0},
        {x2,     -y2 - h, 0, 1},
        {x2 + w, -y2 - h, 1, 1},
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    x += (g->advance.x/64) * sx;
    y += (g->advance.y/64) * sy;
#endif

      ip = 0 ;
      return ;
}
/*
 *   Routine to render freetype text as a greyscale image.
 *   The text is always black
 */
void render_text_as_greyscale(const char *text, FT_Face type, int isize) {

static int   ip  = 0 ;
unsigned int  iu, iu1 ;
int           j, k, m, error ;
const char   *p  ;

FT_GlyphSlot g   ;
FT_UInt      glyph_index ;
GLuint       tex ;
GLuint       vbo ;
char  my_name[]="render_text_as_greyscale" ;
char  *b         ;
char  c          ;
//  __attribute__ :: an attempt to see of it removed need for
//                   repeated calls to
//   glGetFloatv(GL_CURRENT_RASTER_POSITION,posn)
char bitmap[4096] __attribute__ ((aligned (8)))  ;
GLfloat xx, yy, xp, yp;
GLfloat posn[4]  ;
GLboolean boo    ;

/*
 * Specify size
 */
      error = FT_Set_Char_Size(
            type,         /* handle to face object           */
            0,            /* char_width in 1/64th of points  */
            isize*64,     /* char_height in 1/64th of points */
            96,           /* horizontal device resolution,   */
            96 ) ;        /* vertical device resolution      */
      if(error){
        fprintf(stderr, "Routine %s failed to set font size\n",
                        my_name);
        exit(0) ;
      }
/*
 *  Get raster position
 */

      glGetFloatv(GL_CURRENT_RASTER_POSITION,posn) ;
      glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID,&boo) ;
      xx = (GLfloat)posn[0] ;
      yy = (GLfloat)posn[1] ;
      if(ip)printf(" Routine %s.  Current raster = %f %f %f %f\n",
             my_name,posn[0],posn[1],posn[2],posn[3]);
      if(ip)printf(" AA          Position is valid = %i\n",boo) ;


      glActiveTexture(GL_TEXTURE0);
      glGenTextures(1, &tex);
      glBindTexture(GL_TEXTURE_2D, tex);
//      glUniform1i(uniform_tex, 0);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

//      glGenBuffers(1, &vbo);
//      glEnableVertexAttribArray(attribute_coord);
//      glBindBuffer(GL_ARRAY_BUFFER, vbo);
//      glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

      for(p = text; *p; p++) {
        glyph_index = FT_Get_Char_Index(type, *p );
        error = FT_Load_Glyph(type, glyph_index, FT_LOAD_DEFAULT );
        if(error)printf(" Load Glyph ERROR %i\n",error) ;
        if(ip)printf(" glyph_index = %i\n",glyph_index );
        error = FT_Render_Glyph(type->glyph, FT_RENDER_MODE_NORMAL );
        if(error)printf(" Render Glyph ERROR %i\n",error) ;
        if(ip)printf(" II3 ::%c:: \n",(int) *p) ;
        fflush(NULL) ;

        g = type->glyph ;
        if(ip)printf(" glyph_slot = %p\n",(void *)g );

        if(ip)printf("  %i %i :: %i %i :: %i %i :: %i %i %i : %i\n",
          g->bitmap.width,
          g->bitmap.rows,
          g->bitmap_left,
          g->bitmap_top,
          (int)g->advance.x,
          (int)g->advance.y,
          g->bitmap.pitch,
          g->bitmap.pixel_mode,
          g->bitmap.num_grays,
          g->bitmap.rows - g->bitmap_top ) ;
//  Copy from freetype bitmap buffer to OpenGL bitmap
        b = (char *)g->bitmap.buffer ;
        for(iu=0;iu<g->bitmap.rows;iu++){
          iu1 = g->bitmap.rows - 1 - iu ;
          for(j=0;j<g->bitmap.pitch;j++){
            bitmap[iu1*g->bitmap.pitch + j]
                               = b[iu*g->bitmap.pitch + j] ;
          }
        }

        if(ip){
          b = (char *)g->bitmap.buffer ;
          for(iu=0;iu<g->bitmap.rows;iu++){
            iu1 = g->bitmap.rows - 1 - iu ;
            for(j=0;j<g->bitmap.pitch;j++){
              for(k=7;k>-1;k--){
                m = (*b & (1<<k))? 1 : 0 ;
                printf(" %i",m) ;
              }
              b++ ;
            }
            printf("\n") ;
          }


          printf("==============================\n") ;
          b = (char *)bitmap ;
          for(iu=0;iu<g->bitmap.rows;iu++){
            for(j=0;j<g->bitmap.pitch;j++){
              for(k=7;k>-1;k--){
                m = (*b & (1<<k))? 1 : 0 ;
                printf(" %i",m) ;
              }
              b++ ;
            }
//            printf(" :: %x  %x\n",(GLubyte)bitmap[j][0],(GLubyte)bitmap[j][1]) ;
              printf("\n") ;
          }
        }
        if(ip)printf(" posn = %f %f %f %f\n",posn[0],posn[1],posn[2],posn[3]) ;
        if(ip)printf(" posn = %f %f\n",xx,yy) ;
//  Set raster position
        xp = xx + g->bitmap_left ;
        yp = yy + ((int)g->bitmap_top - (int)g->bitmap.rows) ;
        glRasterPos4f((GLfloat)xp,(GLfloat)yp,(GLfloat)posn[2],(GLfloat)posn[3]) ;
        glGetFloatv(GL_CURRENT_RASTER_POSITION,posn) ;
// Both of the above lines are needed, glraster2f fails, reason unknown
//        glRasterPos2f((GLfloat)xx,(GLfloat)yy) ;
//        glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID,&boo) ;
//        if(ip)printf(" Routine %s.  Current raster = %f %f %f %f\n",
//               my_name,posn[0],posn[1],posn[2],posn[3]);
//        if(ip)printf(" BB          Position is valid = %i\n",boo) ;

//  Each row of bitmap is packed into bitmap_pitch bytes
        glPixelStorei(GL_UNPACK_ROW_LENGTH,g->bitmap.pitch) ;
//  Plot bitmap - convert (uint) to (int) before any arithmetic
        glDrawPixels(
          (GLsizei)g->bitmap.width,
          (GLsizei)g->bitmap.rows,
          GL_ALPHA,
          GL_UNSIGNED_BYTE,
          (const GLvoid *)bitmap) ;
        xx = xx + g->advance.x/64.0 ;
      }
      glRasterPos4f((GLfloat)xx,(GLfloat)yy,(GLfloat)posn[2],(GLfloat)posn[3]) ;
      ip = 0 ;
      return ;
}

#endif
