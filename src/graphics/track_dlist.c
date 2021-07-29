/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  track_dlist.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines used to generate OpenGL display lists for the rail track.
 *
 *==============================================================================
 */
int   make_track_display_list(TrkNetNode *section, DynProfile *profile) ;

int   make_track_display_lists(){

int  i ;
int  ip = 0 ;                        // Debug printing = 1
TrkNetNode  *track_node       ;  // Track section needing shape
DynProfile      *profile          ;  // Profile to use
char            my_name[]="make_track_display_lists" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
      profile = zr_rail_profile ;
      if(profile == NULL){
        printf(" ERROR.  Routine %s.  Track profile set to NULL\n",my_name);
        exit(0) ;
      }

      for(i=0;i<track_db.trk_sections_array_size;i++){
        if(ip)printf("    %s :  i = %i\n",my_name,i) ;
        track_node = &(track_db.trk_sections_array[i]) ;
        if(ip)printf("    %s :  track_node = %p\n",my_name,(void *)track_node) ;
        if(track_node->type_of_node != VECTOR_SECTION) continue ;
        if(ip)printf("    %s :  profile    = %p\n",my_name,(void *)profile) ;
        make_track_display_list(track_node, profile) ;
      }
     return 0 ;
}

/*
 *  Routine to generate display lists from the track profile
 *
 *  To display correctly, when looking at each front face with the
 *  track index increasing in the y-direction, the profile index should
 *  increase in the x-direction.
 */

int   make_track_display_list(TrkNetNode *track_section, DynProfile *track_profile){

int    i, j, k, l0, l1, m, n ;
int    n_track_nodes   ;
int    i_tex_dirn      ;   // = 1 texture direction increases in '1' direction
int    gl_display_list ;
int    ip = 0          ;                // debug printing
double xx0, yy0, xx1, yy1, xxn, yyn,
       x0, y0, z0, xt, yt,
       x, y, z, xn, yn, zn,
       u0, v0, u1, v1, xoff, yoff       ;
double xl = 0.0, yl = 0.0, zl = 0.0 ;   // Keep compiler happy when optimising
char   my_name[] = "make_track_display_list" ;

TrackDistLevel *dist_level   ;
TrackSubObject *sub_object   ;
TrkVectorNode  *tv0          ;
RailProfile    *rail_profile ;
TextureNode    *tnode        ;


      if(track_profile == NULL){
        printf(" ERROR.  Routine %s.  Profile is unset (NULL) \n",my_name);
        exit(0) ;
      }


      ip = (123 == track_section->index_of_node) ;
       ip = 0 ;
      if(ip){
        printf("  Enter routine %s\n",my_name) ;
        printf("  Index of node = %i\n",track_section->index_of_node) ;
      }

      track_section->n_dist_levels = track_profile->n_dist_levels ;
      n_track_nodes          = track_section->length_of_vector ;
/*
 * Loop over distances
 */
      for(i=0;i<track_profile->n_dist_levels;i++){

        dist_level = &(track_profile->dist_level[i])          ;
        track_section->distance[i] = dist_level->distance     ;  //  Copy distances for later use
        if(ip)printf("    Distance level = %i,    distance = %f\n",i,dist_level->distance);
/*
 *  Open Display list for this distance
 */
        gl_display_list = glGenLists((GLsizei) 1)      ;
        track_section->opengl_display_list[i] = gl_display_list ;
        glNewList( gl_display_list, GL_COMPILE) ;          //  New Display List for this distance
        if(ip)printf("       i = %i  gl_display_list = %i\n",i,gl_display_list);
        if(ip)printf("       i = %i  n_track_nodes   = %i\n",i,n_track_nodes);
/*
 *  Loop over sub_objects (Usually 1 for the default track)
 */
        for(j=0;j<dist_level->n_sub_objects;j++){
          sub_object = &(dist_level->sub_object[j]) ;
          tnode = sub_object->texture ;
          glBindTexture(GL_TEXTURE_2D, tnode->gl_tex_ref_no) ;
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
          if(ip)printf("        sub-object = %i,  gl_tex_ref_no = %i, "
                       "basic %i, needed %i, loaded %i, name = %s\n",
      j, tnode->gl_tex_ref_no, tnode->basic, tnode->needed,tnode->loaded,tnode->name) ;
/*
 *   Loop over profiles  (Left track, right track, ...)
 */
          for(k=0;k<sub_object->n_profiles;k++){
            rail_profile = &(sub_object->profile[k]) ;
/*
 *  Loop over elements in profile
 */
            for(l0=0,l1=1;l0<rail_profile->n_elements-1;l0++,l1++){
              glBegin(GL_TRIANGLE_STRIP);
//              glBegin(GL_LINE_STRIP);
              xx0 = rail_profile->vertex[l0].X ;        //  Dist to right of track
              yy0 = rail_profile->vertex[l0].Y ;        //  Height
              xx1 = rail_profile->vertex[l1].X ;
              yy1 = rail_profile->vertex[l1].Y ;
              xxn = rail_profile->normal[l0].X ;        // +1 if normal to right
              yyn = rail_profile->normal[l0].Y ;        // +1 if normal upwards
              u0 = rail_profile->uvvector[0].U ;
              v0 = rail_profile->uvvector[0].V ;
              u1 = rail_profile->uvvector[1].U ;
              v1 = rail_profile->uvvector[1].V ;
              i_tex_dirn = (u0 == v0) ;
/*
 * Loop over track nodes
 */

              for(m=0;m<n_track_nodes;m++){
                tv0 = &(track_section->vector[m]) ;
                xoff = (tv0->tile_east_x  - tile_x0 + 0.5)*tile_size ;
                yoff = (tv0->tile_north_z - tile_y0 + 0.5)*tile_size ;
                if(ip)printf("  Posn: i = %i m = %i,  n_sub_nodes = %i \n",i,m,tv0->n_sub_nodes)  ;
                for(n=0;n<tv0->n_sub_nodes;n++){
                  x0 = tv0->track_centre[n].X + xoff  ;
                  y0 = tv0->track_centre[n].Y + yoff  ;
                  z0 = tv0->track_centre[n].Z - tile_h0 + 0.0  ; //  At present 0.0 m offset !!!
                  xt = tv0->tangent_vector[n].X ;
                  yt = tv0->tangent_vector[n].Y ;


                  if(n>0||m>0){
                    if(!i_tex_dirn){
                      u0 = u0 + sqrt((x0-xl)*(x0-xl) + (y0-yl)*(y0-yl) + (z0-zl)*(z0-zl))*0.1 ;
                      u1 = u0 ;
                    }else{
                      v0 = v0 + sqrt((x0-xl)*(x0-xl) + (y0-yl)*(y0-yl) + (z0-zl)*(z0-zl))*0.1 ;
                      v1 = v0 ;
                    }
                  }
                  xl = x0; yl = y0; zl = z0 ;

//                  zt = tv0->tangent_vector[n].Z ;

                  xn =  xxn*yt     ;
                  yn = -xxn*xt     ;
                  x  = x0 + yt*xx0 ;
                  y  = y0 - xt*xx0 ;
                  z  = z0 + yy0    ;
                  x = x/plot_scale ; y = y/plot_scale ; z = (0.01 +z)/plot_scale ;
                  zn = yyn         ;
                  if(ip)printf("  Posn AA : i = %i, m = %i, n = %i :: %f %f %f  :: %f %f :: %f %f :: %f %f %f %f :: %f %f\n",i,m,n, x, y, z,  xx0, yy0,  xt, yt,  xxn, yyn, xn, yn, u0, v0) ;

                  glTexCoord2f((GLfloat)u0,(GLfloat)v0)    ;
                  glNormal3f((GLfloat)xn,(GLfloat)yn,(GLfloat)zn) ;
                  glVertex3f((GLfloat)x, (GLfloat)y,(GLfloat) z)  ;

                  x  = x0 + yt*xx1     ;
                  y  = y0 - xt*xx1     ;
                  z  = z0 + yy1        ;
                  x = x/plot_scale ; y = y/plot_scale ; z = (0.01+ z)/plot_scale ;
                  if(ip)printf("  Posn BB : i = %i, m = %i, n = %i :: %f %f %f  :: %f %f  :: %f %f\n",i,m,n, x, y, z, xx1, yy1, u1, v1) ;
                  glTexCoord2f((GLfloat)u1,(GLfloat)v1)    ;
                 glVertex3f((GLfloat)x, (GLfloat)y, (GLfloat)z)  ;
                }
              }
              glEnd() ;
            }
          }
        }
        glEndList() ;
      }
      return 0 ;
}

int  add_texture_pointers_to_track_profiles(DynProfile *dnode){
  int  i, j    ;
  int  ip = 0  ;   // 0 = no printing, 1 = error printing only
  TrackDistLevel   *dist_level   ;
  TrackSubObject   *sub_object   ;
  TextureNode      *tnode        ;
  char             *name = NULL ;
  char             my_name[] = "add_texture_pointers_to_track_profiles" ;

      if(dnode == NULL){
        printf(" ERROR.  Routine %s.  Track profile set to NULL\n",my_name);
        exit(0) ;
      }
      if(ip>1){
        printf(" Enter routine %s\n",my_name) ;
        printf("   Finding textures for track profile %s\n",dnode->name) ;
      }
/*
 * Loop through distances
 */
      for(i=0;i<dnode->n_dist_levels;i++){
        dist_level = &(dnode->dist_level[i]) ;
/*
 *  Loop through distances
 */
        for(j=0;j<dist_level->n_sub_objects;j++){
          sub_object = &(dist_level->sub_object[j]) ;
          sub_object->texture   = NULL ;
          free(name) ;
          name = strdup(sub_object->texture_name) ;
          name[strlen(name)-4]= '\0' ;
          if(ip>1)printf("  Texture required = %s\n",name) ;
/*
*  Loop through names : texture names do not have final '.ace.
*  Compare lower case versions of names (Microsoft feature)
*/
          for(tnode=texturelist_beg; tnode!=NULL; tnode=tnode->next){
            if(strcmp_ic(name,tnode->name))continue ;
            sub_object->texture = tnode     ;          // save link
            tnode->needed       = 1         ;
            tnode->basic        = 1         ;
            if(ip>1)printf("  Texture found    = %s\n",tnode->name) ;
            break ;
          }
          if(ip && sub_object->texture == NULL)
              printf("  Track Profile %s,   texture %s not found\n",
                                         dnode->name,sub_object->texture_name) ;
        }
      }
      if(NULL != name)free(name) ;
      return 0;
}




















