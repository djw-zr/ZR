/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  update_sounds2.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for processing OpenAL sound
 *
 *==============================================================================
 */

/*
 *==============================================================================
 * Routines to process train sounds
 *==============================================================================
 */
#ifdef OPENAL
int add_sound_pointers_to_wagon(RawWagonNode *rw) ;
int print_sms_node(RawWagonNode *rw);


int setup_train_sms_nodes(void){

  int  ip = 0 ;
  int  n, iret   ;
  TrainNode    *t  ;
  WagonNode    *w  ;
  RawWagonNode *rw ;
  SMS_Node     *sms_node ;
  char         *string1, *string2, *string3 ;
  char         *sms_name ;
  BTree        *btree ;
  char         *my_name = "setup_train_sms_nodes" ;

//      ip1 = ip + l_time_5s ;
      if(ip)printf("  ENTER routine %s\n",my_name) ;

      for(t=trainlist_beg; t!=NULL; t=t->next){
        for(w=t->first; w!=NULL; w=w->next){
          rw = w->raw_wagon ;
          if(rw->sms_node == NULL){
            if(ip){
              printf(" Train %s.  Wagon %s.  Filename %s. Soundfile %s\n",
                     t->name, rw->name, rw->file, rw->sound_file) ;
            }
            string1 = strdup(rw->sound_file) ;
            string2 = strip_quotes(string1)  ;       // string1 freed
            string1 = convert_dir_markers(string2) ; // string2 freed
            sms_name = zr_basename(string1) ;        // string1 untouched
            str2lc(sms_name) ;
            btree = find_btree(sms_master,sms_name) ;
/*
 *  If sound already loaded (for another wagon??) use btree pointer
 */
            if(btree){
              if(ip)printf("  Using existing sms_node for wagon\n") ;
              rw->sms_node = (SMS_Node *)btree->data ;
              free(sms_name) ;
              free(string1)  ;
            }else{
/*
 *  Search for wagon SMS file.  This may be
 *    (a)  In the Sound directory at the same level as the engine or wagon file
 *    (b)  Redirect relative to the Sound directory at the same level
 *    (c)  Redirect relative to a missing Sound directory at the same level.
 */
              if(ip)printf("  Search for wagon SMS file\n") ;
              string2 = zr_full_parentdir(rw->file) ;
              n = strlen(string2) + strlen(string1) + 8 ;
              string3 = (char *)malloc(n*sizeof(char)) ;
              strcpy(string3,string2) ;
/*
 *  Check for redirection
 */
              if(strncmp(string1,"../../",6)){
                strcat(string3,"/Sound/") ;     //  Add sound directory
                strcat(string3,string1) ;
              }else{
                strcat(string3,&string1[2]);    //  Skip initial ".."
              }
              if(ip){
                printf("  Routine %s\n",my_name) ;
                printf("    SMS full filename = %s\n",string3) ;
                printf("    string1 = %s\n",string1) ;
                printf("    string2 = %s\n",string2) ;
              }
/*
 *  Check file exists
 */
              iret = find_msstyle_file(&string3) ;
/*
 *  If not try top level sound directory
 */
              if(iret){
                free(string3) ;
                n = strlen(ORdir) + strlen("/Sound/") + strlen(string1) ;
                string3 = (char *)malloc(n*sizeof(char)) ;
                strcpy(string3,ORdir);
                strcat(string3,"/Sound/");
                strcat(string3,string1);
                printf("  Try filename :: %s\n",string3) ;
                iret = find_msstyle_file(&string3) ;
              }
/*
 *  If not try MSTS top level sound directory
 */
              if(iret && MSTSdir){
                free(string3) ;
                n = strlen(MSTSdir) + strlen("/SOUND/") + strlen(string1) ;
                string3 = (char *)malloc(n*sizeof(char)) ;
                strcpy(string3,MSTSdir);
                strcat(string3,"/SOUND/");
                strcat(string3,string1);
                printf("  Try filename :: %s\n",string3) ;
                iret = find_msstyle_file(&string3) ;
              }
/*
 *  Failure ...
 */
              if(iret){
                printf("  Unable to find SMS file %s\n",string1) ;
                continue ;
              }
/*
 *  Generate and fill new SMS node
 */
              if(ip)printf("  Create new sms_node for wagon\n") ;

              rw->sms_node = (SMS_Node *)malloc(sizeof(SMS_Node)) ;
              sms_master = insert_node(sms_master, sms_name, (void *)rw->sms_node) ;
              load_sound_sms_file(rw->sms_node,string3) ;
              set_sms_group(rw) ;
//              print_sms_node(rw) ;
              add_sound_pointers_to_wagon(rw) ;

              free(string1)  ;
              free(string2)  ;
            }
          }
        }
      }
      if(ip)printf("  EXIT routine %s\n",my_name) ;

      return 0;
}

int set_sms_group(RawWagonNode *rw){

  int         i, ierr ;
  SMS_Node    *sms_node ;
  SmsGrpNode  *smsgrp_node ;
  char        *my_name="set_sms_group" ;

      ierr = 1 ;
      sms_node = rw->sms_node ;
      for(i=0;i<4;i++){
        smsgrp_node = &(sms_node->smsgrp_node[i]) ;
        if(smsgrp_node->n_streams>0){
          sms_node->group = i ;
          ierr = 0 ;
          break ;
        }
      }
      if(ierr){
        printf("  Error in routine %s\n",my_name) ;
        printf("  Wagon %s\n",rw->name) ;
        printf("  No group found\n") ;
      }
      return 0 ;
}

int print_sms_node(RawWagonNode *rw){

  int       i ;
  int       group ;
  int       n_streams ;
  int       n_triggers ;
  SMS_Node  *sms_node ;
  SmsGrpNode *smsgrp_node ;

  char      *my_name = "print_sms_node" ;

      printf("  ENTER routine %s.  Wagon = %s\n",my_name,rw->name) ;
      sms_node = rw->sms_node ;
      group = sms_node->group ;
      printf("        group     = %i\n",group) ;
      smsgrp_node = &(sms_node->smsgrp_node[group]) ;
      n_streams = smsgrp_node->n_streams ;
      printf("        n_streams = %i\n",n_streams) ;
      for(i=0; i<n_streams; i++){
        n_triggers = smsgrp_node->snd_stream[i].n_triggers ;
        printf("        stream %i, n_triggers = %i\n",i,n_triggers) ;
      }


      return 0;

}

int add_sound_pointers_to_wagon(RawWagonNode *rw){

  int  ip = 0 ;
  int  i, j, k, l, n, iret ;
  int  n_streams,
       n_triggers,
       n_files  ;
  SMS_Node     *sms_node ;
  SmsGrpNode   *smsgrp_node ;
  SmsStrNode   *snd_stream ;
  SmsTrgNode   *smstrg_node ;
  SmsFilNode   *smsfil_node ;
  BTree        *btree ;
  SoundFile    *snd_file ;


  char         *wav_file ;
  char         *wav_name ;
  char         *sound_dir = "/Sound/" ;  //
  char         *parentdir = NULL ;;
  char         *train_dir  = "/Trains/Trainset/" ;
  char         *filename ;


  TrainNode    *t  ;
  WagonNode    *w  ;
  char         *string1, *string2, *string3 ;
  char         *sms_name ;
  char         *my_name = "add_sound_pointers_to_wagon" ;

      if(ip)printf("  ENTER routine %s\n",my_name) ;
/*
 *==============================================================================
 *  Check for existing errors
 *==============================================================================
 */
#if 0
      iret = alutGetError() ;
      if(iret){
        printf("  Error in routine %s\n",my_name) ;
        printf("    Existing alut error on entry\n") ;
        printf("    Error value  = %i\n",iret) ;
        printf("    Error string = %s\n",alutGetErrorString(iret)) ;
      }
#endif

      if(ip>1)dump_btree(wav_master,0,"0") ;

      sms_node = rw->sms_node ;
      for(i=0; i<4 ; i++){
        smsgrp_node = &(sms_node->smsgrp_node[i]) ;
        n_streams = smsgrp_node->n_streams ;
        if(n_streams>0)break ;
      }
      if(n_streams == 0){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  No stream found for wagon %s\n",rw->name) ;
        return 1 ;
      }
      sms_node->group = i ;
      if(ip)printf("  Routine %s, wagon %s, group = %i\n",
                           my_name, rw->name, i) ;
/*
 *==============================================================================
 * Generate parent of redirected sound (SMS) file
 *==============================================================================
 */
      string1 = strdup(rw->sound_file) ;
      string2 = strip_quotes(string1) ;
      string1 = convert_dir_markers(string2) ;
      parentdir = zr_full_parentdir(string1) ;
      free(string1) ; string1 = NULL ;

      for(j=0; j<n_streams; j++){
        snd_stream = &(smsgrp_node->snd_stream[j]) ;
        n_triggers = snd_stream->n_triggers ;
        for(k=0; k<n_triggers; k++){
          smstrg_node = &(snd_stream->smstrg_node[k]) ;
          n_files = smstrg_node->n_files ;
          for(l=0; l<n_files; l++){
            smsfil_node = &(smstrg_node->smsfil_node[l]);
/*
 *  Generate raw name
 */
            wav_file = smsfil_node->wav_file ;
            wav_name = zr_basename2(wav_file) ;
            if(ip)printf("  Search for wave file :: %s :: %s\n",wav_name, wav_file) ;
/*
 *==============================================================================
 *  Check if snd_file loaded
 *==============================================================================
 */
            btree = find_btree(wav_master,wav_name) ;
            if(btree && btree->data){
              smsfil_node->soundfile = (SoundFile *)btree->data ;
              snd_file = smsfil_node->soundfile ;
              if(ip)printf(" == snd_file node already created\n") ;
              if(ip)printf("     snd_file = %p\n",(void *)snd_file) ;
              if(ip && snd_file){
                printf("  name     = %s\n",snd_file->name) ;
                printf("  filename = %s\n",snd_file->filename) ;
              }
              free(wav_name) ;
/*
 *==============================================================================
 *  Create snd_file structure
 *  Load wave file
 *==============================================================================
 */
            }else{
              snd_file = (SoundFile *)malloc(sizeof(SoundFile));

              smsfil_node->soundfile = snd_file ;

              snd_file->name   = wav_name ;            //  Lower case name
              snd_file->format = 0 ;
              snd_file->data   = NULL ;
              snd_file->size   = 0 ;
              snd_file->freq   = 0 ;
              snd_file->loop   = 0 ;
              snd_file->buffer = 0 ;  //  Deleating buffer '0' is illegal in OpenAL

/*
 *  Generate full file name
 *  Version 1
 *    ORdir + "/Trains/Trainset/" + Wagon directory name + "/Sound/" + wav_file
 *  Version 2
 *    SMS parent directory + wave file
 *  To be done - look in MSTS sound + train sound directory
 */
#if 0
              string1 = zr_full_parentdir(rw->sms_node->sms_fullfilename) ;
              if(ip){
                printf("  SMS file   = %s\n",rw->sms_node->sms_file) ;
                printf("  SMS full   = %s\n",rw->sms_node->sms_fullfilename) ;
                printf("  wave file  = %s\n",wav_file) ;
                printf("  string1    = %s\n\n",string1) ;
              }
              n = strlen(string1) + strlen(wav_file) + 2 ;
              string2 = (char *)malloc(n*sizeof(char)) ;
              strcpy(string2,string1) ;
              printf("  AA string2  = %s\n",string2) ;
              strcat(string2,"/") ;
              printf("  BB string2  = %s\n",string2) ;
              strcat(string2,wav_file) ;
              printf("  CC string2  = %s\n",string2) ;
              string2 = strip_dot_dirs(string2) ;
              printf("  DD string2  = %s\n\n",string2) ;
              filename = snd_file->filename = string2 ;
              if(ip){
                printf("  SMS file   = %s\n",rw->sms_node->sms_file) ;
                printf("  SMS full   = %s\n",rw->sms_node->sms_fullfilename) ;
                printf("  wave file  = %s\n",wav_file) ;
                printf("  SMS parent = %s\n",string1) ;
                printf("  string2    = %s\n",string2) ;
                printf("  full wave filename  = %s\n",filename) ;
              }
              string1 = NULL ;
#else
              filename = snd_file->filename
                       = find_wagon_wave_file(rw, wav_file) ;
              if(filename == NULL) continue ;
#endif
/*
 *==============================================================================
 *  Load wave file :: filename
 *==============================================================================
 */
ALvoid  *wav_data = NULL  ;
char    *cwav_data = NULL ;
ALsizei z1, z2, z3, z4   ;

ALenum  wav_format ;
ALsizei wav_size   ;
ALsizei i_wav_freq ;
ALfloat wav_freq   ;

              alGenBuffers(1,&(snd_file->buffer)) ;
              iret = alGetError() ;
              if(ip)printf("  alGenBuffers :: iret = %i, %s\n",iret, al_error_code(iret)) ;
#if 0
              wav_data = alutLoadMemoryFromFile(filename, &wav_format, &wav_size, &wav_freq) ;

              iret = alutGetError() ;
              printf("  alutLoadMemoryFromFile :: iret = %i\n",iret) ;
              if(iret){
                printf("  Error in routine %s\n",my_name) ;
                printf("    alutCreateBufferFromFile() error\n") ;
                printf("      Error value  = %i\n",iret) ;
                printf("      Error string = %s\n",alutGetErrorString(iret)) ;
                printf("      Raw Wagon    = %s\n",rw->name) ;
                printf("      *.wav filename      = %s\n",wav_file) ;
                printf("      *.wav full filename = %s\n",filename) ;
                printf("      snd_file->buffer    = %i\n",snd_file->buffer) ;
              }

//            snd_file->buffer = alutCreateBufferFromFile(filename) ;
#else
              if(ip){
                printf("    cwav_data   &cwav_data   = %p  %p\n",(void *)cwav_data,(void *)&cwav_data) ;

                printf("  call load_wave_file :: Filename = %s\n",filename) ;
              }
                iret = load_wave_file(filename, &cwav_data, &wav_format, &wav_size, &i_wav_freq) ;
              if(ip){
                printf("  return from load_wave_file :: iret = %i\n",iret) ;
                printf("    data              = %p\n",(void *)cwav_data) ;
              }
              wav_freq = i_wav_freq ;
              wav_data = cwav_data ;
#endif
/*
 *  Skip sounds where there is no good data
 */
              if(iret) continue ;
/*
 *  Otherwise load the data into the buffer
 */
              if(ip)printf("  buffer, data, format, size, freq = %i %p %i %i %f\n",
                       snd_file->buffer, (void *)wav_data, wav_format, wav_size, wav_freq) ;

              alBufferData(snd_file->buffer, wav_format, wav_data, wav_size, wav_freq) ;
              iret = alGetError() ;
              if(ip)printf("  alBufferData :: iret = %i, %s\n",iret, al_error_code(iret)) ;
              if(iret){
                printf("  buffer, data, format, size, freq = %i %p %i %i %f\n",
                       snd_file->buffer, (void *)wav_data, wav_format, wav_size, wav_freq) ;
              }

              if(ip){
int i_size, i_freq, i_bits, i_chan;

                alGetBufferi(snd_file->buffer, AL_FREQUENCY, &i_freq ) ;
                alGetBufferi(snd_file->buffer, AL_SIZE, &i_size ) ;
                alGetBufferi(snd_file->buffer, AL_BITS, &i_bits ) ;
                alGetBufferi(snd_file->buffer, AL_CHANNELS, &i_chan ) ;
                printf("\n   Wave File    = %s\n",wav_file) ;
                printf("     AL_FREQUENCY = %i\n",i_freq) ;
                printf("     AL_SIZE      = %i\n",i_size) ;
                printf("     AL_BITS      = %i\n",i_bits) ;
                printf("     AL_CHANNELS  = %i\n",i_chan) ;
                printf("\n") ;
              }

              if(btree){
                btree->data = (void *)snd_file ;
              }else{
                wav_master = insert_node(wav_master, wav_name, snd_file) ;
              }
              if(ip){
                printf(" == snd_file node created/updated\n") ;
                printf("    name     = %s\n", wav_name) ;
                printf("    wav_file = %s\n", filename) ;
              }
              free(wav_data) ;
            }
          }
        }
      }
      if(parentdir) free(parentdir) ;

      if(ip)printf("  EXIT routine %s\n",my_name) ;
      return 0;
}

int initialise_wagon_openal_variables(){

  int  ip = 0 ;
  int  i, n, ierr, n_sources ;
  TrainNode     *t  = NULL ;
  WagonNode     *w  = NULL ;
  RawWagonNode  *rw = NULL  ;
  SMS_Node      *sms_node ;
  SmsGrpNode    *smsgrp_node ;
  char          *my_name = "initialise_wagon_openal_variables" ;

      if(ip)printf("  ENTER routine my_name %s\n",my_name) ;
      ierr = alGetError() ;
      if(ierr || ip)printf("  alGenSources :: error = %i :: %i %i %i\n",
                           ierr, AL_OUT_OF_MEMORY, AL_INVALID_VALUE, AL_INVALID_OPERATION) ;

      for(t=trainlist_beg; t != NULL; t=t->next){
        for(w=t->first; w != NULL; w=w->next){
          rw = w->raw_wagon ;
          if(ip)printf("  Routine %s,  wagon = %s\n",my_name, rw->name) ;

          sms_node = rw->sms_node ;
          if(!sms_node){
            printf("  Routine %s error\n",my_name) ;
            printf("    Wagon %s has no sound file defined\n",rw->name) ;
            continue ;
          }
          if(ip)printf("  Routine %s :: FF :: sms_node = %p\n",
                      my_name, (void *)sms_node)  ;
          n = sms_node->group ;
          w->smsgrp_node =
             smsgrp_node = &(sms_node->smsgrp_node[n]) ;
          if(ip)printf("  SMS group = %i,  smsgrp_node = %p\n",
                                n,(void *)smsgrp_node) ;
          w->n_sources =
             n_sources = smsgrp_node->n_streams ;
          if(ip)printf("      n_streams = %i\n",n_sources) ;
          w->snd_stream = (SmsStrNode *)malloc(n_sources*sizeof(SmsStrNode)) ;
          w->source     = (ALuint *)malloc(n_sources*sizeof(ALuint)) ;
          w->source_on  = (int *)malloc(n_sources*sizeof(int)) ;
          w->sound_on   = (int *)malloc(n_sources*sizeof(int)) ;
          w->cue_points = (int *)malloc(n_sources*sizeof(int)) ;
          w->cue_in     = (int *)malloc(n_sources*sizeof(int)) ;
          w->cue_out    = (int *)malloc(n_sources*sizeof(int)) ;
          w->sound_gain = (double *)malloc(n_sources*sizeof(double)) ;
          w->wav_file   = (char **)malloc(n_sources*sizeof(char *)) ;
          for(i=0; i<n_sources; i++){
            w->snd_stream[i] = smsgrp_node->snd_stream[i] ;
            w->source[i]     = 0 ;
            w->source_on[i]  = 0 ;
            w->sound_on[i]   = 0 ;
            w->cue_points[i] = 0 ;
            w->cue_in[i]     = 0 ;
            w->cue_out[i]    = 0 ;
            w->sound_gain[i] = 1.0 ;
            w->wav_file[i]   = NULL ;

            if(ip){
              printf("    stream = %i %i, snd_stream = %p\n",i,n_sources,
                                            (void *)&(w->snd_stream[i])) ;
              printf("    source    = %p\n",(void *)&(w->source)) ;
              printf("    source[i] = %p\n",(void *)&(w->source[i])) ;
              printf("  Routine %s,  call alGenSources()  %p\n",
                               my_name, (void *)&(w->source[i])) ;
            }

            alGenSources(1,&(w->source[i]) ) ;

            if(ip)printf("  Routine %s,  new source  = %i\n",
                               my_name,w->source[i]) ;
            ierr = alGetError() ;
            if(ierr || ip)printf("  alGenSources :: error = %i :: %i %i %i\n",
                               ierr, AL_OUT_OF_MEMORY, AL_INVALID_VALUE, AL_INVALID_OPERATION) ;
          }
//          if(ip)printf("  Routine %s,  call alGenSources()\n",my_name) ;
//          alGenSources(n_sources,w->source) ;
//          ierr = alGetError() ;
//          if(ierr || ip)printf("  alGenSources :: error = %i :: %i %i %i\n",
//                               ierr, AL_OUT_OF_MEMORY, AL_INVALID_VALUE,
//                                     AL_INVALID_OPERATION) ;
          if(ip){
            printf("  Train %s, Wagon %-24.24s",t->name,w->name) ;
            for(i=0; i<n_sources; i++) printf(" %i",w->source[i]) ;
            printf("\n") ;
          }
        }
      }
      if(ip)printf("  EXIT routine my_name %s\n",my_name) ;
      return 0 ;
}


/*
 *==============================================================================
 *   Initialise train sounds
 *   Called during startup
 *==============================================================================
 */

int initial_train_sounds(void){

  int  ip = 0;
  int  i, j, k, l, n, iret ;
  int  i_source,  n_sources  ;
  int  i_trigger, n_triggers ;
  int  tile_x, tile_y ;
  double   X, Y, Z, x, y, z ;
  double   distance ;
  ALfloat  s_x[3], t_x[3] ;

  TrainNode     *t  ;
  WagonNode     *w  ;
  RawWagonNode  *rw ;
  TravellerNode *tv ;
  SMS_Node      *sms_node ;
  SmsGrpNode    *smsgrp_node = NULL ;
  SmsStrNode    *smsstr_node = NULL ;
  SmsTrgNode    *smstrg_node = NULL ;
  SmsFilNode      *smsfil_node ;
  SoundObjectNode *sndobj_node ;

  ALint        source_state ;
  ALuint       source ;
  ALuint       buffer ;
  char         *my_name = "initial_train_sounds" ;

      if(ip)printf("\n  ENTER routine %s\n\n",my_name) ;
/*
 *==============================================================================
 *  Set position of listener
 *==============================================================================
 */
      x = lookat_eye_x*tile_size ;
      y = lookat_eye_y*tile_size ;
      z = lookat_eye_z*tile_size + tile_h0 ;

      alListener3f(AL_POSITION, (float) x, (float) y, (float) z) ;
      alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f) ;
/*
 *==============================================================================
 *  Loop over trains and wagons
 *==============================================================================
 */
      for(t=trainlist_beg; t!=NULL; t=t->next){
        for(w=t->first; w!=NULL; w=w->next){
          rw = w->raw_wagon ;
          if(ip){
            printf("AA :: train = %p :: %p\n",(void *)t, (void *)w->train) ;
            printf("   :: train name = %s\n",w->train->name) ;
            printf("   :: wagon name = %s\n",rw->name) ;
          }
/*
 *==============================================================================
 *  Check OpenAL variables set
 *==============================================================================
 */
          sms_node = rw->sms_node ;
          if(ip)printf("CC\n") ;
          if(sms_node == NULL){
            printf("  ERROR in routine %s\n",my_name) ;
            printf("    sms_node not initialised\n") ;
            printf(" Train %s.  Raw wagon %s.  Filename %s. Soundfile %s\n",
                     t->name, rw->name, rw->file, rw->sound_file) ;
            continue ;
          }
          if(ip)printf("DD  sms_node = %p\n",(void *)sms_node) ;
          smsgrp_node = w->smsgrp_node ;
          if(ip)printf("EE  smsgrp_node = %p\n",(void *)smsgrp_node) ;
          if(!smsgrp_node){
            printf("  ERROR in routine %s\n",my_name) ;
            printf("    wagon smsgrp_node not set\n") ;
            printf(" Train %s.  Raw wagonagon %s.  Filename %s. Soundfile %s\n",
                     t->name, rw->name, rw->file, rw->sound_file) ;
            continue ;
          }
          n_sources = w->n_sources ;
          if(ip)printf("EE  n_sources = %i\n",n_sources) ;
/*
 *==============================================================================
 *  Initialise OpenAL sources for this wagon
 *==============================================================================
 */
          tv = w->traveller ;
          X = tv->rx ;
          Y = tv->ry ;
          Z = tv->rz ;
          distance = sqrt( (x-X)*(x-X) + (y-Y)*(y-Y) +(z-Z)*(z-Z)) ;
          if(ip){
            printf("  distance = %f\n",distance ) ;
            printf("  traveller x  = %f, y  = %f, z  = %f\n", tv->x, tv->y, tv->z) ;
            printf("  traveller rx = %f, ry = %f, rz = %f\n", tv->rx, tv->ry, tv->rz) ;
            printf("  lookat    x  = %f, y  = %f, z  = %f\n", x, y, z ) ;
          }
//          X=x; Y=y; Z=z ;
          for(i_source=0; i_source<n_sources; i_source++){
            source = w->source[i_source] ;
            smsstr_node = &(w->snd_stream[i_source]) ;
            if(ip)printf("  i_source = %i, source = %i, snd_stream = %p\n",
                         i_source, source, (void *)smsstr_node) ;

            alSourcef(source,  AL_PITCH, 1.0f);
            alSourcef(source,  AL_GAIN,  1.0f);
            alSource3f(source, AL_POSITION,  X, Y, Z);
            alSource3f(source, AL_VELOCITY,  0.0f, 0.0f, 0.0f);
            alSourcei(source,  AL_LOOPING, AL_TRUE);
/*
 *==============================================================================
 *   Distances for world/tile sounds
 *   Distances (X, Y, Z) measured in m
 *   Rolloff ~ I/(number of rolloff distances to halve gain)
 *   Half gain distance ~ Ref Dist/RollOff   8 m and 0.16  ~> 50 m
 *                                           8 m and 0.32  ~> 25 m
  *   Sound is consant beyond Max Distance
 *==============================================================================
 */

            alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED) ;
            alSourcef(source, AL_REFERENCE_DISTANCE,   8.0f);
            alSourcef(source, AL_MAX_GAIN,             1.0f);
            alSourcef(source, AL_MAX_DISTANCE,      2000.0f);
            alSourcef(source, AL_ROLLOFF_FACTOR,      0.32f);
            iret = alGetError() ;
            if(iret){
              printf("  Routine %s error when initialising sources",my_name) ;
              printf("          error = %i : %s\n",iret,al_error_code(iret)) ;
            }
/*
 *==============================================================================
 * Loop over triggers
 *==============================================================================
 */
            n_triggers = smsstr_node->n_triggers ;
            if(ip)printf("  HH  n_triggers = %i\n",n_triggers) ;
            for(i_trigger=0; i_trigger<n_triggers; i_trigger++){
              if(ip)printf("  II  trigger = %i",i_trigger) ;
              smstrg_node = &(smsstr_node->smstrg_node[i_trigger]) ;
              if(ip)printf("  trigger type = %i %i\n",
                     smstrg_node->trigger_type,
                     smstrg_node->trigger_disabled) ;
              if(smstrg_node->trigger_type != SND_TRG_INITIAL) continue ;
              if(ip)printf("  Initial trigger at source/stream %i, trigger %i\n",
                i_source, i_trigger) ;
/*
 *  Process initial trigger
 */
              if(ip)printf("  === AAA smstrg_node = %p\n",(void *)smstrg_node) ;
              if(ip)printf("  === BBB sequential  = %i\n",smstrg_node->sequential) ;
              if(smstrg_node->sequential){
                if(ip)printf("  === CCC\n") ;
                smsfil_node = NULL ;
                for(j=0; j<4; j++){
                  if(ip)printf("  === DDD %i\n",j) ;
                  if(smstrg_node->smsfil_node[j].soundfile){
                    smsfil_node = &(smstrg_node->smsfil_node[j]) ;
                    break ;
                  }
                }
                if(smsfil_node ==NULL){
                  printf("   ERROR in Routine %s ::  smsfil_node not found\n",my_name) ;
                }
                if(ip)printf("  === AAA smsfil_node = %p\n",(void *)smsfil_node) ;
                if(ip)printf("  === AAA  = %p\n",(void *)smsfil_node->soundfile) ;
                buffer = smsfil_node->soundfile->buffer ;
                if(ip)printf(" LL  buffer  = %i\n", buffer ) ;
                if(ip)printf(" LL  source  = %i\n", source ) ;

                if(buffer == 0){
#if 0
                  buffer = alutCreateBufferHelloWorld() ;
                  iret = alutGetError() ;
                  if(ip)printf("    Create buffer %i using"
                               " alutCreateBufferHelloWorld."
                         "Alut Error = %s\n", source, alutGetErrorString(iret)) ;
                  smsfil_node->soundfile->buffer = buffer ;
#else
                  printf("  Routine %s.  Buffer missing for SMS %s, Stream %i, Trigger %i, wav file %s\n",
                         my_name, sms_node->sms_file, i_source, i_trigger, smsfil_node->wav_file) ;
                  continue ;
#endif
                }
                alSourcei(source, AL_BUFFER, buffer) ;
                if(ip)printf(" AA al error = %s\n", alGetErrorString() ) ;
                alSourcePlay(source) ;
                if(ip)printf(" BB al error = %s\n", alGetErrorString() ) ;
                alGetSourcei(source, AL_SOURCE_STATE, &source_state);
                if(ip)printf(" CC al error = %s\n", alGetErrorString() ) ;
#if 0
                if(ip)printf("    Start playing  :: final source_state = %s, alut error = %s\n",
                       alStateMessage(source_state), alutGetErrorString(alutGetError()) ) ;
#else
                if(ip)printf("    Start playing  :: final source_state = %s\n",
                       alStateMessage(source_state) ) ;
#endif
/*
 *  Set flag
 */
                w->source_on[i_source] = 1 ;
/*
 *  Most wagins do not have an initial sound
 */
              }else{
                if(ip){
                  printf("  Error in routine %s\n",my_name) ;
                  printf("    Selection method not sequential\n") ;
                }
              }
            }
            if(0){
ALfloat  gain ;
ALfloat  min_gain ;
ALfloat  max_gain ;
ALfloat  max_dist ;
ALfloat  ref_dist ;
ALfloat  rolloff  ;
              alGetSourcei(source, AL_SOURCE_STATE, &source_state);
              if(source_state == AL_PLAYING){
                alGetSourcef(source, AL_GAIN, &gain);
                alGetSourcef(source, AL_MIN_GAIN, &min_gain);
                alGetSourcef(source, AL_MAX_GAIN, &max_gain);
                alGetSourcef(source, AL_MAX_DISTANCE, &max_dist);
                alGetSourcef(source, AL_REFERENCE_DISTANCE, &ref_dist);
                alGetSourcef(source, AL_ROLLOFF_FACTOR, &rolloff);

                printf("  Train.  sms_name %s, distance %f :: %f %f %f %f %f %f\n",
                       sms_node->name, distance,gain, min_gain, max_gain, max_dist, ref_dist, rolloff) ;
              }
            }
          }
        }
      }

      return 0;
}


#endif

/*
 *==============================================================================
 *
 *  Routines for processing sound triggers
 *
 *  At present it deals with wagons only.
 *
 *==============================================================================
 */

/*
 *   Routine to add one sound trigger to a numerically ordered list
 *   Do nothing if trigger it already set
 */

int print_trigger_list(TrgLstNode **s0){

  int         ip = 0 ;
  TrgLstNode *ss, *s1, *sn ;
  char        *my_name = "print_trigger_list" ;

      if(ip) printf("    Enter routine: %s\n", my_name) ;

      s1 = *s0 ;;
      while(s1){
        printf("      s1 = %p, s1->next %p,Trigger = %i\n",
                               (void *)s1,(void *)s1->next,s1->itrigger) ;
        s1 = s1->next ;
      }
      if(ip) printf("    Exit routine: %s\n", my_name) ;

      return 0 ;
}

int add_snd_trigger_to_list(TrgLstNode **s0, int itrigger){

  int         ip = 0 ;
  TrgLstNode *ss, *s1, *sn ;
  char        *my_name = "add_snd_trigger_to_list" ;

      if(ip) printf("  Enter routine: %s\n", my_name) ;

      if(ip)printf("  AA s0 = %p,  itrigger = %i\n",(void *)s0, itrigger) ;
      if(ip)print_trigger_list(s0) ;

      sn = (TrgLstNode *)malloc(sizeof(TrgLstNode)) ;
      sn->next = NULL ;
      sn->itrigger = itrigger ;
/*
 *  No existing list
 */
      if(*s0 == NULL){
        *s0 = sn ;
        if(ip)printf("  BB s0  = %p\n",(void *)s0) ;
        if(ip)printf("  BB *s0 = %p\n",(void *)s0) ;
        if(ip)printf("  BB (*s0)->itrigger = %i\n", (*s0)->itrigger) ;
/*
 *  First element of list matches or is greater than itrigger
 */
      }else if((*s0)->itrigger == itrigger) {
        if(ip)printf("      New Trigger already included\n") ;

      }else if((*s0)->itrigger > itrigger){
        if(ip)printf("      CC  Old trigger %i, New trigger %i\n",
               (*s0)->itrigger, itrigger) ;
        sn->next = *s0  ;
        *s0      = sn   ;
/*
 * Search for end of list or posiiton before larger itrigger
 */
      }else{
        if(ip)printf("      DD  Old trigger %i, New trigger %i\n",
               (*s0)->itrigger, itrigger) ;
        s1 = (*s0) ;
        for(ss = s1->next; ss != NULL; ss = ss->next){
          if(ip)printf("      EE  Old trigger %i, New trigger %i\n",
               ss->itrigger, itrigger) ;
          if(ss->itrigger == itrigger) return 0 ;
          if(ss->itrigger >  itrigger) break ;
          s1 = ss ;
        }
        sn->next = ss ;
        s1->next = sn ;
      }
      if(ip)print_trigger_list(s0) ;
      if(ip) printf("  Exit routine: %s\n", my_name) ;
      return 0 ;
}



/*
 *  Routine to remove a sound trigger from a wagon's ordered list
 */

int remove_snd_trigger_from_list(TrgLstNode **s0, int itrigger){

  int         ip = 0 ;
  TrgLstNode *ss, *s1 ;
  char        *my_name = "remove_snd_trigger_from_list" ;

      if(ip) printf("  Enter routine: %s\n", my_name) ;
/*
 *  No list
 */
      if(s0 == NULL) return 0 ;
/*
 *  First in list
 */
      ss = *s0 ;
      if(ss->itrigger == itrigger){
        *s0 = ss->next ;
        free(ss) ;
        return 0 ;
      }
/*
 *  Longer list
 */
      s1 = *s0 ;
      for(ss = s1; ss != 0 ; ss = ss->next){
        if(ss->itrigger == itrigger){
          s1 = ss->next ;
          free(ss) ;
          return 0 ;
        }
        s1 = ss ;
      }
/*
 *  Not in list
 */
      return 0 ;
}

int add_trigger_to_motor(TrainNode *t, int trigger){

      add_snd_trigger_to_list(&(t->motor->snd_trigger), trigger) ;
      return 0 ;
}

int clear_wagon_sound_trigger_list(WagonNode *w){

  int  ip = 0 ;
  TrgLstNode *tr1, *tr2 ;
  char *my_name="clear_wagon_sound_trigger_list" ;

      if(!w || !(w->snd_trigger)) return 0 ;
      if(ip)printf("  Enter routine %s\n",my_name) ;
      tr1 = w->snd_trigger ;
      while(tr1){
        if(ip)printf("  w->snd_trigger = %p\n", (void *)tr1) ;
        tr2 = tr1->next ;
        free(tr1) ;
        tr1 = tr2 ;
      }
      w->snd_trigger = NULL ;
      if(ip)printf("  Eexit routine %s\n",my_name) ;
      return 0 ;
}
