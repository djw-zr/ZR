/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  update_transfers.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines used to initialise and update transfers - of water, coal and oil
 *   to engines and other goods to wagons.
 *
 *==============================================================================
 */

TransferNode *search_for_transfer(WagonNode *wagon, WagonIntake *intake) ;

/*
 *  Routine to start a transfer.
 *  This only occurs if an engine or wagon of the current train is correctly
 *  placed adjacent to a loading point.
 */


int start_transfer(void){

  int       ip = 0 ;
  int       iret   ;
  WagonNode *wagon,
            *engine,
            *tender ;
  RawWagonNode *raw_wagon ;
  WagonIntake  *intake    ;
  TransferNode *transfer  ;

  char      *my_name = "start_transfer" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
 *  For the moment transfers are only possible for engines and wagons
 *  under player control.  In the case of wagons this means they must be
 *  part of a train containing an engine (motor).
 */
      if(!player_train || !player_train->motor ) return 0 ;
      if(current_transfer){
        printf("  ERROR:  New transfer requested while transfer under way\n") ;
        return 1 ;
      }
/*
 *  Engines and tenders may be anyway in a train - and there may be more
 *  than one of each.  So here we just loop through the waons of a train
 *  to find the first that is within range of a suitable loading point.
 */

      for(wagon=player_train->first; wagon!= NULL; wagon = wagon->next){
        raw_wagon = wagon->raw_wagon    ;
        for(intake = raw_wagon->intake; intake != NULL; intake = intake->next){
          if(0 && ip)printf("  Routine %s.  Intake = %p next = %pi\n",
                  my_name,(void *)intake, (void *)intake->next) ;
          transfer = search_for_transfer(wagon, intake) ;
          if(0 && ip)printf("  Routine %s.  transfer = %p\n",
                                           my_name, (void *)transfer) ;
          if(transfer){
            transfer->on = 1 ;
            current_transfer = transfer ;
            if(ip)printf("  Initialise transfer.  Transfer = %p\n",
                                                         (void *)transfer) ;
            return 0 ;                // Success
          }
        }
      }
      if(ip)printf("  Exit routine %s\n",my_name) ;
      return 0;
}

/*
 *  Routine to initialise the last stages of a transfer.  This usually involves
 *  movement of any loading equipment back to the rest position.
 */

int stop_transfer(void){

  int       ip = 0 ;

  char      *my_name = "stop_transfer" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      if(!current_transfer) return 0 ;
      current_transfer->on = 0 ;
      return 0;
}

/*
 *  Given a wagon and an 'intake' it looks for the closese matching
 *  transfer item within the current track sector.
 */

TransferNode *search_for_transfer(WagonNode *wagon, WagonIntake *intake){

  int            ip = 0 ;
  int            itype,
                 ptype_1,
                 ptype_2  ;
  uint           itrack ;
  uint           jtrack ;  //  Array index ;
  double         wa_dist    ;
  double         ti_dist    ;
  double         radius     ;
  TravellerNode  *traveller ;
  TransferNode   *transfer  ;                   // track.h
  TrkItem        *ti_node   ;
  WorldItem      *witem ;
  PickupObj      *pickup    ;
  char           *my_name = "search_for_transfer" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      traveller = wagon->traveller  ;
      itrack    = traveller->itrack - 1 ;
      for(transfer = transferlist_beg; transfer!= NULL; transfer=transfer->next){
        ti_node = transfer->ti_node ;
        jtrack  = ti_node->track_section ;
        if(ip)printf("   Transfer = %p.  Next = %p\n",
                                  (void *)transfer, (void *)transfer->next) ;
        if(itrack == jtrack){
          if(ip)printf("  Routine my_name.  Found sections %i %i\n",itrack,jtrack) ;
/*
 *  Check compatability
 */
          witem = transfer->wi_node ;
          pickup = &(witem->u.pickup_obj) ;
          ptype_1 = pickup->pickuptype    ;
          ptype_2 = pickup->pickuptype_2  ;
          itype = intake->type ;
          if(ip){
            printf("  Track item uid = %i, type = %i\n",
                                            ti_node->uid,ti_node->type_of_node) ;
            printf("  World item uid = %i, type = %i\n",
                                            witem->uid,witem->worldtype) ;
            printf("  Train and track load types =  %i :: %i  %i\n",
                                            itype, ptype_1, ptype_2) ;
          }
          if(itype!=ptype_1)continue ;
/*
 *  Suitable loading world item found ...
 *  ... now check wagon position.
 */
          wa_dist  = traveller->sect_distance ;
          if(wagon->train_dir == traveller->idirect){
            wa_dist = wa_dist + intake->position ;
          }else{
            wa_dist = wa_dist - intake->position ;
          }
          ti_dist = ti_node->sect_distance ;
          radius  = intake->radius ;
          if(ip)printf("    wa_dist = %f, ti_dist = %f, radius = %f\n",
                            wa_dist, ti_dist, radius) ;
          if(fabs(wa_dist-ti_dist)<radius){
            break ;
          }else{
            printf("  Wagon in wrong position:  wa_dist = %f, ti_dist = %f, radius = %f\n",
                            wa_dist, ti_dist, radius) ;
            continue ;
          }

        }
      }
      if(ip)printf("  Exit routine %s.  Transfer = %p\n",my_name, (void *) transfer) ;

      return transfer ;
}

int update_transfer(void){

  int  ip = 0 ;
  int  on     ;
  double    anim_value ;
  WorldItem *witem ;
  TransferNode *transfer = current_transfer ;

  char *my_name = "update_transfer" ;


      if(!transfer) return 0 ;
      if(0 && ip)printf("  Enter routine %s\n",my_name) ;

      witem = transfer->wi_node ;
      on    = transfer->on      ;
      anim_value = witem->anim_value ;
      if(on){
        if(anim_value != 0.5){
          anim_value = anim_value + 0.008 ;
          if(anim_value > 0.5)anim_value = 0.5 ;
          witem->anim_value = anim_value ;
        }
      }else{
        if(anim_value != 0.0){
          anim_value = anim_value - 0.008 ;
          if(anim_value < 0.0)anim_value = 0.0 ;
          witem->anim_value = anim_value ;
        }
        if(anim_value == 0.0)current_transfer = NULL ;
      }
      if(ip)printf("    Anim Value = %f   %p\n",anim_value, (void *)current_transfer) ;
      return 0 ;
}
