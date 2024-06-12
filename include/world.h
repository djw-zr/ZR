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

typedef struct worldnode    WorldNode ;

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
  double          param_1            ;  // Length (m) for straight,
                                        // Arc (radians) for curved
  double          param_2            ;  // 0 for straight,
                                        // Radius (m) for curved
  double          delta_y            ;  // Elevation change for this subsection
} DynTrackSect ;

typedef struct dyntrackobj {
  uint            section_idx        ;
  uint            collide_flags      ;
  uint            gl_display_list[3] ;
  double          dist_level[3]      ;
  double          elevation          ;
  DynTrackSect    dyn_trk_sect[5]    ;
  TrkSector     *tsnode            ;  // Generated node used to hold
                                        //   display list for dynamic track
} DynTrackObj ;

typedef struct trackobj {
  uint           section_idx         ;  //  Index of track section
  float          elevation           ;
  uint           collide_flags       ;
  uint           collide_function    ;
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
  float           X                  ;  // X dimension of forest
  float           Y                  ;  // Y dimension of forest (MSTS Z)
  int             population         ;  //  Number of treees
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
  uint             signal_sub_object ;  // 32 flags : 1 -> Head is active
  int              n_signal_units    ;  // Number of active heads
  int              *sub_object       ;  // Index of each active head
  uint             *u_data1          ;
  uint             *tr_item          ;  //  Track items corresponding to active ehads
  SignalDB         **signal          ;  //  Signal corresponding to HEAD
//  Data used when processing shape matrices
  uint             n_matrices        ;  //  Number of matrices defined in shape.
  uint             *sm_skip          ;  //  True for matrices for unused head shapes.
  SignalDB         **sm_signal       ;  //  Signal corresponding to active HEAD
  uint             *skip             ;  //  No longer used.
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
  int              n_tracks          ;  //  Number of tracks at crossing
  int              *track_id         ;
  float            *track_dist       ;
  int              n_roads           ;  //  Number of roads at crossing
  int              *road_id          ;
  float            *road_dist        ;
 //  Gate position : Open means open for road traffic
  int              should_be_open    ;  //  should be 1 = open, 0 = closed
  float            gate_position     ;  //  1.0 = open, 0.0 = closed
} LevelCrObj ;

typedef struct pickupobj {
  uint             collide_flags     ;
  float            min_mps           ;   //  Minimum train speed for pickup
  float            max_mps           ;   //  Maximum train speed for pickup
  uint             pickuptype        ;   // pickup_type causes compiler conflict
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
 *  WorldItem nodes contain information on how to display each item
 *  in the landscape.  Some of the data required differs between
 *  different types of object - for example the lights of a signal
 *  or the density of trees in a forest.
 *
 *  Instead of using a separate structure for each type of world item,
 *  a single 'worlditem' structure is used.
 *
 *  Data specific to a particular object type is held in a union
 *  structure.  This means that every object requies the same memory
 *  space as the largest.  If space is ever a problem the union
 *  could be replaced by a set of pointers or a separate structure
 *  used for each type of world item with the union containing the
 *  pointers moved to the 'world node'.
 *
 *  Note:  With (all/some?) TRANSFER objects, the filename is the name of
 *         a texture file.
 */

typedef struct worlditem {
  struct worlditem *next  ;
  int              uid             ;   //  World file UID
                                       //  Used by track database
  WorldNode        *world_node     ;   //  Pointer to world node for tile
  ShapeNode        *snode          ;   //  Pointer to shape node
  char             *filename       ;   //  Shape file name from world file
  char             *filename2      ;   //  Hazard file etc.
  int              worldtype       ;   //  Forest, Speedpost etc
  uint             static_flags    ;   //  See SD constants (at start of file)
  uint             collide_flags   ;   //  As it says
/*
 *  Vector defining objects position in tile.
 *  On input converted from MSTS to geographic axis convention
 */
  double           X               ;   //  X position in tile, geographic coords
  double           Y               ;   //  Y position in tile  :: MSTS Z
  double           Z               ;   //  Z position in tile  :: MSTS Y
/*
 *  Quaternon defining orientation in tile
 *  Quaternions, even MSTS quaternions, use geographic axis convention
 */
  double           A               ;   //  Quaternion used by OR
  double           B               ;   //
  double           C               ;   //
  double           D               ;   //
/*
 *  OpenGL angle and rotation vector :: geographic axis coordinates
 */
  double           AX              ;   //  X coordinate of rotation vector
  double           AY              ;   //  Y coordinate of rotation vector
  double           AZ              ;   //  Z coordinate of rotation vector
  double           ANG             ;   //  Angle of rotation (degrees)
/*
 *  Scale : used to scal unit forest trees to full size
 *  Use geographic axis convention
 */
  double           SX              ;   //  Scale X coordinate
  double           SY              ;   //  Scale Y coordinate
  double           SZ              ;   //  Scale Z coordinate

  double           max_vis_distance ;
  double           anim_value      ;   //  Variable used for animation

  int              iz_off          ;   //  PolygonOffset flag (0,1,2)
  uint             static_detail_level ;
  long             vdb_id          ;
/*
 *  Animation matrix variables.  If a world item is animated, the number of
 *  animation pointers equals the hierarch array size in the item's ShapeNode
 *  structure - but only pointers to animation variables will be non-NULL.
 */
  int              n_animations    ;   // Number of hierarchy animation variables
  double           **animations    ;
/*
 *  The number of associated track items is usually zero.  This is true for signals.
 *  Exceptions are SPEEDPOSTS (1, 2, 3, 4),LEVELCR (2, 4) SIDINGS (2), PLATFORMS (2)
 *                 CARSPAWNER (2)
 *  However the New Forest route needs more (10)  Why?
 */
  int              n_tr_item       ;   //  Number of associated track items (<9)
  int              tr_item_db[16]   ;   //  0 = Rail Item, 1 = Road Item
  int              tr_item_db_id[16];   //  UID of track items (Rail or Road)
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
 *  Sound Object Nodes
 *
 *  Some world tiles have fixed sources of sound - a town, birds, traffic.
 *  As these may generate more than one sound, for example steady traffic
 *  plus an occasional car or the sound of a car movng past, the complte
 *  item is termed a 'sound object', whereas the indicidual components
 *  are the 'sound sources'.  The sound objects are usually generally placed
 *  a few hundred metres from the track.
 *
 */
typedef struct soundobjectnode {
  struct soundobjectnode *next ;
  int        uid            ;
  int        source_set     ;
  double     X              ;  //  Position relative to tile origin
  double     Y              ;
  double     Z              ;
  char       *sms_file      ;  //  Sound Management System (sms) file.
//  SoundNode     *snd_node ;
  SMS_Node  *sms_node       ;
  int        priority_level ;
  int        n_streams      ;
  int        *stream_status ;
#ifdef OPENAL
  ALuint     *source        ;
  int        *sound_on      ;  //  Sound_on - dist_on && source_on
  int        *source_on     ;  //  Source on flags
  int        *last_file     ;  //  File used last time in sequential selection
  int        distance_on    ;  //  source distance < sound max distance
  double     *last_time     ;  //  Last time random trigger used
  double     *delay         ;  //  Last time random trigger used
#endif
} SoundObjectNode ;

/*
 *  Sound Region Node
 *
 *  There are also sound regions connected to track objects.  Each sound
 *  region is usually (always) one of a pair - the sound region lying in
 *  between.  Usually usually represent the sound of wheels clicking as
 *  a wagon crosses a junctions.  Sound is discussed in README.SOUND.
 */
typedef struct soundregionnode {
  struct soundregionnode *next ;
  int            uid          ;
  int            trk_item_id0 ;  // Track Item ID ?
  int            trk_item_id  ;  // Track Item ID
  int            snd_trk_type ;  // Soundregion track Type
  int            flags        ;
  int            detail_level ;
  uint32_t       vdbid        ;  // Usually FFFFFFFF
  double          X ;  //  Position relative to tile origin
  double          Y ;
  double          Z ;
  double         Q0 ;  //  Angle of sound cone
  double         QX ;  //  Direction
  double         QY ;
  double         QZ ;
  double         roty         ;  // Rotation about y (msts vertical) axis
  char           *filename    ;  //  Filename of shape !!
} SoundRegionNode ;

/*
 *  Level Crossing Summary Node
 *
 *    For each world node (or tile) this provides a list of local level crossings
 *    and links to the corresponding world items and rail or road track items.
 *
 *    With MSTS, each level crossing structure has its own track and road item used
 *    to trigger opening of the gates by trains or to identify where road traffic
 *    should stop when the gates are closed to traffic.
 *
 *    Eventually the plan is to combine the logic, so that where the crossing has
 *    two or more structures (i.e. pairs of gates or pairs of lifting arms),
 *    all are linked to the same set of track and road items.
 */


typedef struct levelxsnode {
  uint            n_world            ;  // Number of linked world items
  uint            n_rail             ;  // Number of linked rail items
  uint            n_road             ;  // Number of linked road items
  WorldItem       **world_item       ;  // Array of n_world pointers to world items
  uint            *rail_track_item   ;  // Array of n_rail indices of rail track items
  uint            *road_track_item   ;  // Array of n_road indices of road track items
  uint            *rail_uid          ;  // Array of n_rail track section UIDs
  uint            *rail_i_vec        ;  // Array of n_rail track vector indices
  double          *rail_dist         ;  // Array of n_rail distance along track vector
  double          *rail_s_dist       ;  // Array of n_rail distance along track section
  uint            *road_index        ;  // Array of n_road track section indices
  uint            *road_i_vec        ;  // Array of n_road track vector indices
  double          *road_dist         ;  // Array of n_road distance along track vector
  double          *road_s_dist       ;  // Array of n_road distance along track section
} LevelXSNode ;

/*
 *  Signal Summary Node
 *
 *    For each world node (or tile) this provides a list of signals and links
 *    to the corresponding world items and rail track items.
 *
 *    The aim here is to eventually combine the elements of a level crossing so that
 *    where the crossing has two or more gates, all are linked to the same set of
 *    track and road items.
 */


typedef struct sigsnode {
  uint            n_world            ;  // Number of linked world items
  uint            n_rail             ;  // Number of linked rail items
  WorldItem       **world_item       ;  // Array of pointers to world items
  TrkItem     **rail_track_item  ;  // Array of pointers to rail track items
} SigSNode ;


/*
 *  Visibility sphere
 *    Apparently not used by OR but a quick way of determining
 *    if a world tile needs to be processed.  If none of the
 *    tile's spheres can be seen then none of the tile items
 *    can be seen so the world tile can be skipped.
 */

typedef struct vdbsphere {
  uint             vdb_id ;
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
  WorldItem        *world_item ;     //  Linked List of fixed structures
  SoundObjectNode  *sound_source ;   //  Linked list of sound sources
  SoundRegionNode  *sound_region ;   //  Linked list of sound regions
  BTree            *shape_tree ;     //  Btree containing shapes
  uint             n_level_XS  ;     //  Number of level crossing sumary nodes
  uint             n_sig_S     ;     //  Number of signal sumary nodes
  LevelXSNode      *level_XS   ;     //  Array of level crossing summary nodes
  SigSNode         *signal_S   ;     //  Array of signal summary nodes
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
