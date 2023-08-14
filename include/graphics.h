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

//#include <GL/gl.h>
//#include <GL/glext.h>
//#include <GL/glu.h>
//#include <GL/glut.h>

#ifdef SDL2

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
//#include <SDL2/SDL_ttf.h>

#include <GL/gl.h>
#include <GL/glext.h>

typedef int32_t  i32 ;
typedef uint32_t u32 ;
typedef int32_t  b32 ;

u32 WindowFlags       ;
SDL_Window   *Window  ;
SDL_GLContext Context ;
SDL_Event    *Event   ;
int sdl2_double_buffer = 0 ;   // Set to 1 if double buffering

#ifdef USE_SDLTTF
TTF_Font     *ttf_font ;
TTF_Font     *ttf_font_f12,
             *ttf_font_f14,
             *ttf_font_f16,
             *ttf_font_f18 ;
#endif

b32 Running    = 1 ;
b32 FullScreen = 0 ;

/* Function Prototypes */
void PrintKeyInfo(SDL_KeyboardEvent *key ) ;
void PrintModifiers(Uint16 mod )           ;
void reshape2(int w, int h) ;
int  keyboard_sdl(SDL_Event *event )       ;

#endif

/*
 *  glError variables
 */
GLenum   gl_errorCode ;
GLubyte *gl_errString = (GLubyte *)"No OpenGL Error";


int     l_plot = 0  ;   // True if plotting on
int     tst_tile_x =  1448 ;       //  Initial test  1448
int     tst_tile_y = 10331 ;       //  Initial test 10331
int     n_vanodes_d    ;           //  Used to store number of va nodes displayed
int     id_shape = 1   ;           //  Used to debug display

int show_platforms_and_sidings = 0 ;  //  Turn on/off
int show_platforms_or_sidings  = 0 ;  //  Cycle both/platforms/sidings

GLdouble pi     = M_PI       ;
GLdouble radian = M_PI/180.0 ;
GLdouble degree = 180.0/M_PI ;
GLfloat  gl_offset1 = -1.0    ;
GLfloat  gl_offset2 = -2.0    ;
/*
 *  The 3D view is defined by the variables specifying the viewport
 *  and the "lookat_" variables which specify the position of the eye
 *  the position looked at.
 *
 *  The "lookat_" variables can be relative to a fixed point or a
 *  possibly moving wagon.  The fixes point is specified by the
 *  wagon x y and z coordinates and the relative displacement
 *  by the "relative_" variables.
 *
 *  The derived variables angle_to_north and angle_to_up
 *  are used by subroutines when changing the direction of view.
 *  These then update the offset_centre variables.
 *
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
//GLdouble viewport_far_m  = 10240.0 ; // Furthest point displayed
#ifdef sketch_tracks_and_roads
GLdouble viewport_far_m  = 20480.0 ; // Furthest point displayed
#else
GLdouble viewport_far_m  = 2048.0 ; // Furthest point displayed
#endif
double    tile_size  = 2048.0    ;  // Size of tile (m)
double    plot_scale = 2048.0    ;  // Size in m of one unit in plotting space

// Unit vectors in model space corresponding to:
GLdouble eye_x_x, eye_x_y, eye_x_z ;  //    Unit right vector   (x) in screen
GLdouble eye_y_x, eye_y_y, eye_y_z ;  //    Unit up vector      (y) in screen
GLdouble eye_z_x, eye_z_y, eye_z_z ;  //    Unit forward vector (z) in screen

GLdouble screen_hw_x               ;  // Screen half width at distance 1.0
GLdouble screen_hw_y               ;  // Screen half height at distance 1.0

int      tile_x0                ;  // Local x-origin (east) for plotting
int      tile_y0                ;  // Local y-origin (north) for plotting
double   tile_h0                ;  // Local height origin for plotting

int      tile_eye_x0            ;  // Tile containing viewpoint
int      tile_eye_y0            ;
int      last_eye_x0   = 0      ;  // Previous tile with viewpoint
int      last_eye_y0   = 0      ;
int      new_viewpoint = 0      ;  // Set to 1 when projection changed
GLdouble viewport_near          ;
GLdouble viewport_far           ;

#ifdef full_topog
  int tile_cull_r = 0           ;  // Plot all topography
#else
  int tile_cull_r = 2           ;  // Limit to 3x3 tiles around eye point.
#endif

/*
 *  Logic for to control displays
 */
int display_help_on     = 0 ;  // Help display
int display_switches_on = 0 ;  // Switch/point display
int display_train_operations_on = 0 ;  // Train operations screen


#if defined _Display_Wagons || defined _Display_Shapes
GLdouble offset_eye_x    =   0.01  ;      //  Initial position of eye
GLdouble offset_eye_y    =  -0.03  ;
GLdouble offset_eye_z    =   0.01  ;
GLdouble offset_center_x =   0.01  ;     //  Initial losition looked at
GLdouble offset_center_y =   0.06  ;
GLdouble offset_center_z =   0.00  ;
#elif defined _Display_Textures
#if 1
GLdouble offset_eye_x    =   1.0 ;
GLdouble offset_eye_y    =   0.6 ;
GLdouble offset_eye_z    =   3.0 ;
GLdouble offset_center_x =   1.0  ;
GLdouble offset_center_y =   0.61 ;
GLdouble offset_center_z =   0.0  ;
#else
GLdouble offset_eye_x    =   0.5  ;      //  Initial position of eye
GLdouble offset_eye_y    =   0.5  ;
GLdouble offset_eye_z    =   2.0  ;
GLdouble offset_center_x =   0.5  ;     //  Initial losition looked at
GLdouble offset_center_y =   0.5  ;
GLdouble offset_center_z =   0.0  ;
#endif
#elif defined ROUTE_USA1
//   Baltimore
GLdouble offset_eye_x    =   11.955162 ;      //  Initial position of eye
GLdouble offset_eye_y    =   23.442479 ;
GLdouble offset_eye_z    =    0.114801 ;
GLdouble offset_center_x =   11.929563 ;     //  Initial losition looked at
GLdouble offset_center_y =   23.438227 ;
GLdouble offset_center_z =    0.112526 ;
#elif defined ROUTE_USA2
# if 0
GLdouble offset_eye_x    =   96.969761 ;      //  Initial position of eye
GLdouble offset_eye_y    =   17.013034 ;
GLdouble offset_eye_z    =    0.104136 ;
GLdouble offset_center_x =   96.962818 ;     //  Initial losition looked at
GLdouble offset_center_y =   17.022474 ;
GLdouble offset_center_z =    0.104134 ;
# elif 0
GLdouble offset_eye_x    =    3.428108 ;      //  Initial position of eye
GLdouble offset_eye_y    =   12.033694 ;
GLdouble offset_eye_z    =    0.056888 ;
GLdouble offset_center_x =    3.416407 ;     //  Initial losition looked at
GLdouble offset_center_y =   12.034306 ;
GLdouble offset_center_z =    0.056708 ;
# else                                       //  Missing track
GLdouble offset_eye_x    =    2.772063 ;     //  Initial position of eye
GLdouble offset_eye_y    =   11.971261 ;
GLdouble offset_eye_z    =    0.072513 ;
GLdouble offset_center_x =    2.456401 ;     //  Initial losition looked at
GLdouble offset_center_y =   12.496611 ;
GLdouble offset_center_z =   -0.126627 ;
# endif
#elif defined ROUTE_EUROPE1
GLdouble offset_eye_x    =    1.170625 ;      //  Initial position of eye
GLdouble offset_eye_y    =   41.379704 ;
GLdouble offset_eye_z    =    0.031768 ;
GLdouble offset_center_x =    1.154148 ;     //  Initial losition looked at
GLdouble offset_center_y =   41.399151 ;
GLdouble offset_center_z =    0.026397 ;
#elif defined ROUTE_EUROPE2
GLdouble offset_eye_x    =    0.864958 ;      //  Initial position of eye
GLdouble offset_eye_y    =    1.213952 ;
GLdouble offset_eye_z    =    0.396184 ;
GLdouble offset_center_x =    0.940787 ;     //  Initial losition looked at
GLdouble offset_center_y =    1.252589 ;
GLdouble offset_center_z =    0.387239 ;
#elif defined ROUTE_JAPAN1
GLdouble offset_eye_x    =    6.687026 ;      //  Initial position of eye
GLdouble offset_eye_y    =    0.953594 ;
GLdouble offset_eye_z    =    0.050969 ;
GLdouble offset_center_x =    6.620887 ;     //  Initial losition looked at
GLdouble offset_center_y =    0.900036 ;
GLdouble offset_center_z =    0.042024 ;
#elif defined ROUTE_JAPAN2
GLdouble offset_eye_x    =   31.997668 ;      //  Initial position of eye
GLdouble offset_eye_y    =    0.911984 ;
GLdouble offset_eye_z    =    0.115911 ;
GLdouble offset_center_x =   31.927571 ;     //  Initial losition looked at
GLdouble offset_center_y =    0.961066 ;
GLdouble offset_center_z =    0.115911 ;
#elif defined ROUTE_TUTORIAL
GLdouble offset_eye_x    =    2.028417 ;      //  Initial position of eye
GLdouble offset_eye_y    =   11.507974 ;
GLdouble offset_eye_z    =    0.005371 ;
GLdouble offset_center_x =    1.979062 ;     //  Initial losition looked at
GLdouble offset_center_y =   10.094621 ;
GLdouble offset_center_z =    0.005371 ;
#elif defined ROUTE_NEW_FOREST
#if 1
GLdouble offset_eye_x    =    7.438929 ;      //  Initial position of eye
GLdouble offset_eye_y    =    5.197604 ;
GLdouble offset_eye_z    =    1.333684 ;
GLdouble offset_center_x =    7.477778 ;     //  Initial losition looked at
GLdouble offset_center_y =    5.121358 ;
GLdouble offset_center_z =    1.333684 ;
#else
GLdouble offset_eye_x    =    13.4 ;      //  Initial position of eye
GLdouble offset_eye_y    =    30.4 ;
GLdouble offset_eye_z    =    1.7 ;
GLdouble offset_center_x =    13.5 ;     //  Initial losition looked at
GLdouble offset_center_y =    30.5 ;
GLdouble offset_center_z =    1.3 ;
#endif

/*
 *   Zig-Zag Route
 */
#elif 0
//  Position at Origin
GLdouble offset_eye_x    =   0.000  ;     //  Initial position of eye
GLdouble offset_eye_y    =   0.000  ;
GLdouble offset_eye_z    =   0.000  ;
GLdouble offset_center_x =   1.000  ;     //  Initial losition looked at
GLdouble offset_center_y =   0.000  ;
GLdouble offset_center_z =   0.000 ;
#elif 0
//  Start of first dynamic track section
GLdouble offset_eye_x    =   2.6232  ;     //  Initial position of eye
GLdouble offset_eye_y    =  13.2096  ;
GLdouble offset_eye_z    =   0.0170  ;
GLdouble offset_center_x =   2.6213  ;     //  Initial losition looked at
GLdouble offset_center_y =  13.1986  ;
GLdouble offset_center_z =   0.0170 ;
#elif 0
//  Start of first dynamic track section
GLdouble offset_eye_x    =   2.6229  ;     //  Initial position of eye
GLdouble offset_eye_y    =  13.1886  ;
GLdouble offset_eye_z    =   0.0155  ;
GLdouble offset_center_x =   2.6225  ;     //  Initial losition looked at
GLdouble offset_center_y =  13.1775  ;
GLdouble offset_center_z =   0.0155 ;
#elif 0
//  End of track
GLdouble offset_eye_x    =   2.626  ;     //  Initial position of eye
GLdouble offset_eye_y    =  13.290  ;
GLdouble offset_eye_z    =   0.017  ;
GLdouble offset_center_x =   2.624  ;     //  Initial losition looked at
GLdouble offset_center_y =  13.279  ;
GLdouble offset_center_z =   0.017 ;
#elif 0
//  Position at bridge
GLdouble offset_eye_x    =   2.805  ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.708  ;
GLdouble offset_eye_z    =   0.008  ;
GLdouble offset_center_x =   2.805  ;     //  Initial losition looked at
GLdouble offset_center_y =  12.714  ;
GLdouble offset_center_z =   0.008 ;
#elif 0
//  Position over bridge
GLdouble offset_eye_x    =   2.843  ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.727  ;
GLdouble offset_eye_z    =   0.0056 ;
GLdouble offset_center_x =   2.833  ;     //  Initial losition looked at
GLdouble offset_center_y =  12.737  ;
GLdouble offset_center_z =   0.0056 ;
#elif 0
//  Position near Bowenfels Level Crossing
GLdouble offset_eye_x    =   3.010615 ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.680849 ;
GLdouble offset_eye_z    =   0.003605 ;
GLdouble offset_center_x =   3.081628 ;     //  Initial losition looked at
GLdouble offset_center_y =  12.632812 ;
GLdouble offset_center_z =   0.003605 ;
#elif 0
//  Second position near Bowenfels Level Crossing
GLdouble offset_eye_x    =   3.052724 ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.666317 ;
GLdouble offset_eye_z    =   0.002629 ;
GLdouble offset_center_x =   3.004872 ;     //  Initial losition looked at
GLdouble offset_center_y =  12.595374 ;
GLdouble offset_center_z =   0.002629 ;
#elif 0
//  Third position near Bowenfels Level Crossing
GLdouble offset_eye_x    =   3.023550 ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.680695 ;
GLdouble offset_eye_z    =   0.007024 ;
GLdouble offset_center_x =   3.094493 ;     //  Initial losition looked at
GLdouble offset_center_y =  12.632843 ;
GLdouble offset_center_z =   0.007024 ;
#elif 1
//  Position near start of Bowenfels Siding
GLdouble offset_eye_x    =   3.077779 ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.655553 ;
GLdouble offset_eye_z    =   0.005559 ;
GLdouble offset_center_x =   3.116628 ;     //  Initial losition looked at
GLdouble offset_center_y =  12.579307 ;
GLdouble offset_center_z =   0.005559 ;
#elif 0
//  Position at Bowenfels Station
GLdouble offset_eye_x    =   3.203460  ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.494317  ;
GLdouble offset_eye_z    =   0.005070  ;
GLdouble offset_center_x =   3.158124  ;     //  Initial losition looked at
GLdouble offset_center_y =  12.567084  ;
GLdouble offset_center_z =   0.005070  ;
#elif 0
//  Position at Crossing
GLdouble offset_eye_x    =   3.8802 ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.2528 ;
GLdouble offset_eye_z    =   0.0082 ;
GLdouble offset_center_x =   3.8910 ;     //  Initial losition looked at
GLdouble offset_center_y =  12.2558 ;
GLdouble offset_center_z =   0.0082 ;
#elif 0
//  Position at Lithgow
GLdouble offset_eye_x    =   4.404  ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.402  ;
GLdouble offset_eye_z    =   0.016  ;
GLdouble offset_center_x =   4.434  ;     //  Initial losition looked at
GLdouble offset_center_y =  12.411  ;
GLdouble offset_center_z =   0.016  ;
#elif 0
//  Position near points east of Lithgow
GLdouble offset_eye_x    =   4.8240  ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.5679  ;
GLdouble offset_eye_z    =   0.0180  ;
GLdouble offset_center_x =   4.8490  ;     //  Initial losition looked at
GLdouble offset_center_y =  12.5867  ;
GLdouble offset_center_z =   0.0180  ;
#elif 0
//  Position at Lithgow
GLdouble offset_eye_x    =   4.9649 ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.3976  ;
GLdouble offset_eye_z    =   0.0191 ;
GLdouble offset_center_x =   4.9669 ;     //  Initial losition looked at
GLdouble offset_center_y =  12.3976 ;
GLdouble offset_center_z =   0.0191 ;
#elif 0
//  Position at Lithgow turntable
GLdouble offset_eye_x    =   4.740  ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.500  ;
GLdouble offset_eye_z    =   0.021  ;
GLdouble offset_center_x =   4.740  ;     //  Initial losition looked at
GLdouble offset_center_y =  12.550  ;
GLdouble offset_center_z =   0.021  ;
#elif 0
//  Position at zig-zag
GLdouble offset_eye_x    =   5.771  ;     //  Initial position of eye
GLdouble offset_eye_y    =  12.904  ;
GLdouble offset_eye_z    =   0.068  ;
GLdouble offset_center_x =   5.780  ;     //  Initial losition looked at
GLdouble offset_center_y =  12.901  ;
GLdouble offset_center_z =   0.068  ;
//  Position at Mount Victoria
#elif 0
GLdouble offset_eye_x    =   8.076494  ;
GLdouble offset_eye_y    =   6.484393  ;
GLdouble offset_eye_z    =   0.073429  ;
GLdouble offset_center_x =   8.069136  ;
GLdouble offset_center_y =   6.569811  ;
GLdouble offset_center_z =   0.073429  ;
//  Turntable at Mt Victoria
#else
GLdouble offset_eye_x    =   8.099402  ;
GLdouble offset_eye_y    =   6.549618  ;
GLdouble offset_eye_z    =   0.077335  ;
GLdouble offset_center_x =   8.137168  ;
GLdouble offset_center_y =   6.623739  ;
GLdouble offset_center_z =   0.056594  ;
#endif

/*
 *   Position of eye in model coordinates
 */
GLdouble lookat_eye_x    ;
GLdouble lookat_eye_y    ;
GLdouble lookat_eye_z    ;
GLdouble lookat_center_x ;
GLdouble lookat_center_y ;
GLdouble lookat_center_z ;

GLdouble camera_origin_x  = 0.0   ;   //  Origin of current camera coordinate
GLdouble camera_origin_y  = 0.0   ;   //  relative to tile origin
GLdouble camera_origin_z  = 0.0   ;   //  i.e wagon position or zero
GLdouble lookat_eye_x             ;   //  Location of eye relative to tile origin
GLdouble lookat_eye_y             ;   //  (tile_x0, tile_y0, tile_z0)
GLdouble lookat_eye_z             ;

GLdouble radian_to_north          ;  //  Offset directions relative to north
GLdouble angle_to_north           ;  //  and up.  For wagon based cameras
GLdouble radian_to_up             ;  //  relative to wagon forward axis
GLdouble angle_to_up              ;  //

GLdouble lookat_up_x     =  0.0  ;
GLdouble lookat_up_y     =  0.0  ;
GLdouble lookat_up_z     =  1.0  ;


GLfloat clip_a[6][4]    ;  //  Clip planes in display units
int   initialise_clip_planes(GLfloat clipa[6][4]) ;
int   check_topog_in_scene2(GLfloat xa[2], GLfloat ya[2], GLfloat za[2]) ;
int   check_topographic_blocks() ;

/** Variables for Orthographic projection                **/
/** These are the limits of the current orthographic box **/
int     ortho_col = 0 ;      // Colour used for debug

/*
 *  Default texture to topography
 *  [Simple scheme for use until proper implimentation of land textures}
 */
#ifdef ROUTE_USA2
//char    land_texture_default[] = "bluediamond" ;
//char    land_texture_default[] = "US2GrassTrans" ;
char    land_texture_default[] = "US2ScrubTrans" ;
#elif defined ROUTE_USA1
char    *land_texture_default = "ACleanTrackBase" ;
#elif defined ROUTE_EUROPE1
char    *land_texture_default = "ACleanTrackBase" ;
#elif defined ROUTE_EUROPE2
char    *land_texture_default = "ACleanTrackBase" ;
#elif defined ROUTE_JAPAN1
char    *land_texture_default = "ACleanTrackBase" ;
#elif defined ROUTE_JAPAN2
char    *land_texture_default = "ACleanTrackBase" ;
#elif defined ROUTE_TUTORIAL
char    *land_texture_default = "ACleanTrackBase" ;
#elif defined ROUTE_NEW_FOREST
char    *land_texture_default = "HW_c1" ;
#else
char    *land_texture_default = "BrownScrub" ;
#endif
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
int    l_fps = 1 ;              // When set print frames per second

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



double scale = 1.0;
float view_rotate[16] = { 1,0,0,0,   0, 0.866025404,-0.5,0,
                          0,0.5, 0.866025404,0,   0,0,0,1 };


void  keyboard(unsigned char key, int x, int y)    ;
void  keyboard_up(unsigned char key, int x, int y) ;
void  mouse(int button, int state, int x, int y)   ;
void  motion(int x, int y) ;

void  display(void) ;
void  reinit_vars(void) ;
void  specialkey(int key, int ixm, int iym) ;
int   print_string_in_window(GLfloat wx, GLfloat wy, char *string);
int   print_string_in_window2(GLfloat rx, GLfloat ry, GLfloat rz, char *string);
int   print_string_in_window3(GLfloat wx, GLfloat wy, char *string, int font_size);
int   print_string_in_window_z(char *string, int font_size);
int   glutPrintString(char *string, void *font) ;

#if defined GLUT || defined USE_ZRGLUT
void  glutBitmapCharacter( void *fontID, int character) ;
void  glutBitmapString(void *fontID, const unsigned char *string) ;
int   glutBitmapLength(void *fontID, const unsigned char *string) ;
int   glutBitmapHeight(void *fontID) ;
#endif

void  myLookAt(void)           ;
void  reshape(int w, int h)    ;
void  glut_idle(void)         ;

void  graphics_init(void)      ;
int   display_dynamic_tracks() ;
void  clear_needed_flags(void) ;
void  mark_shapes(void)        ;
void  mark_textures(void)      ;
void  cull_textures(void)      ;
void  cull_display_lists(void) ;
void  graphics_reset(void)     ;
void  cull_topography_display_lists(void) ;
void  load_topography_display_lists(void) ;
void  load_needed_textures()   ;
void  load_needed_display_lists() ;
int   use_tile(int ix, int iy) ;

int   sketch_track_routes(int i) ;  // Sketch routes of tracks

int   display_extra_data(void) ;
int   display_track_info(TravellerNode *t) ;
int   display_help()           ;
int   display_switches(TrainNode *t)   ;

int   display_train_operations(TrainNode *train);
int   init_train_operations_window(int first_time);
void  move_train_operations_window(int x, int y) ;
void  change_train_operations_view(int x, int y) ;

int   make_tile_topog_display_lists(void) ;
int   make_tile_topog_display_list(TileListNode *tnode, GLuint klist) ;

int   make_dynamic_display_lists() ;
void  makeCheckImages(void) ;
int   initialise_eye_vectors() ;
int   check_in_scene(GLdouble x,GLdouble y,GLdouble z,GLdouble r) ;

// Utilities

float zr_fclip(float x, float xmin, float xmax);
int   zr_setv4(GLfloat *v4, float x1, float x2, float x3, float x4) ;
int   zr_setp4(float *v4, float altde, float polar) ;



/*
 *  Tests
 */
int  test_sphere(float r) ;
int  ksphere ;
