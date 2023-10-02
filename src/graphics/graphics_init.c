/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  graphics_init.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to initialise graphics and to control the amount of data
 *     held by the graphics card.
 *
 *   If the graphics card is loaded with too many textures and/or display lists
 *     it can result in objects being generated without their required textures.
 *   OpenRails appear to overcome this problem by minimising the amount of data
 *     stored on the graphics card.  It also appears to minimise the data kept
 *     in main memory - probably because some computers are memory limited.
 *   ZR presently assumes that main memory is not a problem, so all shapes and
 *     textures found are loaded into memory ready for use.
 *   Here the loading and culling of data is controlled by routine graphics_reset.
 *     Routine mark_shapes is used to identify which shapes are needed for
 *     the current viewpoint.  Routine mark_textures then identifies which
 *     textures are needed.  Textures and display lists not longer required
 *     are the culled after which any new textures and display lists are
 *     generated and loaded.
 *
 *==============================================================================
 */

/*
 *==============================================================================
 * Routine to initialise OpenGL graphics and useful constants
 *==============================================================================
 */

void graphics_init(void)
{

int      ip = 2  ;  //  0 = no printing  1, 2 = printing
int      i, j ;
int      ivec[10] ;
GLfloat  l0_amb[]  = {light0_ambi, light0_ambi, light0_ambi, 1.0} ;  //  Value in shadow
GLfloat  l0_dif[]  = {light0_diff, light0_diff, light0_diff, 1.0} ;  //  Value lighted
GLfloat  l0_spc[]  = {light0_spec, light0_spec, light0_spec, 1.0} ;  //  Highlights
float    v4[4] ;
char     my_name[] = "graphics_init" ;

      if(ip){
        printf("***********************************************************\n") ;
        printf(" INITIALISE GRAPHICS\n") ;
        printf("***********************************************************\n") ;
      }
/*
 *  Determine limits
 */
       glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, ivec) ;
       printf("  GL_MAX_ELEMENTS_VERTICES = %i\n",ivec[0]) ;
       glGetIntegerv(GL_MAX_ELEMENTS_INDICES, ivec) ;
       printf("  GL_MAX_ELEMENTS_INDICES  = %i\n",ivec[0]) ;

/*
 *   In case of changes : re-initialise viewport variables
 */
      viewport_width  = main_window_width  ;
      viewport_height = main_window_height ;
      viewport_x0     = main_window_x0     ;
      viewport_y0     = main_window_y0     ;
#if defined _Display_Shapes || defined _Display_Wagons
      viewport_near = 1.0/plot_scale ;
      viewport_far  = 4096.0/plot_scale ;
#endif

/*
 *  Set origin for plotting
 *  This is placed at a local tile intersection to reduce rounding
 *  errors during OpenGL (and other) matrix calculations.  Graphics cards
 *  usually only support 32 bit floating point operations.
 *
 *  Use SW corner
 */
      tile_x0 = tile_west  ;
      tile_y0 = tile_south ;
      tile_h0 = 100.0*floor(trk_min_height/100.0) ;
      if(tile_h0 < 0.0)tile_h0 = 0.0 ;
      if(ip)printf("   Origin for 3D graphics:  Tile = %i, %i.  Height = %.2f m.\n",
                                          tile_x0, tile_y0, tile_h0) ;
/*
 *  Initialise display sub-windows
 */
      init_train_operations_window(1) ;
/*
 *  Initialise cameras
 *
 *  This section is needed to initialise the eye position before
 *       the required shapes and textures are identified.
 *  If there is no route defined and no player train,
 *       then use a default position based on the RouteStart values.
 */
      cameras_init() ;
/*
 *  Start with camera  '0'
 */
      current_camera = 0 ;
#ifdef NO_ROUTE_DEF
      if(player_train){
        current_camera = 1 ;
      }else{
        offset_center_x = project_db.RouteStart[0] - tile_x0 + project_db.RouteStart[2]/plot_scale ;
        offset_center_y = project_db.RouteStart[1] - tile_y0 + project_db.RouteStart[3]/plot_scale ;
        offset_center_z = 0.15 ;
        offset_eye_x    = offset_center_x - 0.1 ;
        offset_eye_y    = offset_center_y - 0.1 ;
        offset_eye_z    = 0.2 ;
      }
#endif
      camera_changed = 1 ;
      camera_new_position() ;
/*
 *   Initialise Lighting
 *   Defaults for glLightModel are
 *       Two sided lighting - off,
 *       Background ambient light = (0.2, 0.2, 0.2, 1.0)
 *       Local specular reflection - off
 *       Separate specular colour  - off
 *
 *  Define light0
 */
      zr_setp4(v4,light0_altde,light0_polar) ;
      glLightfv(GL_LIGHT0,GL_POSITION,v4) ;

      glLightfv(GL_LIGHT0, GL_AMBIENT,  l0_amb) ;
      glLightfv(GL_LIGHT0, GL_DIFFUSE,  l0_dif) ;
      glLightfv(GL_LIGHT0, GL_SPECULAR, l0_spc) ;

      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);

      glEnable(GL_DEPTH_TEST);
      glShadeModel(GL_SMOOTH) ;
//      glShadeModel(GL_FLAT) ;
#if  defined _Display_Normal && ! defined sketch_tracks_and_roads
GLfloat fogColor[4] = {0.8, 0.9, 1.0, 1.0} ;  // Slight blue grey?

      glEnable(GL_FOG) ;
      glHint(GL_FOG_HINT,GL_FASTEST) ;
      glFogfv(GL_FOG_COLOR,fogColor) ;
      glFogi(GL_FOG_MODE,GL_LINEAR) ;
      glFogf(GL_FOG_DENSITY,0.5) ;
      glFogf(GL_FOG_START,0.20)   ;
      glFogf(GL_FOG_END,1.0)     ;
#endif

#if 0
/*
 *  Initialise shapes and textures
 *
 *   Not required - the same calls are made by the camera routine
 *                  camera_new_position which calls graphics_reset.
 */
      if(ip)printf("   Load required shapes and textures\n") ;
      if(ip>1)printf("   Call clear_needed_flags\n") ;
      clear_needed_flags()     ;
#ifndef _Display_Textures
      if(ip>1)printf(" Call mark_shapes\n") ;
      mark_shapes()            ;
#endif
      if(ip>1)printf(" Call mark_textures\n") ;
      mark_textures()          ;
      if(ip>1)printf(" Call load_needed_textures\n") ;
      load_needed_textures()   ;
      if(ip>1)printf(" Call load_needed_display_lists\n") ;
//      load_needed_display_lists() ;
#endif
/*
 *  Generate Topography Display List for each Tile
 */
#ifdef _Display_Normal
      if(ip)printf("   Make tile display lists\n");
//      make_tile_topog_display_lists() ;
# ifdef use_vertex_arrays
      make_tile_vertex_arrays()       ;
# else
      load_topography_display_lists() ;
# endif

#endif
/*
 *  Initialise display lists for track sections when
 *    no shapes are provided in the shape files.
 */
#if defined _Display_Normal  && 0
      if(ip)printf("   Make track display lists\n");
      make_track_display_lists();
      if(ip)printf("   Make road display lists\n");
      make_road_display_lists();
#endif
/*
 *  Initialise display lists for dynamic tracks
 */
#if defined _Display_Normal && 1
      if(ip)printf("   Make dynamic track display lists\n");
      make_dynamic_display_lists();
#endif

      if(ip){
        printf("  Exit graphics_init\n");
        printf("***********************************************************\n") ;
      }

      return  ;
}


/*
 *==============================================================================
 *  Main cull routine calls each sub-routine in turn
 *==============================================================================
 */
void graphics_reset(void) {

int  ip = 0 ;                       //  = 0 for no debug printing
char *my_name = "graphics_reset" ;

      if(ip)printf("  ===================================================\n");
      if(ip)printf("  Enter %s\n",my_name);
#if defined _Display_Normal
//      tile_eye_x0 = tile_x0 + lookat_eye_x ;
//      tile_eye_y0 = tile_y0 + lookat_eye_y ;
      if(ip)printf("   Routine  %s : Call clear_needed_flags\n",my_name) ;
      clear_needed_flags()     ;
      if(ip)printf("   Routine  %s : Call mark_shapes\n",my_name) ;
      mark_shapes()            ;
      if(ip)printf("   Routine  %s : Call mark_textures\n",my_name) ;
      mark_textures()          ;
      if(ip)printf("   Routine  %s : Call cull_topography_display_lists\n",my_name) ;
      cull_topography_display_lists() ;
      if(ip)printf("   Routine  %s : Call cull_textures\n",my_name) ;
      cull_textures()          ;
      if(ip)printf("   Routine  %s : Call cull_display_lists\n",my_name) ;
      cull_display_lists()     ;
/*
 *  Load needed
 */
      if(ip)printf("   Routine  %s : Call load_needed_textures\n",my_name) ;
      load_needed_textures()   ;
      if(ip)printf("   Routine  %s : Call load_topography_display_lists\n",my_name) ;
      load_topography_display_lists() ;
//      if(ip)printf("   Routine  %s : Call load_needed_display_lists\n",my_name) ;
//      load_needed_display_lists() ;
      if(ip)printf("   Exit %s\n",my_name) ;
#else
#ifndef _Display_Textures
      if(ip)printf("   Routine  %s : Call mark_shapes\n",my_name) ;
      mark_shapes()            ;
#endif
      if(ip)printf("   Routine  %s : Call mark_textures\n",my_name) ;
      mark_textures()          ;
      if(ip)printf("   Routine  %s : Call load_needed_textures\n",my_name) ;
      load_needed_textures()   ;

#endif

      return ;
}

/*
 *==============================================================================
 *  Routine to clear all the 'needed' flags in the
 *    shape and texture data structures
 *  before further processing
 *==============================================================================
 */
void clear_needed_flags(void)
{
int            ip = 0 ;
ShapeNode      *snode  ;
TextureNode    *tx_node  ;

      if(ip)printf("   Enter clear_needed_flags\n");
      for(snode=shapelist_beg; snode!=NULL; snode=snode->next){
          snode->needed = snode->basic ;
      }
      for(tx_node=texturelist_beg; tx_node!=NULL; tx_node=tx_node->next){
          tx_node->needed = tx_node->basic ;
      }
#if 0
      for(snode=wshapelist_beg; snode!=NULL; snode=snode->next){
        snode->needed = 0 ;
      }
      for(tx_node=wtexturelist_beg; tx_node!=NULL; tx_node=tx_node->next){
        tx_node->needed = 0 ;
      }
#endif
      return ;
}

/*
 *==============================================================================
 *  Routine to mark all shapes needed for display.
 *    In principal there are a number of possible useful options, for example:
 *    1.  All objects in the 9 tiles around the viewpoint (Openrails option?)
 *    2.  All objects in (1), within the viewport or close by.
 *    3.  All objects in directories of the current route
 *    4.  All objects in the MSTS directories.
 *  For the moment only (1) is coded
 *==============================================================================
 */

void mark_shapes(void)
{
int         ip  = 0 ;
int         idw = 0,
            idi = 0 ;
WorldNode   *wnode ;
WorldItem   *witem ;
ShapeNode   *snode ;
char        my_name[] = "mark_shapes" ;

      if(ip){
        printf("   Enter mark_shapes\n");
        printf("     tile_x = %i %i %.3f %i\n",tile_eye_x0 - tile_x0,
                                      tile_x0, lookat_eye_x, tile_eye_x0) ;
        printf("     tile_y = %i %i %.3f %i\n",tile_eye_y0 - tile_y0,
                                      tile_y0, lookat_eye_y, tile_eye_y0) ;
      }
#if defined _Display_Normal
      if(ip)printf(" Display Normal\n") ;
/*
 *  Option (1) Process tiles around viewpoint
 */
      for(wnode = worldlist_beg;wnode!=NULL;wnode=wnode->next){
        wnode->in_use = use_tile(wnode->tile_x, wnode->tile_y) ;
        idw = ip && (wnode->tile_x == -12583) && (wnode->tile_y == 14768) ;
/*
 * Loop though world items and mark as needed
 */
        for(witem = wnode->world_item; witem!=NULL;witem = witem->next){
          idi = idw && (witem->uid == 4485) ;
          snode = witem->snode ;
          if(snode != NULL){
            if(0 == snode->needed){
              snode->needed = 1;
              if(0 == ip) continue ;
              if(snode->name == NULL){
                if(ip)printf("     Set needed for shape :: NONAME\n") ;
              }else{
                if(ip)printf("     Set needed for shape :: %s\n",snode->name) ;
              }
            }
          }
          if(idi)printf("  shape %s,  needed = %i\n",snode->name, snode->needed) ;
        }
      }


// For options -D_Display_Shapes and  -D_Display_Wagons mark all as needed
#elif defined _Display_Shapes
      if(ip)printf(" Display Shapes\n") ;
      for(snode= shapelist_beg; snode!=NULL;snode=snode->next){
        snode->needed = 1 ;
//        printf(" Shape %s.  Needed = %i\n",snode->name, snode->needed) ;
      }
#elif defined _Display_Wagons
      if(ip)printf(" Display Wagons\n") ;
      for(snode= wshapelist_beg; snode!=NULL;snode=snode->next){
        snode->needed = 1 ;
//        printf(" Wagon %s.  Needed = %i\n",snode->name, snode->needed) ;
      }
#endif
      return ;
}

/*
 *==============================================================================
 *  Routine to mark textures needed for display.
 *    Routine loops through shapes and marks each required textue
 *==============================================================================
 */

void mark_textures(void)
{
ShapeNode   *snode  ;
TextureNode *tx_node  ;
int         i, n    ;
int         ip = 0  ;    // Debug printing

#ifndef _Display_Textures
      if(ip)printf("   Enter mark_textures\n");
      if(ip)printf("   Process shape textures\n");
      for(snode=shapelist_beg; snode!=NULL; snode=snode->next){
        if(0 == snode->needed) continue ;
        if(ip>1)printf("     Shape  %s, s_file %s\n",snode->name, snode->s_file) ;
        n = snode->n_textures ;
        for(i=0; i<n; i++){
          if(ip>1)printf("     %i, %i ::  %p  ::  %s\n",
                 i, n, (void *)snode->texture[i], snode->texture[i]->name) ;
          if(NULL == snode->texture[i]) continue;
          tx_node = snode->texture[i] ;
          tx_node->needed = 1           ;
          if(ip==1)printf("    Texture  %i, %i ::  %s\n",i, n, tx_node->name) ;
        }
      }
      if(ip)printf("   Process wagon textures\n");
      for(snode=wshapelist_beg; snode!=NULL; snode=snode->next){
        if(0 == snode->needed) continue ;
        if(ip>1)printf("     Wagon shape  %s  needs textures:\n",snode->name) ;
        n = snode->n_textures ;
        for(i=0; i<n; i++){
          if(ip>1)printf("     %i, %i ::  %p\n",i, n, (void *)snode->texture[i]) ;
          if(NULL == snode->texture[i]) continue;
          tx_node = snode->texture[i] ;
          tx_node->needed = 1           ;
          if(ip)printf("     %i, %i ::  %s\n",i, n, tx_node->name) ;
        }
      }
      if(ip)printf("   Exit mark_textures\n");
#else
      for(tx_node=texturelist_beg; tx_node!=NULL; tx_node=tx_node->next){
        tx_node->needed = 1 ;
      }
      for(tx_node=wtexturelist_beg; tx_node!=NULL; tx_node=tx_node->next){
        tx_node->needed = 1 ;
      }
#endif
      return ;
}
/*
 *==============================================================================
 *  Routine to remove from graphics card all textures no longer needed.
 *==============================================================================
 */
void cull_textures(void)
{
TextureNode *tx_node ;
int         ip = 0 ;

      if(ip)printf("   Enter cull_textures\n");
      for(tx_node=texturelist_beg; tx_node!=NULL; tx_node=tx_node->next){
        if(ip)printf("     Cull texture AA  ref %i, need %i, load %i, name  %s\n",
                      tx_node->gl_tex_ref_no, 1 == tx_node->needed,
                     1 == tx_node->loaded, tx_node->name) ;
        if(1 == tx_node->needed || 0 == tx_node->loaded) continue ;
        if(ip)printf("     Cull texture BB %i  %s\n",
                                          tx_node->gl_tex_ref_no,tx_node->name) ;
        glDeleteTextures((GLsizei) 1, &(tx_node->gl_tex_ref_no) ) ;
        tx_node->gl_tex_ref_no = 0 ;
        tx_node->loaded = 0 ;
      }
#if 0
      for(tx_node=wtexturelist_beg; tx_node!=NULL; tx_node=tx_node->next){
        if(1 == tx_node->needed || 0 == tx_node->loaded) continue ;
        if(ip>1)printf("     Cull texture BB %i  %s\n",
                                           tx_node->gl_tex_ref_no,tx_node->name) ;
        glDeleteTextures((GLsizei) 1, &(tx_node->gl_tex_ref_no) ) ;
        tx_node->loaded = 0 ;
      }
#endif
      return ;
}
/*
 *==============================================================================
 *  Routine to remove from graphics card all display lists no longer needed.
 *  This is probably not much use at the moment because display lists appear
 *  to be stored in main memory.  However something similar will be needed
 *  once index arrays are used.
 *==============================================================================
 */
void cull_display_lists(void)
{
ShapeNode      *snode        ;
LodControl     *lod_control  ;
DistLevel      *dist_level   ;
uint           i, j          ;
int            ip = 0        ;

      if(ip)printf("   Enter cull_display_lists\n");
      for(snode=shapelist_beg; snode!=NULL; snode=snode->next){
        if(1 == snode->needed || 0 == snode->loaded) continue ;
        if(ip>1)printf("     Cull static display list %s\n",snode->name) ;
        for(i=0; i < snode->n_lod_controls; i++){
          lod_control = &(snode->lod_control[i]) ;
          for(j=0; i < lod_control->n_dist_levels; i++){
            dist_level = &(lod_control->dist_level[j]) ;
            glDeleteLists(dist_level->gl_display_list, (GLsizei) 1)  ;
            dist_level->gl_display_list = 0        ;
          }
        }
        snode->loaded = 0 ;
      }
#if 0
      for(snode=wshapelist_beg; snode!=NULL; snode=snode->next){
        if(1 == snode->needed || 0 == snode->loaded) continue ;
        if(ip>1)printf("     Cull wagon display list %s\n",snode->name) ;
        for(i=0; i < snode->n_lod_controls; i++){
          lod_control = &(snode->lod_control[i]) ;
          for(j=0; i < lod_control->n_dist_levels; i++){
            dist_level = &(lod_control->dist_level[j]) ;
            glDeleteLists(dist_level->gl_display_list, (GLsizei) 1)  ;
            dist_level->gl_display_list = 0        ;
          }
        }
        snode->loaded = 0 ;
      }
#endif
      return ;
}
/*
 *==============================================================================
 *  Routine to remove from graphics card all topography no longer needed.
 *  This is probably not much use at the moment because display lists appear
 *  to be stored in main memory.  However something similar will be needed
 *  once index arrays are used.
 *==============================================================================
 */
void cull_topography_display_lists(void){

int           ix, iy ;
int           ip = 0 ;
TileListNode *tl_node ;
TerrainData  *tl_data ;

      if(ip)printf("   Enter cull_topography_display_lists\n");
      for(tl_node = tilelist_head; tl_node != NULL; tl_node=tl_node->next){
        if(0 == tl_node->needed)continue ;
#ifdef use_vertex_arrays
        tl_data = &(tl_node->terrain_data) ;
//        printf("   tile = %s  : vertex = %p :: %p\n",
//               tl_node->name, (void *)tl_data, (void *)tl_data->va_vertex) ;
        if(NULL == tl_data->va_vertex) continue ;
#else
        if(0 == tl_node->gl_display_list) continue ;
#endif
        ix = tl_node->tilex ;
        iy = tl_node->tiley ;
        if(use_tile(ix,iy)) continue ;
#ifdef use_vertex_arrays
# ifdef cull_vertex_arrays
        free(tl_data->va_vertex)  ; tl_data->va_vertex  = NULL ;
        free(tl_data->va_normal)  ; tl_data->va_normal  = NULL ;
        free(tl_data->va_texture) ; tl_data->va_texture = NULL ;
        free(tl_data->va_index1)  ; tl_data->va_index1  = NULL ;
# endif
#else
# ifdef cull_display_lists
        glDeleteLists(tl_node->gl_display_list, (GLsizei) 1)  ;
        tl_node->gl_display_list = 0 ;
# endif
#endif
        tl_node->needed          = 0 ;
        if(ip>1)printf("     Cull topog for tile %i %i :: %i\n", ix, iy,
                                                     use_tile(ix,iy)) ;
      }
      if(ip)printf("   Exit  cull_topography_display_lists\n");
      return ;
}
/*
 *==============================================================================
 *  Routine to load graphics card display with required topography
 *==============================================================================
 */
void load_topography_display_lists(void){

int          k, n, ix, iy, klist ;
int          ip = 0 ;
TileListNode *tl_node ;
VANode       * va_node ;

      if(ip)printf("   Enter load_topography_display_lists\n");
      if(ip)printf("     land_texture = %p\n",(void *)land_texture) ;
      for(tl_node = tilelist_head; tl_node != NULL; tl_node=tl_node->next){
        ix = tl_node->tilex ;
        iy = tl_node->tiley ;
        if(ip)printf("   Tile  %i  %i  use_tile = %i, vertex = %p\n",
           ix,iy,use_tile(ix,iy),(void *)tl_node->terrain_data.va_vertex) ;
        if((iy < 10261 || iy > 10265) && !use_tile(ix,iy)) continue ;     // Tile topography not needed
        tl_node->needed = 1 ;
#ifdef use_vertex_arrays
        if(NULL == tl_node->terrain_data.va_vertex){
          make_tile_vertex_array(tl_node) ;
        }else{
          n = tl_node->terrain_data.nbx*tl_node->terrain_data.nby ;
          for(k=0;k<n;k++){
            va_node = &(tl_node->terrain_data.va_node[k]) ;
            va_node->in_use = check_topog_in_scene2(va_node->xa,va_node->ya,va_node->za);
          }
        }
#else
        if(0 != tl_node->gl_display_list) continue ;
        klist = glGenLists(1) ;
        make_tile_topog_display_list(tl_node, klist) ;
        tl_node->gl_display_list = klist ;
#endif
        if(ip>1)printf("     Initialise topog for tile %i %i \n", ix, iy) ;
      }
      if(ip)printf("   Exit  load_topography_display_lists\n");
      return ;
}

/*
 *==============================================================================
 *  Routine to load graphics card with required textures.
 *==============================================================================
 */
void load_needed_textures(void)
{
TextureNode *tx_node  ;
int         ipp = 0,
            ip  = 0  ;

      if(ipp)printf("   Enter load_needed_textures\n");
      for(tx_node=texturelist_beg; tx_node!=NULL; tx_node=tx_node->next){
//        ip = ipp && (tx_node == land_texture) ;
//        ip = ipp && !strcmp(tx_node->name,"FootBrBrank") ;
        if(ip)printf("     Texture name = %s, needed = %i, loaded = %i\n",
                                tx_node->name,tx_node->needed, tx_node->loaded);
        if( ip && (tx_node == land_texture)){
          printf(" Routine load_needed_textures  ================\n") ;
          printf("  Texture = %s\n",tx_node->name) ;
          printf("   basic  = %i\n",tx_node->basic) ;
          printf("   needed = %i\n",tx_node->needed) ;
          printf("   loaded = %i\n",tx_node->loaded) ;
        }
        if(0 == tx_node->needed || 1 == tx_node->loaded) continue ;
        make_texture_resident(tx_node) ;
        tx_node->loaded = 1 ;
        if(ip)printf("       Texture name = %s, needed = %i, loaded = %i, pointer = %p\n",
                        tx_node->name,tx_node->needed, tx_node->loaded, (void *)tx_node );
      }

      for(tx_node=wtexturelist_beg; tx_node!=NULL; tx_node=tx_node->next){
//        ip = ipp && (tx_node == land_texture) ;
//        ip = ipp && !strcmp(tx_node->name,"FootBrBrank") ;
        if(ip)printf("     Texture name = %s, needed = %i, loaded = %i\n",
                                tx_node->name,tx_node->needed, tx_node->loaded);
        if(0 == tx_node->needed || 1 == tx_node->loaded) continue ;
        if(ip>1)printf("     Install wagon texture : %s\n",tx_node->name) ;
        make_texture_resident(tx_node) ;
        tx_node->loaded = 1 ;
        if(ip)printf("       Texture name = %s, needed = %i, loaded = %i, pointer = %p\n",
                        tx_node->name,tx_node->needed, tx_node->loaded, (void *)tx_node );
      }
      return ;
}
/*
 *==============================================================================
 *  Routine to load graphics card with required shape display lists.
 *==============================================================================
 */

void load_needed_display_lists(void)
{
ShapeNode  *snode ;
LodControl *lod_control ;
int        ip = 0 ;
uint       i, j   ;
int        ic = 0, id = 0, ie = 0 ;

      if(ip)printf("   Enter load_needed_display_lists\n");
      for(snode=shapelist_beg; snode!=NULL; snode=snode->next, ic++){
        if(ip>2){
          printf("     Check shape : %s,  needed = %i,  loaded = %i\n",
                 snode->name,snode->needed,snode->loaded) ;
          for(i=0;i<snode->n_lod_controls;i++){
            lod_control = &(snode->lod_control[i]) ;
            for(j=0;j<lod_control->n_dist_levels;j++){
              printf("    lod = %i,  dist = %i,  gl_display_list = %i\n",
                     i, j, lod_control->dist_level[j].gl_display_list) ;
            }
          }
        }
        if(snode->needed && snode->loaded){
         ie++ ;
         printf("  Display list ie = %i, shape = %s\n",ie,snode->name) ;
        }
        if(0 == snode->needed || 1 == snode->loaded) continue ;
        if(!strncmp(snode->name,"mark",4)) continue ;                //  ERROR
        if(ic<700)continue ;
        generate_shape_display_list(snode) ;
        snode->loaded = 1 ;
        id++ ;
        if(ip>1)printf("     Install shape : %s\n",snode->name) ;
        if(ip>2){
          printf("     Check shape : %s,  needed = %i,  loaded = %i\n",
                 snode->name,snode->needed,snode->loaded) ;
          for(i=0;i<snode->n_lod_controls;i++){
            lod_control = &(snode->lod_control[i]) ;
            for(j=0;j<lod_control->n_dist_levels;j++){
              printf("    lod = %i,  dist = %i,  gl_display_list = %i\n",
                     i, j, lod_control->dist_level[j].gl_display_list) ;
            }
          }
        }
      }
      printf("  DISPLAY LISTS ic = %i,  id = %i, ie = %i\n",ic,id,ie) ;
/*
 *  Wagons no longer use display lists
 */
#if 0
      for(snode=wshapelist_beg; snode!=NULL; snode=snode->next){
        if(ip)printf("  Shape name = %s, needed = %i, loaded = %i\n",snode->name,snode->needed, snode->loaded);
        if(0 == snode->needed || 1 == snode->loaded) continue ;
        generate_shape_display_list(snode) ;
        snode->loaded = 1 ;
        if(ip>1)printf("     Install wagon : %s\n",snode->name) ;
        if(ip)printf("  Shape name = %s, needed = %i, loaded = %i\n",snode->name,snode->needed, snode->loaded);
      }
#endif
      return ;
}
/*
 *   Function use_tile() returns 1 if a tile and its contents
 *     are to be used.  This occurs if tile_cull_r (graphics.h)
 *     is zero of if the distance from the eye tile is less than
 *     tile_cull_r.
 *
 *     ix, iy  absolute tile indices
 */

int use_tile(int ix, int iy){

int  i1, j1 ;

      if(0 == tile_cull_r) return 1 ;
#if 0
      printf("  tile_x, tile_y = %i %i   :: %i %i :: %i %i :: %i :: %i %i :: %i %i\n",
        ix,iy, tile_eye_x0, tile_x0, tile_eye_y0, tile_y0, tile_cull_r,
        (abs(ix-tile_eye_x0-tile_x0)),(abs(ix-tile_eye_x0-tile_x0)<tile_cull_r),
        (abs(iy-tile_eye_y0-tile_y0)),(abs(iy-tile_eye_y0-tile_y0)<tile_cull_r)
        );
#endif

      i1 = ix-tile_x0-tile_eye_x0 ; if(i1<0) i1 = -i1 ;
      j1 = iy-tile_y0-tile_eye_y0 ; if(j1<0) j1 = -j1 ;
      if ( (i1 < tile_cull_r) && (j1 < tile_cull_r) )return 1 ;
      return 0 ;
}
