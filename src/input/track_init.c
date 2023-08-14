/*
 * *****************************************************************************
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File : track_init.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Subroutines to initialise the track items - this includes determining
 *   the item position in terms of track section vector and distance
 *   along the vector.
 *
 * *****************************************************************************
 */

/*
 *   Routine to determine position of track items
 */



int find_track_item_positions(){

uint        i, j, k, ip = 0 ;
uint        trk_item_tile_e, trk_item_tile_n ;   //  Should these be int ????
uint        trk_vect_tile_e, trk_vect_tile_n ;
double      trk_item_e, trk_item_n ;
double      trk_vect_e, trk_vect_n, trk_vect_len, dist ;


TrkItem   *trk_item ;
TrkSector   *trk_sect ;
TrkVectorNode *trk_vect ;
char          *my_name = "find_track_item_positions" ;

      if(ip)printf(" Enter %s\n",my_name) ;
/*
 *   Process rail items
 */
      for(i=0;i<track_db.trk_items_array_size;i++){
        trk_item = &track_db.trk_items_array[i] ;
        trk_item_tile_e = trk_item->tile_east_x ;
        trk_item_tile_n = trk_item->tile_north_z ;
        trk_item_e = trk_item->east_x ;
        trk_item_n = trk_item->north_z ;
        for(j=0;j<track_db.trk_sections_array_size;j++){
          trk_sect = &track_db.trk_sections_array[j] ;
          if(trk_sect->type_of_node != VECTOR_SECTION)continue ;
          for(k=0;k<trk_sect->length_of_vector;k++){
            trk_vect = &trk_sect->vector[k] ;
            trk_vect_tile_e = trk_vect->tile_east_x  ;
            trk_vect_tile_n = trk_vect->tile_north_z ;
            trk_vect_e   = trk_vect->east_x  ;
            trk_vect_n   = trk_vect->north_z ;
            trk_vect_len = trk_vect->length  ;
/*
 *  Cull vectors with origins further away than the length of the vector
 */
            dist = (pow(trk_vect_tile_e - trk_item_tile_e, 2) +
                    pow(trk_vect_tile_n - trk_item_tile_n, 2) ) * pow(tile_size, 2)
                 +  pow(trk_vect_e - trk_item_e, 2) +
                 +  pow(trk_vect_n - trk_item_n, 2)   ;
            if(dist > pow(trk_vect_len, 2)) continue ;
/*
 *  Locate nearest point within vector
 */

          }
        }
      }

      if(ip)printf(" Exit  %s\n",my_name) ;
      return 0 ;
}


int sketch_track_items(){

int      i, j, k, ip = 0   ;
double  scale = 1.0/plot_scale ;
double  x = 0., y=0.0 , z=0.0, dist ;

TrkItem     *trk_item  ;
WorldItem       *w ;
char     string[256] ;
char     *my_name = "sketch_track_items" ;

      if(ip)printf("Enter %s\n",my_name) ;

      glDisable(GL_LIGHTING)   ;
      glDisable(GL_TEXTURE_2D) ;
      glDisable(GL_BLEND)      ;
      glShadeModel(GL_FLAT)     ;
      for(i=0;i<(int)track_db.trk_items_array_size;i++){
        trk_item = &track_db.trk_items_array[i] ;

        global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                trk_item->tile_east_x, trk_item->tile_north_z,
                trk_item->east_x, trk_item->north_z, trk_item->height_y,
                &x, &y, &z );

        dist = pow(x-lookat_eye_x,2) + pow(y-lookat_eye_y,2) ;
        if(dist > 0.04)continue ;

        glColor3f(0.8,1.0,0.8) ;
        glBegin(GL_LINES) ;
          glVertex3d(x, y, z) ;
          z = z + 6.0*scale ;
          glVertex3d(x, y, z) ;
        glEnd() ;
        glColor3f(1.0,1.0,1.0) ;
/*
 *  Print track information
 *  Note 'print_string_in_window'
 *    on entry disables GL_LIGHTING, GL_TEXTURE_2D,  enables GL_BLEND
 *    on exit  enables  GL_LIGHTING, GL_TEXTURE_2D and GL_BLEND
 *
 */
        sprintf(string," TRACK ITEM - %i  %i %s\n",trk_item->uid,
                                                   trk_item->type_of_node,
                                     token_trackdb[trk_item->type_of_node]) ;
        print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string) ;

        sprintf(string," Track ITEM - %i %i :: %f %f %f\n",trk_item->tile_east_x,
                                                       trk_item->tile_north_z,
                                                       trk_item->east_x,
                                                       trk_item->north_z,
                                                       trk_item->height_y) ;
        z = z - 0.5*scale ;
        print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);

        sprintf(string," Track ITEM - %i %i :: %f %f\n",trk_item->p_tile_east_x,
                                                       trk_item->p_tile_north_z,
                                                       trk_item->p_east_x,
                                                       trk_item->p_north_z) ;
        z = z - 0.5*scale ;
        print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);

        sprintf(string," Track ITEM - %f %s\n",trk_item->s_data1,
                                               trk_item->s_data2) ;
        z = z - 0.5*scale ;
        print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);


        if(PLATFORM == trk_item->type_of_node){
          sprintf(string," PLATFORM - %s :: %i :: %s\n",trk_item->platform_data1,
                                                        trk_item->platform_data2,
                                                        trk_item->platform_name) ;
          z = z - 1.0*scale ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
          sprintf(string," STATION  - %s :: %i :: %i\n",trk_item->station,
                                                        trk_item->platform_wait_time,
                                                        trk_item->platform_passengers) ;
          z = z - 1.0*scale ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
        }else if(SIDING == trk_item->type_of_node){
          sprintf(string," SIDING - %s :: %i :: %s\n",trk_item->siding_data1,
                                                      trk_item->siding_data2,
                                                      trk_item->siding_name) ;
          z = z - 1.0*scale ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
        }else if(SIGNALS == trk_item->type_of_node){
          sprintf(string," SIGNALS - %s :: %i :: %f:: %s\n",trk_item->signal_data1,
                                                      trk_item->signal_direction,
                                                      trk_item->signal_data3,
                                                      trk_item->siding_name) ;
          z = z + (4.0 + 3*(trk_item->signal->uid%3))*scale ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
          sprintf(string," SIGNALS - %i :: %i %i %i %i :: %i %i %i %i :: %i %i %i %i :: %i %i %i %i\n",
                 trk_item->signal_num_dirs,
                 trk_item->signal_dirs[0][0],trk_item->signal_dirs[0][1],
                 trk_item->signal_dirs[0][2],trk_item->signal_dirs[0][3],
                 trk_item->signal_dirs[1][0],trk_item->signal_dirs[1][1],
                 trk_item->signal_dirs[1][2],trk_item->signal_dirs[1][3],
                 trk_item->signal_dirs[2][0],trk_item->signal_dirs[2][1],
                 trk_item->signal_dirs[2][2],trk_item->signal_dirs[2][3],
                 trk_item->signal_dirs[3][0],trk_item->signal_dirs[3][1],
                 trk_item->signal_dirs[3][2],trk_item->signal_dirs[3][3]) ;
          z = z - 0.5*scale ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
          sprintf(string," SIGNALS - s = %i, w = %i :: %s %s %s\n",
                 trk_item->signal->uid, trk_item->signal->witem->uid,
                 trk_item->signal->shape_name, trk_item->signal->type_name,
                 trk_item->signal->world_name) ;
          z = z - 0.5*scale ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);

        }
#if 0
          sprintf(string," - TILE %i %i ::XYZ %10.3f  %10.3f  %10.3f\n",
                  vec[j].tile_east_x,vec[j].tile_north_z,
                  vec[j].east_x,vec[j].north_z,vec[j].height_y);
          z = z - 1.0*scale ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);

          if(w->worldtype==306){
            for(k=0;k<5;k++){
              d = &w->u.dyn_track_obj.dyn_trk_sect[k] ;
              if(d->uid==-1)continue ;
              sprintf(string," - %i %i  (%i %.2f %.1f)",v->flag1,v->flag2,d->is_curved, d->param_1,d->param_2);
              z = z - 1.0*scale ;
              glColor3f((GLfloat)v->flag1,0.0,(GLfloat)v->flag2) ;
              print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
              glColor3f(1.0,1.0,1.0) ;
            }
          }else{
              glColor3f(1.0,1.0,1.0) ;
              sprintf(string," - World Type = %i %s",w->worldtype,token_idc[w->worldtype]);
              z = z - 1.0*scale ;
              print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
              glColor3f(1.0,1.0,1.0) ;
          }
#endif
        glDisable(GL_LIGHTING)   ;  //Correct for print_string_in_window
        glDisable(GL_TEXTURE_2D) ;
        glDisable(GL_BLEND)      ;
      }
      glEnable(GL_LIGHTING)   ;
      glEnable(GL_TEXTURE_2D) ;
      glDisable(GL_BLEND) ;

      if(ip)printf("Exit  %s\n",my_name) ;

      return 0 ;
}
