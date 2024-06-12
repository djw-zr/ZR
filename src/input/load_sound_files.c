/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  load_consist.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for reading the sound management (.sms) and sound files
 *
 *==============================================================================
 */

/*
 *  Sound Management System (*.sms) files arestored in:
 *    *  The route SOUND directory
 *    *  The top level SOUND directory
 *  Some routes may also need sms files from a MSTS SOUND directory
 */

int read_files_section(SmsTrgNode *smstrg_node, MSfile *msfile) ;

int load_sound_files(void){

  int    ip = 0 ;
  int    iret   ;
  int    len, i, n ;
  char   *dir_name ;
  char   *file_name ;
  char   *name = NULL ;
  DIR    *cdir      ;
  struct dirent *f_entry  ;
  SMS_Node      *sms_node ;
  SoundFile     *snd_file ;
  BTree         *btree    ;

  char   *snd_dir  = "/SOUND" ;
  char   *my_name = "load_sound_files"    ;


      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
 *==============================================================================
 *  Loop over route directory and  top level directory
 *  At each level process sound management system files
 *==============================================================================
 */
      for(i=0; i<2; i++){
/*
 *==============================================================================
 *  Search for SOUND directories
 *==============================================================================
 */
        if(i == 0){
          dir_name = (char *)malloc(strlen(ORroutedir)+strlen(snd_dir) + 1 ) ;
          strcpy(dir_name,ORroutedir) ;
          strcat(dir_name,snd_dir) ;
        }else{
          dir_name = (char *)malloc(strlen(ORdir)+strlen(snd_dir) + 1 ) ;
          strcpy(dir_name,ORdir) ;
          strcat(dir_name,snd_dir) ;
        }
        iret = find_msstyle_file(&dir_name) ;
        if(iret){
          printf("  Routine %s.  Unable to find directory %s\n",my_name,dir_name);
          free(dir_name) ;
          continue ;
        }
/*
 *==============================================================================
 *  Open Sound directory
 *==============================================================================
 */
        if(ip)printf("  Sound directory name = %s\n",dir_name) ;

        cdir = opendir(dir_name) ;
        if(cdir == NULL){
          printf("  Routine %s error\n", my_name) ;
          printf("   Unable to find sound directory\n") ;
          printf("   Directory name = %s\n",dir_name) ;
          printf("   Program stopping ... \n") ;
          free(dir_name) ;
          close_system() ;
        }
/*
 *==============================================================================
 *  Loop through sound files
 *  Process sound management system files and add needed *.wav files to
 *  btree
 *==============================================================================
 */
        while ((f_entry = readdir(cdir)) != NULL) {
          len = strlen(f_entry->d_name) ;
          if(ip)printf("  Found directory file = %s :: %s\n",
              f_entry->d_name,&(f_entry->d_name[len-4]) ) ;
          if(!strncmp_ic(".sms",&(f_entry->d_name[len-4]),4)) {
            if(ip)printf("      Found sms file = %s :: %s\n",
                f_entry->d_name,&(f_entry->d_name[len-4]) ) ;

            if(name)free(name) ;
            name = zr_corename(f_entry->d_name) ;
            str2lc(name) ;
            btree = find_btree(sms_master,name) ;
            if(btree == NULL) continue ;

            if(ip)printf("      NEEDED sms file = %s\n",f_entry->d_name ) ;

            sms_node = (SMS_Node *) btree->data ;
            file_name = (char *)malloc(strlen(dir_name)+strlen(f_entry->d_name)+2) ;
            strcpy(file_name,dir_name) ;
            strcat(file_name,"/") ;
            strcat(file_name,f_entry->d_name) ;
/*
 *==============================================================================
 *  Load SMS file
 *==============================================================================
 */
            load_sound_sms_file(sms_node,file_name) ;
/*
 *==============================================================================
 *  Clean up
 *==============================================================================
 */
            free(file_name) ;
            file_name = NULL ;
          }
        }
        closedir(cdir) ;
        free(dir_name) ;
      }
/*
 *==============================================================================
 *  Loop over route directory and  top level directory
 *  At each level process sound (*.wav) files
 *==============================================================================
 */
      for(i=0; i<2; i++){
/*
 *==============================================================================
 *  Search for SOUND directories
 *==============================================================================
 */
        if(i == 0){
          dir_name = (char *)malloc(strlen(ORroutedir)+strlen(snd_dir) + 1 ) ;
          strcpy(dir_name,ORroutedir) ;
          strcat(dir_name,snd_dir) ;
        }else{
          dir_name = (char *)malloc(strlen(ORdir)+strlen(snd_dir) + 1 ) ;
          strcpy(dir_name,ORdir) ;
          strcat(dir_name,snd_dir) ;
        }
        iret = find_msstyle_file(&dir_name) ;
        if(iret){
          printf("  Routine %s.  Unable to find directory %s\n",my_name,dir_name);
          free(dir_name) ;
          continue ;
        }
/*
 *==============================================================================
 *  Open Sound directory
 *==============================================================================
 */
        if(ip)printf("  Sound directory name = %s\n",dir_name) ;

        cdir = opendir(dir_name) ;
        if(cdir == NULL){
          printf("  Routine %s error\n", my_name) ;
          printf("   Unable to find sound directory\n") ;
          printf("   Directory name = %s\n",dir_name) ;
          printf("   Program stopping ... \n") ;
          free(dir_name) ;
          close_system() ;
        }
/*
 *==============================================================================
 *  Loop through sound files
 *  Process sound *.wav files
 *==============================================================================
 */
        while ((f_entry = readdir(cdir)) != NULL) {
          len = strlen(f_entry->d_name) ;
          if(ip)printf("  Found directory file = %s :: %s\n",
              f_entry->d_name,&(f_entry->d_name[len-4]) ) ;
          if(!strncmp_ic(".wav",&(f_entry->d_name[len-4]),4)) {
            if(ip)printf("      Found wav file = %s :: %s\n",
                f_entry->d_name,&(f_entry->d_name[len-4]) ) ;

            if(name)free(name) ;
            name = zr_corename(f_entry->d_name) ;  //  Remove extension
            str2lc(name) ;                         //  Convert to lower case
            btree = find_btree(wav_master,name) ;
/*
 *  Skip files that are not needed
 */
            if(btree == NULL){
              if(ip)printf("      wav file = %s :: Not needed :: %s\n",f_entry->d_name,name ) ;
/*
 *  Load needed files that have not previously been loaded
 */
            }else if(btree->data == NULL){
              if(ip)printf("      wav file = %s :: is needed :: %s\n",f_entry->d_name,name ) ;
              snd_file = (SoundFile *)malloc(sizeof(SoundFile)) ;
              btree->data = (void *)snd_file ;

              snd_file->name   = name ;            //  Lower case name
              name = NULL ;
              snd_file->format = 0 ;
              snd_file->data   = NULL ;
              snd_file->size   = 0 ;
              snd_file->freq   = 0 ;
              snd_file->loop   = 0 ;
              snd_file->buffer = 0 ;  //  Deleating buffer '0' is illegal in OpenAL

              file_name = (char *)malloc(strlen(dir_name)+strlen(f_entry->d_name)+2) ;
              strcpy(file_name,dir_name) ;
              strcat(file_name,"/") ;
              strcat(file_name,f_entry->d_name) ;
              snd_file->filename = file_name ;

              if(soundfile_beg == NULL){
                  soundfile_beg = snd_file ;
              }else{
                soundfile_end->next = snd_file ;
              }
              soundfile_end = snd_file ;
/*
 *==============================================================================
 *  Try loading the sound file
 *  Using alut - load sound file into a new buffer.
 *  See /usr/include/AL/alut.h
 *==============================================================================
 */
#ifdef OPENAL
#if 1
ALvoid  *wav_data = NULL  ;
char    *cwav_data = NULL ;
ALsizei z1, z2, z3, z4   ;

ALenum  wav_format ;
ALsizei wav_size   ;
ALsizei i_wav_freq ;
ALfloat wav_freq   ;

              if(ip)printf("  call load_wave_file :: Filename = %s\n",file_name) ;
              iret = load_wave_file(file_name, &cwav_data, &wav_format, &wav_size, &i_wav_freq) ;
              if(ip){
                printf("  return from load_wave_file :: iret = %i\n",iret) ;
                printf("    data              = %p\n",(void *)cwav_data) ;
              }
              wav_freq = i_wav_freq ;
              wav_data = cwav_data ;

              if(iret)  continue ; // No good data

              alGenBuffers(1,&(snd_file->buffer)) ;
              iret = alGetError() ;
              if(ip)printf("  alGenBuffers :: iret = %i, %s\n",iret, al_error_code(iret)) ;

              alBufferData(snd_file->buffer, wav_format, wav_data, wav_size, wav_freq) ;
              iret = alGetError() ;
              if(ip)printf("  alBufferData :: iret = %i, %s\n",iret, al_error_code(iret)) ;
              if(iret){
                printf("  buffer, data, format, size, freq = %i %p %i %i %f\n",
                       snd_file->buffer, (void *)wav_data, wav_format, wav_size, wav_freq) ;
              }


#else
              snd_file->buffer = alutCreateBufferFromFile(file_name) ;
              iret = alutGetError() ;
              if(iret || ip)printf("  Call alutCreateBufferFromFile."
                             " Error return = %s\n", alutGetErrorString(iret)) ;
#endif
#endif
            }
          }
        }
        closedir(cdir) ;
        free(dir_name) ;
      }
      if(ip){
        printf("\n  Routine: %s\n    BTree for wav_master\n",my_name);
        dump_btree(wav_master,0,"X") ;
      }
/*
 *==============================================================================
 *  Loop over all SMS files and setup SmsFilNode soundfile pointers
 *==============================================================================
 */
int        j, k, l, m, nf;
SMS_Node   *sms_list ;
SmsStrNode *smsstr_node ;
SmsGrpNode *smsgrp_node ;
SmsTrgNode *smstrg_node ;
SmsFilNode *smsfil_node ;
char       *wav_file ;

      for(sms_list = sms_list_beg; sms_list != NULL; sms_list = sms_list->next){
        if(ip)printf("  Routine %s ::  PROCESS SMS FILE %s\n",my_name, sms_list->name) ;
        for(i=0; i<4; i++){
          smsgrp_node = &(sms_list->smsgrp_node[i]) ;
          if(ip)printf("  Priority = %i.  Node = %p\n",i, (void *)smsgrp_node) ;
          if(!smsgrp_node)continue ;
          n = smsgrp_node->n_streams ;
          for(j=0; j<n; j++){
            smsstr_node = &(smsgrp_node->snd_stream[j]) ;
            if(ip)printf("    Stream = %i.  Node = %p\n",j, (void *)smsstr_node) ;
            m = smsstr_node->n_triggers ;
            for(k=0; k<m; k++){
              smstrg_node = &(smsstr_node->smstrg_node[k]) ;
              if(ip)printf("      Trigger = %i.  Node = %p\n",k, (void *)smstrg_node) ;
              nf = smstrg_node->n_files ;
              for(l=0; l<nf; l++){
                smsfil_node = &(smstrg_node->smsfil_node[l]) ;
                wav_file = zr_basename2(smsfil_node->wav_file) ;  //  No directory or extension
                str2lc(wav_file) ;                                //  Convert to lower case
                if(ip)printf("      File = %i.  Node = %p.  File = %s\n",
                                             k, (void *)smsfil_node, wav_file) ;
                btree = find_btree(wav_master,wav_file) ;
                if(btree){
                  smsfil_node->soundfile = (SoundFile *)btree->data ;
                  smsfil_node->buffer    = smsfil_node->soundfile->buffer ;
                }else{
                  smsfil_node->soundfile = NULL ;
                  smsfil_node->buffer    = 0 ;
                }
                if(ip)printf("      Soundfile = %p.  Buffer = %i\n",
                (void *)smsfil_node->soundfile, smsfil_node->buffer) ;
                if(wav_file) free(wav_file) ; wav_file = NULL ;
              }
            }
          }
        }
      }
/*
 *==============================================================================
 * Loop over all the sound region structures and set up the priority level,
 * n_streams and initialise the stream.
 *==============================================================================
 */
WorldNode       *wnode ;
SoundObjectNode *sndobj_node ;
int             tile_x, tile_y ;
int             n_sources ;
double          X, Y, Z ;

      for(wnode = worldlist_beg; wnode != NULL; wnode = wnode->next){
        tile_x = wnode->tile_x ;
        tile_y = wnode->tile_y ;
        for(sndobj_node = wnode->sound_source; sndobj_node != NULL;
                                               sndobj_node = sndobj_node->next){
          if(ip)printf("  Routine %s.  Process SOUND_SOURCE tile_x = %i,"
                       "tile_y = %i, uid = %i,  file = %s\n",
                       my_name,tile_x, tile_y, sndobj_node->uid, sndobj_node->sms_file) ;
          sms_node = sndobj_node->sms_node ;
          X = (tile_x - tile_x0 + 0.5)*tile_size + sndobj_node->X ;
          Y = (tile_y - tile_y0 + 0.5)*tile_size + sndobj_node->Y ;
          Z = sndobj_node->Z;
          if(!sms_node){
            printf("  ERROR in routine: %s\n",my_name) ;
            printf("      sms_file = %s\n", sndobj_node->sms_file) ;
            printf("      sms_node is not set\n") ;
            printf("      error in file %s, at line %i\n",__FILE__,__LINE__) ;
            continue ;
          }
          for(i=0;i<4;i++){
            smsgrp_node = &(sms_node->smsgrp_node[i]) ;
            if(smsgrp_node) break ;
          }
          if(smsgrp_node == NULL){
            printf("  ERROR. Routine %s\n", my_name) ;
            printf("     Sound sorce has no priority set\n") ;
            printf("     tile_x = %i, tile_y = %i\n",tile_x, tile_y) ;
            close_system() ;
          }
          sndobj_node->priority_level = i ;
          sndobj_node->n_streams = n_sources  = smsgrp_node->n_streams ;
          if(ip)printf("    Priority level = %i.  n_streams = %i\n",i,n_sources) ;
          sndobj_node->stream_status = (int *)malloc(n_sources*sizeof(int)) ;
          sndobj_node->source        = (ALuint *)malloc(n_sources*sizeof(ALuint)) ;
          sndobj_node->sound_on      = (int *)malloc(n_sources*sizeof(int)) ;
          sndobj_node->source_on     = (int *)malloc(n_sources*sizeof(int)) ;
          sndobj_node->last_file     = (int *)malloc(n_sources*sizeof(int)) ;
          sndobj_node->last_time     = (double *)malloc(n_sources*sizeof(double)) ;
          sndobj_node->delay         = (double *)malloc(n_sources*sizeof(double)) ;
          sndobj_node->distance_on   = 1 ;
/*
 *  Initialise
 */
          for(i=0; i<n_sources; i++){
            sndobj_node->stream_status[i] = 0 ;
            sndobj_node->sound_on[i]      = 0 ;
            sndobj_node->source_on[i]     = 0 ;
            sndobj_node->last_file[i]     = 0 ;
            sndobj_node->last_time[i]     = 0.0 ;
            sndobj_node->delay[i]         = 0.0 ;

            alGenSources(1, &(sndobj_node->source[i])) ;

            iret = alGetError() ;
            if(ip)printf("      Stream %i.  Source = %i. alError = %i %s\n",
                    i,sndobj_node->source[i], iret, al_error_code(iret)) ;
            if(iret != AL_NO_ERROR){
              printf("  ERROR in routine: %s\n",my_name) ;
              printf("      sms_file = %s\n", sndobj_node->sms_file) ;
              printf("      alGetErrer non zero.  %i  %s\n",
                                                   iret, al_error_code(iret)) ;
             printf("      Stream %i.  Source = %i. \n",
                                                    i,sndobj_node->source[i]) ;
             printf("      error in file %s, at line %i\n",__FILE__,__LINE__) ;
              continue ;
            }
            alSourcef(sndobj_node->source[i], AL_PITCH, 1.0f);
            alSourcef(sndobj_node->source[i], AL_GAIN,  1.0f);
            alSource3f(sndobj_node->source[i], AL_POSITION,  X, Y, Z);
            alSource3f(sndobj_node->source[i], AL_VELOCITY,  0.0f, 0.0f, 0.0f);
            alSourcei(sndobj_node->source[i], AL_LOOPING, AL_TRUE);
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
            alSourcef(sndobj_node->source[i], AL_REFERENCE_DISTANCE,   8.0f);
            alSourcef(sndobj_node->source[i], AL_MAX_GAIN,             1.0f);
            alSourcef(sndobj_node->source[i], AL_MAX_DISTANCE,      2000.0f);
            alSourcef(sndobj_node->source[i], AL_ROLLOFF_FACTOR,      0.32f);
          }
        }
      }

      return 0 ;
}

int  load_sound_sms_file(SMS_Node *sms_node, char *file_name){

  int  ip = 0 ;
  int  iret   ;
  int  isg    ;
  int  n_streams, i_stream, n_triggers, i_trigger, i, n ;
  MSfile   msfile0 ;
  MSfile  *msfile = &msfile0 ;
  SmsGrpNode  *smsgrp_node   ;
  SmsStrNode  *smsstr_node   ;
  SmsTrgNode  *smstrg_node   ;
  SmsVolNode  *smsvol_node   ;
  SmsFrqNode  *smsfrq_node   ;
  SmsFilNode  *smsfil_node   ;
  char *token1 = NULL,
       *token2 = NULL,
       *token3 = NULL,
       *token4 = NULL,
       *token5 = NULL,
       *token6 = NULL,
       *token7 = NULL,
       *token8 = NULL,
       *token9 = NULL,
       *string = NULL,
       *string2= NULL ;
  char *my_name = "load_sound_sms_file" ;

      if(ip)printf("    ENTER routine %s\n",my_name) ;
/*
 *==============================================================================
 *  Initialise node
 *==============================================================================
 */
      initialise_sms_node(sms_node) ;
      sms_node->sms_fullfilename = sms_node->sms_file = strdup(file_name) ;
      sms_node->name     = zr_basename2(file_name) ;
/*
 *==============================================================================
 *  Open MS/KUJU style file
 *==============================================================================
 */
      iret = open_msfile(file_name, msfile, 0, 0) ;
      if(ip)printf("   SMS file name = %s\n",file_name) ;
      if(iret){
        printf("  Routine '%s' error\n", my_name) ;
        printf("    Unable to open Microsoft/Kuju style file\n") ;
        printf("    File name = %s\n",file_name) ;
        printf("    Program stopping ... \n") ;
        close_system() ;
      }
/*
 *   Level 0 : Train
 */
#if 1
      for(;;){
        if(token1)free(token1) ;
        token1 = ctoken(msfile) ;
        if(is_rbr(token1)) break ;
        if(!strcmp(token1,eof_mark)) break ;
        if(ip)printf("    Level 1 :: token1 = %s \n",token1) ;
        SWITCH(token1)
/*
 *  Top level 1.  Tr_SMS
 *    There should be a single top level Tr_SMS token
 */
          CASE("Tr_SMS")
            skip_lbr(msfile) ;
/*
 *  Level 2.  ScalabiltyGroup
 *
 *    The scalibility group is concerned with priority.  Values are
 *    5, 3, 1 and 0, with 5 the highest priority.
 */
            for(;;){
              if(token2)free(token2) ;
              token2 = ctoken(msfile) ;
              if(is_rbr(token2)) break ;
              if(ip)printf("      Level 2 :: token2 = %s \n",token2) ;
              SWITCH(token2)
                CASE("Skip")
                CASE("Comment")
                  skip_lbr(msfile);
                  skippast_rbr(msfile) ;
                  break ;
                CASE("ScalabiltyGroup")
                  skip_lbr(msfile) ;
                  isg = itoken(msfile) ;
                  if(ip)printf("      Scalability group = %i\n",isg) ;
                  if(isg == 5)     smsgrp_node = &(sms_node->smsgrp_node[0]) ;
                  else if(isg == 3)smsgrp_node = &(sms_node->smsgrp_node[1]) ;
                  else if(isg == 1)smsgrp_node = &(sms_node->smsgrp_node[2]) ;
                  else if(isg == 0)smsgrp_node = &(sms_node->smsgrp_node[3]) ;
                  else {
                    printf("  ERROR.  Routine %s.  Faulty ScalabilityGroup\n",my_name) ;
                    printf("          ScalabilityGroup = %i\n",isg) ;
                    return 1  ;
                  }
/*
 *  Level 3.  ScalabiltyGroup tokens
 */
                  for(;;){
                    if(token3)free(token3) ;
                    token3 = ctoken(msfile) ;
                    if(is_rbr(token3)) break ;
                    if(ip)printf("        Level 3 :: token3 = %s \n",token3) ;
                    SWITCH(token3)
                      CASE("Skip")
                      CASE("Comment")
                        skip_lbr(msfile) ;
                        skippast_rbr(msfile) ;
                        break ;
                      CASE("Activation")
                        skip_lbr(msfile) ;
/*
 *  Level 4.  Activation
 */
                        for(;;){
                          if(token4)free(token4) ;
                          token4 = ctoken(msfile) ;
                          if(is_rbr(token4)) break ;
                          if(ip)printf("          Level 4 :: token4 = %s \n",token4) ;
                          SWITCH(token4)
                            CASE("Skip")
                            CASE("Comment")
                              skip_lbr(msfile) ;
                              skippast_rbr(msfile) ;
                              break ;
                            CASE("CabCam")
                              skip_lbr(msfile) ;
                              skip_rbr(msfile) ;
                              smsgrp_node->active.cab_cam = 1 ;
                              break ;
                            CASE("ExternalCam")
                              skip_lbr(msfile) ;
                              skip_rbr(msfile) ;
                              smsgrp_node->active.ext_cam = 1 ;
                              break ;
                            CASE("PassengerCam")
                              skip_lbr(msfile) ;
                              skip_rbr(msfile) ;
                              smsgrp_node->active.pas_cam = 1 ;
                              break ;
                            CASE("Distance")
                              skip_lbr(msfile) ;
                              smsgrp_node->active.dist = dtoken(msfile) ;
                              skip_rbr(msfile) ;
                              break ;
                            DEFAULT
                              printf("  Routine '%s' error\n",my_name) ;
                              printf("  Unrecognised level 4 token.  Token = '%s'\n",token4) ;
                              skip_lbr(msfile)   ;
                              skippast_rbr(msfile) ;
                              break ;
                          END
                        }
                        if(token4){free(token4) ; token4 = NULL; }
                        break ;
                      CASE("Deactivation")
                        skip_lbr(msfile) ;
/*
 *  Level 4.  Dectivation
 */
                        for(;;){
                          if(token4)free(token4) ;
                          token4 = ctoken(msfile) ;
                          if(is_rbr(token4)) break ;
                          if(ip)printf("          Level 4 :: token4 = %s \n",token4) ;
                          SWITCH(token4)
                            CASE("Skip")
                            CASE("Comment")
                              skip_lbr(msfile);
                              skippast_rbr(msfile) ;
                              break ;
                            CASE("CabCam")
                              skip_lbr(msfile) ;
                              skip_rbr(msfile) ;
                              smsgrp_node->deacti.cab_cam = 1 ;
                              break ;
                            CASE("ExternalCam")
                              skip_lbr(msfile) ;
                              skip_rbr(msfile) ;
                              smsgrp_node->deacti.ext_cam = 1 ;
                              break ;
                            CASE("PassengerCam")
                              skip_lbr(msfile) ;
                              skip_rbr(msfile) ;
                              smsgrp_node->deacti.pas_cam = 1 ;
                              break ;
                            CASE("Distance")
                              skip_lbr(msfile) ;
                              smsgrp_node->deacti.dist = dtoken(msfile) ;
                              skip_rbr(msfile) ;
                              break ;
                            DEFAULT
                              printf("  Routine '%s' error\n",my_name) ;
                              printf("  Unrecognised level 4 token.  Token = '%s'\n",token4) ;
                              skip_lbr(msfile)   ;
                              skippast_rbr(msfile) ;
                          END
                        }
                        if(token4){free(token4) ; token4 = NULL; }
                        break ;
/*
 *  Level 3  Volume
 */
                      CASE("Volume")
                      CASE("volume")
                        skip_lbr(msfile) ;
                        smsgrp_node->volume = dtoken(msfile) ;
                        skip_rbr(msfile) ;
                        break ;
/*
 *  Level 3  Stereo
 */
                      CASE("Stereo")
                        skip_lbr(msfile) ;
                        smsgrp_node->stereo = 1 ;
                        skip_rbr(msfile) ;
                        break ;
/*
 *  Level 3  LoadAllWaves
 */
                      CASE("LoadAllWaves")
                        skip_lbr(msfile) ;
                        smsgrp_node->loadall_waves = itoken(msfile) ;
                        skip_rbr(msfile) ;
                        break ;
/*
 *  Level 3  Streams
 */
                      CASE("Streams")
                        skip_lbr(msfile) ;
                        n_streams = smsgrp_node->n_streams = itoken(msfile) ;
                        smsgrp_node->snd_stream =
                              (SmsStrNode *)malloc(n_streams*sizeof(SmsStrNode)) ;
                        i_stream = 0 ;
                        for(i=0;i<n_streams;i++){
                          smsstr_node = &(smsgrp_node->snd_stream[i]) ;
                          smsstr_node->priority    = 0    ;
                          smsstr_node->volume      = 1.0  ;
                          smsstr_node->n_triggers  = 0    ;
                          smsstr_node->smstrg_node = NULL ;
                          smsstr_node->buffer_set  = 0    ;
                          smsstr_node->source_set  = 0    ;
                          smsstr_node->buffer      = 0    ;
                          smsstr_node->source      = 0    ;

                          smsvol_node = &(smsstr_node->smsvol_node) ;
                          smsvol_node->dist_control  = 0 ;
                          smsvol_node->speed_control = 0 ;
                          smsvol_node->var1_control  = 0 ;
                          smsvol_node->var2_control  = 0 ;
                          smsvol_node->var3_control  = 0 ;
                          smsvol_node->granuality    = 0 ;
                          smsvol_node->n_points      = 0 ;
                          smsvol_node->xxx_array     = NULL ;
                          smsvol_node->vol_array     = NULL ;

                          smsfrq_node = &(smsstr_node->smsfrq_node) ;
                          smsfrq_node->dist_control  = 0 ;
                          smsfrq_node->speed_control = 0 ;
                          smsfrq_node->var1_control  = 0 ;
                          smsfrq_node->var2_control  = 0 ;
                          smsfrq_node->var3_control  = 0 ;
                          smsfrq_node->granuality    = 0 ;
                          smsfrq_node->n_points      = 0 ;
                          smsfrq_node->xxx_array     = NULL ;
                          smsfrq_node->frq_array     = NULL ;
                        }
/*
 *  Level 4.  Stream
 */
                        for(;;){
                          if(token4)free(token4) ;
                          token4 = ctoken(msfile) ;
                          if(is_rbr(token4)) break ;
                          if(ip)printf("          Level 4 :: token4 = %s \n",token4) ;
                          SWITCH(token4)
                            CASE("Skip")
                            CASE("Comment")
                              skip_lbr(msfile);
                              skippast_rbr(msfile) ;
                              break ;
                            CASE("Stream")
                              skip_lbr(msfile) ;
                              if(i_stream<n_streams){
                                smsstr_node = &(smsgrp_node->snd_stream[i_stream++]) ;
                              }else{
                                printf("  Routine '%s' error\n",my_name) ;
                                printf("  To many sound streams  File = '%s'\n",file_name) ;
                                close_system()  ;
                              }
/*
 *  Level 5.  Stream Items
 */
                              for(;;){
                                if(token5)free(token5) ;
                                token5 = ctoken(msfile) ;
                                if(is_rbr(token5)) break ;
                                if(ip)printf("            Level 5 :: token5 = %s \n",token5) ;
                                SWITCH(token5)
                                  CASE("Skip")
                                  CASE("Comment")
                                    skip_lbr(msfile);
                                    skippast_rbr(msfile) ;
                                    break ;
                                  CASE("Priority")
                                    skip_lbr(msfile) ;
                                    smsstr_node->priority = itoken(msfile) ;
                                    skip_rbr(msfile) ;
                                    break ;
                                  CASE("Volume")
                                  CASE("volume")
                                    skip_lbr(msfile) ;
                                    smsstr_node->volume   = dtoken(msfile) ;
                                    skip_rbr(msfile) ;
                                    break ;
                                  CASE("Triggers")
                                    skip_lbr(msfile) ;
/*
 *  Level 6 Triggers
 */
                                    n_triggers = smsstr_node->n_triggers = itoken(msfile) ;
                                    smsstr_node->smstrg_node =
                                        (SmsTrgNode *)malloc(n_triggers*sizeof(SmsTrgNode)) ;
                                    i_trigger = 0 ;
                                    for(i=0;i<n_triggers;i++){
                                      smstrg_node = &(smsstr_node->smstrg_node[i]) ;
                                      smstrg_node->trigger_type       = 0 ;
                                      smstrg_node->trigger_disabled   = 0 ;

                                      smstrg_node->distance_inc_past  = 0 ;
                                      smstrg_node->distance_dec_past  = 0 ;
                                      smstrg_node->speed_inc_past     = 0 ;
                                      smstrg_node->speed_dec_past     = 0 ;
                                      smstrg_node->variable1_inc_past = 0 ;
                                      smstrg_node->variable1_dec_past = 0 ;
                                      smstrg_node->variable2_inc_past = 0 ;
                                      smstrg_node->variable2_dec_past = 0 ;
                                      smstrg_node->variable3_inc_past = 0 ;
                                      smstrg_node->variable3_dec_past = 0 ;
                                      smstrg_node->stream_vol         = 0 ;
                                      smstrg_node->var_value          = 0.0 ;

                                      smstrg_node->play_one_shot     = 0 ;
                                      smstrg_node->start_loop        = 0 ;
                                      smstrg_node->release_loop      = 0 ;
                                      smstrg_node->release_loop_jump = 0 ;
                                      smstrg_node->start_loop_release   = 0 ;
                                      smstrg_node->n_files           = 0 ;
                                      smstrg_node->smsfil_node       = NULL ;
                                      smstrg_node->sequential        = 0 ;
                                      smstrg_node->random            = 0 ;
                                      smstrg_node->disable_trigger   = 0 ;
                                      smstrg_node->enable_trigger    = 0 ;

                                      smstrg_node->i_discrete_trigger = 0 ;
                                      smstrg_node->a_discrete_trigger = 0 ;

                                      smstrg_node->delay_min         = 0.0 ;
                                      smstrg_node->delay_max         = 0.0 ;
                                      smstrg_node->distance_min      = 0.0 ;
                                      smstrg_node->distance_max      = 0.0 ;
                                      smstrg_node->volume_min        = 0.0 ;
                                      smstrg_node->volume_max        = 0.0 ;
                                                                          }

                                    for(;;){
                                      if(token6)free(token6) ;
                                      token6 = ctoken(msfile) ;
                                      if(is_rbr(token6)) break ;
                                      if(ip)printf("              Level 6 :: token6 = %s \n",token6) ;
                                      SWITCH(token6)
                                        CASE("Skip")
                                        CASE("Comment")
                                          skip_lbr(msfile);
                                          skippast_rbr(msfile) ;
                                          break ;
/*
 *  Level 7 Initial_Trigger
 */
                                        CASE("Initial_Trigger")
                                          skip_lbr(msfile) ;
                                          smstrg_node = &(smsstr_node->smstrg_node[i_trigger++]) ;
                                          smstrg_node->trigger_type = SND_TRG_INITIAL ;
                                          for(;;){
                                            if(token7)free(token7) ;
                                            token7 = ctoken(msfile) ;
                                            if(is_rbr(token7)) break ;
                                            if(ip)printf("                Level 7 :: token7 = %s \n",token7) ;
                                            SWITCH(token7)
                                              CASE("Skip")
                                              CASE("Comment")
                                                skip_lbr(msfile);
                                                skippast_rbr(msfile) ;
                                                break ;
                                              CASE("StartLoop")
                                                smstrg_node->start_loop = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("StartLoopRelease")
                                                smstrg_node->start_loop_release = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("DisableTrigger")
                                                smstrg_node->disable_trigger = 1 ;
                                                skip_lbr(msfile) ;
                                                smstrg_node->a_discrete_trigger = itoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("SetStreamVolume")
                                                skip_lbr(msfile) ;
                                                smstrg_node->stream_vol = itoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              DEFAULT
                                                printf("  Routine '%s' error\n",my_name) ;
                                                printf("  Unrecognised level 7 token.  Token = '%s'\n",token7) ;
                                                skip_lbr(msfile)   ;
                                                skippast_rbr(msfile) ;
                                            END
                                          }
                                          if(token7){free(token7) ; token7 = NULL; }
                                          break ;
/*
 *  Level 7 Variable_Trigger
 */
                                        CASE("Variable_Trigger")
                                          skip_lbr(msfile) ;
                                          smstrg_node = &(smsstr_node->smstrg_node[i_trigger++]) ;
                                          smstrg_node->trigger_type = SND_TRG_VARIABLE ;
                                          for(;;){
                                            if(token7)free(token7) ;
                                            token7 = ctoken(msfile) ;
                                            if(is_rbr(token7)) break ;
                                            if(ip)printf("                Level 7 :: token7 = %s \n",token7) ;
                                            SWITCH(token7)
                                              CASE("Skip")
                                              CASE("Comment")
                                                skip_lbr(msfile);
                                                skippast_rbr(msfile) ;
                                                break ;
                                              CASE("Distance_Inc_Past")
                                                smstrg_node->distance_inc_past = 1 ;
                                                smstrg_node->var_value = dtoken(msfile) ;
                                                break ;
                                              CASE("Distance_Dec_Past")
                                                smstrg_node->distance_dec_past = 1 ;
                                                smstrg_node->var_value = dtoken(msfile) ;
                                                break ;
                                              CASE("Speed_Inc_Past")
                                                smstrg_node->speed_inc_past = 1 ;
                                                smstrg_node->var_value = dtoken(msfile) ;
                                                break ;
                                              CASE("Speed_Dec_Past")
                                                smstrg_node->speed_dec_past = 1 ;
                                                smstrg_node->var_value = dtoken(msfile) ;
                                                break ;
                                              CASE("Variable1_Inc_Past")
                                                smstrg_node->variable1_inc_past = 1 ;
                                                smstrg_node->var_value = dtoken(msfile) ;
                                                break ;
                                              CASE("Variable1_Dec_Past")
                                                smstrg_node->variable1_dec_past = 1 ;
                                                smstrg_node->var_value = dtoken(msfile) ;
                                                break ;
                                              CASE("Variable2_Inc_Past")
                                                smstrg_node->variable2_inc_past = 1 ;
                                                smstrg_node->var_value = dtoken(msfile) ;
                                                break ;
                                              CASE("Variable2_Dec_Past")
                                                smstrg_node->variable2_dec_past = 1 ;
                                                smstrg_node->var_value = dtoken(msfile) ;
                                                break ;
                                              CASE("Variable3_Inc_Past")
                                                smstrg_node->variable3_inc_past = 1 ;
                                                smstrg_node->var_value = dtoken(msfile) ;
                                                break ;
                                              CASE("Variable3_Dec_Past")
                                                smstrg_node->variable3_dec_past = 1 ;
                                                smstrg_node->var_value = dtoken(msfile) ;
                                                break ;
                                              CASE("SetStreamVolume")
                                              CASE("setstreamvolume")
                                                skip_lbr(msfile) ;
                                                smstrg_node->stream_vol = itoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("PlayOneShot")
                                                smstrg_node->play_one_shot = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("StartLoop")
                                                smstrg_node->start_loop = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("StartLoopRelease")
                                                smstrg_node->start_loop_release = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("ReleaseLoopRelease")
                                                smstrg_node->release_loop = 1 ;
                                                skip_lbr(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("ReleaseLoopReleaseWithJump")
                                                smstrg_node->release_loop_jump = 1 ;
                                                skip_lbr(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("EnableTrigger")
                                                smstrg_node->enable_trigger  = 1 ;
                                                smstrg_node->disable_trigger = 0 ;
                                                skip_lbr(msfile) ;
                                                smstrg_node->a_discrete_trigger = itoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("DisableTrigger")
                                                smstrg_node->enable_trigger  = 0 ;
                                                smstrg_node->disable_trigger = 1 ;
                                                skip_lbr(msfile) ;
                                                smstrg_node->a_discrete_trigger = itoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("SetStreamVolume")
                                                skip_lbr(msfile) ;
                                                smstrg_node->stream_vol = itoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              DEFAULT
                                                printf("  Routine '%s' error\n",my_name) ;
                                                printf("  Unrecognised level 7 token.  Token = '%s'\n",token7) ;
                                                skip_lbr(msfile)   ;
                                                skippast_rbr(msfile) ;
                                            END
                                          }
                                          if(token7){free(token7) ; token7 = NULL; }
                                          break ;

/*
 *  Level 7 Random_Trigger
 */
                                        CASE("Random_Trigger")
                                          skip_lbr(msfile) ;
                                          smstrg_node = &(smsstr_node->smstrg_node[i_trigger++]) ;
                                          smstrg_node->trigger_type = SND_TRG_RANDOM ;
                                          for(;;){
                                            if(token7)free(token7) ;
                                            token7 = ctoken(msfile) ;
                                            if(ip)printf("                Level 7 :: token7 = %s \n",token7) ;
                                            if(is_rbr(token7)) break ;
                                            SWITCH(token7)
                                              CASE("Skip")
                                              CASE("Comment")
                                                skip_lbr(msfile);
                                                skippast_rbr(msfile) ;
                                                break ;
                                              CASE("Delay_Min_Max")
                                                skip_lbr(msfile) ;
                                                smstrg_node->delay_min = dtoken(msfile) ;
                                                smstrg_node->delay_max = dtoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("Volume_Min_Max")
                                                skip_lbr(msfile) ;
                                                smstrg_node->volume_min = dtoken(msfile) ;
                                                smstrg_node->volume_max = dtoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("StartLoop")
                                                smstrg_node->start_loop = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("PlayOneShot")
                                                smstrg_node->play_one_shot = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("SetStreamVolume")
                                                skip_lbr(msfile) ;
                                                smstrg_node->stream_vol = itoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              DEFAULT
                                                printf("  Routine '%s' error\n",my_name) ;
                                                printf("  Unrecognised level 7 token.  Token = '%s'\n",token7) ;
                                                skip_lbr(msfile)   ;
                                                skippast_rbr(msfile) ;
                                            END
                                          }
                                          if(token7){free(token7) ; token7 = NULL; }
                                          break ;

                                        CASE("Discrete_Trigger")
                                          skip_lbr(msfile) ;
                                          smstrg_node = &(smsstr_node->smstrg_node[i_trigger++]) ;
                                          smstrg_node->trigger_type = SND_TRG_DISCRETE ;
                                          smstrg_node->i_discrete_trigger = itoken(msfile) ;
                                          for(;;){
                                            if(token7)free(token7) ;
                                            token7 = ctoken(msfile) ;
                                            if(ip)printf("                Level 7 :: token7 = %s \n",token7) ;
                                            if(is_rbr(token7)) break ;
                                            SWITCH(token7)
                                              CASE("Skip")
                                              CASE("Comment")
                                                skip_lbr(msfile);
                                                skippast_rbr(msfile) ;
                                                break ;
                                              CASE("Delay_Min_Max")
                                                skip_lbr(msfile) ;
                                                smstrg_node->delay_min = dtoken(msfile) ;
                                                smstrg_node->delay_max = dtoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("Volume_Min_Max")
                                                skip_lbr(msfile) ;
                                                smstrg_node->volume_min = dtoken(msfile) ;
                                                smstrg_node->volume_max = dtoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("StartLoop")
                                                smstrg_node->start_loop = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("StartLoopRelease")
                                                smstrg_node->start_loop_release = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("ReleaseLoopRelease")
                                                smstrg_node->release_loop = 1 ;
                                                skip_lbr(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("ReleaseLoopReleaseWithJump")
                                                smstrg_node->release_loop_jump = 1 ;
                                                skip_lbr(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("PlayOneShot")
                                                smstrg_node->play_one_shot = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("SetStreamVolume")
                                                skip_lbr(msfile) ;
                                                smstrg_node->stream_vol = itoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              DEFAULT
                                                printf("  Routine '%s' error\n",my_name) ;
                                                printf("  Unrecognised level 7 token.  Token = '%s'\n",token7) ;
                                                skip_lbr(msfile)   ;
                                                skippast_rbr(msfile) ;
                                            END
                                          }
                                          if(token7){free(token7) ; token7 = NULL; }
                                          break ;

                                        CASE("Dist_Travelled_Trigger")
                                          skip_lbr(msfile) ;
                                          smstrg_node = &(smsstr_node->smstrg_node[i_trigger++]) ;
                                          smstrg_node->trigger_type = SND_TRG_DIST_TRAV ;
                                          for(;;){
                                            if(token7)free(token7) ;
                                            token7 = ctoken(msfile) ;
                                            if(ip)printf("                Level 7 :: token7 = %s \n",token7) ;
                                            if(is_rbr(token7)) break ;
                                            SWITCH(token7)
                                              CASE("Skip")
                                              CASE("Comment")
                                                skip_lbr(msfile);
                                                skippast_rbr(msfile) ;
                                                break ;
                                              CASE("Dist_Min_Max")
                                                skip_lbr(msfile) ;
                                                smstrg_node->distance_min = dtoken(msfile) ;
                                                smstrg_node->distance_max = dtoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("Volume_Min_Max")
                                                skip_lbr(msfile) ;
                                                smstrg_node->volume_min = dtoken(msfile) ;
                                                smstrg_node->volume_max = dtoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("StartLoop")
                                                smstrg_node->start_loop = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("StartLoopRelease")
                                                smstrg_node->start_loop_release = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("ReleaseLoopRelease")
                                                smstrg_node->release_loop = 1 ;
                                                skip_lbr(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("ReleaseLoopReleaseWithJump")
                                                smstrg_node->release_loop_jump = 1 ;
                                                skip_lbr(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              CASE("PlayOneShot")
                                                smstrg_node->play_one_shot = 1 ;
                                                read_files_section(smstrg_node, msfile) ;
                                                break ;
                                              CASE("SetStreamVolume")
                                                skip_lbr(msfile) ;
                                                smstrg_node->stream_vol = itoken(msfile) ;
                                                skip_rbr(msfile) ;
                                                break ;
                                              DEFAULT
                                                printf("  Routine '%s' error\n",my_name) ;
                                                printf("  Unrecognised level 7 token.  Token = '%s'\n",token7) ;
                                                skip_lbr(msfile)   ;
                                                skippast_rbr(msfile) ;
                                            END
                                          }
                                          if(token7){free(token7) ; token7 = NULL; }
                                          break ;

                                        DEFAULT
                                          printf("  Routine '%s' error\n",my_name) ;
                                          printf("  Unrecognised level 6 token.  Token = '%s'\n",token6) ;
                                          skip_lbr(msfile)   ;
                                          skippast_rbr(msfile) ;
                                      END
                                    }
                                    if(token6){free(token6) ; token6 = NULL; }
                                    break ;
/*
 *  Level 6  Volume Curve
 */
                                  CASE("VolumeCurve")
                                    skip_lbr(msfile) ;
                                    smsvol_node = &(smsstr_node->smsvol_node) ;
                                    for(;;){
                                      if(token6)free(token6) ;
                                      token6 = ctoken(msfile) ;
                                      if(is_rbr(token6)) break ;
                                      SWITCH(token6)
                                        CASE("Skip")
                                        CASE("Comment")
                                          skip_lbr(msfile);
                                          skippast_rbr(msfile) ;
                                          break ;
                                        CASE("SpeedControlled")
                                          smsvol_node->speed_control = 1 ;
                                          break ;
                                        CASE("DistanceControlled")
                                          smsvol_node->dist_control = 1 ;
                                          break ;
                                        CASE("Variable1Controlled")
                                          smsvol_node->var1_control = 1 ;
                                          break ;
                                        CASE("Variable2Controlled")
                                          smsvol_node->var2_control = 1 ;
                                          break ;
                                        CASE("Variable3Controlled")
                                          smsvol_node->var3_control = 1 ;
                                          break ;
                                        CASE("Granularity")
                                          skip_lbr(msfile) ;
                                          smsvol_node->granuality = dtoken(msfile) ; ;
                                          skip_rbr(msfile) ;
                                          break ;
                                        CASE("CurvePoints")
                                          skip_lbr(msfile) ;
                                          n = smsvol_node->n_points = itoken(msfile) ;
                                          smsvol_node->xxx_array =
                                                        (double *)malloc(n*sizeof(double)) ;
                                          smsvol_node->vol_array =
                                                        (double *)malloc(n*sizeof(double)) ;
                                          for(i=0;i<n;i++) {
                                            smsvol_node->xxx_array[i] = dtoken(msfile) ;
                                            smsvol_node->vol_array[i] = dtoken(msfile) ;
                                          }
                                          skippast_rbr(msfile) ;  //  Some files have extra terms
                                          break ;
                                        DEFAULT
                                          printf("  Routine '%s' error\n",my_name) ;
                                          printf("  Unrecognised level 6 token.  Token = '%s'\n",token6) ;
                                          skip_lbr(msfile)   ;
                                          skippast_rbr(msfile) ;
                                      END
                                    }
                                    if(token6){free(token6) ; token6 = NULL; }
                                    break ;
/*
 *  Level 6  Frequency Curve
 */

                                  CASE("FrequencyCurve")
                                    skip_lbr(msfile) ;
                                    smsfrq_node = &(smsstr_node->smsfrq_node) ;
                                    for(;;){
                                      if(token6)free(token6) ;
                                      token6 = ctoken(msfile) ;
                                      if(is_rbr(token6)) break ;
                                      SWITCH(token6)
                                        CASE("Skip")
                                        CASE("Comment")
                                          skip_lbr(msfile);
                                          skippast_rbr(msfile) ;
                                          break ;
                                        CASE("DistanceControlled")
                                          smsfrq_node->dist_control = 1 ;
                                          break ;
                                        CASE("SpeedControlled")
                                          smsfrq_node->speed_control = 1 ;
                                          break ;
                                       CASE("Variable1Controlled")
                                          smsfrq_node->var1_control = 1 ;
                                          break ;
                                        CASE("Variable2Controlled")
                                          smsfrq_node->var2_control = 1 ;
                                          break ;
                                        CASE("Variable3Controlled")
                                          smsfrq_node->var3_control = 1 ;
                                          break ;
                                         CASE("Granularity")
                                          skip_lbr(msfile) ;
                                          smsfrq_node->granuality = dtoken(msfile) ; ;
                                          skip_rbr(msfile) ;
                                          break ;
                                        CASE("CurvePoints")
                                          skip_lbr(msfile) ;
                                          n = smsfrq_node->n_points = itoken(msfile) ;
                                          smsfrq_node->xxx_array =
                                                        (double *)malloc(n*sizeof(double)) ;
                                          smsfrq_node->frq_array =
                                                        (double *)malloc(n*sizeof(double)) ;
                                          for(i=0;i<n;i++) {
                                            smsfrq_node->xxx_array[i] = dtoken(msfile) ;
                                            smsfrq_node->frq_array[i] = dtoken(msfile) ;
                                          }
                                          skip_rbr(msfile) ;
                                          break ;
                                        DEFAULT
                                          printf("  Routine '%s' error\n",my_name) ;
                                          printf("  Unrecognised level 6 token.  Token = '%s'\n",token5) ;
                                          skip_lbr(msfile)   ;
                                          skippast_rbr(msfile) ;
                                      END
                                    }
                                    if(token6){free(token6) ; token6 = NULL; }
                                    break ;
                                  DEFAULT
                                    printf("  Routine '%s' error\n",my_name) ;
                                    printf("  Unrecognised level 5 token.  Token = '%s'\n",token5) ;
                                    skip_lbr(msfile)   ;
                                    skippast_rbr(msfile) ;
                                END
                              }
                              if(token4){free(token4) ; token4 = NULL; }
                              break ;
                            DEFAULT
                              printf("  Routine '%s' error\n",my_name) ;
                              printf("  Unrecognised level 4 token.  Token = '%s'\n",token4) ;
                              skip_lbr(msfile)   ;
                              skippast_rbr(msfile) ;
                          END
                        }
                        if(token4){free(token4) ; token4 = NULL; }
                        break ;
                      DEFAULT
                      printf("  Routine '%s' error\n",my_name) ;
                      printf("  Unrecognised level 3 token.  Token = '%s'\n",token3) ;
                      skip_lbr(msfile)   ;
                      skippast_rbr(msfile) ;
                    END
                  }
                  if(token3){free(token3) ; token3 = NULL; } ;
                  break ;
                DEFAULT
                  if(is_eof(token2)){          // Final token ')' is missing
                    return_token(")", msfile) ;
                    break ;
                  }
                  printf("  Routine '%s' error\n",my_name) ;
                  printf("  Unrecognised level 2 token.  Token = '%s'\n",token2) ;
                  skip_lbr(msfile)   ;
                  skippast_rbr(msfile) ;
                  break ;
              END
            }
            if(token2){free(token2) ; token2 = NULL; } ;
            break ;
          DEFAULT
            if(is_eof(token1)){          // Final token ')' is missing
              return_token(")", msfile) ;
              break ;
            }
            printf("  Routine '%s' error\n",my_name) ;
            printf("  Unrecognised level 1 token.  Token = '%s'\n",token1) ;
            skip_lbr(msfile)   ;
            skippast_rbr(msfile) ;
            break ;
        END
      }
      if(token1)free(token1) ; token1 = NULL ;
#endif
      close_msfile(msfile) ;
      return 0 ;
}

int initialise_sms_node(SMS_Node  *sms_node){

  int i ;
  SmsGrpNode  *smsgrp_node ;

      sms_node->name         = NULL ;
      sms_node->sms_file     = NULL ;
      sms_node->sms_fullfilename = NULL ;
      sms_node->needed       = 0 ;
      sms_node->loaded       = 0 ;
      for(i=0;i<4;i++){
        smsgrp_node = &sms_node->smsgrp_node[i] ;
        smsgrp_node->active.cab_cam = 0 ;
        smsgrp_node->active.ext_cam = 0 ;
        smsgrp_node->active.pas_cam = 0 ;
        smsgrp_node->deacti.cab_cam = 0 ;
        smsgrp_node->deacti.ext_cam = 0 ;
        smsgrp_node->deacti.pas_cam = 0 ;
        smsgrp_node->snd_stream = NULL ;
        smsgrp_node->volume     = 0.0  ;
        smsgrp_node->stereo        = 0 ;
        smsgrp_node->loadall_waves = 0 ;
        smsgrp_node->n_streams     = 0 ;
      }
      return 0 ;
}

