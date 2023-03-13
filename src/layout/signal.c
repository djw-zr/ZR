/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  signal.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for processing signals while timestepping the program
 *
 *   This routine are based primarily on the OpenRails file SIGSCRfile.cs
 *   authored by Rob Roeterdink and the files Signals.cs and SignalEnums.cs
 *
 *==============================================================================
 */
#if 0
void  sh_process_script(SignalHead *signal_head);
void  sh_process_basic(SignalHead *signal_head);
void  sh_set_aspect_to_limit(SignalHead *signal_head, int limit);

/*
 *   Routine called to update the aspects of all signal heads
 */
void  sh_update(SignalHead *signal_head){

  int ip = 0
  char *my_name[] = "sh_update" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      if(signal_head->signal_type == NULL) return ;
      if(signal_head->script_name != NULL)
        sh_process_script(signal_head) ;
      else
        sh_process_basic(signal_head)  ;
      return ;
}

/*
 *   Routine called to update signal heads with specified
 *   signal script
 */
void  sh_process_script(SignalHead *signal_head){

  int ip = 0
  char *my_name[] = "sh_process_script" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
      sh_process_basic(signal_head)  ;               //  Temporary fix

      return ;
}

/*
 *   Routine called to update signal heads with no specified
 *   signal script
 */
void  sh_process_basic(SignalHead *signal_head){

  int ip = 0
  char *my_name[] = "sh_process_basic" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      if(signal_block_state(signal_head->main_signal) == SIG_CLEAR)
         sh_set_aspect_to_limit(signal_head, 0) ;
      else
         sh_set_aspect_to_limit(signal_head, 1)  ;
      return ;
}

/*
 *   Routine to set the signal aspect to the least or most restrictive aspect
 *   limit = 0 -> most restrictive
 */
void  sh_set_aspect_to_limit(SignalHead *signal_head, int limit){

  int ip = 0
  int i, j, k, n, m ;
  RawSignal *raw_signal ;
  SigType   *sig_type   ;
  SigAspect *sig_aspect
  char *my_name[] = "sh_process_basic" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      raw_signal = signal_head->raw_signal ;
// If raw signal data is missing
      if(raw_signal == NULL) {
        signal_head->aspect = (limit) ? SIG_CLEAR_2 : SIG_STOP ;
        return ;
      }
//  Normal case
      n = raw_signal->n_sig_types ;
//  Look for most restrictive
      if(limit == 0){
        signal_head->aspect = SIG_CLEAR_2 ;
        for(i = 0; i<n, i++){
          sig_type = &(raw_signal->sig_type[i]) ;
          m = sig_type->n_sig_aspects ;
          for(j=0; j<m; j++){
            k = sig_type->sig_aspect[j].status ;
            if(k<signal_head->aspect)signal_head->aspect = k ;
            if(k == SIG_STOP) return ;
          }
        }
      }else{
        signal_head->aspect = SIG_STOP ;
        for(i = 0; i<n, i++){
          sig_type = &(raw_signal->sig_type[i]) ;
          m = sig_type->n_sig_aspects ;
          for(j=0; j<m; j++){
            k = sig_type->sig_aspect[j].status ;
            if(k>signal_head->aspect)signal_head->aspect = k ;
            if(k == SIG_CLEAR_2) return ;
          }
        }
      }
      return ;
}
#endif

#if 0
SignalDB *find_signal(char *name){

int      ip = 0 ;
SignalDB *signal ;
char     *my_name = "find_signal" ;

      if(ip){
        printf(" signal_list_beg = %p\n",(void *)signallist_beg) ;
      }
      for(signal=signallist_beg; signal != NULL ; signal = signal->next){
        if(ip){
          printf("  Signal name = %s\n",signal->shape_name) ;
        }
        if(strcmp(signal->shape_name,name) == 0){
          printf(" Routine %s.  Found signal %s\n",my_name, name) ;
          return signal ;
        }
      }
      printf("  Routine %s, unable to find signal %s\n",my_name,name) ;
      return NULL ;
}
#endif



