/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  signal_script_functions.c
 *
 *   This file contains routines used to process the signal scripts
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *==============================================================================
 */

double sc_pop_stack(void) {
  double  z  ;
  int     ip ;

      ip = (n_sig1 == signal0->uid) ;

      z = sc_stack[i_stack] ;
      if(i_stack > 0) i_stack-- ;
      if(ip)printf("  sc_pop_stack :: i_stack = %i\n",i_stack) ;
      return z ;
}
int sc_push_stack(double x){
  int     ip ;

      ip = (n_sig1 == signal0->uid) ;

      if(i_stack < n_stack-1){
        i_stack++;
        sc_stack[i_stack] = x ;
      }else{
        printf("  Stack overflow while processing Signal Script\n") ;
        printf("  Program stopping ...\n") ;
        exit(0) ;
      }
      if(ip)printf("  sc_push_stack :: i_stack = %i\n",i_stack) ;
      return 0 ;
}

/*
 *  Routine block_state
 *  This returns the state of the block ahead.  If the block is unoccupied
 *    then this returns BLOCK_CLEAR, otherwise one of the other BLOCK_ values
 *    is returned depending on the first item found which causes a non-clear
 *    state.
 *   SIG_OCCUPIED,     is occupied by one or more wagons not moving in opposite direction
 *   SIG_JN_OBSTRUCTED is impassable due to switch position, a moving train or not accessable
} ;
 */
double block_state(SignalDB *signal){

  int    ip = 0   ;
  int    i, j, k, jn, iret ;
  int    l_start  ;
  int    l_found  ;
  int    n_items  ;
  double d1, d2   ;
  TrkItem       *trk_item  ;
  TrkItem       *trk_item2 ;
  TrkSector     *trk_sect,
                *trk_sect2 ;
  SignalDB      *signal2   ;
  SigSubObj     *sig_subobj ;
  TravellerNode t, *tt;
  TrainNode     *train ;
  uint          track_section    ;
  uint          track_item_uid   ;
  uint          track_item_index ;
  uint          idirect2 ;

  char        *my_name  = "block_state" ;

//      ip = (67 == signal->uid) || (87 == signal->uid) ;
//      ip = (97 == signal->uid) ;
//      ip = (67 == signal->uid) ;
//      ip = (90 == signal->uid) || (91 == signal->uid) ;
//      ip = (69 == signal->uid) || (74 == signal->uid) ;
      ip = (n_sig1 == signal0->uid) ;
//      if(!ip) return 0.0 ;

      trk_item = signal->trk_item           ;
      sig_subobj = signal->sig_subobj ;
/*
 *  Print data
 */
      track_section    = trk_item->track_section    ;
      track_item_uid   = trk_item->uid    ;
      track_item_index = trk_item->trk_item_index   ;

      if(ip){
        printf("\n") ;
        printf("  Enter routine %s\n",my_name) ;
        printf("  signal     = %p\n",(void *)signal) ;
        printf("  signal uid = %i\n",signal->uid)    ;
        printf("  signal direction =  %i\n", trk_item->signal_direction) ;
        printf("\n") ;
        printf("  track Item       =  %p\n", (void *) trk_item) ;
        printf("  track item uid   =  %i\n", track_item_uid) ;
        printf("  track item index =  %i\n", track_item_index) ;
        printf("\n") ;
        printf("  track section    =  %i\n", track_section) ;

      }
//  DEBUG or REALITY

      if(track_section == 0)return (double) SIG_OCCUPIED ;

      trk_sect   = &(track_db.trk_sections_array[track_section]) ;
//      printf(" trk_sect                = %p\n",(void *)trk_sect) ;
//      printf(" trk_sect->trk_item_list = %p\n",(void *)trk_sect->trk_item_list) ;
      if(!(trk_sect->trk_item_list)) exit(0) ;
      i          = trk_sect->trk_item_list[track_item_index] ;
//      printf(" i = %i\n", i) ;
      trk_item2  = &(track_db.trk_items_array[i]) ;
//      printf(" trk_item2               = %p\n",(void *)trk_item2) ;

      if(ip){
        printf("  Check :: Index of node = %i,"
               "  Index via track section = %i\n",
                  trk_item->uid, trk_item2->uid) ;
      }
/*
 *  Initialise a ghost traveller to start at the signal and
 *  with a direction of travel towards the next signal
 */
      init_trav(&t) ;
      t.tn            = &track_db.trk_sections_array[trk_item->track_section] ;
      t.sect_distance = trk_item->sect_distance ;
      t.idirect       = trk_item->signal_direction ;  // 0 facing lower values
      t.itrack        = trk_item->track_section + 1 ;

      if(ip){
        printf("  Traveller starting on track %i, direction %i, distance %f, length %f\n",
                   t.itrack, t.idirect, t.sect_distance, t.tn->length) ;
      }

      if(sig_subobj->sig_back_facing)t.idirect = !t.idirect ;
/*
 *  Initialise variables for first section
 */
      if(t.idirect){
        n_items = t.tn->trk_item_number ;
        jn = 1 + n_items-track_item_index ;
        d1 = 0.0 ;
        d2 = t.sect_distance ;
      }else{
        jn = 1 + track_item_index ;
        d1 = t.sect_distance ;
        d2 = t.tn->length ;
      }
      l_start = 1 ;
      l_found = 0 ;
/*
 *  Loop over the track sections
 */
      for(;;){
        n_items = t.tn->trk_item_number ;
        if(l_start){
          l_start = 0 ;
        }else{
          jn = 0 ;
          d1 = 0.0 ;
          d2 = t.tn->length ;
        }
/*
 *  Loop over track items looking for signals
 */
        if(ip){
          printf("  Loop AA : track section = %i, %p\n",
                              t.tn->uid, (void *)t.tn) ;
          printf("  Loop AA : jn = %i, n_items = %i, idirect = %i\n",
                              jn, n_items,t.idirect) ;
        }
        for(j=jn;j<n_items;j++){
          i = (t.idirect) ? n_items - j - 1: j ;
          k = t.tn->trk_item_list[i] ;
          if(ip)printf("  Loop BB : j = %i, i = %i, k = %i\n",j,i,k) ;
          trk_item = &(track_db.trk_items_array[k]) ;

          if(ip)printf("  Next track item = %i :: %s\n",
                  trk_item->uid,
                  token_trackdb[trk_item->type_of_node]);
          if(trk_item->type_of_node != SIGNALS)continue ;
/*
 *  Check the signal is facing the same direction
 */
          signal2 = trk_item->signal ;
          idirect2 = trk_item->signal_direction ;
          if(signal2->sig_subobj->sig_back_facing)idirect2 = !idirect2 ;
          if(idirect2 != t.idirect) continue;

          if(ip)printf("  Found signal at track item %i\n",
                                        trk_item->uid);
          if(trk_item->signal->sig_type->msts_function == SIG_NORMAL){
            if(ip)printf("  Signal is SIG_NORMAL\n") ;
            l_found = 1 ;
            if(t.idirect){
              d1 = trk_item->sect_distance ;
            }else{
              d2 = trk_item->sect_distance ;
            }
            break ;
          }
        }
/*
 *  Search over trains
 *  If any front or back is found in this section between d1 and d2
 *  return SIG_OCCUPIED
 */
        if(trainlist_beg){
          for(train=trainlist_beg; train != NULL; train=train->next){
            tt = &train->front ;
            for(i=0;i<2;i++){
              if(i)tt = &train->back ;
              if(ip)printf("  Test train %s  :: %i :: track sections = %i, %i,"
                     "  distances = %f  : %f  %f\n",train->name, i,
                     tt->itrack, t.itrack,
                     tt->sect_distance, d1, d2) ;
              if(tt->itrack != t.itrack)continue ;
              if(tt->sect_distance >= d1 && tt->sect_distance <= d2){
                if(ip)printf(" Found train \n") ;
                return (double) SIG_OCCUPIED ;
              }
            }
          }
        }
/*
 *  If signal found but no train - exit loop to final test
 */
        if(l_found) break;
/*
 *  If no signal and no train move to the next section
 */
        iret = trk_next(&t, t.idirect? -1 : 1) ;
        if(iret){
          if(iret == 1)return (double) SIG_CLEAR ;         //  Reached end of line
          if(iret == 2)return (double) SIG_JN_OBSTRUCTED ; //  Junction set against
        }
      }
/*
 *  Search over trains to see if they are passing track position
 *  This is to check for trains that are so long that the two signals
 *  are both within the length of the train.
 */
      if(trainlist_beg){
        for(train=trainlist_beg; train != NULL; train=train->next){
          iret = search_for_track_position(&train->front, &train->back,
                      trk_item->track_section, trk_item->sect_distance) ;
          if(iret) return (double) SIG_OCCUPIED ;
        }
      }
      return (double) SIG_CLEAR ;
}

/*
 *  Routine route_set
 *  Returns TRUE if this signal head has no junction link.
 *  Otherwise TRUE if the route is switched ahead such that a train passing
 *    this signal would reach the specified junction (specified by the
 *    link) with the switch in the correct orientation.
 *  Otherwise FALSE.
 */
int route_set(SignalDB *signal){

  uint   ip  = 0 ;
  uint   i       ;
  uint   icount  = 0 ;
  TrkSector   *trk_sect ;
  TrkItem   *trk_item ;

  uint          idirect ;
  uint          trk_branch ;
  uint          jn_link   ;
  uint          jn_chk    ;
  uint          jn_branch ;
  uint          n_in_pins ;
  uint          n_ot_pins ;
  uint          old_sect_index  ;
  uint          jn_sect_index   ;
  uint          new_sect_index  ;
  uint          old_pin   ;

  char          *my_name  = "route_set" ;
/*
 *  If there is no junction link - return TRUE
 */
//      ip = (90 == signal->uid) || (91 == signal->uid) ;
//      ip = (69 == signal->uid) || (74 == signal->uid) ;
      ip = (n_sig1 == signal0->uid) ;
/*
 *  Track items store the track section index - not the uid
 */

      trk_item    = signal->trk_item ;
      idirect     = trk_item->signal_direction  ;
      jn_link     = trk_item->signal_dirs[0][0] ;
      jn_chk      = trk_item->signal_dirs[0][1] ;
      jn_branch   = trk_item->signal_dirs[0][2] ;

      trk_sect    = &track_db.trk_sections_array[trk_item->track_section] ;

      if(ip)printf("  Signal %i : jn_link %i :: %i %i %i, shape = %s, type = %s\n",
             signal->uid,
             jn_link, jn_chk, jn_branch, trk_item->signal_dirs[0][3],
             signal->shape_name, signal->type_name) ;
/*
 *  Print data
 */
      if(ip){
        printf("\n") ;
        printf("  Enter routine %s\n",my_name) ;
        printf("  signal             = %p\n",(void *)signal) ;
        printf("  signal uid         = %i\n", signal->uid)    ;
        printf("  signal direction   =  %i\n", idirect) ;
        printf("  signal sig_jn_link =  %i :: %i %i %i %i\n",signal->uid,
             jn_link, jn_chk, jn_branch, trk_item->signal_dirs[0][3]) ;
        printf("  sig_subobj jn_link =  %i\n",signal->sig_subobj->sig_jn_link) ;

        printf("\n") ;
        printf("  track section      =  %i\n", trk_item->track_section) ;
      }
      if(!signal->sig_subobj->sig_jn_link) return (double) 1.0 ;
/*
 **************************************************************************
 *  Loop until junction or end of line found
 **************************************************************************
 */
      new_sect_index = trk_sect->uid ; ;
      for(;;){
        if(icount++ > 5){
          if(ip)printf("\n  Routine %s :: FAILURE : icount > 5\n\n",my_name) ;
          return (double) 0.0 ;    //  Fail
        }
/*
 *  Check pins correct for a vector type section ...
 */
        if(ip){
          printf("\n  New Loop.  icount = %i\n",icount) ;
          printf("              Section index = %i  :: %i\n",
                 trk_sect->uid,new_sect_index) ;
        }
        n_in_pins = trk_sect->type_of_pin[0] ;
        n_ot_pins = trk_sect->type_of_pin[1] ;
        if(n_in_pins != 1 || n_ot_pins != 1){
          printf("  ERROR in routine %s\n",my_name) ;
          printf("  ERROR : Wrong number of old pins : n_in_pins, n_ot_pins ="
                                            " %i %i\n",  n_in_pins,n_ot_pins) ;
          exit(1) ;
        }
/*
 *  ... and move to next junction or end section
 */
        old_sect_index = new_sect_index ;
        if(idirect){
          jn_sect_index = trk_sect->pin_to_section[0] ;
        }else{
          jn_sect_index = trk_sect->pin_to_section[1] ;
        }
        if(ip){
          printf("  Routine %s :: idirect  = %i\n",my_name, idirect ) ;
          printf("                old_sect_index  = %i\n",old_sect_index) ;
          printf("                jn_sect_index   = %i\n",jn_sect_index) ;
        }
/*
 *  Move 'trk_sect' to junction node or end of line node
 */
        trk_sect = &track_db.trk_sections_array[jn_sect_index-1] ;      //  Remember '-1'!
        n_in_pins = trk_sect->type_of_pin[0] ;
        n_ot_pins = trk_sect->type_of_pin[1] ;
        if(n_in_pins+n_ot_pins>3){
          printf("  ERROR in routine %s\n",my_name) ;
          printf("  ERROR : Wrong number of new pins : n_in_pins, n_ot_pins = %i %i\n",
                                                                n_in_pins,n_ot_pins) ;
          exit(1) ;
        }
        if(ip){
          printf("  Junction section:  %i %i\n",trk_sect->uid,jn_sect_index);
          printf("    Number of in and out pins %i %i\n", n_in_pins, n_ot_pins) ;
          printf("    Pinned sections:          %i %i %i\n",
                                  trk_sect->pin_to_section[0],trk_sect->pin_to_section[1],
                                  trk_sect->pin_to_section[2]) ;
        }
/*
 * End of track
 */
        if(trk_sect->type_of_node == END_SECTION){
          if(ip){
            printf("    Routine %s :  End of section. Section = %i, index = %i\n",
                                                        my_name, jn_sect_index, jn_sect_index-1) ;
            printf("                  Type of node = %i %s\n",
                      trk_sect->type_of_node, token_trackdb[trk_sect->type_of_node]) ;
          }
          return (double) 0.0 ;    //  Search for junction failed
        }
/*
 * Error
 */
        if(n_in_pins+n_ot_pins < 2){
          printf("    Routine %s : Error :  End of section. Section = %i, index = %i\n",
                                                         my_name, jn_sect_index,jn_sect_index-1) ;
          printf("                          Type of node = %i %s\n",
                       trk_sect->type_of_node, token_trackdb[trk_sect->type_of_node]) ;
          return (double) 0.0 ;    //  Search for junction failed
        }
/*
 *  Search for old track section
 */
        old_pin = 99 ;
        for(i=0 ; i<n_in_pins+n_ot_pins ; i++){
          if(old_sect_index == trk_sect->pin_to_section[i]){old_pin = i ; break ; } ;
        }
        if(old_pin == 99){
          printf("  ERROR in routine %s\n",my_name) ;
          printf("  ERROR : Unable to find old pin in new track section\n") ;
          printf("  ERROR : Old pin = %i, new pins = ",old_pin) ;
          for(i=0 ; i<n_in_pins+n_ot_pins ; i++){ printf(" %i",trk_sect->pin_to_section[i]) ; }
          printf("\n");
          exit(1) ;
        }
        if(ip)printf("    Old section was pin %i, section was %i\n",
                                                  old_pin,trk_sect->pin_to_section[old_pin]) ;
/*
 *  Check for required junction
 */
        trk_branch = trk_sect->branch ;
        if(ip){
          printf("  Routine %s :: jn_sec    = %i\n",my_name,jn_sect_index) ;
          printf("                jn_link   = %i\n",jn_link) ;
          printf("                jn_chk    = %i\n",jn_chk) ;
          printf("                old_pin   = %i\n",old_pin) ;
          printf("                trk_branch  = %i\n",trk_branch) ;
          printf("                jn_branch = %i\n",jn_branch) ;
        }
        if(jn_sect_index == jn_link){
          if(ip){
            printf("  Routine %s.  Junction %i found\n",my_name,jn_link) ;
            printf("               Junction check flag = %i (1 == check path)\n",jn_chk) ;
          }
          if(!jn_chk) return (double) 1.0 ;        //  Junction found - no checks
          if(ip){
            printf("               old_pin = %i\n",old_pin) ;
            printf("               trk_branch = %i\n",trk_branch) ;
            printf("               jn_branch  = %i\n",jn_branch) ;
          }
#if 0
          if((1 == trk_branch && 1 == jn_branch) || (2 == trk_branch && 0 == jn_branch))
                                      return (double) 1.0 ;  // Junction found, branch set
          else                        return (double) 0.0 ;  // Branch test failed
#else
          if(1 == trk_sect->mainline){
            if(trk_branch == jn_branch+1) return (double) 1.0 ;  // Junction found, branch set
            else                          return (double) 0.0 ;  // Branch test failed
          }else{
            if((1 == trk_branch && 1 == jn_branch) || (2 == trk_branch && 0 == jn_branch))
                                        return (double) 1.0 ;  // Junction found, branch set
            else                        return (double) 0.0 ;  // Branch test failed
          }
#endif
        }
/*
 *  Move to next section
 *  Check that switch is set
 */
        if(old_pin>0){
          if((uint)old_pin != trk_branch){
            return (double) 0.0 ;             //  Junction set against route
          }else{
            new_sect_index = trk_sect->pin_to_section[0] ;
          }
        }else{
          new_sect_index = trk_sect->pin_to_section[trk_sect->branch] ;
        }
/*
 *  Move 'trk_section' to next stretch of line
 *        Note: track section 'pin_to_section' array stores the track section uid
 */
        trk_sect = &track_db.trk_sections_array[new_sect_index-1] ;


        idirect = !(trk_sect->pin_to_section[0] == jn_sect_index);
        if(ip){
          printf("  New normal section.  index = %i", new_sect_index) ;
          printf("      junction section index = %i\n", jn_sect_index) ;
          printf("           pin_to_section[0] = %i\n", trk_sect->pin_to_section[0]) ;
          printf("           idirect           = %i\n", idirect) ;
        }
      }
}

/*
 *  Return most or least restrictive state of the specified type at the
 *  next signal along the track.
 *  Return SIGASP_STOP if no signal head of the specified function type is identified.
 */
double next_sig_mr(double type){

      type = type  ;
      return (double) SIG_STOP ;
}
/*
 *  Return least restrictive state
 *
 *  From comments in the signal script for the MSTS USA2 route, what appears
 *  to be needed is the state of the next signal (after signal0) of type itype
 *  SIG_NORMAL etc.
 */
double next_sig_lr(double type){

  int  iret  ;

      iret = find_next_signal(signal0,nint(type)) ;

      return (double) iret ;
}
/*
 *  As next_sig_mr but for the opposing signal - (the signal controlling
 *  entry into this block from the opposite direction).
 */
double opp_sig_mr(double type){

      type = type  ;
      return 0 ;
}
/*
 *  Return least restrictive state
 */
double this_sig_lr(double type){

int     ip = 0 ;
int     i  ;
SigType *sig_type = signal0->sig_type ;
char    *my_name  = "this_sig_mr"  ;

      type = type ;

      for(i=7;i>=0;i--){
        if(sig_type->sig_draw_a[i]) break ;
      }
      return (double) i ;
}
/*
 *  As above but for the signal at the same position and in the same direction
 *  which matches the specified type.
 *
 *  Returns -1 if no signal of the specified type is identified.
 */
double this_sig_mr(double type){

int     ip = 0 ;
int     i  ;
SigType *sig_type = signal0->sig_type ;
char    *my_name  = "this_sig_mr"  ;

      type = type ;

      for(i=0;i<8;i++){
        if(sig_type->sig_draw_a[i]) break ;
      }
      return (double) i ;
}
/*
 *  Return least restrictive state
 */
double opp_sig_lr(SignalDB *signal){

      signal = signal ;
      return 0 ;
}

/*
 *  Search along the track ahead for signal heads of type 'fn_type' until a signal head
 *   of type 'end_type' is found, or until the switched track ahead can no longer be
 *   followed.  Signal heads of type 'fn_type' at the start signal and the end signal
 *   are included.  If any encountered signal head with function type 'fn_type' or
 *   'end_type' is linked, then this link is checked against the state of the linked
 *   switch - if it is not set as required by the link (see 'route_set' for further
 *   description) then it is ignored.
 *
 *   Returns the most restrictive indication of the identified signal heads.
 *   Returns SIGASP_STOP if no signal head of the function type end_type is found.
 *   This input is designed to find the state of an old UK or European distance signal
 *     head (pass in SIGFN_NORMAL and SIGFN_DISTANCE respectively for this purpose).
 */

int dist_multi_signal_mr(int fn_type /*SIGFN*/, int end_type /*SIGFN*/){

  int  iret ;

      iret = find_next_signal_mr(signal0, fn_type, end_type) ;
      return iret ;
}


/*
 *  See if the current signal has the specified feature (e.g. a number plate).
 *  Since a signal script applies to a single signal head, the specified feature
 *   is deemed to exist if there is any matching signal shape sub-object that
 *   faces the same direction as the current signal head.
 *  The SIGSUBT_ values are used to describe sub-objects within a signal
 *  shape (refer to the signal configuration file details above).
 */
int sig_feature(int n){

  SigSubObj *sig_subobj = signal0->sig_subobj ;

      if(n == SIG_NUMBER_PLATE){
        return sig_subobj->sig_number_plate ;
      }else if(n == SIG_GRADIENT_PLATE){
        return sig_subobj->sig_gradient_plate ;
      }else if(n == SIG_DECOR){
        return sig_subobj->sig_decor ;
      }else if(n == SIG_USER1){
        return sig_subobj->sig_user1 ;
      }else if(n == SIG_USER2){
        return sig_subobj->sig_user2 ;
      }else if(n == SIG_USER3){
        return sig_subobj->sig_user3 ;
      }else if(n == SIG_USER4){
        return sig_subobj->sig_user4 ;
      }else
        return 0 ;
}


/*
 *   Routine based on OpenRails routine of the same name.  It uses the
 *   raw signal SignalAspect structure to convert the signal's 'state' to the
 *   DrawState 'draw_state' index.  This is the index into the set of
 *   lights to draw.  The use of two indices seems odd - but presumably
 *   has (or had) a reason.
 */

int def_draw_state(int k){

int     ip = 0 ;
int     i  ;
static int icount = 0 ;
SigType *sig_type = signal0->sig_type ;
char    *my_name  = "def_draw_state"  ;


//     ip = ip && (97 == signal0->uid) ;
//      ip = (n_sig1 == signal0->uid) ;
      if(k<0 || k>8){
        printf("  Routine %s called with state index out of range\n",my_name) ;
        printf("    Index       = %i\n",k) ;
        printf("   Signal0  %i :: %s :: %s\n",
                   signal0->uid, signal0->shape_name, signal0->type_name) ;
        return -1;
      }
/*
 *  If error : return most restrictive state
 */
      if(!(sig_type->sig_draw_a[k])){
        if(icount<4){
          printf("  Routine %s called with unsupported state index %i\n",my_name, k) ;
          printf("  Signal %i :: %s :: %s\n",
                     signal0->uid, signal0->shape_name, signal0->type_name) ;
          icount ++ ;
        }
        k = (int)this_sig_mr(0.0) ;
      }
      if(ip){
        printf("  Routine %s\n", my_name) ;
        printf("    Signal shape = %s\n",signal0->shape_name) ;
        printf("    Signal type  = %s\n",signal0->type_name) ;
        printf("    Signal_uid   = %i\n",signal0->uid)  ;
        printf("    Input state  = %i\n",k) ;
        printf("    sig_type     = %p\n", (void *)sig_type) ;
        printf("    sig_draw_a   = %p\n", (void *)sig_type->sig_draw_a) ;
        printf("    sig_draw_a[0]  = %p\n", (void *)sig_type->sig_draw_a[0]) ;
        printf("    sig_draw_a[1]  = %p\n", (void *)sig_type->sig_draw_a[1]) ;

        printf("    Output options :\n") ;
        for(i=0;i<8;i++){
          if(sig_type->sig_draw_a[i]){
            printf("%i ",sig_type->sig_draw_a[i]->index) ;
          }else{
            printf("%p ",(void *)sig_type->sig_draw_a[i]) ;
          }
        }
        if(!(sig_type->sig_draw_a[k])){
          printf("\n  Routine %s called with unsupported state index\n",my_name) ;
          printf("  Signal %i :: shape_name = %s :: type_name = %s :: index = %i\n",
                    signal0->uid, signal0->shape_name, signal0->type_name,k) ;
          return 0 ;
        }
        printf("\n**********************************************************\n") ;
        printf("    Set draw_state to %i :: %s \n",sig_type->sig_draw_a[k]->index,
               token_signal_aspect[sig_type->sig_draw_a[k]->index]) ;
        printf("**********************************************************\n") ;
      }
      return sig_type->sig_draw_a[k]->index ;
}

int dist_multi_sig_mr(int ix, int iy){

      if(ix || iy > 10000) printf("dist_multi_sig_mr\n") ;

      return 0 ;
}

int train_has_call_on(){

      return 0 ;
}
int train_has_call_on_restricted(){

      return 0 ;
}

int sc_save(char* name){

  int   ip = 0 ;
  double x ;
  char   *my_name = "sc_save" ;

      ip = (n_sig1 == signal0->uid)  ;

      if(!strcmp(name,"state")){
        signal0->state = nint(sc_pop_stack()) ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"zrv1")){
        signal0->zrv1 = nint(sc_pop_stack()) ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"zrv2")){
        signal0->zrv2 = nint(sc_pop_stack()) ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"zrv3")){
        signal0->zrv3 = nint(sc_pop_stack()) ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"zrv4")){
        signal0->zrv4 = nint(sc_pop_stack()) ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"zrv5")){
        signal0->zrv5 = nint(sc_pop_stack()) ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"zrv6")){
        signal0->zrv6 = nint(sc_pop_stack()) ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"zrv7")){
        signal0->zrv7 = nint(sc_pop_stack()) ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"zrv8")){
        signal0->zrv8 = nint(sc_pop_stack()) ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"draw_state")){
        signal0->draw_state = nint(sc_pop_stack()) ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"enabled")){
        signal0->enabled = nint(sc_pop_stack()) ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"next_state")){
        sc_next_state = sc_pop_stack() ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"has_number_plate")){
        has_number_plate = sc_pop_stack() ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else if(!strcmp(name,"has_gradient_plate")){
        has_gradient_plate = sc_pop_stack() ;
        if(ip)printf("  +++ Routine %s. Stack saved to variable %s \n",my_name,name) ;
      }else{
        printf("###### Routine %s. Variable name %s not recognised\n",my_name,name) ;
      }
      return 0 ;
}

/*
 *  Functions
 *  Functions without parameters have a dummy parameter placed on the stack.
 */

int sc_func(char *name){

  int    ip = 0 ;
  double x,y ;
  char   *my_name = "sc_func" ;

      ip = (n_sig1 == signal0->uid)  ;

      if(!strcmp(name,"block_state")){
        x = sc_pop_stack() ;
        if(ip)printf("  Call routine block_state, x = %f\n",x) ;
        x = block_state(signal0) ;
        if(ip)printf("  Return from block_state,  x = %f\n",x) ;
        sc_push_stack(x) ;
        if(ip)printf("  +++ Routine %s. Function '%s' processed\n",my_name,name) ;
      }else if(!strcmp(name,"route_set")){
        x = sc_pop_stack() ;
        x = route_set(signal0) ;
        sc_push_stack(x) ;
        if(ip)printf("  +++ Routine %s. Function '%s' processed\n",my_name,name) ;
      }else if(!strcmp(name,"next_sig_lr")){
        x = sc_pop_stack() ;
        x = next_sig_lr(x) ;
        sc_push_stack(x) ;
      }else if(!strcmp(name,"this_sig_lr")){
        x = sc_pop_stack() ;
        x = this_sig_lr(x) ;
        sc_push_stack(x) ;
        if(ip)printf("  +++ Routine %s. Function '%s' processed\n",my_name,name) ;
      }else if(!strcmp(name,"next_sig_mr")){
        x = sc_pop_stack() ;
        x = next_sig_mr(x) ;
        sc_push_stack(x) ;
      }else if(!strcmp(name,"this_sig_mr")){
        x = sc_pop_stack() ;
        x = this_sig_mr(x) ;
        sc_push_stack(x) ;
        if(ip)printf("  +++ Routine %s. Function '%s' processed\n",my_name,name) ;
      }else if(!strcmp(name,"next_sig_lr")){
        x = sc_pop_stack() ;
        x = next_sig_lr(x) ;
        sc_push_stack(x) ;
      }else if(!strcmp(name,"this_sig_lr")){
        x = sc_pop_stack() ;
        x = this_sig_lr(x) ;
        sc_push_stack(x) ;
        if(ip)printf("  +++ Routine %s. Function '%s' processed\n",my_name,name) ;
      }else if(!strcmp(name,"sig_feature")){
        x = sc_pop_stack() ;
        x = sig_feature(nint(x)) ;
        sc_push_stack(x) ;
        if(ip)printf("  +++ Routine %s. Function '%s' processed\n",my_name,name) ;
      }else if(!strcmp(name,"def_draw_state")){
        x = sc_pop_stack() ;
        x = (double) def_draw_state(nint(x)) ;
        sc_push_stack(x) ;
        if(ip)printf("  +++ Routine %s. Function '%s' processed\n",my_name,name) ;
      }else if(!strcmp(name,"dist_multi_sig_mr")){
        y = sc_pop_stack() ;
        x = sc_pop_stack() ;
        x = dist_multi_sig_mr(nint(x),nint(y));
        sc_push_stack(x) ;
        if(ip)printf("  +++ Routine %s. Function '%s' processed\n",my_name,name) ;
      }else if(!strcmp(name,"TrainHasCallOn")){
        sc_pop_stack() ;
        x = train_has_call_on();
        sc_push_stack(x) ;
        if(ip)printf("  +++ Routine %s. Function '%s' processed\n",my_name,name) ;
      }else if(!strcmp(name,"TrainHasCallOn_Restricted")){
        sc_pop_stack() ;
        x = train_has_call_on_restricted();
        sc_push_stack(x) ;
        if(ip)printf("  +++ Routine %s. Function '%s' processed\n",my_name,name) ;
      }else{
        printf("###### Routine %s. Function name '%s' not recognised\n",my_name,name) ;
      }
      return 0 ;
}

/*
 * Variables based on OpenRails file SignalEnums.cs
 */

int sc_var(char *name){

  int  ip = 0 ;
  char *my_name = "sc_var" ;

      ip = (n_sig1 == signal0->uid)  ;

      if(!strcmp(name,"SIGFN_NORMAL")){
        sc_push_stack((double)SIG_NORMAL) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFN_DISTANCE")){
        sc_push_stack((double)SIG_DISTANCE) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFN_REPEATER")){
        sc_push_stack((double)SIG_REPEATER) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFN_SHUNTING")){
        sc_push_stack((double)SIG_SHUNTING) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFN_INFO")){
        sc_push_stack((double)SIG_INFO) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFN_SPEED")){
        sc_push_stack((double)SIG_SPEED) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFN_ALERT")){
        sc_push_stack((double)SIG_ALERT) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFN_UNKNOWN")){
        sc_push_stack((double)SIG_UNKNOWN) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGASP_STOP")){
        sc_push_stack((double)SIG_STOP) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGASP_STOP_AND_PROCEED")){
        sc_push_stack((double)SIG_STOP_AND_PROCEED) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGASP_RESTRICTING")){
        sc_push_stack((double)SIG_RESTRICTING) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGASP_APPROACH_1")){
        sc_push_stack((double)SIG_APPROACH_1) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGASP_APPROACH_2")){
        sc_push_stack((double)SIG_APPROACH_2) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGASP_APPROACH_3")){
        sc_push_stack((double)SIG_APPROACH_3) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGASP_CLEAR_1")){
        sc_push_stack((double)SIG_CLEAR_1) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGASP_CLEAR_2")){
        sc_push_stack((double)SIG_CLEAR_2) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"state")){
        sc_push_stack((double)signal0->state) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"zrv1")){
        sc_push_stack((double)signal0->zrv1) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"zrv2")){
        sc_push_stack((double)signal0->zrv2) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"zrv3")){
        sc_push_stack((double)signal0->zrv3) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"zrv4")){
        sc_push_stack((double)signal0->zrv4) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"zrv5")){
        sc_push_stack((double)signal0->zrv5) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"zrv6")){
        sc_push_stack((double)signal0->zrv6) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"zrv7")){
        sc_push_stack((double)signal0->zrv7) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"zrv8")){
        sc_push_stack((double)signal0->zrv8) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"next_state")){
        sc_push_stack(sc_next_state) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"draw_state")){
        sc_push_stack((double)signal0->draw_state) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"enabled")){
        sc_push_stack((double)signal0->enabled) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFEAT_NUMBER_PLATE")){
        sc_push_stack((double)SIG_NUMBER_PLATE) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFEAT_GRADIENT_PLATE")){
        sc_push_stack((double)SIG_GRADIENT_PLATE) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFEAT_USER1")){
        sc_push_stack((double)SIG_USER1) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFEAT_USER2")){
        sc_push_stack((double)SIG_USER2) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFEAT_USER3")){
        sc_push_stack((double)SIG_USER3) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"SIGFEAT_USER4")){
        sc_push_stack((double)SIG_USER4) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"has_number_plate")){
        sc_push_stack((double)has_number_plate) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"has_gradient_plate")){
        sc_push_stack((double)has_gradient_plate) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"BLOCK_CLEAR")){
        sc_push_stack((double)SIG_CLEAR) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"BLOCK_OCCUPIED")){
        sc_push_stack((double)SIG_OCCUPIED) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else if(!strcmp(name,"BLOCK_JN_OBSTRUCTED")){
        sc_push_stack((double)SIG_JN_OBSTRUCTED) ;
        if(ip)printf("  +++ Routine %s. Variable '%s' to stack\n",my_name,name) ;
      }else{
        printf("###### Routine %s. Variable name '%s' not recognised\n",my_name,name) ;
        printf("       Signal uid = %i, type = %s\n",signal0->uid, signal0->type_name) ; ;
      }
      return 0 ;
}


