/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  update_train sounds.c
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
 * Routine to update train sounds
 *==============================================================================
 */


#ifdef OPENAL



int update_train_sounds(void){

  int  ip = 0;
  int  ip1 = 0,
       ip2 = 0;          //  Print summary of changes
  int  i, j, k, iret, idata ;
  static int   first_pass = 1;
  static int   sms_error  = 0;
  int          n_sources  = 0 ,
               n_triggers = 0 ;
  int          action     = 0 ;
  ALuint       source     = 0 ;
  ALint        source_state ;
  TrainNode    *t  = NULL ;
  WagonNode    *w  = NULL ;
  RawWagonNode *rw = NULL ;
  SMS_Node     *sms_node = NULL ;
  BTree        *btree = NULL ;
  SmsStrNode   *snd_stream = NULL ;
  SmsTrgNode   *trg = NULL ;
  SmsFilNode   *smsfil_node = NULL ;
  SmsVolNode   *smsvol_node = NULL ;
  SmsFrqNode   *smsfrq_node = NULL ;
  ALuint       buffer = 0 ;
  ALfloat      gain ;

  double       dd = 0.0f,
               d1 = 0.0f,
               d2 = 0.0f ;
  double       x, y, z, X, Y, Z, U, V, W ;
  double       distance, speed, speed_l ;
  char         *trigger_string = NULL ;
  char         *action_string = NULL ;
  char         buffer_string[12] ;
  char         speed_string[12] ;
  char         string128[128] ;
  char         *my_name = "update_train_sounds" ;

      ip1 = ip && l_time_5s ;
      if(ip1)printf("  Enter routine %s\n",my_name) ;

      x = lookat_eye_x*tile_size ;
      y = lookat_eye_y*tile_size ;
      z = lookat_eye_z*tile_size + tile_h0 ;

      for(t=trainlist_beg; t!=NULL; t=t->next){

        if(ip1){
          printf("    Train speed      = %f\n",t->speed) ;
          printf("    Train last speed = %f\n",t->last_speed) ;
        }
        sprintf(speed_string," :: S %5.1f",t->speed) ;

        for(w=t->first; w!=NULL; w=w->next){
          rw = w->raw_wagon ;


          if(ip1)printf("  Train %s :: Wagon %s\n",t->name, w->name) ;
          sms_node = rw->sms_node ;
          if(sms_node == NULL){
            if(sms_error++<10){      //  Error
              printf("  ERROR in routine %s\n",my_name) ;
              printf("    sms_node unset for wagon %s\n", rw->name) ;
            }
            continue ;
          }
          n_sources = w->n_sources ;
          for(i=0; i<n_sources; i++){
//            if(i!=1) continue;
            source = w->source[i] ;
            snd_stream = &(w->snd_stream[i]) ;
            n_triggers = snd_stream->n_triggers ;
            for(j=0; j<n_triggers; j++){
//              if(j>0) break;
              trg = &(snd_stream->smstrg_node[j]) ;
/*
 *==============================================================================
 *  Triggers
 *==============================================================================
 */
              action = 0 ;
              trigger_string = NULL ;
              action_string = NULL ;
              sprintf(buffer_string,"    ");
              if(ip1){
                printf("   source = %i : %i, trigger = %i\n",i,(int)source,j) ;
                printf("     trigger_type     = %i\n",trg->trigger_type) ;
                printf("     speed_inc_past   = %i\n",trg->speed_inc_past) ;
                printf("     speed_dec_past   = %i\n",trg->speed_dec_past) ;
                printf("     value            = %f\n",trg->var_value) ;
              }
/*
 *==============================================================================
 *  Variable Trigger
 *==============================================================================
 */
              switch(trg->trigger_type) {
  TrgLstNode *tl ;
                case SND_TRG_INITIAL:
                  trigger_string = "InitialTrigger" ;
                  action = first_pass ;
                  break ;
                case SND_TRG_VARIABLE:
                if(trg->speed_inc_past || trg->variable1_inc_past ||
                   trg->variable2_inc_past || trg->variable3_inc_past ){
                  speed   = t->     speed ; //  Triggers include negative speeds.
                  speed_l = t->last_speed ;
                  if(trg->variable1_inc_past){
                    speed = speed/2.0 ; speed_l = speed_l/2.0 ;
                  }else if(trg->variable2_inc_past){
                    speed = speed ; speed_l = speed_l;
                  }else if(trg->variable3_inc_past){
                    speed = speed/2.0 ; speed_l = speed_l/2.0 ;
                    break ;  //  Not sure what this represents
                  }
                  action = speed_l <  trg->var_value &&
                           speed   >= trg->var_value ;
                  trigger_string = "VariableTrigger -> SpeedIncPast" ;
                  if(ip1){
                    printf("  AA  %i :: %i :: %i\n", t->last_speed <  trg->var_value, t->speed      >= trg->var_value, action) ;
                  }
                }else if(trg->speed_dec_past || trg->variable1_dec_past ||
                   trg->variable2_dec_past || trg->variable3_dec_past){
                  speed   = t->     speed ;
                  speed_l = t->last_speed ;
                  if(trg->variable1_dec_past){
                    speed = speed/2.0 ; speed_l = speed_l/2.0 ;
                  }else if(trg->variable2_dec_past){
                    speed = speed/2.0 ; speed_l = speed_l/2.0 ;
                  }else if(trg->variable3_dec_past){
                    speed = speed/2.0 ; speed_l = speed_l/2.0 ;
                    break ;  //  Not sure what this represents
                  }
                  action = speed_l >= trg->var_value &&
                           speed   <  trg->var_value ;
                  trigger_string = "VariableTrigger -> SpeedDecPast" ;
                  if(ip1){
                    printf("  AA  %i :: %i :: %i\n", t->last_speed >=  trg->var_value, t->speed      < trg->var_value, action) ;
                  }
                }
                break ;
                case SND_TRG_DISCRETE:
                  for(tl=w->snd_trigger; tl != NULL; tl=tl->next){
                    if(ip)printf("  Descrete trigger  %i :: %i\n",
                           tl->itrigger, trg->i_discrete_trigger) ;
                    if(tl->itrigger == trg->i_discrete_trigger){
                      action = 1;
                      remove_snd_trigger_from_list(&(w->snd_trigger), tl->itrigger) ;
                      trigger_string = "DiscreteTrigger " ;
                    break;
                    }
                  }
                  break ;
                default:
                  break ;
              }
/*
 *==============================================================================
 *  Volume
 *==============================================================================
 */
//  Basic Gain
              gain = snd_stream->volume ;
//  Random component
              if(trg->volume_max != 0.0){
                 dd = (double)rand()/RAND_MAX ;
                 d1 = trg->volume_min + dd*
                     (trg->volume_max - trg->volume_min) ;
                 gain =gain*d1 ;
              }
              w->sound_gain[i] = gain ;
//  Volume curve
              smsvol_node = &snd_stream->smsvol_node;
              if(smsvol_node){
                if(smsvol_node->speed_control){
                  d2 = interpolate_array(smsvol_node->n_points,t->speed,
                         smsvol_node->xxx_array, smsvol_node->vol_array);
                  gain = gain*d2 ;
                }
              }
              iret = alGetError() ;  // Clear errors
              alSourcef(source, AL_GAIN, gain) ;
              iret = alGetError() ;
              if(iret != AL_NO_ERROR){
                printf("  ERROR AL_GAIN in routine %s\n",my_name) ;
                printf("    Train = %s\n",t->name) ;
                printf("    Wagon = %s\n",w->name) ;
                printf("    Error code = %i :: %s\n",
                                  iret,al_error_code(iret)) ;
                printf("    Source = %i\n",(int)source) ;
                printf("    gain   = %f : %f %f %f : %f\n",(double)gain, dd, d1, d2, t->speed) ;
                printf("    array :: ") ;
                for(k=0; k<smsvol_node->n_points; k++){
                  printf(" : %i, %f %f",k, smsvol_node->xxx_array[k], smsvol_node->xxx_array[k]) ;
                }
                printf("\n") ;
              }
/*
 *==============================================================================
 *  Actions
 *==============================================================================
 */
//              if(ip1)printf("   action         = %i\n",action) ;

              if(action == 0) continue;
              if(ip1){
                printf("  Routine %s.  Train %s :: Wagon %s  :: Source %i :: Trigger %i %s :: Speed %f %f :: AL Source %i\n",
                my_name,t->name, w->name, i, j, trigger_string, t->speed, t->last_speed, (int)source);
                printf("   start_loop      = %i\n",trg->start_loop) ;
                printf("   release_loop    = %i\n",trg->release_loop) ;
              }
              if(trg->start_loop || trg->start_loop_release || trg->play_one_shot){

                if(trg->sequential){
                  if(trg->n_files != 1  && 0){   //  Code to be written
                    printf("  Routine %s ERROR.  Train %s, Wagon %s\n",my_name,t->name, w->name) ;
                    printf("    Sequential selection but number of files = %i\n",trg->n_files) ;
                  }
                  smsfil_node = &(trg->smsfil_node[0]) ;
                }else if(trg->random){
int nn ;
                  if(trg->n_files < 2 && 0){
                    printf("  Routine %s ERROR.  Train %s, Wagon %s\n",my_name, t->name, w->name) ;
                    printf("    Random selection but number of files = %i\n",trg->n_files) ;
                  }
                  nn = (double)trg->n_files*(double)rand()/(double)RAND_MAX ;
                  if(nn > trg->n_files)nn = trg->n_files ;
                  smsfil_node = &(trg->smsfil_node[nn]) ;
                }

                action_string = " StartLoop   :: " ;
                buffer = smsfil_node->soundfile->buffer ;
                sprintf(buffer_string,"%4i",buffer) ;
                if(ip)printf("  Turn source on.   Train %s, Wagon %s, Source %i : %i, Trigger %i, Buffer %i\n",
                          t->name, w->name, i, (int)source, j, buffer) ;
                if(ip)printf("   smsfil_node = %p\n",
                                  (void *)smsfil_node) ;
                if(ip)printf("   soundfile   = %p\n",
                                  (void *)smsfil_node->soundfile) ;
                if(ip)printf("   wave file   = %s\n",
                                          smsfil_node->wav_file) ;
/*
 *  Stop Source if source playing
 */
                alGetSourcei(source, AL_SOURCE_STATE, &source_state);
                if(source_state == AL_PLAYING){
                  alSourceStop(source) ;
                  iret = alGetError() ;
                  if(iret != AL_NO_ERROR){
                    printf("  ERROR 1 in routine %s\n",my_name) ;
                    printf("    Train = %s\n",t->name) ;
                    printf("    Wagon = %s\n",w->name) ;
                    printf("    Error code = %i :: %s\n",
                                      iret,al_error_code(iret)) ;
                    printf("    Source = %i\n",(int)source) ;
                  }
                  w->wav_file[i] = NULL ;
                }
/*
 *  Add new buffer to source
 */
                if(buffer){
                  alSourcei(source,AL_BUFFER,buffer) ;
                  iret = alGetError() ;
                  if(iret != AL_NO_ERROR){
                    printf("  ERROR 2 in routine %s\n",my_name) ;
                    printf("    Train = %s\n",t->name) ;
                    printf("    Wagon = %s\n",w->name) ;
                    printf("    Error code = %i :: %s\n",
                                      iret,al_error_code(iret)) ;
                    printf("    Source = %i\n",(int)source) ;
                    printf("    Buffer = %i\n",(int)buffer) ;
                  }
/*
 *  Play buffer once - or loop
 */
                  if(trg->play_one_shot){
                    alSourcei(source,  AL_LOOPING, AL_FALSE);
                  }else{
                    alSourcei(source,  AL_LOOPING, AL_TRUE);
                  }
/*
 *  Start source playing
 */
                  alSourcePlay(source) ;
                  w->source_on[i] = 1 ;
                  w->wav_file[i] = smsfil_node->wav_file ;
                  iret = alGetError() ;
                  if(iret != AL_NO_ERROR){
                    printf("  ERROR 3 in routine %s\n",my_name) ;
                    printf("    Train = %s\n",t->name) ;
                    printf("    Wagon = %s\n",w->name) ;
                    printf("    Error code = %i :: %s\n",
                                      iret,al_error_code(iret)) ;
                  printf("    Source = %i\n",(int)source) ;
                  }
                }
/*
 *  Handle start_loop_release
 */
                if(trg->start_loop_release){
int i_size, i_freq, i_bits = 0, i_chan, i_bytes, n_samp ;

                  alGetBufferi(buffer, AL_FREQUENCY, &i_freq ) ;
                  alGetBufferi(buffer, AL_SIZE, &i_size ) ;
                  alGetBufferi(buffer, AL_BITS, &i_bits ) ;
                  alGetBufferi(buffer, AL_CHANNELS, &i_chan ) ;
/*
 *  Check for errors
 */
                  if(i_freq == 0 || i_chan == 0)continue ;
                  if(ip){
                    printf("  Buffer with start_loop_release\n") ;
                    printf("    AL_FREQUENCY = %i\n",i_freq) ;
                    printf("    AL_SIZE      = %i\n",i_size) ;
                    printf("    AL_BITS      = %i\n",i_bits) ;
                    printf("    AL_CHANNELS  = %i\n",i_chan) ;
                  }
                  w->cue_points[i] = 1 ;
                  i_bits = i_bits/8 ; if(i_bits <1)i_bits = 1 ;
                  n_samp = i_size/(i_bits*i_chan) ;
                  w->cue_in[i]  = n_samp/4   ;
                  w->cue_out[i] = 7*n_samp/8 ;
                }else{
                  w->cue_points[i] = 0 ;
                }
/*
 *  Release
 */
              }else if(trg->release_loop || trg->release_loop_jump){
                action_string = " ReleaseLoop :: " ;
                if(ip1)printf("  Turn source off.  Train %s, Wagon %s, Source %i, Trigger %i, AL Source %i\n",
                          t->name, w->name, i, j, (int) source) ;
                alSourceStop(source) ;
                iret = alGetError() ;
                if(iret != ALC_NO_ERROR){
                  printf("  ERROR 4 in routine %s\n",my_name) ;
                  printf("    Train = %s\n",t->name) ;
                  printf("    Wagon = %s\n",w->name) ;
                  printf("    Error code = %i %s\n",iret, al_error_code(iret)) ;
                  printf("    T %s W %s SMS %s, S %i, T %i, Source %i\n",
                         t->name, rw->name, sms_node->name, i, j, (int)source) ;
                }

                alGetSourcei(source, AL_SOURCE_STATE, &source_state);
                if(source_state != AL_STOPPED && source_state != AL_INITIAL){
                  printf("  Routine %s error\n",my_name) ;
                  printf("    OpenAL source %i is not stopped\n",( int)source) ;
                  printf("     Train %s, Wagon %s, Source %i, Trigger %i,"
                         " AL Source %i,  Source status = %s\n", t->name, w->name,
                             i, j, (int) source, alStateMessage(source_state)) ;
                }



                w->source_on[i] = 0 ;
                if(trg->release_loop_jump || w->cue_points[i]){
                  alSourcei(source,  AL_SAMPLE_OFFSET, w->cue_out[i]);
                  alSourcei(source,  AL_LOOPING, AL_FALSE);
                  alSourcePlay(source) ;
                  w->cue_points[i] = 0 ;
                }
              }
/*
 *==============================================================================
 *  Print summmary line
 *==============================================================================
 */
              if(ip2){
                printf(" Train Sounds : T %s : W %12s : S %2i : T %2i : AL_S %4i",
                                         t->name, w->name, i, j, (int)source) ;
                if(trigger_string)printf(" :: %s",trigger_string) ;
                if(action_string) printf(" :: %s",action_string) ;
                printf(" %s ",speed_string) ;
                printf(" %s ",buffer_string) ;
                if(w->wav_file[i])printf(" WF %s", w->wav_file[i]) ;
                printf("\n") ;
              }
            }
/*
 *==============================================================================
 *  Update sources with cue points
 *==============================================================================
 */
            if(w->cue_points[i] && 0){
  ALfloat sec_off ;
  int     samp_off, byte_off ;
              alGetSourcef(source,AL_SEC_OFFSET,&sec_off) ;
              if(ip)printf("    AL_SEC_OFFSET = %f\n",(double)sec_off) ;
              alGetSourcei(source,AL_SAMPLE_OFFSET,&samp_off) ;
              if(ip)printf("    AL_SAMPLE_OFFSET = %i\n",samp_off) ;
              alGetSourcei(source,AL_BYTE_OFFSET,&byte_off) ;
              if(ip)printf("    AL_BYTE_OFFSET = %i\n",byte_off) ;
              if(samp_off>w->cue_out[i]){
                alSourcei(source,  AL_SAMPLE_OFFSET, w->cue_in[i]);
              }
            }
/*
 *==============================================================================
 *  Apply any speed dependent volume or frequency correction
 *==============================================================================
 */
              smsvol_node = &snd_stream->smsvol_node ;
              smsfrq_node = &snd_stream->smsfrq_node ;

              if((t-> speed != t->last_speed ) &&
                ((smsvol_node && smsvol_node->speed_control) ||
                 (smsfrq_node && smsfrq_node->speed_control))
              ){
/*
 *==============================================================================
 *  Effect of speed on volume
 *==============================================================================
 */
                if(smsvol_node && smsvol_node->speed_control){
                  d2 = interpolate_array(smsvol_node->n_points,t->speed,
                         smsvol_node->xxx_array, smsvol_node->vol_array);
                  gain = d2*w->sound_gain[i] ;

                  alSourcef(source, AL_GAIN, gain) ;
                  iret = alGetError() ;

                  if(iret != AL_NO_ERROR){
                    printf("  ERROR AL_GAIN in routine %s\n",my_name) ;
                    printf("    Train = %s\n",t->name) ;
                    printf("    Wagon = %s\n",w->name) ;
                    printf("    Error code = %i :: %s\n",
                                      iret,al_error_code(iret)) ;
                    printf("    Source = %i\n",(int)source) ;
                    printf("    gain   = %f : %f %f %f : %f\n",(double)gain, dd, d1, d2, t->speed) ;
                    printf("    array :: ") ;
                    for(k=0; k<smsvol_node->n_points; k++){
                      printf(" : %i, %f %f",k, smsvol_node->xxx_array[k], smsvol_node->xxx_array[k]) ;
                    }
                    printf("\n") ;
                  }
                }
/*
 *==============================================================================
 *  Effect of speed on pitch
 *  MSTS wave files need a sampling rate of 11025 Hz.  Frequency array values
 *  equal to this have no change in pitch.  For other values the change
 *  is proportional to the array value/11025.
 *==============================================================================
 */
                if(smsfrq_node && smsfrq_node->speed_control && 0){
ALfloat pitch ;
                  pitch = interpolate_array(smsfrq_node->n_points,t->speed,
                         smsfrq_node->xxx_array, smsfrq_node->frq_array);
                  pitch = pitch/11025.0 ;
                  if(pitch>0.2 || pitch < 5.0) {
                    alSourcef(source, AL_PITCH, pitch) ;
                    iret = alGetError() ;

                    if(iret != AL_NO_ERROR){
                      printf("  ERROR AL_PITCH in routine %s\n",my_name) ;
                      printf("    Train = %s\n",t->name) ;
                      printf("    Wagon = %s\n",w->name) ;
                      printf("    Error code = %i :: %s\n",
                                        iret,al_error_code(iret)) ;
                      printf("    Source = %i\n",(int)source) ;
                      printf("    gain   = %f : %f\n",(double)pitch, t->speed) ;
                      printf("    array :: ") ;
                      for(k=0; k<smsfrq_node->n_points; k++){
                        printf(" : %i, %f %f",k, smsfrq_node->xxx_array[k], smsfrq_node->frq_array[k]) ;
                      }
                      printf("\n") ;
                    }
                  }
                }

              }

/*
 *==============================================================================
 *  Debug
 *==============================================================================
 */
            if(l_time_5s && ip){
ALfloat  gain ;
ALfloat  min_gain ;
ALfloat  max_gain ;
ALfloat  max_dist ;
ALfloat  ref_dist ;
ALfloat  rolloff  ;
              alGetSourcei(source, AL_SOURCE_STATE, &source_state);
              if(source_state == AL_PLAYING || 1){
                sms_node = rw->sms_node ;
                X = w->traveller->rx ;
                Y = w->traveller->ry ;
                Z = w->traveller->rz ;
                distance = sqrt( (x-X)*(x-X) + (y-Y)*(y-Y) +(z-Z)*(z-Z)) ;

                alGetSourcef(source, AL_GAIN, &gain);
                alGetSourcef(source, AL_MIN_GAIN, &min_gain);
                alGetSourcef(source, AL_MAX_GAIN, &max_gain);
                alGetSourcef(source, AL_MAX_DISTANCE, &max_dist);
                alGetSourcef(source, AL_REFERENCE_DISTANCE, &ref_dist);
                alGetSourcef(source, AL_ROLLOFF_FACTOR, &rolloff);

                printf("  Train.  sms_name %s, distance %f :: %f %f %f %f %f %f,  Source %i  %s\n",
                       sms_node->name, distance,gain, min_gain, max_gain, max_dist, ref_dist, rolloff,
                       (int)source, alStateMessage(source_state)) ;
              }
            }
/*
 *   End Loop over Sources
 */
          }       //  End of 'i' loop over sources
/*
 *==============================================================================
 *  Set wagon position and velocity
 *==============================================================================
 */
          X = w->traveller->rx ;
          Y = w->traveller->ry ;
          Z = w->traveller->rz ;
          distance = sqrt( (x-X)*(x-X) + (y-Y)*(y-Y) +(z-Z)*(z-Z)) ;
          w->distance_on = (distance < 2000.0) ;

          if(ip1)printf("  ###@### x,y,z %f %f %f  X,Y,Z %f %f %f dist %f :: %i\n",
                                 x,y,z,X,Y,Z,distance, w->distance_on) ;

          if(w->distance_on){
            U = w->traveller->vx ;
            V = w->traveller->vy ;
            W = w->traveller->vz ;
//            printf(" source, sound: ") ;
/*
 *==============================================================================
 *  Loop over all sources linked to each wagon
 *==============================================================================
 */
            for(i=0; i<n_sources; i++){
//              printf(" %i %i :",w->source_on[i],w->sound_on[i]) ;
              if(w->source_on[i] && !w->sound_on[i]){
                w->sound_on[i] = 1 ;
                alSourcePlay(w->source[i]) ;
              }
              if(w->sound_on[i]){
                alSource3f(w->source[i], AL_POSITION,  X, Y, Z);
                alSource3f(w->source[i], AL_VELOCITY,  U, V, W);
              }
            }
//            printf("\n") ;
          }else{
            for(i=0; i<n_sources; i++){
              if(w->sound_on[i]){
                w->sound_on[i] = 0 ;
                alSourceStop(w->source[i]) ;
              }
            }
          }
/*
 *  Delete any unused sound triggers (i.e. bell on train withut a bell.)
 */
          clear_wagon_sound_trigger_list(w) ;
        }
      }

      if(ip2 && l_time_5s){
        for(t=trainlist_beg; t!=NULL; t=t->next){
          for(w=t->first; w!=NULL; w=w->next){
            printf(" Summary : T %s : W %s :: ",t->name,w->name) ;
            for(i=0; i<w->n_sources; i++){
              printf("%i",w->source_on[i]) ;
            }
            printf(" ::") ;
            for(i=0; i<w->n_sources; i++){
              alGetSourcei(w->source[i], AL_BUFFERS_QUEUED, &idata) ;
              printf(" %i",idata) ;
            }
            printf(" ::") ;
            for(i=0; i<w->n_sources; i++){
              alGetSourcei(w->source[i], AL_BUFFERS_PROCESSED, &idata) ;
              printf(" %i",idata) ;
            }
            printf(" ::") ;
            for(i=0; i<w->n_sources; i++){
              alGetSourcei(w->source[i], AL_SOURCE_RELATIVE, &idata) ;
              printf(" %i",idata) ;
            }
            printf(" ::") ;
            for(i=0; i<w->n_sources; i++){
              alGetSourcei(w->source[i], AL_BUFFER, &idata) ;
              printf(" %i",idata) ;
            }
            printf(" ::") ;
            for(i=0; i<w->n_sources; i++){
              alGetSourcei(w->source[i], AL_SOURCE_STATE, &idata) ;
              printf(" %i",idata) ;
            }
            printf("\n") ;
            for(i=0; i<w->n_sources; i++){
              if(w->wav_file[i]){
                alGetSourcei(w->source[i], AL_SOURCE_STATE, &source_state);
                printf(" Stream %2i :: Wave file %s :: Source %i :: SourceState %s\n",
                            i, w->wav_file[i], w->source[i],alStateMessage(source_state) )  ;
              }
            }

          }
        }
#if 0
        printf("  Complete Sound List\n") ;
        for(i=0; i<200; i++){
          alGetSourcei(i,AL_SOURCE_STATE,&source_state) ;
int       iret = alGetError() ;
          printf("Source %i, State %s,  Error %s\n",
                 i,alStateMessage(source_state),al_error_code(iret)) ;
        }
#endif
      }
      first_pass = 0 ;
      if(sms_error)sms_error = 10 ;  //  Only list missing sms on first entry

      return 0 ;
}


#endif
