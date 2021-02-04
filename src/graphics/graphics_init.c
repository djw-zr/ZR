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
 *   Here the loading and culling of data is controlled by routine graphics_cull.
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

int      ip = 1  ;  //  0 = no printing
GLfloat  l0_amb[]  = {light0_ambi, light0_ambi, light0_ambi, 1.0} ;  //  Value in shadow
GLfloat  l0_dif[]  = {light0_diff, light0_diff, light0_diff, 1.0} ;  //  Value lighted
GLfloat  l0_spc[]  = {light0_spec, light0_spec, light0_spec, 1.0} ;  //  Highlights
float    v4[4] ;

      if(ip){
        printf("***********************************************************\n") ;
        printf(" INITIALISE GRAPHICS\n") ;
        printf("***********************************************************\n") ;
      }
/*
 *  Initialise graphic variables
 */
      radian_to_north = atan2(lookat_centre_x-lookat_eye_x,
                              lookat_centre_y-lookat_eye_y) ;
      angle_to_north  = degree*radian_to_north  ;
      radian_to_up    = 0.5*M_PI - atan2( lookat_centre_z-lookat_eye_z,
                               sqrt(pow( lookat_centre_x-lookat_eye_x, 2)
                                   +pow( lookat_centre_y-lookat_eye_y, 2)) ) ;
      angle_to_up     = degree*radian_to_up  ;
#if 0
      lookat_centre_z = lookat_eye_z + cos(radian_to_up) ;
      lookat_centre_x = lookat_eye_x + sin(radian_to_up)*sin(radian_to_north) ;
      lookat_centre_y = lookat_eye_y + sin(radian_to_up)*cos(radian_to_north) ;
#endif
      clip_0[0]= clip_x ; clip_0[1]= clip_y ; clip_0[2]= clip_z ; clip_0[3]= clip_c ;
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
      if(ip)printf("   Origin for 3D graphics:  Tile = %i, %i.  Height = %.2f m.\n",
                                          tile_x0, tile_y0, tile_h0) ;
      tile_eye_x0 = tile_x0 + lookat_eye_x ;
      tile_eye_y0 = tile_y0 + lookat_eye_y ;
/*
 *   Initialise Lighting
 */
      GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
      GLfloat local_view[] = { 0.0 };
/*
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

/*
 *  Initialise shapes and textures
 */
      if(ip)printf("   Load required shapes and textures\n") ;
      if(ip>1)printf("   Call clear_needed_flags\n") ;
      clear_needed_flags()     ;
#ifndef _Display_Textures
      if(ip>1)printf(" Call mark_shapes\n") ;
      mark_shapes()            ;
#endif
      mark_textures()          ;
      load_needed_textures()   ;
      load_needed_display_lists() ;
/*
 *  Generate Topography Display List for each Tile
 */
#ifdef _Display_Normal
     if(ip)printf("   Make tile display lists\n");
//     make_tile_topog_display_lists() ;
     load_topography_display_lists() ;
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





      if(ip)printf("***********************************************************\n") ;

      return  ;
}


/*
 *==============================================================================
 *  Main cull routine calls each sub-routine in turn
 *==============================================================================
 */
void graphics_cull(void)
{
int  ip = 0                   ;  //  = 0 for no debug printing

      if(ip)printf("  Enter graphics_cull\n");
#if defined _Display_Normal
      tile_eye_x0 = tile_x0 + lookat_eye_x ;
      tile_eye_y0 = tile_y0 + lookat_eye_y ;
      if(ip)printf("   GC Call clear_needed_flags\n") ;
      clear_needed_flags()     ;
      if(ip)printf("   GC Call mark_shapes\n") ;
      mark_shapes()            ;
      mark_textures()          ;
      cull_topography_display_lists() ;
//      cull_textures()          ;
      cull_display_lists()     ;
      load_topography_display_lists() ;
      load_needed_textures()   ;
      load_needed_display_lists() ;
#endif

      return ;
}

/*
 *==============================================================================
 *  Routine to clear all the 'needed' flags in the shape and textures
 *    data structures before furtehr processing
 *==============================================================================
 */
void clear_needed_flags(void)
{
int            ip = 0 ;
ShapeNode      *snode  ;
TextureNode    *tnode  ;

      if(ip)printf("   Enter clear_needed_flags\n");
      for(snode=shapelist_beg; snode!=NULL; snode=snode->next){
          snode->needed = snode->basic ;
      }
      for(tnode=texturelist_beg; tnode!=NULL; tnode=tnode->next){
          tnode->needed = tnode->basic ;
      }
      for(snode=wshapelist_beg; snode!=NULL; snode=snode->next){
        snode->needed = 0 ;
      }
      for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
        tnode->needed = 0 ;
      }
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
int         ip = 0 ;
WorldNode   *wnode ;
WorldItem   *witem ;
ShapeNode   *snode ;
int tile_x = tile_x0 + lookat_eye_x ; //  i coordinate of central tile
int tile_y = tile_y0 + lookat_eye_y ; //  j coordinate of central tile

int it, jt ;

      if(ip){
        printf("   Enter mark_shapes\n");
        printf("     tile_x = %i %i %.3f %i\n",tile_x, tile_x0,
                                          lookat_eye_x, tile_eye_x0) ;
        printf("     tile_y = %i %i %.3f %i\n",tile_y, tile_y0,
                                          lookat_eye_y, tile_eye_y0) ;
      }
#if defined _Display_Normal
      if(ip)printf(" Display Normal\n") ;
/*
 *  Option (1) Process 9 tiles around viewpoint
 */
      for(wnode = worldlist_beg;wnode!=NULL;wnode=wnode->next){
        it = wnode->tile_x - tile_x ;
        jt = wnode->tile_y - tile_y ;
//        printf(" Processing world tile:  %i %i  ::  %i %i  ::  %i %i  ::  %i %i\n",
//               wnode->tile_x, wnode->tile_y, tile_x0,  tile_y0,
//               (int) lookat_eye_x, (int)lookat_eye_y, it, jt) ;
        wnode->in_use = 0 ;
        if(it<-1 || it>1 || jt<-1 || jt>1) continue ;  // Skip distant tiles
        wnode->in_use = 1 ;
        if(ip)printf("     Processing tile:  %i %i  ::  %i %i  ::  %i %i\n",
               it, jt, wnode->tile_x, wnode->tile_y, tile_x0,  tile_y0);
// Loop though world items and mark as needed
        for(witem = wnode->world_item; witem!=NULL;witem = witem->next){
          snode = witem->snode ;
          if(snode != NULL){
            if(0 == snode->needed){
              snode->needed = 1;
              if(0 == ip) continue ;
              if(snode->name == NULL){
                printf("     Set needed for shape :: NONAME\n") ;
              }else{
                printf("     Set needed for shape :: %s\n",snode->name) ;
              }
            }
          }
        }
      }
// For options -D_Display_Shapes and  -D_Display_Wagons mark all as needed
#elif defined _Display_Shapes
      printf(" Display Shapes\n") ;
      for(snode= shapelist_beg; snode!=NULL;snode=snode->next){
        snode->needed = 1 ;
//        printf(" Shape %s.  Needed = %i\n",snode->name, snode->needed) ;
      }
#elif defined _Display_Wagons
      printf(" Display Wagons\n") ;
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
TextureNode *tnode  ;
int         i, n    ;
int         ip = 0  ;    // Debug printing

#ifndef _Display_Textures
      if(ip)printf("   Enter mark_textures\n");
      if(ip)printf("   Process shape textures\n");
      for(snode=shapelist_beg; snode!=NULL; snode=snode->next){
        if(0 == snode->needed) continue ;
        if(ip>1)printf("     Shape  %s  needs textures:\n",snode->name) ;
        n = snode->n_textures ;
        for(i=0; i<n; i++){
          if(ip>1)printf("     %i, %i ::  %p\n",i, n, (void *)snode->texture[i]) ;
          if(NULL == snode->texture[i]) continue;
          tnode = snode->texture[i] ;
          tnode->needed = 1           ;
          if(ip)printf("     %i, %i ::  %s\n",i, n, tnode->name) ;
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
          tnode = snode->texture[i] ;
          tnode->needed = 1           ;
          if(ip)printf("     %i, %i ::  %s\n",i, n, tnode->name) ;
        }
      }
      if(ip)printf("   Exit mark_textures\n");
#else
      for(tnode=texturelist_beg; tnode!=NULL; tnode=tnode->next){
        tnode->needed = 1 ;
      }
      for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
        tnode->needed = 1 ;
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
TextureNode *tnode ;
int         ip = 0 ;

      if(ip)printf("   Enter cull_textures\n");
      for(tnode=texturelist_beg; tnode!=NULL; tnode=tnode->next){
        if(1 == tnode->needed || 0 == tnode->loaded) continue ;
        if(ip>1)printf("     Cull texture AA %i  %s\n",
                                          tnode->gl_tex_ref_no,tnode->name) ;
        glDeleteTextures((GLsizei) 1, &(tnode->gl_tex_ref_no) ) ;
        tnode->loaded = 0 ;
      }
      for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
        if(1 == tnode->needed || 0 == tnode->loaded) continue ;
        if(ip>1)printf("     Cull texture BB %i  %s\n",
                                           tnode->gl_tex_ref_no,tnode->name) ;
        glDeleteTextures((GLsizei) 1, &(tnode->gl_tex_ref_no) ) ;
        tnode->loaded = 0 ;
      }
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
int            i, j          ;
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
TileListNode *tnode ;

      if(ip)printf("   Enter cull_topography_display_lists\n");
      for(tnode = tilelist_head; tnode != NULL; tnode=tnode->next){
        if(0 == tnode->gl_display_list) continue ;
        ix = tnode->tilex ;
        iy = tnode->tiley ;
        if(use_tile(ix,iy)) continue ;
        glDeleteLists(tnode->gl_display_list, (GLsizei) 1)  ;
        tnode->needed          = 0 ;
        tnode->gl_display_list = 0 ;
        if(ip>1)printf(" Cull topog for tile %i %i :: %i\n", ix, iy,
                                                     use_tile(ix,iy)) ;
      }
      return ;
}
/*
 *==============================================================================
 *  Routine to load graphics card display with required topography
 *==============================================================================
 */
void load_topography_display_lists(void){

int          ix, iy, klist ;
int          ip = 0 ;
TileListNode *tnode ;

      if(ip)printf("   Enter load_topography_display_lists\n");
      if(ip)printf("   land_texture = %p\n",(void *)land_texture) ;
      for(tnode = tilelist_head; tnode != NULL; tnode=tnode->next){
        if(0 != tnode->gl_display_list) continue ;
        ix = tnode->tilex ;
        iy = tnode->tiley ;
        if(!use_tile(ix,iy)) continue ;

        klist = glGenLists(1) ;
        make_tile_topog_display_list(tnode, klist) ;

        tnode->needed          = 1     ;
        tnode->gl_display_list = klist ;
        if(ip>1)printf(" Load topog for tile %i %i :: %i\n", ix, iy,
                                                     use_tile(ix,iy)) ;
      }
      return ;
}

/*
 *==============================================================================
 *  Routine to load graphics card with required textures.
 *==============================================================================
 */
void load_needed_textures(void)
{
TextureNode    *tnode  ;
int            ip = 0 ;

      if(ip)printf("   Enter load_needed_textures\n");
      for(tnode=texturelist_beg; tnode!=NULL; tnode=tnode->next){
        if(0 == tnode->needed || 1 == tnode->loaded) continue ;
        if(ip)printf("     Install normal texture : %s\n",tnode->name) ;
        make_texture_resident(tnode) ;
        tnode->loaded = 1 ;
      }

      for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
        if(0 == tnode->needed || 1 == tnode->loaded) continue ;
        if(ip>1)printf("     Install wagon texture : %s\n",tnode->name) ;
        make_texture_resident(tnode) ;
        tnode->loaded = 1 ;
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
ShapeNode      *snode  ;
int            ip = 0 ;

      if(ip)printf("   Enter load_needed_display_lists\n");
      for(snode=shapelist_beg; snode!=NULL; snode=snode->next){
        if(0 == snode->needed || 1 == snode->loaded) continue ;
        generate_shape_display_list(snode) ;
        snode->loaded = 1 ;
        if(ip>1)printf("     Install shape : %s\n",snode->name) ;
      }

      for(snode=wshapelist_beg; snode!=NULL; snode=snode->next){
        if(0 == snode->needed || 1 == snode->loaded) continue ;
        generate_shape_display_list(snode) ;
        snode->loaded = 1 ;
        if(ip>1)printf("     Install wagon : %s\n",snode->name) ;
      }
      return ;
}
/*
 *   Function use_tile() returns 1 if a tile and its contents
 *     are to be used.  This occurs if tile_cull_r (graphics.h)
 *     is zero of if the distance from the eye tile is less than
 *     tile_cull_r.
 */

int use_tile(int ix, int iy){

      if(0 == tile_cull_r) return 1 ;
      return  abs(ix-tile_eye_x0)<tile_cull_r &&
              abs(iy-tile_eye_y0)<tile_cull_r ;
}
