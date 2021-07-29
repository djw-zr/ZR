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

double  eng_speed    =  0.0    ;
double  eng_seconds0 = -1.0    ;
struct  travellernode trav_node_0 ;

/*
 *  TravellerNodes contain information on the position of
 *  each traveller in space and relative to the track layout.
 *
 *  When each train moves the primary traveller on the engine is moved a
 *  distance corresponding to the speed of the train and the time since the
 *  last update.  Other travellers on engines and wagons in the train are
 *  moved so that the track separation between travellers remains constant.
 *
 */

typedef struct travellernode {
  ShapeNode     *wagon      ;  //  Current wagon
  TrkNetNode    *tn         ;  //  Current track section in track network
  TrkVectorNode *vn         ;  //  Current vector in track section

  double       x            ;  //  Position (m) of traveller
  double       y            ;  //         ... relative to vector start
  double       z            ;  //         ...
  double       ang_deg      ;  //  Rotation of traveller relative to start point
                               //  degrees
  double       position     ;  //  Position within section

  int          itrack       ;  //  Current track node
  int          ivector      ;  //  Index of current vector
  int          idirect      ;  //  1/0 = increase/decrease in distance moved by
                               //        traveller increases vector position
} TravellerNode ;

/*
 *  WagonNodes are used to specify wagons on the track, wagons in consists
 *  and basic wagons kept in memory ready to be added to trains or consists.
 *  TravellerNodes are added when trains are constructed.  The next and prev
 *  pointers to WagonNodes are added when consists are constructed and
 *  modified (or added) when trains are constructed.
 */

typedef struct wagonnode{
  struct wagonnode  *next      ;  //  Next wagon in train or NULL
  struct wagonnode  *prev      ;  //  Previous wagon or NULL
  char              *name      ;  //  Name of wagon type (as in raw_wagon)
  int               index      ;  //  Position of wagon in train or consist
  int               motor      ;  //  True (non-zero) if this has a motor
  int               direction  ;  //  True if front in direction of train front
  int               n_travel   ;  //  Number of traveller nodes.
  double            dist_front ;  //  Distances from wagon shape origin ..
  double            dist_back  ;  //  .. to limits of front and back buffers
  TravellerNode     *traveller ;  //  Vector of traveller nodes
  ShapeNode         *shape     ;  //  Shape node defining wagon
  RawWagonNode      *raw_wagon ;  //  Node with wagon's basic data
} WagonNode ;

/*
 *   TrainNode are used to specify 'trains', located on the track,
 *   and 'consists', standard trains that can be used to generate
 *   a new train at a chosen point on the track.
 */

typedef struct trainnode{
  struct trainnode  *next  ;  // Next train in linked list
  char              *name  ;  // Unique name of train (or consist)
  double            speed  ;  // Speed of train
  WagonNode         *motor ;  // Pointer to primary motor or NULL
  WagonNode         *first ;  // Pointer to first wagon in train
  WagonNode         *last  ;  // Pointer to last wagon in train
} TrainNode ;




