/*
 *******************************************************************************
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File signal_file.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   This file contains routines for reading file:
 *                               ".../Routes/<route>/sigcfg.dat".
 *   and populating the signal database.
 *
 * *****************************************************************************
 *
 *   Notes on "sigcfg.dat"
 *
 *   MSTS has files sigcfg.dat and sigscr.dat in each Routes directory.
 *     File sigcfg.dat describes each of the signals.
 *     File sigscr.dat contains scripts/mini-programs needed to set each
 *       set of signals
 *
 *   The file "sigcfg.data" is (usually?) an uncompressed text file.  It is
 *   initially opened using routine ?? to strip the headers, if necessary
 *   uncompress the file, and set flags for reading using teh routines in
 *   file "token.c".
 *
 *   The data is read and saved in "SignalDB" structure "raw_signal_db".  The
 *   structure is defined in "signals.h", the database is defined in "zr.h".
 *
 * *****************************************************************************
 */
void raw_signal_db_init(void);


int read_signals_db(void){

  int  iret, ip = 0 ;
  int  i, l, n1, m1, n2, m2, n3, m3, n4, m4 ;
  MSfile  msfile0             ;  // Strrage for file data
  MSfile  *msfile = &msfile0  ;
  FILE *fp ;
  LightTex  *light_tex, *light_tex_end = NULL ;
  LightTab  *light_tab  ;
  SigType   *sig_type   ;
  SigLight  *sig_light  ;
  SigDraw   *sig_draw   ;
  SigAspect *sig_aspect ;
  SigShape  *sig_shape  ;
  SigSubObj *sig_subobj ;
  SigScriptFile *sig_scrfile, *sig_scrfile_prev = NULL  ;

  char *token1 = NULL,
      *token2 = NULL,
      *token3 = NULL,
      *token4 = NULL,
      *token5 = NULL,
      *string = NULL, *string2;
  char my_name[] ="read_signals_db" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
 *  The file sigcfg.dat should be in the main "Route" directory (ORRoutedir))
 */
      l = strlen(ORroutedir) + strlen("sigcfg.dat") + 1;
      string = (char *) malloc(l) ;
      strcpy(string,ORroutedir)   ;
      strcat(string,"sigcfg.dat") ;
// Check file exists
      iret = zr_find_msfile2(string) ;
      if(iret){
        printf("    Routine %s.  Data file does not exist\n",my_name) ;
        printf("    Filename = %s\n",string)  ;
        return 0 ;
      }
      if(ip)printf("  Routine %s.  Opening file %s\n",my_name,string) ;
      fp = gopen(string,"r") ;
      if(fp == NULL){
        printf("  Unable to find sigcfg.dat file\n") ;
        printf("     Full filename = %s\n",string) ;
        printf("  Program stopping ...\n") ;
        exit(1) ;
      }
      gclose(fp) ;
/*
*  Open as a normal msfile tagged file
*/
      iret = open_msfile(string, msfile, 0, 0) ;
      if(iret){
        printf("  ERROR : routine 'open_msfile' returns error %i\n",iret) ;
        printf("          while opening file %s\n",string) ;
        printf("  Program stopping ...\n") ;
        exit(1) ;
      }
      if(ip)printf(" File found: %s\n", string) ;
      free(string) ;
/*
 *  Initialise raw signal database
 */
      raw_signal_db_init() ;
/*
 *  Main Loop
 */
      for(;;){
        if(token1)free(token1) ;
        token1 = ctoken(msfile) ;
        if(is_rbr(token1)) break ;
        if(!strcmp(token1,eof_mark)) break ;
        if(ip)printf("  AA  token 1 = %s \n",token1) ;
        SWITCH(token1)
/*
 *  Top level 1.  Light textures
 *  Defines a single light texture, used as background to draw lit lights onto signals
 */
          CASE("LightTextures")
            skip_lbr(msfile) ;
            raw_signal_db->n_light_tex = n1 = itoken(msfile) ;
//            raw_signal_db->light_tex = (LightTex *)malloc(n1*sizeof(LightTex)) ;
            m1 = 0 ;
//  Loop over Light Textures
            for(;;){
              token2 = new_tmp_token(msfile) ;
              if(ip)printf("    BB  token 2  = %s \n",token2) ;
              if(is_rbr(token2))break ;
              SWITCH(token2)
                CASE("LightTex")
                  skip_lbr(msfile) ;
//                  if(m1>=n1){
//                    printf("  ERROR in routine %s\n",my_name) ;
//                    printf("  Too many Light Textures") ;
//                    m1-- ;
//                  }
//                  light_tex = &(raw_signal_db->light_tex[m1++]);
                  light_tex = (LightTex *)malloc(sizeof(LightTex)) ;
                  if(raw_signal_db->light_tex == NULL){
                     raw_signal_db->light_tex = light_tex ;
                  }else light_tex_end->next = light_tex ;
                  light_tex_end   = light_tex ;
                  light_tex->next = NULL ;

                  light_tex->name     = ctoken(msfile) ;
                  light_tex->ace_file = ctoken(msfile) ;
                  for(i=0;i<4;i++){
                    light_tex->flag[i] = dtoken(msfile) ;
                  }
                  skip_rbr(msfile) ;
                  zr_str2lcnqne(light_tex->name) ;
                  zr_str2lcnqne(light_tex->ace_file) ;
                  break ;
                DEFAULT
                  printf("  ERROR in routine %s\n",my_name) ;
                  printf("  Unrecognised token.  Token = '%s'",token2) ;
                  exit(0) ;
              END
            } // End of loop over Light Textures
            break ;
/*
 *  Top level 2.  Light Table
 */
          CASE("LightsTab")
            skip_lbr(msfile) ;
            raw_signal_db->n_light_tab = n2 = itoken(msfile) ;
            raw_signal_db->light_tab = (LightTab *)malloc(n2*sizeof(LightTab)) ;
            m2 = 0 ;
//  Loop over Lights Table Entries
            for(;;){
              token2 = new_tmp_token(msfile) ;
              if(ip)printf("    BB  token 2  = %s \n",token2) ;
              if(is_rbr(token2))break ;
              SWITCH(token2)
                CASE("LightsTabEntry")
                  skip_lbr(msfile) ;
                  if(m2>=n2){
                    printf("  ERROR in routine %s\n",my_name) ;
                    printf("  Too many Light Table entries") ;
                    m2-- ;
                  }
                  light_tab = &(raw_signal_db->light_tab[m2++]);
                  light_tab->name = ctoken(msfile) ;
                  token3 = new_tmp_token(msfile) ;
                  if(strcmp(token3,"colour")){
                    printf("  ERROR in routine %s\n",my_name) ;
                    printf("  Unrecognised token.  Token = '%s'\n",token3) ;
                    printf("            while expecting 'colour' token\n") ;
                    exit(0) ;
                  }
                  skip_lbr(msfile) ;
//  Convert colours for OpenGL i.e. float in order R, G, B, A
                  light_tab->colour[3] = itoken(msfile)/255.0 ;
                  light_tab->colour[0] = itoken(msfile)/255.0 ;
                  light_tab->colour[1] = itoken(msfile)/255.0 ;
                  light_tab->colour[2] = itoken(msfile)/255.0 ;
                  skip_rbr(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                DEFAULT
                  printf("  ERROR in routine %s\n",my_name) ;
                  printf("  Unrecognised token.  Token = '%s'",token2) ;
                  exit(0) ;
              END
            }// End Loop over Lights Table Entries
            break ;
/*
 *  Top level 3.  Signal Types
 */
          CASE("SignalTypes")
            skip_lbr(msfile) ;
            raw_signal_db->n_sig_types = n2 = itoken(msfile) ;
            raw_signal_db->sig_type = (SigType *)malloc(n2*sizeof(SigType)) ;
            m2 = 0 ;
//  Loop over Signal Types
            for(;;){
              token2 = new_tmp_token(msfile) ;
              if(ip)printf("    BB  token2  = %s \n",token2) ;
              if(is_rbr(token2)){
                if(m2 != n2){                       //  Count error i.e. USA1
                  raw_signal_db->n_sig_types = m2;
                }
                break ;
              }
              SWITCH(token2)
                CASE("SignalType")
                  skip_lbr(msfile) ;
                  if(m2>=n2){
                    printf("  ERROR in routine %s\n",my_name) ;
                    printf("  Too many SignalTypes") ;
                    m2-- ;
                  }
//  Initialise this Signal Type
                  sig_type = &(raw_signal_db->sig_type[m2++]);
                  sig_type->name = ctoken(msfile) ;
                  sig_type->msts_function = 0 ;
                  sig_type->semaphore = 0 ;        //  No Semaphore
                  sig_type->no_gantry = 0 ;        //  OK for gantry
                  sig_type->abs       = 0 ;        //  Unknown
                  sig_type->semaphore_info = 1.0 ; //  Default 1s flashing
                  sig_type->light_tex_ref  = NULL ;
                  sig_type->light_tex_node = NULL ;

                  sig_type->n_sig_lights  = 0 ;
                  sig_type->n_sig_dstates = 0 ;
                  sig_type->n_sig_aspects = 0 ;
                  sig_type->sig_light     = NULL ;
                  sig_type->sig_draw      = NULL ;
                  sig_type->sig_aspect    = NULL ;
                  sig_type->sig_script    = NULL ;
                  if(ip)printf(" == Signal Type Name = %s\n",sig_type->name) ;

//  Loop collecting Properties of this Signal Type
//  Signal enums defined in 'enum.h'.
                  for(;;){
                    token3 = new_tmp_token(msfile) ;
                    if(ip)printf("      CC  token3  = %s \n",token3) ;
                    if(is_rbr(token3))break ;
                    SWITCH(token3)
                      CASE("SignalFnType")
                        skip_lbr(msfile) ;
                        string = new_tmp_token(msfile) ;
                        string2 = zr_to_upper(string)  ;
                        if(!strcmp(string2,"NORMAL")){
                          sig_type->msts_function = SIG_NORMAL ;
                        }else if(!strcmp(string2,"DISTANCE")){
                          sig_type->msts_function = SIG_NORMAL ;
                        }else if(!strcmp(string2,"REPEATER")){
                          sig_type->msts_function = SIG_REPEATER ;
                        }else if(!strcmp(string2,"SHUNTING")){
                          sig_type->msts_function = SIG_SHUNTING ;
                        }else if(!strcmp(string2,"INFO")){
                          sig_type->msts_function = SIG_INFO ;
                        }else if(!strcmp(string2,"SIG_SPEED")){
                          sig_type->msts_function = SIG_SPEED ;
                        }else if(!strcmp(string2,"ALERT")){
                          sig_type->msts_function = SIG_ALERT ;
                        }else{
                          printf("  Routine %s error\n",my_name) ;
                          printf("  Unrecognised signal type :: %s\n",string) ;
                          printf("  Program stopping ... \n") ;
                          exit(0) ;
                        }
                        skip_rbr(msfile) ;
                        break ;
                      CASE("SignalLightTex")
                        skip_lbr(msfile) ;
                        sig_type->light_tex_ref = ctoken(msfile) ;
                        zr_str2lcnqne(sig_type->light_tex_ref) ;  // Strip quotes, convert to l.c.
                        skip_rbr(msfile) ;
                        break;
                      CASE("SigFlashDuration")
                        skip_lbr(msfile) ;
                        sig_type->sig_flash[0] = dtoken(msfile) ;
                        sig_type->sig_flash[1] = dtoken(msfile) ;
                        skip_rbr(msfile) ;
                        break;
                        break;
                      CASE("SemaphoreInfo")
                        skip_lbr(msfile) ;
                        sig_type->semaphore_info = dtoken(msfile) ;
                        skip_rbr(msfile) ;
                        break;
                      CASE("SignalFlags")
                        skip_lbr(msfile) ;
                        string = new_tmp_token(msfile) ;
                        if(!strcmp(string,"SEMAPHORE")){
                          sig_type->semaphore = 1 ;
                        }else if(!strcmp(string,"ABS")){
                          sig_type->abs       = 1 ;
                        }else if(!strcmp(string,"NO_GANTRY")){
                          sig_type->no_gantry = 1 ;
                        }else{
                          printf("  Routine %s error\n",my_name) ;
                          printf("  Unrecognised signal flag :: %s\n",string) ;
                          printf("  Program stopping ... \n") ;
                          exit(0) ;
                        }
                        skip_rbr(msfile) ;
                        break;
                      CASE("SignalLights")
                        skip_lbr(msfile) ;
                        sig_type->n_sig_lights = n3 = itoken(msfile) ;
                        sig_type->sig_light = (SigLight *)
                                                 malloc(n3*sizeof(SigLight)) ;
                        for(i=0;i<n3;i++){
                          sig_light = &(sig_type->sig_light[i]) ;
                          sig_light->index = -1 ;
                          sig_light->name  = NULL ;
                          sig_light->posn[0] = 0.0 ;
                          sig_light->posn[1] = 0.0 ;
                          sig_light->posn[2] = 0.0 ;
                          sig_light->radius           = 0.0  ;
                          sig_light->semaphore_change = 0    ;
                          sig_light->light_tab        = NULL ;
                        }
                        m3 = 0 ;
//  Loop over Lights of this Signal Type
                        for(;;){
                          token4 = new_tmp_token(msfile) ;
                          if(ip)printf("        DD  token4  = %s \n",token4) ;
                          if(is_rbr(token4)){
                            sig_type->n_sig_lights = n3 ;
                            break ;
                          }
                          SWITCH(token4)
                            CASE("SignalLight")
                              skip_lbr(msfile) ;
                              if(m3>=n3){
                                printf("  ERROR in routine %s\n",my_name) ;
                                printf("  Too many Signal Lights") ;
                                m3-- ;
                              }
                              sig_light = &(sig_type->sig_light[m3++]) ;
                              sig_light->index = itoken(msfile)  ;
                              sig_light->name  = ctoken(msfile)  ;
//  Loop over properties of this Light
                              for(;;){
                                token5 = new_tmp_token(msfile) ;
                                if(ip)printf("          EE  token5  = %s \n",token5) ;
                                if(is_rbr(token5))break ;
                                SWITCH(token5)
                                  CASE("Position")
                                    skip_lbr(msfile) ;
                                    for(i=0;i<3;i++){
                                       sig_light->posn[i] = dtoken(msfile) ;
                                    }
                                    skip_rbr(msfile) ;
                                    break ;
                                  CASE("Radius")
                                    skip_lbr(msfile) ;
                                    sig_light->radius = dtoken(msfile) ;
                                    skip_rbr(msfile) ;
                                    break ;
                                  CASE("SignalFlags")
                                    skip_lbr(msfile) ;
                                    for(;;){
                                      string = new_tmp_token(msfile) ;
                                      if(is_rbr(string)) break ;
                                      if(!strcmp(string,"SEMAPHORE_CHANGE")){
                                        sig_light->semaphore_change = 1 ;
                                      }else{
                                        printf("  Routine %s error\n",my_name) ;
                                        printf("  Unrecognised signal flag :: %s\n",string) ;
                                        printf("  Program stopping ... \n") ;
                                        exit(0) ;
                                      }
                                    }
                                    break ;
                                  DEFAULT
                                    printf("  Routine %s error\n",my_name) ;
                                    printf("  Unrecognised signal light token :: %s\n",token5) ;
                                    printf("  Program stopping ... \n") ;
                                    exit(0) ;
                                END
                              }   //  End loop oer light properties
                              break ;
                            DEFAULT
                              printf("  Routine %s error\n",my_name) ;
                              printf("  Unrecognised signal lights token :: %s\n",token5) ;
                              printf("  Program stopping ... \n") ;
                              exit(0) ;
                          END
                        }  // End loop over Lights of this Signal Type
                        break ;
                      CASE("SignalDrawStates")
                        skip_lbr(msfile) ;
                        sig_type->n_sig_dstates = n3 = itoken(msfile) ;
                        sig_type->sig_draw = (SigDraw *)malloc(n3*sizeof(SigDraw)) ;
                        for(i=0;i<n3;i++){
                          sig_draw = &(sig_type->sig_draw[i]);
                          sig_draw->index           = -1 ;
                          sig_draw->name            = NULL ;
                          sig_draw->n_draw_lights   = 0 ;
                          sig_draw->draw_light_i    = NULL ;
                          sig_draw->semaphore_posn  = 0    ;
                          sig_draw->flashing        = 0    ;
                          sig_draw->draw_light      = NULL ;
                        }
                        m3 = 0 ;
// Loop over Signal Draw States
                        for(;;){
                          token4 = new_tmp_token(msfile) ;
                          if(ip)printf("        DD  token4  = %s \n",token4) ;
                          if(is_rbr(token4))break ;
                          SWITCH(token4)
                            CASE("SignalDrawState")
                              skip_lbr(msfile) ;
                              if(m3>=n3){
                                printf("  ERROR in routine %s\n",my_name) ;
                                printf("  Too many Signal Lights") ;
                                m3-- ;
                              }
                              sig_draw = &(sig_type->sig_draw[m3++]) ;
                              sig_draw->index = itoken(msfile) ;
                              sig_draw->name  = ctoken(msfile) ;
                              sig_draw->n_draw_lights = 0 ;
//  Loop over properties of this Signal Draw State
                              for(;;){
                                token5 = new_tmp_token(msfile) ;
                                if(ip)printf("          EE  token5  = %s \n",token5) ;
                                if(is_rbr(token5))break ;
                                SWITCH(token5)
                                  CASE("DrawLights")
                                    skip_lbr(msfile) ;
                                    sig_draw->n_draw_lights = n4 = itoken(msfile) ;
                                    sig_draw->draw_light_i = (int *)malloc(n4*sizeof(int)) ;
                                    for(i=0;i<n4;i++){
                                      string = new_tmp_token(msfile) ;
                                      if(ip)printf("            GG  string  = %s \n",string) ;
                                      if(strcmp(string,"DrawLight")){
                                        printf("  Routine %s error\n",my_name) ;
                                        printf("  Unrecognised DrawLight token :: %s\n",string) ;
                                        printf("  Program stopping ... \n") ;
                                        exit(0) ;
                                      }
                                      skip_lbr(msfile) ;
                                      sig_draw->draw_light_i[i] = itoken(msfile) ;
                                      string = new_tmp_token(msfile) ;
                                      if(ip)printf("              HH  string  = %s \n",string) ;
                                      if(!strcmp(string,"SignalFlags")){
                                        skip_lbr(msfile) ;
                                        for(;;){
                                          string = new_tmp_token(msfile) ;
                                          if(ip)printf("              II  string  = %s \n",string) ;
                                          if(!is_rbr(string))break ;
                                          SWITCH(string)
                                            CASE("FLASHING")
                                              sig_draw->flashing = 1 ;
                                              break ;
                                            DEFAULT
                                              printf("  Routine %s error\n",my_name) ;
                                              printf("  Unrecognised DrawLight token :: %s\n",string) ;
                                              printf("  Program stopping ... \n") ;
                                              exit(0) ;
                                          END
                                        }
                                        skip_rbr(msfile) ;
                                      }else{
                                        if(ip)printf("              Return token  = %s \n",string) ;
                                        return_token(string,msfile) ;
                                      }
                                      skip_rbr(msfile) ;
                                    }
                                    skip_rbr(msfile) ;
                                    break ;
                                  CASE("SemaphorePos")
                                    skip_lbr(msfile) ;
                                    sig_draw->semaphore_posn = itoken(msfile) ;
                                    skip_rbr(msfile) ;
                                    break ;
                                  DEFAULT
                                    printf("  Routine %s error\n",my_name) ;
                                    printf("  Unrecognised Signal Draw State token:: %s\n",token5) ;
                                    printf("  Program stopping ... \n") ;
                                    exit(0) ;
                                END
                              }  // End loop over properties of Signal Draw State
                              break ;
                            DEFAULT
                              printf("  Routine %s error\n",my_name) ;
                              printf("  Unrecognised signal lights token :: %s\n",token4) ;
                              printf("  Program stopping ... \n") ;
                              exit(0) ;
                          END
                        }  //End Loop over Signal Draw States
                        break ;
                      CASE("SignalAspects")
                        skip_lbr(msfile) ;
                        sig_type->n_sig_aspects = n3 = itoken(msfile) ;
                        sig_type->sig_aspect = (SigAspect *)malloc(n3*sizeof(SigAspect)) ;
                        for(i=0;i<n3;i++){
                          sig_aspect = &(sig_type->sig_aspect[i]) ;
                          sig_aspect->status  = -1 ;
                          sig_aspect->draw_state_name = NULL ;
                          sig_aspect->mph     = 0.0 ;
                          sig_aspect->asap    = 0   ;
                          sig_aspect->sig_draw = NULL ;
                        }
                        m3 = 0;
// Loop over Signal aspects
                        for(;;){
                          token4 = new_tmp_token(msfile) ;
                          if(ip)printf("        DD  token4  = %s \n",token4) ;
                          if(is_rbr(token4)){
                            sig_type->n_sig_aspects = m3 ;  //  Allow for sigcfg.dat error
                            break ;
                          }
                          SWITCH(token4)
                            CASE("SignalAspect")
                              skip_lbr(msfile) ;
                              if(m3>=n3){
                                printf("  ERROR in routine %s\n",my_name) ;
                                printf("  Too many Signal Lights") ;
                                m3-- ;
                              }
                              sig_aspect = &(sig_type->sig_aspect[m3++]) ;
                              string = new_tmp_token(msfile) ;
                              if(ip)printf("          EE  token4  = %s \n",string) ;
                              if(!strcmp(string,"STOP"))                 sig_aspect->status = SIG_STOP ;
                              else if(!strcmp(string,"STOP_AND_PROCEED"))sig_aspect->status = SIG_STOP_AND_PROCEED ;
                              else if(!strcmp(string,"RESTRICTING"))     sig_aspect->status = SIG_RESTRICTING ;
                              else if(!strcmp(string,"APPROACH_1"))      sig_aspect->status = SIG_APPROACH_1 ;
                              else if(!strcmp(string,"APPROACH_2"))      sig_aspect->status = SIG_APPROACH_2 ;
                              else if(!strcmp(string,"APPROACH_3"))      sig_aspect->status = SIG_APPROACH_3 ;
                              else if(!strcmp(string,"CLEAR_1"))         sig_aspect->status = SIG_CLEAR_1 ;
                              else if(!strcmp(string,"CLEAR_2"))         sig_aspect->status = SIG_CLEAR_2 ;
                              else{
                                printf("  Routine %s error\n",my_name) ;
                                printf("  Unrecognised Signal Aspect :: string = %s\n",string) ;
                                printf("  Program stopping ... \n") ;
                                exit(0) ;
                              }
                              sig_aspect->draw_state_name = ctoken(msfile) ;
                              sig_aspect->mph             = 0.0 ;
                              sig_aspect->kph             = 0.0 ;
                              for(;;){
                                string = new_tmp_token(msfile) ;
                                if(ip)printf("            FF  token4  = %s \n",string) ;
                                if(is_rbr(string))break;
                                SWITCH(string)
                                  CASE("SpeedMPH")
                                    skip_lbr(msfile) ;
                                    sig_aspect->mph    = dtoken(msfile) ;
                                    skip_rbr(msfile) ;
                                    break ;
                                  CASE("SpeedKPH")
                                    skip_lbr(msfile) ;
                                    sig_aspect->kph    = dtoken(msfile) ;
                                    skip_rbr(msfile) ;
                                    break ;
                                  CASE("SignalFlags")
                                    skip_lbr(msfile) ;
                                    string = new_tmp_token(msfile) ;
                                    if(ip)printf("              HH  token4  = %s \n",string) ;
                                    SWITCH(string)
                                      CASE("ASAP")
                                        sig_aspect->asap  = 1 ;
                                        break;
                                      DEFAULT
                                        printf("  Routine %s error\n",my_name) ;
                                        printf("  Unrecognised Signal FLAG :: string = %s\n",string) ;
                                        printf("  Program stopping ... \n") ;
                                        exit(0) ;
                                    END
                                    skip_rbr(msfile) ;
                                    break ;
                                  DEFAULT
                                    printf("  Routine %s error\n",my_name) ;
                                    printf("  Unrecognised Signal Aspect Component :: string = %s\n",string) ;
                                    printf("  Program stopping ... \n") ;
                                    exit(0) ;
                                END
                              }
//                              skip_rbr(msfile) ;
                              break ;
                            DEFAULT
                              printf("  Routine %s error\n",my_name) ;
                              printf("  Unrecognised Signal Aspect token :: %s\n",token4) ;
                              printf("  Program stopping ... \n") ;
                              exit(0) ;
                          END
                        }  // End Loop over Signal Aspects
                        break ;
                      CASE("SignalNumClearAhead")
                        skip_lbr(msfile) ;
                        sig_type->n_clear_ahead = itoken(msfile) ;
                        skip_rbr(msfile) ;
                        break ;
                      DEFAULT
                        printf("  Routine %s error\n",my_name) ;
                        printf("  Unrecognised Signal Type token :: %s\n",token3) ;
                        printf("  Program stopping ... \n") ;
                        exit(0) ;
                    END
                  }  //  End loop over properties of this Signal Type
                  break ;
                DEFAULT
                  break ;
              END
            }  // End Top level Loop over Signal Types
            break ;
          CASE("SignalShapes")
            skip_lbr(msfile) ;
            raw_signal_db->n_sig_shapes = n1 = itoken(msfile) ;
            raw_signal_db->sig_shape = (SigShape *)malloc(n1*sizeof(SigShape)) ;
            for(m1=0;m1<n1;m1++){
              sig_shape = &(raw_signal_db->sig_shape[m1]) ;
              token2 = new_tmp_token(msfile) ;
              if(ip)printf("    BB  token2  = %s \n",token2) ;
              SWITCH(token2)
                CASE("SignalShape")
                  skip_lbr(msfile) ;
                  sig_shape->shapefile = ctoken(msfile) ;
                  sig_shape->name      = ctoken(msfile) ;
                  string = new_tmp_token(msfile) ;
                  if(ip)printf("      CC  string  = %s \n",string) ;
                  if(strcmp(string,"SignalSubObjs")){
                    printf("  Routine %s error\n",my_name) ;
                    printf("  Unrecognised sub-token of %s\n","SignalShape") ;
                    printf("  Token is %s\n",string) ;
                    printf("  Program stopping ... \n") ;
                    exit(0) ;
                  }
                  skip_lbr(msfile) ;
                  sig_shape->n_sig_subobjs = n2 =itoken(msfile) ;
                  sig_shape->sig_subobj = (SigSubObj *)malloc(n2*sizeof(SigSubObj)) ;
                  for(i=0;i<n2;i++){
                    sig_subobj = &(sig_shape->sig_subobj[i]) ;
                    sig_subobj->index              = -1 ;
                    sig_subobj->name               = NULL ;
                    sig_subobj->full_name          = NULL ;
                    sig_subobj->sig_decor          = 0 ;
                    sig_subobj->sig_signal_head    = 0 ;
                    sig_subobj->sig_gradient_plate = 0 ;
                    sig_subobj->sig_number_plate   = 0 ;
                    sig_subobj->sig_user1          = 0 ;  // Used by AU_NSW_SW_SS route
                    sig_subobj->sig_user2          = 0 ;
                    sig_subobj->sig_user3          = 0 ;
                    sig_subobj->sig_user4          = 0 ;
                    sig_subobj->sig_optional       = 0 ;
                    sig_subobj->sig_default        = 0 ;
                    sig_subobj->sig_back_facing    = 0 ;
                    sig_subobj->sig_jn_link        = 0 ;
                    sig_subobj->n_sig_jn_lnk       = 0 ;
                    sig_subobj->sig_script_type     = NULL ;

                  }
                  for(m2=0;m2<n2;m2++){
                    sig_subobj = &(sig_shape->sig_subobj[m2]) ;
                    string = new_tmp_token(msfile) ;
                    if(ip)printf("        DD  string  = %s \n",string) ;
                    if(strcmp(string,"SignalSubObj")){
                      printf("  Routine %s error\n",my_name) ;
                      printf("  Unrecognised sub-token of %s\n","SignalSubObjs") ;
                      printf("  Token is %s\n",string) ;
                      printf("  Program stopping ... \n") ;
                      exit(0) ;
                    }
                    skip_lbr(msfile) ;
                    sig_subobj->index = itoken(msfile) ;
                    sig_subobj->name  = ctoken(msfile) ;
                    sig_subobj->full_name = ctoken(msfile) ;
                    for(;;){
                      token3 = new_tmp_token(msfile) ;
                      if(ip)printf("          EE  token3  = %s \n",token3) ;
                      if(is_rbr(token3))break ;
                      SWITCH(token3)
                        CASE("SigSubType")
                          skip_lbr(msfile) ;
                          string = new_tmp_token(msfile) ;
                          if(!strcmp(string,"SIGNAL_HEAD")){
//                            sig_subobj->sig_subtype |= 1 ;
                            sig_subobj->sig_signal_head = 1 ;
                          }else if(!strcmp(string,"GRADIENT_PLATE")){
//                            sig_subobj->sig_subtype |= 2 ;
                            sig_subobj->sig_gradient_plate = 1 ;
                          }else if(!strcmp(string,"NUMBER_PLATE")){
//                            sig_subobj->sig_subtype |= 4 ;
                            sig_subobj->sig_number_plate = 1 ;
                          }else if(!strcmp(string,"USER1")){
                            sig_subobj->sig_user1 = 1 ;
                          }else if(!strcmp(string,"USER2")){
                            sig_subobj->sig_user2 = 1 ;
                          }else if(!strcmp(string,"USER3")){
                            sig_subobj->sig_user3 = 1 ;
                          }else if(!strcmp(string,"USER4")){
                            sig_subobj->sig_user4 = 1 ;
                          }else if(!strcmp(string,"DECOR")){
                            sig_subobj->sig_decor = 1 ;
                          }else {
                            printf("  Routine %s error\n",my_name) ;
                            printf("  Unrecognised sub-token of %s\n","SigSubType") ;
                            printf("  Token is %s\n",string) ;
                            printf("  Program stopping ... \n") ;
                            exit(0) ;
                          }
                          skip_rbr(msfile) ;
                          break ;
                        CASE("SignalFlags")
                          skip_lbr(msfile) ;
                          for(;;){
                            string = new_tmp_token(msfile) ;
                            if(is_rbr(string)) break ;
                            string2 = zr_to_upper(string) ;
                            if(!strcmp(string2,"DEFAULT")){
                              sig_subobj->sig_default = 1 ;
                            }else if(!strcmp(string2,"JN_LINK")){
                              sig_subobj->sig_jn_link = 1 ;
                              if(ip){
                                printf(" &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n") ;
                                printf("  sig_jn_link     = %i\n",sig_subobj->sig_jn_link) ;
                                printf(" &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n") ;
                              }
                            }else if(!strcmp(string2,"BACK_FACING")){
                              sig_subobj->sig_back_facing = 1 ;
                            }else if(!strcmp(string2,"OPTIONAL")){
                              sig_subobj->sig_optional = 1 ;
                            }else{
                              printf("  Routine %s error\n",my_name) ;
                              printf("  Unrecognised sub-token of %s\n","SignalFlags") ;
                              printf("  Token is %s\n",string) ;
                              printf("  Program stopping ... \n") ;
                              exit(0) ;
                            }
                          }
                          break ;
                        CASE("SigSubSType")
                          skip_lbr(msfile) ;
                          sig_subobj->sig_script_type = ctoken(msfile) ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("SigSubJnLinkIf")
                          skip_lbr(msfile) ;
                          sig_subobj->n_sig_jn_lnk  = itoken(msfile) ;
                          sig_subobj->sig_jn_lnk_a[0] = itoken(msfile) ;
                          if(sig_subobj->n_sig_jn_lnk>1){
                            sig_subobj->sig_jn_lnk_a[1] = itoken(msfile) ;
                          }
                          if(ip){
                            printf(" &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n") ;
                            printf("  n_sig_jn_lnk     = %i\n",sig_subobj->n_sig_jn_lnk) ;
                            printf("  n_sig_jn_lnk_a[] = %i %i\n",
                                      sig_subobj->sig_jn_lnk_a[0],sig_subobj->sig_jn_lnk_a[1]) ;
                            printf(" &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n") ;
                          }
                          skip_rbr(msfile) ;
                          break ;
                        DEFAULT
                          printf("  Routine %s error\n",my_name) ;
                          printf("  Unrecognised sub-token of %s\n","SignalSubObj") ;
                          printf("  Token is %s\n",string) ;
                          printf("  Program stopping ... \n") ;
                          exit(0) ;
                          break ;
                      END
                    }
                  }
                  skip_rbr(msfile) ;
                  break ;
                DEFAULT
                  printf("  Routine %s error\n",my_name) ;
                  printf("  Unrecognised SignalShape token :: %s\n",token3) ;
                  printf("  Program stopping ... \n") ;
                  exit(0) ;
              END
              skip_rbr(msfile) ;
            }
            skip_rbr(msfile) ;
            break ;
          CASE("ScriptFiles")
            skip_lbr(msfile) ;

//#ifdef ROUTE_AU_NSW_SW_SS
//            return 0 ;   //  For the moment the signals are too complicated
//#endif
/*
 * Loop over script file entries
 */
            for(;;){
              token2 = new_tmp_token(msfile) ;
              if(ip)printf("    Script Files section.  token 2  = %s \n",token2) ;
              if(is_rbr(token2))break ;
              SWITCH(token2)
                CASE("ScriptFile")
                  skip_lbr(msfile) ;
                  sig_scrfile = (SigScriptFile *)malloc(sizeof(SigScriptFile)) ;
                  sig_scrfile->scriptfile_name = ctoken(msfile) ;
                  sig_scrfile->next = NULL  ;
                  if(!raw_signal_db->script_file){
                    raw_signal_db->script_file = sig_scrfile ;
                    raw_signal_db->n_scripts   = 1 ;
                  }else{
                    sig_scrfile_prev->next = sig_scrfile ;
                    raw_signal_db->n_scripts++ ;
                  }
                  sig_scrfile_prev = sig_scrfile ;
                  skip_rbr(msfile) ;
                  printf("     ScriptFile : %i name = %s\n",
                         raw_signal_db->n_scripts, sig_scrfile->scriptfile_name) ;
                  break ;
                DEFAULT
                  printf("  Routine %s error\n",my_name) ;
                  printf("  Unrecognised ScriptFile token :: %s\n",token2) ;
                  printf("  Program stopping ... \n") ;
                  exit(0) ;
              END
            }
            skip_rbr(msfile) ;
            break ;
          DEFAULT
            printf("  Routine %s error\n",my_name) ;
            printf("  Unrecognised Top Loop token :: %s\n",token1) ;
            printf("  Program stopping ... \n") ;
            exit(0) ;
            break ;
        END
      }  //  End of Main Loop
      if(token1)free(token1) ;
      return 0;
}

void raw_signal_db_init(void){

      raw_signal_db->n_light_tex  = 0 ;
      raw_signal_db->n_light_tab  = 0 ;
      raw_signal_db->n_sig_types  = 0 ;
      raw_signal_db->n_sig_shapes = 0 ;
      raw_signal_db->n_scripts    = 0 ;

      raw_signal_db->light_tex    = NULL ;
      raw_signal_db->light_tab    = NULL ;
      raw_signal_db->sig_type     = NULL ;
      raw_signal_db->sig_shape    = NULL ;
      raw_signal_db->script_file  = NULL ;

      return ;
}
