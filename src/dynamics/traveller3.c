/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  traveller3.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Further routines used with travellers
 *
 *   Initially the routines need to be robust and accurate but as they are
 *   the core part of the dynamics code they eventually also need to be very
 *   efficient
 *
 *==============================================================================
 */

/*
 *  Routine to search between to two travellers or 'ghost' travellers
 *  representing the start and end of a train.  The value of 'idirect'
 *  in the end traveller must correctly represent the direction of the
 *  train.  The values of 'itrack' and 'sect_distance' must be set
 *  for both travelelrs.
 *
 *  Routine returns '1' if the track position lies between the two travellers
 *  otherwise it returns zero.
 */

int search_for_track_position(TravellerNode *t_beg, TravellerNode *t_end,
                              uint tr_sect, double tr_dist){
  int   ip = 0 ;
  int    iret ;
  double d1, d2 ;
  TravellerNode t ;   //  Ghost traveller
  char *my_name = "search_for_track_position" ;

/*
 *  Initialise ghost traveller
 */
      init_trav(&t) ;
      t.itrack        = t_end->itrack  ;
      t.sect_distance = t_end->sect_distance ;
      t.idirect       = t_end->idirect ;
      t.tn            = t_end->tn      ;
/*
 *  Loop over track section from position of t_end to position of t_beg
 */
      for(;;){
        if(tr_sect == t.itrack){             //  Section matches
          if(t.itrack == t_beg->itrack){
            if(t.idirect){
              d1 = t_beg->sect_distance ;    //  End of train
              d2 = t.tn->length ;            //  End of section
            }else{
              d1 = 0.0 ;                     //  Start of section
              d2 = t_beg->sect_distance ;    //  End of train
            }
          }else if(t.itrack == t_end->itrack){
            if(t.idirect){
              d1 = 0.0 ;                     //  Start of section
              d2 = t_end->sect_distance ;     // Start of train
            }else{
              d1 = t_end->sect_distance ;    //  Start of train
              d2 = t.tn->length ;            //  End of section
            }
          }else{
            d1 = 0.0 ;                       //  Start of section
            d2 = t.tn->length ;              //  End of section
          }
/*
 *  Test : return 1 if position within limits
 */
          if(tr_dist >= d1 && tr_dist <= d2) return 1 ;
          return 0 ;
        }
/*
 *  If the ghost traveller has reached the start of the train, exit, ...
 */
        if(t_beg->itrack == t.itrack) return 0 ;
/*
 *  ... otherwise move to the next track section
 */
        if(t.idirect)
          iret = trv_next(&t) ;
        else
          iret = trv_prev(&t) ;
        if(iret) return 0 ;    //  Track end or blocked junction
      }
      exit(0) ;                  // Should never be reached
}
