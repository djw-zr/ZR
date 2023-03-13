/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  signal_script_functions2.c
 *
 *   This file contains routines used to process teh signal scripts
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *==============================================================================
 */

/*
 *  Routine to find next signal of a given type
 */

int find_next_signal(SignalDB *signal, int itype){

  int  ip = 0;

  int  i, j, k, jn, iret, n_items, l_start ;
  uint idirect2 ;

  TrkItem       *trk_item   ;
  TravellerNode t        ;
  SignalDB      *signal2 ;
  SigSubObj     *sig_subobj ;

  char *my_name = "find_next_signal" ;

//      ip = (69 == signal->uid) || (74 == signal->uid) ;

      trk_item   = signal->trk_item ;
      sig_subobj = signal->sig_subobj ;
/*
 * Initialise ghose traveller
 *   Note 'trk_item->track_section' is the index in the array,
 *         not the uid of the track section
 */

      init_trav(&t) ;
      t.tn            = &track_db.trk_sections_array[trk_item->track_section] ;
      t.sect_distance = trk_item->sect_distance ;
      t.idirect       = trk_item->signal_direction ;  // 0 facing lower values
      t.itrack        = trk_item->track_section + 1 ;

      if(sig_subobj->sig_back_facing)t.idirect = !t.idirect ;
/*
 *  Initialise variables for first section
 */
      if(t.idirect){
        n_items = t.tn->trk_item_number ;
        jn = n_items - trk_item->trk_item_index ;
      }else{
        jn = 1 + trk_item->trk_item_index ;
      }
      l_start = 1 ;
/*
 *  Loop over the track sections
 */
      for(;;){
        n_items = t.tn->trk_item_number ;
        if(l_start){
          l_start = 0 ;
        }else{
          jn = 0 ;
        }
/*
 *  Loop over track items looking for signals
 */
        if(ip){
         printf("  Loop AA : track section = %i, %p\n",
                             t.tn->uid, (void *)t.tn) ;
         printf("  Loop AA : jn = %i, n_items = %i, t.idirect = %i\n",
                             jn, n_items, t.idirect) ;
        }
        for(j=jn;j<n_items;j++){
          i = (t.idirect) ? n_items - j - 1 : j ;
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

          if(ip)printf("  Found signal %i, state %i %s, at track item %i\n",
                     signal2->uid,
                     signal2->state,token_signal_aspect[signal2->state],
                     trk_item->uid);
          if(signal2->sig_type->msts_function == itype){
            if(ip)printf("  Signal uid = %i,  type %i %s :: aspect = %i %s\n",
                            trk_item->signal->uid, itype, token_signal_function[itype],
                            trk_item->signal->state,
                            token_signal_aspect[signal2->state]) ;
            return signal2->state ;
          }
        }
/*
 *  If no signal found, move to the next section
 */
        iret = trk_next(&t, t.idirect? -1 : 1) ;
/*
 *  If iret != 0, there must be an error, instead of finding a signal, the end of
 *  line has been reached or there is a junction against the route.
 *  Return SIG_STOP as an error.
 */
        if(iret){
          return SIG_STOP ;
        }
      }
      exit(0) ; //  Should never be reached
}

/*
 *  Routine usually used to find UK normal signals repeated by a distant signal
 *  Search along the track until a signal of itype is found, a signal of type2
 *  is found or the track is blocked.
 *
 *  If type1 is found, return its state.  Otherwise return SIG_STOP
 */

int find_next_signal_mr(SignalDB *signal, int itype, int itype2){

  int  ip = 0;

  int  i, j, k, jn, iret, n_items, l_start ;
  uint idirect2 ;

  TrkItem       *trk_item   ;
  TravellerNode t        ;
  SignalDB      *signal2 ;
  SigSubObj     *sig_subobj ;

  char *my_name = "find_next_signal" ;

//      ip = (69 == signal->uid) || (74 == signal->uid) ;

      trk_item   = signal->trk_item ;
      sig_subobj = signal->sig_subobj ;
/*
 * Initialise ghose traveller
 *   Note 'trk_item->track_section' is the index in the array,
 *         not the uid of the track section
 */

      init_trav(&t) ;
      t.tn            = &track_db.trk_sections_array[trk_item->track_section] ;
      t.sect_distance = trk_item->sect_distance ;
      t.idirect       = trk_item->signal_direction ;  // 0 facing lower values
      t.itrack        = trk_item->track_section + 1 ;

      if(sig_subobj->sig_back_facing)t.idirect = !t.idirect ;
/*
 *  Initialise variables for first section
 */
      if(t.idirect){
        n_items = t.tn->trk_item_number ;
        jn = n_items - trk_item->trk_item_index ;
      }else{
        jn = 1 + trk_item->trk_item_index ;
      }
      l_start = 1 ;
/*
 *  Loop over the track sections
 */
      for(;;){
        n_items = t.tn->trk_item_number ;
        if(l_start){
          l_start = 0 ;
        }else{
          jn = 0 ;
        }
/*
 *  Loop over track items looking for signals
 */
        if(ip){
         printf("  Loop AA : track section = %i, %p\n",
                             t.tn->uid, (void *)t.tn) ;
         printf("  Loop AA : jn = %i, n_items = %i, t.idirect = %i\n",
                             jn, n_items, t.idirect) ;
        }
        for(j=jn;j<n_items;j++){
          i = (t.idirect) ? n_items - j - 1 : j ;
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

          if(ip)printf("  Found signal %i, state %i %s, at track item %i\n",
                     signal2->uid,
                     signal2->state,token_signal_aspect[signal2->state],
                     trk_item->uid);
          if(signal2->sig_type->msts_function == itype){
            if(ip)printf("  Signal uid = %i,  type %i %s :: aspect = %i %s\n",
                            trk_item->signal->uid, itype, token_signal_function[itype],
                            trk_item->signal->state,
                            token_signal_aspect[signal2->state]) ;
            return signal2->state ;
          }
          if(signal2->sig_type->msts_function == itype2){
            if(ip)printf("  Signal uid = %i,  type %i %s :: aspect = %i %s\n",
                            trk_item->signal->uid, itype, token_signal_function[itype],
                            trk_item->signal->state,
                            token_signal_aspect[signal2->state]) ;
            return SIG_STOP ;
          }

        }
/*
 *  If no signal found, move to the next section
 */
        iret = trk_next(&t, t.idirect? -1 : 1) ;
/*
 *  If iret != 0, there must be an error, instead of finding a signal, the end of
 *  line has been reached or there is a junction against the route.
 *  Return -1 as an error.
 */
        if(iret){
          return -1 ;
        }
      }
      exit(0) ; //  Should never be reached
}

