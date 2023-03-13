/*
 * *****************************************************************************
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File : tiles.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to handle the loading of tile data
 *
 * *****************************************************************************
 */

/*
 * *****************************************************************************
 *  Routine tilelist_init
 *
 *  Routine to initialise the list of tiles needed to run the program
 *
 *  Parameters are the boundaries of the track world determined from the track
 *  data file.
 * *****************************************************************************
 */

static TileListNode  *tiledata_new(int i, int j) ;

int tilelist_init(int tile_w, int tile_e, int tile_s, int tile_n)
{
  int  i, j, nn, ni, len ;
  char *name      = NULL,
       *t_file    = NULL ;
  FILE *fp ;
  TileListNode  *tilelist_new ;
/*
 * Set up tile_array, an array of pointers to the tile nodes
 */
      len = (tile_e + 1 - tile_w)*(tile_n + 1 - tile_s) ;
      tile_array = (TileListNode **)malloc(len*sizeof(TileListNode *)) ;
      tile_array_num = len ;

      ni = tile_e + 1 - tile_w ;
      for(j=tile_s;j<tile_n+1;j++){
        for(i=tile_w;i<tile_e+1;i++){
          tilelist_new = tiledata_new(i,j) ;
          name = TileToFileName(i, j, 15);
// Check for T file
          len = strlen(name) + strlen(ORroutedir) + strlen("TILES/") + 3 ;
          t_file    = (char *)malloc(len) ;
          strcpy(t_file   ,ORroutedir)    ;
          strcat(t_file   ,"TILES/")      ;
          strcat(t_file   ,name)          ;
          strcat(t_file   ,".t")          ;
          fp = gopen(t_file   ,"r")       ;
          if(NULL == fp){
//            printf(" File not found : %i %i : %s\n",i,j,name);
            nn = (j-tile_s)*ni + i - tile_w ;
            tile_array[nn] = NULL ;
            free(name) ;
            free(t_file   );
          }else{
//            printf(" File found     : %i %i : %s\n",i,j,name);
            nn = (j-tile_s)*ni + i - tile_w ;
            tile_array[nn] = tilelist_new   ;
            tilelist_new->name      = name      ;
            tilelist_new->t_file    = t_file    ;
            tilelist_new->t_found   = 1         ;
            tilelist_new->next      = tilelist_head ;
            tilelist_head = tilelist_new ;
//            printf(" tilelist_head = %p\n",(void *)tilelist_head) ;
//            printf(" tilelist_new  = %p\n",(void *)tilelist_new)  ;
//            printf(" i, j, n       = %i %i %i \n",i-tile_w, j-tile_s, nn)  ;
//            printf(" tilelist_new  = %p\n",(void *)tile_array[nn])  ;
            gclose(fp) ;
          }
        }
      }
      return 0;
}

static TileListNode *tiledata_new(int i, int j)
{
  TileListNode *tiledata ;


      tiledata = (TileListNode *)malloc(sizeof(TileListNode)) ;
      tiledata->next      = NULL ;
      tiledata->tilex     = i ;
      tiledata->tiley     = j ;
      tiledata->zoom      = 0 ;
      tiledata->name      = NULL ;
      tiledata->t_file    = NULL ;
      tiledata->t_found   = 0 ;
      tiledata->loaded    = 0 ;
      tiledata->needed    = 0 ;
      tiledata->gl_display_list = 0  ;

      tiledata->terrain_data.terrain_errthreshold_scale = 0.0 ;
      tiledata->terrain_data.terrain_water_height_offset_SW = 0.0 ;
      tiledata->terrain_data.terrain_water_height_offset_SE = 0.0 ;
      tiledata->terrain_data.terrain_water_height_offset_NE = 0.0 ;
      tiledata->terrain_data.terrain_water_height_offset_NW = 0.0 ;
      tiledata->terrain_data.terrain_alwaysselect_maxdist   = 0.0 ;
      tiledata->terrain_data.terrain_nsamples               = 0   ;
      tiledata->terrain_data.terrain_sample_rotation        = 0.0 ;
      tiledata->terrain_data.terrain_sample_floor           = 0.0 ;
      tiledata->terrain_data.terrain_sample_scale           = 0.0 ;
      tiledata->terrain_data.terrain_sample_size            = 0.0 ;
      tiledata->terrain_data.ybuffer                        = NULL ;
      tiledata->terrain_data.ebuffer                        = NULL ;
      tiledata->terrain_data.nbuffer                        = NULL ;
      tiledata->terrain_data.asbuffer                       = 0   ;
      tiledata->terrain_data.fbuffer                        = 0   ;
      tiledata->terrain_data.number_of_shaders              = 0   ;
      tiledata->terrain_data.number_of_patchsets            = 0   ;
      tiledata->terrain_data.tile_shader                    = NULL ;
      tiledata->terrain_data.tile_patchset                  = NULL ;
      tiledata->terrain_data.elevations                     = NULL ;
#ifdef use_vertex_arrays
      tiledata->terrain_data.va_vertex  = NULL ;
      tiledata->terrain_data.va_normal  = NULL ;
      tiledata->terrain_data.va_texture = NULL ;
      tiledata->terrain_data.nva_index1 = 0    ;
      tiledata->terrain_data.va_index1  = NULL ;
      tiledata->terrain_data.nva_index2 = 0    ;
      tiledata->terrain_data.va_index2  = NULL ;
      tiledata->terrain_data.nbx  = 0 ;
      tiledata->terrain_data.nby  = 0 ;
      tiledata->terrain_data.va_node    = NULL ;
#endif
      return tiledata ;
}


/*
 * *****************************************************************************
 *
 *  TileToFileName()
 *  Routine to convert a tile index to the core name of a tile file.
 *
 *  This is based on routine 'FromTileXZ' in
 *                                      "System/OrtsFormatsMsts/TerrainFile.cs".
 *
 *  The scheme initially splits to world of tiles into a 2 by 2 checkerboard
 *  style test region and then generates a 2-bit flag.  Bit 2 is set if the
 *  tile lies in an first row (row 0 in 'C') of the test region and bit 1 is
 *  set if the tile lies in a column of different parity (i.e. row 1 and
 *  column 0 or visa versa).
 *     The size of the grid is then halved and, if necessary, the origin moved
 *  so that the tile lies within the new 2 by 2 test region.  The process is
 *  then repeated.
 *     After every two subdivisions the four bits generated are converted to a
 *  hexadecimal character which is added to the name.
 *     The number of subdivisions depends on the parameter zoom.  This has values:
 *       zoom =
 *          DMLarge = 11,  for 32km by 32km tiles
 *          DMSmall = 12,  for 16km by 16km tiles
 *                    13,  for  8km by 8km (Not Used)
 *          Large   = 14,  for 4km by 4km tiles
 *          Small   = 15,  for 2km by 2km tiles
 *     If zoom is odd the name starts with a '-', otherwise '_'.
 *
 * *****************************************************************************
 */

char *TileToFileName(int tilex, int tiley, int zoom)
{
  int  i  ;                   // Loop index for sub-divisions
  int  ic    =  0     ;       // Character count for name
  int  rectx = -16384 ;       // Southern limit of test region
  int  rectz = -16384 ;       // Western limit of test region
  int  rectw =  16384 ;       // Half width of test region
  int  recth =  16384 ;       // Half height of test region
  int  bit4  =  0     ;       // Integer to contain 4-bit number
  int  east           ;       // True if tile in eastern half of test region
  int  north          ;       // True if tile in northern half of test region
  char name[10]       ;       //
  char *string        ;       // Pointer for new string returned by routine
  char hex[] = "0123456789abcdef" ; //Used for bit4 to hex conversion

      name[ic++] = (zoom % 2) ? '-' : '_'  ;  // Set first character
      for (i = 0; i < zoom; i++){
          east  = (tilex >= rectx + rectw) ;  //  True if tile is in the east
          north = (tiley >= rectz + recth) ;  //  True if tile is in the north
          bit4 <<= 2 ;
          bit4 += (north ? 0 : 2) + (east ^ north ? 0 : 1) ;
          if (i % 2 == 1){
              name[ic++]= hex[bit4] ;
              bit4 = 0 ;
          }
          if (east)  rectx += rectw ;        //  Shift test region if east
          if (north) rectz += recth ;        //  Shift test region if north
          rectw /= 2 ;                       //  Halve size of test region
          recth /= 2 ;
      }
      if (zoom % 2 == 1) name[ic++]= hex[bit4 <<= 2] ;  // Shift to allow for missing 2-bit
      name[ic] = '\0' ;                      //  Add '\0' to denote end of string

      string = (char *)malloc(strlen(name)); //  New memory to contain filename
      return strcpy(string,name) ;
}

int FileNameToTile(char *file, int *tile_x, int *tile_y, int *zoo_m){
  int  i, j, n ;
  int  tilex   ;
  int  tiley   ;
  int  zoom, iodd   ;
  int  rectx = -16384 ;       // Southern limit of test region
  int  recty = -16384 ;       // Western limit of test region
  int  rectw =  16384 ;       // Half width of test region
  int  recth =  16384 ;       // Half height of test region
  char c      ;
  char hex[]     = "0123456789abcdef" ;
  char my_name[] = "FileNameToTile"   ;

      *tile_x = *tile_y = *zoo_m = 16385 ;
// Find length of name or length of 'base name' before '.'.
      n = 0 ;
      while(file[n+1] != '\0' && file[n+1] != '.') n++ ;
      zoom = n*2 ;
      iodd  = (file[0] == '-') ? 1 : 0 ;
      zoom = zoom - iodd ;

      tilex = rectx ;
      tiley = recty ;

      for(i=1;i<n+1;i++){
        c = file[i] ;
        if(NULL==strchr(hex,c)){
          printf(" Routine %s : ERROR : Tile filename contains illegal"
                 " character : Filename = %s\n",my_name,file) ;
          return 1 ;
        }
        for(j=0;j<16;j++) if(hex[j]==c) break ;
        if(!(j & 8)){
          tiley = tiley + recth ;
          if( (j & 4)) tilex = tilex + rectw ;
        }else{
          if(!(j & 4)) tilex = tilex + rectw ;
        }
        rectw = rectw/2 ;
        recth = recth/2 ;

        if(i == n && iodd) break ;

        if(!(j & 2)){
          tiley = tiley + recth ;
          if( (j & 1)) tilex = tilex + rectw ;
        }else{
          if(!(j & 1)) tilex = tilex + rectw ;
        }
        rectw = rectw/2 ;
        recth = recth/2 ;
      }

      *tile_x = tilex ;
      *tile_y = tiley ;
      *zoo_m  = zoom ;
      return 0;
}

/*
 *  Routine tilelistinit2
 *
 *  Routine to open directory TILES and find the tile *.y files
 *    saving the file names and tile positions in a TileList
 *    structure.
 *
 */
int tilelist_init2(){

  int    i, k, len1, len2, iret ;
  int    ip = 0 ;
  char   *tdir_name      ;
  DIR    *tdir = NULL    ;
  struct dirent *f_entry ;
  TileListNode  *tl_node   ;
  char   my_name[] = "tilelist_init2" ;

/*
 *  1.  Search for TILES directory
 */
      len1 = strlen(ORroutedir) + strlen("TILES/") + 1 ;
      tdir_name = (char *)malloc(len1) ;
      strcpy(tdir_name,ORroutedir)     ;
      strcat(tdir_name,"TILES/")       ;
      if(ip)printf(" Trying directory TILES = %s\n",tdir_name) ;
      iret = zr_find_msfile2(tdir_name) ;
      if(!iret)tdir = opendir(tdir_name) ;
      if(iret || tdir == NULL){
        printf(" Routine %s : ERROR : Unable to open TILES directory\n",
                                                               my_name) ;
        printf("    Directory name = %s\n", tdir_name) ;
        printf("    Program stopping ...\n") ;
        exit(1) ;
      }
/*
 *  2.  Loop throught files
 */
      while ((f_entry = readdir(tdir)) != NULL) {
        len2 = strlen(f_entry->d_name) ;
        if(strcmp(".t",&(f_entry->d_name[len2-2]))!= 0) continue ;
        if(ip)printf("  Found tile file = %s\n", f_entry->d_name);
/*
 *  Create new TileListNode
 */
        tl_node = tiledata_new(0,0) ;

        tl_node->name   = (char *)malloc((len2-1)) ;
        tl_node->t_file = (char *)malloc((len1+len2)) ;

        strcpy(tl_node->t_file, tdir_name) ;
        strcat(tl_node->t_file, f_entry->d_name) ;
        strncpy(tl_node->name, f_entry->d_name,len2-2) ;
        tl_node->name[len2-2] = '\0' ;

        if(ip)printf("    Name     = %s\n", tl_node->name) ;
        if(ip)printf("    Filename = %s\n", tl_node->t_file) ;

        FileNameToTile(tl_node->name,  &tl_node->tilex,
                      &tl_node->tiley, &tl_node->zoom) ;
        if(ip)printf("    tilex    = %i\n", tl_node->tilex) ;
        if(ip)printf("    tiley    = %i\n", tl_node->tiley) ;
        if(ip)printf("    zoom     = %i\n", tl_node->zoom)  ;

        tl_node->t_found   = 1     ;
        tl_node->next      = tilelist_head ;
        tilelist_head    = tl_node ;
      }
      tile_west  =  16384 ;
      tile_east  = -16384 ;
      tile_south =  16384 ;
      tile_north = -16384 ;

      for(tl_node = tilelist_head; tl_node != NULL; tl_node=tl_node->next){
        if(tl_node->tilex < tile_west ) tile_west  = tl_node->tilex ;
        if(tl_node->tilex > tile_east ) tile_east  = tl_node->tilex ;
        if(tl_node->tiley < tile_south) tile_south = tl_node->tiley ;
        if(tl_node->tiley > tile_north) tile_north = tl_node->tiley ;
        if(ip)printf("    Tiles    = %s   :: %i %i ::   %i %i %i %i\n",
                          tl_node->name, tl_node->tilex, tl_node->tiley,
                          tile_west, tile_east, tile_south, tile_north) ;
      }
      printf("    tile_west    = %i\n", tile_west) ;
      printf("    tile_east    = %i\n", tile_east) ;
      printf("    tile_south   = %i\n", tile_south) ;
      printf("    tile_north   = %i\n", tile_north) ;
      tile_x0 = tile_west  ;
      tile_y0 = tile_south ;
/*
 *  Fill tile array
 */
      len1 = (tile_east - tile_west + 1)*(tile_north - tile_south + 1) ;
      tile_array = (TileListNode **)malloc(len1*sizeof(TileListNode *)) ;
      tile_array_num = len1 ;

      for(i=1;i<len1;i++) tile_array[i] = NULL ;

      for(tl_node = tilelist_head; tl_node != NULL; tl_node=tl_node->next){
        k = (tl_node->tiley-tile_south)*(tile_east - tile_west + 1)
            + tl_node->tilex-tile_west ;
        tile_array[k] = tl_node ;
      }

      free(tdir_name) ;
      closedir(tdir) ;
      return 0 ;
}
