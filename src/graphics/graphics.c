/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  graphics.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Graphics subroutines used by ZR.
 *   The present code is based on OpenGL 2.1 and uses free glut for the
 *   interface with the windowing system and the keyboard.
 *
 *   The main display routines are now in files 'display_init.c' and
 *   'display.c'.  Keyboard input is handled by 'keyboard.c'.
 *
 *   This file includes the glut call-back routines:
 *     reshape()
 *   Plus the graphics version of process_args and graphics utility routines.
 *
 *==============================================================================
 */
int  sdl2PrintString(char *string);

//FT_Library ft;
//FT_Face face;

/*
 * *****************************************************************************
 *   Reshape - called when window size is changed
 *         also called when the graphics screen is opened
 * *****************************************************************************
 */
#ifdef SDL2

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void reshape2(int w, int h)
{
#pragma GCC diagnostic pop

      SDL_GetWindowSize(Window, &viewport_width, &viewport_height) ;
      glViewport(0, 0, viewport_width, viewport_height);
      viewport_aspect = (GLfloat)viewport_width/(GLfloat)viewport_height    ;
      glMatrixMode(GL_PROJECTION) ;
      glLoadIdentity() ;
      gluPerspective(viewport_fovy,viewport_aspect,viewport_near,viewport_far);
      glMatrixMode(GL_MODELVIEW);
}


#else
void reshape(int w, int h)
{
int tx, ty, tw, th   ;        //  Origin (x, y), with and height of viewport.
int      ip = 0 ;             // 0 = no printing
GLint   iparms[4] ;

      if(l_pd  || ip)printf(" Enter reshape\n");

//    GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th) ;
      glGetIntegerv(GL_VIEWPORT,iparms);
      tx = iparms[0] ;
      ty = iparms[1] ;
      tw = w ;
      th = h ;
      if(ip)printf("  Reshape : %i %i %i %i\n",tx,ty,tw,th);

      glViewport((GLsizei) tx, (GLsizei) ty, (GLsizei) tw, (GLsizei) th);
      viewport_width  = tw ;
      viewport_height = th ;
/*
 *  Initialise matrices
 */
        if(l_pd  || ip)printf(" Use gluPerspective\n");
        viewport_aspect = (GLfloat)tw/(GLfloat)th    ;
#if defined _Display_Shapes || defined _Display_Wagons
        viewport_near = 1.0/plot_scale ;
        viewport_far  = 4096.0/plot_scale ;
#elif _Display_Textures
        viewport_near =  0.1 ;
        viewport_far  = 10.0 ;
#else
        viewport_near   = viewport_near_m/plot_scale ;
        viewport_far    = viewport_far_m/plot_scale  ;
#endif
        glMatrixMode(GL_PROJECTION) ;
        glLoadIdentity() ;
        gluPerspective(viewport_fovy,viewport_aspect,viewport_near,viewport_far);
        camera_changed = 1;
        camera_new_position() ;

        if(l_pd || ip)printf(" Exit  reshape()\n");
}
#endif


//void glWindowPos2ff(GLfloat x, GLfloat y) ;

//  Use glWindowPosn
int  print_string_in_window(GLfloat wx, GLfloat wy, char *string)
{
/*
 *  Print string at fixed position on screen.
 *    wx = starting pixel relative to left of screen
 *    wy = starting pixel relative to bottom of screen
 *
 *  font can be one of the standard glut fonts:
 *
 *       GLUT_BITMAP_8_BY_13
 *       GLUT_BITMAP_9_BY_15
 *       GLUT_BITMAP_TIMES_ROMAN_10
 *       GLUT_BITMAP_TIMES_ROMAN_24
 *       GLUT_BITMAP_HELVETICA_10
 *       GLUT_BITMAP_HELVETICA_12
 *       GLUT_BITMAP_HELVETICA_18
 */
//char my_name[] = "print_string_in_window" ;
//      printf("  Enter %s\n",my_name) ;

      glWindowPos2f(wx,wy) ;
      print_string_in_window_z(string, 18) ;

      return 0 ;
}

//  Use glRasterPosn
int  print_string_in_window2(GLfloat rx, GLfloat ry, GLfloat rz, char *string){

//char my_name[] = "print_string_in_window2" ;
//      printf("  Enter %s\n",my_name) ;

      glPushAttrib(GL_ENABLE_BIT) ;
      glDisable(GL_LIGHTING)  ;
      glDisable(GL_TEXTURE_2D) ;
      glEnable(GL_BLEND)      ;

      glRasterPos3d(rx, ry, rz) ;
      print_string_in_window_z(string, 12) ;

      glPopAttrib() ;
      return 0 ;
      }

//  Use glWindowPosn
int print_string_in_window3(GLfloat wx, GLfloat wy, char *string, int font_size){

//char my_name[] = "print_string_in_window3" ;
//      printf("  Enter %s\n",my_name) ;
      glWindowPos2f(wx,wy) ;
      print_string_in_window_z(string, font_size) ;
      return 0;
}

int    print_string_in_window_z(char *string, int font_size){

char my_name[] = "print_string_in_window_z" ;
#ifdef USE_FREETYPE
      render_text_as_greyscale(string, ft_verdana, font_size-2) ;  // Fixed space
#elif defined USE_SDLTTF
      switch(font_size) {
        case 12:
          ttf_font = ttf_font_f12 ;
          break ;
        case 14:
          ttf_font = ttf_font_f14 ;
          break ;
        case 16:
          ttf_font = ttf_font_f16 ;
          break ;
        case 18:
          ttf_font = ttf_font_f18 ;
          break ;
        default:
          printf("  ERROR in routine %s\n",my_name) ;
          printf("    Font size %i is not supported. \n",font_size) ;
          printf("  Program stopping ...\n") ;
          exit(1) ;
      }
      sdl2PrintString(string) ;
#else
void *font ;
      switch(font_size) {
        case 10:
          font = GLUT_BITMAP_HELVETICA_10 ;
          break ;
        case 12:
          font = GLUT_BITMAP_HELVETICA_12 ;
          break ;
        case 18:
          font = GLUT_BITMAP_HELVETICA_18 ;
          break ;
        default:
          printf("  ERROR in routine %s\n",my_name) ;
          printf("    Font size %i is not supported. \n",font_size) ;
          printf("  Program stopping ...\n") ;
          exit(1) ;
      }
      glutBitmapString(font, (const unsigned char *)string) ;
//      glutPrintString(string,font);
#endif
      return 0 ;
}

int  width_string_in_window(char *string, int font_size){

char my_name[] = "width_string_in_window_z" ;

#ifdef USE_FREETYPE
      return strlen(string)*font_size*0.75 ;
#elif defined USE_SDLTTF
      return strlen(string)*font_size*0.75 ;
#else
void *font ;
      switch(font_size) {
        case 10:
          font = GLUT_BITMAP_HELVETICA_10 ;
          break ;
        case 12:
          font = GLUT_BITMAP_HELVETICA_12 ;
          break ;
        case 18:
          font = GLUT_BITMAP_HELVETICA_18 ;
          break ;
        default:
          printf("  ERROR in routine %s\n",my_name) ;
          printf("    Font size %i is not supported. \n",font_size) ;
          printf("  Program stopping ...\n") ;
          exit(1) ;
      }
      return glutBitmapLength(font, (const unsigned char *)string) ;
#endif
}
int  height_font_in_window(int font_size){

char my_name[] = "width_string_in_window_z" ;

#ifdef USE_FREETYPE
      return font_size*0.75 ;
#elif defined USE_SDLTTF
      return font_size*0.75 ;
#else
void *font ;
      switch(font_size) {
        case 10:
          font = GLUT_BITMAP_HELVETICA_10 ;
          break ;
        case 12:
          font = GLUT_BITMAP_HELVETICA_12 ;
          break ;
        case 18:
          font = GLUT_BITMAP_HELVETICA_18 ;
          break ;
        default:
          printf("  ERROR in routine %s\n",my_name) ;
          printf("    Font size %i is not supported. \n",font_size) ;
          printf("  Program stopping ...\n") ;
          exit(1) ;
      }
      return glutBitmapHeight(font) ;
#endif
}



#ifdef USE_SDLTTF
int  sdl2PrintString(char *string){
  int     ip = 0   ;
  int     ired, igreen, iblue, ialpha ;
  Uint32  i, j, k, l ;
  Uint8   pred, pgreen, pblue, palpha ;
  SDL_Surface  *surface ;                           //  SDL_surface.h
  SDL_PixelFormat *pf ;                       //  SDL_pixels.h
  SDL_Color    fg = {255, 255, 255, 100},
               bg = {255,   0,   0,   0} ;          //  SDL_pixels.h
  GLfloat color[4]    ;
  int text_width  ;
  int text_height ;
  int pitch       ;
  Uint8 *px       ;
  Uint8 *pg       ;

      glGetFloatv(GL_CURRENT_COLOR, color) ;
      ired   = nint(color[0]*255) ;
      igreen = nint(color[1]*255) ;
      iblue  = nint(color[2]*255) ;
//      ialpha = nint(color[3]*255) ;
//      printf("  GL_CURRENT_COLOR = %f %f %f %f\n",color[0],color[1],color[2],color[3]) ;

      surface = TTF_RenderText_Shaded(ttf_font, string, fg, bg) ;
      text_width  = surface->w      ;
      text_height = surface->h      ;
      pitch       = surface->pitch  ;
//      pf          = surface->format ;
      px          = surface->pixels ;


      pg = malloc(text_width*text_height*4*sizeof(Uint8)) ;
      k = 0 ;
      l = 0 ;
      for(j=0;j<text_height;j++){
        k = (text_height-j-1)*pitch  ;
        for(i=0;i<text_width;i++,k++,l+=4){
           pg[l]   = px[k]  ;     //  alpha
           pg[l+1] = iblue  ;         //  blue
           pg[l+2] = igreen ;         //  green
           pg[l+3] = ired   ;         //  red
        }
      }
      glDrawPixels(text_width, text_height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pg) ;
      SDL_FreeSurface(surface) ;
      free(pg) ;
      return 0 ;
}
#endif

#if defined GLUT || defined USE_ZRGLUT
int glutPrintString(char *string, void *font){

int len, i;

    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(font, string[i]);
    }
    return 0 ;
}
#endif

/*
 * Test graphics
 */
void makeCheckImages(void)
{
  int  i, j, c ;

      for(i=0;i<checkImageHeight; i++) {
        for(j=0;j<checkImageWidth;j++) {
          c = (((i&0x8)==0) ^ ((j&0x8)==0)) *150 ;
          checkImage[i][j][0] = (GLubyte) c;
          checkImage[i][j][1] = (GLubyte) c;
          checkImage[i][j][2] = (GLubyte) c;
          checkImage[i][j][3] = (GLubyte) 10;
        }
      }
      for(i=0;i<subImageHeight; i++) {
        for(j=0;j<subImageWidth;j++) {
          c = (((i&0x4)==0) ^ ((j&0x4)==0)) *255 ;
          subImage[i][j][0] = (GLubyte) c;
          subImage[i][j][1] = (GLubyte) c;
          subImage[i][j][2] = (GLubyte) c;
          subImage[i][j][3] = (GLubyte) 10;
        }
      }
//      c_width = checkImageWidth ;
//      c_height = checkImageHeight ;
//      c_rgba = (GLubyte *)checkImage ;

      return ;
}


/*
 *==============================================================================
 * Routine to return height of points within surrounding tiles
 *   Computationally uneconomic but the code is cleaner and easier to debug.
 *
 *==============================================================================
 */
double tile_height(int tile_x, int tile_y, int i, int j, int len){
  int          nn, nt;
  TileListNode *tile_node ;

      if(i<0){
        tile_x = tile_x - 1 ;  i = i + len  ;
      }else if(i>len-1){
        tile_x = tile_x + 1 ;  i = i - len  ;
      }
      if(j<0){
        tile_y = tile_y + 1 ;  j = j + len  ;
      }else if(j>len-1){
        tile_y = tile_y - 1 ;  j = j - len  ;
      }
      if(tile_x < tile_west || tile_x > tile_east ||
         tile_y < tile_south || tile_y > tile_north) return 0.0 ;
      nt = tile_east + 1 - tile_west ;
      nn = (tile_y-tile_south)*nt + tile_x - tile_west ;
      tile_node = tile_array[nn] ;

      if(tile_node == NULL || tile_node->t_found == 0) return 0.0 ;

      return tile_node->terrain_data.terrain_sample_floor
           + tile_node->terrain_data.terrain_sample_scale
           * tile_node->terrain_data.elevations[j*len + i] ;
}

/*
 * =============================================================================
 */

/*
 *  Routine to initialise the normalised eye vectors 'eye_?_?'
 *  defined in 'graphics .h'.
 *  They are unit vectors in the model view space which correspond
 *  to the directions right, up and forward seen by the eye in the
 *  viewport.
 */

int  initialise_eye_vectors(){
int       ip = 0 ;
GLdouble  x, y, z, r3, r2 ;
char      my_name[]="initialise_eye_vectors";

      ipp = ip ;

      if(ip)printf("\n  Enter %s\n",my_name) ;
      if(ip)printf("  Lookat_Centre = %f %f %f\n",lookat_center_x,lookat_center_y,lookat_center_z) ;
      if(ip)printf("  Lookat_Eye    = %f %f %f\n", lookat_eye_x, lookat_eye_y, lookat_eye_z) ;
      x = lookat_center_x - lookat_eye_x ;
      y = lookat_center_y - lookat_eye_y ;
      z = lookat_center_z - lookat_eye_z ;
      r2 = sqrt(x*x + y*y)       ;
      r3 = sqrt(x*x + y*y + z*z) ;

      eye_x_x =  y/r2   ;    //  Vector to right of viewport/screen
      eye_x_y = -x/r2   ;
      eye_x_z = 0.0     ;    //  Vector kept horizontal in ZR

      eye_y_x = -z*x/r2 ;    //  Vector up viewport screen
      eye_y_y =  z*y/r2 ;
      eye_y_z =  r2     ;

      eye_z_x =  x/r3   ;    //  Vector into viewport screen
      eye_z_y =  y/r3   ;
      eye_z_z =  z/r3   ;

     if(ip)printf("  eye_x_*    = %f %f %f\n",eye_x_x,eye_x_y,eye_x_z) ;
     if(ip)printf("  eye_y_*    = %f %f %f\n",eye_y_x,eye_y_y,eye_y_z) ;
     if(ip)printf("  eye_z_*    = %f %f %f\n",eye_z_x,eye_z_y,eye_z_z) ;

      screen_hw_y = tan(radian*viewport_fovy*0.5) ;
      screen_hw_x = screen_hw_y*viewport_width/viewport_height ;

      return 0 ;
}


/*
 *  Routine to determine whether any part of a sphere radius r
 *  around point (x, y, z) can be seen through the viewport
 *  This does not check the depth buffer to see if other object
 *  may hide the sphere
 *     Position of object    :  (x,y,z)
 *     Radius of object      :       r
 *     Out of sight distance :       d  (idnored if d is zero)
 *  NOTE: During development this assumes tile units
 *
 *     returns 0 if out of range
 *             1 if within range
 */

int   check_in_scene(GLdouble x, GLdouble y, GLdouble z, GLdouble r){

int      k ;
double  sum ;
char      my_name[]="check_in_scene" ;

/*
 *  Use clip planes
 */
      for(k=0;k<6;k++){
        sum = x*clip_a[k][0] + y*clip_a[k][1] + z*clip_a[k][2] + clip_a[k][3] ;
        if(sum + r < 0.0) return 0 ;
      }
      return 1 ;
}


int   check_topog_in_scene(TileListNode *tl_node){

int     i, j, k, l ;
double  xa[2], ya[2], x, y, z, sum ;
char    my_name[]="check_topog_in_scene" ;

      xa[0] = tl_node->tilex - tile_x0 ;  xa[1] = xa[0] + 1.0 ;
      ya[0] = tl_node->tiley - tile_y0 ;  ya[1] = ya[0] + 1.0 ;
      z = lookat_eye_z ;
/*
 *  Loop over clip planes
 *  To pass, every clip plane test must be satified by at least one
 *  of the limits of the tile
 */
// Loop over tests
      for(k=0;k<6;k++){
        l = 0 ;
        for(i=0;i<2;i++){
          x = xa[i] ;
          for(j=0;j<2;j++){
            y = ya[j] ;
            sum = x*clip_a[k][0] + y*clip_a[k][1] + z*clip_a[k][2] + clip_a[k][3] ;
            if(sum > 0.0){ l++ ; break ;}
          }
        }
// test l
        if(0==l)return 0;
      }
      return 1 ;
}
/*
 *   Routine to check if object cube is visible in viewport
 *   The cube will not be visible if all of the points fail one of
 *   the tests
 */
int   check_topog_in_scene2(GLfloat xa[2], GLfloat ya[2], GLfloat za[2]){

int     i, j, k, l, m ;
double  x, y, z, sum ;
char    my_name[]="check_topog_in_scene2" ;

/*
 *  Loop over clip planes
 *  To pass every clip plane test must be satified by at least one
 *  of the corner points of the brick
 */
// Loop over tests
      for(l=0;l<6;l++){
        m = 0 ;
        for(i=0;i<2;i++){
          x = xa[i] ;
          for(j=0;j<2;j++){
            y = ya[j] ;
            for(k=0;k<2;k++){
              z = za[k] ;
              sum = x*clip_a[l][0] + y*clip_a[l][1] + z*clip_a[l][2] + clip_a[l][3] ;
              if(sum > 0.0){ m++ ; break ;}
            }
          }
        }
// test m
        if(0==m)return 0;  //  Failure
      }
      return 1 ;           // Success
}






/*
 *  Routines to transform from MSTS coordinates to the local coordinates
 *  introduced to reduce rounding errors when displaying the 3-D scene.
 *
 *  tile_e0, tile_n0         :: MSTS coordinates (east,north) of local
 *                                                         coordinates origin tile
 *  h0                       :: Height (m) at origin of local coordinates
 *  size                     :: Size of MSTS tiles (m).
 *  scale_dist               :: One 'local distance unit' equals 'scale_dist' metres
 *  tile_e,  tile_n          :: MSTS coordinates of current tile
 *  e, n                     :: MSTS coordinates relative to centre of current tile
 *  h                        :: MSTS height
 *  x, y, z                  :: Position in local coordinates
 */

int  global2local(int tile_e0, int tile_n0, double h0, double size, double scale,
                int tile_e,  int tile_n,
                double e,     double n,     double h,
                double *x,    double *y,    double *z ){
double  scalei = 1.0/scale ;

      *x = scalei*((tile_e - tile_e0 + 0.5)*size + e) ;
      *y = scalei*((tile_n - tile_n0 + 0.5)*size + n) ;
      *z = scalei*(h - h0) ;
      return 0 ;
}

int  local2msts(int tile_e0, int tile_n0, double h0, double size, double scale,
                double x,     double y,     double z,
                int *tile_e, int *tile_n,
                double *e,    double *n,    double *h){
float xp, yp ;
int   le, ln ;

      xp = x*scale ;                              //  Convert to metres
      yp = y*scale ;
      le = (xp >= 0.0)? xp/size : xp/size - 1.0;  //  Tile in local coordinate
      ln = (yp >= 0.0)? yp/size : yp/size - 1.0;  //  Tile in local coordinate
      *e = xp - (le - 0.5)*size ;
      *n = yp - (ln - 0.5)*size ;
      *h = h0 + z*scale ;
      *tile_e = le + tile_e0 ;         // Add back origin of local tiles
      *tile_n = ln + tile_n0 ;
      return 0 ;
}

int test_transforms(){

int   tile_e0 = 3000,   tile_n0 =  500  ;
float size    = 2040.0, scale   = plot_scale, h0 = tile_h0 ;
int   tile_e, tile_n ;
double x, y, z, e, n, h ;

      tile_e = 3010 ;
      tile_n =  520 ;
      e =  100.0 ;
      n = -200.0 ;
      h = plot_scale ;
      global2local(tile_e0, tile_n0, h0, size, scale, tile_e, tile_n, e, n, h,
                 &x, &y, &z );
      printf(" global2local = %i %i :: %f %f %f -> %f %f %f\n",
             tile_e, tile_n, e, n, h, x, y, z ) ;

      local2msts(tile_e0, tile_n0, h0, size, scale,
                 x, y, z,
                 &tile_e, &tile_n, &e, &n, &h );
      printf(" local2msts = %i %i :: %f %f %f <- %f %f %f\n",
             tile_e, tile_n, e, n, h, x, y, z ) ;
      exit(0);
      return 0 ;
}
/*
 *   Transform a MSTS wagon coordinate to the corresponding local co-ordinate
 *   including the translation along the current track vector.  This mimics
 *   the transformation used when drawing wagons and is used to check if a
 *   wagon is within the scene
 *
 *   x0, y0, z0  ::  Wagon vector
 *   xt, yt, zt  ::  position relative to origin of track vector (metres)
 *   a,  b,  c   ::  rotations of track about its origin (degrees)
 *   scalei      ::  convert from metres to model grid unit
 *   rx, ry, rz  ::  position of track vector origin (grid units)
 *
 */
int  mstswagon2local(double x0, double y0, double z0,
                     double xt, double yt, double zt,
                     double a,  double b,  double c,
                     double scalei,
                     double rx, double ry, double rz,
                     double *x, double *y, double *z){

double x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, cc, ss;

      x1 = x0 + xt ;
      y1 = y0 + yt ;
      z1 = z0 + zt ;
//  Roll
      cc = cos(c*radian) ; ss = sin(c*radian) ;
      x2 = cc*x1 - ss*y1 ;
      y2 = ss*x1 + cc*y1 ;
      z2 = z1 ;
//  Pitch
      cc = cos(a*radian) ; ss = sin(a*radian) ;
      x3 = x2 ;
      y3 = cc*y2 - ss*z2 ;
      z3 = ss*y2 + cc*z2 ;
//  Yaw
      cc = cos(b*radian) ; ss = sin(b*radian) ;
      x4 = cc*x3 + ss*z3 ;
      y4 = y3 ;
      z4 =-ss*x3 + cc*z3 ;
//  Convert from MSTS coordinates to ZR model coordinates
//          and add rail vector origin
      *x  = scalei*x4 + rx ;
      *y  = scalei*z4 + ry ;
      *z  = scalei*y4 + rz ;

      return 0 ;
}

/*
 *  Routine to check for OpenGL error and print an error message
 */
int  check_glerror(){
  GLenum  n ;
      n = glGetError() ;
      if(n == GL_NO_ERROR) return 0;

      gl_errorCode = n ;
      gl_errString = (GLubyte *)gluErrorString(n) ;

      if(n == GL_INVALID_ENUM)     printf(" OpenGL invalid enum argument \n") ;
      if(n == GL_INVALID_VALUE)    printf(" OpenGL invalid argument value\n") ;
      if(n == GL_INVALID_OPERATION)printf(" OpenGL invalid operation\n") ;
      if(n == GL_STACK_OVERFLOW)   printf(" OpenGL stack overflow\n") ;
      if(n == GL_STACK_UNDERFLOW)  printf(" OpenGL stack underflow\n") ;
      if(n == GL_OUT_OF_MEMORY)    printf(" OpenGL out of memory\n") ;
      if(n == GL_TABLE_TOO_LARGE)  printf(" OpenGL table too large\n") ;
      return n;
}
int  check_glerror2(char* string){
  GLenum  n ;
      n = glGetError() ;
      if(n == GL_NO_ERROR) return 0;

      gl_errorCode = n ;
      gl_errString = (GLubyte *)gluErrorString(n) ;

      printf(" OpenGL error :: message = %s\n",string) ;
      if(n == GL_INVALID_ENUM)     printf(" OpenGL invalid enum argument \n") ;
      if(n == GL_INVALID_VALUE)    printf(" OpenGL invalid argument value\n") ;
      if(n == GL_INVALID_OPERATION)printf(" OpenGL invalid operation\n") ;
      if(n == GL_STACK_OVERFLOW)   printf(" OpenGL stack overflow\n") ;
      if(n == GL_STACK_UNDERFLOW)  printf(" OpenGL stack underflow\n") ;
      if(n == GL_OUT_OF_MEMORY)    printf(" OpenGL out of memory\n") ;
      if(n == GL_TABLE_TOO_LARGE)  printf(" OpenGL table too large\n") ;
      return n;
}
