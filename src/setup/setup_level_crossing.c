 /*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  setup_level_crossing.c
 *
 *   This file contains routines used to initialise the level crossing
 *   summary structures.  These are designed to simplify the processing
 *   of level crossings by providing a single list of level crossings
 *   in each world tile, and to simplify the calculation of crossings that
 *   should be open by converting the (x,y,x) track item position to a track
 *   section number an a distance along the track.
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *==============================================================================
 */

int init_levelxs_node(void){

  WorldNode  *wnode ;
  WorldItem  *witem ;
  LevelXSNode *snode ;

  int  wtype ;
  int  i, n, m, n_rail, n_road, i_rail, i_road ;
  int  ip = 1 ;
  char *my_name = "init_levelxs_node" ;


      if(ip)printf("   Enter routine: %s\n",my_name) ;
/*
 *  Loop over tiles
 */
      for(wnode=worldlist_beg ; wnode != NULL; wnode = wnode->next){
        n = 0 ;
/*
 *  Find number of level crossings on the tile
 */
        for(witem= wnode->world_item; witem != NULL ; witem = witem->next){
          if(LEVELCR == witem->worldtype) n++ ;
        }
        wnode->n_level_XS = n ;
        wnode->level_XS   = NULL   ;
        if(!n)continue ;                     /* Skip if no level crossings */
/*
 *  Allocate an array of 'LevelXSNode' structures
 *                      - one for each level crossing on tile
 */
        snode =  (LevelXSNode *)malloc(n*sizeof(LevelXSNode))   ;
        wnode->level_XS = snode ;
/*
 *  Loop over level crossings again
 *  - this time save the data in the LevelXSNode structure
 */
        for(witem= wnode->world_item; witem != NULL ; witem = witem->next){
          if(LEVELCR != witem->worldtype) continue ;
          m = 1 ;
          snode->n_world = m ;
/*
 *  Allocate array of pointers to the (single) world item
 */
          snode->world_item = (WorldItem **)malloc(m*sizeof(WorldItem *)) ;

//          printf("  n, m = %i %i\n",n,m);
//          printf("  snode->world_item    = %p\n",(void *)snode->world_item);
/*  Add the pointer */
          snode->world_item[0] = witem ;
//          printf("  snode->world_item    = %p\n",(void *)snode->world_item);
//          printf("  snode->world_item[0] = %p\n",(void *)snode->world_item[0]);
          n_road = n_rail = 0 ;
/* Find number of rail and road track items */
          for(i=0;i<witem->n_tr_item;i++){
            if(0 == witem->tr_item_db[i]) n_rail++ ;
            else if(1 == witem->tr_item_db[i]) n_road++ ;
            else {
              printf("  Routine %s error.  Index out of range\n",my_name) ;
              exit(0) ;
            }
          }
          snode->n_rail = n_rail ;
          snode->n_road = n_road ;
/* Generate arrays for track and road item indices,
                                     section indices and section distances */
          snode->rail_track_item = (uint *)malloc(n_rail*sizeof(uint)) ;
          snode->rail_index      = (uint *)malloc(n_rail*sizeof(uint)) ;
          snode->rail_i_vec      = (uint *)malloc(n_rail*sizeof(uint)) ;
          snode->rail_dist       = (double *)malloc(n_rail*sizeof(double)) ;
          snode->road_track_item = (uint *)malloc(n_road*sizeof(uint)) ;
          snode->road_index      = (uint *)malloc(n_road*sizeof(uint)) ;
          snode->road_i_vec      = (uint *)malloc(n_road*sizeof(uint)) ;
          snode->road_dist       = (double *)malloc(n_road*sizeof(double)) ;
          if(ip){
            printf("  Tile = %i %i\n",wnode->tile_x,wnode->tile_y) ;
            printf("  witem = %i %i %s :: %s\n",witem->uid, witem->worldtype,
            token_idc[witem->worldtype],witem->filename) ;
            printf(" snode->rail_track_item = %p\n",(void *)snode->rail_track_item) ;
            printf(" snode->rail_index      = %p\n",(void *)snode->rail_index) ;
            printf(" snode->rail_i_vec      = %p\n",(void *)snode->rail_i_vec) ;
            printf(" snode->rail_distt      = %p\n",(void *)snode->rail_dist) ;
            printf(" snode->road_track_item = %p\n",(void *)snode->road_track_item) ;
            printf(" snode->road_index      = %p\n",(void *)snode->road_index)  ;
            printf(" snode->road_i_vec      = %p\n",(void *)snode->road_i_vec) ;
            printf(" snode->road_dist       = %p\n",(void *)snode->road_dist) ;
          }
/* Add track item indices to lists */
          i_rail = i_road = 0 ;
          for(i=0;i<witem->n_tr_item;i++){
            if(0 == witem->tr_item_db[i]){
               snode->rail_track_item[i_rail] = witem->tr_item_db_id[i] ;
               if(ip)printf(" i = %i, i_rail = %i, snode->rail_track_item[] = %i\n",
                      i,i_rail,snode->rail_track_item[i_rail] ) ;
               i_rail++ ;
            }else if(1 == witem->tr_item_db[i]){
               snode->road_track_item[i_road] = witem->tr_item_db_id[i] ;
               if(ip)printf(" i = %i, i_road = %i, snode->road_track_item[] = %i\n",
                      i,i_road,snode->road_track_item[i_road] ) ;
               i_road++ ;
            }
          }
          snode++ ;   //  End of initialising summary node, move pointer on
        }             //  End of loop over world items in tile
/*
 *  Print summary of 'LevelXSNode' snode
 */
      }     //  End of loop over tiles
      return 0;
}

/*
 *  Routine find_track_item_posn
 *
 *  Given a road or rail track database and index of a track item it finds the
 *  track vector passing closest to the track item and the position of the
 *  closest point along the section.
 *
 *  Note 'track vector' here means a single piece of straight track or
 *       a piece of curved track with constant curvature.
 */

int find_track_item_posn(TrkDataBase *database, int id,
          uint *section_index, uint *vector_index, double *distance){

  uint  i, j ;
  int   ip = 0 ;
  int   found = 0 ;
  int   trk_item_vector_index ;
  uint  length_of_vector ;

  TrkItemNode   *item ;
  TrkSectNode   *sect ;
  TrkVectorNode *vect ;

  double ti_x, ti_y, ti_z ;    //  Track item position
  double tv_x, tv_y, tv_z ;    //  Track vector origin
  double o_x,  o_y,  o_z  ;    //  Centre of track arc
  double e_x,  e_y,  e_z  ;    //  End point of track vector
  double c_x,  c_y,  c_z  ;    //  Closest track point to track item
  double aa,   bb,   cc   ;    //  Rotation angles

  double nx, ny, nz ;          //  Normal to track
  double tx, ty, tz ;          //  Track tangent at start point
  double rx, ry, rz ;          //  Third axis - radial direction
  double vx, vy, vz ;          //  Unit vector from arc centre to track item

  double track_length,         //  Length of track
         track_distance,       //  Track distance to closest point
         track_radius,         //  Radius of curved track
         alpha,                //  Solution point angle or distance
         last_error = 1000.0,  //  Previous smallest error
         error      = 1001.0,  //  Error at closest point
         sa,                   //  Sign of Curvature
         dott, dotr, temp, s, c  ;

  char   *my_name = "find_track_item_posn" ;

      if(ip)printf("   Enter routine: %s\n",my_name) ;
      if(ip)printf("   id = %i\n",id) ;
/*
 *  Search for track section and section vector
 *                           containing track item 'id'
 */
      *section_index = 0 ;
      *vector_index  = 0 ;
      for(i=0; i<database->trk_sections_array_size; i++){
        sect = &(database->trk_sections_array[i]) ;
        if(VECTOR_SECTION != sect->type_of_node)continue ;
        for(j=0;j<sect->trk_item_number;j++){
          if(sect->trk_item_vec[j] == (uint)id){
            *section_index = sect->index_of_node ;   // = i + 1 !!
            trk_item_vector_index = j ;
            found = 1 ;
            break ;              // Exit inner loop
          }
        }
        if(found)break ;         // Exit outer loop
      }
      if(!found){
        printf("   No track section found containing track item :: %i\n",id);
        return 1 ;
      }
      if(ip)printf("   Track section found containing track item %i :: %i %i\n",
                           id,*section_index,trk_item_vector_index);
/*
 *  Find distance along vector *i_vec of track section *index
 *                        to the point closest to the track item 'id'
 */
      item = &(database->trk_items_array[id]) ;
/*
 *  Calculate location of track item - metres relative to tile origin
 */
      global2local(tile_x0, tile_y0, tile_h0, tile_size, 1.0,
                         item->tile_east_x, item->tile_north_z,
                         item->east_x, item->north_z, item->height_y,
                         &ti_x, &ti_y, &ti_z) ;
      if(ip){
        printf("   Tile origin at: %i %i %f\n",tile_x0, tile_y0, tile_h0) ;
        printf("   Track Item at:  %f %f %f\n",ti_x, ti_y, ti_z) ;
      }
/*
 *  Loop over sections
 *  NOTE:  Track section with index "n" is specified in
 *         trk_sections_array[n-1];
 */

      sect = &(database->trk_sections_array[*section_index-1]) ;
      length_of_vector = sect->length_of_vector ;
      vect = sect->vector ;
      found = 0 ;
      for(i=0;i<length_of_vector;i++){

/*
 *  Calculate origin of vector
 */
        global2local(tile_x0, tile_y0, tile_h0, tile_size, 1.0,
                         vect->tile_east_x, vect->tile_north_z,
                         vect->east_x, vect->north_z, vect->height_y,
                         &tv_x, &tv_y, &tv_z) ;
        if(ip)printf("    Vector origin at:  %f %f %f :: %i :: %f %f %f :: %f\n",
                                   tv_x, tv_y, tv_z,vect->is_curved,
                                   vect->ax,vect->ay,vect->az,vect->ang);

        track_length = vect->length ;
        aa = degree*vect->a_east_x ;
        bb = degree*vect->a_height_y ;
        cc = degree*vect->a_north_z ;
/*
 * Calculate unit normal and unit tangent
 */
        mstswagon2local(0., 1., 0., 0., 0., 0., aa, bb, cc, 1.,
                                    0., 0., 0., &nx, &ny, &nz) ;
        mstswagon2local(0., 0., 1., 0., 0., 0., aa, bb, cc, 1.,
                                    0., 0., 0., &tx, &ty, &tz) ;
/*
 *  Straight section
 */
        if(!(vect->is_curved)){
          alpha = (ti_x-tv_x)*tx + (ti_y-tv_y)*ty + (ti_z-tv_z)*tz ;
          if((alpha>=0.0 && alpha<=track_length) || ip){

            c_x = tx*alpha  + tv_x ;
            c_y = ty*alpha  + tv_y ;
            c_z = tz*alpha  + tv_z ;

            error = sqrt( (ti_x-c_x)*(ti_x-c_x) + (ti_y-c_y)*(ti_y-c_y)
                                                + (ti_z-c_z)*(ti_z-c_z) ) ;
            if(ip){
              e_x = tx*track_length + tv_x ;
              e_y = ty*track_length + tv_y ;
              e_z = tz*track_length + tv_z ;

              printf("    Straight track\n") ;
              printf("    Length, Rotate  :  %f %f\n", track_length, vect->ang) ;
              printf("    Normal          :  %f %f %f\n",nx, ny, nz) ;
              printf("    Tangent         :  %f %f %f\n",tx, ty, tz) ;
              printf("    Closest at      :  %f %f %f :: %f %f\n",
                                                c_x, c_y, c_z, alpha, error);
              printf("    Vector end at   :  %f %f %f :: %f\n",
                                                e_x, e_y, e_z, track_length);
            }
          }
/*
 *  Curved Section
 */
        }else{
          track_radius = vect->radius ;
/*
 *  Calculate bi-normal - unit vector to right of track
 *                        = unit tangent vector X unit normal vector
 */

          mstswagon2local(1., 0., 0., 0., 0., 0., aa, bb, cc, 1.,
                                    0., 0., 0., &rx, &ry, &rz) ;

          sa = (vect->angle > 0.0) ? 1.0 : -1.0 ;
          o_x = tv_x + sa*rx*track_radius ;   //  Centre of arc
          o_y = tv_y + sa*ry*track_radius ;
          o_z = tv_z + sa*rz*track_radius ;

          temp = nx*ti_x + ny*ti_y + nz*ti_z ;
          vx = ti_x - temp*nx - o_x ;
          vy = ti_y - temp*ny - o_y ;
          vz = ti_z - temp*nz - o_z ;
          temp = 1.0/sqrt(vx*vx + vy*vy + vz*vz) ;
          vx = vx*temp ;             //  Unit vector from centre to track item
          vy = vy*temp ;
          vz = vz*temp ;

          dott = vx*tx + vy*ty + vz*tz ;
          dotr = vx*rx + vy*ry + vz*rz ;

          alpha = atan2(dott,dotr)    ;  //  Angle (radian) between vx and rx
          if(alpha > 0.5*pi)alpha = pi - alpha ;
          alpha = alpha*sa ;
          alpha = track_radius*alpha  ;  //  Distance along track
          if((alpha>=0.0 && alpha<=track_length) || ip){

            c_x = o_x + track_radius*vx ;
            c_y = o_y + track_radius*vy ;
            c_z = o_z + track_radius*vz ;

            error = sqrt( (ti_x-c_x)*(ti_x-c_x) + (ti_y-c_y)*(ti_y-c_y)
                                                + (ti_z-c_z)*(ti_z-c_z) ) ;
            if(ip){
              s = sin(vect->angle) ;
              c = cos(vect->angle) ;
              e_x = o_x - (c*rx - s*tx)*sa*track_radius ; // End position
              e_y = o_y - (c*ry - s*ty)*sa*track_radius ;
              e_z = o_z - (c*rz - s*tz)*sa*track_radius ;

              printf("=+==Curved track\n") ;
              printf("    Length, Rotate  :  %f %f\n", track_length, vect->ang) ;
              printf("    Radius, Angle   :  %f %f :: %f %f %f\n",
                                              vect->radius, vect->angle, c, s, sa) ;
              printf("    Normal          :  %f %f %f\n",nx, ny, nz) ;
              printf("    Tangent         :  %f %f %f\n",tx, ty, tz) ;
              printf("    Bi-Normal       :  %f %f %f\n",rx, ry, rz) ;
              printf("    Centre of Arc   :  %f %f %f\n",o_x,  o_y,  o_z)  ;
              printf("    Track item      :  %f %f %f\n",ti_x, ti_y, ti_z) ;
              printf("    Centre to t item:  %f %f %f\n",vx, vy, vz) ;
              printf("    dott, dotr, atan:  %f %f %f\n",dott, dotr, atan2(dott,dotr)) ;
              printf("    Closest at      :  %f %f %f :: %f %f\n",
                                                c_x, c_y, c_z, alpha, error);
              printf("    Vector end at   :  %f %f %f :: %f\n",
                                                e_x, e_y, e_z, track_length);
            }
          }
        }
/*
 *  Save best match
 */
        if(ip)printf("    Update          :  %f %f :: %f %f :: %i\n",
                         alpha, track_length, error, last_error, found) ;
        if(alpha>=0.0 && alpha<=track_length){
          if(error<last_error){
            *vector_index = i  ;
            *distance  = alpha ;
            last_error = error ;
            found = 1          ;
          }
        }
        vect++ ;       //  Move to next vector
      }
      if(!found){
        printf("  Routine %s error:",my_name) ;
        printf("  Unable to find vector containing track item\n") ;
      }
      if(ip)printf("   Track Item %i found at section %i, vector %i\n",
                           id,*section_index,*vector_index);

      return 0 ;
}

/*
 *  Program to set up level crossing summary data ready for efficient handling
 *  of level crossings in main program loop
 */

int setup_level_crossings(void){

  WorldNode   *wnode ;
  LevelXSNode *snode ;
  int          i, j ;
  int          rail_track_item ;

  int        ip = 0 ;
  char       *my_name = "setup_level_crossings" ;


      if(ip)printf("   Enter routine: %s\n",my_name) ;
      init_levelxs_node() ;
      if(ip)printf("   Routine: %s :: Loop over level crossings\n",my_name) ;

/*  Loop over world tiles and level crossings */
      for(wnode=worldlist_beg ; wnode != NULL; wnode = wnode->next){
        if(wnode->n_level_XS>0)
        if(ip)printf("  New tile :: %i %i :: n_level_XS = %i\n",
                wnode->tile_x, wnode->tile_y,wnode->n_level_XS) ;
        for(i = 0 ; i < (int) wnode->n_level_XS; i++){
          snode = &(wnode->level_XS[i]) ;
          if(ip){
            printf("  world  = %i  %p\n", snode->n_world,(void *) snode->world_item) ;
            printf("  n_rail = %i  %p\n", snode->n_rail,(void *) snode->rail_track_item) ;
            printf("  n_road = %i  %p\n", snode->n_road,(void *) snode->road_track_item) ;
          }
/*  Loop over rail track items */
          for(j=0;j<(int)snode->n_rail;j++){
            rail_track_item = snode->rail_track_item[j] ;
//      find_track_item_posn()
            find_track_item_posn(&track_db,rail_track_item,
               &(snode->rail_index[j]),&(snode->rail_i_vec[j]),
                                       &(snode->rail_dist[j] ) ) ;
          }
/*  Loop over road track items */

//      find_track_item_posn()
        }
      }
      return 0 ;
}
