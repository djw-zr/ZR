/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  topog_dlist.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to set up the display lists for topography
 *
 *==============================================================================
 */
/*
 *==============================================================================
 *   Generate a topography display list for each of the tiles.
 *
 *   The calculation of normals appears to be computationally expensive so the
 *   use of display lists saves a lot of time.
 *
 *   In the MSTS ascii files, positions are given in terms of a tile, specified
 *   by the integers tile_x and tile_y for east and north, and floating points
 *   numbers X and Z (also east and north), specifying in meters the distances
 *   from the centre of the tile.
 *
 *   However the topography files (*.y_raw) are organised differently.  They
 *   contain a 2-D array (typically 256x256) in which the column index runs from
 *   west to east but the row index runs from north to south.
 *
 *   In a way this is all cackhanded - using a left-had rule in which the thumb
 *   and first two fingers denote east, up and north instead of the more usual
 *   right hand rule, in which the the first two are the normal x and y and
 *   the third index is upwards.
 *
 *   Similarly the array organisation is the mathematician's matrix layout
 *   instead of the longitude/latitude (or x/y) layout used by the physical
 *   sciences.
 *
 *   But it is no good complaining - just remember the odd convention.
 *==============================================================================
*/

//  Values used for debugging

static int      tx_chk =  1451,
                ty_chk = 10331,
                i_chk  =   153,
                j_chk  =    25  ;
static double   x_chk  =  5.597613,
                y_chk  = 12.902323;

int make_tile_topog_display_list(TileListNode *tile_node, GLuint klist){

int            i, j, k, l1, l2, n1, n2, n3, n4, i3, j3, m3 ;
int            tile_x, tile_y ;
int            ip = 0     ;  // Dbug printing
int            nht        ;  // Number of tile heights in each direction
double         dx_topog   ;  // Horizontal spacing in plot space units
double         dm_topog   ;  // Horizontal spacing in metres
double         dd_texture = 100 ;  // Size of texture in plot space units
double         floor, scale, v1[3], v2[3], v3[3], vv, av ;
GLdouble       tx1, ty1, ty2, x1, x2, y1, y2, h1, h3, h4 ;
GLdouble       ttx1, ttx2, tty1, tty2 ; // Texture coordinates
GLdouble       *normals, *heights ;
int            *flata, flat ;         // = 1 if adjacent to a flat(ish) area
unsigned short *elevations  ;
char           my_name[] = "make_tile_topog_display_list" ;

GLfloat  mat_amb_land[] = {0.2, 0.2, 0.2, 1.0};
GLfloat  mat_dif_land[] = {0.9, 0.9, 0.9, 1.0};
GLfloat  mat_spc_land[] = {0.5, 0.5, 0.5, 1.0};

      if(ip)printf(" Enter routine %s\n",my_name) ;
      floor      = tile_node->terrain_data.terrain_sample_floor    ;
      scale      = tile_node->terrain_data.terrain_sample_scale    ;
      elevations = tile_node->terrain_data.elevations ;
      nht  = tile_node->terrain_data.terrain_nsamples ;
      dx_topog = 1.0/nht            ;
      dm_topog = dx_topog*tile_size ;
      dd_texture = nht*0.5          ;

      tile_x = tile_node->tilex ;
      tile_y = tile_node->tiley ;
      if(ip){
        printf(" Routine %s, tile_x = %i, tile_y = %i\n",my_name,tile_x,tile_y) ;
//        ip = (tile_x==1449) && (tile_y==10331) ;
        printf("  ip = %i\n",ip) ;
      }
/*
 *  Define material properties of land.
 *  Determines surface light values before textures are applied.
 */
      glMaterialfv(GL_FRONT,GL_AMBIENT,mat_amb_land) ;
      glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_dif_land) ;
      glMaterialfv(GL_FRONT,GL_SPECULAR,mat_spc_land) ;
/*
 *  Check texture reference exists
 */
      if(!land_texture){
        printf("  ERROR.  Routine %s called when land_texture not installed\n",
                                                                       my_name);
      }
      if(!land_texture->gl_tex_ref_no){
        printf("  ERROR.  Routine %s called when land_texture not installed\n",
                                                                       my_name);
      }
/*
 *   Create oversize arrays including halo rows and columns
 *   for i/j = -1, nht and nht+1.  Used to join the topography
 *   of different tiles. *
 */
      m3 = nht + 3 ;
      heights = (GLdouble *) malloc(m3*m3*sizeof(GLdouble)) ;
      flata   = (int *) malloc(m3*m3*sizeof(int)) ;
      normals = (GLdouble *) malloc(3*m3*m3*sizeof(GLdouble)) ;

      for(k=0;k<m3*m3;k++)   heights[k] = 0.0 ;
      for(k=0;k<m3*m3;k++)   flata[k]   =   0 ;
      for(k=0;k<3*m3*m3;k++) normals[k] = 0.0 ;
/*
 * Fill height array from current tile and surrounding tiles
 */
      for(j3=0;j3<m3;j3++){
        j = j3 - 1 ;
        for(i3=0;i3<m3;i3++){
          i = i3 - 1 ;
          if(j>=0 && j<nht && i>=0 && i<nht){
            heights[j3*m3 + i3] = floor + scale*elevations[j*nht + i] ;
          }else{
            heights[j3*m3 + i3] = tile_height(tile_x, tile_y, i, j, nht);
          }
          if(ip && i>10 && i<15 && j>10 && j<15){
            printf("  Heights  i, j = %i, %i :: %f %f :: %i %f\n",
                  i, j, floor, scale, elevations[j*nht + i], heights[j3*m3 + i3]) ;
          }

        }
      }
/*
 *  Fill missing values needed to calculate normals with something sensible
 */
      for(i3=0;i3<m3;i3++){
        if(heights[           i3]==0.0)heights[           i3] = heights[    m3+i3] ;
        if(heights[(nht+1)*m3+i3]==0.0)heights[(nht+1)*m3+i3] = heights[nht*m3+i3] ;
        if(heights[(nht+2)*m3+i3]==0.0)heights[(nht+2)*m3+i3] = heights[nht*m3+i3] ;
      }
      for(j3=0;j3<m3;j3++){
        if(heights[j3*m3      ]==0.0)heights[j3*m3      ] = heights[j3*m3+  1] ;
        if(heights[j3*m3+nht+1]==0.0)heights[j3*m3+nht+1] = heights[j3*m3+nht] ;
        if(heights[j3*m3+nht+2]==0.0)heights[j3*m3+nht+2] = heights[j3*m3+nht] ;
      }
      if(heights[(nht+1)*m3] == 0.0 )heights[(nht+1)*m3] = heights[nht*m3+1] ;
      if(heights[nht+1     ] == 0.0 )heights[nht+1]      = heights[nht + m3] ;
      if(heights[(nht+1)*m3+nht+1] == 0.0 )heights[(nht+1)*m3+nht+1] = heights[nht*m3+nht] ;
/*
 *    Generate normal vectors - one for each vertex of the topography
 *       based on the average slope between the nearest four vertices.
 *
 */
int   i1a[8] = {  1,  1,  0,  0, -1, -1,  0,  0 };  // E,  E,  N,  N,  W,  W,  S,  S
int   j1a[8] = {  0,  0, m3, m3,  0,  0,-m3,-m3 };
int   i2a[8] = {  1,  1,  1, -1, -1, -1, -1,  1 };  // SE, NE, NE, NW, NW, SW, SW, SE
int   j2a[8] = {-m3, m3, m3, m3, m3,-m3,-m3,-m3 };
int   km ;
double s1a[8], s2a[8], sab[8], sm, sign;


      for(j=0;j<nht+1;j++){            // Tile plus 2 row overlap
        for(i=0;i<nht+1;i++){          // Tile plus 2 column overlap
          n3 = (j+1)*m3 + i + 1 ;      // Location in expanded array
          km = -1   ;
          sm = 1000.0  ;
          for(k=0;k<8;k++){
            s1a[k] = (heights[n3+i1a[k]+j1a[k]]-heights[n3]) ;
            s2a[k] = (heights[n3+i2a[k]+j2a[k]]-heights[n3+i1a[k]+j1a[k]]) ;
            sab[k] = fabs(s1a[k]) > fabs(s2a[k]) ? fabs(s1a[k]) : fabs(s2a[k])   ;
            if(sab[k]<sm){sm = sab[k]; km = k ; }
          }


          ty1 =     j*dx_topog - 1     ;  //  Northern row of strip, tile y location
          tx1 =     i*dx_topog        ;  //  Tile x location
          y1  = tile_y - tile_y0 - ty1 ;  //  Northern row of strip, world y location
          x1  = tile_x - tile_x0 + tx1 ;  //  World x location
//
//  DEBUG
//

          if(ip && (fabs(x1-x_chk)<0.015 && fabs(y1-y_chk)<0.015) ){
            printf(" Height  tile = %4i %4i :: i, j = %4i %4i :: x, y = %7.3f %7.3f :: height = %7.3f :: Slope = %i  %f :: %f %f\n",tile_x,tile_y,i,j,x1,y1,(double)heights[n3],km,(double)sm, (double)dm_topog, (double)(sm/dm_topog));
          }
          if(ip && (tx_chk==tile_x) && (ty_chk==tile_y) && (i_chk==i) && (j_chk==j) ){
int ii, jj, kk ;
            printf("   Heights around (i,j) = %i  %i  :: \n",i,j) ;
            for(jj=0;jj<5;jj++){
            printf(" j value = %i    :: ",j-2+jj) ;
            for(ii=0;ii<5;ii++){
              kk = i - 1 + ii + m3*(j - 1 + jj) ;  // arrray origin at (1,1) because of overlap
              printf("   %8.3f  ",heights[kk]) ;
            }
            printf("\n") ;
            }
          }

/*
 *  This treats a 2.5% slope as 'flat'.
 */
          flat = sm/dm_topog < 0.025 ;
/*
 *  Calculate normal for 'flat' vertices using the minimum slope
 */
          if(flat && 1){
            sign  = km & 1 ? 1 : -1 ;      // sign = +1 for odd km, -1 for even
            v1[0] = i1a[km]*dm_topog ;
            v1[1] = j1a[km]*dm_topog/m3 ;
            v1[2] = s1a[km]       ;
            v2[0] = sign*(i2a[km] - i1a[km])*dm_topog ;
            v2[1] = sign*(j2a[km] - j1a[km])*dm_topog/m3 ;
            v2[2] = s2a[km]       ;
            v3[0] =    v1[1]*v2[2] - v1[2]*v2[1] ;
            v3[1] =    v1[2]*v2[0] - v1[0]*v2[2] ;
            v3[2] =    v1[0]*v2[1] - v1[1]*v2[0] ;
/*
 *  Calculate normal normals
 */
          }else{
            v1[0] = 2.0*dm_topog ;
            v1[1] = 0.0          ;
            v1[2] = (heights[n3+1] -heights[n3-1])  ; // Index inceases eastwards
            v2[0] = 0.0          ;
            v2[1] = 2.0*dm_topog ;
            v2[2] = (heights[n3-m3]-heights[n3+m3]) ; // Index inceases southwards
            v3[0] =  - v2[1]*v1[2] ;
            v3[1] =  - v2[2]*v1[0] ;
            v3[2] =    v1[0]*v2[1] ;
          }

/*
 *  Normalise(!)
 */
          vv = 1.0/sqrt(v3[0]*v3[0] +v3[1]*v3[1] + v3[2]*v3[2]) ;

          normals[3*n3  ] = v3[0]*vv ;
          normals[3*n3+1] = v3[1]*vv ;
          normals[3*n3+2] = v3[2]*vv ;
          flata[n3]       = flat ;
          if(ip && (fabs(x1-x_chk)<0.015 && fabs(y1-y_chk)<0.015) ){
            printf(" Normal  tile = %4i %4i :: i, j = %4i %4i :: x, y = %7.3f %7.3f :: flat = %i :: Normal =  %f %f %f\n",tile_x,tile_y,i,j,x1,y1,flat,(double)(v3[0]*vv),(double)(v3[1]*vv),(double)(v3[2]*vv));
          }
        }
      }
/*
 *  Generate Tile display List
 */
      glNewList(klist,GL_COMPILE) ;
      glBindTexture(GL_TEXTURE_2D, land_texture->gl_tex_ref_no) ;

      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;
#if 0
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR) ;
#endif
/*
 * Loop over tile
 *
 * 1.  North - south loop over tile
 *     Most southern row includes one vertex from next tile
 */
      for(j=0;j<nht;j++){     // Tile plus one row on tile to south
        ty1 =     j*dx_topog - 1     ;  //  Northern row of strip, tile y location
        ty2 = (j+1)*dx_topog - 1      ;  //  Southern row
        y1  = tile_y - tile_y0 - ty1 ;  //  Northern row of strip, world y location
        y2  = tile_y - tile_y0 - ty2 ;  //  Southerm row
        tty1 = y1*dd_texture ;
        tty2 = y2*dd_texture ;
/*
 *  2.  East - west loop over tile + 1
 *
 *  2.1  Start strip - add first two points
 */
        x2   = tile_x - tile_x0 ;
        ttx2 = x2*dd_texture    ;
        n3   = (j+1)*m3 + 1     ;
        n4   = n3 + m3          ;
        h3 = (heights[n3] - tile_h0)/plot_scale ;
        h4 = (heights[n4] - tile_h0)/plot_scale ;

        glBegin(GL_TRIANGLE_STRIP) ;

        glTexCoord2d(ttx2, tty1) ;
        glNormal3dv(&normals[3*n3]) ;
        glVertex3d(x2, y1, h3) ;

        glTexCoord2d(ttx2, tty2) ;
        glNormal3dv(&normals[3*n4]) ;
        glVertex3d(x2, y2, h4) ;

        check_glerror2(" topog_dlist AA : after starting GL_TRIANGLE_STRIP\n") ;
/*
 *   Cycle over cells of strip - including overlap
 */
        l2  = 0 ;
        for(i=0;i<nht;i++){     // Cycle over cells of strip
          l1 = (i == nht-1) ;   // End cell of row

          x1   = x2   ;
          ttx1 = ttx2 ;
          n1   = n3   ;
          n2   = n4   ;
          h1   = h3   ;
//        h2   = h4   ;    //  Not normally used

          x2   = x1 + dx_topog ;
          ttx2 = x2*dd_texture ;
          n3   = n1 + 1 ;
          n4   = n2 + 1 ;
          h3 = (heights[n3] - tile_h0)/plot_scale ;
          h4 = (heights[n4] - tile_h0)/plot_scale ;

          if(ip && i>10 && i<15 && j>10 && j<15){
            printf("  Heights  i, j = %i, %i, heights = %f %f : %f %f : %f %f\n",
                  i, j, h3, h4, heights[n3], heights[n4], tile_h0, plot_scale) ;
          }


// Flag possible track regions needing a break in the triangle strip
          av = (heights[n4] + heights[n1])*0.5  ;
          l2 = ( ((fabs(heights[n4]-heights[n1])/dm_topog) < 0.04)
              && ((fabs(heights[n2]-heights[n3])/dm_topog) > 0.04)
              && ( ((fabs(heights[n2]-av)/dm_topog) < 0.02)
                || ((fabs(heights[n3]-av)/dm_topog) < 0.02) ) ) ;

          if(ip && (fabs(x1-x_chk)<0.015) && (fabs(y1-y_chk)<0.015) && 1){
            printf(" Logic  tile = %4i %4i :: i, j = %4i %4i :: x1, x2, y1, y2= %7.3f %7.3f  %7.3f %7.3f :: height = %7.3f %7.3f  %7.3f %7.3f\n",
                   tile_x,tile_y,i,j,x1,x2,y1,y2,(
                   double)heights[n1],(double)heights[n2],(double)heights[n3],(double)heights[n4]);
            printf(" Logic  tile = %4i %4i :: i, j = %4i %4i :: l1, l2, = %i %i :: funcs = %7.3f %7.3f  %7.3f %7.3f :: %f  %f\n",
                   tile_x,tile_y,i,j, l1, l2,
                   (double)fabs(heights[n4]-heights[n1])/dm_topog,
                   (double)fabs(heights[n2]-heights[n3])/dm_topog,
                   (double)fabs(heights[n2]-av)/dm_topog,
                   (double)fabs(heights[n3]-av)/dm_topog, av, dm_topog);
          }

          if(l2 && 1){
//  Special case.
//  This is to prevent some land overlying rail track.  The break in GL_TRIANGLE_STRIP may be
//      computationally expensive as it is used everywhere.  A better scheme may be to only
//      use the scheme when a rail track is nearby - once a "find nearest track point" routine is
//      available.
//
//  Complete triangle n1, n2, n4
            glTexCoord2d(ttx2, tty2) ;
            glNormal3dv(&normals[3*n4]) ;
            glVertex3d(x2, y2, h4) ;
            glEnd() ;
//  Draw triangle n1, n4, n3
            glBegin(GL_TRIANGLES) ;

            glTexCoord2d(ttx1, tty1) ;
            glNormal3dv(&normals[3*n1]) ;
            glVertex3d(x1, y1, h1) ;

            glTexCoord2d(ttx2, tty2) ;
            glNormal3dv(&normals[3*n4]) ;
            glVertex3d(x2, y2, h4) ;

            glTexCoord2d(ttx2, tty1) ;
            glNormal3dv(&normals[3*n3]) ;
            glVertex3d(x2, y1, h3) ;

            glEnd() ;

//  Restart GL_TRIANGLE_STRIP
            if(!l1){
              glBegin(GL_TRIANGLE_STRIP) ;

              glTexCoord2d(ttx2, tty1) ;
              glNormal3dv(&normals[3*n1]) ;
              glVertex3d(x2, y1, h3) ;

              glTexCoord2d(ttx2, tty2) ;
              glNormal3dv(&normals[3*n4]) ;
              glVertex3d(x2, y2, h4) ;
            }
//  Normal case
          }else{
            glTexCoord2d(ttx2, tty1) ;
            glNormal3dv(&normals[3*n3]) ;
            glVertex3d(x2, y1, h3) ;

            glTexCoord2d(ttx2, tty2) ;
            glNormal3dv(&normals[3*n4]) ;
            glVertex3d(x2, y2, h4) ;
          }
        }
        if(!l2)glEnd() ;
      }
/*
 * Option to plot normal vectors - length 1 m.
 */
#if 0
      glDisable(GL_LIGHTING) ;
      glDisable(GL_TEXTURE_2D) ;
      glColor3f(1.0,1.0,1.0) ;
      glBegin(GL_LINES) ;
      for(j=0;j<nht;j++){
      for(i=0;i<nht;i++){
        y1 = tile_y - tile_y0 - j*dx_topog + 1.0 ;
        x1 = tile_x - tile_x0 + i*dx_topog ;
        n1 =  (j + 1)*m3 + i + 1 ;
        h1  = (heights[n1] - tile_h0)/plot_scale ;
        glVertex3d(x1, y1, h1) ;
        x1 = x1 + normals[3*n1]*1.0/plot_scale ;
        y1 = y1 + normals[3*n1+1]*1.0/plot_scale ;
        h1 = h1 + normals[3*n1+2]*1.0/plot_scale ;
        glVertex3d(x1, y1, h1) ;
      }
      }
      glEnd() ;
      glEnable(GL_LIGHTING) ;
      glEnable(GL_TEXTURE_2D) ;
#endif
      glEndList() ;
      free(normals) ;
      free(heights) ;
      free(flata) ;
      return 0 ;
}
