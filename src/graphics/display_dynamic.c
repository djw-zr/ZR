/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  display_dynamic.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Contains routines to display dynamic rail tracks
 *
 *==============================================================================
 */

int display_dynamic_tracks(){

int         i  ;
int         ip = 0          ;  // debug printing
int         id = 0          ;  // debug display
int         gl_display_list ;
int         lod_control     ;
float       distance        ;
float       dist_level      ;

int         tx, ty          ;
float       x, y, z         ;
GLfloat     a, b, c, d      ;
GLfloat    scale = 1.0/plot_scale;
WorldNode   *wnode          ;
WorldItem   *witem          ;
DynTrackObj *dyn_track_obj  ;
char        string[256]     ;
char        my_name[] = "display_dynamic_tracks" ;

static int ifirst = 1 ;       //  Only print when routine is first entered

      if(ip)printf("  Enter routine %s\n",my_name) ;
      glEnable(GL_LIGHTING);
      glEnable(GL_TEXTURE_2D) ;
      glEnable(GL_BLEND) ;
      glEnable(GL_CULL_FACE) ;

//  All dynamic track normals should have unit length
      glEnable(GL_RESCALE_NORMAL) ;  // Faster : for use with uniform scaling

//  Specify texture properties
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;

//  Cycle over tiles
      for(wnode=worldlist_beg; wnode != NULL; wnode=wnode->next){
        if(!wnode->in_use) continue ;
        tx = wnode->tile_x ;
        ty = wnode->tile_y ;
        if(ip)printf("  World tile %i  %i\n",tx,ty) ;

//  Cycle over world items
        for(witem = wnode->world_item ; witem != NULL; witem = witem->next){

//          id = (1449 == tx) && (10331 == ty) &&  (225 == witem->uid) ;
          ip = id && (1 == ifirst) ;
          if(ip)printf("  World item %i %i\n",witem->worldtype,DYNTRACK ) ;

          if(witem->worldtype != DYNTRACK && witem->worldtype != 306)continue ;

          global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                       wnode->tile_x,wnode->tile_y,
                       witem->X, witem->Y, witem->Z, &x, &y, &z) ;
          if(ip)printf("  World item:  %i %i :: %.3f   %.3f   %.3f :: %.4f   %.4f   %.4f\n",
            witem->uid, witem->worldtype, witem->X, witem->Y, witem->Z, x, y, z) ;

          dyn_track_obj   = &(witem->u.dyn_track_obj) ;

//  Plot simple column at start of dynamic track (x, y, z)
          if(id){
            glDisable(GL_TEXTURE_2D) ;
            glDisable(GL_LIGHTING) ;

            glColor3f((GLfloat)1.0,(GLfloat)1.0,(GLfloat)1.0) ;
            glBegin(GL_LINE_STRIP) ;
              glVertex3f((GLfloat)x, (GLfloat)y, (GLfloat)z) ;
              glVertex3f((GLfloat)x, (GLfloat)y, (GLfloat)(z+10.0/plot_scale)) ;
            glEnd();

            glEnable(GL_TEXTURE_2D) ;
            glEnable(GL_LIGHTING) ;
          }

//  Transform coordinate system
          glMatrixMode(GL_MODELVIEW) ;
          glPushMatrix() ;
          glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z) ;

          a = witem->ANG ;
          b = witem->AX ;
          c = witem->AY ;
          d = witem->AZ ;
          glRotatef(a,b,d,c) ;    // Move to final position and rotate
          glRotatef((GLfloat) 90.0, (GLfloat) 0.0, (GLfloat) 0.0, (GLfloat) 1.0 ) ;
          glScalef(scale,scale,scale) ;

// For the moment display all distances
          for(i=0;i<3;i++){
            gl_display_list = dyn_track_obj->gl_display_list[i] ;
            glCallList(gl_display_list) ;
          }

//  Print position of shape on screen
          if(id){
            glDisable(GL_TEXTURE_2D) ;
            glDisable(GL_LIGHTING) ;

            sprintf(string," - DYN TRACK :: %i %i :: %i :: %i %i %i :: %f :: %f :: %f",
            tx, ty, witem->uid,
            dyn_track_obj->gl_display_list[0],
            dyn_track_obj->gl_display_list[1],
            dyn_track_obj->gl_display_list[2],
            (double)x,(double)y,(double)z);
            glColor3f(1.0,1.0,1.0) ;
            print_string_in_window2((GLfloat) 0.0, (GLfloat) 0.0, (GLfloat)5.0, string);

            glEnable(GL_LIGHTING)   ;
            glEnable(GL_TEXTURE_2D) ;
          }

          glPopMatrix() ;
        }   // end loop over world items
      }   // end loop over tiles
      if(ifirst>0)ifirst-- ;
      glDisable(GL_RESCALE_NORMAL) ;


      return 0;
}


