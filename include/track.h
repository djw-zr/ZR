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
int               n_dist_levels ;  // Number of distance levels
TrackDistLevel    *dist_level   ;  // Data for each LOD distance
}  ;

struct trackdistacelevel {
float             distance      ;  // Maximum distance to use this data
int               n_sub_objects ;  // Number of sub-objects
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
int               n_profiles    ;  // Number of profiles
RailProfile       *profile      ;  // Profiles to use with this LOD
}  ;                               // Typedef : TrackSubObject

struct    railprofile {            // Rail profile  (Polyline in OR)
char              *name         ;  // Name of this profile
int               n_elements    ;  // Number of vertices, normals and uvvectors
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
int               index         ;  //
float             gauge         ;  //
float             length        ;  //
float             radius        ;  //
float             angle         ;  //  Radians as in DynTrackSect
float             skew          ;
int               water_scoop   ;  //
} ;

struct sectionidx{
int               num_sections  ;  //
float             east_x        ;  //  Offset of section start from shape origin
float             height_y      ;  //
float             north_z       ;  //
float             angle         ;  //  Around height_y axis
int               *section      ;  //  Indices
} ;

struct trackshape{
TrackShape        *next         ;
int               index         ;
int               num_paths     ;  //
char*             filename      ;  //
int               main_route    ;  //
float             clearance_dist;  //
SectionIdx        *section_idx  ;  //
int               tunnel_shape  ;  //
int               road_shape    ;  //
int               manual_junction_shape  ;  //
int               crossover_shape        ;  //
int               num_xover     ;
Vector3           *xover        ;
}  ;



