/*
 * *****************************************************************************
 *  File zm_track.h
 *
 *  This file contains the track definitions needed by zm
 *
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
typedef struct vector3           Vector3        ;
typedef struct dynprofile        DynProfile     ;

struct vector3 {
  float              X ;
  float              Y ;
  float              Z ;
} ;                                     //  typedef Vector3 defined near top of file

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

