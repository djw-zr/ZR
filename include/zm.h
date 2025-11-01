/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  zm.h
 *   Copyright 2025  David J. Webb
 *
 *   ZM menue program for ZR
 *
 *   This file is part of ZR.
 *   Released under licence GPL-3.0-or-later
 *
 *   ZR is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   ZR is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *==============================================================================
 */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gdk/gdk.h>
#include <sys/mman.h>
#include <stdbool.h>

#if 0
#include "enum.h"        //  Needed before switch.h (both use DEFAULT)
#include "switch.h"
#include "typedef.h"
#include "zm_track.h"
#include "zm_btree.h"
#include "world.h"

/*
 *==============================================================================
 *  TextureNode  Structure
 *  This stores information on the texture image
 *
 *  NOTE:  The MSTS ACE file surface format often differs from the normal
 *         Microsoft convention
 *==============================================================================
 */

typedef struct texturenode {
  struct texturenode *next            ;  //  Pointer to the next node
  char               *name            ;  //  Name without '.ace'
  char               *filename        ;  //  Full filename
  int                basic            ;  //  = 1 if always needed
  int                needed           ;  //  True if needed for graphics
  int                loaded           ;  //  True if loaded into graphics card
  int                width            ;  //  Width of base texture
  int                height           ;  //  Height of base texture
  int                is_alpha         ;  //  True if file has 8-bit alpha channel
  int                is_mask          ;  //  True if file has 1-bit mask channel
  int                alpha_is_mask    ;  //  True if alpha used as mask
  int                n_textures       ;  //  Number of mipmaps (1 if base only)
  uint8_t            **texture        ;  //  Pointers to mipmaps in memory
  int                surface_format   ;  //  MSTS ACE file 'surface format'
  int                surface_format_orig  ;  //  Original
  int                gl_mem_format    ;  //  GL_RGB, GL_RGBA etc
  int                gl_mem_packing   ;  //  GL_UNSIGNED_BYTE etc
  int                gl_tex_ref_no    ;  //  Output from glGenTexture (openGL)
} TextureNode ;

/**
 *  MSblock structure.  MSTS files are structured as a series of
 *  binary or text blocks, each of which can contain a mixture
 *  of data and more blocks.  The MSblock structure is used to
 *  keep track of the current position and level in the
 *  overall structure.  For binary files it also keeps the
 *  position of the end byte of the current block at each level.
 */

typedef struct msblock {
  char *token_name  ;              /// Text Name    (Text Files)
  int  itoken       ;              /// Token Number (Binary Files)
  int  flags        ;
  int  l_label      ;
  int  length       ;
  char *label       ;
  int  byte_end     ;
} MSblock  ;

/*
 *  MSfile structure used to save data used while reading each
 *  MSTS format file whether text or binary
 *
 *  GNU/Linux like most other modern software uses the UTF-8
 *  representation of characters for text.
 *
 *  However MSTS traditionally used utf-16 two byte characters.
 *  In disk files these were stroed in little endian order,
 *  i.e. with the least significant byte first.
 *
 */

typedef struct msfile {              // MSTS files
  FILE *fp           ;                 // Standard file pointer
  char *filename     ;                 // File name
  int  utf16le       ;                 // File uses UTF-16
  int  utf8          ;                 // File uses UTF-8
  int  compress      ;                 // Compressed file (gzip?)
  int  text          ;                 // Text file
  int  binary        ;                 // Binary file
  int  world         ;                 // World file
  int  texture       ;                 // Texture file
  char *token_unused ;                 // Token from text file awaiting use
  MSblock level[20]  ;                 // Information on level
} MSfile ;

char    eof_mark[] = "******Z" ;   // Use to flag end-of-file in text files.


/*
 *  Structure PdbNode
 *
 *  This contains basic information on each Project (i.e. one of the
 *  MSTS standard routes or au_great_zig_zag).  At present the
 *  program is only set up for one project at a time with the node
 *  being the first node in a linked list with the origin at pdb0.
 *  This is done to allow the option for links to other projects
 *  in the same geographical region.
 */
typedef struct dlpointernode     DLPointerNode  ;
typedef struct pdbnode           PdbNode        ;

typedef struct envnode {                 //  Needs to be placed before trknode
  char  *weather ;
  char  *envfile ;
} Envnode ;
typedef struct envnode *Envptr ;

typedef struct pdbnode {
  struct pdbnode *next         ;   // Next node
  char   *filename             ;
  char   *filetype             ;
  char   *RouteID              ;
  char   *Name                 ;
  char   *Description          ;
  char   *Description2          ;
  char   *Graphic              ;
  char   *LoadingScreen        ;
  char   *FileName             ;

  char * DefaultSignalSMS      ;
  char * DefaultCrossingSMS    ;
  char * DefaultWaterTowerSMS  ;
  char * DefaultCoalTowerSMS   ;
  char * DefaultDieselTowerSMS ;

  int    Electrified           ;
  int    Mountains             ;
  int    OverheadWireHeight    ;
  int    PassengerRuleSet      ;
  int    FreightRuleSet        ;
  int    SignalSet             ;
  int    GantrySet             ;
  int    TrackGauge            ;
  int    Era                   ;
  int    MilepostUnitsMiles    ;
  int    MilepostUnitsKilometers ;
  int    MaxLineVoltage        ;
  int    RouteStart[4]         ;

  int    ORTSSwitchSMSNumber      ;
  int    ORTSCurveSMSNumber       ;
  int    ORTSCurveSwitchSMSNumber ;

  double SpeedLimit            ;
  double TerrainErrorScale     ;
  double TempRestrictedSpeed   ;
  double DerailScale           ;
  double TimetableTollerance   ;
  double GravityScale          ;

  double ORTSForestClearDistance ;

  struct envnode  Environment[12] ;

} PdbNode ;

/*
 *  Pointer node for use in doubly-linked lists.
 *  Can be used for lists of strings
 *  or pointers to anything.
 */

struct dlpointernode {
  struct dlpointernode *next    ;
  struct dlpointernode *prev    ;
  void                 *pointer ;
} ;

/*
 *   ConsistNode is used to specify standard groups of engines
 *   and/or wagons that can be used to initialise a new train at
 *   any point on the track.
 *
 *   When a train is constructed from this data, a new set of wagon nodes
 *   has to be generated.
 *
 *   Note:  There is also a Flip() option.  This structure does not allow for extra data.
 */


struct consistnode{
  struct consistnode *next        ;  // Next consist in linked list
  struct consistnode *prev        ;  // Next consist in linked list
  char               *name        ;  // Short unique name of train (or consist)
  char               *description ;  // Brief summary
  char               *file_name   ;  // Consist filename
  double             max_velocity ;  // Maximum velocity allowed m/s
  double             max_acceleration ;// Maximum acceleration or deacceleration m/s2
  double             durability   ;  // Durability of cargo
  int                n_wagons     ;  // Number of wagons
  DLPointerNode      *first       ;  // Start and end of linked list of pointers
  DLPointerNode      *last        ;  //   to raw_wagon_nodes in consist
} ;

#include "zm_functions.h"


/*
 *  ZR system definitions used by zr
 */

void init_token() ;
int   return_token(char *token, MSfile *msfile) ;
FILE *gopen(char* fname, char *ftype) ;
FILE *gcaseopen(char* fname, char *ftype) ;
FILE *fcaseopen(char const *path, char const *mode) ;
int  gclose(FILE *f) ;
int  n_open_files = 0 ;

int   read_int16(FILE *fp) ;
uint  read_uint16(FILE *fp) ;

PdbNode *project_db ;

char    *MSTSdir    = NULL ;       // Top directory for MSTS files (if any)
char    *ORdir      = NULL ;       // Top directory for OR routes (if any)
char    *ORdname    = NULL ;       // Shortened name of top directory
char    *ORroute    = NULL ;       // Name of route
char    *ORroutedir = NULL ;       // Full name of route directory
char    *ZRdotdir   = NULL ;       // Location of users .zr file
char    *ZRconfig   = NULL ;       // Location of user config file ($Home/.zr/

char    *ORrouteace = NULL ;

ree   *dir_master = NULL ;       // BTree with links to directories
BTree   *ORdirs     = NULL ;       //  BTree containing top directories
BTree   *ORroutes   = NULL ;       //  BTree containing routes
BTree   *ORconsists = NULL ;       //  BTree containing consists
BTree   *ORplatforms = NULL ;      //  BTree containing platforms
int      n_ordirs   = 0    ;       //  Number of ORdirs
int      n_orroutes = 0    ;       //  Number of ORroutes
int      n_consists = 0    ;       //  Number of consists
int      n_platforms = 0   ;       //  Number of platforms

ConsistNode  *consistlist_beg  = NULL ;  // Pointer to first node in list of consists
ConsistNode  *consistlist_end  = NULL ;

TrkDataBase track_db   ;    // Track database - with track sections and track items

int      trk_cell_west,       //  Index of western track cell row
         trk_cell_east,       //  Index of eastern track cell tow
         trk_cell_south,      //  Index of southern track cell
         trk_cell_north  ;    //  Index of northern track cell
float    trk_min_height,
         trk_max_height  ;
double   tile_h0                ;  // Local height origin for plotting
#else

#include "fcaseopen.h"
#include "btree.h"
typedef u_int uint ;
typedef float GLfloat ;
typedef double GLdouble ;
typedef u_int GLuint  ;
typedef uint8_t GLubyte ;
typedef u_int GLenum  ;
typedef u_int GLsizei ;

#include "typedef.h"
#include "msts.h"      //  Set up msts structures (if needed)
#include "enum.h"
#include "struct.h"
#include "wagon.h"
#include "world.h"
#include "track.h"
#include "display_info.h"
#include "train.h"
#include "camera.h"
#include "sigscr.h"
#include "signals.h"
#include "graphics.h"

#ifdef OPENAL
#include "sound.h"
#endif

#if defined FFMPEG || defined LIBPNG || defined PPM
#include "movie.h"
#endif

#ifdef SHADERS
#include "shaders.h"
#endif

#include "zm_functions.h"    //  Call this last

void init_token() ;
int   return_token(char *token, MSfile *msfile) ;
FILE *gopen(char* fname, char *ftype) ;
FILE *gcaseopen(char* fname, char *ftype) ;
FILE *fcaseopen(char const *path, char const *mode) ;
int  gclose(FILE *f) ;

int   read_int16(FILE *fp) ;
uint  read_uint16(FILE *fp) ;



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

char    *MSTSdir    = NULL ;       // Top directory for MSTS files (if any)
char    *ORdir      = NULL ;       // Top directory for OR routes (if any)
char    *ORdname    = NULL ;       // Shortened name of top directory
char    *ORroute    = NULL ;       // Name of route
char    *ORroutedir = NULL ;       // Full name of route directory
char    *ORrouteace = NULL ;
char    *ORconsist  = NULL ;       // Name of consist
char    *ORconsistdir = NULL ;     // Full name of consist directory
char    *ORplatform = NULL ;       // Name of platform

char    *ZRdotdir   = NULL ;       // Location of users .zr file
char    *ZRconfig   = NULL ;       // Location of user config file ($Home/.zr/config)
char    *ZRfonts           ;       // Location of user font file ($Home/.zr/fonts)
char    eof_mark[] = "******Z" ;   // Use to flag end-of-file in text files.
char    *nftw_file  = NULL ;       // Use to pass filenames to/from routine nftw

BTree   *ORdirs     = NULL ;       //  BTree containing top directories
BTree   *ORroutes   = NULL ;       //  BTree containing routes
BTree   *ORconsists = NULL ;       //  BTree containing consists
BTree   *ORplatforms = NULL ;      //  BTree containing platforms
int      n_ordirs   = 0    ;       //  Number of ORdirs
int      n_orroutes = 0    ;       //  Number of ORroutes
int      n_consists = 0    ;       //  Number of consists
int      n_platforms = 0   ;       //  Number of platforms

int     n_open_files = 0  ;  // used by gopen and gclose

struct timespec run_clock0  ;      //  Start time
struct timespec run_clock1  ;      //  Current time
struct timespec zr_clock_1[4] ;
struct timespec zr_clock_2[4] ;
double zr_clock_time[4][5]  ;      //  Arrays to use for timing
double start_seconds        ;      //  Start time (seconds) from zr.c
double run_seconds          ;      //  Run time (seconds) from start of display loop
double delta_seconds        ;      //  Interval since last entering display loop
double last_p2s  = 0.0      ;      //  Last time when 0.2s timer reset
double last_1s = 0.0        ;      //  Last time when 1s timer reset
double last_5s = 0.0        ;      //  Last time when 5s timer reset
double last_30s = 0.0       ;      //  Last time when 30s timer reset
int    l_time_p2s = 0       ;      //  0.2 s timer
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
int     i_zrt   = 0 ;           //  Toggle under keyboard control
int     l_ip    = 0 ;           //  Used to control some debug printing

// Top level pointers to structures

PdbNode     *project_db ;   // Project database
TrkDataBase track_db   ;    // Track database - with track sections and track items
TrkDataBase road_db    ;    // Road database  - with road sections and road items

// Top level pointers to tsection.dat items

TrackSection  *track_section_beg = NULL ;
TrackSection  *track_section_end = NULL ;

TrackShape    *track_shape_beg   = NULL ;
TrackShape    *track_shape_end   = NULL ;

BTree         *shape_master = NULL ;  // BTree containing all shapes used by world files
BTree         *sms_master   = NULL ;  // BTree containing all sounds (*.sms) used by world files
BTree         *wav_master   = NULL ;  // BTree listing all sound (*.wav) files used by sms files
BTree         *enum_master  = NULL ;  // BTree linking enum strings to values
BTree         *dir_master   = NULL ;  // BTree with links to directories

// Tiles

TileListNode  *tilelist_head = NULL ;  // Linked list of World Tile nodes
TileListNode  **tile_array = NULL   ;  // 2-D array of pointers to nodes : indexed (east, north)
int           tile_array_num = 0    ;  // Number of nodes in array
int           tile_array_nx  = 0    ;  // Number of nodes east-west
int           tile_array_ny  = 0    ;  // Number of nodes north-south
char          **tile_mask_array = NULL ;  // Mask used to prevent frest trees overlapping other objects

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

RawWagonNode *rawwagonlist_beg = NULL ;  // Pointer to first node in list of basic wagon structures
RawWagonNode *rawwagonlist_end = NULL ;  // Pointer to last node
ShapeNode    *wshapelist_beg   = NULL ;  // Pointer to first node in list of wagon shapes
ShapeNode    *wshapelist_end   = NULL ;  // Pointer to last node
SMS_Node     *wsmslist_beg     = NULL ;
SMS_Node     *wsmslist_end     = NULL ;
TextureNode  *wtexturelist_beg = NULL ;
TextureNode  *wtexturelist_end = NULL ;
TextureNode  *splash_node      = NULL ;  // Splash screen texture node
ConsistNode  *consistlist_beg  = NULL ;  // Pointer to first node in list of consists
ConsistNode  *consistlist_end  = NULL ;  // Pointer to last node

BTree        *rwag_master   = NULL ;  // Btree of all wagon nodes
BTree        *wshape_master = NULL ;  // Btree of all wagon shape nodes
BTree        *wtex_master   = NULL ;  // Btree of all wagon texture files

BTree        *sound_master  = NULL ;


//World

WorldNode  *worldlist_beg = NULL    ;  // Pointer to first node in list
WorldNode  *worldlist_end = NULL    ;  // Pointer to last node in list
int        world_token_offset = 300 ;

int        load_world_filenames()   ;
int        load_world(WorldNode *w) ;
int        load_world_soundfile(WorldNode *w) ;
int        list_wfile_item(WorldItem *wi) ;

//Sound

SMS_Node   *sms_list_beg  = NULL ;
SMS_Node   *sms_list_end  = NULL ;
SoundFile  *soundfile_beg = NULL ;
SoundFile  *soundfile_end = NULL ;


// Textures

TextureNode *texturelist_beg = NULL ;
TextureNode *texturelist_end = NULL ;
int         load_texture_filenames();
int         load_texture(TextureNode *t) ;
BTree       *texture_tree = NULL    ;

// Rail and road profiles

DynProfile  *zr_rail_profile = NULL   ;
DynProfile  *zr_road_profile = NULL   ;
int         make_default_track_profile() ;

//Signals
//  The raw database with descriptions of each of the signal types
//  The main database with information on each signal

RawSignalDB raw_signal_db0    ;   //  Database with descriptions of the different types of signal
RawSignalDB *raw_signal_db = &raw_signal_db0 ;
int         n_signals       = 0    ;
SignalDB    *signallist_beg = NULL ;   //  The signal database - each of the individual signals
SignalDB    *signallist_end = NULL ;
nodeType    *sTree = NULL     ;

int         n_turntables       = 0    ;
int         turntable_rotating = 0    ;
TurnTable   *turntablelist_beg = NULL ;
TurnTable   *turntablelist_end = NULL ;
TurnTable   *current_turntable = NULL ;  //
TrkSector   end_sector0               ;  //  Dummy end sector for turntables

int          n_transfers       = 0    ;
TransferNode *transferlist_beg = NULL ;
TransferNode *transferlist_end = NULL ;
TransferNode *current_transfer = NULL ;

int         n_levelcrossings   = 0    ;




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

char    *application_box_class    = "application_box";
int      application_box_padding  = 8;
char    *application_id           = "com.petermoulding.gtk_box_borders";
char    *application_name         = "Gtk box borders";
int      application_height       = 600;
int      application_width        = 600;
char    *border_class             = "border";
char    *border_solid             = "border_solid";

char *css =
    ".application_box { background-color: #00ffff; }\n"
    ".border          { border-color: #cccccc; border-style: solid; border-width: 2px; }\n"
    ".border_solid    { border-style: solid; }\n"
    ".border_margin   { margin: 2px; }\n"
    ".border_padded   { padding: 2px; }\n"
    ".border_both     { margin: 2px; padding: 2px; }\n"
    ".border_color    { background-color: navy; border-color: lime; color: yellow; font-style: italic; }\n"
    ".border_none     { border-style: none; }\n"
    ".border_hidden   { border-style: hidden; }\n"
    ".border_dashed   { border-style: dashed; }\n"
    ".border_dotted   { border-style: dotted; }\n"
    ".border_double   { border-style: double; }\n"
    ".border_groove   { border-style: groove; }\n"
    ".border_ridge    { border-style: ridge; }\n"
    ".border_inset    { border-style: inset; }\n"
    ".border_outset   { border-style: outset; }\n"
    ;

/*
 *   Gtk Widgits
 */

GtkWidget	*window;
GtkWidget	*fixed1;
GtkWidget	*button1;
GtkWidget	*button2;
GtkWidget	*button3;
GtkWidget	*button4;
GtkWidget	*label1;
GtkWidget	*label2;
GtkWidget	*label3;
GtkWidget	*label4;
GtkWidget	*label5;

GtkWidget	*radio1;
GtkWidget	*radio2;
GtkWidget	*radio3;
GtkWidget	*check1;
GtkWidget	*toggle1;
GtkWidget	*spin1;
GtkWidget	*switch1;
GtkWidget	*combo1;
GtkWidget	*entry1;
GtkWidget	*combo2;
GtkWidget	*entry2;
GtkWidget	*combo3;
GtkWidget	*entry3;
GtkWidget	*combo4;
GtkWidget	*entry4;
GtkWidget	*combo5;
GtkWidget	*entry5;
GtkWidget	*combo6;
GtkWidget	*entry6;
GtkWidget	*combo7;
GtkWidget	*entry7;
GtkWidget	*combo8;
GtkWidget	*entry8;
GtkWidget	*combo9;
GtkWidget	*entry9;
GtkWidget	*combo10;
GtkWidget	*entry10;
GtkWidget	*combo11;
GtkWidget	*entry11;
GtkWidget	*combo12;
GtkWidget	*entry12;

GtkWidget *frame5;
GtkWidget *frame6;
GtkWidget *ali5;
GtkWidget *fixed5;

GtkWidget *stack1;
GtkWidget *stkfxd1;
GtkWidget *stkfxd2;

GtkWidget *ScrollWindow ;
GtkWidget *ScrollPort ;
GtkWidget *TextView ;


GtkWidget	*color1;
GtkWidget	*file1;
GtkWidget	*font1;
GtkWidget	*volume1;
GtkWidget	*scroll1;
GtkWidget	*frame1;
GtkWidget	*frame2;
GtkWidget	*frame3;
GtkWidget	*frame4;
GtkWidget	*image1;
GtkWidget	*image2;
GtkWidget	*timer;
GtkWidget	*menu1;
GtkWidget	*new;
GtkWidget	*vc1;
GtkWidget	*prog1;
GtkWidget	*start1;
GtkWidget	*PingTime;
GtkWidget	*PingTimeDisplay;
GtkWidget	*ScrollWindow;
GtkWidget	*ViewPort;
GtkWidget	*TextView;
//GtkWidget	*saveText;
GtkTextBuffer	*TextBuffer;

GtkBuilder	*builder;

GtkListStore	*liststore1;
GtkAdjustment	*adjustment2;

/*
 *   My Gtk Controls
 *
 *   Unfortunately combo box triggers are called every time
 *   something is changed.  I notmally only want it called
 *   when the user makes a new choice.  This is done by
 *   turning the following flags off when the list of items
 *   or anything else is being changed.
 */
int combo1_active = 0 ;
int combo2_active = 0 ;
int combo3_active = 0 ;
int combo4_active = 0 ;
int combo5_active = 1 ;
int combo6_active = 1 ;
int combo7_active = 1 ;
int combo8_active = 1 ;
int combo9_active = 1 ;
int combo10_active = 1 ;
int combo11_active = 1 ;
int combo12_active = 1 ;

