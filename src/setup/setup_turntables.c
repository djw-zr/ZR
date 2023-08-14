/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  setup_turntabless.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for initialising turntables
 *
 *==============================================================================
 */

int setup_turntables(void){

  int        ip = 0 ;
  int        w_uid, tile_x, tile_y  ;
  uint       i, j   ;
  TrkSector  *ts, *ts1, *ts2 ;
  TrkVectorNode *tv ;
  TrkItem    *ti ;
  TurnTable  *tt ;
  WorldNode  *wn ;
  WorldItem  *wi ;
  TT_Path    *path, *path0 ;
  char      *my_name = "setup_turntables" ;


      if(ip ){
        printf("\n  Enter routine %s\n",my_name);
        for(tt=turntablelist_beg; tt!=NULL; tt =tt->next){
          printf("      tt   uid = %i\n",tt->w_uid) ;
        }
      }
/*
 *  Loop over tiles and world items looking for a match to the turntable
 *  values.  Then save a pointer to the world item and its angle of rotation
 *  in the tt structure (after converting to the same convention as used by
 *  the track vectors),
 */
      for(wn=worldlist_beg; wn!=0; wn=wn->next){
        for(wi=wn->world_item; wi!=NULL; wi=wi->next){
          for(tt=turntablelist_beg; tt!=NULL; tt=tt->next){
            if(tt->w_uid == wi->uid && tt->tile_x == wn->tile_x
                                    && tt->tile_y == wn->tile_y ){
              tt->witem  = wi      ;
              tt->tile_x0 = wi->world_node->tile_x ;
              tt->tile_y0 = wi->world_node->tile_y ;
              tt->x0      = wi->X ; // Convert MSTS to ZR (physics) coordinates
              tt->y0      = wi->Z ;
              tt->z0      = wi->Y ;
              tt->ax0     = wi->AX ;
              tt->ay0     = wi->AZ ;
              tt->az0     = wi->AY ;
              tt->angle0  = (wi->AY>0.0) ? 360.0-wi->ANG : wi->ANG ;
              tt->angle   = tt->angle ;
              tt->i_dirn  = 0    ;
              tt->tt_list = NULL ;

              tt->trk_vector0.tile_east_x  = tt->tile_x0 ;
              tt->trk_vector0.tile_north_z = tt->tile_y0 ;

              tt->trk_vector0.distance0    = 0.0 ;

              if(ip){
                printf("      tt  Found world ::  uid = %i :: Tile = %i %i\n",
                               wi->uid, wn->tile_x, wn->tile_y) ;
                printf("      tt  X, Y, Z = %f %f %f ::"
                           " AX, AY, AZ, ANG =%f %f %f %f\n",
                             wi->X, wi->Y, wi->Z, wi->AX, wi->AY, wi->AZ, wi->ANG) ;
                printf("\n") ;
              }
            }
          }
        }
      }
/*
 *  Search track database for reference to the same world items
 *
 *  Dummy sections of track have more than 1 vector
 *    and are connected to end sections
 */
      for(i=0;i<track_db.trk_sections_array_size;i++){
        ts = &(track_db.trk_sections_array[i]) ;
        if(ts->type_of_node != VECTOR_SECTION)continue ;
        if(ts->length_of_vector == 1){
          ts1 = &(track_db.trk_sections_array[ts->pin_to_section[0]-1]) ;
          ts2 = &(track_db.trk_sections_array[ts->pin_to_section[1]-1]) ;
          if(ts1->type_of_node == END_SECTION &&
             ts2->type_of_node == END_SECTION) continue ;
        }
        for(j=0; j<ts->length_of_vector;j++){
          tv = &(ts->vector[j]) ;
          wi = tv->world_item ;
          if(!wi)continue ;
          tile_x = wi->world_node->tile_x ;
          tile_y = wi->world_node->tile_y ;
          w_uid  = wi->uid ;
          for(tt=turntablelist_beg; tt!=NULL; tt=tt->next){
            if(w_uid == tt->w_uid && tile_x == tt->tile_x
                                  && tile_y == tt->tile_y){
              path = (TT_Path *)malloc(sizeof(TT_Path)) ;

              path0 = tt->path ;
              if(path0 == NULL){
                tt->path = path ;
              }else{
                for(; path0->next!=NULL; path0=path0->next){
                  ;
                }
                path0->next = path ;
              }

              path->next       = NULL ;
              path->trk_sector = ts ;
              path->trk_vector = tv ;
              path->trk_sector_uid = i + 1 ;
              path->trk_vector_id  = j ;
              path->angle      = tv->a_height_y*180.0/pi ;
              tt->n_paths++ ;
              if(fabs(path->angle - tt->angle0)<0.1)tt->f_path = path ;
              if(ip){
                printf("    tt  uid = %i, trk_sector uid = %i, "
                     " MSTS angles east_x = %f, height_y = %f, north_z = %f, length = %f\n",
                     tt->w_uid, ts->uid,
                     tv->a_east_x,tv->a_height_y*180.0/pi,tv->a_north_z,
                     tv->length) ;
                printf("        trk_sector uid = %i, length_of_vector = %i, pins = %i %i "
                ":: %i %i %i :: %i %i %i\n",
                     ts->uid, ts->length_of_vector,
                     ts->type_of_pin[0], ts->type_of_pin[1],
                     ts->pin_to_section[0], ts->pin_to_section[1], ts->pin_to_section[2],
                     ts->pin_info[0], ts->pin_info[1], ts->pin_info[2]) ;
                ts1 = &(track_db.trk_sections_array[ts->pin_to_section[0]-1]) ;
                printf("        trk_sector uid = %i, length_of_vector = %i, pins = %i %i "
                ":: %i %i %i :: %i %i %i\n",
                     ts1->uid, ts1->length_of_vector,
                     ts1->type_of_pin[0], ts1->type_of_pin[1],
                     ts1->pin_to_section[0], ts1->pin_to_section[1], ts1->pin_to_section[2],
                     ts1->pin_info[0], ts1->pin_info[1], ts1->pin_info[2]) ;
                ts1 = &(track_db.trk_sections_array[ts->pin_to_section[1]-1]) ;
                printf("        trk_sector uid = %i, length_of_vector = %i, pins = %i %i "
                ":: %i %i %i :: %i %i %i\n",
                     ts1->uid, ts1->length_of_vector,
                     ts1->type_of_pin[0], ts1->type_of_pin[1],
                     ts1->pin_to_section[0], ts1->pin_to_section[1], ts1->pin_to_section[2],
                     ts1->pin_info[0], ts1->pin_info[1], ts1->pin_info[2]) ;
                printf("\n") ;
              }
            }
          }
        }
      }
/*
 *  Print data on turntable and path
 */
      if(ip){
        printf("\n") ;
        for(tt=turntablelist_beg; tt!=NULL; tt=tt->next){
          printf("    Turntable.\n") ;
          printf("    World item :: tile = %i %i :: uid = %i, tt angle0 = %f\n",
                 tt->tile_x, tt->tile_y, tt->w_uid, tt->angle0) ;
#if 1
          for(path = tt->path; path!= NULL; path=path->next){
            printf("      Path angle = %7.2f\n", path->angle) ;

            ts = path->trk_sector ;
            for(i=0;i<ts->length_of_vector;i++){
              tv = &(ts->vector[i]) ;
              printf("  Vector %i  ang = %f, X, Y, Z = %f %f %f, a_height_y = %f, length = %f\n",
                 i, tv->ang, tv->east_x, tv->height_y, tv->north_z,
                 tv->a_height_y*180/pi, tv->length) ;
            }
#endif
          }
          printf("\n") ;
        }
        printf("\n") ;
      }
      return 0 ;
}
