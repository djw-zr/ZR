/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  topog_cva.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to set up the display lists for topography
 *   Using vertex arrays and buffer objects.
 *
 *==============================================================================
 */
/*
 *==============================================================================
 *   Routine to create vertex arrays for each of the tiles.
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

int make_tile_vertex_arrays(){

int          ip = 0 ;  // Debug
int          ix, iy ;
TileListNode *tnode ;
char         my_name[] = "make_tile_vertex_arrays" ;

      if(ip)printf(" Enter: %s\n",my_name) ;
      for(tnode=tilelist_head;tnode!=NULL;tnode=tnode->next){
        ix = tnode->tilex ;
        iy = tnode->tiley ;
        if(!use_tile(ix,iy)) continue ;     // Tile topography not needed
        tnode->needed          = 1     ;
        make_tile_vertex_array(tnode)  ;
      }
      return 0;
}

int make_tile_vertex_array(TileListNode *tnode){

int            i, j, k, l, kt, l1, l2, n1, n2, n3, n4, i3, j3, m3 ;
int            tile_x, tile_y ;
int            ip = 0     ;  // Debug printing
int            nht        ;  // Number of tile heights in each direction
double         dx_topog   ;  // Horizontal spacing in plot space units
double         dm_topog   ;  // Horizontal spacing in metres
double         dd_texture ;  // Texture repeats per topographic sample
double         floor, scale, v1[3], v2[3], v3[3], vv, av ;
GLdouble       tx1, tx2, ty1, ty2, x1, x2, y1, y2, h1, h2, h3, h4 ;
GLdouble       ttx1, ttx2, tty1, tty2 ; // Texture coordinates
GLdouble       *normals, *heights ;
int            *flata, flat ;         // = 1 if adjacent to a flat(ish) area
unsigned short *elevations  ;
char           my_name[] = "make_tile_vertex_array" ;

int            max_i, max_v ;
int            nbx, nby, ncx, ncy, ib, jb, nvt, ii, jj, ij;
VANode         *va_node ;

TerrainData    *terrain = &(tnode->terrain_data) ;
GLfloat        *vertex ;
#ifdef normal_byte
  GLbyte         *normal  ;
#else
  GLfloat        *normal  ;
#endif
#ifdef texture_short
  GLshort        *texture ;
#else
  GLfloat        *texture ;
#endif
GLuint         *index1, *index ;
GLfloat        xa[2],ya[2],za[2] ;
GLdouble       scalei = 1.0/plot_scale ;

GLfloat  mat_amb_land[] = {0.2, 0.2, 0.2, 1.0};
GLfloat  mat_dif_land[] = {0.9, 0.9, 0.9, 1.0};
GLfloat  mat_spc_land[] = {0.5, 0.5, 0.5, 1.0};


      if(ip)printf(" Enter routine %s\n",my_name) ;


      floor      = tnode->terrain_data.terrain_sample_floor ;
      scale      = tnode->terrain_data.terrain_sample_scale ;
      elevations = tnode->terrain_data.elevations           ;
      nht        = tnode->terrain_data.terrain_nsamples     ;

      dx_topog = 1.0/nht               ;  // Spacing in tile units
      dm_topog = tile_size/nht         ;  // Spacing in m
      dd_texture = 2.0                 ;  // Texture distance per terrain spacing

      tile_x = tnode->tilex ;
      tile_y = tnode->tiley ;
      if(ip)printf("  Tile_x = %i %i, tile_y = %i %i :: %i\n",
                                 tile_x,tile_x0,tile_y,tile_y0,tnode->needed) ;
      if(ip)printf("  plot_scale = %f %f\n",(double)plot_scale,(double)scalei) ;
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
      if(!land_texture->gl_tex_ref_no){
        printf("  ERROR.  Routine %s called for tile (%i, %i) "
        "when land_texture not installed.\n",my_name,tile_x,tile_y);
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
double hh  ;
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
 *==============================================================================
 *  Initilise vertex arrays
 *
 *  Low level graphics cards may struggle with GPU storage for the full
 *  256 by 256 topographic array data.  The code therefor includes the option
 *  of splitting the data into nbx*nby blocks, each containing ncx*ncy cells.
 *
 *  Implementations denote recommended maximum amounts of vertex and index data, which may be queried by calling glGet with argument GL_MAX_ELEMENTS_VERTICES and GL_MAX_ELEMENTS_INDICES. If end−start+1 is greater than the value of GL_MAX_ELEMENTS_VERTICES, or if count is greater than the value of GL_MAX_ELEMENTS_INDICES, then the call may operate at reduced performance. There is no requirement that all vertices in the range [start,end] be referenced. However, the implementation may partially process unused vertices, reducing performance from what could be achieved with an optimal index set.
 *
 *
 *
 *==============================================================================
 */
/*
 *  Determine limits
 */
      glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_v) ;
      if(ip)printf("  GL_MAX_ELEMENTS_VERTICES = %i\n",max_v) ;
      glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &max_i) ;
      if(ip)printf("  GL_MAX_ELEMENTS_INDICES  = %i\n",max_i) ;
/*
 *   Number of vertices = number of
 *   Number of indices = 6 * number of cells
 */

      nbx = 1 ;
      nby = 1 ;
      for(;;){
        ncx = nht/nbx ;
        ncy = nht/nby ;
        nvt = (ncx+1)*(ncy+1) ;
        if(nvt <= max_v && 6*ncx*ncy <= max_i)break ;
        if(nbx == nby)  nbx = nbx*2 ;
        else            nby = nbx   ;
      }
      if(ip || 0)printf("  Topography split into %i by %i blocks\n",nbx,nby) ;
/*
 *  Allocate Vertex array nodes
 */
      terrain->nbx = nbx ;
      terrain->nby = nby ;
      terrain->va_node = (VANode *)malloc(nbx*nby*sizeof(VANode)) ;
      for(jb=0;jb<nby;jb++){
        for(ib=0;ib<nbx;ib++){
          va_node = &terrain->va_node[jb*nby+ib] ;
          va_node->ncx       = ncx ;
          va_node->ncy       = ncy ;
          va_node->nva_index = 6*ncx*ncy ;
          va_node->in_use    = 1   ;
//  Allocate memory
          vertex  = va_node->va_vertex  = (GLfloat *)malloc(3*nvt*sizeof(GLfloat)) ;
#ifdef normal_byte
          normal  = va_node->va_normal  = (GLbyte *)malloc(3*nvt*sizeof(GLbyte)) ;
#else
          normal  = va_node->va_normal  = (GLfloat *)malloc(3*nvt*sizeof(GLfloat)) ;
#endif

#ifdef texture_short
          texture = va_node->va_texture = (GLshort *)malloc(2*nvt*sizeof(GLshort)) ;
#else
          texture = va_node->va_texture = (GLfloat *)malloc(2*nvt*sizeof(GLfloat)) ;
#endif
          index   = va_node->va_index  = (GLuint *)malloc(6*ncx*ncy*sizeof(GLuint)) ;

          if(ip){
            printf("  AA  ib, jb = %i %i\n",ib,jb) ;
            printf("  AA vertex  = %p %p\n",(void *)vertex, (void *)va_node->va_vertex);
            printf("  AA normal  = %p %p\n",(void *)normal, (void *)va_node->va_normal);
            printf("  AA texture = %p %p\n",(void *)texture, (void *)va_node->va_texture);
            printf("  AA index   = %p %p\n",(void *)index, (void *)va_node->va_index);
          }
/*
 *  2.  Initialise vertices and textures
 *        i,j   index of cell in sub-arrays
 *        ii,jj index in full array
 *        k = index of first coordinate of 3-vectors (vertex, normal)
 *        l = index of first vertex of 2-vectors (texture)
 *        k = index of first coordinate describing vertex
 */
          for(j=0,k=0,l=0;j<ncy+1;j++){
            jj = jb*ncy + j ;
            for(i=0;i<ncx+1;i++,k+=3,l+=2){
              ii = ib*ncx + i ;
              ij = (jj+1)*m3 + ii + 1 ;

              vertex[k  ] = (tile_x - tile_x0)     +  dx_topog*ii ;
              vertex[k+1] = (tile_y - tile_y0 + 1) -  dx_topog*jj ;
              vertex[k+2] = (heights[ij]-tile_h0)*scalei ;
//  Track limits
              if(0==k){
                xa[0] = xa[1] = vertex[0] ;
                ya[0] = ya[1] = vertex[1] ;
                za[0] = za[1] = vertex[2] ;
              }else{
                if(vertex[k]<xa[0])xa[0] = vertex[k];
                if(vertex[k]>xa[1])xa[1] = vertex[k];
                if(vertex[k+1]<ya[0])ya[0] = vertex[k+1];
                if(vertex[k+1]>ya[1])ya[1] = vertex[k+1];
                if(vertex[k+2]<za[0])za[0] = vertex[k+2];
                if(vertex[k+2]>za[1])za[1] = vertex[k+2];
              }
#ifdef normal_byte
              normal[k  ] =   0 ;
              normal[k+1] =   0 ;
              normal[k+2] = 127 ;
#else
              normal[k  ] = normals[3*ij]   ;
              normal[k+1] = normals[3*ij+1] ;
              normal[k+2] = normals[3*ij+2] ;
#endif
#ifdef texture_short
              texture[l  ]  = i ;
              texture[l+1]  = j ;
#else
              texture[l  ] = dd_texture*i     ;
              texture[l+1] = dd_texture*j     ;
#endif
            }
          }
          for(j=0,k=0;j<ncy;j++){
            for(i=0;i<ncx;i++,k+=6){
              ij = j*(ncx+1) + i ;
              index[k  ] = ij           ;
              index[k+1] = ij + ncx + 1 ;
              index[k+2] = ij + ncx + 2 ;
              index[k+3] = ij           ;
              index[k+4] = ij + ncx + 2 ;
              index[k+5] = ij + 1       ;
            }
          }
/*
 *  save limits
 */
          va_node->xa[0] = xa[0] ; va_node->xa[1] = xa[1] ;
          va_node->ya[0] = ya[0] ; va_node->ya[1] = ya[1] ;
          va_node->za[0] = za[0] ; va_node->za[1] = za[1] ;
          va_node->in_use = 1 ;    //  Tests not availale at this stage


          if(ip>1 && tile_x==1448 && tile_y == 10331){
            printf("  Vertex Array  tile_x, tile_y = %i %i\n",tile_x,tile_y) ;
            for(i=0;i<ncx+1;i++){
              j = 0 ;
              k = j*(ncx+1) + i ;
              printf(" i, j = %3i %3i  vertex = %f %f %f\n",
                     i,j, vertex[3*k],vertex[3*k+1],vertex[3*k+2]) ;
            }
            printf("\n");
            for(j=0;j<ncy+1;j++){
              i = 0 ;
              k = j*(ncx+1) + i ;
              printf(" i, j = %3i %3i  vertex = %f %f %f\n",
                     i,j,vertex[3*k],vertex[3*k+1],vertex[3*k+2]) ;
            }
            printf("\n");
            for(k=0;k<6*ncx;k++){
              i = 3*index[k] ;
              printf("  k, i = %4i %4i :: x, y, h = %f %f %f\n",
                                 k,i,vertex[i],vertex[i+1],vertex[i+2]);
            }
            printf("\n");
          }
        }
      }
      free(normals) ;
      free(heights) ;
      free(flata) ;
      return 0 ;
}

int check_topographic_blocks(){

int          ip = 0       ;  // Debug
int          ix, iy, k, n ;
TileListNode *tnode       ;
VANode       *va_node     ;
char         my_name[] = "check_topographic_blocks" ;

      if(ip)printf(" Enter: %s\n",my_name) ;
      for(tnode=tilelist_head;tnode!=NULL;tnode=tnode->next){
        ix = tnode->tilex ;
        iy = tnode->tiley ;
        if(ip)printf("  tile_x, tile_y, use_tile = %i %i %i :: %i %i :: %i %i :: %i\n",
        ix,iy,use_tile(ix,iy), tile_eye_x0, tile_x0, tile_eye_x0, tile_y0, tile_cull_r);
        if(!use_tile(ix,iy)) continue ;     // Tile topography not needed
        tnode->needed          = 1     ;

        if(ip)printf(" name = %s, nbx = %i, nby = %i\n",tnode->name,
               tnode->terrain_data.nbx,
               tnode->terrain_data.nby ) ;
        n = tnode->terrain_data.nbx*tnode->terrain_data.nby ;
        if(ip)printf(" n = %i\n",n) ;
        if(n != 256){
          tnode->needed = 0;
          continue ;
        }
        for(k=0;k<n;k++){
          if(ip)printf(" %i",k) ;
          va_node = &(tnode->terrain_data.va_node[k]) ;
          va_node->in_use = check_topog_in_scene2(va_node->xa,va_node->ya,va_node->za);
        }
        if(ip)printf("\n") ;
      }
      if(ip)printf(" Exit: %s\n",my_name) ;

      return 0 ;
}
