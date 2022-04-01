/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  zr.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file specifying the variables and functions used by ZR
 *
 *==============================================================================
 */

#ifndef __ZR_H__
#define __ZR_H__

#ifdef  SDL2
 #ifdef zr_freetype
  #define USE_FREETYPE
 #else
  #define USE_ZRGLUT
 #endif
#else
 #define GLUT
 #ifdef zr_freetype
  #define USE_FREETYPE
 #endif
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <libgen.h>
#include <math.h>
#include <stdlib.h>
//#include <assert.h>
#include <time.h>
#include <errno.h>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#ifdef WINDOWS
  #include <io.h>           //  This may not be necessary
#else
  #include <sys/dir.h>
#endif
//#include <udunits2.h>

#define GL_GLEXT_PROTOTYPES  // Needed for glWindowPos2f()
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef GLUT
  #include <GL/freeglut.h>
#endif

#ifdef USE_ZRGLUT
  #include "zr_glut.h"
#endif

#ifdef SDL2
  #include <stdint.h>
  #include <assert.h>
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_opengl_glext.h>
#endif

#ifdef  USE_FREETYPE
  #include "freetype_zr.h"
#endif

//#ifdef USE_SDLTTF
//  #include <SDL2/SDL_ttf.h>
//#endif

#ifdef PORTAUDIO
  #include <stdint.h>
  #include "portaudio.h"
#endif

#include "btree.h"
typedef u_int uint ;

#include "msts.h"      //  Set up msts structures (if needed)
#include "enum.h"
#include "struct.h"
#include "track.h"
#include "wagon.h"
#include "world.h"
#include "display_info.h"
#include "train.h"
#include "camera.h"
#include "functions.h"    //  Call this last

#ifndef SDL2
  extern int versionsort(const struct dirent **, const struct dirent **);
#endif

/*
 *  Script to convert SWITCH statements into a series of if-elseif statements
 *  SWITCH and CASE cannot be used within a similar SWITCH block.  Within the
 *  same subroutine it is safe to use SWITCH2 and CASE2.
 *
 *  If one of the switch cases contains a separate set of switches, the second
 *   set should be contained within a separate '{ ... }' block.  Usually this
 *   will be a separate 'for', 'while' or 'do' block.
 *
 *  {SWITCHL(token)           <-  Note '{'
 *     CASE("Text_1")
 *       ...
 *       {                    // Block start
 *         Another set of SWITCHL, CASE, DEFAULT and END
 *       }                   // Block end
 *       break ;
 *     CASE("Text_2")
 *       ...
 *       break ;
 *     DEFAULT
 *       ...
 *   END}                    <-  Note '}'
 *
 */

#ifndef SWITCH_CASE_INIT
#define SWITCH_CASE_INIT
char *__switch_p__ ;
int  __switch_next__ ;
  #define SWITCH(X)  for ( __switch_p__ = X, __switch_next__=1; __switch_p__ ; __switch_p__=0, __switch_next__=1) { {
  #define CASE(X)         } if (!__switch_next__ || !(__switch_next__ = strcmp(__switch_p__, X))) {
  #define DEFAULT         } {
  #define END         } }
// Use SWITCH2 and CASE2 for inner loops.
char *__switch_p2__ ;
int  __switch_next2__ ;
  #define SWITCH2(Y)   for ( __switch_p2__ = Y, __switch_next2__=1; __switch_p2__ ; __switch_p2__=0, __switch_next2__=1) { {
  #define CASE2(Y)         } if (!__switch_next2__ || !(__switch_next2__ = strcmp(__switch_p2__, Y))) {
#endif

  #define SWITCHL(X) char *__switch_p__ ; int  __switch_next__ ; for ( __switch_p__ = X, __switch_next__=1; __switch_p__ ; __switch_p__=0, __switch_next__=1) { {

/*
 *  Special Variables
 */


char    *MSTSdir    ;        // Top directory for MSTS files (if any)
char    *ORdir      ;        // Top directory for OR routes (if any)
char    *ORroutedir ;        // Top directory of current route (if any)
char    *ZRconfig   ;        // Location of user config file ($Home/.zr/config)
char    *ZRfonts    ;        // Location of user font file ($Home/.zr/fonts)
char    eof_mark[] = "******Z" ;   // Use to flag end-of-file in text files.

int     n_open_files = 0  ; // used by gopen and gclose

struct timespec run_clock0  ;      //  Start time
struct timespec run_clock1  ;      //  Current time
struct timespec zr_clock_1[4] ;
struct timespec zr_clock_2[4] ;
double zr_clock_time[4][5]  ;      //  Arrays to use for timing
double start_seconds        ;      //  Start time (seconds) from zr.c
double run_seconds          ;      //  Run time (seconds) from start of display loop
double last_1s = 0.0        ;      //  Last time when 1s timer reset
double last_5s = 0.0        ;      //  Last time when 5s timer reset
double last_30s = 0.0       ;      //  Last time when 30s timer reset
int    l_time_1s = 0        ;      //  1 s timer
int    l_time_5s = 0        ;      //  5 s timer
int    l_time_30s = 0       ;      //  30 s timer
int    junction_error = 0   ;      //  Set when wagon enters junction from wrong branch

/*
 * Control/debug array
 */
char    name_control[50] ="";
char    name_control_old[50] ="";
int     n_control = 200 ;
int     i_control1[200],
        i_control2[200] ;
int     i_control1_old[200],
        i_control2_old[200] ;
int     l_disp0 = 1,            //  True for printing during display()
        l_disp1 = 0 ;           //  True if new position
int     i_zra   = 0 ;           //  Switch under keyboard control

// Top level pointers to structures

TrkDataBase track_db ;    // Track database - with track sections and track items
TrkDataBase road_db  ;    // Road database  - with road sections and road items

// Top level pointers to tsection.dat items

TrackSection  *track_section_beg = NULL ;
TrackSection  *track_section_end = NULL ;

TrackShape    *track_shape_beg   = NULL ;
TrackShape    *track_shape_end   = NULL ;

BTree         *shape_master = NULL ; // Btree containing all shapes used by world files

// Tiles

TileListNode  *tilelist_head = NULL ;
TileListNode  **tile_array          ;  //2-D array of pointers relating tile coordinates to nodes.
int           tile_array_num        ;  // Number of nodes in array

//Shapes

ShapeNode  *shapelist_beg = NULL    ;  // Pointer to first node in list of shapes
ShapeNode  *shapelist_end = NULL    ;  // Pointer to last node
int        load_shape_filenames()   ;
int        load_shape(ShapeNode *s) ;
int        current_block_level = -1 ;  // Used while reading shape files

// Trains, Wagons and Engines

TrainNode    *trainlist_beg  = NULL ;
TrainNode    *trainlist_end  = NULL ;
TrainNode    *player_train   = NULL ;

RawWagonNode *rawwagonlist_beg  = NULL ;  // Pointer to first node in list of basic wagon structures
RawWagonNode *rawwagonlist_end  = NULL ;  // Pointer to last node
ShapeNode    *wshapelist_beg = NULL    ;  // Pointer to first node in list of wagon shapes
ShapeNode    *wshapelist_end = NULL    ;  // Pointer to last node
TextureNode  *wtexturelist_beg = NULL ;
TextureNode  *wtexturelist_end = NULL ;

//World

WorldNode  *worldlist_beg = NULL    ;  // Pointer to first node in list of world items
WorldNode  *worldlist_end = NULL    ;  // Pointer to last node in list
int        load_world_filenames()   ;
int        load_world(WorldNode *w) ;
int        world_token_offset = 300 ;
int        list_wfile_item(WorldItem *wi) ;

// Textures
TextureNode *texturelist_beg = NULL ;
TextureNode *texturelist_end = NULL ;
int         load_texture_filenames();
int         load_texture(TextureNode *t) ;

// Rail and road profiles

DynProfile  *zr_rail_profile = NULL   ;
DynProfile  *zr_road_profile = NULL   ;
int         make_default_track_profile() ;


int         trk_cell_west,       //  Index of western track cell row
            trk_cell_east,       //  Index of eastern track cell tow
            trk_cell_south,      //  Index of southern track cell
            trk_cell_north  ;    //  Index of northern track cell
float       trk_min_height,
            trk_max_height  ;
int         tile_west,
            tile_east,
            tile_south,
            tile_north     ;

//  Project data base pointers

PdbNode         *pdb_beg = NULL     ;  //  Top level pointer to trk structures
PdbNode         *pdb_end = NULL     ;
SidingItem      *siding_item0       ;
SpeedPostItem   *speed_post_item0   ;
SignalItem      *signal_item0       ;
PlatformItem    *platform_item0     ;
LevelCrItem     *levelcr_item0      ;
CrossoverItem   *crossover_item0    ;
PickupItem      *pickup_item0       ;
SoundRegionItem *soundregion_item0  ;
HazzardItem     *hazzard_item0       ;

// Special matrices

float  m2g[16] = {0, 1, 0, 0,  0, 0, 1, 0,
                  1, 0, 0, 0,  0, 0, 0, 1 };
float  g2m[16] = {0, 0, 1, 0,  1, 0, 0, 0,
                  0, 1, 0, 0,  0, 0, 0, 1 };


#endif
