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

/*
 *  Routine to transform from MSTS coordinates, tile units and metres
 *     to units of scale_dist
 *  The routine was introduced to reduce rounding errors when
 *     displaying the 3-D scene.
 *
 *  tile_e0, tile_n0     :: MSTS coordinates (east,north) of local
 *                                                     coordinates origin tile
 *  h0                   :: Height (m) at origin of local coordinates
 *  size                 :: Size of MSTS tiles (m).
 *  scale_dist           :: One 'local distance unit' equals 'scale_dist' metres
 *  tile_e,  tile_n      :: MSTS coordinates of current tile
 *  e, n                 :: MSTS coordinates (m) relative to centre of current tile
 *  h                    :: MSTS height
 *  x, y, z              :: Position in 'scale_dist' units
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
#if 0
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


#endif
/*
 *   Transform a MSTS wagon coordinate to the corresponding local co-ordinate
 *   including the translation along the current track vector.  This mimics
 *   the transformation used when drawing wagons and is used to check if a
 *   wagon is within the scene
 *
 *   x0, y0, z0  ::  Wagon vector (metres)
 *   xt, yt, zt  ::  position relative to origin of track vector (metres)
 *   a,  b,  c   ::  rotations of track about its origin (degrees)
 *   scalei      ::  convert from metres to model grid unit
 *   rx, ry, rz  ::  position of track vector origin (grid units)
 *   x,  y, z    ::  final position in scale units
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
#if 0
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
#endif
