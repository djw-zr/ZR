/*
 * =============================================================================
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  world.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file containing the structures used by ZR to store the data
 *   included in each world file.
 *
 * =============================================================================
 */

/*
 *  World files 'Static Flag' constants defined in OpenRails as enums.
 */
static uint   SF_RoundShadow       = 0x00002000 ;
static uint   SF_RectangularShadow = 0x00004000 ;
static uint   SF_TreelineShadow    = 0x00008000 ;
static uint   SF_DynamicShadow     = 0x00010000 ;
static uint   SF_AnyShadow         = 0x0001E000 ;
static uint   SF_Terrain           = 0x00040000 ;
static uint   SF_Animate           = 0x00080000 ;
static uint   SF_Global            = 0x00200000 ;
/*
 *  World files 'Platform Data Flag' constants defined in OpenRails as enums.
 */
static uint   PD_PlatformLeft      = 0x00000002 ;
static uint   PD_PlatformRight     = 0x00000004 ;

typedef struct carspawnobj {
  uint            carspawnerlist_idx ;
  float           car_frequency      ;  //  Default 5
  float           car_av_speed       ;  // Default 20 m/s (~50 mph)
  char            *list_name         ;
} CarSpawnObj ;


typedef struct dyntracksect {
  uint            is_curved          ;  // SubBlock 1 = curved, 0 = straight
  int             uid                ;  // Id  (maybe -1??)
  float           param_1            ;  // Length (m) for straight,
                                        // Arc (radians) for curved
  float           param_2            ;  // 0 for straight,
                                        // Radius (m) for curved
  float           delta_y            ;  // Elevation change for this subsection
} DynTrackSect ;

typedef struct dyntrackobj {
  uint            section_idx        ;
  uint            collide_flags      ;
  uint            gl_display_list[3] ;
  float           dist_level[3]      ;
  float           elevation          ;
  DynTrackSect    dyn_trk_sect[5]    ;
  TrkSectNode     *tsnode            ;  // Generated node used to hold
                                        //   display list for dynamic track
} DynTrackObj ;

typedef struct trackobj {
  uint           section_idx         ;  //  Index of track section
  float          elevation           ;
  uint           collide_flags       ;
  int            tile_x              ;  // Position of Junction Node
  int            tile_y              ;  //        "
  float          X                   ;  //        "
  float          Y                   ;  //        "
  float          Z                   ;  //        "
  TrkVectorNode  *track_vec          ;  //  Pointer to corresponding vector node
} TrackObj ;

typedef struct forestobj {
  char            *tree_texture      ;
  float           scale_range_min    ;
  float           scale_range_max    ;
  float           X                  ;
  float           Y                  ;  // MSTS Z
  int             population         ;
  float           width              ;
  float           height             ;
} ForestObj ;

typedef struct speedpostobj {
  char            *speed_digit_tex   ;
  float           *shapes_info       ;
  float           size               ;
  float           dx                 ;
  float           dy                 ;
} SpeedPostObj ;

typedef struct sidingobj {
  int             siding_data        ;
} SidingObj ;

typedef struct transferobj {
  float            width             ;
  float            height            ;
} TransferObj ;

typedef struct signalobj {
  uint             signal_sub_object ;
  int              n_signal_units    ;
  int              *sub_object       ;
  uint             *u_data1          ;
  uint             *tr_item          ;
} SignalObj ;

typedef struct staticobj {
  int              no_direct_light   ;
} StaticObj ;

typedef struct levelcrobj {
  float            warning_time      ;
  float            minimum_distance  ;
  int            crash_probability   ;
  int              level_cr_data_1   ;
  int              level_cr_data_2   ;
  int              visible           ;
  int              silent            ;
  float            initial_timing    ;
  float            serious_timing    ;
  float            anim_timing       ;
} LevelCrObj ;

typedef struct pickupobj {
  uint             collide_flags     ;
  float            min_mps           ;   //  Minimum train speed for pickup
  float            max_mps           ;   //  Maximum train speed for pickup
  uint             pickuptype        ; // pickup_type causes compiler conflict
  uint             pickuptype_2      ;
  float            pickup_options    ;      // Should this be uint
  float            animation_speed   ;
  float            quantity_available_kg ;   //  Weight in kg
  float            feed_rate_kgps    ;      //  Rate in kg/second
} PickupObj ;

typedef struct platformobj {
  uint             platform_data     ; // Not saved by OR
} PlatformObj ;

/*
 *  Instead of using a separate structure for each type of world item,
 *  a single 'worlditem' is used which contains common information.
 *  Data specific to a particular object type is held in a union
 *  structure.  This means that every object requies the same memory
 *  space as the largest.  If space is ever a problem the union
 *  could be replaced by a set of pointers or a separate structure
 *  used for each type of world item with the union containing the
 *  pointers moved to the 'world node'.
 */

typedef struct worlditem {
  struct worlditem *next  ;
  int              uid             ;   //  World file UID
                                       //  Used by track database
  int              worldtype       ;   //  Forest, Speedpost etc
  char             *filename       ;   //  Shape file name from world file
  ShapeNode        *snode          ;   //  Pointer to shape node
  uint             static_flags    ;   //  See SD constants (at start of file)
  uint             collide_flags   ;   //  As it says
// Vector and Quaternon defining objects position and orientation in tile
  double           X               ;   //  X position in tile, geographic coords
  double           Y               ;   //  Y position in tile
  double           Z               ;   //  Z position in tile

  double           A               ;   //  Quaternion used by OR
  double           B               ;   //
  double           C               ;   //
  double           D               ;   //

  double           AX              ;   //  X coordinate of rotation vector
  double           AY              ;   //  Y coordinate of rotation vector
  double           AZ              ;   //  Z coordinate of rotation vector
  double           ANG             ;   //  Angle of rotation (degrees)

  int              vdb_id          ;
  int              iz_off          ;   //  PolygonOffset flag (0,1,2)
  uint             static_detail_level ;
  int              n_tr_item       ;
  float            tr_item_db[4]   ;
  float            tr_item_db_id[4];
  double           max_vis_distance ;
  union {                                 //  Item specific data
    TrackObj       track_obj       ;
    DynTrackObj    dyn_track_obj   ;
    CarSpawnObj    car_spawn_obj   ;
    ForestObj      forest_obj      ;
    LevelCrObj     levelcr_obj     ;
    PickupObj      pickup_obj      ;
    PlatformObj    platform_obj    ;
    SidingObj      siding_obj      ;
    SignalObj      signal_obj      ;
    StaticObj      static_obj      ;
    SpeedPostObj   speed_post_obj  ;
    TransferObj    transfer_obj    ;
  } u ;
} WorldItem ;

/*
 *  Visibility sphere
 *    Apparently not used by OR but a quick way of determining
 *    if a world tile needs to be processed.  If none of the
 *    tile's spheres can be seen then none of the tile items
 *    can be seen so the world tile can be skipped.
 */

typedef struct vdbsphere {
  int              vdb_id ;
  float            X      ;          //  Centre of 'sphere'
  float            Y      ;
  float            Z      ;
  float            radius ;          //  Radius of sphere
} VDbSphere ;

/*
 *   The information from each world file is saved in a "world node".
 *   This contains basic items and a pointer to a list of "items"
 *   specified in the world file.  The items are primarily fixed
 *   structures, possibly with moving parts (i.e. level crossings),
 *   but also include local 'hazards', often animals that may stray
 *   onto the tracks, and 'car spawners' for vehicles moving on
 *   the roads.
 *
 *   To reduce graphics storage only tiles marked as 'in_use' are
 *   plotted.
 */

typedef struct worldnode {
  struct worldnode *next  ;
  int              tile_x ;          //  East co-ordinate of tile
  int              tile_y ;          //  North co-ordinate of tile
  int              in_use ;          //  Used for shapes
  char             *name  ;          //  Short name of world file
  char             *wfile ;          //  Full directory name of worldfile
  int              tr_watermark  ;
  uint             vdbid_count ;     //  Number of visibility spheres
  VDbSphere        *vdb_sphere ;     //  Info on visibiliaty from a distance
  WorldItem        *world_item ;     //  List of fixed structures
  BTree            *shape_tree ;     //  Btree containing shapes
} WorldNode ;

/*
 *  Structure containing central position, tangent and cross track vectors
 *  used to define a section of track.
 */


typedef struct vector3x3 Vector3x3 ;
struct vector3x3 {
Vector3x3        *next  ;
float            vxx     ;     //  Point on central axis
float            vyy     ;
float            vzz     ;
float            vtx     ;     //  Tangent vector
float            vty     ;
float            vtz     ;
float            vnx     ;     //  Cross track vector (to right of tangent)
float            vny     ;
float            vnz     ;
} ;
