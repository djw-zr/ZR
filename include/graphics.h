/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  graphics.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Graphics variables used by ZR
 *
 *==============================================================================
 */
#define GL_GLEXT_PROTOTYPES  // Needed for glWindowPos2f()

#if !defined _Display_Shapes && !defined _Display_Wagons \
                             && !defined _Display_Textures
  #define _Display_Normal
#endif

#include <GL/gl.h>
//#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>

int     l_plot = 0  ;   // True if plotting on
int     o_plot = 0  ;   // True if orthographic plot
int     tst_tile_x =  1448 ;       //  Initial test  1448
int     tst_tile_y = 10331 ;       //  Initial test 10331


GLdouble pi     = M_PI       ;
GLdouble radian = M_PI/180.0 ;
GLdouble degree = 180.0/M_PI ;
/*
 *  The initial 3D view is defined by the following variables
 *  plus the lookat_eye and lookat_centre variables defined
 *  below.  The derived variables angle_to_north and angle_to_up
 *  are used by subroutines when changing the direction of view.
 *  These then update the lookat_centre variables.
 */

int   main_window_width  = 1500 ;
int   main_window_height = 1000 ;
int   main_window_x0     =  100 ;
int   main_window_y0     =  100 ;

GLsizei  viewport_width  = 1500 ;   // = main_window_width
GLsizei  viewport_height = 1000 ;   // = main_window_height
GLsizei  viewport_x0     =  100 ;   // = main_window_x0
GLsizei  viewport_y0     =  100 ;   // = main_window_y0

GLdouble viewport_fovy   = 30.0 ;
GLdouble viewport_aspect =  1.0 ;
GLdouble viewport_near_m =  1.0 ;    // Nearest point displayed 1 m(m)
GLdouble viewport_far_m  = 10240.0 ; // Furthest point displayed ~10 km (m)
// Unit vectors in model space corresponding to:
GLdouble eye_x_x, eye_x_y, eye_x_z ;  //    Unit right vector   (x) in screen
GLdouble eye_y_x, eye_y_y, eye_y_z ;  //    Unit up vector      (y) in screen
GLdouble eye_z_x, eye_z_y, eye_z_z ;  //    Unit forward vector (z) in screen

GLdouble screen_hw_x               ;  // Screen half width at distance 1.0
GLdouble screen_hw_y               ;  // Screen half height at distance 1.0

int      tile_x0                ;  // Local x-origin (east) for plotting
int      tile_y0                ;  // Local y-origin (north) for plotting
float    tile_h0    = 900.0     ;  // Local height origin for plotting
float    tile_size  = 2048.0    ;  // Size of tile (m)
float    plot_scale = 2048.0    ;  // Size in m of one unit in plotting space
int      tile_eye_x0            ;  // Tile containing viewpoint
int      tile_eye_y0            ;

int      new_viewpoint = 0      ;  // Set to 1 when projection changed
GLdouble viewport_near          ;
GLdouble viewport_far           ;
#ifdef full_topog
  int tile_cull_r = 0           ;  // Plot all topography
#else
  int tile_cull_r = 2           ;  // Limit to 3x3 tiles around eye point.
#endif

#if defined _Display_Wagons || defined _Display_Shapes
GLdouble lookat_eye_x    =   0.01  ;      //  Initial position of eye
GLdouble lookat_eye_y    =  -0.03  ;
GLdouble lookat_eye_z    =   0.01  ;
GLdouble lookat_centre_x =   0.01  ;     //  Initial losition looked at
GLdouble lookat_centre_y =   0.06  ;
GLdouble lookat_centre_z =   0.00  ;
#elif defined _Display_Textures
GLdouble lookat_eye_x    =   1.0 ;
GLdouble lookat_eye_y    =   0.6 ;
GLdouble lookat_eye_z    =   3.0 ;
GLdouble lookat_centre_x =   1.0  ;
GLdouble lookat_centre_y =   0.61 ;
GLdouble lookat_centre_z =   0.0  ;
#elif 0
//  Position at bridge
GLdouble lookat_eye_x    =   2.805  ;     //  Initial position of eye
GLdouble lookat_eye_y    =  12.708  ;
GLdouble lookat_eye_z    =   0.008  ;
GLdouble lookat_centre_x =   2.805  ;     //  Initial losition looked at
GLdouble lookat_centre_y =  12.714  ;
GLdouble lookat_centre_z =   0.008 ;
#elif 0
//  Position over bridge
GLdouble lookat_eye_x    =   2.843  ;     //  Initial position of eye
GLdouble lookat_eye_y    =  12.727  ;
GLdouble lookat_eye_z    =   0.0056 ;
GLdouble lookat_centre_x =   2.833  ;     //  Initial losition looked at
GLdouble lookat_centre_y =  12.737  ;
GLdouble lookat_centre_z =   0.0056 ;
#elif 1
//  Position near Bowenfels Level Crossing
GLdouble lookat_eye_x    =   3.010615 ;     //  Initial position of eye
GLdouble lookat_eye_y    =  12.680849 ;
GLdouble lookat_eye_z    =   0.003605 ;
GLdouble lookat_centre_x =   3.081628 ;     //  Initial losition looked at
GLdouble lookat_centre_y =  12.632812 ;
GLdouble lookat_centre_z =   0.003605 ;
#elif 1
//  Position at Bowenfels Station
GLdouble lookat_eye_x    =   3.203460  ;     //  Initial position of eye
GLdouble lookat_eye_y    =  12.494317  ;
GLdouble lookat_eye_z    =   0.005070  ;
GLdouble lookat_centre_x =   3.158124  ;     //  Initial losition looked at
GLdouble lookat_centre_y =  12.567084  ;
GLdouble lookat_centre_z =   0.005070  ;
#elif 1
//  Position at Lithgow turntable
GLdouble lookat_eye_x    =   4.740  ;     //  Initial position of eye
GLdouble lookat_eye_y    =  12.500  ;
GLdouble lookat_eye_z    =   0.021  ;
GLdouble lookat_centre_x =   4.740  ;     //  Initial losition looked at
GLdouble lookat_centre_y =  12.550  ;
GLdouble lookat_centre_z =   0.021  ;
#elif 1
//  Position at zig-zag
GLdouble lookat_eye_x    =   5.771  ;     //  Initial position of eye
GLdouble lookat_eye_y    =  12.904  ;
GLdouble lookat_eye_z    =   0.068  ;
GLdouble lookat_centre_x =   5.780  ;     //  Initial losition looked at
GLdouble lookat_centre_y =  12.901  ;
GLdouble lookat_centre_z =   0.068  ;
//  Position at Mount Victoria
#else
GLdouble lookat_eye_x    =   8.076494  ;
GLdouble lookat_eye_y    =   6.484393  ;
GLdouble lookat_eye_z    =   0.073429  ;
GLdouble lookat_centre_x =   8.069136  ;
GLdouble lookat_centre_y =   6.569811  ;
GLdouble lookat_centre_z =   0.073429  ;
#endif
GLdouble radian_to_north        ;  // Initialised by graphics_init
GLdouble angle_to_north         ;  //   to agree with lookat variables
GLdouble radian_to_up           ;  //   ditto
GLdouble angle_to_up            ;  //   ditto

GLdouble lookat_up_x     =  0.0  ;
GLdouble lookat_up_y     =  0.0  ;
GLdouble lookat_up_z     =  1.0  ;



/**  gluLookAt variables  */

float  eye_x    =   0.2 ;
float  eye_y    = -20.0 ;
float  eye_z    =   4.0 ;
float  lookat_x =   0.0 ;
float  lookat_y =   0.0 ;
float  lookat_z =  -0.3 ;

float  clip_near=   0.01;  // Near clipping point
float  clip_far =  10.0 ;  // Far clipping point
float  clip_x   =   0.0 ;  // Clipping plane x
float  clip_y   =   0.0 ;  // Clipping plane y
float  clip_z   =   0.0 ;  // Clipping plane z
float  clip_c   =  -1.0 ;  // Clipping plane constant
int  clip_i_min =  0    ;
int  clip_i_max =  0    ;
int  clip_j_min =  0    ;
int  clip_j_max =  0    ;
int  clip_k_min =  0    ;
int  clip_k_max =  0    ;
GLdouble  clip_0[4]= {0.0, 0.0, 0.0, 0.0} ; // Also initialised by graphics_init
                                            //   to agree with clipping plane.
float  lookat_t[3] ;
int    lookat_opt = 0 ;    //  Used in plotting lookat point
int    iclip_on   = 0 ;    //  Clipping plane off/on

//int    lookat_el =    45.0 ;
//int    lookat_az =   355.0 ;
int    lookat_el =    89.0 ;
int    lookat_az =     0.0 ;
float  lookat_dd =     7.0 ;

/** Variables for Orthographic projection                **/
/** These are the limits of the current orthographic box **/
int     ortho_set = 0 ;      // equals 1 when limits are valid
int     ortho_col = 0 ;      // Colour used for debug
double  ortho_left, ortho_right, ortho_bottom, ortho_top,
        ortho_near, ortho_far ;

/*
 *  Default texture to topography
 *  [Simple scheme for use until proper implimentation of land textures}
 */
char    land_texture_default[] = "BrownScrub" ;
TextureNode *land_texture = NULL              ;  // Non-zero if present

/*
 *  OpenGL Display List
 */
GLuint  tile_dlist_beg = 0 ;      // Start of Tile display lists
GLuint  tile_dlist_end = 0 ;      // End
GLsizei tile_dlist_num = 0 ;      // Number

/** Live variables passed to glut */

int   main_window ;
int   menu_r = 0;

// glut_sphere_option

GLfloat sphere_white[]       = { 0.7, 0.7, 0.7, 1.0 };
int   l_glut_sphere = 0 ;
int   wireframe = 0;
int   segments = 64;
/*
 *==============================================================================
 * Test Textures
 *==============================================================================
 */
#define checkImageWidth   64
#define checkImageHeight  64
#define subImageWidth     16
#define subImageHeight    16
static GLubyte checkImage[checkImageHeight][checkImageWidth][4] ;
static GLubyte subImage[subImageHeight][subImageWidth][4] ;

int    icount = 0;
int    ipp = 0 ;
time_t time_now, time_last = 0, time_inc ;
int    fps   = 0 ;              // Frames per second when set
int    l_fps = 0 ;              // When set print frames per second

/*
 *==============================================================================
 *   Lighting
 *==============================================================================
 */

GLfloat   light0_altde =  50.0 ;      //  Altitude of light0  (Zenith = 90)
GLfloat   light0_polar = 150.0 ;      //  Polar angle of light0 (North = 0)
GLfloat   light0_ambi  =   0.7 ;      //  Intensity of 'ambient' white light0
GLfloat   light0_diff  =   1.0 ;      //  Intensity of 'diffusive' white light0
GLfloat   light0_spec  =   0.2 ;      //  Intensity of 'specular' white light0
GLfloat   light0_shin  =   0.5 ;      //  Shininess



float scale = 1.0;
float view_rotate[16] = { 1,0,0,0,   0, 0.866025404,-0.5,0,
                          0,0.5, 0.866025404,0,   0,0,0,1 };


void  keyboard(unsigned char key, int x, int y);
void  display(void) ;
void  reinit_vars(void) ;
void  specialkey(int key, int ixm, int iym) ;
void  print_string_in_window(GLfloat rx, GLfloat ry, char *string);
void  print_string_in_window2(GLfloat rx, GLfloat ry, GLfloat rz, char *string);
void  glutPrintString(char *string, void *font) ;

void  myLookAt(void)           ;
void  reshape(int w, int h)    ;
void  myGlutIdle(void)         ;

void  graphics_init(void)      ;
int   display_dynamic_tracks() ;
int   display_extra_data(void) ;
void  clear_needed_flags(void) ;
void  mark_shapes(void)        ;
void  mark_textures(void)      ;
void  cull_textures(void)      ;
void  cull_display_lists(void) ;
void  cull_topography_display_lists(void) ;
void  load_topography_display_lists(void) ;
void  load_needed_textures()   ;
void  load_needed_display_lists() ;
int   use_tile(int ix, int iy) ;

int   make_tile_topog_display_lists(void) ;
int   make_tile_topog_display_list(TileListNode *tnode, GLuint klist) ;

int   make_dynamic_display_lists() ;
void  makeCheckImages(void) ;
int   initialise_eye_vectors() ;
int   check_in_scene(GLdouble x,GLdouble y,GLdouble z,GLdouble r,GLdouble d) ;

// Utilities

float zr_fclip(float x, float xmin, float xmax);
int   zr_setv4(GLfloat *v4, float x1, float x2, float x3, float x4) ;
int   zr_setp4(float *v4, float altde, float polar) ;



/*
 *  Tests
 */
int  test_sphere(float r) ;
int  ksphere ;
