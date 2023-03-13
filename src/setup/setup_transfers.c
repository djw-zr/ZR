/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  setup_transfers.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for initialising transfer objects
 *     Initially : water towers only
 *
 *==============================================================================
 */

/*
 *  MSTS and OpenRails use a single class of transfer object to load fuel
 *  and water to engines and load and unload wagons.
 */

int setup_transfers(void){

  int   ip = 0 ;
  WorldNode  *wnode ;
  WorldItem  *witem ;
  PickupObj  *pp    ;
  TransferNode *tf_node ;

  char  *nil ="(nil)";
  char  *my_name="setup_transfers" ;

      if(ip)printf("   Enter routine: %s\n",my_name) ;
/*
 *  Loop over the world items, finding each of the transfer objects.
 *  List the key data and the data for the associated track objects.
 */
      for(wnode=worldlist_beg ; wnode != NULL; wnode = wnode->next){
        for(witem= wnode->world_item; witem != NULL ; witem = witem->next){
          if(witem->worldtype != PICKUP_ALT)continue ;
//          if(!witem->n_tr_item)continue ;
          printf("  World item = %4i,  World type = %4i, %s, name = %s"
                 "  n_tr_items = %i,    %i, %i :: Transfer Type = %i\n",
                   witem->uid, witem->worldtype,  token_idc[witem->worldtype],
                   witem->filename,
                   witem->n_tr_item, witem->tr_item_db[0],
                   witem->tr_item_db_id[0],
                   witem->u.pickup_obj.pickuptype) ;
//  Ignore road items (for now?)
          if(!witem->n_tr_item || witem->tr_item_db[0]) continue ;
          pp = &(witem->u.pickup_obj) ;
          if(0)printf("    Pickup speed %f to %f.  Type = %i, %i.  Options = %f."
                 " Speed = %f, Available = %f.  Feed rate = %f.\n",
                 pp->min_mps, pp->max_mps, pp->pickuptype, pp->pickuptype_2,
                 pp->pickup_options,
                 pp->animation_speed, pp->quantity_available_kg,
                 pp->feed_rate_kgps) ;
          tf_node = (TransferNode *)malloc(sizeof(TransferNode)) ;
          if(!transferlist_beg){
            transferlist_beg = tf_node ;
          }else{
            transferlist_end->next = tf_node ;
          }
          transferlist_end = tf_node ;
          tf_node->next = NULL ;
          tf_node->wi_node = witem ;
          tf_node->ti_node = &(track_db.trk_items_array[witem->tr_item_db_id[0]]) ;
          if(witem->n_tr_item > 1){
            printf("  Routine %s ERROR\n",my_name) ;
            printf("  World item %i (PICKUP) has more than one associated track item\n",
                                                                              witem->uid) ;
            printf("  Program ZR ignoring all but the first\n") ;
          }
          n_transfers++ ;   //  Count of transfer items (zr.h)
        }
      }
/*
 *  Loop over the track items
 */

      return 0 ;
}
