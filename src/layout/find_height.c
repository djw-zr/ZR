/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  find_height.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routine for finding the height of topography.
 *   Routines providing a mask for yjr placing of forest trees.
 *
 *==============================================================================
 */

int  set_mask_around_item(int tile_x, int tile_y,
                          double x, double y, double radius) ;
int  set_mask_at_point(int tile_x, int tile_y, int ix, int iy, int n) ;

/*
 *  Routine to return the height of topography (m)
 *
 *  tile_x1, tile_y1  :: MSTS global index of tile
 *  x1, y1            :: Location relative (m) to centre of tile
 */
int find_height(int tile_x1, int tile_y1, double x1, double y1, double *height){

  int    ip = 0 ;
  int    tile_x, tile_y ;
  int    ix, iy, k, nsamples ;
  double xx, yy ;
  double floor, scale ;

  static int icount = 0 ;

  unsigned short *heights    ;
  TileListNode   *tlist_node ;
  TerrainData    *tdata_node ;

  char  *my_name = "find_height" ;

//      icount++ ;
//      ip = icount < 100 ;

      tile_x = tile_x1 - tile_west  ;
      tile_y = tile_y1 - tile_south ;
      xx = x1 ;
      yy = y1 ;

//      ip = (tile_x1 == 1451 && tile_y1 == 10331) ;
      if(ip)printf("  Routine %s : AA : %i %i : %f %f\n",
                             my_name, tile_x1, tile_y1, x1, y1) ;
/*
 *  Check values
 */
      if(tile_x < 0 || tile_x >= tile_array_nx || tile_y < 0 || tile_y >= tile_array_ny){
        if(ip || 1){
          printf("  Routine %s error 1\n",my_name) ;
          printf("   Tile indices out of range\n") ;
          printf("   Indices are %i %i, Limits are 0:%i and 0:%i",
                      tile_x, tile_y, tile_array_nx-1, tile_array_ny-1) ;
          printf("   tile_x1 = %i, tile_y1 = %i, tile_east = %i, tile_south = %i\n",
                     tile_x1, tile_y1, tile_east, tile_north) ;
          printf("   x1 = %10.3f, y1 = %10.3fi\n", x1, y1) ;
        }
        *height = 0.0 ;
        return 1 ;
      }
/*
 *  Load tile data
 */
      k = tile_x + tile_y*tile_array_nx ;
      tlist_node = tile_array[k] ;
      if(tlist_node == NULL ){
        if(ip || 1){
          printf("  Routine %s error 2\n",my_name) ;
          printf("   data request for missing tile\n") ;
          printf("   Tile requested is %i %i\n", tile_x, tile_y) ;
        }
        *height = 0.0 ;
        return 1 ;
      }
      tdata_node = &(tlist_node->terrain_data) ;
      nsamples   = tdata_node->terrain_nsamples ;
/*
 *  Calculate nearest height point
 */
      ix = nint((xx/tile_size + 0.5)*nsamples) ;
      iy = nint((yy/tile_size + 0.5)*nsamples) ;
/*
 *  Check within tile
 */
      while(ix<0){
        tile_x = tile_x - 1;  ix = ix + nsamples ;
      }
      while(ix>=nsamples){
        tile_x = tile_x + 1;  ix = ix - nsamples ;
      }
      while(iy<0){
        tile_y = tile_y - 1;  iy = iy + nsamples ;
      }
      while(iy>=nsamples){
        tile_y = tile_y + 1;  iy = iy - nsamples ;
      }
/*
 *  Check values
 */
      if(tile_x < 0 || tile_x >= tile_array_nx || tile_y < 0 || tile_y >= tile_array_ny){
        if(ip || 1){
          printf("  Routine %s error 3\n",my_name) ;
          printf("   Tile indices out of range\n") ;
          printf("   Indices are %i %i, Limits are 0:%i and 0:%i\n",
                      tile_x, tile_y, tile_array_nx-1, tile_array_ny-1) ;
          printf("   tile_x1 = %i, tile_y1 = %i, tile_east = %i, tile_south = %i\n",
                     tile_x1, tile_y1, tile_east, tile_north) ;
          printf("   x1 = %10.3f, y1 = %10.3f\n", x1, y1) ;
        }
        *height = 0.0 ;
        return 1 ;
      }
/*
 *  Load pointers, floor and scale
 */
      k = tile_x + tile_y*tile_array_nx ;
      tlist_node = tile_array[k] ;
      if(tlist_node == NULL ){
        if(ip || 1){
          printf("  Routine %s error 4\n",my_name) ;
          printf("   data request for missing tile\n") ;
          printf("   Tile requested is %i %i\n", tile_x, tile_y) ;
        }
        *height = 0.0 ;
        return 1 ;
      }
/*
 *  Load height
 */
      tdata_node = &(tlist_node->terrain_data) ;
      heights = tdata_node->elevations ;

      floor = tdata_node->terrain_sample_floor ;
      scale = tdata_node->terrain_sample_scale ;

      k = (nsamples-1-iy)*nsamples + ix ;
      if(k<0 || k>nsamples*nsamples){
        if(ip || 1){
          printf("  Routine %s error 4\n",my_name) ;
          printf("    Height array index out of range\n") ;
          printf("    ix = %i, iy = %i, k = %i, nsamples^2 = %i\n",
                      ix,iy,k,nsamples*nsamples) ;
        }
        *height = 0.0 ;
        return 1 ;
      }
      *height = floor + scale*heights[k] ;

      if(0 && ip)printf(" Routine %s : Heights: T %i %i, X %8.3f %8.3f, H %8.3f\n",
            my_name,tile_x1, tile_y1,x1,y1,*height) ;

      return 0 ;
}
/*
 *  Routine to check (tree) positions against rail and road track mask
 *   tile_x1, ile_y1 are MST global tile indices
 *   x1, y1 are distances in m relative to centre of tile.
 */



int check_tile_mask(int tile_x1, int tile_y1, double x1, double y1){

  int  ip = 0 ;
  int  tile_x, tile_y,
       ix, iy,
       nsamples, k ;
  double       x, y ;
  TileListNode *tlist_node ;
  TerrainData  *tdata_node ;
  char         *mask_array ;
  char         *my_name = "check_tile_mask" ;

//      ip = (tile_x1 == 1541 && tile_y1 == 10411) ;

      tile_x = tile_x1 - tile_x0 ;
      tile_y = tile_y1 - tile_y0 ;

      k = tile_x + tile_y*tile_array_nx ;
      tlist_node = tile_array[k] ;
      tdata_node = &(tlist_node->terrain_data) ;
      nsamples   = tdata_node->terrain_nsamples ;

      x  = x1/tile_size + 0.5 ;
      y  = y1/tile_size + 0.5 ;
      ix = nint(x*nsamples) ;
      iy = nint(y*nsamples) ;

      if(ip){
        printf("\n  Enter routine %s\n",my_name) ;
        printf("    tile_x1, tile_y1 = %6i %6i, x1, y1 = %8.3f %f8.3\n",
                    tile_x1, tile_y1, x1, y1) ;
        printf("    tile_x, tile_y = %3i %3i, x, y = %8.3f %8.3f,"
                  " ix, iy = %3i %3i\n",
                    tile_x, tile_y, x, y, ix, iy) ;
      }
/*
 *  Check within tile
 */
      while(ix<0){
        tile_x = tile_x - 1;  ix = ix + nsamples ;
      }
      while(ix>=nsamples){
        tile_x = tile_x + 1;  ix = ix - nsamples ;
      }
      while(iy<0){
        tile_y = tile_y - 1;  iy = iy + nsamples ;
      }
      while(iy>=nsamples){
        tile_y = tile_y + 1;  iy = iy - nsamples ;
      }
/*
 *  Check values
 */
      if(tile_x < 0 || tile_x >= tile_array_nx || tile_y < 0 || tile_y >= tile_array_ny){
        if(ip || 1){
          printf("  Routine %s error\n",my_name) ;
          printf("   Tile indices out of range\n") ;
          printf("   Indices are %i %i, Limits are 0:%i and 0:%i\n",
                      tile_x, tile_y, tile_array_nx-1, tile_array_ny-1) ;
          printf("   tile_x1 = %i, tile_y1 = %i, tile_east = %i, tile_south = %i\n",
                     tile_x1, tile_y1, tile_east, tile_north) ;
          printf("   x1 = %10.3f, y0 = %10.3f\n", x1, y1) ;
        }
        return 0 ;
      }
/*
 *  Find tile data and calculate offset
 */
      k = tile_x + tile_y*tile_array_nx ;
      mask_array = tile_mask_array[k]  ;

      if(!mask_array) return 1 ;  //  Outside valid tiles

      k = ix + iy*nsamples ;
      return mask_array[k] ;
}

/*
 *  Routine to initialise rail and road track mask
 */

int  tile_mask_init(void){

  int            ip = 0 ;
  int            icount = 0 ;
  int            i, j, k ;
  TerrainData    *tdata_node ;
  int            nsamples, nvolumes ;
  int            tile_x, tile_y ;
  double         x, y, r ;
  double         radius, xx, yy ;

  char  *mask_array ;
  WorldNode *wnode ;
  WorldItem *witem ;
  ShapeNode *snode ;
  ShapeVolume *shape_vol ;
  char      *my_name = "tile_mask_init" ;

      if(ip)printf("  SETUP MASK\n") ;
/*
 *  Allocate array of pointers to the masks for each tile
 */
      tile_mask_array = (char **)malloc(tile_array_num*sizeof(char *)) ;
/*
 * Loop over tile array.  For each existing tile generate a mask array
 */
      for(i=0; i<tile_array_num; i++){
        if(tile_array[i]){
          tdata_node = &(tile_array[i]->terrain_data) ;
          nsamples = tdata_node->terrain_nsamples ;
          mask_array = (char *)malloc(nsamples*nsamples*sizeof(char)) ;
          for(j=0;j<nsamples*nsamples; j++)mask_array[j] = 0 ;
        }else{
          mask_array = NULL ;
        }
        tile_mask_array[i] = mask_array ;
      }
/*
 *  Check array by listing elements
 */
      if(ip){
        printf("  Tile mask array: & = %p\n",(void *)tile_mask_array) ;
        for(i=0; i<tile_array_nx; i++){
          for(j=0; j<tile_array_ny; j++){
            k = j*tile_array_nx + i ;
            printf("  i, j = %2i %2i :: %6i %6i :: %p\n",
                   i,j,i+tile_west,j+tile_south,(void *)tile_mask_array[k]) ;
          }
        }
/*
 *  Graphical output
 */
        for(j=tile_array_ny-1; j>=0; j--){
          printf("  j = %2i  ",j) ;
          for(i=0; i<tile_array_nx; i++){
            k = j*tile_array_nx + i ;
            printf("%c",tile_mask_array[k] ? 'X' : '.') ;
          }
          printf("\n") ;
        }
      }
/*
 *  Loop over tiles and set arrays to represent areas around each world item
 */
      if(ip)printf("  Routine %s.  Start loop over world nodes\n",my_name) ;

      for(wnode=worldlist_beg; wnode!=NULL; wnode=wnode->next){
        tile_x = wnode->tile_x ;
        tile_y = wnode->tile_y ;
//        ip = (tile_x == 1451) && (tile_y == 10331) ;
        if(ip)printf("    tile_x, tile_y = %i %i, ip = %i, witem = %p\n",
                                     tile_x,tile_y, ip, (void *)wnode->world_item) ;
/*
 *  Loop over world items
 */
        for(witem=wnode->world_item; witem != NULL; witem=witem->next){
          snode = witem->snode ;
//          ip = ip && witem->uid == 517 ;
          if (ip)printf("      uid = %i, worldtype, = %i, snode = %p\n",
              witem->uid, witem->worldtype, (void *)snode) ;

          if(NULL == snode || NULL == snode->name)continue ;
          if(FOREST == witem->worldtype)continue ;
          if(DYNTRACK == witem->worldtype)continue ;
          if(306 == witem->worldtype)continue ;

          nvolumes = snode->nvolumes ;
          if(0 == nvolumes) continue ;

//          ip = (i_count++%10) == 0 ;
          if(ip)printf("      W %5i %5i  %5i", tile_x, tile_y, witem->uid) ;
          for(k=0; k<nvolumes ;k++){
            shape_vol = &(snode->shape_vol[k]) ;
            radius = shape_vol->radius  + 10.0 ;  //  Distance before forest trees
            xx = witem->X + shape_vol->vec.X ;
            yy = witem->Y + shape_vol->vec.Z ;
//            zz = witem->Z + shape_vol->vec.Y ;
/*
 *  Calculate position and radius in plot_scale units
 *  This might go wrong if plot_scale and the size of the tiles ever
 *  differ.
 */
#if 0
            global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                                          tile_x, tile_y, xx, yy, zz, &x, &y, &z) ;
#else
            x = xx/tile_size + 0.5 ;
            y = yy/tile_size + 0.5 ;
            r = radius/tile_size ;
#endif
            if(ip)printf(" : %f : %f %f ",r, x, y) ;
            if(ip)printf(" : %3i %3i",nint(256*x), nint(256*y) ) ;

/*
 *  Mark all topography points within 'radius' of the above point
 *  Variables in units of tile size
 */
            set_mask_around_item(tile_x-tile_x0, tile_y-tile_y0, x, y, r) ;
          }
          if(ip)printf(" : %s\n",witem->snode->name) ;
        }
        if(ip){
int   i, j, k, m ;
char  *mask_array ;
          m = tile_x + tile_y*tile_array_nx ;
          mask_array = tile_mask_array[m]  ;
          if(mask_array){
            for(j=255; j>=0; j--){
              printf(" %3i ::",j) ;
              for(i=0; i<256;i++){
                k = j*256 + i ;
                if(mask_array[k]){
                  printf("X") ;
                }else{
                  printf(".") ;
                }
              }
              printf("\n") ;
            }
            printf("\n") ;
          }
        }
//        if(ip) close_system() ;
      }
/*
 *  Copy origin of pointers to calling routine
 *  and exit
 */
      return 0 ;
}

/*
 *   Set mask around each object
 *   tile_x1, tile_y1  :  tile index relative to local origin tile_x0, tile_y0
 *   x1, y1            :  position relative to SW corner of tile (units of tile size)
 *   radius            ;  radius of circle (units of tile size)
 */

int  set_mask_around_item(int tile_x1, int tile_y1,
                          double x1, double y1,  double radius){
  int  ip = 0 ;
  int    i, j, ix, iy, ir, k, nsamples ;
  double floor, scale, x, y ;
  int tile_x, tile_y ;

  static int icount = 0 ;
  TileListNode   *tlist_node ;
  TerrainData    *tdata_node ;


  char  *my_name = "set_mask_around_item" ;

      tile_x = tile_x1 ;
      tile_y = tile_y1 ;
      x = x1 ;
      y = y1 ;

//      ip = (tile_x1+tile_x0 == 1451) && (tile_y1+tile_y0 == 10331) ;

      if(ip){
        printf("  Enter routine %s\n", my_name) ;
        printf("  tile_x, tile_y = %6i, %6i :: x, y, r = %f %f %f\n",
               tile_x, tile_y, x, y, radius) ;
        printf("  tile_array_nx = %i\n",tile_array_nx) ;
      }

/*
 *  Load tile data
 */
      k = tile_x + tile_y*tile_array_nx ;

      tlist_node = tile_array[k] ;
      if(tlist_node == NULL ){
        if(ip || 1){
          printf("  Routine %s error 1\n",my_name) ;
          printf("   data request for missing tile\n") ;
          printf("   Tile requested is %i %i\n", tile_x, tile_y) ;
        }
        return 1 ;
      }
      tdata_node = &(tlist_node->terrain_data) ;
      nsamples   = tdata_node->terrain_nsamples ;
      if(ip)printf("  tdata_node = %p, nsamples = %i\n",
                      (void *)tdata_node, nsamples) ;
/*
 *  Calculate nearest height point
 */
      ix = nint(x*nsamples) ;
      iy = nint(y*nsamples) ;
      ir = nint(radius*nsamples) + 1 ;
      if(ip){
        printf("  Set mask around point %i %i, radius %i \n", ix, iy, ir) ;
      }
      for(i=0; i<ir; i++){
        for(j=0; j<ir; j++){
          if(i*i + j*j > ir*ir) continue ;
          set_mask_at_point(tile_x, tile_y, ix+i, iy+j, nsamples) ;
          if(ix!=0)set_mask_at_point(tile_x, tile_y, ix-i, iy+j, nsamples) ;
          if(iy!=0){
            set_mask_at_point(tile_x, tile_y, ix+i, iy-j, nsamples) ;
            if(ix!=0)set_mask_at_point(tile_x, tile_y, ix-i, iy-j, nsamples) ;
          }
        }
      }
      if(ip){
        printf("  Exit routine %s\n", my_name) ;
      }

      return 0 ;
}

int  set_mask_at_point(int tile_x0, int tile_y0, int ix0, int iy0, int nsamples){

  int  ip = 0 ;
  int  tile_x = tile_x0,
       tile_y = tile_y0,
       ix     = ix0,
       iy     = iy0,
       k     ;
  char  *mask_array ;
  char *my_name = "set_mask_at_point" ;

      if(ip){
        printf("\n  Enter routine %s\n",my_name) ;
        printf("    tile_x0, tile_y0 = %6i %6i, ix0, iy0 = %4i %4i\n",
                    tile_x0, tile_y0, ix0, iy0) ;
      }
/*
 *  Check within tile
 */
      while(ix<0){
        tile_x = tile_x - 1;  ix = ix + nsamples ;
      }
      while(ix>=nsamples){
        tile_x = tile_x + 1;  ix = ix - nsamples ;
      }
      while(iy<0){
        tile_y = tile_y - 1;  iy = iy + nsamples ;
      }
      while(iy>=nsamples){
        tile_y = tile_y + 1;  iy = iy - nsamples ;
      }
/*
 *  Check values
 */
      if(tile_x < 0 || tile_x >= tile_array_nx || tile_y < 0 || tile_y >= tile_array_ny){
        if(ip){
          printf("  Routine %s error\n",my_name) ;
          printf("   Tile indices out of range\n") ;
          printf("   Indices are %i %i, Limits are 0:%i and 0:%i\n",
                      tile_x, tile_y, tile_array_nx-1, tile_array_ny-1) ;
          printf("   tile_x0 = %i, tile_y0 = %i, tile_east = %i, tile_south = %i\n",
                     tile_x0, tile_y0, tile_east, tile_north) ;
          printf("   ix0 = %4i, y0 = %4i\n", ix0, iy0) ;
        }
        return 0 ;                         //  Stray out of supported tile region
      }

      k = tile_x + tile_y*tile_array_nx ;
      mask_array = tile_mask_array[k]  ;
      if(!mask_array) return 0 ;           //  Stray out of supported tile region

      k = ix + iy*nsamples ;
      mask_array[k] = 1 ;

      return 0 ;
}


