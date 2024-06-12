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

typedef struct wagonintake WagonIntake ;

typedef struct rawenginenode {
char         *type           ;
char         *name           ;
char         *long_name      ;
char         *description    ;
double       wheel_radius    ;
int          num_wheels      ;
double       head_out[3]     ;  //  to right, up, forward
char         *sound          ;
} RawEngineNode ;



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


struct wagonintake {
WagonIntake      *next    ;
double          position  ;
double          radius    ;
char            *name     ;
int             type      ;
} ;


typedef struct rawwagonnode {
struct rawwagonnode *next    ;
int              is_engine   ;      //  True if engine
int              is_needed   ;
int              is_loaded   ;
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
int              has_doors   ;

double           width       ;
double           height      ;
double           length      ;
double           mass        ;
double           wheelradius           ;
double           driverwheelradius     ;  //  For efficiency:
double           inv_wheelradius       ;  //   Multiplying by inverse
double           inv_driverwheelradius ;  //   faster than division
int              numwheels   ;
//  Water, Coal, Fuel
WagonIntake       *intake     ;
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
double           brakepipevolume     ;
int              n_ortsbrakefriction ;
double           *ortsbrakefriction1 ;
double           *ortsbrakefriction2 ;
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
SMS_Node         *sms_node           ;
ALuint           *sources            ;    //  Array of sound sources
char             *full_name          ;

RawEngineNode    *raw_engine         ;
struct rawwagonnode *tender  ;      //  Engines only: pointer to tender
} RawWagonNode  ;

