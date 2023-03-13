/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  track.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Data structures used in generating tracks
 *   Based on OpenRails DynamicTrack.cs and related files
 *
 *==============================================================================
 */


/*
 *  Dynatrack data
 *  This specifies the track profile and other data required to generate
 *    sections of track.  The profile is not a full shape structure but
 *    for consistancy the naming and organisation used here is similar.
 *    Because the number of objects at each level is not known beforehand,
 *    i.e. not given in any of the input files - these structures use
 *    linked lists with NULL at the end of each list.
 */
//typedef struct dynprofile DynProfile ;
typedef struct transfernode TransferNode ;

struct dynprofile {
DynProfile        *next         ;  // Next in linked list of track profiles
char              *name         ;  // Name of this track profile
enum LODMethod    lod_method    ;  // Scheme for combining LOD data
enum PitchControl pitch_control ;  // Scheme for controlling error on curves
float             cangle_max    ;  // Maximum chord angle (from centre of circle)
float             pitch_max     ;  // Distance (m) controlling error on curves
                                   // CHORDLENGTH       : maximum length of chord
                                   // CHORDDISPLACEMENT : max displacement from circle
float             gauge         ;  // Track gauge
float             inner         ;  // Distance of rail inner face from centre line
float             outer         ;  // Distance of rail outer face from centre line
uint              n_dist_levels ;  // Number of distance levels
TrackDistLevel    *dist_level   ;  // Data for each LOD distance
}  ;

struct trackdistacelevel {
float             distance      ;  // Maximum distance to use this data
uint              n_sub_objects ;  // Number of sub-objects
TrackSubObject    *sub_object   ;  // Sub-objects at this distance
}  ;                               // Typedef : TrackDistLevel

struct tracksubobject{
char              *name         ;  // Name for this LOD
char              *texture_name ;  // Name of Texture
TextureNode       *texture      ;  // Pointer to texture
char              *shader_name  ;  // Shader to use (MSTS/Direct X value?)
char              *light_model  ;  // Name of Light model (Ditto)
char              *tex_add_mode_name; // Name of texture method to use (Ditto)
int               alpha_test    ;  // 1/0 : if 1 use alpha test
int               ESD_alt_tex   ;  // ESD_Alternative_Texture
int               MipMap_LOD_Bias; // Bias to use with MipMaps
uint              n_profiles    ;  // Number of profiles
RailProfile       *profile      ;  // Profiles to use with this LOD
}  ;                               // Typedef : TrackSubObject

struct    railprofile {            // Rail profile  (Polyline in OR)
char              *name         ;  // Name of this profile
uint              n_elements    ;  // Number of vertices, normals and uvvectors
Vector3           *vertex       ;  // Vertices of profile
Vector3           *normal       ;  // Normals
UVVector2         *uvvector     ;  // Texture coordinates
UVVector2         delta_tex     ;  // Texture coordinate displacement
}  ;                               // Typedef : RailProfile

/*
 *==============================================================================
 *   Track Sections data
 *
 *    This contains data from the TrackSections and TrackShape
 *    items in files tsection.dat
 *
 *==============================================================================
 */
typedef struct tracksection TrackSection ;
typedef struct trackshape   TrackShape   ;
typedef struct sectionidx   SectionIdx   ;

struct tracksection{
TrackSection      *next         ;  //
uint              index         ;  //
float             gauge         ;  //
float             length        ;  //
float             radius        ;  //
float             angle         ;  //  Radians as in DynTrackSect
float             skew          ;
int               water_scoop   ;  //
} ;

struct sectionidx{
uint              num_sections  ;  //
float             east_x        ;  //  Offset of section start from shape origin
float             height_y      ;  //
float             north_z       ;  //
float             angle         ;  //  Around height_y axis
int               *section      ;  //  Indices
} ;

struct trackshape{
TrackShape        *next         ;
int               index         ;
uint               num_paths     ;  //
char*             filename      ;  //
int               main_route    ;  //
float             clearance_dist;  //
SectionIdx        *section_idx  ;  //
int               tunnel_shape  ;  //
int               road_shape    ;  //
int               manual_junction_shape  ;  //
int               crossover_shape        ;  //
uint               num_xover     ;
Vector3           *xover        ;
}  ;

/*
 *==============================================================================
 *   TurnTable data
 *
 *    The dummy versions of track sector and vector define the single section
 *    of track rotating with the turntable.  When not aligned with a proper
 *    section of track wagons on the turntable are drawn using the dummy data.
 *
 *==============================================================================
 */
typedef struct tt_path    TT_Path   ;
typedef struct tt_list    TT_List   ;

struct tt_path{
  TT_Path         *next ;
  TrkSector       *trk_sector ;  //  Track sector of this path
  TrkVectorNode   *trk_vector ;  //  Track vctor of this path
  int             trk_sector_uid,
                  trk_vector_id ;
  double          angle       ;  //  Rotation relative to its initial position
} ;

struct tt_list{
  TT_List        *next  ;
  TrainNode      *train ;
} ;


struct turntable{
  TurnTable       *next  ;
  WorldItem       *witem ;
  int             w_uid  ;
  int             tile_x ;
  int             tile_y ;
  double          x_offset ;
  double          y_offset ;
  double          z_offset ;
  int             trackshapeindex ;
  char            *animation ;
  double          diameter   ;
/*
 *   Variables used to constrct the rotating track vector 'trl_vector0'
 *
 *   The angle variable here is the angle of the turntable world item converted
 *   to the same coordinate system as the angles in each path structure -
 *   The path structure angles are copies of those in the track vectors.
 */
  int             tile_x0 ;
  int             tile_y0 ;
  double          x0      ;  //  Coordinates of central point
  double          y0      ;  //  ZR (physics) coordinates
  double          z0      ;
  double          ax0     ;  //  Coordinates of central point
  double          ay0     ;
  double          az0     ;
  double          angle0  ;  //  Initial/zero angle of turntable world item (deg)

  double          angle   ;  //  Current angle of turntable (deg)
  int             n_paths ;  //  Number of paths
  TT_Path         *f_path ;  //  Current path (=0 if not alligned)
  int             i_final ;  //  True if turntable moving to final position
  int             i_180   ;  //  True if turntable 180 degrees off from path
  TT_Path         *path   ;

  TrkSector       trk_sector0 ;  //  Dummy tt sector
  TrkVectorNode   trk_vector0 ;  //     ... and vector

  int             i_dirn  ; // 1 clockwise, -1 anti-clockwise, 0 = no motion

  TT_List         *tt_list ;
} ;

struct transfernode{
  TransferNode    *next ;
  WorldItem       *wi_node ;
  TrkItem         *ti_node ;
  int             on       ;  //  = 1 when transfer in progress
} ;
