/*
 * =============================================================================
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  display_info.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file containing the structures used by ZR to store the data
 *   included in each world file.
 *
 * =============================================================================
 */

int   display_extra_data(){

int        ip = 0  ;
int        tx, ty  ;
GLfloat    xc, yc, zc   ;
GLfloat    vx, vy, vz   ;
GLfloat    xx, yy, zz   ;
GLfloat    m2d = 1.0/plot_scale ;  // Convert from metres to display usints
GLfloat    dist, dist0, height0  ;

WorldNode   *wnode ;
WorldItem   *witem ;
ShapeNode   *snode ;
LodControl  *lod_control ;
DistLevel   *dist_level  ;
char        string[2048] ;

      if(!display_info) return 0 ;
      xc = lookat_centre_x ;
      yc = lookat_centre_y ;
      zc = lookat_centre_z ;

      dist0   = m2d*display_info_radius ;
      height0 = m2d*display_info_height ;
      dist0   = dist0*dist0 ;

      glDisable(GL_LIGHTING) ;
      glDisable(GL_TEXTURE_2D) ;

      glColor3f((GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0) ;
      glBegin(GL_LINES) ;
        glVertex3d(xc, yc, (GLfloat) 0.0) ;
        glVertex3d(xc, yc, (GLfloat) 1.0) ;
      glEnd() ;

      glColor3f((GLfloat)1.0,(GLfloat)1.0,(GLfloat)1.0) ;
      sprintf(string," - Lookat Point  :: %f %f %f ", xc,yc,zc);
      print_string_in_window2((GLfloat) xc, (GLfloat) yc, (GLfloat) zc, string);
      glColor3f((GLfloat)1.0,(GLfloat)1.0,(GLfloat)1.0) ;

      for(wnode=worldlist_beg; wnode != NULL; wnode=wnode->next){
        if(0 == wnode->in_use) continue ;
        tx = wnode->tile_x - tile_x0 ;
        ty = wnode->tile_y - tile_y0 ;
        for(witem = wnode->world_item ; witem != NULL; witem = witem->next){
          xx = (tx+0.5)*tile_size +  witem->X ;
          yy = (ty+0.5)*tile_size +  witem->Y ;
          zz = witem->Z - tile_h0        ;
          xx = xx*m2d ;
          yy = yy*m2d ;
          zz = zz*m2d ;
          dist = (xx-xc)*(xx-xc) + (yy-yc)*(yy-yc) ;
          if(dist > dist0)continue ;

          glColor3f((GLfloat)0.0,(GLfloat)1.0,(GLfloat)1.0) ;
          glBegin(GL_LINES) ;
            glVertex3d(xx, yy, zz) ;
            zz = zz + height0      ;
            glVertex3d(xx, yy, zz) ;
          glEnd() ;
          glColor3f(1.0,1.0,1.0) ;
          sprintf(string," - WORLD   :: uid = %i, type =  %i"
                         " :: item at :: %f %f %f ",
                         witem->uid,witem->worldtype,xx,yy,zz);
          print_string_in_window2((GLfloat) xx, (GLfloat) yy, (GLfloat) zz, string);
        }
      }

      glEnable(GL_LIGHTING) ;



      return 0 ;
}
