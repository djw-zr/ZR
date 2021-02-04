/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  dynamic_dlist.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Contains routines to generate a display list for the dynamic sections of
 *   track.
 *
 *==============================================================================
 */

static Vector3x3 *make_vector3x3(void) ;

static double     vxx, vyy, vzz ;
static double     vtx, vty, vtz ;
static double     vnx, vny, vnz ;                //  mathcalls.h has a yn !!

int make_dynamic_display_lists(){

int    i, j, k, l0, l1, m, n ;
int    n_track_nodes   ;
int    i_tex_dirn      ;   // = 1 texture direction increases in '1' direction
int    iflag           ;
int    gl_display_list ;
int    ip = 0          ;                // debug printing
double xx0, yy0, xx1, yy1, xxn, yyn,
       x0, y0, z0,
       x, y, z, xt, yt, zt, xn, yn, zn,
       u0, v0, u1, v1, xoff, yoff,
       delta_a, aa, asign, tmp     ;
double xl = 0.0, yl = 0.0, zl = 0.0 ;   // Keep compiler happy when optimising
int    tile_x,  tile_y ;

WorldNode      *wnode ;
WorldItem      *witem ;
DynTrackObj    *ditem ;
DynTrackSect   *dsect ;
DynProfile     *track_profile ;
RailProfile    *rail_profile  ;
TrackDistLevel *dist_level    ;
TrackSubObject *sub_object    ;
TrkVectorNode  *tv0   ;
TextureNode    *tnode ;
Vector3x3      *v3x3  ;
Vector3x3      *v3x3_beg = NULL ;
Vector3x3      *v3x3_end = NULL ;

double        rx, ry, rz ;
double        p1, p2, p3 ;

char   my_name[] = "make_dynamic_display_lists" ;


      if(ip)printf(" Enter routine %s\n",my_name) ;
      track_profile = zr_rail_profile ;
      if(track_profile == NULL){
        printf(" ERROR. Routine %s.  On entry profile set to NULL\n",my_name) ;
        exit(0) ;
      }

      for(wnode=worldlist_beg; wnode != NULL; wnode=wnode->next){
        tile_x = wnode->tile_x ;
        tile_y = wnode->tile_y ;
//  Switch on debug pringing for a particular tile
//        ip = ((tile_x == 1449) && (tile_y = 10331)) ;
        if(ip)printf("  ++ World tile node = %p\n",(void *)wnode) ;
        if(ip)printf("  ++ World tile %i  %i\n",tile_x, tile_y) ;
        for(witem = wnode->world_item ; witem != NULL; witem = witem->next){
          if(witem->worldtype != DYNTRACK && witem->worldtype != 306)continue ;
//  Switch on debug pringing for a particular tile and uid
//          ip = ip && (witem->uid == 225) ;
          if(ip)printf("  World tile node = %p\n",(void *)wnode) ;
          if(ip)printf("  World tile %i  %i\n",tile_x, tile_y) ;
          fflush(NULL) ;
          if(ip)printf("  World item node = %p\n",(void *)witem) ;
          if(ip)printf("  World item:  %i %i :: %.3f   %.3f   %.3f\n",
            witem->uid, witem->worldtype, witem->X, witem->Y, witem->Z) ;
          fflush(NULL) ;
/*
 *  1.  Create a series of 3x3 vectors specifying the centre of the track,
 *      the unit tangent and the unit normal
 */
          vxx = 0.0 ; vyy = 0.0 ; vzz = 0.0 ;    //  Centre of track
          vtx = 1.0 ; vty = 0.0 ; vtz = 0.0 ;    //  Tangent
          vnx = 0.0 ; vny = 0.0 ; vnz = 1.0 ;    //  Normal
          v3x3 = make_vector3x3() ;
          if(v3x3_beg == NULL) v3x3_beg       = v3x3 ;
          else                 v3x3_end->next = v3x3 ;
          v3x3_end = v3x3 ;
          if(ip)printf("  v3x3     = %p\n",(void *)v3x3 );
          if(ip)printf("  v3x3_beg = %p\n",(void *)v3x3_beg );
          if(ip)printf("  v3x3_end = %p\n",(void *)v3x3_end );
/*
 *  Loop over the dynamic track sectors
 */
          ditem = &(witem->u.dyn_track_obj) ;
          if(0)printf(" AA %p\n",(void *)ditem);
          for(i=0;i<5;i++){
            if(0)printf(" BB\n");
            dsect = &(ditem->dyn_trk_sect[i]) ;
            if(0)printf(" BB %p\n",(void *)dsect);
            p1 = dsect->param_1 ;                    //  Distance or Angle
            p2 = dsect->param_2 ;                    //  Zero     or Radius
            p3 = dsect->delta_y ;
            if(ip)printf(" BB i = %i, params  = %.3f %.3f %.3f\n",i, p1,p2,p3);
            if(p1 == 0.0 && p2 == 0.0)continue ;
// Straight section   p1 = length (m)  p2 = zero
            if(p2 == 0){
             if(ip)printf(" CC\n");
              vxx = vxx + p1*vtx ;
              vyy = vyy + p1*vty ;
              v3x3->next = make_vector3x3() ;
              v3x3       = v3x3->next ;
// Curved section   p1 = angle (radians),  p2 = radius (m).
            }else{
              aa = fabs(p1) ;
              asign = (p1 >= 0) ? 1.0 : -1.0 ;
              if(track_profile->pitch_control == CHORDLENGTH){
                delta_a = track_profile->pitch_max/p2  ;          // p2 = radius
              }else{
                delta_a = 2.0*sqrt(track_profile->pitch_max/p2) ;
              }
              if(ip)printf(" pitch_max = = %10.4f\n",track_profile->pitch_max);
              if(ip)printf(" BBB p1, p2, aa, asign = %.3f %.3f %.3f %.3f\n",p1,p2,aa,asign);
              n = ceil(aa/delta_a)  ;
              delta_a = aa/n        ;
              if(ip)printf(" FF n = %i,  delta_a = %.3f\n",n,delta_a);

              if(p1>0){
                rx = vxx + vty*p2 ;
                ry = vyy - vtx*p2 ;
              }else{
                rx = vxx - vty*p2 ;
                ry = vyy + vtx*p2 ;
              }

              for(j=0, aa=delta_a; j<n; j++, aa+=delta_a){
                if(p1 > 0){
                  vxx = rx + p2*sin(aa) ;  //  Center line
                  vyy = ry + p2*cos(aa) ;
                  vtx = cos(aa) ;                                      //  Tangent
                  vty =-sin(aa) ;
                }else{
                  vxx = rx + p2*sin(aa) ;  //  Center line
                  vyy = ry - p2*cos(aa) ;
                  vtx =  cos(aa) ;                    //  Tangent
                  vty =  sin(aa) ;
                }
                v3x3->next = make_vector3x3() ;
                v3x3       = v3x3->next ;
                if(ip)printf(" GG  vxx, vyy = %9.3f  %9.3f :: vtx, vty =  %9.3f  %9.3f\n",
                                       vxx,vyy,vtx,vty);
              }
            }
            v3x3_end = v3x3 ;
            if(ip)printf(" CC2 %p %p\n",(void *)v3x3,(void *)v3x3_end );
          }
/*
 *   Create display list for distance levels
 */
          for(i=0;i<3;i++){
            dist_level = &(track_profile->dist_level[i])     ;
//          track_section->distance[i] = dist_level->distance ;  //  Copy distances for later use
            gl_display_list = glGenLists((GLsizei) 1)        ;
            glNewList( gl_display_list, GL_COMPILE)          ;
            ditem->gl_display_list[i] = gl_display_list      ;
            ditem->dist_level[i]      = dist_level->distance ;
            if(ip)printf("\n JJ  %i  ::  %.3f  %i\n",i,ditem->dist_level[i],
              ditem->gl_display_list[i]) ;
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
                          j, tnode->gl_tex_ref_no, tnode->basic, tnode->needed,
                                                   tnode->loaded, tnode->name) ;
/*
 *   Loop over profiles  (Left track, right track, ...)
 */
              for(k=0;k<sub_object->n_profiles;k++){
                rail_profile = &(sub_object->profile[k]) ;
/*
 *  Loop over elements in profile
 */
                if(ip)printf("  PROFILE :: nelements = %i\n",rail_profile->n_elements) ;
                for(l0=0,l1=1;l0<rail_profile->n_elements-1;l0++,l1++){
                  glBegin(GL_TRIANGLE_STRIP);
//                glBegin(GL_LINE_STRIP);
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
                  if(ip)printf(" Profile :  xx0, yy0 = %10.4f  %10.4f  ::   xx1, yy1 = %10.4f  %10.4f\n",xx0,yy0,xx1,yy1);
                  if(ip)printf(" Profile :  xxn, yyn = %10.4f  %10.4f\n",xxn,yyn);
                  if(ip)printf(" Profile :  u0,  v0  = %10.4f  %10.4f  ::   u1,  v1  = %10.4f  %10.4f\n",u0,v0,u1,v1);
/*
 * Loop over track nodes
 */
                  iflag = 0 ;
                  for(v3x3=v3x3_beg; v3x3!=NULL; v3x3=v3x3->next){
                    x0 = v3x3->vxx ;
                    y0 = v3x3->vyy ;
                    z0 = v3x3->vzz ;
                    xt = v3x3->vtx ;
                    yt = v3x3->vty ;
                    if(ip)printf(" x0 = %10.4f, y0= %10.4f, z0 = %10.4f, xt = %10.4f, yt = %10.4f\n",
                                 (double)x0, (double)y0, (double)z0, (double)xt, (double)yt) ;
//  Move texture origin after first pair of vertices
                    if(iflag){
                      if(!i_tex_dirn){
                        u0 = u0 + sqrt((x0-xl)*(x0-xl) + (y0-yl)*(y0-yl) + (z0-zl)*(z0-zl))*0.1 ;
                        u1 = u0 ;
                      }else{
                        v0 = v0 + sqrt((x0-xl)*(x0-xl) + (y0-yl)*(y0-yl) + (z0-zl)*(z0-zl))*0.1 ;
                        v1 = v0 ;
                      }
                    }
                    xl = x0; yl = y0; zl = z0 ;
                    iflag = 1;

  //                  zt = tv0->tangent_vector[n].Z ;

                    xn =  xxn*yt     ;
                    yn = -xxn*xt     ;
                    x  = x0 + yt*xx0 ;
                    y  = y0 - xt*xx0 ;
                    z  = z0 + yy0    ;
                    if(ip)printf(" x  = %10.4f, y  = %10.4f, z  = %10.4f, xt = %10.4f, yt = %10.4f\n",
                                 (double)x , (double)y , (double)z , (double)xt, (double)yt) ;
//                    x = x/plot_scale ; y = y/plot_scale ; z = (0.01 +z)/plot_scale ;
                    zn = yyn         ;
                    if(ip)printf(" x  = %10.4f, y  = %10.4f, z  = %10.4f, xt = %10.4f, yt = %10.4f\n",
                                 (double)x , (double)y , (double)z , (double)xt, (double)yt) ;
                    if(ip)printf("  Posn AA : i = %i %.3f  %.3f  %.3f  :: %.3f  %.3f  :: %.3f  %.3f  %.3f  :: %.3f  %.3f  :: %.3f  %.3f  :: %.3f  %.3f  :: %.3f  %.3f \n",i, x0, y0, z0,  xt, yt,   x, y, z,  xx0, yy0,  xxn, yyn, xn, yn, u0, v0) ;

                    if(ip)printf("  Posn BBN : i = %i  :: %.5f  %.5f  %.5f\n",i, xn, yn, zn) ;
                    if(ip)printf("  Posn BB1 : i = %i  :: %.5f  %.5f  %.5f   :: %.5f  %.5f \n",
                                                                         i,x, y, z, u0, v0) ;
                    glTexCoord2f((GLfloat)u0,(GLfloat)v0)    ;
                    glNormal3f((GLfloat)xn,(GLfloat)yn,(GLfloat)zn) ;
//                    glNormal3f((GLfloat)0.0,(GLfloat)0.0,(GLfloat)-1.0) ;
                    glVertex3f((GLfloat)x, (GLfloat)y,(GLfloat) z)  ;

                    x  = x0 + yt*xx1     ;
                    y  = y0 - xt*xx1     ;
                    z  = z0 + yy1        ;
//                    x = x/plot_scale ; y = y/plot_scale ; z = (0.01+ z)/plot_scale ;
                    if(ip)printf("  Posn BB2 : i = %i  :: %.5f  %.5f  %.5f   :: %.5f  %.5f \n",
                                                                         i,x, y, z, u1, v1) ;
                    glTexCoord2f((GLfloat)u1,(GLfloat)v1)    ;
                    glVertex3f((GLfloat)x, (GLfloat)y, (GLfloat)z)  ;
                  }
                  glEnd() ;
                  if(ip)printf("===============================================\n\n");
                }
              }
            }
            glEndList() ;
          }
/*
 * Free allocated memory
 */
          for(v3x3 = v3x3_beg ; v3x3 != NULL ; v3x3 = v3x3->next)free(v3x3) ;
          v3x3_end = NULL ;
          v3x3_beg = NULL ;
        }
      }
      return 0 ;
}

Vector3x3  *make_vector3x3(void){
Vector3x3  *v3x3 ;

      v3x3 = (Vector3x3 *)malloc(sizeof(Vector3x3)) ;
      v3x3->next = NULL ;
      v3x3->vxx = vxx ; v3x3->vyy = vyy ; v3x3->vzz = vzz ;
      v3x3->vtx = vtx ; v3x3->vty = vty ; v3x3->vtz = vtz ;
      v3x3->vnx = vnx ; v3x3->vny = vny ; v3x3->vnz = vnz ;
      return v3x3 ;
}


