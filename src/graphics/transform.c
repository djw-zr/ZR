/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  transform.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Transformation routines used by ZR
 *
 *==============================================================================
 */

/*
 *   Routine to return modelview matrix corresponding to a
 *   given traveller.  Note: the returned 4x4 matrix is of the
 *   form *matrix[column][row] - opposite to the normal
 *   C convention.
 */
#define ZR_COORD

int transform_travel_posn(TravellerNode *t, GLdouble *mview){

  int      ip = 0  ;
  float    x, y, z ;
  GLdouble a, b, c ;
  GLdouble scalei      = 1.0/plot_scale ;
  GLdouble rail_height = 0.27  ;  // Best fit placing wheels on track
  TrkVectorNode *vn    = t->vn ;
  char     *my_name = "transform_travel_posn" ;

/*
 *  Find start of track vector section
 */
      global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                         vn->tile_east_x, vn->tile_north_z,
                         vn->east_x , vn->north_z, vn->height_y+rail_height,
                         &x, &y, &z) ;

      glMatrixMode(GL_MODELVIEW) ;
      glPushMatrix() ;

      glLoadIdentity() ;
      if(ip){
        printf(" MODELVIEW AA\n") ;
        zr_print_modelview() ;
      }
//  Move to start of track section
      glTranslated((GLdouble)x, (GLdouble)y, (GLdouble)z) ;
      if(ip){
        printf(" MODELVIEW BB\n") ;
        zr_print_modelview() ;
      }
//  Convert following distances from metres to plot_scale
      glScalef(scalei,scalei,scalei) ;
      if(ip){
        printf(" MODELVIEW DD\n") ;
        zr_print_modelview() ;
      }
/*
 *  Change rotation signs to allow for change from
 *  left hand MSTS axes to right hand ZR axes
 */
      a = -degree*vn->a_east_x   ;
      b = -degree*vn->a_height_y ;
      c = -degree*vn->a_north_z  ;

// Track Direction - (Aircraft Yaw) at start of section
      glRotated((GLdouble)b,(GLdouble)0.0,(GLdouble)0.0,(GLdouble)1.0) ;
      if(ip){
        printf(" MODELVIEW EE  %10.6f\n",(double)b) ;
        zr_print_modelview() ;
      }

// Track Slope - (Aircraft Pitch)
      glRotated((GLdouble)a,(GLdouble)1.0,(GLdouble)0.0,(GLdouble)0.0) ;
      if(ip){
        printf(" MODELVIEW FF  %10.6f\n",(double)a) ;
        zr_print_modelview() ;
      }
// Track Camber - (Aircraft Roll) - Normally zero
      if(c != (GLdouble)0.0)
        glRotated((GLdouble)c,(GLdouble)0.0,(GLdouble)1.0,(GLdouble)0.0) ;
      if(ip){
        printf(" MODELVIEW GG  %10.6f\n",(double)c) ;
        zr_print_modelview() ;
      }
// Move to traveller position on track section
// (convert Traveller MSTS position to ZR axes)
      glTranslated((GLdouble)(t->x),(GLdouble)(t->z),(GLdouble)(t->y)) ;
      if(ip){
        printf(" MODELVIEW HH  %10.6f  %10.6f  %10.6f\n",
                          (double)(t->x),(double)(t->y),(double)(t->z)) ;
        zr_print_modelview() ;
      }
//  Traveller rotation due to track curvature
      glRotated((GLdouble)(t->ang_deg),(GLdouble)0.0,(GLdouble)0.0,(GLdouble)1.0) ;
      if(ip){
        printf(" MODELVIEW II  %10.6f\n", (double)(-t->ang_deg)) ;
        zr_print_modelview() ;
      }

      glGetDoublev(GL_MODELVIEW_MATRIX,mview) ;
      glPopMatrix() ;
//      eng_speed = 0.0 ;

      return 0 ;
}

/*
 *  Routine to multiply vector by OpenGL style matrix
 *   B = M*A
 */

int zr_gl_matrix_prod(GLdouble *b, GLdouble *m, GLdouble *a){

  int i, j, k ;

      for(i=0;i<4;i++){
        b[i] = 0.0 ;
        for(j=0;j<4;j++){
          k = i + 4*j ;
          b[i] = b[i] + m[k]*a[j] ;
        }
      }
      return 0;
}

int zr_print_modelview(){

  int      j      ;
  GLdouble mv[4][4] ;

      glGetDoublev(GL_MODELVIEW_MATRIX,&mv[0][0]) ;
      for(j=0;j<4;j++){
        printf("    %10.6f %10.6f %10.6f %10.6f\n",
                      mv[0][j],mv[1][j],mv[2][j],mv[3][j]) ;
      }
      return 0 ;
}


