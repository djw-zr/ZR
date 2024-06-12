/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  update_trains.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for initialising trains, consists, wagons and travellers
 *
 *==============================================================================
 */

/*
 *   Routine to update train and wagon positions
 */


int update_trains(void){

  int       ip = 0 ;
  int       ipp = 0 ;

  int       iret = 0 ;  //  Return code
  int       i_crash   = 0 ;
  int       i_collide = 0 ;
  int       idirect  ;
  int       isect    ;
  TrainNode *t0, *t1 = NULL ;
  WagonNode *w           ;
  TravellerNode *z0, *z1, gz0, gz1 ;
  TrkSector     *ts      ;
  double    d0, d1, d2   ;
  double    length       ;
  double    time, del_t  ;
  double    dist_to_move ;
  double    dist_remain, dist_moved   ;
  char      *my_name = "update_trains" ;


      ipp = ip && i_zrt ;
      if(ipp){
        printf("\n  Enter %s.  Run seconds = %f :: %i %i %i\n",my_name,run_seconds,
               ip,i_zrt,ipp) ;
//        print_train_data() ;
      }
      time = run_seconds ;
/*
 *==============================================================================
 *  Loop over trains
 *==============================================================================
 */
      for(t0 = trainlist_beg; t0 != NULL ; t0=t0->next){

        ip = ipp && (t0->speed != 0.0) ;
        if(ip)printf("  Routine %s. Main Loop :: train = %s, speed = %f\n",
                                                 my_name,t0->name,t0->speed) ;
        del_t = delta_seconds ;
        if(0.0 == t0->speed) continue ;    //  Nothing to be done
        if(ip)printf("\n  Routine %s.  Train is moving\n",my_name) ;
/*
 *  If player train is moving set camera change
 *  NOTE:  Only needed if current camera is connected to player train
 */
        if(t0 == player_train)camera_moved = 1 ;
        junction_error = 0 ;
/*
 *  First move to limit of train in direction of motion
 */
        if(t0->speed > 0.0){
          w = t0->first          ;    //  Front of train
          if(w == NULL) return 0 ;
          gz0  = *w->traveller   ;    //  Copy to temporary traveller
          gz0.wagon = NULL       ;
          if(ip)printf(" Routine %s AA call trv_move\n",my_name) ;
          iret = trv_move(&gz0, 0.5*w->raw_wagon->length) ;
        }else{
          w = t0->last           ;    //  Back of train
          if(w == NULL) return 0 ;
          gz0 = *w->traveller    ;    //  Copy to temporary traveller
          gz0.wagon = NULL       ;
          if(ip)printf(" Routine %s BB call trv_move\n",my_name) ;
          iret = trv_move(&gz0,-0.5*w->raw_wagon->length) ;
        }
        if(iret || junction_error){
          printf("  Routine %s.  Train overlaps buffers or incorrectly"
                                                    " set junction\n",my_name) ;
          trv_print(&gz0) ;
          exit (0) ;
        }
        gz1  = gz0             ;    //  Make a copy
/*
 *  Next check for train collision with buffers or incorrectly set junction
 *  during the current timestep
 */
        dist_to_move = del_t*t0->speed ;
        dist_remain  = dist_to_move ;

       if(ip)printf(" Routine %s CC call trv_move\n",my_name) ;
       iret = trv_move(&gz1, dist_to_move) ; //  Move temporary traveller
/*
 *  If crash detected, print error and change timestep for this train
 *  Unfortunately dist_remain is not changed (yet).
 */
        i_crash = (iret || junction_error) ;
        if(i_crash){
          del_t = (fabs(dist_to_move) - fabs(dist_remain))/fabs(t0->speed) ;
          if(iret){
            printf("  Train %s crashed into end of line.\n",t0->name) ;
          }else{
            printf("  Train %s derailed at junction.\n",t0->name) ;
          }
/*
 *  If the train has not crashed, user traveller gz0 to check for
 *  collisions with other trains.  .
 */
        }else{
          dist_moved  = 0.0 ;
          dist_remain = dist_to_move ;
          do{
            idirect = gz0.idirect  ;
            isect   = gz0.itrack   ;
            ts      = &track_db.trk_sections_array[isect-1] ; //  Remember '-1'!
            if(ip)printf("\n  Track section = %i, idirect = %i, speed = %f, name = %s"
                   "   dist remain = %f\n",
                        isect, idirect, t0->speed,
                        (gz0.wagon) ? gz0.wagon->name : "(null)", dist_remain ) ;

            if(gz0.idirect && t0->speed>0.0){
              d0 = gz0.sect_distance ;
              d1 = d0 + dist_remain ;
              if(d1 > ts->length){
                d1 = ts->length ;
                dist_remain  -= (ts->length - d0) ;
              }else{
                dist_remain = 0.0 ;
              }
            }else if(gz0.idirect && t0->speed<0.0){
              d1   = gz0.sect_distance ;
              d0   = d1 + dist_remain  ;
              if(d0 < 0.0){
                d0 = 0 ;
                dist_remain += d1 ;
              }else{
                dist_remain = 0.0 ;
              }
            }else if(!gz0.idirect && t0->speed>0.0){
              d1   = gz0.sect_distance ;
              d0   = d1 - dist_remain  ;
              if(d0 < 0.0){
                d0 = 0 ;
                dist_remain -= d1 ;
              }else{
                dist_remain = 0.0 ;
              }
            }else{
              d0 = gz0.sect_distance ;
              d1 = d0 - dist_remain ;
              if(d1 > ts->length){
                d1 = ts->length ;
                dist_remain  -= (ts->length - d0) ;
              }else{
                dist_remain = 0.0 ;
              }
            }
            if(ip)printf("  TT  d0 = %f, d1 = %f, d_remain = %f\n",d0, d1, dist_remain) ;
/*
*  Loop over all other trains searching for collisions.
*  This is linited to collisions between the front and end of trains
*  Collisions with wagons at crossovers and junctions are not covered.
*/
            i_collide = 0 ;
            for(t1 = trainlist_beg; t1!=NULL; t1=t1->next){
              if(t1 == t0)continue ;
              z0 = &(t1->front) ;
              z1 = &(t1->back)  ;
              if(ip)printf("    Check for train collision\n") ;
              if(ip)printf("    Train0 = %s, Front = %i %f, Back = %i %f  "
                                "Motor = %i %f\n",
                    t0->name, t0->front.itrack,
                              t0->front.sect_distance,
                              t0->back.itrack,
                              t0->back.sect_distance,
                    (t0->motor) ?t0->motor->traveller->itrack : 0,
                    (t0->motor) ?t0->motor->traveller->sect_distance : 0.0) ;

              if(ip)printf("    Train1 = %s, Front = %i %f, Back = %i %f  "
                           "Motor = %i %f  ::   %i %f :: %i %f %f\n",
                    t1->name, z0->itrack, z0->sect_distance,
                              z1->itrack, z1->sect_distance,
                    (t1->motor) ?t1->motor->traveller->itrack : 0,
                    (t1->motor) ?t1->motor->traveller->sect_distance : 0.0,                              gz0.itrack, gz0.sect_distance, isect, d0, d1) ;
  /*
  *  Does either end of the second train lie in the same track section
  *              and within the distance moved by the ?
  */
              if(ip)printf("  Routine %s.  Z0 :: %f %f :: %f\n",my_name,
                                        d0, d1, z0->sect_distance ) ;
              if(ip)printf("  Routine %s.  Z1 :: %f %f :: %f\n",my_name,
                                        d0, d1, z1->sect_distance ) ;
              if(isect == (int) z0->itrack){
                if(z0->sect_distance > d0 && z0->sect_distance < d1){
                  i_collide = 1 ;   //  Collision with front of train t1
                  if(ip)printf("  COLLIDE with front of train t1\n") ;
                  dist_moved += (z0->idirect) ? (z0->sect_distance - d0)
                                              : (d1 - z0->sect_distance) ;
                  break ;
                }
              }
              if(isect == (int)z1->itrack){
                if(z1->sect_distance > d0 && z1->sect_distance < d1){
                  i_collide = -1 ;   //  Collision with back of train t1
                  if(ip)printf("  COLLIDE with back of train t1\n") ;
                  dist_moved += (z0->idirect) ? (z1->sect_distance - d0)
                                              : (d1 - z1->sect_distance) ;
                  break ;
                }
              }
            }
            if(i_collide) break ;
/*
 *  If dist_remain != 0.0, move ghost traveller to next section.
 *  This also checks for possible crashes.
 */
            if(dist_remain == 0.0)break ;

            if( idirect == (dist_remain > 0.0) ){
              i_crash = trk_next(&gz0,1) ;
            }else{
              i_crash = trk_next(&gz0,-1) ;
            }
            if(i_crash){
              printf("  Routine %s error.  End of line or junction error"
                                " while checking for train collisions\n", my_name) ;
              printf("   idirect = %i\n",idirect) ;
              trv_print(&gz0) ;
              exit(1) ;
            }
          }while(dist_remain != 0.0) ;
/*
 *  In the case of train collisions, set the timestep sufficient to move the
 *  train to the collision point.  The coupling togeather of trains approaching
 *  each other at low speed is treated near the end.
 */
         if(i_collide){
            del_t = fabs(dist_moved/t0->speed) ;
            if(ip)printf(" Collide. Dist moved = %f, speed = %f, del_t = %f\n",
                          dist_moved, t0->speed, del_t) ;
          }
        }
/*
 *  End of checks for current train.
 *  Next update each of the engines and wagons in the train
 */
        for(w=t0->last; w!=NULL; w=w->prev){
          if(w == t0->last){
            dist_moved = del_t*t0->speed ;
          }else{
            dist_moved = (w->dist_back + w->next->dist_front)        ;
            copy_traveller_position(w->traveller,w->next->traveller) ;
          }
          if(ip)printf(" Routine %s DD call trv_move\n",my_name) ;
          iret = trv_move(w->traveller, dist_moved) ;
/*
 *  Update wheels on wagon
 */
          dist_moved = del_t*t0->speed ;
          if(w->train_dir){
            w->wheel_angle += degree*dist_moved*w->raw_wagon->inv_wheelradius ;
          }else{
            w->wheel_angle -= degree*dist_moved*w->raw_wagon->inv_wheelradius ;
          }
          while(w->wheel_angle > 360.0) w->wheel_angle -= 360.0 ;
          while(w->wheel_angle <   0.0) w->wheel_angle += 360.0 ;

          if(w->is_engine){
            if(w->train_dir){
              w->driverwheel_angle +=
                              degree*dist_moved*w->raw_wagon->inv_driverwheelradius ;
            }else{
              w->driverwheel_angle -=
                              degree*dist_moved*w->raw_wagon->inv_driverwheelradius ;
            }
            while(w->driverwheel_angle > 360.0) w->driverwheel_angle -= 360.0 ;
            while(w->driverwheel_angle <   0.0) w->driverwheel_angle += 360.0 ;
          }
        }  //  End loop over wagons
/*
 *  Update train's front and back travellers
 */
        copy_traveller_position(&(t0->front), t0->first->traveller) ;
        if(ip)printf(" Routine %s EE call trv_move\n",my_name) ;
        trv_move(&(t0->front), 0.5*t0->first->raw_wagon->length) ;
        copy_traveller_position(&(t0->back), t0->last->traveller) ;
        if(ip)printf(" Routine %s FF call trv_move\n",my_name) ;
        trv_move(&(t0->back), -0.5*t0->last->raw_wagon->length) ;
/*
 *  Current train in final position ;  Process any train collision.
 */
        if(i_collide != 0){
          printf("  TRAINS COLLIDE   Speed = %f\n", t0->speed) ;
          if(!t1){
            printf("  Routine %s error."
                   "  Train pointer t1 set to NULL after crash.\n",my_name) ;
          }
          if(fabs(t0->speed) < 2.0){               //  A bit high!
            join_trains(t0,t1,i_collide) ;
          }else{
            crash_trains(t0,t1,i_collide) ;
          }
        }else if(i_crash != 0){
          crash_train(t0,i_crash) ;
        }
      }    //  End loop over trains
/*
 *==============================================================================
 *  Second loop over trains and wagons
 *  Update traveller X, Y, Z variables and variables which
 *         do not involve train movement using full del_t
 *==============================================================================
 */
      del_t = delta_seconds ;
//      ip = 0 && l_time_5s ;

  double   a, b, c, rx0, ry0, rz0, xx, yy, zz ;
  double   rail_height = 0.27 ;  // Best fit placing wheels on track
  GLfloat  scalei = 1.0/plot_scale ;
  TrkVectorNode *vn ;
  TravellerNode *tv ;         ;

      for(t0 = trainlist_beg; t0 != NULL ; t0=t0->next){
        for(w=t0->last; w!=NULL; w=w->prev){
/*
 *  Update traveller variables
 */

          tv = w->traveller ;
          rx0 = tv->rx;
          ry0 = tv->ry;
          rz0 = tv->rz;

          vn = tv->vn ;
          a = degree*vn->a_east_x   ;
          b = degree*vn->a_height_y ;
          c = degree*vn->a_north_z  ;

          global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                             vn->tile_east_x, vn->tile_north_z,
                             vn->east_x, vn->north_z, vn->height_y+rail_height,
                             &xx, &yy, &zz ) ;
          mstswagon2local(0.0, 0.0, 0.0, tv->x, tv->y, tv->z, a, b, c, scalei,
                             xx, yy, zz,
                             &(tv->rx), &(tv->ry), &(tv->rz) ) ;
          if(ip){
            printf("  UPDATE TRAINS\n");
            printf("  Wagon         = %s\n",w->name) ;
            printf("  Vector origin = %f %f %f\n",tv->x,tv->y,tv->z) ;
            printf("  Track  origin = %f %f %f\n",xx,yy,zz) ;
            printf("  Position      = %f %f %f\n",tv->rx,tv->ry,tv->rz) ;
          }
          tv->rx = tv->rx*plot_scale ;
          tv->ry = tv->ry*plot_scale ;
          tv->rz = tv->rz*plot_scale + tile_h0 ;
          xx = lookat_eye_x*tile_size ;
          yy = lookat_eye_y*tile_size ;
          zz = lookat_eye_z*tile_size + tile_h0 ;
          if(ip){
            printf("  Position (m)  = %f %f %f\n",tv->rx,tv->ry,tv->rz) ;
            printf("  Eye (m)       = %f %f %f\n",xx, yy, zz) ;
          }

          if(del_t == 0.0){
            tv->vx = tv->vy = tv->vz = 0.0 ;
          }else{
            tv->vx = (tv->rx - rx0)/del_t ;
            tv->vy = (tv->ry - ry0)/del_t ;
            tv->vz = (tv->rz - rz0)/del_t ;
          }
/*
 *  Engine Wipers
 */
          if(w->has_wipers && w->wipers_on){
//            printf("    WIPER ANG = %f, dell_t = %f\n",w->wiper_ang,del_t) ;
            w->wipers_ang = w->wipers_ang
                                           + del_t*180.0 ;    //  Back and forth in 2s
            if(w->wipers_ang > 360.0){
              if(w->wipers_off){                        //  Park wipers
                w->wipers_ang = 0.0 ;
                w->wipers_on  = 0   ;
                w->wipers_off = 0   ;
              }else{
                w->wipers_ang -= 360.0 ;
              }
            }
          }
          if(ip){
            printf("   Wagon = %s\n",player_train->motor->name);
            printf("   has_wipers = %i\n",player_train->motor->has_wipers);
            printf("   wipers_on  = %i\n",player_train->motor->wipers_on);
            printf("   wipers_off = %i\n",player_train->motor->wipers_off);
            printf("   WIPERs ANG = %f, dell_t = %f,  wipers_on = %i\n",
                  w->wipers_ang,del_t,w->wipers_on) ;
          }
/*
 *  Engine Mirrors
 */
          if(w->has_mirrors){
            if(w->mirrors_out && w->mirrors_dist < 1.0){
              w->mirrors_dist = w->mirrors_dist + del_t ;    //  Open in 1s
              if(w->mirrors_dist > 1.0) w->mirrors_dist = 1.0 ;
            }else if(!w->mirrors_out && w->mirrors_dist > 0.0){
              w->mirrors_dist = w->mirrors_dist - del_t ;    //  Close in 1s
              if(w->mirrors_dist < 0.0) w->mirrors_dist = 0.0 ;
            }
          }
          if(ip){
            printf("   Wagon = %s\n",player_train->motor->name);
            printf("   has_mirrors = %i\n",player_train->motor->has_mirrors);
            printf("   mirrors_out = %i\n",player_train->motor->mirrors_out);
            printf("   MIRROR DIST = %f, dell_t = %f,  mirrors_out = %i\n",
                  w->mirrors_dist,del_t,w->mirrors_out) ;
          }
/*
 *  Engine Pantographs
 */
          if(w->has_pantographs){
            if(w->pantographs_up && w->pantographs_dist < 1.0){
              w->pantographs_dist = w->pantographs_dist + del_t ;    //  Open in 1s
              if(w->pantographs_dist > 1.0) w->pantographs_dist = 1.0 ;
            }else if(!w->pantographs_up && w->pantographs_dist > 0.0){
              w->pantographs_dist = w->pantographs_dist - del_t ;    //  Close in 1s
              if(w->pantographs_dist < 0.0) w->pantographs_dist = 0.0 ;
            }
          }
          if(ip){
            printf("   Wagon = %s\n",player_train->motor->name);
            printf("   has_pantographs = %i\n",player_train->motor->has_pantographs);
            printf("   pantographs_up  = %i\n",player_train->motor->pantographs_up);
            printf("   PANTOGRAPH DIST = %f, dell_t = %f,  pantographs_up = %i\n",
                    w->pantographs_dist,del_t,w->pantographs_up) ;
          }
        }
/*
 *  End loop over trains
 */
      }
      time_last = time ;
      if(ip)printf("  Exit %s\n",my_name) ;
      return 0 ;
}
