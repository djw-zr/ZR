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
 *     myGlutIdle()
 *     reshape()
 *   Plus the graphics version of process_args and graphics utility routines.
 *
 *==============================================================================
 */

/*
 * *****************************************************************************
 *   Reshape - called when window size is changed
 *         also called when the graphics screen is opened
 * *****************************************************************************
 */
void reshape(int w, int h)
{
  int tx, ty, tw, th   ;        //  Origin (x, y), with and height of viewport.
  int      ip = 0 ;             // 0 = no printing
  GLdouble scale_w, scale_h, scale, centre_x, centre_y ;
  GLfloat position[] = { 3.0, 3.0, 3.0, 0.0 };              // Light position
  GLint   iparms[4] ;
  double   sc ;
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
 * For Orthographic prjection scale track tiles to fit within window.
 */
    if(o_plot){
      if(l_pd  || ip)printf(" Use glOrtho\n");
        if(0 == ortho_set){
          sc = 0.90 ;

          scale_w = tw*sc/(tile_east - tile_west + 1);
          scale_h = th*sc/(tile_north - tile_south + 1);
          if(scale_w < scale_h) scale = 0.5/scale_w ;
            else scale = 0.5/scale_h;
          centre_x = (tile_east  + tile_west  - 1)*0.5 - tile_x0 ;
          centre_y = (tile_north + tile_south - 1)*0.5 - tile_y0 ;
          ortho_left      = centre_x - w*scale ;
          ortho_right     = centre_x + w*scale ;
          ortho_bottom    = centre_y - h*scale ;
          ortho_top       = centre_y + h*scale ;
          ortho_near      = -0.01 ;
          ortho_far       =  1.0;
          ortho_set = 1 ;
        }
        sc = (ortho_right-ortho_left)/(ortho_top - ortho_bottom);
        if(l_pd || ip)printf(" Ortho %f %f %f %f %f %f %f\n",
                              (double)ortho_left,(double)ortho_right,
                              (double)ortho_bottom,(double)ortho_top,
                              (double)ortho_near,(double)ortho_far,sc);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(ortho_left, ortho_right, ortho_bottom, ortho_top,
                                        ortho_near,   ortho_far) ;
        glScalef(1.0,1.0,-1.0) ;
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLightfv(GL_LIGHT0, GL_POSITION, position);  //  REQUIRED after MODULVIEW CHANGE
/*
 *  For
 */
    }else{
      if(l_pd  || ip)printf(" Use gluPerspective\n");
      viewport_aspect = (GLfloat)tw/(GLfloat)th    ;
      viewport_near   = viewport_near_m/plot_scale ;
      viewport_far    = viewport_far_m/plot_scale  ;
      glMatrixMode(GL_PROJECTION) ;
      glLoadIdentity() ;
      gluPerspective(viewport_fovy,viewport_aspect,viewport_near,viewport_far);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity() ;
      gluLookAt(lookat_eye_x,lookat_eye_y,lookat_eye_z,
               lookat_centre_x,lookat_centre_y,lookat_centre_z,
               lookat_up_x,lookat_up_y,lookat_up_z) ;
      glLightfv(GL_LIGHT0, GL_POSITION, position) ;  //  REQUIRED after MODELVIEW CHANGE
      initialise_eye_vectors() ;
    }
//    new_viewpoint = 1 ;   // graphics_cull() not needed with reshape
    if(l_pd || ip)printf(" Exit  reshape()\n");

}

/***************************************** myGlutIdle() ***********/

void myGlutIdle( void )
{
int ip = 0                ;  // = 1 for debug printing
int        tile_eye_x = lookat_eye_x ;
int        tile_eye_y = lookat_eye_y ;
static int last_eye_x = 0 ;
static int last_eye_y = 0 ;

/*
 * If viewpoint has moved to a new tile, check for changes in
 * the required shapes and textures
 */
      if(lookat_eye_x<0) tile_eye_x-- ;
      if(lookat_eye_y<0) tile_eye_y-- ;
      if(new_viewpoint && ((tile_eye_x != last_eye_x)
                        || (tile_eye_y != last_eye_y))){
        if(ip)printf(" myGlutIdle :: Tile %i %i :: Eye %f %f :: Last %i %i\n",
                  tile_eye_x, tile_eye_y, lookat_eye_x, lookat_eye_y,
                  last_eye_x, last_eye_y) ;
        graphics_cull() ;
        last_eye_x = tile_eye_x ;
        last_eye_y = tile_eye_y ;
        new_viewpoint = 0 ;
      }

/*
 * According to the GLUT specification, the current window is
 * undefined during an idle callback.  So we need to explicitly change
 *it if necessary
 */
        glutPostRedisplay();
        if(ip)printf(" Exit myGlutIdle\n");
        return ;
}


void  print_string_in_window(GLfloat wx, GLfloat wy, char *string)
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
void *font ;

      glWindowPos2f(wx,wy) ;   /* Initial window pixel position */
      font = GLUT_BITMAP_HELVETICA_18 ;
      glutPrintString(string,font);
      return ;
}

void  print_string_in_window2(GLfloat rx, GLfloat ry, GLfloat rz, char *string)
{
/*
 *  Print string at fixed position in 3D space coordinates
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

void *font ;
      glRasterPos3d(rx,ry, rz) ;
      font = GLUT_BITMAP_HELVETICA_12 ;
      glutPrintString(string,font);
    }

void glutPrintString(char *string, void *font)
{
int len, i;

    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(font, string[i]);
    }
}

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
      if(ip)printf("  Lookat_Centre = %f %f %f\n",lookat_centre_x,lookat_centre_y,lookat_centre_z) ;
      if(ip)printf("  Lookat_Eye    = %f %f %f\n",lookat_eye_x,lookat_eye_y,lookat_eye_z) ;
      x = lookat_centre_x - lookat_eye_x ;
      y = lookat_centre_y - lookat_eye_y ;
      z = lookat_centre_z - lookat_eye_z ;
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
 */

int   check_in_scene(GLdouble x, GLdouble y, GLdouble z, GLdouble r, GLdouble d){

int       ip = 1;
GLdouble  xx0, yy0, zz0, xx1, yy1, zz1, xx, yy,
          r0, rr ;
char      my_name[]="check_in_scene" ;

      ip = ipp ;
      ipp = 0 ;
      if(ip)printf("\n  Enter %s\n",my_name) ;
      if(ip)printf("  x,                          y,               z    = %f %f %f,  r,  d = %f %f\n",x,y,z,r,d) ;
      if(ip)printf("  lookat_eye_x,    lookat_eye_y,    lookat_eye_z    = %f %f %f\n",lookat_eye_x,lookat_eye_y,lookat_eye_z) ;
      xx1 = x - lookat_eye_x ;
      yy1 = y - lookat_eye_y ;
      zz1 = z - lookat_eye_z ;
      if(ip)printf("  xx1,                      yy1,              zz1   = %f %f %f\n",xx1,yy1,zz1) ;
      xx0 = eye_x_x*xx1 + eye_x_y*yy1 + eye_x_z*zz1 ;
      yy0 = eye_y_x*xx1 + eye_y_y*yy1 + eye_y_z*zz1 ;
      zz0 = eye_z_x*xx1 + eye_z_y*yy1 + eye_z_z*zz1 ;
      if(ip)printf("  xx0,                      yy0,              zz0   = %f %f %f\n",xx0,yy0,zz0) ;

      rr = xx0*xx0 + yy0*yy0 + zz0*zz0 ;
      r0 = 200.0/plot_scale       ;                                          // 200 m in units of tile
      if(rr < r0*r0) return 1 ;                                          // Plot everything within 200m
      if(zz0 < 0.0)  return 0 ;                                          // Object behind eye/camera
      if(d != 0.0 && zz0 > d) return 0 ;                                 // Object to far away

      xx = xx0/zz0 ; yy = yy0/zz0 ;  rr = r/zz0  ;
      if(ip)printf("                      screen_hw_x,     screen_hw_y  = %f %f \n",screen_hw_x,screen_hw_y) ;
      if(ip)printf("  AA                           xx,              yy  = %f %f \n",xx,yy) ;

      if(xx < (screen_hw_x + rr) && xx > -(screen_hw_x + rr) &&
         yy < (screen_hw_y + rr) && xx > -(screen_hw_y + rr) ) return 1 ;  // Within enlaged window
      return 0 ;                                                           // Outside window
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

int  global2local(int tile_e0, int tile_n0, float h0, float size, float scale,
                int tile_e,  int tile_n,
                float e,     float n,     float h,
                float *x,    float *y,    float *z){
float  scalei = 1.0/scale ;

      *x = (float)scalei*((tile_e - tile_e0 + 0.5)*size + e) ;
      *y = (float)scalei*((tile_n - tile_n0 + 0.5)*size + n) ;
      *z = (float)scalei*(h - h0) ;
      return 0 ;
}

int  local2msts(int tile_e0, int tile_n0, float h0, float size, float scale,
                float x,     float y,     float z,
                int *tile_e, int *tile_n,
                float *e,    float *n,    float *h){
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
float x, y, z, e, n, h ;

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
 *  Routine to check for OpenGL error and print an error message
 */
int  check_glerror(){
  GLenum  n ;
      n = glGetError() ;
      if(n == GL_NO_ERROR) return 0;
      if(n == GL_INVALID_ENUM)     printf(" OpenGL invalid enum argument \n") ;
      if(n == GL_INVALID_VALUE)    printf(" OpenGL invalid argument value\n") ;
      if(n == GL_INVALID_OPERATION)printf(" OpenGL invalid operation\n") ;
      if(n == GL_STACK_OVERFLOW)   printf(" OpenGL stack overflow\n") ;
      if(n == GL_STACK_UNDERFLOW)  printf(" OpenGL stack underflow\n") ;
      if(n == GL_OUT_OF_MEMORY)    printf(" OpenGL out of memory\n") ;
      if(n == GL_TABLE_TOO_LARGE)  printf(" OpenGL table too large\n") ;
      return 1;
}
int  check_glerror2(char* string){
  GLenum  n ;
      n = glGetError() ;
      if(n == GL_NO_ERROR) return 0;
      printf(" OpenGL error :: message = %s\n",string) ;
      if(n == GL_INVALID_ENUM)     printf(" OpenGL invalid enum argument \n") ;
      if(n == GL_INVALID_VALUE)    printf(" OpenGL invalid argument value\n") ;
      if(n == GL_INVALID_OPERATION)printf(" OpenGL invalid operation\n") ;
      if(n == GL_STACK_OVERFLOW)   printf(" OpenGL stack overflow\n") ;
      if(n == GL_STACK_UNDERFLOW)  printf(" OpenGL stack underflow\n") ;
      if(n == GL_OUT_OF_MEMORY)    printf(" OpenGL out of memory\n") ;
      if(n == GL_TABLE_TOO_LARGE)  printf(" OpenGL table too large\n") ;
      return 1;
}
