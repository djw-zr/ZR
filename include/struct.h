/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  structure.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file specifying the structures used by ZR
 *
 *==============================================================================
 */

typedef struct vector3           Vector3        ;
typedef struct dynprofile        DynProfile     ;
typedef struct trackdistacelevel TrackDistLevel ;
typedef struct tracksubobject    TrackSubObject ;
typedef struct railprofile       RailProfile    ;
typedef struct dlpointernode     DLPointerNode  ;
typedef struct wavfilenode       WavFileNode    ;

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

typedef struct envnode {                 //  Needs to be placed before trknode
  char  *weather ;
  char  *envfile ;
} Envnode ;
typedef struct envnode *Envptr ;

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

typedef struct pdbnode {
  struct pdbnode *next         ;   // Next node
  char   *filename             ;
  char   *filetype             ;
  char   *RouteID              ;
  char   *Name                 ;
  char   *Description          ;
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
 * *****************************************************************************
 *  Track Database
 *  Database input file is usually track_db defined in zr.h
 *
 *  Each track section node has one or more track vector nodes.  For end nodes
 *  and junction nodes, there is just one 'vector' giving the location of the
 *  correspondng world shape item (the moving parts of a junction or some track
 *  for end nodes).  For 'vector nodes' there is an array describing a section
 *  of track, each section being a straight line or a curve of fixed radius.
 *
 *  Notes based on those in Openrails TrackDatabaseFile.cs
 *
 * *****************************************************************************
 */
typedef struct trkvectornode
{
struct trkvectornode   *next ;
uint                    tsec_section_index   ;    //  Index of the track section in file 'tsection.dat'.
uint                    tsec_shape_index     ;    //  Index of the track shape in file 'tsection.dat'
                                                  //  Pointer to track section
                                                  //  Pointer to track shape
int                     wfname_east_x        ;    //  Tile east (MSTS x) location in the world file
int                     wfname_north_z       ;    //  TileZ north (MSTS Z) location in the world file
uint                    worldfileuid         ;    //  UID of this piece of track in the worldfile
                                                  //  Note world file may differ from tile
struct worlditem        *world_item          ;    //  Pointer to corresponding world item node
double                  ax                   ;    //  Rotation axis (x,y,z).  Unit vector.
double                  ay                   ;    //    Copied from world structure so may not correspond
double                  az                   ;    //    to tangent at start of vector,
double                  ang                  ;    //  Angle of rotation (degrees)

uint                    flag1                ;    //  Usually 0 may point to conecting pin in junction, sometimes 2
uint                    flag2                ;    //  Usually 1, set to 0 when track is flipped around, sometimes 2 or 3
                                                  //  In Zig-Zag route: flag2 = 1/0, flag1 is opposite
uint                    is_curved            ;    //  0 = straight,  1 = Curved with radius and angle as well as length
uint                    is_dynamic           ;    //  0 = normal track, 1 = dynamic track
char                    string[2]            ;    //  Usually 00
uint                    tile_east_x          ;    //  East  (MSTS X) value of the location-tile
uint                    tile_north_z         ;    //  North (MSTS Z)value of the location-tile
double                  east_x               ;    //  East (MSTS X) location within the tile where the node is located
double                  height_y             ;    //  Height (MSTS Y) location within the tile where the node is located
double                  north_z              ;    //  North (MSTS Z) location within the tile where the node is located
double                  a_east_x             ;    //  Angle around east describing the initial direction of the node
double                  a_height_y           ;    //  Angle around vertical axis describing the initial direction of the node
double                  a_north_z            ;    //  Angle around north axis describing the initial direction of the node
/*
 *  Unfortunately Track Section has a number of meanings in OpenRails and ZR.  The following variables are copied
 *  from the TrackSections database which describes each 'vector' either as a straight line of a given length
 *  or as a curve of fixed radius and fixed subtended angle.
 *  Maybe Track Shapes database might be better - except that 'shape' is used to refer to the 3-D shape
 */
double                  length               ;    //  Length of vector section
double                  radius               ;    //  Radius of curve
double                  angle                ;    //  Arc seen from centre of curvature  (radians)

/*
 *  The distance of the vector origin from the section origin is added to simplify the calculation of positions
 *  relative to the section origin.
 */
double                  distance0            ;    //  Distance of vector origin from section origin

/*
 *   Sub modes are added to better represent curved tracks.  The first sub-node corresponds to the current node
 *   (a_east_x, a_height_y, a_north_z).  The remainder represent extra nodes added between between the current
 *   node and the next.  Straight sections have no extra track points.  The final sub-node of the last vector node
 *   corresponds to the position of the pin in the connecting track.
 *
 *   The procedure allows the track to be plotted from the track vector nodes and the profile without
 *   an intermediate shape file.
 */
uint                    n_sub_nodes          ;    //  Number of sub nodes (>=1
Vector3                 *track_centre        ;    //  Vector giving position of track cente relative to Tile
Vector3                 *left_vector         ;    //  Unit vector to the left across track (including any camber)
Vector3                 *tangent_vector      ;    //  Unit vector along the track  (including any gradient)
} TrkVectorNode ;

/*
 *  TrkSector contains data on each track or road section.
 *  The nodes are either end nodes, junction nodes or vector nodes.  Vector nodes
 *  connect, at each ned to junction or end nodes.  End nodes connect to a single
 *  vector node.  Junction nodes to three vector nodes.
 *
 *  The vector node structures contain a series of connected 'vectors', wach of
 *  which describes a section of track that is either straight or has fixed
 *  curvature.
 *
 *  NOTE:  Usually the track section 'uid' is one more than the array
 *  index in the array 'trk_sections_array' in the track database (TrkDataBase).
 *
 *   See Openrails TrackdatabaseFile.cs :: uid, trjunctionnode, InPins etc.
 *
 *   At junctions: branch = 0 :  Route in to base of fork (i,e,'Y')
 *                        = 1 :  Left hand path
 *                        = 2 :  Right hand path
 */

typedef struct trksectnode TrkSector ;

struct trksectnode
{
  struct trksectnode     *next ;
  uint                   uid                 ;  //  Track section UID : Must be unique [= array index+1]
  enum trackdb           type_of_node        ;  //  [ NONE, VECTOR_SECTION, END_SECTION, JUNCTION ]]
  uint                   jn[3]               ;  //  Data for Junction Node.  jn[1] may refer to tsection.dat
  uint                   en                  ;  //  Data for End Node
  uint                   length_of_vector    ;  //  Number of vector nodes
  TrkVectorNode          *vector             ;  //  Pointer to array of track vector nodes
  uint                   trk_item_number     ;  //  Number of track items
  uint                   *trk_item_list      ;  //  Pointer to ordered array of track item UIDs
  uint                   type_of_pin[2]      ;  //  Number of: [0] InPins. [1] OutPins
  uint                   pin_to_section[3]   ;  //  Index of connected (pinned) Track Section.
  uint                   pin_info[3]         ;  //  Index of pin in connected section connected to this section
  TrkSector              *trk_sector[3]      ;  //  Pointer to connected track section
  uint                   straight            ;  //  Index (1 or 2) of straight branch in junction
  uint                   branch              ;  //  Index (1 or 2) of current switched branch
  uint                   mainline            ;  //  Junction branch (1 or 2) that was Pin[1] in track database
  DynProfile             *profile            ;  //  Profile used for this track
  uint                   n_dist_levels       ;  //  Number of distance levels
  float                  distance[3]         ;  //  Distances (Copied from profile)
  int                    opengl_display_list[3] ;  //  Corresponding display list
  enum LODMethod         lod_method          ;  //  Scheme for combining LOD data (Copied from profile)
/*
 *  The length of the section is added to speed up searches along the track
 */
  double                 length              ;  //  Length of section.  Non-zero for VECTOR_SCTION nodes.
} ;

/*
 *  NOTE:  The 'uid' is a UID.  Usually the uid is the same as the array
 *  index in the array 'trk_items_array' in the track database (TrkDataBase).

 *  See TrackDatabaseFile.cs Line 717 and following
 */

typedef struct trkitemnode
{
  uint                   uid       ;  //  Track item UID : Must be unique
  enum trackdb           type_of_node        ;
/*
 *  Additional data to simplify searching
 */
  uint                   track_section       ;  //  Array index of section containing item. Not uid.
  uint                   trk_item_index      ;  //  Index in containing track_section ordered list.
  double                 sect_distance       ;  //  Distance of item from origin of track section
/*
 *  Position data
 */
  uint                   tritemrdata         ;
  uint                   tritempdata         ;
  uint                   tritemsdata         ;
  uint                   tritemsrdata        ;

  uint                   tile_east_x         ;  //  East  (MSTS X) value of the location-tile
  uint                   tile_north_z        ;  //  North (MSTS Z)value of the location-tile
  uint                   p_tile_east_x       ;  //  East  (MSTS X) value of the location-tile
  uint                   p_tile_north_z      ;  //  North (MSTS Z)value of the location-tile
  double                 east_x              ;  //  east (MSTS X) location within the tile where the node is located
  double                 height_y            ;  //  height (MSTS Y) location within the tile where the node is located
  double                 north_z             ;  //  north (MSTS Z) location within the tile where the node is located
  double                 p_east_x            ;  //
  double                 p_north_z           ;  //
/*
 *  Other track item data
 */
  double                 s_data1             ;
  char                  *s_data2             ;

  uint                   sr_data1            ;  //  Sound region data
  uint                   sr_data2            ;
  double                 sr_data3            ;
// Siding data
  char                  *siding_data1        ;  // "00000000" or "ffff0000""
  uint                   siding_data2        ;  //  Index of 'other' end
  char                  *siding_name         ;  //  "Sheep Yards""

  char                  *signal_data1        ;  // Flags1  00000001 if junction link set
  uint                   signal_direction    ;  // Direction 0/1 relative to track vectors
                                                // 0 = Signal points to lower track vector indices
  double                 signal_data3        ;  // Float ??  (Height??)
  char                  *signal_type_name    ;  // Signal_type is possibly a reserved word in C
  SignalDB              *signal              ;  // Summary node used for signals
//
  uint                   signal_num_dirs     ;  //  Number of junction links?
//  4 integers per path
//    1 = junction link
//    2 = switch position is important (or link is a junction - not a vector node)
//    3 = index of path
//        See Signals.cs. AddHead function. Line ~10202
  uint                   signal_dirs[4][4]   ;

  uint                   speedpost_flags     ;  //  Speedpost data
  int                    is_warning          ;
  int                    is_limit            ;
  int                    is_milepost         ;
  int                    is_resume           ;
  int                    is_passanger        ;
  int                    is_freight          ;
  int                    is_mph              ;
  int                    show_number         ;
  int                    show_dot            ;
  double                 speed_ind           ;  //
  int                    display_number      ;  //
  double                 angle               ;

  uint                   crossover_data1     ;  //  Crossover data
  uint                   crossover_data2     ;  //

  uint                   pickup_data1        ;  //  Pickup data
  char                  *pickup_data2        ;  //
                                                //  End 0         End 1
  char                  *platform_data1      ;  // "00000000" or "ffff0000"
  uint                   platform_data2      ;  // Index of 'other' platform item
  char                  *platform_name       ;  // i.e. "Bowenfels - No 1"
  char                  *station             ;  // i.e. "Bowenfels"
  uint                   platform_wait_time  ;
  uint                   platform_passengers ;

} TrkItem ;

typedef struct trkdatabase
{
  uint                  serial_number ;
  uint                  trk_sections_array_size ;
  uint                  trk_items_array_size    ;
  TrkSector             *trk_sections_array     ;
  TrkItem               *trk_items_array        ;
} TrkDataBase ;

/*
 * *****************************************************************************
 *  Road Database - shares structures with Track Database
 *  Base is usually road_db defined in zr.h
 * *****************************************************************************
 */
#if 0
typedef struct roadsectionnode
{
  struct roadsectionnode *next ;
  uint                   uid       ;  //  This is used to link sections so must be unique
  enum trackdb           type_of_node        ;  //  [ NoType, TrVectorNode, TrEnd Node]
  uint                   en                  ;  //  Data for End Node
  uint                   length_of_vector    ;  //  Number of vector nodes
  TrkVectorNode          *vector             ;  //  Pointer to array of vector nodes (Shared with Track)
  uint                   road_item_number    ;  //  Number of road items
  uint                   *road_item_vec      ;  //  Index of road item
  uint                   type_of_pin[3]      ;
  uint                   pin_to_section[3]   ;  //   Section to pin
  uint                   pin_info[3]         ;  //   Extra pin data (??)
} RoadSectionNode ;

typedef struct roaditemnode
{
  uint                   uid       ;
  enum trackdb           type_of_node        ;
  uint                   tile_east_x         ;  //  East  (MSTS X) value of the location-tile
  uint                   tile_north_z        ;  //  North (MSTS Z)value of the location-tile
  uint                   p_tile_east_x       ;  //  East  (MSTS X) value of the location-tile
  uint                   p_tile_north_z      ;  //  North (MSTS Z)value of the location-tile
  double                 east_x              ;  //  east (MSTS X) location within the tile where the node is located
  double                 height_y            ;  //  height (MSTS Y) location within the tile where the node is located
  double                 north_z             ;  //  north (MSTS Z) location within the tile where the node is located
  double                 p_east_x            ;  //
  double                 p_north_z           ;  //
  double                 s_data1             ;
  char                  *s_data2             ;
} RoadItemNode ;

typedef struct roaddatabase
{
  uint                  serial_number ;
  uint                  road_sections_array_size ;
  uint                  road_items_array_size   ;
  RoadSectionNode       *road_sections_array     ;
  RoadItemNode          *road_items_array       ;
} RoadDataBase ;
#endif
/*
 * *****************************************************************************
 *  Track Items  ::  Storage
 * *****************************************************************************
 */

typedef struct tritemsdata {
  double    d1 ;
  long      i1 ;
} TrItemSData ;

typedef struct tritemsrdata {
  long      i1[2] ;
  double    d1 ;
} TrItemSRData ;

typedef struct tritempdata {
  double    d1[2] ;
  long      i1[2] ;
} TrItemPData ;

typedef struct tritemrdata {
  double    d1[3] ;
  long      i1[2] ;
} TrItemRData ;

/*
 * *****************************************************************************
 *  Track Items  ::  Primary structures
 * *****************************************************************************
 */
typedef struct trsignalrdir {
  struct trsignalrdir *next ;
  double        ddata[4] ;
  long          idata[2] ;
} TrSignalRDir ;

typedef struct sidingitem {
  struct sidingitem *next ;
  long         itemid ;
  TrItemSData  sdata ;
  TrItemRData  rdata ;
  long         idata[2] ;
  char         *name ;
} SidingItem ;

typedef struct speedpostitem {
  struct speedpostitem *next ;
  long         itemid ;
  TrItemSData  sdata ;
  TrItemPData  pdata ;
  TrItemRData  rdata ;
  long         idata[2] ;
  double       ddata ;
} SpeedPostItem ;

typedef struct  signalitem{
  struct signalitem *next ;
  long         itemid ;
  TrItemSData  sdata ;
  TrItemRData  rdata ;
  long         idata[2] ;
  double       ddata ;
  char         *sig_name ;
  TrSignalRDir *sig_dirs ;
} SignalItem ;

typedef struct platformitem {
  struct platformitem *next ;
  long         itemid ;          //  TrItemId
  TrItemSData  sdata ;           //
  TrItemRData  rdata ;           //
  long         idata1 ;           // PlatformTrItemData 1
  long         idata2 ;           // PlatformTrItemData 2
  char         *name     ;        // PlatformName
  char         *station  ;        // Station
  int          min_wait_time ;    // PlatformMinWaitingTime
  int          n_pass_waiting ;   // PlatformNumPassengersWaiting
} PlatformItem ;

typedef struct levelcritem {
  struct levelcritem *next ;
  long         itemid ;          //  TrItemId
  TrItemSData  sdata ;           //
  TrItemPData  pdata ;           //
  TrItemRData  rdata ;           //
} LevelCrItem ;

typedef struct crossoveritem {
  struct crossoveritem *next ;
  long         itemid ;          //  TrItemId
  TrItemSData  sdata ;           //
  TrItemRData  rdata ;           //
  long         idata1 ;          // CrossoverTrItemData 1
  long         idata2 ;          // CrossoverTrItemData 2
} CrossoverItem ;

typedef struct pickupitem {
  struct pickupitem *next ;
  long         itemid ;          //  TrItemId
  TrItemSData  sdata ;           //
  TrItemPData  pdata ;           //
  TrItemRData  rdata ;           //
  long         idata1 ;          // PickupTrItemData 1
  long         idata2 ;          // PickupTrItemData 2
} PickupItem ;

typedef struct soundregionitem {
  struct soundregionitem *next ;
  long         itemid ;           //  TrItemId
  TrItemSData  sdata  ;           //
  TrItemPData  pdata  ;           //
  TrItemRData  rdata  ;           //
  TrItemSRData srdata ;           //
} SoundRegionItem ;

typedef struct hazzarditem {
  struct hazzarditem *next ;
  long         itemid ;           //  TrItemId
  TrItemSData  sdata  ;           //
  TrItemPData  pdata  ;           //
  TrItemRData  rdata  ;           //
} HazzardItem ;

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
  GLubyte            **texture        ;  //  Pointers to mipmaps in memory
  int                surface_format   ;  //  MSTS ACE file 'surface format'
  int                surface_format_orig  ;  //  Original
  GLenum             gl_mem_format    ;  //  GL_RGB, GL_RGBA etc
  GLenum             gl_mem_packing   ;  //  GL_UNSIGNED_BYTE etc
  GLuint             gl_tex_ref_no    ;  //  Output from glGenTexture (openGL)
} TextureNode ;

/*
 *==============================================================================
 *  Tile Structures  (Data relating to individual world tiles)
 *  1.  data from T files (*.t)
 *==============================================================================
 */

typedef struct texslotnode {
  char               *filename        ;
  int                A                ;
  int                B                ;
} TexSlotNode ;

//ter_uvc_node_arr = (TerUvcNode *)malloc(ter_uvc_num*sizeof(teruvcnode)) ;

typedef struct texuvcalcnode {
  int                A                ;
  int                B                ;
  int                C                ;
  float              D                ;
} TexUVCalcNode ;


typedef struct tileshader{
  char              *name                          ;
  int               number_of_texslots             ;
  int               number_of_uvcalcs              ;
  TexSlotNode       *tex_slot                      ;  // Array
  TexUVCalcNode     *uvcalc                        ;  // Array
} TileShader  ;

typedef struct tilepatch{
  uint              flags                          ;
  float             centre_x                       ;
  float             average_y                      ;
  float             centre_z                       ;
  float             factor_y                       ;
  float             range_y                        ;
  float             radius_m                       ;
  int               index                          ;
  float             X                              ;
  float             Y                              ;
  float             W                              ;
  float             B                              ;
  float             C                              ;
  float             H                              ;
  float             error_bias                     ;
} TilePatch  ;

typedef struct tilepatchset{
  int               terrain_patchset_distance      ;
  int               terrain_patchset_npatches      ;
  int               number_of_patches              ;
  TilePatch         *tile_patch                    ; //  Array
} TilePatchSet  ;

typedef struct vanode{               //  Vertex array node
  float            xa[2]          ;  //  Min and max x coordinate
  float            ya[2]          ;  //  Min and max y
  float            za[2]          ;  //  Min and max z
  int              ncx            ;  //  Cells in x direction
  int              ncy            ;  //  Cells in y direction
  int              in_use         ;  //  Flag
//  Vector array pointers
  GLfloat             *va_vertex  ;  //  Vertex array
#ifdef normal_byte
  GLbyte              *va_normal  ;  //  Normal array
#else
  GLfloat             *va_normal  ;
#endif
#ifdef texture_short
  GLshort             *va_texture ;  // Texture array
#else
  GLfloat             *va_texture ;
#endif
  GLuint              nva_index   ;  //  Number of indices
  GLuint              *va_index   ;  //  Index array
} VANode  ;

typedef struct terraindata{
  float             terrain_errthreshold_scale     ;
  float             terrain_water_height_offset_SW ;  // In meters
  float             terrain_water_height_offset_SE ;
  float             terrain_water_height_offset_NE ;
  float             terrain_water_height_offset_NW ;
  float             terrain_alwaysselect_maxdist   ;
  int               terrain_nsamples               ;  // Points in each direction
  float             terrain_sample_rotation        ;
  float             terrain_sample_floor           ;  // Base level of tile (m) (Tiles.cs line 321)
  float             terrain_sample_scale           ;  // Metres per number in Y file
  float             terrain_sample_size            ;
  char              *ybuffer                       ;
  char              *ebuffer                       ;
  char              *nbuffer                       ;
  int               asbuffer                       ;
  int               fbuffer                        ;
  int               number_of_shaders              ;
  int               number_of_patchsets            ;
  TileShader        *tile_shader                   ;  // Array
  TilePatchSet      *tile_patchset                 ;  // Array
  unsigned short    *elevations                    ;  // Array of heights (Y file)
#ifndef no_vertex_arrays
//  Vertex array pointers
  GLfloat             *va_vertex                   ;
 #ifdef normal_byte
  GLbyte              *va_normal                   ;
 #else
  GLfloat             *va_normal                   ;
 #endif
 #ifdef texture_short
  GLshort             *va_texture                  ;
 #else
  GLfloat             *va_texture                  ;
 #endif
  GLuint              nva_index1                   ;
  GLuint              *va_index1                   ;
  GLuint              nva_index2                   ;
  GLuint              *va_index2                   ;
  int                 nbx                          ;  // Nodes in x direction
  int                 nby                          ;  // Nodes in y direction
  VANode              *va_node                     ;  // Vertex array nodes
#endif
} TerrainData ;

/*
 * *****************************************************************************
 *  Tiles : Top Level
 * *****************************************************************************
 */

typedef struct tilelistnode {
  struct  tilelistnode *next ;
  int       tilex            ;
  int       tiley            ;
  int       zoom             ;  // Size of tile
  char      *name            ;  // Name of '*.t' file
  char      *t_file          ;  // Full name of '*.t' file
  int       t_found          ;
  int       loaded           ;  // Non-zero when Data in main memory
  int       needed           ;
  int       gl_display_list  ;  // Non-zero when display list loaded
  TerrainData terrain_data   ;
} TileListNode  ;

/*
 * *****************************************************************************
 *  Shape Database
 * *****************************************************************************
 */

struct vector3 {
  float              X ;
  float              Y ;
  float              Z ;
} ;                                     //  typedef Vector3 defined near top of file

typedef struct uvvector2 {
  float              U ;
  float              V ;
} UVVector2 ;

typedef struct rgba {
  float              A ;
  float              R ;
  float              G ;
  float              B ;
} RGBA ;

typedef struct matrix4x3 {
  char            *name ;
  enum MatrixType  type ;
  enum MatrixAnim  anim ;
  float              AX ;
  float              AY ;
  float              AZ ;
  float              BX ;
  float              BY ;
  float              BZ ;
  float              CX ;
  float              CY ;
  float              CZ ;
  float              DX ;
  float              DY ;
  float              DZ ;
} Matrix4x3 ;

/*
 *   Sphere enclosing a sub-object.  This is used to quickly determine if
 *   the sub-object is within view.
 *
 *   Usually the number of shapevolumes is one more than the number of sub-objects
 *   But sometimes there is only one of each.
 */

typedef struct shapevolume {
  Vector3            vec    ;  //  Vector from origin of the shape to centre of sphere.
  float              radius ;  //  Radius of sphere
} ShapeVolume ;

/*
 * Low level shape texture specification.
 * It contains the index of one of the shape's texture
 *   plus flags specifying how the texture is used.
 *
 *   Maybe this should be called TextureStyle as it contains both
 *   the texture and the parameters needed to display it.
 */

typedef struct texlevellow {
  int           iImage        ;    //  Index of texture in shapes vector of textures
  int           FilterMode    ;    // ??
  float         MipMapLODBias ;    // ??
  uint          BorderColor   ;    //  Often 0xff000000
}TexLevelLow ;

typedef struct lightmaterial {
  int           flags        ;
  int           DiffColIdx    ;
  int           AmbColIdx    ;
  int           SpecColIdx    ;
  int           EmissiveColIdx    ;
  float         SpecPower ;
} LightMaterial ;

typedef struct uvops {
  uint          type         ;
  uint          tex_addr_mode     ;
  uint          src_uv_idx     ;
  float         unknown_param_3 ;
  float         unknown_param_4 ;
} UVOps ;

typedef struct lightmodelcfg {
  uint          flags        ;
  uint          n_uv_ops     ;
  UVOps         *uv_ops      ;
} LightModelCfg ;

typedef struct vtxstate {
  uint          flags         ;
  uint          imatrix       ;
  uint          light_mat_idx ;
  uint          light_cfg_idx ;
  uint          light_flags   ;
  uint          imatrix2      ;
} VTXState ;



typedef struct primstate {
  char          *name         ;
  uint          flags         ;
  int           ishader       ;
  uint          n_tex_idxs    ;  //  Number of textures required (usually 1)
  uint          *tex_idx      ;  //  Index to texture pointers in shapenode->image_tex
  float         zbias         ;
  uint          ivtx_state    ;  //  Index of vertex state in ShapeNode->VTXState structure
  int           alpha_test_mode ;
  int           light_cfgidx  ;
  int           zbuf_mode     ;  //  Usually 1 (presumably meaning 'Use z-buffer')
} PrimState ;


typedef struct geom_node {
  uint               tx_light_cmds     ;
  uint               nodex_tx_light_cmds ;
  uint               tri_lists         ;
  uint               line_lists        ;
  uint               pt_lists          ;
  uint               num_prims         ;
  uint               num_flat_sections ;
  uint               num_prim_idxs     ;
}  GeomNode ;

typedef struct vertex {                   //  Data for each vertex used at this distance/sub-object
  uint               flags             ;
  uint               ipoint            ;  //  Index of vertex in shapenode->vertex list
  uint               inormal           ;  //  Index of normal in shapenode->normal list
  uint               color1            ;  //  Usually solid white (possible backup if texture is missing)
  uint               color2            ;  //  Usually solid black (possible ??)
  uint               n_vertex_uvs      ;  //  Usually 1 (Possibly more if textures merged - see primstate)
  uint               *vertex_uv        ;  //  Indexes of texture coordinates in shapenode->uvpoint list
}  Vertex ;

typedef struct vertexset {
  uint               vtx_state_idx     ;
  uint               start_vtx_idx     ;
  int                vtx_count         ;
}  VertexSet ;


typedef struct vertexidx {      //  Triangles for plotting
  uint               a     ;    //  Index of 1st vertex in list : sub_object->vertex
  uint               b     ;    //  Index of 2nd vertex in list : sub_object->vertex
  uint               c     ;    //  Index of 3rd vertex in list : sub_object->vertex
}  VertexIdx ;

typedef struct trilist {        //  Three arrays of triangles to plot with normals and flags.
  uint               prim_state_trilist ;   //  Index of Primative State (includes index of Texture)
  uint               n_vertex_idxs      ;   //  Number of triangles
  VertexIdx         *vertex_idx         ;   //  Sets of triangle vertices to plot
  uint               n_normal_idxs      ;   //  Number - should equal number of triangles
  uint               *normal_idx        ;   //  Corresponding normals in list shapenode->normal
  uint               n_flags            ;   //  Number - should equal number of triangles
  uint              *flag               ;   //  Corresponding flags
}  TriList ;

typedef struct animkey {
  uint               type             ;
  uint               frame            ;
  float              X                ;
  float              Y                ;
  float              Z                ;
  float              W                ;
  float              tension          ;
  float              continuity       ;
  float              bias             ;
  float              in               ;
  float              out              ;
}  AnimKey ;


typedef struct animcontroller {
  uint              type              ;
  uint              n_anim_keys       ;
  AnimKey           *anim_key          ;
} AnimController ;


typedef struct animnode {
  char              *name             ;
  uint              n_controllers     ;
  AnimController    *controller       ;
} AnimNode ;

typedef struct animation {
  uint               frame_count      ;
  uint               frame_rate       ;
  uint               n_anim_nodes     ;
  AnimNode           *anim_node       ;
} Animation ;


typedef struct subobject {
  uint               flags             ;
  uint               sort_vector_idx   ;
  uint               vol_idx           ;  // Possibly the shapevolume to use but usually = -1
                                          // Anyway the shapevolumes are also usually roughly equal
                                          // and the first (so far) is always the largest.
  uint               src_vtx_fmt_flags ;
  uint               dst_vtx_fmt_flags ;
  uint               face_normals      ;
  uint               tx_light_cmds     ;
  uint               nodex_tri_list_idxs ;
  uint               tri_list_idxs     ;
  uint               linelist_idxs     ;
  uint               nodex_tri_list_idxs2 ;
  uint               tri_lists         ;
  uint               line_lists        ;
  uint               pt_lists          ;
  uint               nodex_tri_lists   ;
  uint               n_geom_nodes      ;
  GeomNode           *geom_node        ;
  uint               n_node_maps       ;
  uint               *node_map         ;
  uint               n_shaders         ;
  uint               *shader           ;
  uint               n_light_cfgs      ;
  uint               *light_cfg        ;

  uint               n_vertices        ;  //  Number of vertices used for this distance/sub-object
  Vertex             *vertex           ;  //  List of vertices with normals and texture coordinates
  uint               n_vertex_sets     ;  //  Number
  VertexSet          *vertex_set       ;  //  Start and end of sections within 'vertex' list (Use??)
  uint               n_tri_lists       ;  //  Number of lists of triangles to plot
  TriList            *tri_list         ;  //  Lists of triangles and associated information
}  SubObject ;

typedef struct distance_level {
  uint               index             ;  // Distance level index
  float              dlevel_selection  ;  //  Maximum distance(m) for this level of detail.
  uint               n_hierarchy       ;  //  Size of hierarchy array
  int                *hierarchy        ;  //  If hierarchy[i] = j, then the matrix[j]
                                  // transformation must be applied before matrix [i]
  uint                n_sub_objects     ;  //  Number of sub-objects to plot
  SubObject          *sub_object       ;  //  Pointers to sub-object data
  uint               gl_display_list   ;  // Dsplay list for this sub-object in this range
}  DistLevel ;

typedef struct lodcontrol {
  int                dlev_bias         ;
  uint               n_dist_levels     ;  //  Number of levels of detail
  DistLevel          *dist_level       ;  //  Pointers to detail data for various distances
}  LodControl ;

typedef struct esdcomplexbox{
  float              f1[12]  ;
} EsdComplexBox ;

/*
 *==============================================================================
 *  Structure ShapeNode - Intialised by load_shape_filenames() in 'shape.c'.
 *==============================================================================
 */

typedef struct shapenode {
  struct shapenode   *next     ;
  char               *name     ;
  char               *s_file   ;
  char               *sd_file  ;
  uint               basic     ;             // = 1 always needed
  uint               needed    ;             // = 1 if needed for graphics
  uint               loaded    ;             // = 1 if loaded into graphics card
//  int                gl_display_list ;     //  OpenGl display list index
//  int                gl_display_list2 ;
  uint               flags1    ;
  uint               flags2    ;
  uint               nvolumes  ;    // Number of shape volumes (1 per lod_control ??)
  ShapeVolume        *shape_vol ;   // Array of shape volumes
  uint               nshaders  ;
  char               **shader  ;
  uint               nfilters  ;
  char               **filter  ;
  uint               npoints   ;
  Vector3            *point    ;          //  Array of vectors
  uint               nnormals  ;
  Vector3            *normal   ;          //  Array of normal vectors
  uint               nuvpoints ;
  UVVector2          *uvpoint  ;          //  Array of texture vectors
  uint               nsort_vectors ;
  Vector3            *sort_vector  ;
  uint               ncolors   ;
  RGBA               *color    ;
  uint               nmatrices ;
  Matrix4x3          *matrix   ;          //  MSTS 4x3 matrices
  uint               n_textures       ;   //  Number of textures used (OR: nimages)
  char               **texture_name   ;   //  (OR: image)
  TextureNode        **texture        ;   //  Array of pointers to the Textures
                                          //  used by this shape (OR: image_tex)
  uint               use_texture ;        //  = 1 if texture available
  uint               n_texlevel_low   ;
  TexLevelLow        *texlevel_low    ;   //  Low level data on displaying textures
  uint               nlight_materials ;
  LightMaterial      *light_material  ;
  uint               nlight_model_cfgs ;
  LightModelCfg      *light_model_cfg  ;
  uint               n_vtx_states      ;
  VTXState           *vtx_state        ;
  uint               n_prim_states     ;
  PrimState          *prim_state       ;

  uint               dlevel_selection  ;
  uint               nhierarchy        ;    //  CONFUSE WITH distance level !!!!  Should be here...  ??
  int                *hierarchy        ;
  uint               *hierarchy_flag   ;    //  1=Wheel, 2=Bogie
  uint               n_lod_controls    ;
  LodControl         *lod_control      ;
  uint               n_animations      ;
  Animation          *animation        ;
// SD File Data
  int                esd_detail_level          ;
  int                esd_alternative_texture   ;
  float              esd_tunnel_length         ;
  float              esd_tunnel_f1             ;
  float              esd_tunnel_f2             ;
  int                esd_tunnel_i1             ;
  int                esd_tunnel_i2             ;
  int                esd_no_visual_obstruction ;
  int                esd_snapable              ;
  char               *esd_sound_file_name      ;
  float              esd_bell_animation_fps    ;
  float              esd_bounding_box_xmin     ;
  float              esd_bounding_box_xmax     ;
  float              esd_bounding_box_ymin     ;
  float              esd_bounding_box_ymax     ;
  float              esd_bounding_box_zmin     ;
  float              esd_bounding_box_zmax     ;
  uint               n_esd_complex_box         ;
  float              *esd_complex_box          ;  //  Array n*3
//  ZR extras
  int                no_culling                ;  //  Some/all surfaces are double sided
} ShapeNode  ;



