 /*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  update_level_crossing.c
 *
 *   This file contains routines used to update the state of the level
 *   crossings each timestep.
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *==============================================================================
 */

uint search_for_track_item(TravellerNode *t, LevelXSNode *s, double d, uint idir) ;
uint search_for_track_item2(TravellerNode *t1, TravellerNode *t2, LevelXSNode *lc) ;
uint update_level_crossing_flag(WorldItem *wi, int f) ;
uint update_level_crossing_position(WorldItem *wi) ;

int update_level_crossings(void){

  int     ip = 0 ;
  int     i, found ;
  double  dist, max_dist ;
  double  dist_forward   ;
  double  dist_back      ;

  double      warn_time  = 20.0 ;   //  Warning time (s)
  double      extra_dist = 20.0 ;   //  Extra distance (m) from crossing

  WorldNode   *wnode   ;                      // world.h
  LevelXSNode *lxsnode ;                      // world.h
  WorldItem   *witem   ;                      // world.h
  TrkItemNode *ti_node ;                      // struct.h
  TrainNode   *train   ;                      // train.h
  WagonNode     *wagon1, *wagon2 ;            // train.h
  TravellerNode *trav1,  *trav2  ;            // train.h
  TrkVectorNode *vect1           ;            // struct.h

  int          ti_tile_east, ti_tile_north,
               wa_tile_east, wa_tile_north  ;
  double       ti_east, ti_north, ti_height,
               wa_east, wa_north, wa_height,
               tx, ty, tz,                     // Position of traveller (m)
               wx, wy, wz             ;        //  Position of wagon (m)
  int          itrack, ivector, idirect     ;
  char *my_name = "update_level_crossing" ;


      if(ip)printf("  Enter routine %s\n",my_name) ;
      if(NULL == trainlist_beg) return 0 ;     // No trains
/*
 *   Loop to operate crossings only
 */
      if(!l_time_1s){
        for(wnode=worldlist_beg ; wnode != NULL; wnode = wnode->next){
          if(0 == wnode->n_level_XS)continue ;
          for(i = 0 ; i < (int) wnode->n_level_XS; i++){
            lxsnode = &(wnode->level_XS[i]) ;
            witem   = *(lxsnode->world_item) ;
            update_level_crossing_position(witem) ;
          }
        }
        return 0 ;
      }

/*
 *  Loop over world tiles
 */
      for(wnode=worldlist_beg ; wnode != NULL; wnode = wnode->next){
        if(0 == wnode->n_level_XS)continue ;
/*
 *  Loop over level crossings in tile
 */
        for(i = 0 ; i < (int) wnode->n_level_XS; i++){
          lxsnode = &(wnode->level_XS[i]) ;
          witem   = *(lxsnode->world_item) ;
/*
 *  Determine position of first track item at Level Crossing
 *  global2local() is defined in graphics.c
 */
          ti_node = &(track_db.trk_items_array[lxsnode->rail_track_item[0]]) ;
          ti_tile_east  = ti_node->tile_east_x ;
          ti_tile_north = ti_node->tile_north_z ;

          global2local(tile_x0, tile_y0, tile_h0, tile_size, 1.0,
                            ti_node->tile_east_x, ti_node->tile_north_z,
                            ti_node->east_x, ti_node->north_z, ti_node->height_y,
                            &tx, &ty, &tz) ;
/*
 *  Loop over trains
 */
          for(train=trainlist_beg; train != NULL; train=train->next){
            wagon1 = train->first ;
            wagon2 = train->last  ;
            trav1  = wagon1->traveller ;
            trav2  = wagon2->traveller ;
            vect1  = trav1->vn ;
            wa_tile_east  = vect1->wfname_east_x  ;
            wa_tile_north = vect1->wfname_north_z ;
/*
 *  Skip if on a distant tile
 */
            if(abs(wa_tile_east-ti_tile_east)>1
              || abs(wa_tile_north-ti_tile_north)>1) continue ;
/*
 *  Distance moved in next 20s plus 100 m
 *  Eventually this should allow for acceleration etc.
 */
            max_dist = fabs(train->speed)*warn_time ;
/*
 *  Calculate position of traveller in ZR space (m)
 */
            traveller2local(trav1, &wx, &wy, &wz) ;
            dist = sqrt((tx-wx)*(tx-wx) + (tz-wz)*(tz-wz) + (tz-wz)*(tz-wz) ) ;
            found = dist < max_dist + 100.0 ;
            if(!found && wagon1 != wagon2){
              traveller2local(trav2, &wx, &wy, &wz) ;
              dist = sqrt((tx-wx)*(tx-wx) + (tz-wz)*(tz-wz) + (tz-wz)*(tz-wz) ) ;
              found = dist < max_dist + 100.0 ;
            }
            if(!found)continue ;
/*
 *  Search from train to track item
 */
            if(ip){
              printf("   Train: %s near World Item: %s.  Track item: %i."
                     "  Node type = %i %s. "
                     "   Search distance = %f, distance = %f\n",
                            train->name, witem->filename, ti_node->index_of_node,
                            ti_node->type_of_node, token_trackdb[ti_node->type_of_node],
                            max_dist+100.0, dist) ;
              printf("     Traveller at  :  %f %f %f\n",wx, wy, wz) ;
              printf("     Track Item  at:  %f %f %f\n",tx, ty, tz) ;
              printf("  NEW TRAIN %p\n",(void *) train) ;
            }
            dist = 0.5*wagon1->raw_wagon->length + 10.0 ;
            if(train->speed > 0.0){
               dist_forward = dist + max_dist ;
               dist_back    = dist ;
            }else{
               dist_forward = dist  ;
               dist_back    = dist + max_dist ;
            }
            if(ip)printf("  SEARCH Forward  %f\n",dist_forward) ;
            found = search_for_track_item(trav1, lxsnode, dist_forward,1) ;
            if(ip && found)printf("  Found level crossing  AA %i\n",i) ;
            if(!found){
              if(ip)printf("  SEARCH Back  %f\n",dist_back) ;
              found = search_for_track_item(trav2, lxsnode, dist_back,0) ;
              if(ip && found)printf("  Found level crossing  BB %i\n",i) ;
            }
            if(!found){
              if(ip)printf("  SEARCH Between\n") ;
              found = search_for_track_item2(trav1, trav2, lxsnode) ;
              if(ip && found)printf("  Found level crossing  CC %i\n",i) ;
            }
            update_level_crossing_flag(witem,found) ;
            update_level_crossing_position(witem) ;
          }
        }
      }
      return 0 ;
}

/*
 *  Routine to search forward from TravellerNode t for a distance d
 *  to look for the Track Item specified in LevelXSNode lc.
 *     s_dirn  :: (1/0) 1 if Search Direction matches Train Direction
 */


uint search_for_track_item(TravellerNode *t, LevelXSNode *lc, double d, uint s_dirn){

  uint     i, ip = 0   ;
  int      iret        ;
  uint     tr_index    ;    //  Current track index
  uint     tr_vect     ;    //  Current vector index
  uint     tr_dirn     ;    //  1/0, 1 if Vector Direction equals Train Direction
  double   dd_remain   ;    //  Distance remaining
  double   dd_move     ;    //  Distance to move
  double   tr_posn     ;    //  Position of traveller along current vector
  double   tr_length   ;    //  Length of current track vector
  double   ti_posn     ;    //  Position of current track item along vector
  TravellerNode  tr    ;    //  Pseudo traveller node
  char     *my_name = "search_for_track_item" ;

      if(ip)printf("    Enter routine %s\n",my_name) ;
      if(ip)printf("    distance = %f,  direction = %i\n",d,s_dirn) ;
      tr        = *t ;
      tr.wagon  = NULL ;     //   A pseudo traveller with no wagon
      dd_remain =  d ;

      for(;;){
        tr_index  = tr.itrack     ;
        tr_vect   = tr.ivector    ;
        tr_posn   = tr.position   ;
        tr_length = tr.vn->length ;
        tr_dirn   = tr.idirect    ;

/*
 *  Return 1 if current track/vector/position match any of the search values?
 */
        for(i=0;i<lc->n_rail;i++){
          if(ip)printf("   tr  %i ::  index = %i, vector = %i, position = %f\n",
                 i, tr.itrack, tr.ivector, tr.position) ;
          if(ip)printf("   lc  %i ::  index = %i, vector = %i, position = %f\n",
                 i, lc->rail_index[i], lc->rail_i_vec[i], lc->rail_dist[i]) ;


          if(tr_index == lc->rail_index[i] && tr_vect == lc->rail_i_vec[i]){
            ti_posn = lc->rail_dist[i] ;
            if(ip)printf("       %i %i ::  %f %f :: %f %f\n",
                    s_dirn, tr_dirn, ti_posn, tr_posn, tr_length, dd_remain) ;
            if(s_dirn == tr_dirn){
              if(ti_posn >= tr_posn && ti_posn <= tr_length
                  && ti_posn <= tr_posn + dd_remain)  return 1 ;
            }else{
              if(ti_posn <= tr_posn && ti_posn >= 0.0
                  && ti_posn >= tr_posn - dd_remain)  return 1 ;
            }
          }
        }
        dd_remain = dd_remain - tr_length ;
        if(dd_remain <= 0.0)  break ;
/*
 *  Move pseudo traveller to next track vector
 */
        if(s_dirn == tr_dirn){
          iret = trv_next(&tr) ;
        }else{
          iret = trv_prev(&tr) ;
        }
        if(iret) return 0 ;        // End of track
      }
      return 0 ;
}

/*
 *  Routine used to search for track item forwards
 *         from traveller node t2 to traveler node t1
 */

uint search_for_track_item2(TravellerNode *t1, TravellerNode *t2, LevelXSNode *lc){

  uint     i, ip = 0   ;
  uint     ic    = 0   ;
  int      iret        ;
  uint     tr_index    ;    //  Current track index
  uint     tr_vect     ;    //  Current vector index
  uint     tr_dirn     ;    //  1/0, 1 if Vector Direction equals Train Direction
  uint     tr1_index   ;    //  Track index of traveller t1
  uint     tr1_vect    ;    //  Vector index of traveller t1
  double   tr1_posn    ;    //  Position of traveller t1
  double   ti_posn     ;    //  Position of current track item along vector
  TravellerNode  tr    ;    //  Pseudo traveller node
  char     *my_name = "search_for_track_item2" ;

      if(ip)printf("      Enter routine %s\n",my_name) ;
      tr        = *t2 ;
      tr1_index = t1->itrack   ;
      tr1_vect  = t1->ivector  ;
      tr1_posn  = t1->position ;
      if(ip){
        printf("   t1  ::  index = %i, vector = %i, position = %f\n",
                  t1->itrack, t1->ivector, t1->position) ;
        printf("   t2  ::  index = %i, vector = %i, position = %f\n",
                  t2->itrack, t2->ivector, t2->position) ;
        printf("   lc  ::  index = %i, vector = %i, position = %f,"
              "  rail_track_item = %i,  index = %i\n",
                  lc->rail_index[0], lc->rail_i_vec[0], lc->rail_dist[0],
                  lc->rail_track_item[0],
                  track_db.trk_items_array[lc->rail_track_item[0]].index_of_node) ;
        if(lc->n_rail>1)
        printf("   lc2 ::  index = %i, vector = %i, position = %f"
              "  rail_track_item = %i,  index = %i\n",
                  lc->rail_index[1], lc->rail_i_vec[1], lc->rail_dist[1],
                  lc->rail_track_item[1],
                  track_db.trk_items_array[lc->rail_track_item[1]].index_of_node) ;
      }

      for(;;ic++){
        tr_index  = tr.itrack     ;
        tr_vect   = tr.ivector    ;
        tr_dirn   = tr.idirect    ;

/*
 *  Return 1 if current track/vector/position match any of the search values?
 */
        for(i=0;i<lc->n_rail;i++){
          if(ip)printf("  TEST   %i  :: %i %i :: %i %i\n",
                 i,tr_index, lc->rail_index[i], tr_vect, lc->rail_i_vec[i]) ;
          if(tr_index == lc->rail_index[i] && tr_vect == lc->rail_i_vec[i]){
            if(tr_index != tr1_index || tr_vect != tr1_vect) return 1;
            ti_posn = lc->rail_dist[i] ;

            if(ip)printf("  TEST2  %i :: %f %f\n",tr_dirn,ti_posn, tr1_posn) ;
            if(tr_dirn){
              if(ti_posn <= tr1_posn)  return 1 ;
            }else{
              if(ti_posn >= tr1_posn)  return 1 ;
            }
            if(ip)printf("  FAIL\n") ;
            return 0 ;  //  Track section found but no track item
          }
        }
        if(ip)printf("  TEST3  %i %i :: %i %i\n",tr_index, tr1_index, tr_vect, tr1_vect) ;

        if(tr_index == tr1_index && tr_vect == tr1_vect) return 0 ;
/*
 *  Move pseudo traveller to next track vector
 */
        if(tr_dirn){
          iret = trv_next(&tr) ;
        }else{
          iret = trv_prev(&tr) ;
        }
        if(ip)printf(" NEW ::  %i %i %f\n",tr.itrack, tr.ivector, tr.position) ;
        if(iret){
          printf("  Routine %s error\n",my_name) ;
          printf("  End of track section found 'within train'!\n") ;
          exit(0) ;
        }
        if(ic > 1000){
          printf("  Routine %s error\n",my_name) ;
          printf("  Loop not exiting after searching %i track sections\n",ic) ;
          exit(0) ;
        }
      }

      return 0 ;
}


uint  update_level_crossing_flag(WorldItem *wi, int found){

  int  ip = 0 ;
  int  i ;
  LevelCrObj *lc    ;
  char     *my_name = "update_level_crossing_variables" ;

      if(ip)printf("      Enter routine %s\n",my_name) ;
      lc = &(wi->u.levelcr_obj) ;
      if(found){
        if(ip)printf("   Level Crossing should be open\n") ;
        lc->should_be_open = 0 ;
      }else{
        lc->should_be_open = 1 ;
      }
      return 0 ;
}

uint  update_level_crossing_position(WorldItem *wi){

  int  ip = 0 ;
  int  i ;
  LevelCrObj *lc    ;
  char     *my_name = "update_level_crossing_variables" ;

      if(ip)printf("      Enter routine %s\n",my_name) ;

      lc = &(wi->u.levelcr_obj) ;

      if(ip)printf("        AA should_be_open = %i, gate_position = %f, delta_seconds = %f\n",
                    lc->should_be_open, lc->gate_position, delta_seconds  ) ;


      if(lc->should_be_open && lc->gate_position != 0.0){
        lc->gate_position -= 0.1*delta_seconds ;
        lc->gate_position = (lc->gate_position > 0.0) ? lc->gate_position : 0.0 ;
      }else if(lc->gate_position != 0.5){
        lc->gate_position += 0.1*delta_seconds ;
        lc->gate_position = (lc->gate_position < 0.5) ? lc->gate_position : 0.5 ;
      }
      wi->anim_value = lc->gate_position ;

    if(ip)printf("        BB should_be_open = %i, gate_position = %f, delta_seconds = %f\n",
                    lc->should_be_open, lc->gate_position, delta_seconds  ) ;
        return 0 ;
}
