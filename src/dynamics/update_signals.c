/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  update_signals.c
 *
 *   This file contains routines used to update the state of each signal
 *   each timestep.
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *==============================================================================
 */
int    update_signal(SignalDB *signal, int icount);
extern nodeType *sTree ;                             //  Defined in zr.h


/*
 *  Routine 'update_signals()' is called by routine display() before
 *  redrawing the main display screen.  Once every second it checks each
 *  signal and updates it aspect depending on the state of nearby signals
 *  and trains.
 *
 *  Note there are often more than one signal in each signal structure
 *  described in the world file.
 */

int update_signals(void){

  int          ip = 0;
  SignalDB     *signal;
  char         *my_name = "update_signals";
  static int   icount = 0 ;

      if(l_time_1s){
        icount++ ;
        if(icount > 3)icount = 0 ;
      }
/*
 *  Loop over signals
 */
      if(ip)printf("  Enter %s\n",my_name) ;
      for(signal = signallist_beg; signal != NULL ; signal = signal->next){
/*
 *  Full update of signal logic once every second
 */
        if(l_time_1s)update_signal(signal, icount);
/*
 *  Update semaphore and lights every entry
 */
        if(signal->sig_type->semaphore){
          if(signal->state >= SIG_CLEAR_1){
            if(0.5 != signal->semaphore_pos){
              signal->semaphore_pos += 2.0*delta_seconds;
              if(signal->semaphore_pos > 0.5) signal->semaphore_pos = 0.5 ;
            }
          }else{
            if(0.0 != signal->semaphore_pos){
              signal->semaphore_pos -= 2.0*delta_seconds;
              if(signal->semaphore_pos < 0.0) signal->semaphore_pos = 0.0 ;
            }
          }
        }
      }
      if(ip)printf("  Exit %s\n",my_name) ;
      return 0;
}

int update_signal(SignalDB *signal, int icount){

  int  ip = 0;
  int  i, j, k, uid, wuid, ifound;
  static int   ecount = 0  ;
  SignalObj    *sig_object ;
  RawSignalDB  *raw_signal ;
  WorldItem    *witem      ;
  SigType      *sig_type   ;
  char         *sig_name   ;
  char         *my_name = "update_signal";

      sig_type = signal->sig_type ;
      uid   = signal->uid ;
      witem = signal->witem ;
      wuid  = witem->uid ;

//      ip = (1570 == signal->uid) ;
//      ip = (n_sig1 == uid)  ;   // n_sig defined in zr.c
//      ip = (n_sig1 == uid) && l_time_5s  ;   // n_sig defined in zr.c
      if(ip){
        printf("====================================================\n") ;
        printf("  Enter routine %s.  Signal = %i\n",my_name,signal->uid) ;
      }
/*
 *  Catch errors
 */
      k = signal->state ;
      ifound = 0 ;
/*
 *  Some signals, i.e. Theatre displays in the MECoast route have no aspects
 *  As these cannot be handled yet they are skipped.
 */
      if(sig_type->n_sig_aspects == 0) return 0 ;

      if(sig_type->sig_draw_ia[k] == -1 && ecount++ < 4){
        printf("  ++++++++++++++++++ SIGNAL ERROR ++++++++++++++++++++++\n") ;
        printf("  Routine %s entered.  Signal = %i %s.  State = %i\n",
                 my_name, signal->uid, signal->sig_type->name, signal->state) ;
        printf("    state = %i, sig_draw_ia[state] = %i\n",
                  k, sig_type->sig_draw_ia[k]) ;
        printf("    Signal draw array   = ") ;
        for(j=0;j<8;j++)printf("  %2i %p",sig_type->sig_draw_ia[j],
                                  (void *)sig_type->sig_draw_a[j] ) ;
        printf("\n") ;
        printf("    Signal aspect array = ") ;
        for(j=0;j<8;j++)printf("  %2i %p",sig_type->sig_aspect_ia[j],
                                  (void *)sig_type->sig_aspect_a[j] ) ;
        printf("\n") ;
        for(i=0;i<8;i++){
          if(sig_type->sig_draw_ia[i] != -1){
            ifound = 1 ;
            signal->state = i ;
            break ;
          }
        }
        if(!ifound){
          printf("  Routine %s unable to find draw_state\n", my_name) ;
          printf("    Signal = %i %s\n",
                 signal->uid, signal->sig_type->name) ;
          return 0 ;
        }
        printf("  Routine %s : state set to %i\n", my_name, signal->state) ;
      }
/*
 * Debug print statements
 */

      if(ip){
        printf("\n====================================================\n") ;
        printf("  Routine %s, signal = %i %s, state = %i, witem uid = %i\n",
                 my_name, signal->uid, signal->sig_type->name, signal->state,
                 signal->witem->uid) ;
        printf("  Time = %f\n",run_seconds) ;
        printf(" Signal draw array   = ") ;
        for(j=0;j<8;j++)printf("  %2i %p",sig_type->sig_draw_ia[j],
                                  (void *)sig_type->sig_draw_a[j] ) ;
        printf("\n") ;
        printf(" Signal aspect array = ") ;
        for(j=0;j<8;j++)printf("  %2i %p",sig_type->sig_aspect_ia[j],
                                  (void *)sig_type->sig_aspect_a[j] ) ;
        printf("\n") ;
      }
/*
 *  Print signal data
 */
      if(ip){
        printf("  Signal uid = %i, name = %s :: %s\n",
                         uid,signal->shape_name,signal->type_name);
        printf("         state = %i, draw_state = %i aspect = %i, icount = %i\n",
                         signal->state,signal->draw_state,signal->aspect,icount) ;
        printf("                   wuid       = %i\n",wuid) ;
        printf("                   sig_shape  = %p\n",(void *)signal->sig_shape) ;
        printf("                       name   = %s\n",signal->sig_shape->name) ;
        printf("                       s_file = %s\n",signal->witem->filename) ;
        printf("                   sig_object = %p\n",(void *)signal->sig_object) ;
        printf("                   trk_item   = %p\n",(void *)signal->trk_item) ;
        printf("                   trk_item uid    = %i\n",signal->trk_item->uid) ;
        printf("                   trk_item index  = %i\n",signal->trk_item->trk_item_index) ;
        printf("                   trk_item section  = %i\n",signal->trk_item->track_section) ;
        printf("                   sig_type_name   = %s\n",signal->trk_item->signal_type_name) ;
        printf("                   sig_type_name   = %s\n",signal->sig_type->name) ;
        printf("                   sig direction   = %i\n",signal->trk_item->signal_direction) ;
        printf("                   back_facing     = %i\n",signal->sig_subobj->sig_back_facing) ;
        printf("                   n_animations    = %i\n",witem->n_animations) ;
        printf("                   animations      = %p\n",(void *)witem->animations);
        if(witem->animations){
          for(i=0;i<witem->n_animations;i++){
            printf("                   animations i = %i\n", i);
            printf("                   animations[]   = %p\n",
                                                  (void *)witem->animations[i]);
            if(witem->animations[i]){
              printf("                     value  = %f\n",*witem->animations[i]);
            }
          }
        }
        printf("====================================================\n") ;
      }
/*
 *  Process signal
 */
      process_signal_script(signal) ;
      signal->aspect = signal->draw_state ;
      if(witem == NULL){
        printf("  ERROR : Signal uid = %i, witem = %p\n",uid,(void *)witem);
      }

//      sig_name = signal->trk_item->signal_type_name ;

/*
 *  Print signal data
 */
      if(ip){
        printf("  Signal uid = %i, name = %s :: %s\n",
                         uid,signal->shape_name,signal->type_name);
        printf("         state = %i, draw_state = %i aspect = %i, icount = %i\n",
                         signal->state,signal->draw_state,signal->aspect,icount) ;
        printf("                   wuid       = %i\n",wuid) ;
        printf("                   sig_shape  = %p\n",(void *)signal->sig_shape) ;
        printf("                       name   = %s\n",signal->sig_shape->name) ;
        printf("                       s_file = %s\n",signal->witem->filename) ;
        printf("                   sig_object = %p\n",(void *)signal->sig_object) ;
        printf("                   trk_item   = %p\n",(void *)signal->trk_item) ;
        printf("                   trk_item uid    = %i\n",signal->trk_item->uid) ;
        printf("                   trk_item index  = %i\n",signal->trk_item->trk_item_index) ;
        printf("                   trk_item section  = %i\n",signal->trk_item->track_section) ;
        printf("                   sig_type_name   = %s\n",signal->trk_item->signal_type_name) ;
        printf("                   sig_type_name   = %s\n",signal->sig_type->name) ;
        printf("                   sig direction   = %i\n",signal->trk_item->signal_direction) ;
        printf("                   back_facing     = %i\n",signal->sig_subobj->sig_back_facing) ;
        printf("                   n_animations    = %i\n",witem->n_animations) ;
        printf("                   animations      = %p\n",(void *)witem->animations);
        if(witem->animations){
          for(i=0;i<witem->n_animations;i++){
            printf("                   animations i = %i\n", i);
            printf("                   animations[]   = %p\n",
                                                  (void *)witem->animations[i]);
            if(witem->animations[i]){
              printf("                     value  = %f\n",*witem->animations[i]);
            }
          }
        }
        printf("====================================================\n") ;
      }

      return 0;
}


