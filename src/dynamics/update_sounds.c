/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  update_sounds.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for processing OpenAL sound
 *
 *==============================================================================
 */

//char * alErrorString(int i){

//     return al_error_code(i) ;
//}

char  *al_error_code(ALenum iret){

      if(iret == AL_NO_ERROR){
        return "AL_NO_ERROR" ;
      }else if(iret == AL_INVALID_NAME){
        return "AL_INVALID_NAME" ;
      }else if(iret == AL_INVALID_ENUM){
        return "AL_INVALID_ENUM" ;
      }else if(iret == AL_INVALID_VALUE){
        return "AL_INVALID_VALUE" ;
      }else if(iret == AL_INVALID_OPERATION){
        return "AL_INVALID_OPERATION" ;
      }else if(iret == AL_OUT_OF_MEMORY){
        return "AL_OUT_OF_MEMORY" ;
      }else
        return "UNRECOGNISED ERROR" ;
}



char *alGetErrorString(){
  int i ;

     i = alGetError() ;
     if(i == AL_NO_ERROR)          return "AL_NO_ERROR"           ;
     if(i == AL_INVALID_NAME)      return "AL_INVALID_NAME)"      ;
     if(i == AL_INVALID_ENUM)      return "AL_INVALID_ENUM)"      ;
     if(i == AL_INVALID_VALUE)     return "AL_INVALID_VALUE)"     ;
     if(i == AL_INVALID_OPERATION) return "AL_INVALID_OPERATION)" ;
     if(i == AL_OUT_OF_MEMORY)     return "AL_OUT_OF_MEMORY)"     ;
     return NULL ;
}
char *alStateMessage(ALuint i){

     if(i == AL_INITIAL) return "AL_INITIAL)" ;
     if(i == AL_PLAYING) return "AL_PLAYING)" ;
     if(i == AL_PAUSED)  return "AL_PAUSED)"  ;
     if(i == AL_STOPPED) return "AL_STOPPED)"  ;
     return NULL ;
}

int update_sounds(void){

  int ip = 0 ;
  int i, nn, mm, ip1 ;
  static int first_pass = 1;
  int n_streams,  i_stream  ;
  int n_triggers, i_trigger ;
  int tile_x, tile_y ;
  int action ;
  int looping ;
  double   X, Y, Z, x, y, z ;
  double   distance ;
  double   delay, dd, d1, d2 ;
  ALfloat  s_x[3], t_x[3] ;

  WorldNode *wnode ;
  SoundObjectNode *sndobj_node ;
  SMS_Node        *sms_node    ;
  SoundFile       *soundfile   ;
  SmsGrpNode      *smsgrp_node ;
  SmsStrNode      *smsstr_node ;
  SmsTrgNode      *smstrg_node ;
  SmsFilNode      *smsfil_node ;
  TravellerNode   *t           ;
  char            *corename    ;
  char *my_name = "update_sounds" ;


      ip1 = ip && l_time_5s ;
      if(ip1)printf("  Enter routine %s\n",my_name) ;
/*
 *==============================================================================
 *  Set position of listenerif(speed_l<0.0f) speed_l = -speed_l ;
 *==============================================================================
 */
      x = lookat_eye_x*tile_size ;
      y = lookat_eye_y*tile_size ;
      z = lookat_eye_z*tile_size + tile_h0 ;

      alListener3f(AL_POSITION, (float) x, (float) y, (float) z) ;
/*
 *  Listener velocity is usually the same as wagon velocity
 *  The exception are camera 0 and 8
 */
      if(current_camera == 0 || current_camera == 8){
        alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f) ;
      }else{
        if(current_camera == 3){
          t     = player_train->last->traveller ;
        }else{
          t     = player_train->first->traveller ;
        }
        alListener3f(AL_VELOCITY, t->vx, t->vy, t->vz) ;
      }
/*
 *==============================================================================
 * Initialise train sounds
 *==============================================================================
 */
      if(first_pass){
//        printf("  first_pass = %i\n",first_pass) ;
        initial_train_sounds() ;
        first_pass = 0 ;
      }
/*
 *==============================================================================
 *  Loop over fixed sound objects (birds, town, road) (SoundObjectNodes)
 *==============================================================================
 */
      for(wnode = worldlist_beg; wnode != NULL; wnode = wnode->next){
        tile_x = wnode->tile_x ;
        tile_y = wnode->tile_y ;

//        ip1= ip && l_time_5s && tile_x == 1452 && tile_y == 10331 ;

        for(sndobj_node = wnode->sound_source; sndobj_node != NULL;
                                               sndobj_node = sndobj_node->next){
          X = (tile_x - tile_x0 + 0.5)*tile_size + sndobj_node->X ;
          Y = (tile_y - tile_y0 + 0.5)*tile_size + sndobj_node->Y ;
          Z = sndobj_node->Z;
          distance = sqrt( (x-X)*(x-X) + (y-Y)*(y-Y) +(z-Z)*(z-Z)) ;
          sndobj_node->distance_on = (distance <= 2000.0) ;

//          ip1 = sndobj_node->distance_on ;
          if(ip1){
            printf("  sound uid = %i,  distance = %f :: X = %f, Y = %f, Z = %f "
                                 ":: x = %f, y = %f, z = %f \n",
                                 sndobj_node->uid, distance, X, Y, Z, x, y, z ) ;
          }
//          if(sndobj_node->uid != 50028) continue ;

          sms_node = sndobj_node->sms_node ;
          if(sms_node == NULL){
            printf("  ERROR. Routine %s\n", my_name) ;
            printf("     sms_node == NULL\n") ;
            printf("     tile_x = %i, tile_y = %i\n",tile_x, tile_y) ;
            printf("     sound source node:\n") ;
            printf("           uid         = %i\n",sndobj_node->uid) ;
            printf("           n_streams   = %i\n",sndobj_node->n_streams) ;
            printf("           priority   = %i\n",sndobj_node->priority_level) ;
            printf("           sms_file    = %s\n",sndobj_node->sms_file) ;
            exit(0) ;
          }
          i = sndobj_node->priority_level ;
          smsgrp_node = &(sms_node->smsgrp_node[i]);
          if(smsgrp_node == NULL){
            printf("  ERROR. Routine %s\n", my_name) ;
            printf("     Sound source has no priority set\n") ;
            printf("     tile_x = %i, tile_y = %i\n",tile_x, tile_y) ;
            exit(0) ;
          }
/*
 *==============================================================================
 *  Loop over streams
 *==============================================================================
 */
int        stream_status ;
SmsTrgNode *smstrg_node ;
ALint      source_state ;
ALuint     source ;

          n_streams = smsgrp_node->n_streams ;
          if(ip1){
            printf("  sms_node = %p,  priority = %i, sms_pri_node = %p, n_streams = %i\n",
                   (void *)sms_node, i, (void *)smsgrp_node, n_streams) ;
          }
          for(i_stream = 0 ; i_stream < n_streams; i_stream++){

 //           if(i_stream != 2) continue ;
 //           ip1 = ip1 && (i_stream == 2) ;

            smsstr_node   = &(smsgrp_node->snd_stream[i_stream]) ;
//            stream_status = sndobj_node->stream_status[i_stream] ;
            source = sndobj_node->source[i_stream] ;
            alGetSourcei(source, AL_SOURCE_STATE, &source_state);
            alGetSourcei(source, AL_LOOPING, &looping);
/*
 *   Print debug information
 */
            if(ip1){
              printf("\n") ;
              printf("  Routine %s.  tile_x = %i, tile_y = %i, sound uid = %i,"
                     "  stream = %i, AL_SOURCE_STATE = %s\n", my_name, tile_x, tile_y,
                 sndobj_node->uid, i_stream, alStateMessage(source_state) ) ;
              printf("       sms name = %s,  file = %s\n",
                                         sms_node->name, sms_node->sms_file) ;
              printf("       distance = %f :: X = %f, Y = %f, Z = %f "
                                                  ":: x = %f, y = %f, z = %f \n",
                                     distance, X, Y, Z, x, y, z ) ;
              alGetSourcefv(source, AL_POSITION, s_x) ;
              alGetListenerfv(AL_POSITION, t_x) ;
              printf("       distance    = %f :: X = %f, Y = %f, Z = %f "
                                                  ":: x = %f, y = %f, z = %f \n",
                              distance, s_x[0], s_x[1], s_x[2], t_x[0], t_x[1], t_x[2] ) ;
              printf("       distance_on = %i\n",
                                     sndobj_node->distance_on) ;
              printf("       source_on   = %i\n",
                                     sndobj_node->source_on[i_stream]) ;
              printf("       sound_on    = %i\n",
                                     sndobj_node->sound_on[i_stream]) ;
              printf("       looping    = %i\n",looping) ;
            }
/*
 *==============================================================================
 *   If distance_on (Distance > 2000m and source plying) :: Stop sound
 *==============================================================================
 */

            if(!(sndobj_node->distance_on)){           // distance > 500.0 m
              if(source_state == AL_PLAYING){
                alSourceStop(source) ;
                sndobj_node->stream_status[i_stream] = 0 ;
                if(ip1)printf("  Routine %s ::  STOP PLAYING.  tile_x = %i, tile_y = %i,"
                     "  Source uid = %i, stream = %i,  source_state = %s, alError = %s\n",
                         my_name, tile_x, tile_y, sndobj_node->uid, i_stream,
                         alStateMessage(source_state), alGetErrorString()) ;
              }
/*
 *==============================================================================
 *  If distance < 500m and Source is silent
 *==============================================================================
 */
            }else if(source_state != AL_PLAYING &&
                     sndobj_node->sound_on[i_stream] && looping){
              alSourcePlay(source) ;
              if(ip1)printf("  Routine %s ::  START PLAYING.  tile_x = %i, tile_y = %i,"
                     "  Source uid = %i,  stream = %i, initial source_state = %s\n",
                         my_name, tile_x, tile_y, sndobj_node->uid, i_stream,
                         alStateMessage(source_state)) ;
            }
/*
 *==============================================================================
 *    Loop over Triggers
 *==============================================================================
 */
            for(i=0;i<smsstr_node->n_triggers; i++){
              action = 0 ;
              smstrg_node = &(smsstr_node->smstrg_node[i]) ;
              if(ip1)printf("    Trigger %i :: trigger_type = %i, n_files = %i\n",
                     i, smstrg_node->trigger_type, smstrg_node->n_files) ;
/*
 *==============================================================================
 *    Variable Trigger
 *==============================================================================
 */
              switch(smstrg_node->trigger_type){
                case(SND_TRG_INITIAL):
                case(SND_TRG_VARIABLE):
                  if(smstrg_node->n_files==0)break ;
                  if(source_state == AL_PLAYING) break ;
                  action = 1 ;
                  break ;
/*
 *==============================================================================
 *    Random Trigger
 *==============================================================================
 */
                case(SND_TRG_RANDOM):
                  if(smstrg_node->n_files == 0 )break ;
                  if(source_state == AL_PLAYING) break ;
/*
 *  If source is not playing and delay not set:
 *    Set up a new delay (if not already set up)
 */
                  if(sndobj_node->delay[i_stream] == 0.0){
                    dd = (double)rand()/(double)RAND_MAX ;
                    sndobj_node->delay[i_stream] = smstrg_node->delay_min +
                           dd*(smstrg_node->delay_max - smstrg_node->delay_min) ;
                    sndobj_node->last_time[i_stream] = run_seconds ;
                    if(ip1)printf("  Random trigger :: RANDOM   = %f\n",dd) ;
                  }
                  if(ip1)printf("  Random trigger :: delay val   = %f\n",
                                sndobj_node->delay[i_stream]) ;
                  if(ip1)printf("  Random trigger :: run_seconds = %f\n",
                                run_seconds) ;
                  if(ip1)printf("  Random trigger :: last_time   = %f\n",
                                sndobj_node->last_time[i_stream]) ;
/*
 *  If delay exceeded, set action, set last time, unset delay.
 */
                  delay = run_seconds - sndobj_node->last_time[i_stream] ;
                  if(ip1)printf("  Random trigger :: delay       = %f\n",
                                delay) ;
                  if(delay > sndobj_node->delay[i_stream] ){
                    action = 1 ;
                    sndobj_node->delay[i_stream] = 0.0 ;
                    sndobj_node->last_time[i_stream] = run_seconds ;
                  }
                  break ;
                default :
                  break ;
              }
              if(ip1)printf("  Processed trigger %i,  action = %i\n",i,action) ;
/*
 *==============================================================================
 *    Action 1.  Select file
 *==============================================================================
 */
              if(action){
//  Select file
                mm = smstrg_node->n_files ;
                nn = 0 ;
                smsfil_node = NULL ;
                if(mm > 0){
                  if(smstrg_node->sequential){
                    nn = sndobj_node->last_file[i_stream] ;
                    nn++ ;
                  }else if(smstrg_node->random){
                    dd = (double)rand()/RAND_MAX ;
                    nn = dd * mm ;
                  }
                  if(nn>=mm) nn = 0 ;
                  smsfil_node = &(smstrg_node->smsfil_node[nn]) ;
                  sndobj_node->last_file[i_stream] = nn ;
                }
                if(ip1)printf("  File = %i, n_files = %i\n",nn,mm) ;

//  Fix up case of no sound
                if(smsfil_node->buffer == 0){
#if 0

                  smsfil_node->buffer = alutCreateBufferHelloWorld() ;
                  source = sndobj_node->source[i_stream]
                                               = smsfil_node->buffer ;
                  if(ip1)printf("    Create buffer %i using alutCreateBufferHelloWorld."
                         "Alut Error = %s\n", source, alutGetErrorString(alutGetError())) ;
#else
                  printf("  Routine %s.  Buffer missing for SMS %s, Stream %i, Trigger %i, wav file %s\n",
                         my_name, sndobj_node->sms_file, i_stream, i, smsfil_node->wav_file) ;
                  continue ;
#endif
                }

/*
 *==============================================================================
 *    Action 1.  Command
 *==============================================================================
 */
                if(smstrg_node->play_one_shot || smstrg_node->release_loop_jump){
                  if(ip1)printf("  Looping OFF\n") ;
                  alSourcei(source, AL_LOOPING, AL_FALSE);
                }else{
                  if(ip1)printf("  Looping ON\n") ;
                  alSourcei(source, AL_LOOPING, AL_TRUE);
                }
                if(smstrg_node->release_loop) {
//  Stop
                  alSourceStop(source) ;
                  sndobj_node->stream_status[i_stream] = 0 ;
                  sndobj_node->source_on[i_stream] = 0 ;
                  sndobj_node->sound_on[i_stream]  = 0 ;
                  if(ip1)printf("  Source Stop\n") ;
                }else{
                  alSourcei(source, AL_BUFFER, smsfil_node->buffer) ;
//  Volume
                  if(smstrg_node->volume_max != 0.0){
                    dd = (double)rand()/RAND_MAX ;
                    d1 = smstrg_node->volume_min + dd*
                     (smstrg_node->volume_max - smstrg_node->volume_min) ;
                    if(d1 <= 1.0f && smsstr_node->volume <= 1.0f){
                      alSourcef(source, AL_GAIN, d1*smsstr_node->volume) ;
                    }
                  }else{
                    if(smsstr_node->volume <= 1.0f){
                      alSourcef(source, AL_GAIN, smsstr_node->volume) ;
                    }
                  }
//  Play
                  if(sndobj_node->distance_on){
                    alSourcePlay(source) ;
                  }else{
                    alSourceStop(source) ;
                  }
                  sndobj_node->stream_status[i_stream] = 1 ;
                  sndobj_node->source_on[i_stream] = 1 ;
                  sndobj_node->sound_on[i_stream] = sndobj_node->distance_on  ;
                  alGetSourcei(source, AL_LOOPING, &looping);
                  if(ip1)printf("  Source Play\n") ;
                  if(ip1)printf("  distance_on = %i\n",sndobj_node->distance_on) ;
                  if(ip1)printf("  i_stream    = %i\n",i_stream) ;
                  if(ip1)printf("  source_on   = %i\n",
                                     sndobj_node->source_on[i_stream]) ;
                  if(ip1)printf("  sound_on    = %i\n",
                                     sndobj_node->sound_on[i_stream]) ;
                  if(ip1)printf("  looping    = %i\n",looping) ;
                }
              }
            }  //  End loop over triggers
/*
 *==============================================================================
 *   End of Main Loops
 *==============================================================================
 */
            if(l_time_5s && 0){
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

                printf("  Object.  sms_name %s, distance %f :: %f %f %f %f %f %f\n",
                       sms_node->name, distance,gain, min_gain, max_gain, max_dist, ref_dist, rolloff) ;
              }
            }

          }  //  End of loop over sources
        }    //  End of loop over world items
      }      //  End of loop over tiles


      return 0 ;
}
