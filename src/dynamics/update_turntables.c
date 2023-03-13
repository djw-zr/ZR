/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  update_turntables.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines used to update turntables
 *
 *==============================================================================
 */

int  move_train_to_tt_track(TurnTable *tt)   ;
int  move_train_from_tt_track(TurnTable *tt) ;
int  search_for_track_vector(TrainNode *train, TurnTable *tt) ;

/*
 *   anim value = 0.0 initially
 *                1.0  complete rotation
 *                Speed = 1/60 revolutions/sec is stately
 */

int update_turntable(TurnTable *tt){

  int       ip = 0 ;
  int       final = 0 ;
  WorldItem *witem ;
  double    r_time = 60.0 ;  //  Time to rotate turntable
  double    offset ;
  double    ss, cc, r ;
  TrkVectorNode *tv ;
  TrainNode     *tr ;
  WagonNode     *wa ;
  TravellerNode *tz ;
  TrkVectorNode *vn ;
  char      *my_name = "update_turntable" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      if(!tt) return 0  ;
      witem = tt->witem ;
/*
 *  Normal rotation of turntable : clockwise or anti-clockwise
 */
      if(tt->i_dirn){
        witem->anim_value += tt->i_dirn*delta_seconds/r_time ;
        if(witem->anim_value >  1.0){
          witem->anim_value -= 1.0 ;
        }else if(witem->anim_value < 0.0){
          witem->anim_value += 1.0 ;
        }
        tt->angle = tt->angle0 - witem->anim_value*360.0 ;

        tt->trk_vector0.a_height_y = tt->angle*pi/180.0 ;
        tv = &(tt->trk_vector0) ;
        ss = sin((tt->angle+180.0)*pi/180.0) ;
        cc = cos((tt->angle+180.0)*pi/180.0) ;
        r   = tt->diameter*0.5 ;
        tv->east_x   = tt->x0 + ss*r + tt->x_offset ;
        tv->north_z  = tt->z0 + cc*r + tt->z_offset ;
        tv->height_y = tt->y0 + tt->y_offset ;
/*
 *  Final closure
 */
      }else if(tt->i_final){
        offset = tt->angle - tt->f_path->angle ;
        if(tt->i_180)offset = offset + 180.0 ;

        if(offset > 180.0){
          while(offset > 180.0)offset = offset - 360.0 ;
        }else if(offset < -180.0){
          while(offset < -180.0)offset = offset + 360.0 ;
        }
        if(offset > 0){
          if(offset > 360.0*delta_seconds/r_time){
            witem->anim_value += delta_seconds/r_time ;
          }else{
            witem->anim_value += offset/360.0 ;
            final = 1 ;
          }
          if(witem->anim_value > 1.0)witem->anim_value -= 1.0 ;
        }else{
          if(offset < -360.0*delta_seconds/r_time){
            witem->anim_value -= delta_seconds/r_time ;
          }else{
            witem->anim_value += offset/360.0 ;
            final = 1 ;
          }
          if(witem->anim_value < 0.0)witem->anim_value += 1.0 ;
        }
      }
      tt->angle = tt->angle0 - witem->anim_value*360.0 ;
      if(tt->angle < 0.0){
        while(tt->angle <   0.0) tt->angle += 360.0 ;
      }else  if(tt->angle > 360.0){
        while(tt->angle > 360.0) tt->angle -= 360.0 ;
      }

      tt->trk_vector0.a_height_y = tt->angle*pi/180.0 ;
      tv = &(tt->trk_vector0) ;
      ss = sin((tt->angle+180.0)*pi/180.0) ;
      cc = cos((tt->angle+180.0)*pi/180.0) ;
      r   = tt->diameter*0.5 ;
      tv->east_x   = tt->x0 + ss*r + tt->x_offset ;
      tv->north_z  = tt->z0 + cc*r + tt->z_offset ;
      tv->height_y = tt->y0 + tt->y_offset ;

      if(final){
        move_train_from_tt_track(current_turntable) ;
        tt->i_final = 0 ;
        current_turntable = NULL ;
      }

      if(ip && l_time_1s){
        printf("  Routine %s, i_dirn = %i, i_final = %i, "
                 "anim_value = %f, tt->angle = %f",
               my_name, tt->i_dirn,tt->i_final,  witem->anim_value, tt->angle) ;
        if(tt->f_path)printf(", tt->f_path->angle = %f",tt->f_path->angle) ;
        printf("\n") ;
      }

      camera_changed = 1 ;
      return 0 ;
}

int rotate_tt_clockwise(void){

  int  ip = 0   ;
  int  blocked ;
  char *my_name = "rotate_tt_clockwise" ;

      if(ip)printf("  Enter routine %s, turntable = %p\n",
                      my_name, (void *)current_turntable) ;

      if(current_turntable) return 0 ;  //  Skip if table turning
      current_turntable = find_tt_in_window() ;
      if(!current_turntable)return 0 ;
      if(ip)printf("              Turntable found = %p, f_path = %p\n",
                   (void *)current_turntable, (void *)current_turntable->f_path) ;
/*
 *  Check for train blocking turntable from turning
 *   1.  Check turntable is aligned
 *   2.  Check that train is not blocking
 *   3.  Move any train to turntable track
 */
      if(current_turntable->f_path){
        blocked = move_train_to_tt_track(current_turntable) ;
        if(blocked) return 0 ;
      }
      current_turntable->i_dirn  = -1 ;
      current_turntable->i_final =  0 ;
      return 1 ;  // Success
}


int rotate_tt_anticlockwise(void){

  int  ip = 0 ;
  int  blocked ;
  char *my_name = "rotate_tt_anticlockwise" ;

      if(ip)printf("  Enter routine %s, turntable = %p\n",
                      my_name, (void *)current_turntable) ;

      if(current_turntable) return 0 ;  //  Skip if table turning
      current_turntable = find_tt_in_window() ;
      if(!current_turntable)return 0 ;
      if(ip)printf("              Turntable found = %p, f_path = %p\n",
                   (void *)current_turntable, (void *)current_turntable->f_path) ;
/*
 *  Check for train blocking turntable from turning
 *   1.  Check turntable is aligned
 *   2.  Check that train is not blocking
 *   3.  Move any train to turntable track
 */
      if(current_turntable->f_path){
        blocked = move_train_to_tt_track(current_turntable) ;
        if(blocked) return 0 ;
      }
      current_turntable->i_dirn  =  1 ;
      current_turntable->i_final =  0 ;
      return 1 ;  // Success
}

int  end_tt_rotation(void){

  int  ip = 0 ;
  TT_Path   *path ;
  double    angle ;
  char *my_name = "end_tt_rotation" ;

      if(ip)printf("  Enter routine %s, turntable = %p\n",
                      my_name, (void *)current_turntable) ;
/*
 *   Check paths - is the turntable almost aligned?
 */
      for(path = current_turntable->path; path != NULL; path=path->next){
        angle = current_turntable->angle - path->angle ;
        if(angle > 180.0){
          while(angle > 180.0)angle = angle - 360.0 ;
        }else if(angle < -180.0){
          while(angle < -180.0)angle = angle + 360.0 ;
        }
        if(fabs(angle) < 10.0){
          current_turntable->f_path  = path ;
          current_turntable->i_final = 1    ;
          current_turntable->i_dirn  = 0    ;
          current_turntable->i_180   = 0    ;
          if(ip)printf("  Exit routine %s.  Turntable aligned\n",my_name) ;
          return 0;
        }
/*
 *  Is turntable alligned but in opposite direction?
 */
        angle = current_turntable->angle - path->angle - 180.0 ;
        if(angle > 180.0){
          while(angle > 180.0)angle = angle - 360.0 ;
        }else if(angle < -180.0){
          while(angle < -180.0)angle = angle + 360.0 ;
        }
        if(fabs(angle) < 10.0){
          current_turntable->f_path  = path ;
          current_turntable->i_final = 1    ;
          current_turntable->i_dirn  = 0    ;
          current_turntable->i_180   = 1    ;
          if(ip)printf("  Exit routine %s.  Turntable aligned 180 degrees\n",
                                                                  my_name) ;
          return 0;
        }
      }
/*
 *  Not aligned.  Stop rotation
 */
      current_turntable->i_dirn  = 0    ;
      current_turntable->i_final = 0    ;
      current_turntable->f_path  = NULL ;
      current_turntable          = NULL ;

      if(ip)printf("  Exit routine %s.  Turntable not aligned\n",my_name) ;
      return 0 ;
}

/*
 *   Routine to find the nearest turntable to the viewer observable in
 *   the window.
 */

TurnTable *find_tt_in_window(void){

  int         ip = 0 ;
  TurnTable   *tt,
              *tt0 = NULL ;
  WorldItem   *witem ;
  double      distance,
              distance0 = 0.0 ;
  char        *my_name = "find_tt_in_window" ;

      if(ip)printf("  Enter routine %s\n", my_name) ;

      for(tt=turntablelist_beg; tt!=NULL; tt=tt->next){
        witem = tt->witem ;
        distance = find_witem_in_scene(witem) ;
        if(distance > 0.0){
          if(!tt0 || distance < distance0){
            distance0 = distance ;
            tt0       = tt       ;
          }
        }
        if(ip)printf("  Routine %s.  Turntable %p, distance = %f, distance0 = %f\n",
                  my_name, (void *)tt, distance, distance0) ;
      }
      if(ip)printf("  Exit  routine %s\n", my_name) ;

      return tt0 ;
}

/*
 *  Routine to check that a given world item is observeable by the viewer.
 */

double find_witem_in_scene(WorldItem *witem){

  int         ip = 0 ;
  int         iret   ;
  double      a, b, c, d, x, y, z, sx, sy, sz, ssx, ssy, ssz,
              radius, ttx, tty, ttz, scalei, distance ;
  ShapeNode   *snode     ;
  ShapeVolume *shape_vol ;
  WorldNode   *wnode     ;
  char        *my_name = "find_witem_in_scene" ;
/*
 *  Determine position of shape origin and rotation
 */
     if(ip)printf("    Enter routine %s\n",my_name) ;
     wnode = witem->world_node ;
     scalei = 1.0/plot_scale   ;

     global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                    wnode->tile_x, wnode->tile_y,
                    witem->X, witem->Y, witem->Z, &x, &y, &z) ;
      a = witem->ANG ;
      b = witem->AX ;
      c = witem->AY ;
      d = witem->AZ ;
/*
 *  Determine position of shape volume and radius
 */
      snode = witem->snode ;
      shape_vol = &snode->shape_vol[0] ;
      sx     = shape_vol->vec.X  ; sx = sx*scalei ;
      sz     = shape_vol->vec.Y  ; sz = sz*scalei ;  //  Transform MSTS to zr axes
      sy     = shape_vol->vec.Z  ; sy = sy*scalei ;
      radius = shape_vol->radius ; radius = radius*scalei ;

      if(a != 0.0 && (sz != 0.0 ||sx != 0.0 ||sy != 0.0) ){
        zrRotateAxyz(a, b, c, d, sx, sy, sz, &ttx, &tty, &ttz) ;
        ssx = x + ttx ;
        ssy = y + tty ;
        ssz = z + ttz ;
      }else{
        ssx = x + sx ;
        ssy = y + sy ;
        ssz = z + sz ;
      }
/*
 *  Is the shape in the current scene?
 */
      iret = check_in_scene( (GLdouble) ssx,  (GLdouble) ssy, (GLdouble) ssz,
                              (GLdouble) 2.0*radius) ;
      if(ip) printf("    Routine %s.  check_in_scene returns %i\n",my_name, iret) ;
      if(iret == 0) return 0.0 ;
/*
 *  If so return distance to nearest point of sphere
 */
      distance = sqrt( (ssx - lookat_eye_x)*(ssx - lookat_eye_x)
                      +(ssy - lookat_eye_y)*(ssy - lookat_eye_y)
                      +(ssz - lookat_eye_z)*(ssz - lookat_eye_z) ) ;
      return distance ;
}

/*
 *  Routine to check truntable and if possible move trains from
 *    normal fixed track to rotating turntable track
 *  1. Check for trains on turntable.
 *  2.  If none : return 0.
 *  3.  Otherwise :
 *  4.    If train(s) overlapping edge of turntable : return 1
 *  5.    Otherwise : Transfer trains to turntable track
 *
 *    REMEMBER TO ADD DUMMY TRAIN COVERING TURNTABLE VECTOR
 */

int  move_train_to_tt_track(TurnTable *tt){

  int           ip = 0 ;
  int           iret, iret1 ;
  TrainNode     *tr  ;
  WagonNode     *wa  ;
  TrkVectorNode *tv  ;
  TravellerNode *trav ;
  RawWagonNode  *rw  ;
  TT_List       *tt_list,
                *tt_list_next ;
  int           itrack, ivector, idirect ;
  double        ss, cc, r ;
  char          *my_name = "move_train_to_tt_track" ;

      if(ip)printf("\n  Enter routine %s\n",my_name) ;
/*
 *  For each train - travel from front of train to back of train
 *                   looking for turntable sector and vector
 *  If sector and vector are found, but other sectors/vectors are also
 *     found then return 1.  Turntable cannot turn.
 *  Add trains to local list.
 */
      iret = 0 ;
      for(tr=trainlist_beg; tr!=NULL; tr=tr->next){
        iret = search_for_track_vector(tr, tt) ;
        if(ip){
          printf("  Routine %s.  Test train %s.  rret = %i\n",
                                                 my_name, tr->name, iret) ;
        }
/*
 *  If turntable blocked
 */
        if(iret == -1) return 1 ;  //  Turntable blocked
/*
 *  If train on turntable
 */
        if(iret){
/*
 *  Generate dummy track values
 */
          if(ip)printf("    Train on turntable.  tt->tt_list = %p\n",
                                         (void *)tt->tt_list) ;
          if(!tt->tt_list){
            if(ip)printf("    Generate dummy track.  angle = %f angle0 = %f\n",
              tt->angle, tt->angle0) ;
            tv = &(tt->trk_vector0) ;
            ss = sin((tt->angle+180.0)*pi/180.0) ;
            cc = cos((tt->angle+180.0)*pi/180.0) ;
            r   = tt->diameter*0.5 ;
            tv->east_x   = tt->x0 + ss*r + tt->x_offset ;
            tv->north_z  = tt->z0 + cc*r + tt->z_offset ;
            tv->height_y = tt->y0 + tt->y_offset ;
            tv->a_east_x   = 0.0 ;
            tv->a_north_z  = 0.0 ;
            tv->a_height_y = tt->az0 ;
          }
/*
 *  Add train to list of trains on turntable
 */
          tt_list = (TT_List *)malloc(sizeof(TT_List *)) ;
          tt_list->next  = tt->tt_list ;
          tt_list->train = tr     ;
          tt->tt_list   = tt_list ;
          if(ip){
            for(tt_list=tt->tt_list; tt_list!=NULL; tt_list = tt_list->next)
              printf("  tt_list = %p, tt_list->next = %p, tt_list->train = %p\n",
                     (void *)tt_list, (void *)tt_list->next, (void *)tt_list->train) ;
          }
/*
 *  Move train to dummy turntable
 */
          if(ip)printf("    Move train %s to dummy turntable\n", tr->name) ;
          for(wa = tr->first; wa!=NULL; wa=wa->next){
            trav = wa->traveller ;
            trav->tn = &(tt->trk_sector0) ;
            trav->vn = &(tt->trk_vector0) ;
            trav->itrack  = 0 ;
            trav->ivector = 0 ;
            if(tt->i_180){
              trav->idirect = (trav->idirect) ? 0 : 1 ;
              trav->vect_position = tt->diameter - trav->vect_position ;
              trav->sect_distance = tt->diameter - trav->sect_distance ;
            }
            trv_move(trav,0.001) ;
          }
          if (ip)printf("     Routine %s.    Train %s moved to dummy turntable\n",
                            my_name, tr->name) ;
        }
      }
/*
 *  If no trains found return 0.  Turntable can turn.
 */
      return 0 ;
}


/*
 *  Routine to move any trains from rotating turntable track back
 *    to the normal fixed track.
 */

int  move_train_from_tt_track(TurnTable *tt){

  int           ip = 0   ;
  TT_List       *tt_list ;
  TT_List       *tt_list_prev ;
  TT_Path       *f_path  ;
  TrainNode     *tr      ;
  WagonNode     *wa      ;
  TravellerNode *trav ;
  char      *my_name = "move_train_from_tt_track" ;
/*
 *   If no trains return 0
 */
      if(!tt || !tt->tt_list) return 0 ;
      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
 *  Move trains from the turntable track to the real track allowing
 *     for the orientation of the turntable.
 */
      f_path = tt->f_path ;
      tt_list_prev = NULL ;
      for(tt_list=tt->tt_list; tt_list!=NULL; tt_list=tt_list->next){
        tr = tt_list->train ;
        for(wa = tr->first; wa!=NULL; wa=wa->next){
          trav = wa->traveller ;
          trav->tn = f_path->trk_sector ;
          trav->vn = f_path->trk_vector ;
          trav->itrack  = f_path->trk_sector_uid ;
          trav->ivector = f_path->trk_vector_id  ;
          if(tt->i_180){
            trav->idirect = (trav->idirect) ? 0 : 1 ;
            trav->vect_position = tt->diameter - trav->vect_position ;
            trav->sect_distance = tt->diameter - trav->sect_distance ;
          }
          trv_move(trav,0.001) ;
        }

        copy_traveller_position(&(tr->front), tr->first->traveller) ;
        trv_move(&(tr->front), 0.5*tr->first->raw_wagon->length) ;
        copy_traveller_position(&(tr->back), tr->last->traveller) ;
        trv_move(&(tr->back), -0.5*tr->last->raw_wagon->length) ;

        if(tt_list_prev)free(tt_list_prev) ;
        tt_list_prev = tt_list ;
        if(ip)printf("    Move train %s\n",tr->name) ;
      }
      if(tt_list_prev)free(tt_list_prev) ;
      tt->tt_list = NULL ;

      if(ip)printf("  Exit  routine %s\n",my_name) ;
      return 0 ;
}

/*
 *  Routine to search trains to see if any are in contact with
 *  turntable tt
 *  Return -1 :: Turntable blocked by train
 *          0 :: Turntable not affected by train
 *          1 :: Turntable can move - contains train
 */

int search_for_track_vector(TrainNode *train, TurnTable *tt){

  int   i, ip = 0 ;
  int   l1, l2, l3, iret, i_path ;

  uint  tr_sect  ;  //  Current track sector (array index + 1)
  uint  tr_vect  ;  //  Current track vector
  uint  tr_dirn  ;
  uint  ts_dirn  ;  // Search direction relative to track
  double dd_remain ;
  double tt_length ;

  TrkSector     *tt_sect ;
  TrkVectorNode *tt_vect ;
  int            tt_sect_uid, tt_vect_id ;
  TravellerNode tr1, tr2, tr ;  // Pseudo traveller node
  TT_Path       *path ;
  char     *my_name = "search_for_track_vector" ;

      if(ip){
        printf("    Enter routine %s\n",my_name) ;
      }

      tt_length = tt->diameter ;

/*
 *  Check if front or back are on turntable
 */
      tr1 = train->front ;
      tr2 = train->back  ;
      for(path = tt->path; path!= NULL; path=path->next){
        tt_sect_uid = path->trk_sector_uid ;
        tt_vect_id  = path->trk_vector_id  ;

        if(ip)printf("  Turntable   sector_uid = %i, vector_id = %i"
                     "      Train %s,  sector_uid = %i : %i, vector_id = %i : %i\n",
                     tt_sect_uid, tt_vect_id, train->name,
                     tr1.itrack, tr2.itrack, tr1.ivector, tr2.ivector) ;

        l1 = ((int) tr1.itrack == tt_sect_uid && (int) tr1.ivector == tt_vect_id) ;
        l2 = ((int) tr2.itrack == tt_sect_uid && (int) tr2.ivector == tt_vect_id) ;
        if(ip)printf("  l1 = %i, l2 = %i\n",l1,l2) ;
        if(l1 || l2){
          if(l1 && l2){
            l3=  ( tr1.vect_position > 1.0 && tr2.vect_position < tt_length-1.0)
               ||( tr2.vect_position > 1.0 && tr1.vect_position < tt_length-1.0) ;
            if(l3) return 1 ;  //  Train on turntable
          }
          return -1 ;          //  Train blocking turntable
        }
      }
/*
 * Is the train on a single vector?  If so it does not affect turntable
 */
      if(tr1.itrack == tr2.itrack && tr1.ivector == tr2.ivector) return 0 ;
/*
 *  Check if train crosses turntable by moving tr2 until tr1 reached
 */
      for(;;){
        if(tr2.idirect == train->last->train_dir){
          iret = trv_next(&tr2) ;
        }else{
          iret = trv_prev(&tr2) ;
        }
        if(iret){
          printf("  Routine %s\n",my_name) ;
          printf("  Error while searching from back to front of train\n") ;
          printf("  Return code from trk_move routine = %i\n",iret) ;
          return 0 ;  //  Assume everything OK.
                      //  Maybe points switched under stationary train
        }

        l2 = (tr2.itrack == tr1.itrack && tr2.ivector == tr1.ivector) ;
        if(l2) return 0 ;   //  tr1 reached, turntable free to turn

        for(i_path = 0; i_path<tt->n_paths; i_path++){
          tt_sect_uid = tt->path[i_path].trk_sector_uid ;
          tt_vect_id  = tt->path[i_path].trk_vector_id  ;
          l2 = ((int) tr2.itrack == tt_sect_uid && (int) tr2.ivector == tt_vect_id) ;
          if(l2) return -1 ;  //  Turntable blocked by train
        }
      }
      return 0;  //  No trains affect turntable
}
