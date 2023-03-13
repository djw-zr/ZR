
/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  switch.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for processing the points/switches at junctions
 *
 *==============================================================================
 */

/*
 *  Routine to change the switch in front of the player train
 */

int change_forward_switch_for_player_train(){

int           ip = 0 ;
int           n          ;
double        df         ;  // lenght of end wagons
TravellerNode *tff, tf   ;
TrkSector   *tn1, *tfn ;
WorldItem     *witem = NULL ;  // World item describing junction
char          *my_name = "change_forward_switch_for_player_train" ;

      if(!player_train) return 1 ;
      tff = player_train->first->traveller ;
      if(!tff)return 1 ;
      tf = *tff ;
      df = tf.wagon->raw_wagon->length ;
      if(df>0.0) trv_move(&tf, 0.5*df) ;
      tfn = tf.tn ;
      n = tfn->pin_to_section[tf.idirect ? 1 : 0] ;
      tn1 = &track_db.trk_sections_array[n-1]   ;  // Section in front
      if(tn1->branch != 0){
        witem = tn1->vector->world_item ;
        tn1->branch = (1 == tn1->branch) ? 2 : 1 ;
        witem->anim_value = (tn1->branch == tn1->straight) ? 0.0 : 0.5 ;
        if(ip){
          printf("  Routine %s,  Switch changed.",my_name) ;
          printf("    Forward switch uid  = %i\n",tn1->uid) ;
          printf("    New value of branch = %i\n",tn1->branch) ;
          printf("    Value of straight   = %i\n",tn1->straight) ;
          printf("    Animation value     = %f\n",witem->anim_value) ;
        }
      }
      return 0 ;
}

/*
 *  Routine to change the switch behind the player train
 */

int change_rearward_switch_for_player_train(){

int           ip = 0 ;
int           n          ;
double        df         ;  // lenght of end wagons
TravellerNode *tbf, tb   ;
TrkSector   *tn2, *tbn ;
WorldItem     *witem = NULL ;  // World item describing junction
char          *my_name = "change_rearward_switch_for_player_train" ;

      if(!player_train) return 1;
      tbf = player_train->last->traveller ;
      if(!tbf) return 1 ;
      tb = *tbf ;
      df = tb.wagon->raw_wagon->length ;
      if(df>0.0) trv_move(&tb, -0.5*df) ;
      tbn = tb.tn ;
      n = tbn->pin_to_section[tb.idirect ? 0 : 1] ;
      tn2 = &track_db.trk_sections_array[n-1]   ;  // Section in front
      if(tn2->branch != 0){
        witem = tn2->vector->world_item ;
        tn2->branch = (1 == tn2->branch) ? 2 : 1 ;
        witem->anim_value = (tn2->branch == tn2->straight) ? 0.0 : 0.5 ;
        if(ip){
          printf("  Routine %s,  Switch changed.",my_name) ;
          printf("    Rearward switch uid = %i\n",tn2->uid) ;
          printf("    New value of branch = %i\n",tn2->branch) ;
          printf("    Value of straight   = %i\n",tn2->straight) ;
          printf("    Animation value     = %f\n",witem->anim_value) ;
        }
      }
      return 0 ;
}

/*
 *  Routine to reset any switches that prevent progress in the
 *  track beyond the next switch and the next NORMAL signal.
 */

int clear_track_beyond_next_switch(){

int           i, n, m, iret, idirect, sig_direct, back_facing ;
int           ip = 0 ;
double        df         ;  // length of end wagons
//double        clear_dist  =   0.0 ;   //  Distance cleared
//double        clear_dist0 = 200.0 ;   //  Distance to clear
//double        sect_distance       ;   //  Length of section
TravellerNode *tff, tf   ;
TrkSector   *tn1, *tfn ;
TrkItem   *ti        ;
WorldItem     *witem     ;  // World item describing junction
SignalDB      *signal    ;
char          *my_name = "clear_track_beyond_next_switch" ;

      if(!player_train) return 1 ;       //  No player train
      tff = player_train->first->traveller ;
      if(!tff) return 1 ;                //  No traveller
      tf = *tff ;                        //  Create a traveller
      df = tf.wagon->raw_wagon->length ;
      if(df>0.0) trv_move(&tf, 0.5*df) ; //  Start from front of train
      tf.wagon = NULL ;                 //  Convert to ghost traveller
      tf.shape = NULL ;

      tfn = tf.tn ;                     //  Track index
      idirect = tf.idirect ;            //  Drection of train relative to track
      if(ip){
        printf("\n  Enter routine %s\n",my_name) ;
        printf("  Track section = %i, type = %i %s, idirect = %i\n",
             tfn->uid, tfn->type_of_node,
             token_trackdb[tfn->type_of_node],idirect) ;
        trv_print(&tf) ;
      }

      n = tfn->pin_to_section[idirect ? 1 : 0] ;
      if(ip)printf("  n = %i\n",n) ;
      tn1 = &track_db.trk_sections_array[n-1]       ;  // Section in front
      if(ip)printf("  Initial end section   = %i, type = %i %s\n",
             tn1->uid, tn1->type_of_node, token_trackdb[tn1->type_of_node]) ;
      if(tn1->type_of_node == END_SECTION){
        if(ip)printf("  Exit routine at END_SECTION\n") ;
        return 0 ;
      }
      if(ip)printf("  Move through junction :  Direction = %i\n",idirect ? 1 : -1) ;

      iret = trk_next(&tf,idirect ? 1 : -1) ;            //  Try to move through points
      if(iret == 1){
        if(ip)printf("  Exit routine :  trk_next found END_SECTION\n") ;
        return 0 ;
      }
      if(iret == 2){
        if(ip)printf("  Exit routine at First Junction : Points blocking route\n\n") ;
        return 1 ;
      } ;
/*
 *  Search following sections until a NORMAL signal or End of Track found
 *  Signals are track item SIGNALS.  See: trk_item_db.c "SignalItem".
 */

      tfn = tf.tn ;
      if(ip){
        printf("  ENTER MAIN LOOP OVER TRACK SECTIONS\n") ;
      }
      for(;;){
/*
 *  Process vector section
 */
        tfn = tf.tn ;
        n = tf.tn->trk_item_number ;
        if(ip){
          printf("\n  New Loop\n") ;
          printf("  Next track section AA = %i, type = %i %s\n",
              tfn->uid, tfn->type_of_node, token_trackdb[tfn->type_of_node]) ;
          trv_print(&tf) ;
          printf("  Traveller :: Tracknode %i, ivector %i, idirect %i, vect_position %f, sect_distance %f\n",
                tf.itrack, tf.ivector, tf.idirect, tf.vect_position, tf.sect_distance);
          printf("\n  Number of track items = %i\n",n) ;
        }
//        sect_distance = tf.sect_distance ;
/*
 *  Loop through track items in direction of train
 */
        idirect = tf.idirect ;
        for(i=0;i<n;i++){
          m = tf.tn->trk_item_list[tf.idirect ? i : n-1-i] ;
          ti = &(track_db.trk_items_array[m]) ;
          if(ip)printf("    Track Item %i, m = %i, distance = %f, type = %i %s\n",
                    i, m, ti->sect_distance,
                    ti->type_of_node, token_trackdb[ti->type_of_node]) ;
/*
 * Process signal - otherwise skip to next track item
 */
          if(ti->type_of_node != SIGNALS)continue ;

          signal = ti->signal ;
          if(signal->sig_type->msts_function != SIG_NORMAL)continue ;
//  If sig_direct == 0, signal points towards the start of the track section
          sig_direct = signal->trk_item->signal_direction ;
          back_facing = signal->sig_subobj->sig_back_facing ;
          if(ip)printf("  Found signal  train direction = %i, signal direction = %i, "
                 "back_facing = %i\n", idirect, sig_direct, back_facing) ;
          if(back_facing == (sig_direct == idirect)){
            if(ip)printf("  Found NORMAL signal\n") ;
            return 0;                          //  Success
          }
          if(ip){
            printf("  Found signal %i :: %s :: %s\n",
                   signal->uid, signal->shape_name, signal->type_name) ;
          }
        }
/*
 *  Print data on junction or end section
 */
        idirect = tf.idirect ;
        n = tfn->pin_to_section[idirect ? 1 : 0] ;
        tn1 = &track_db.trk_sections_array[n-1]       ;  // Section in front
        if(ip)printf("    Junction/End section   = %i, type = %i %s\n",
         tn1->uid, tn1->type_of_node, token_trackdb[tn1->type_of_node]) ;
        if(tn1->type_of_node == END_SECTION){
          if(ip)printf("  Reached END_SECTION\n") ;
          break ;
        }
        if(ip)printf("    Junction data : type_of_pin %i %i, section %i %i %i, info %i %i %i"
               " strait %i, branch %i\n",tn1->type_of_pin[0],tn1->type_of_pin[1],
               tn1->pin_to_section[0],tn1->pin_to_section[1],tn1->pin_to_section[2],
               tn1->pin_info[0],tn1->pin_info[1],tn1->pin_info[2],
               tn1->straight, tn1->branch) ;
/*
 *  Move beyond junction
 */
        if(ip)printf("\n    Move through junction AA :  Direction = %i\n",idirect ? 1 : -1) ;
        iret = trk_next(&tf,idirect ? 1 : -1) ;
        if(ip){
          printf("    Routine trk_next returned %i.  Current tracknode is ...\n",iret) ;
          printf("    Tracknode %i, ivector %i, idirect %i, vect_position %f, sect_distance %f\n",
               tf.itrack, tf.ivector, tf.idirect, tf.vect_position, tf.sect_distance);
          trv_print(&tf) ;
        }
        if(iret){
          if(ip){
            printf("\n    Junction blocking route.  Change junction\n") ;
            printf("      branch = %i\n",tn1->branch ) ;
            printf("    Junction data : type_of_pin %i %i, section %i %i %i, info %i %i %i"
                  " strait %i, branch %i\n",tn1->type_of_pin[0],tn1->type_of_pin[1],
                  tn1->pin_to_section[0],tn1->pin_to_section[1],tn1->pin_to_section[2],
                  tn1->pin_info[0],tn1->pin_info[1],tn1->pin_info[2],
                  tn1->straight, tn1->branch) ;
          }
          if(tn1->branch != 0){
            witem = tn1->vector->world_item ;
            if(tn1->branch == 1){
              tn1->branch =2 ;
              witem->anim_value = 0.5 ;
            }else{
              tn1->branch =1 ;
              witem->anim_value = 0.0 ;
            }
          }
          if(ip)printf("  Junction data : type_of_pin %i %i, section %i %i %i, info %i %i %i"
                 " strait %i, branch %i\n",tn1->type_of_pin[0],tn1->type_of_pin[1],
                 tn1->pin_to_section[0],tn1->pin_to_section[1],tn1->pin_to_section[2],
                 tn1->pin_info[0],tn1->pin_info[1],tn1->pin_info[2],
                 tn1->straight, tn1->branch) ;

          idirect = tf.idirect ;
          iret = trk_next(&tf,idirect ? 1 : -1) ;
          if(ip){
            printf("  Routine trn_next returned %i.  Current tracknode is ...\n",iret) ;
            printf("  Tracknode %i, ivector %i, idirect %i, vect_position %f, sect_distance %f\n",
                 tf.itrack, tf.ivector, tf.idirect, tf.vect_position, tf.sect_distance);
          }
          if(iret){
            printf("Routine %s error\n",my_name) ;
            printf(" ... while trying to change switch direction\n") ;
            exit(0) ;
          }
        }
//        clear_dist += sect_distance;
//        if(clear_dist > clear_dist0) return 0 ;
        idirect = tf.idirect ;
      }
      return 0 ;
}

int clear_track_beyond_previous_switch(){

int           i, n, m, iret, idirect, sig_direct, back_facing ;
int           ip = 0 ;
double        df         ;  // lenght of end wagons
//double        clear_dist  =   0.0 ;   //  Distance cleared
//double        clear_dist0 = 200.0 ;   //  Distance to clear
//double        sect_distance       ;   //  Length of section
TravellerNode *tff, tf   ;
TrkSector   *tn1, *tfn ;
TrkItem   *ti        ;
WorldItem     *witem     ;  // World item describing junction
SignalDB      *signal    ;
char          *my_name = "clear_track_beyond_next_switch" ;

      if(!player_train) return 1 ;       //  No player train
      tff = player_train->last->traveller ;
      if(!tff) return 1 ;                //  No traveller
      tf = *tff ;                        //  Create a traveller
      df = tf.wagon->raw_wagon->length ;
      if(df>0.0) trv_move(&tf,-0.5*df) ; //  Start from back of train
      tf.wagon = NULL ;                  //  Convert to ghost traveller
      tf.shape = NULL ;

      tfn = tf.tn ;
      idirect = tf.idirect ;
      if(ip){
        printf("\n  Enter routine %s\n",my_name) ;
        printf("  Track section = %i, type = %i %s, idirect = %i\n",
             tfn->uid, tfn->type_of_node,
             token_trackdb[tfn->type_of_node],idirect) ;
        trv_print(&tf) ;
      }

      n = tfn->pin_to_section[idirect ? 0 : 1] ;
      if(ip)printf("  n = %i\n",n) ;
      tn1 = &track_db.trk_sections_array[n-1]       ;  // Section behind
      if(ip)printf("  Initial end section   = %i, type = %i %s\n",
             tn1->uid, tn1->type_of_node, token_trackdb[tn1->type_of_node]) ;
      if(tn1->type_of_node == END_SECTION){
        if(ip)printf("  Exit routine at END_SECTION\n") ;
        return 0 ;
      }
      if(ip)printf("  Move through junction :  Direction = %i\n",idirect ? -1 : 1) ;

      iret = trk_next(&tf,idirect ? -1 : 1) ;            //  Try to move through points
      if(iret == 1){
        if(ip)printf("  Exit routine :  trk_next found END_SECTION\n") ;
        return 0 ;
      }
      if(iret == 2){
        if(ip)printf("  Exit routine at First Junction : Points blocking route\n\n") ;
        return 1 ;
      } ;
/*
 *  Search following sections until a NORMAL signal or End of Track found
 *  Signals are track item SIGNALS.  See: trk_item_db.c "SignalItem".
 *
 *
 */

      tfn = tf.tn ;
      if(ip){
        printf("  ENTER MAIN LOOP\n") ;
      }
      for(;;){
/*
 *  Process vector section
 */
        tfn = tf.tn ;
        n = tf.tn->trk_item_number ;
        if(ip){
          printf("\n  New Loop\n") ;
          printf("  Next track section AA = %i, type = %i %s\n",
              tfn->uid, tfn->type_of_node, token_trackdb[tfn->type_of_node]) ;
          trv_print(&tf) ;
          printf("  Traveller :: Tracknode %i, ivector %i, idirect %i, vect_position %f, sect_distance %f\n",
                tf.itrack, tf.ivector, tf.idirect, tf.vect_position, tf.sect_distance);
          printf("\n  Number of track items = %i\n",n) ;
        }
//        sect_distance = tf.sect_distance ;
/*
 *  Loop through track items in direction of train
 */
        idirect = tf.idirect ;
        for(i=0;i<n;i++){
          m = tf.tn->trk_item_list[tf.idirect ? n-1-i : i] ;
          ti = &(track_db.trk_items_array[m]) ;
          if(ip)printf("    Track Item %i, m = %i, distance = %f, type = %i %s\n",
                    i, m, ti->sect_distance,
                    ti->type_of_node, token_trackdb[ti->type_of_node]) ;
          if(ti->type_of_node != SIGNALS)continue ;
/*
 * Find signal
 */
          signal = ti->signal ;                             //  0 = point to start of vectors
          if(signal->sig_type->msts_function != SIG_NORMAL)continue ;
          sig_direct = signal->trk_item->signal_direction ;  // 0 = point to start of vectors
          back_facing = signal->sig_subobj->sig_back_facing ;
          if(ip)printf("  Found signal  train direction = %i, signal direction = %i, back_facing = %i\n",
                 idirect, sig_direct, back_facing) ;
          if(back_facing != (sig_direct == idirect)){
            if(ip)printf("  Found NORMAL signal\n") ;
            return 0;                          //  Success
          }
          if(ip){
            printf("  Found signal %i :: %s :: %s\n",
                   signal->uid, signal->shape_name, signal->type_name) ;
          }

        }
/*
 *  Print data on junction or end section
 */
        idirect = tf.idirect ;
        n = tfn->pin_to_section[idirect ? 0 : 1] ;
        tn1 = &track_db.trk_sections_array[n-1]       ;  // Section behind
        if(ip)printf("    Junction/End section   = %i, type = %i %s\n",
              tn1->uid, tn1->type_of_node, token_trackdb[tn1->type_of_node]) ;
        if(tn1->type_of_node == END_SECTION){
          if(ip)printf("  Reached END_SECTION\n") ;
          break ;
        }
        if(ip)printf("    Junction data : type_of_pin %i %i, section %i %i %i, info %i %i %i"
               " strait %i, branch %i\n",tn1->type_of_pin[0],tn1->type_of_pin[1],
               tn1->pin_to_section[0],tn1->pin_to_section[1],tn1->pin_to_section[2],
               tn1->pin_info[0],tn1->pin_info[1],tn1->pin_info[2],
               tn1->straight, tn1->branch) ;
/*
 *  Move beyond junction
 */
        if(ip)printf("\n    Move through junction AA :  Direction = %i\n",idirect ? 1 : -1) ;
        iret = trk_next(&tf,idirect ? -1 : 1) ;
        if(ip){
          printf("    Routine trk_next returned %i.  Current tracknode is ...\n",iret) ;
          printf("    Tracknode %i, ivector %i, idirect %i, vect_position %f, sect_distance %f\n",
               tf.itrack, tf.ivector, tf.idirect, tf.vect_position, tf.sect_distance);
          trv_print(&tf) ;
        }
        if(iret){
          if(ip){
            printf("\n    Junction blocking route.  Change junction\n") ;
            printf("      branch = %i\n",tn1->branch ) ;
            printf("    Junction data : type_of_pin %i %i, section %i %i %i, info %i %i %i"
                  " strait %i, branch %i\n",tn1->type_of_pin[0],tn1->type_of_pin[1],
                  tn1->pin_to_section[0],tn1->pin_to_section[1],tn1->pin_to_section[2],
                  tn1->pin_info[0],tn1->pin_info[1],tn1->pin_info[2],
                  tn1->straight, tn1->branch) ;
          }
          if(tn1->branch != 0){
            witem = tn1->vector->world_item ;
            if(tn1->branch == 1){
              tn1->branch =2 ;
              witem->anim_value = 0.5 ;
            }else{
              tn1->branch =1 ;
              witem->anim_value = 0.0 ;
            }
          }
          if(ip)printf("  Junction data : type_of_pin %i %i, section %i %i %i, info %i %i %i"
                 " strait %i, branch %i\n",tn1->type_of_pin[0],tn1->type_of_pin[1],
                 tn1->pin_to_section[0],tn1->pin_to_section[1],tn1->pin_to_section[2],
                 tn1->pin_info[0],tn1->pin_info[1],tn1->pin_info[2],
                 tn1->straight, tn1->branch) ;

          idirect = tf.idirect ;
          iret = trk_next(&tf,idirect ? -1 : 1) ;
          if(ip){
            printf("  Routine trn_next returned %i.  Current tracknode is ...\n",iret) ;
            printf("  Tracknode %i, ivector %i, idirect %i, vect_position %f, sect_distance %f\n",
               tf.itrack, tf.ivector, tf.idirect, tf.vect_position, tf.sect_distance);
          }
          if(iret){
            printf("Routine %s error\n",my_name) ;
            printf(" ... while trying to change switch direction\n") ;
            exit(0) ;
          }
        }
//        clear_dist += sect_distance;
//        if(clear_dist > clear_dist0) return 0 ;
        idirect = tf.idirect ;
      }
      return 0 ;
}
