/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  train.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Data structures used for travellers
 *   Based on Traveller.cs and other OpenRails files
 *
 *==============================================================================
 */

/*
 *==============================================================================
 *
 *  Organisaiton of storage for trains and wagons
 *
 *   At the top level is a linked list of trains, which are engines, engines
 *   plus trucks or groups of trucks placed on the rail network.
 *
 *   Each train contains information on the train, such as the controlling
 *   engine (if any) plus a linked list of wagons, some of which may be engines
 *   and tenders.
 *
 *   Each wagon will have it own information plus a link to common information
 *   for that type of wagon (type raw_wagon) and links to one or two travellers.
 *   Each traveller has its own information on position.
 *
 *   Engineering data on each wagon is stored in raw_wagon structures.  This
 *   also contains links to the shape structures.
 *
 *   Standard collections of wagons are stored in consists.  These can be
 *   defined in input files and contain links to the raw wagon files.  Each
 *   train is normally initialised by copying the linked list structure
 *   from one of the consists.
 *
 *==============================================================================
 */

/*
 *  TravellerNodes contain information on the position of
 *  each traveller in space and relative to the track layout.
 *
 *  When each train moves the primary traveller at the back of the train
 *  is moved a distance corresponding to the speed of the train and the time
 *  since the last update.  Other travellers on engines and wagons in the train
 *  have their variables updated each timestep that the train moves - such
 *  that the track separation between travellers remains constant.
 *
 *  Note:  if(idirect == 0) ang_deg = ang_deg + 180 (traveller.c)
 *
 *  Note:  typedefs moved to typedef.h
 */


struct travellernode {
  ShapeNode    *shape       ;  //  Shape of current wagon
  WagonNode    *wagon       ;  //  Current wagon  (== NULL for dummy wagons)
  TrkSector    *tn          ;  //  Current track section in track network
  TrkVectorNode *vn         ;  //  Current vector in track section

  double       x           ;  //  Position (m) of traveller
  double       y            ;  //         ... relative to vector start
  double       z            ;  //         ...
  double       rx           ;  //  Position (m) of traveller
  double       ry           ;  //         ... relative to origin of layout
  double       rz           ;  //         ...
  double       vx           ;  //  Velocity (m/s) of traveller
  double       vy           ;  //  Velocity (m/s) of traveller
  double       vz           ;  //  Velocity (m/s) of traveller
  double       ang_deg      ;  //  Rotation of traveller relative to start point
                               //  degrees
  double       vect_position ;  //  Position within track vector
  double       sect_distance ;  //  Distance within track section

  uint         itrack       ;  //  Current track node
  uint         ivector      ;  //  Index of current vector
  uint         idirect      ;  //  1/0 = increase/decrease in distance moved by
                               //        train increases traveller vector position
}  ;

/*
 *  WagonNodes are used to specify wagons on the track, wagons in consists
 *  and basic wagons kept in memory ready to be added to trains or consists.
 *  TravellerNodes are added when trains are constructed.  The next and prev
 *  pointers to WagonNodes are added when consists are constructed and
 *  modified (or added) when trains are constructed.
 */

struct wagonnode{
  WagonNode     *next      ;  //  Next wagon in train or NULL
  WagonNode     *prev      ;  //  Previous wagon or NULL
  TrainNode     *train     ;
  char          *name      ;  //  Name of wagon type (as in raw_wagon)
  int           index      ;  //  Position of wagon in train or consist
  int           is_engine  ;  //  True (non-zero) if this has a motor
  uint          train_dir  ;  //  True if wagon in same direction as train
  int           n_travel   ;  //  Number of traveller nodes.
  int           bell_on      ;  // = 1 when engine bell is on
  int           sander_on    ;  // = 1 when engine sander is on
  int           has_wipers   ;  // = 1 when engine has wipers
  int           wipers_on    ;  // = 1 when wipers switched on
  int           wipers_off   ;  // = 1 when wiper switched off
  int           has_mirrors  ;  // = 1 when engine has driver mirrors
  int           mirrors_out  ;  // = 1 when mirrors (moving) out
  int           has_pantographs  ;  // = 1 when engine has pantographs
  int           pantographs_up   ;  // = 1 when pantographs (being) raised

  double        dist_front ;  //  Distances from wagon shape origin ..
  double        dist_back  ;  //  .. to limits of front and back buffers
  double        wheel_angle       ;  // Degrees
  double        driverwheel_angle ;  // Degrees

  double        wipers_ang   ;  // 0 to 360 - converted 0.5(sin(ang)+1.0)
  double        mirrors_dist ;  // 0 to 1
  double        pantographs_dist ;  // 0 to 1

  TravellerNode *traveller ;  //  Vector of traveller nodes
  ShapeNode     *shape     ;  //  Shape node defining wagon
  RawWagonNode  *raw_wagon ;  //  Node with wagon's basic data (wagon.h)
// Lists of sound triggers - bells, horn etc.
  TrgLstNode    *snd_trigger ;  //  Linked list of (recent) sound triggers
  ActLstNode    *snd_active  ;  //  Linked list of active sounds
/*
 *  OpenAL variables
 *  Note: streams in SMS files are sources in OpenAL
 */
#ifdef OPENAL
  SmsGrpNode    *smsgrp_node ;  //
  int           n_sources    ;  //
  ALuint        *source      ;  //  OpenAL stream numbers
  int           *sound_on    ;  //  Sound_on - dist_on && source_on
  int           *source_on   ;  //  Source on flags
  double        *sound_gain  ;  //  Sound gain before speed etc. controls
  int           distance_on  ;  //  Wagon distance < sound max distance
  int           *cue_points  ;  //  Wave file cue points active
  int           *cue_in      ;
  int           *cue_out     ;
  char          **wav_file   ;  //  Last wave files added to sources
  SmsStrNode    *snd_stream  ;  //  Pointers to SMS sound stream nodes
#endif

} ;

/*
 *   TrainNode; used to specify each train on the track.  This includes
 *   isolated wagons and groups of wagons without an engine.
 */

struct trainnode{
  struct trainnode  *next        ;  // Next train in linked list
  char              *name        ;  // Short unique name of train (or consist)
  char              *description ;  // Brief summary
  double            speed        ;  // Speed of train
  double            last_speed   ;  // Speed at last graphics update
  double            max_velocity ;  // Maximum velocity allowed
  double            durability   ;  // Durability of cargo
  int               n_wagons     ;  // Number of wagons
  WagonNode         *motor       ;  // Pointer to primary motor or NULL
  WagonNode         *first       ;  // Pointer to first wagon in train
  WagonNode         *last        ;  // Pointer to last wagon in train
  TravellerNode     front        ;  // Location of the front of the train
  TravellerNode     back         ;  // Location of the back of the train
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

