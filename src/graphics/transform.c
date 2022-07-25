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
 *  Routine to return model position (m) of a traveller
 */

int traveller2local(TravellerNode *t, double *x, double *y, double *z){

  int     ip = 0;
  TrkVectorNode *vn = t->vn ;                      // struc.h, train.h
  double  xt = t->x,   yt = t->y,   zt = t->z,
          av = vn->a_east_x, bv = vn->a_height_y, cv = vn->a_north_z,
          xv, yv, zv, cc, ss,
          x1, y1, z1, x2, y2, z2 ;
  char *my_name = "traveller2local" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
 *  Convert vector origin to final model position (m)
 */
      global2local(tile_x0, tile_y0, tile_h0, tile_size, 1.0,
                            vn->tile_east_x, vn->tile_north_z,
                            vn->east_x, vn->north_z, vn->height_y,
                            &xv, &yv, &zv) ;
      if(ip){
        printf("    Train: %s,  Wagon: %s %i\n",t->wagon->train->name,
                                                t->wagon->name, t->wagon->index) ;
        printf("    %i, %i, %f :: %f, %f :: %i, %i :: %f %f %f\n",
                            tile_x0, tile_y0, tile_h0, tile_size, 1.0,
                            vn->wfname_east_x, vn->wfname_north_z,
                            vn->east_x, vn->north_z, vn->height_y  ) ;
        printf("    Vector origin at  : %10.2f %10.2f %10.2f\n", xv, yv, zv) ;
        printf("    Traveller offset  : %10.4f %10.4f %10.4f\n", xt, yt, zt) ;
        printf("    Yaw, Pitch, Roll  : %10.4f %10.4f %10.4f\n", bv, av, cv) ;
      }
//  Roll
      cc = cos(cv) ; ss = sin(cv) ;
      x1 = cc*xt - ss*yt ;
      y1 = ss*xt + cc*yt ;
      z1 = zt ;
//  Pitch
      cc = cos(av) ; ss = sin(av) ;
      x2 = x1 ;
      y2 = cc*y1 - ss*z1 ;
      z2 = ss*y1 + cc*z1 ;
//  Yaw
      cc = cos(bv) ; ss = sin(bv) ;
      x1 = cc*x2 + ss*z2 ;
      y1 = y2 ;
      z1 =-ss*x2 + cc*z2 ;
/*
 *  Add vector origin
 */
      *x = x1 + xv ;
      *y = z1 + yv ;
      *z = y1 + zv ;
      if(ip){
        printf("    Traveller new pos : %10.4f %10.4f %10.4f\n", x1, y1, z1) ;
        printf("    Traveller final   : %10.2f %10.2f %10.2f\n", *x, *y, *z) ;
      }
      return 0 ;
}




/*
 *   Routine to return modelview matrix corresponding to a
 *   given traveller.  Note: the returned 4x4 matrix is of the
 *   form *matrix[column][row] - opposite to the normal
 *   C convention.
 */

int traveller2modelview(TravellerNode *t, GLdouble *mview){

  int      ip = 0  ;
  double   x, y, z ;
  GLdouble a, b, c ;
  GLdouble scalei      = 1.0/plot_scale ;
  GLdouble rail_height = 0.27  ;  // Best fit placing wheels on track
  TrkVectorNode *vn    = t->vn ;
  char     *my_name = "traveller2modelview" ;

      if(ip)printf("  Enter %s\n",my_name) ;
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
//  Move to start of track section
      glTranslated((GLdouble)x, (GLdouble)y, (GLdouble)z) ;
//  Convert following distances from metres to plot_scale
      glScalef(scalei,scalei,scalei) ;
/*
 *  Change rotation signs to allow for change from
 *  left hand MSTS axes to right hand ZR axes
 */
      a = -degree*vn->a_east_x   ;
      b = -degree*vn->a_height_y ;
      c = -degree*vn->a_north_z  ;

// Track Direction - (Aircraft Yaw) at start of section

      glRotated((GLdouble)b,(GLdouble)0.0,(GLdouble)0.0,(GLdouble)1.0) ;

// Track Slope - (Aircraft Pitch)

      glRotated((GLdouble)a,(GLdouble)1.0,(GLdouble)0.0,(GLdouble)0.0) ;

// Track Camber - (Aircraft Roll) - Normally zero

      if(c != (GLdouble)0.0)
        glRotated((GLdouble)c,(GLdouble)0.0,(GLdouble)1.0,(GLdouble)0.0) ;

// Move to traveller position on track section
// (convert Traveller MSTS position to ZR axes)

      glTranslated((GLdouble)(t->x),(GLdouble)(t->z),(GLdouble)(t->y)) ;

//  Traveller rotation due to track curvature

      glRotated((GLdouble)(t->ang_deg),(GLdouble)0.0,(GLdouble)0.0,(GLdouble)1.0) ;

      glGetDoublev(GL_MODELVIEW_MATRIX,mview) ;
      glPopMatrix() ;

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

/*
 *  Routine to rotate vector (x,y,z) around axis (ax,ay,az) by angle 'ang'
 *  Result is vector (xx,yy,zz)
 */

int rotate_axis_angle( double x,  double y,  double z,
                   double ax, double ay, double az, double ang,
                   double *xx, double *yy, double *zz){
  double c, s, c1 ;

      c = cos(ang*radian) ; c1 = 1.0-c ;
      s = sin(ang*radian) ;

      *xx = x*(ax*ax*c1+c)    + y*(ax*ay*c1-az*s) + z*(ax*az*c1+ay*s) ;
      *yy = x*(ay*ax*c1+az*s) + y*(ay*ay*c1+c)    + z*(ay*az*c1-ax*s) ;
      *zz = x*(ax*az*c1-ay*s  + y*(ay*az*c1+ax*s) + z*(az*az*c1+c))   ;

      return 0 ;
}

/*
 *  Routine to rotate vector (0,1,0) around axis (ax,ay,az) by angle 'ang'
 *  Result is vector (xx,yy,zz)
 */

int rotate_axis_angle_010( double ax, double ay, double az, double ang,
                           double *xx, double *yy, double *zz){
  double c, s, c1 ;

      c = cos(ang*radian) ; c1 = 1.0-c ;
      s = sin(ang*radian) ;

      *xx = ax*ay*c1-az*s ;
      *yy = ay*ay*c1+c    ;
      *zz = ay*az*c1+ax*s ;

      return 0 ;
}

