/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  wagon.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *
 *    Wagon structure variables used by ZR
 *
 *==============================================================================
 */
#if 0
typedef struct rawenginenode {
int          has_wipers      ;
int          has_mirrors     ;
int          has_pantographs ;
} RawEngineNode ;
#endif
typedef struct lightstate {
double          duration  ;
uint            colour    ;
double          position[3] ;
double          azimuth[3]  ;
int             transition ;
double          radius    ;
} LightState ;


typedef struct wagonlight {
int             type      ;
// Conditions
int             control   ;
int             timeofday ;
int             unit      ;
int             service   ;

int             fade_in   ;
int             fade_out  ;
int             cycle     ;
int             n_states  ;
LightState      *states   ;
} WagonLight ;


typedef struct rawwagonnode {
struct rawwagonnode *next    ;
int              is_engine   ;      //  True if engine
char             *name       ;
char             *file       ;      // *.eng or *.wag file
char             *s_file     ;      // Shape file
char             *sd_file    ;      // Additional information file
char             *fs_file    ;      // Freight shape file
char             *image      ;      // Silhouette image file (*.jpg)
ShapeNode        *shape      ;      // Corresponding shape node
ShapeNode        *f_shape    ;      // Corresponding freight shape

// Properites defined in *.wag files

char             *type       ;      //  Carriage etc
int              has_wipers  ;
int              has_mirrors ;
int              has_pantographs ;

double           width       ;
double           height      ;
double           length      ;
double           mass        ;
double           wheelradius           ;
double           driverwheelradius     ;  //  For efficiency:
double           inv_wheelradius       ;  //   Multiplying by inverse
double           inv_driverwheelradius ;  //   faster than division
int              numwheels   ;
// Freight data
double           f_max_level ;
double           f_min_level ;
int              f_anim_flag ;
// Coupling
int              ncoupling   ;      // Normally 1 coupling type
char             *couplingtype ;
int              couplinghasrigidconnection1 ;
double           stiffness1  ;
double           stiffness2  ;
double           break1      ;
double           break2      ;
double           r01         ;
double           r02         ;
char             *couplingtype2 ;   // Back coupling if different
int              couplinghasrigidconnection2 ;
double           stiffness21  ;
double           stiffness22  ;
double           damping21    ;
double           damping22    ;
double           break21      ;
double           break22      ;
double           r021         ;
double           r022         ;
//  Brakes
char             *brakeequipmenttype ;
char             *brakesystemtype    ;
double           maxbrakeforce       ;
double           maxhandbrakeforce   ;
double           emergencyresvolumemultiplier   ;
double           triplevalveratio    ;
double           maxreleaserate      ;
double           maxapplicationrate  ;
double           maxauxilarychargingrate ;
double           emergencyreschargingrate ;
double           emergencyrescapacity     ;
double           brakecylinderpressureformaxbrakebrakeforce ;  // !!
double           brakepipevolume          ;
double           ortsbrakefriction1[15]   ;
double           ortsbrakefriction2[15]   ;
//  Friction
char             *ortsbearingtype    ;
double           ortsdavis_a         ;
double           ortsdavis_b         ;
double           ortsdavis_c         ;
double           ortswagonfrontalarea ;
double           ortsdavisdragconstant;
double           centreofgravity[3]  ;
double           ortstrackgauge      ;
double           ortsrigidwheelbase  ;
double           ortsunbalancedsuperelevation ;
// Lights
int              n_lights            ;
WagonLight       *wagon_light        ;

// Freight animations

//  Passenger view
char             *passengercabinfile ;
double           passengercabinheadpos[3] ;
double           startdirection[3]   ;
double           rotationlimit[3]    ;

// Misc

char             *sound_file         ;
char             *full_name          ;
int              has_doors           ;

//RawEngineNode    *raw_engine         ;
struct rawwagonnode *tender  ;      //  Engines only: pointer to tender
} RawWagonNode  ;

