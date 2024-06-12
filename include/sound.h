/*
 * =============================================================================
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  sound.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file containing the structures used by ZR to store
 *   sound data
 *
 * =============================================================================
 */

#ifdef OPENAL

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

  ALCcontext *al_context;
  ALCdevice  *al_device;

#endif

/*
 *  All Sound Management Items in use by the program have a SMS_Node,
 *    with its name, full filename and information on whether it is
 *    needed and loaded.  The following are defined in typedef.h:
 *
 *  typedef.h :: typedef struct smsnode    SMS_Node ;
 *            :: typedef struct smsgrpnode SmsGrpNode ;
 *            :: typedef struct smsactnode SmsActNode ;
 *            :: typedef struct smsstrnode SmsStrNode ;
 *
 *   Some MSTS Variables - see MSTSDieselLocomotive.cs
 *
 *         Variable1 =  ThrottlePercent / 100.0
 *         EngineRPM =  Variable2 * (MaxRPM - IdleRPM) + IdleRPM
 *         Variable2 =  (EngineRPM - IdleRPM)/(MaxRPM - IdleRPM)
 *         Variable3 =  DynamicBrakeForce/ MaxDynamicBrakeForce
 *
 *   Note:  The file ctn-z19-eng.sms used by engines in the
 *         Zig-zag railway has variable1 triggers in the range
 *         0 to 44, with volume control and frequency control
 */

struct soundfile{
  struct soundfile *next     ;
  char             *name ;
  char             *filename ;
  ALenum    format ;
  ALvoid    *data  ;
  ALsizei   size   ;
  ALsizei   freq   ;
  ALboolean loop   ;
  ALuint    buffer ;
};

struct smsfilnode{
  char      *wav_file  ;
  int       iopt       ;
  ALuint    buffer     ;
  SoundFile *soundfile ;
} ;

struct smsvolnode {
  int    dist_control  ;
  int    speed_control ;
  int    var1_control  ;
  int    var2_control  ;
  int    var3_control  ;
  double granuality    ;
  int    n_points      ;
  double *xxx_array    ;
  double *vol_array    ;
} ;

struct smsfrqnode {
  int    dist_control  ;
  int    speed_control ;
  int    var1_control  ;
  int    var2_control  ;
  int    var3_control  ;
  double granuality    ;
  int    n_points      ;
  double *xxx_array    ;
  double *frq_array    ;
} ;

struct smstrgnode{
  int    trigger_type       ;  //  initial, random, distance, descrete
  int    trigger_disabled   ;  // 1/0 Discrete trigger off/on

  int    distance_inc_past  ;  // Distance_Inc_Past
  int    distance_dec_past  ;  // Distance_Dec_Past
  int    speed_inc_past     ;  // Speed_Inc_Past
  int    speed_dec_past     ;  // Speed_Dec_Past
  int    variable1_inc_past ;  // Variable1_Inc_Past
  int    variable1_dec_past ;  // Variable1_Dec_Past
  int    variable2_inc_past ;  // Variable2_Inc_Past
  int    variable2_dec_past ;  // Variable2_Dec_Past
  int    variable3_inc_past ;  // Variable3_Inc_Past
  int    variable3_dec_past ;  // Variable3_Dec_Past
  int    stream_vol         ;  // SetStreamVolume
  double var_value          ;  //
  int    play_one_shot      ;  // PlayOneShot
  int    start_loop         ;  // StartLoop
  int    start_loop_release ;  // StartLoopRelease
  int    release_loop       ;  // ReleaseLoopRelease
  int    release_loop_jump  ;  // ReleaseLoopReleaseWithJump
  int    n_files            ;
  SmsFilNode *smsfil_node   ;
  int    sequential         ;
  int    random             ;
  int    disable_trigger    ;  // EnableTrigger
  int    enable_trigger     ;  // DisableTrigger

  int    i_discrete_trigger ;  //  Number of descrete trigger
  int    a_discrete_trigger ;  //  Trigger to disable/enable

  double delay_min     ;
  double delay_max     ;
  double distance_min  ;
  double distance_max  ;
  double volume_min    ;
  double volume_max    ;
} ;

struct smsstrnode {
  int    priority   ;
  double volume     ;
  int    n_triggers ;
  SmsTrgNode *smstrg_node ;
  SmsVolNode smsvol_node ;
  SmsFrqNode smsfrq_node ;

  int    buffer_set ;
  int    source_set ;
  ALuint buffer ;
  ALuint source ;
} ;

struct smsactnode {
  int    cab_cam ;
  int    ext_cam ;
  int    pas_cam ;
  double dist    ;
} ;

struct smsgrpnode{
  SmsActNode  active ;
  SmsActNode  deacti ;
  double      volume ;
  int         stereo ;
  int         loadall_waves ;
  int         n_streams  ;
  SmsStrNode  *snd_stream ;
} ;

/*
 *   The data from each Sound Management File is saved in an
 *   SMS_Node structure.  Each sound source and sound region
 *   will use the data from one of the SMS_Nodes.
 */

struct smsnode {
  struct smsnode *next ;  // Next in list "sms_list"  ("sms_list_beg").
  char         *name ;
  char         *sms_file  ;
  char         *sms_fullfilename ;
  int          needed ;
  int          loaded ;
  int          group ;           //  Group to use.
  SmsGrpNode   smsgrp_node[4] ;  //  Groups 5, 3, 1 and 0.
} ;

/*
 *   INDIVIDUAL SOUND SOURCE STRUCTURES
 *
 *   Each active sound source, i.e. a wagon, a signal, a level crossing,
 *   a section of rail etc. has a set of trigger structures.  This is
 *   a linked list of triggers which have been set but not processed.
 *
 *   It also has a linked list of sounds that have been started.
 *
 */

/*
 *   Sound trigger structure
 */

struct trglstnode {
  struct trglstnode  *next    ;
  int                 itrigger ;
} ;

/*
 *   Sound started structure
 */

struct actlstnode {
  struct actlstnode  *next    ;
  int                 isound ;
} ;


/*
 *  wave file structure
 */
struct wavfilenode{
    char chunk_id[4];
    int chunk_size;
    char format[4];
    char subchunk1_id[4];
    int subchunk1_size;
    short int audio_format;
    short int num_channels;
    int sample_rate;
    int byte_rate;
    short int block_align;
    short int bits_per_sample;
    short int extra_param_size;
    char subchunk2_id[4];
    int subchunk2_size;
} header;
