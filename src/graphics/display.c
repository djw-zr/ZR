/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  display.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Contains routine display() and support routines
 *   Routine display() is called repeatedly to redraw the 3D scene
 *
 *   The graphics code is based on openglut amd OpenGL 2.1
 *
 *==============================================================================
 */


int plot_tile_outlines(void)   ;  // Plot outlines of all tiles
int sketch_road_routes(void)   ;  // Sketch routes of roads
int display_shapes(void)       ;  // Plot all shapes on a grid
int display_wagons(void)       ;  // Plot all wagons on a grid
int display_textures(void)     ;  // Plot all textures on a grid

int    i_display = 0  ;
double x_last, y_last ;

/*
 *==============================================================================
 *   Main display routine
 *==============================================================================
 */
void display(void){

  int      iret ;
  uint     i, j, k  ;
  int      ip = 0   ;  // DEBUG
  int      icount = 0 ;
  char     string[128];
  double   scalei = 1.0/plot_scale ;
  double   last_seconds ;
  TileListNode  *tl_node ;
  TrainNode     *train ;
  static double tile_time_used, shape_time_used, track_time_used,
                dtrack_time_used  ;
  char          *my_name = "display" ;
  struct timespec tv ;

#ifdef _Display_Normal

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
  clock_t       tile_t_beg,  tile_t_end  ;
  clock_t       shape_t_beg, shape_t_end ;
  clock_t       track_t_beg, track_t_end ;
  clock_t       dtrack_t_beg, dtrack_t_end ;
  int           n_tiles_plotted, n_shapes_plotted, n_tracks_plotted ;
#pragma GCC diagnostic pop

        tile_t_beg  = 0.0 ;
        tile_t_end  = 0.0 ;
        shape_t_beg = 0.0 ;
        shape_t_end = 0.0 ;
        track_t_beg = 0.0 ;
        track_t_end = 0.0 ;
        dtrack_t_beg = 0.0 ;
        dtrack_t_end = 0.0 ;
        n_tiles_plotted  = 1 ;
        n_shapes_plotted = 1 ;
        n_tracks_plotted = 1 ;
#endif

        i_display++ ;
        l_time_p2s = l_time_1s = l_time_5s = l_time_30s = 0 ;
        last_seconds = run_seconds ;

        clock_gettime(CLOCK_MONOTONIC, &run_clock1) ;
        run_seconds = (run_clock1.tv_sec - run_clock0.tv_sec)
                    + (run_clock1.tv_nsec - run_clock0.tv_nsec)*0.000000001 ;
        delta_seconds = run_seconds - last_seconds ;
/*
 *  Limit to 30 l_fps
 */
        delay_seconds = 0. ;
        if(delta_seconds < 0.033){
          delay_seconds = 0.033 - delta_seconds ;
          tv.tv_sec = 0 ;
          tv.tv_nsec = delay_seconds*1000000000 ;
          iret = nanosleep(&tv,NULL) ;
          clock_gettime(CLOCK_MONOTONIC, &run_clock1) ;
          run_seconds = (run_clock1.tv_sec - run_clock0.tv_sec)
                      + (run_clock1.tv_nsec - run_clock0.tv_nsec)*0.000000001 ;
          delta_seconds = run_seconds - last_seconds ;
        }
        run_seconds = fmod(run_seconds,1000000.0) ;  // Allow for repeat on 32 bit computers
        if(run_seconds - last_p2s > 0.2){
          l_time_p2s = 1 ;
          last_p2s = run_seconds ;

          if(run_seconds - last_1s > 1.0){
            l_time_1s = 1 ;
            last_1s = run_seconds ;

            if(run_seconds - last_5s > 5.0){
              l_time_5s = 1 ;
              last_5s = run_seconds ;

              if(run_seconds - last_30s > 30.0){
                l_time_30s = 1 ;
                last_30s = run_seconds ;
              }
            }
          }
        }
        check_glerror2("My routine 'display', AA AA\n") ;
#ifdef DISPLAY_5_SECONDS
        if(!l_time_5s) return ;
#endif

//  Note light0_ values can be modified by the keyboard (see keyboard.c).

GLfloat  mat_amb[]  = {light0_ambi, light0_ambi, light0_ambi, 1.0} ;  //  Value in shadow
GLfloat  mat_dif[]  = {light0_diff, light0_diff, light0_diff, 1.0} ;  //  Value lighted
GLfloat  mat_spc[]  = {light0_spec, light0_spec, light0_spec, 1.0} ;  //  Highlights
GLfloat  high_shininess[] = { 100.0 };
GLfloat  mid_shininess[]  = {  50.0 };
GLfloat  low_shininess[]  = {  10.0 };
GLfloat  v4[4] ;


      if(ip){
        printf(" ++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf(" Enter display :: %i\n",i_display++) ;
        printf(" ++++++++++++++++++++++++++++++++++++++++++++++++++\n");
      }
/*
 *  Start multi-processor section
 */
//#pragma omp parallel
{
//#pragma omp single
{
/*
 *  Start MP  task 1
 */
//#pragma omp section
{
#ifdef _Display_Normal
/*
 *  Update trains while land and shapes are being drawn
 */
      update_trains() ;  // Update using monotonic time
/*
 *  Update shapes while wagons are being drawn
 */
      update_level_crossings() ;
      update_turntable(current_turntable) ;
      update_signals() ;
 #ifdef OPENAL
      update_sounds()  ;
      update_train_sounds() ;
 #endif
/*
 *  Update water towers and other transfer points
 */
 #ifndef ROUTE_NEW_FOREST
      update_transfer() ;
 #endif
/*
 * Breakpoint between drawing land and shapes  ...
 *    ... and drawing wagons
 */
#endif
}
/*
 *  Start MP 2
 */
//#pragma omp section
{
/*
 *  If position has changed - update viewpoint
 */
      if(camera_changed || camera_moved)camera_new_position() ;
/*
 *   Initialise graphics
 *   Clear-colour normally set to light blue (sky)
 *      but to darker colour for displaying shapes and wagons
 */
#if defined _Display_Shapes || defined _Display_Wagons
      glClearColor((GLfloat) 0.1,(GLfloat) 0.2,(GLfloat) 0.0,(GLfloat) 0.0);
#else
      glClearColor((GLfloat) 0.8,(GLfloat) 0.9,(GLfloat) 1.0,(GLfloat) 0.0);
#endif

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDepthRange(0.0,1.0);

      glEnable(GL_DEPTH_TEST)    ;
      glDisable(GL_LINE_STIPPLE) ;
      glDisable(GL_TEXTURE_2D)   ;
      glColor3f(0.0,0.0,0.0) ;
      glPolygonOffset(0.0,0.0) ;
/*
 *  Define light0
 */
      zr_setp4(v4,light0_altde,light0_polar) ;
      glLightfv(GL_LIGHT0,GL_POSITION,v4) ;
/*
 *  Define default material
 */
      glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat_amb)  ;
      glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat_dif)  ;
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_spc) ;
      glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS, high_shininess) ;
/*
 *==============================================================================
 * Draw Outline of Tiles
 *==============================================================================
 */
#if defined grid_lines      || defined _Display_Shapes \
 || defined _Display_Wagons || defined _Display_Textures
      plot_tile_outlines() ;
#endif
/*
 *==============================================================================
 *   Display Shapes in a group
 *==============================================================================
 */
#ifdef _Display_Shapes
      display_shapes() ;
/*
 *==============================================================================
 *   Display Wagons
 *==============================================================================
 */
#elif defined _Display_Wagons
      display_wagons() ;
/*
 *==============================================================================
 *   Display Textures
 *==============================================================================
 */
#elif defined _Display_Textures
      display_textures() ;
/*
 *==============================================================================
 *  Display full 3D scene
 *==============================================================================
 */
#elif defined _Display_Normal
      glLineWidth(3.0) ;
/*
 *    Sketch routes of roads and tracks
 */
#ifdef sketch_tracks_and_roads
      track_t_beg = clock() ;
      sketch_track_routes(1) ;
      sketch_road_routes() ;
      track_t_end = clock() ;
      n_tracks_plotted = 1 ;
#endif
/*
 *==============================================================================
 *   Display tiles
 *==============================================================================
 */
#if 1
      tile_t_beg = clock() ;
      n_tiles_plotted = 0  ;

      glShadeModel(GL_SMOOTH) ;
//      glShadeModel(GL_FLAT) ;
      glEnable(GL_LIGHTING);

      glCullFace(GL_BACK) ;
//      glDisable(GL_CULL_FACE) ;  //  Needed for some AU_NSW Routes !!
//      glEnable(GL_CULL_FACE) ;

      glEnable(GL_TEXTURE_2D) ;
//      glEnable(GL_BLEND) ;
      glDisable(GL_BLEND) ;

      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;

#ifndef no_vertex_arrays
/*
 *   Enable Vertex Arrays
 */
      glEnableClientState(GL_VERTEX_ARRAY) ;
      glEnableClientState(GL_NORMAL_ARRAY) ;
      glEnableClientState(GL_TEXTURE_COORD_ARRAY) ;
#endif
      n_vanodes_d = 0 ;
      for(tl_node = tilelist_head; tl_node != NULL; tl_node=tl_node->next){
        if(0==tl_node->needed) continue ;
        display_topog_water(tl_node) ;
#ifndef no_vertex_arrays
        if(ip)printf("  Calling display_tile_vertex_array\n") ;
        display_tile_vertex_array(tl_node) ;
#else
        glCallList((GLuint) tl_node->gl_display_list) ;
#endif
        n_tiles_plotted++ ;
      }
/*
 *   Disable Client States.  Reset Lights and Material Properties
 */
#ifndef no_vertex_arrays
      glDisableClientState(GL_VERTEX_ARRAY) ;
      glDisableClientState(GL_NORMAL_ARRAY) ;
      glDisableClientState(GL_TEXTURE_COORD_ARRAY) ;
      zr_setp4(v4,light0_altde,light0_polar) ;
      glLightfv(GL_LIGHT0, GL_POSITION, v4);
/*
 *  Define default material
 */
      glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat_amb)  ;
      glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat_dif)  ;
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_spc) ;
      glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS, high_shininess) ;
#endif
      tile_t_end   = clock() ;
      if(0 && l_pp)printf(" PLOT TOPOGRAPHY.  Tiles plotted = %i\n",n_tiles_plotted) ;
      check_glerror2("Routine 'display'.  OpenGL error detected after plotting topography\n") ;
#endif
/*
 *==============================================================================
 *  End of topography
 *==============================================================================
 */
/*
 *==============================================================================
 *  Plot Shapes
 *==============================================================================
 */
#if 1
/*
 *==============================================================================
 *  Initialise switches for plotting shapes
 *==============================================================================
 */
      glEnable(GL_LIGHTING);
//  Define front face polygons as going clockwise
//  Needed for MSTS shapes after transforms
      glFrontFace(GL_CW) ;
//  Cull back faces but this should not be used for wagons as some parts
//  are then missing.
      glCullFace(GL_BACK) ;
      glEnable(GL_CULL_FACE) ;

      glEnable(GL_TEXTURE_2D) ;
      glEnable(GL_BLEND) ;
      glEnable(GL_ALPHA_TEST) ;
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glAlphaFunc(GL_GREATER,0.5);

      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;  // Default
/*
 *==============================================================================
 *  Loop over the world files
 *  For each shape encountered:
 *      1.  set transformation
 *      2.  call  display list
 *==============================================================================
 */
#if 1
      {                   //  New block for shapes ===============================
  WorldNode   *wnode ;    //  One WorldNode per tile ...
  WorldItem   *witem ;    //  ... each points to a list of WorldItems ...
  ShapeNode   *snode ;    //  ... many of which have shapes to display
  LodControl  *lod_control    ;
  DistLevel   *dist_level     ;

  int         gl_display_list ;
  static int  l_here = 0      ;  // used to print on first pass only
  int         iwi, idi        ;
  double      dist            ;
  double      scale= 1.0/plot_scale ;

  GLdouble    x, y, z    ;
  GLdouble    a, b, c, d ;

      shape_t_beg = clock() ;
/*
 *  Loop over tiles
 */
      for(wnode=worldlist_beg; wnode != NULL; wnode=wnode->next){
        if(0 == wnode->in_use) continue ;
        if(0 && l_pp)printf(" New tile tile_x, tile_y, in_use = %i %i %i\n",
                           wnode->tile_x, wnode->tile_y,  wnode->in_use ) ;
//        iwi = l_here && (wnode->tile_x == 1449) && (wnode->tile_y == 10331) ;
        iwi = (4 == i_zra) && (wnode->tile_x == 1451) && (wnode->tile_y == 10331) ;
//        if(iwi) i_zra = 0 ;
/*
 *  Loop over items within this tile
 */
        for(witem=wnode->world_item; witem != NULL; witem=witem->next){
          snode = witem->snode ;
//          idi = iwi && (witem->uid == 181) && i_display<2  ;
          idi = iwi && (witem->uid == 226)   ;
          if(idi){
            printf("  routine display - tile :: %i %i  :: item = %i\n",
                    wnode->tile_x, wnode->tile_y, witem->uid );
            printf("  shape : %s,  needed = %i,  loaded = %i\n",
                 snode->name,snode->needed,snode->loaded) ;
          }
/*
 *  Shape nodes causing errors !!
 *  To be investigated ... .
 */
          if(NULL == snode || NULL == snode->name)continue ;
          if(DYNTRACK == witem->worldtype)continue ;
          if(306 == witem->worldtype)continue ;
/*
 *  PolygonOffset code
 *  The indes iz_off is 1 for all track items and 2 for switch/junctions to raise
 *  the item above the land.
 *     glPolygonOffset(a,b)
 *      a used for a*DZ where DZ is related (how?) to the screen area of the polygon
 *      b used for b*r where r is the smallest value guaranteed to produce a resolvable offset.
 */
          if(witem->iz_off){
            glEnable(GL_POLYGON_OFFSET_FILL) ;  //  Apply when filling
            if(2== witem->iz_off){
              glPolygonOffset(gl_offset2,gl_offset2) ;
            }else{
              glPolygonOffset(gl_offset1,gl_offset1) ;
            }
          }else{
            glDisable(GL_POLYGON_OFFSET_FILL) ;
          }
//  Debug printing if new position and and first time with debug statements
//  String 'test_shape' is defined in "zr.c".

//          ip = 0 && (l_pp || i_display<2) && l_here &&
//                                      !strncmp(snode->name,test_shape,12)  ;
          ip = idi ;
          if(ip)printf(" New world item : shape name  = %s\n",snode->name) ;
/*
 *   Convert from MSTS location to coordinates used for graphics
 */
            global2local(tile_xl, tile_yl, tile_h0, tile_size, plot_scale,
                          wnode->tile_x, wnode->tile_y,
                          witem->X, witem->Y, witem->Z, &x, &y, &z) ;
            a = witem->ANG ;
            b = witem->AX ;
            c = witem->AY ;
            d = witem->AZ ;
/*
 *  Check the shape is in the scene - using only the first ShapeVolume (for the moment)
 */
ShapeVolume *shape_vol ;
double      radius, sx, sy, sz, sxs, sys, szs, ssx, ssy, ssz, ttx, tty, ttz ;

            shape_vol = &snode->shape_vol[0] ;

            sx     = shape_vol->vec.X  ; sxs = sx*scalei ;
            sz     = shape_vol->vec.Z  ; szs = sz*scalei ;  //  Transform MSTS to zr axes
            sy     = shape_vol->vec.Y  ; sys = sy*scalei ;
            radius = shape_vol->radius ; radius = radius*scalei ;
/*
 *  The position of the centre of the sphere needs to be rotated before
 *  being added to the origin of the shape.
 *
 *  Should sx, sy, sz be added to the x, y, z before rotation?
 */
            if(a != 0.0 && (sz != 0.0 ||sx != 0.0 ||sy != 0.0) ){
              if(ip)printf("  Rotate\n") ;
//              zrRotateAxyz(a, b, c, d, sx, sy, sz, &ttx, &tty, &ttz) ;
              zrRotateAxyz(a, b, d, c, sxs, sys, szs, &ttx, &tty, &ttz) ;
              ssx = x + ttx ;
              ssy = y + tty ;
              ssz = z + ttz ;
            }else{
              if(ip)printf("  No Rotation\n") ;
              ssx = x + sxs ;  ttx = 0.0 ;
              ssy = y + sys ;  tty = 0.0 ;
              ssz = z + szs ;  ttz = 0.0 ;
            }
            if(ip){
              printf("  Check in scene \n") ;
              printf("  plot_scale = %f,  tile_size = %f, scalei = %f\n",
                        plot_scale, tile_size, scalei) ;
              printf("  tile_x0 = %6i, tile_y0 = %6i, tile_h0 = %f\n", tile_x0, tile_y0, tile_h0) ;
              printf("  tile_x1 = %6i, tile_y1 = %6i\n", tile_xl, tile_yl) ;
              printf("  World item tile:      X = %i, Y = %i\n",
                                            wnode->tile_x,wnode->tile_y) ;
              printf("  World item position:  X = %10.2f, Y = %10.2f, Z = %10.2f\n",
                                            witem->X, witem->Y, witem->Z) ;
              printf("  World item graphics:  X = %10.2f, Y = %10.2f, Z = %10.2f\n",
                                            x*plot_scale, y*plot_scale, z*plot_scale) ;
              printf("  World item rotation:  ANG = %f, X = %6.3f, Y = %6.3f, Z = %6.3f\n",
                                            a, b, d, c) ;
              printf("  Shape volume centre:  X = %10.2f, Y = %10.2f, Z = %10.2f\n", sx, sy, sz);
              printf("  Shape volume rotated: X = %10.2f, Y = %10.2f, Z = %10.2f\n",
                     ttx*plot_scale, tty*plot_scale, ttz*plot_scale);
              printf("               radius     = %10.2f\n",radius*plot_scale) ;
              printf("  Shape volume final:   X = %f, Y = %f, Z = %f\n",
                                            ssx*plot_scale, ssy*plot_scale, ssz*plot_scale) ;
              printf("  Shape volume final2:  X = %f, Y = %f, Z = %f\n",
                            (ssx+0.5)*plot_scale, (ssy+0.5)*plot_scale, ssz*plot_scale) ;
//              printf("               radius     = %f\n",radius) ;
//              printf("  Rotation           :  X = %f, Y = %f, Z = %f\n",ttx,tty,ttz) ;
//              printf("  Final position:    :  X = %f, Y = %f, Z = %f\n",ssx,ssy,ssz) ;
            }
/*
 *  Check if item is within window
 *  Factor of 2.0 is needed because of problems with some long track sections.
 */
            iret = check_in_scene( (GLdouble) ssx,  (GLdouble) ssy, (GLdouble) ssz,
                                   (GLdouble) 2*radius, ip) ;
            if(ip){
              if(iret)printf(" World item : shape is in scene\n") ;
              else    printf(" World item : shape out of scene\n") ;
              i_zra = 0 ;
            }
            if(0==iret) continue ;
            n_shapes_plotted++ ;
/*
 * Forest trees need rotation towards eye position
 */
            if(FOREST == witem->worldtype) a =  eye_z_ang - 90.0;

            glMatrixMode(GL_MODELVIEW) ;
            glPushMatrix() ;
/*
 *  Move to shape final orientation and then position in geographic
 *  coordinates.
 *  Note apparent reversal of order of transformations
 */
            glTranslated(x, y, z) ;   // Move to final position ...
            glRotated(a,b,d,c) ;      // ... and rotate
            glScaled(witem->SX, witem->SY, witem->SZ) ;  //  Scale
/*
 *  Convert from MSTS axes (x-east, y-up, z-north) used in shape files
 *    to geographical axes (x-east, y-north, z-up)
 */
#ifdef geo_coord
            glDisable(GL_CULL_FACE) ;
            glScaled(scale,scale,scale) ;
#else
            glRotated(90.0, 1.0, 0.0, 0.0 ) ;
            glScaled(scale,scale,-scale) ;  // Change from left hand to right hand axes
#endif
/*
 *  Plot shape
 *  Loop over the lod_controls and select the appropiate 'distance level'
 *  Note:  At the moment the distance between the eye and the shape is not
 *         properly checked before using each of the display lists.
 */
            dist = sqrt((x-lookat_eye_x)*(x-lookat_eye_x)
                    + (y-lookat_eye_y)*(y-lookat_eye_y)
                    + (z-lookat_eye_z)*(z-lookat_eye_z) )*plot_scale ;

            for(i=0; i<snode->n_lod_controls; i++){
              lod_control = &(snode->lod_control[i]) ;
              if(ip)printf(" lod_control index = %i\n",i) ;
              dist_level = NULL ;
              for(j=0; j<lod_control->n_dist_levels; j++){
                if(ip)printf("    distance level index = %i\n",j) ;
                dist_level = &(lod_control->dist_level[j])  ;
                if(dist < dist_level->dlevel_selection) break ;
              }
              if(j == lod_control->n_dist_levels){
                continue ;
              }

#ifdef use_shape_display_list
//  Keep the 'optimised' compiler happy
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
              gl_display_list = dist_level->gl_display_list ;
#pragma GCC diagnostic pop
              if(gl_display_list <= 0) continue ;
              if(ip && i_display<2)
              printf("  Display: shape = %s, lod = %i, dist = %i, gl_display_list = %i\n",
                                         snode->name,i,j,gl_display_list);
              glCallList(gl_display_list) ;
#else
//              printf("  Display: shape = %s, lod = %i, dist = %i\n",snode->name,i,j);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
              if(ip && i_display<2){
                l_pd = ip ;
                printf("  Display: shape = %s, lod = %i, dist = %i\n",snode->name,i,j);
              }
#if 0
              if(2643==witem->uid && l_time_1s){
                printf("  Witem = %i, %p, %p, %p %f %f %f\n",witem->uid, (void *)witem,
                       (void *)snode, (void *)dist_level, x, y, z) ;
              }
#endif
//  Fix for USA1 signals
              if(snode->no_culling) glDisable(GL_CULL_FACE) ;
              display_shape(witem, snode, dist_level) ;
              if(snode->no_culling) glEnable(GL_CULL_FACE) ;
              l_pd = 0 ;
#pragma GCC diagnostic pop
#endif
              if(ip && i_display<2)printf("  Display: shape displayed\n");
            }
/*
 *  Print position of shape on screen
 */
#if 0
            glDisable(GL_BLEND) ;
            glDisable(GL_ALPHA_TEST);
            glDisable(GL_TEXTURE_2D) ;
            glDisable(GL_LIGHTING) ;

            sprintf(string," - SHAPE %s :: %f :: %f :: %f ",snode->name,(double)x,(double)y,(double)z);
            if(ip)printf(" string = %s\n",string) ;
            fflush(NULL) ;
            glBegin(GL_LINES) ;
              glVertex3d(x, y, z) ;
              glVertex3d(x, y+5, z) ;
            glEnd() ;
            glColor3f(1.0,1.0,1.0) ;
            print_string_in_window2((GLfloat) x+1, (GLfloat) y+5, (GLfloat) z, string);

            glEnable(GL_BLEND) ;
            glEnable(GL_ALPHA_TEST) ;
            glEnable(GL_TEXTURE_2D) ;
            glEnable(GL_LIGHTING) ;
#endif
/*
 *  PolygonOffset code
 */
            if(witem->iz_off){
              glDisable(GL_POLYGON_OFFSET_FILL) ;
            }
            glPopMatrix() ;
          }
        }
        l_here = 0 ;  // Turn off local logic after first pass
      }   // END of block for shapes

      glDisable(GL_BLEND) ;
      glDisable(GL_ALPHA_TEST);
      glDisable(GL_TEXTURE_2D) ;

      shape_t_end = clock() ;
      glDisable(GL_RESCALE_NORMAL) ;
//  Disable culling
      glDisable(GL_CULL_FACE) ;
//  Restore counter-clockwise default for front-facing polygons
      glFrontFace(GL_CCW) ;
//      printf("  End display of fixed shapes\n") ;
#endif
      glFlush() ;     /* Send last buffer to display */
/*
 *==============================================================================
 *    End of fixed shapes
 *==============================================================================
 */
/*
 *==============================================================================
 *   Display dynamic tracks
 *==============================================================================
 */
#if 1
/*
 *  Reset defaults
 */
      zr_setp4(v4,light0_altde,light0_polar) ;
      glLightfv(GL_LIGHT0, GL_POSITION, v4);
      glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat_amb)  ;
      glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat_dif)  ;
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_spc) ;
      glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS, high_shininess) ;

      dtrack_t_beg = clock() ;
      display_dynamic_tracks() ;
      dtrack_t_end   = clock() ;
#endif
//      printf("  End display of dynamic tracks\n") ;
/*
 * Breakpoint between drawing land and shapes  ...
 *    ... and drawing wagons
 */
//#pragma omp barrier
/*
 *==============================================================================
 *  Display Trains
 *==============================================================================
 */
#if 1
{
int     it1 = 0,  //  Debug traveller
        it = 0 ;

  int     gl_display_list ;
  int     n_dist_levels   ;
  double   x, y, z, xr, yr, zr, dist  ;
  double   rail_height = 0.27 ;  // Best fit placing wheels on track
  GLfloat a, b, c, d ;
  GLfloat scalei = 1.0/plot_scale ;

  TrainNode     *train ;
  WagonNode     *wagon ;
  RawWagonNode  *r_wagon ;
  ShapeNode     *f_shape ;
  TravellerNode *tv    ;
  TrkVectorNode *vn    ;
  LodControl    *lod_control ;
  DistLevel     *dist_level  ;

      it = it1 && l_disp1 ;

      glPolygonOffset(0.0,0.0) ;
#ifdef culling_on_for_wagons
      glCullFace(GL_BACK) ;
      glEnable(GL_CULL_FACE) ;
#else
      glDisable(GL_CULL_FACE) ;
#endif
      glFrontFace(GL_CW) ;

      glEnable(GL_TEXTURE_2D) ;
      glEnable(GL_BLEND) ;
      glEnable(GL_ALPHA_TEST) ;
      glAlphaFunc(GL_GREATER,0.1);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;

  int i_train, i_wagon ;

      i_train = 0 ;
      for(train = trainlist_beg; train != NULL ; train=train->next){
//        if(l_time_1s)printf("  Train  %p %p :: name = %s\n",
//                        (void *)train->first, (void *)train->last, train->name) ;
        i_train++ ;
        i_wagon = 0 ;
        for(wagon = train->first; wagon != NULL; wagon = wagon->next){
//          if(l_time_1s)printf("    Wagon  %p %p :: name = %s\n",
//                  (void *)wagon->prev, (void *)wagon->next, wagon->name) ;
          i_wagon++ ;
          if(!strcmp(wagon->name,"Default")) continue ;
          tv = wagon->traveller ;
/*
 *  Check traveller2local position against OpenGL position
 */
#if 0
{
  double  wx, wy, wz ;
          traveller2local(tv, &wx, &wy, &wz) ;
          wx = wx / plot_scale ;
          wy = wy / plot_scale ;
          wz = wz / plot_scale ;
          glMatrixMode(GL_MODELVIEW) ;
          glPushMatrix() ;
            glDisable(GL_LIGHTING) ;
            glDisable(GL_TEXTURE_2D) ;
            glLineWidth(5.0) ;
//  Location of traveller
//        using traveller modelview matrix
            glBegin(GL_LINES) ;

            glColor3f((GLfloat)1.0,(GLfloat)1.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) wx, (GLfloat)wy, (GLfloat)(wz-1.0) ) ;
            glVertex3f((GLfloat) wx, (GLfloat)wy, (GLfloat)(wz+1.0) ) ;

            glEnd() ;
            glEnable(GL_LIGHTING) ;
            glEnable(GL_TEXTURE_2D) ;
          glPopMatrix() ;
}
#endif
          vn = tv->vn ;
          a = degree*vn->a_east_x   ;
          b = degree*vn->a_height_y ;
          c = degree*vn->a_north_z  ;
/*
 *   Convert origin of the current track vector MSTS coordinates to
 *   coordinates relative to local graphics origin
 */
          global2local(tile_xl, tile_yl, tile_h0, tile_size, plot_scale,
                         vn->tile_east_x, vn->tile_north_z,
                         vn->east_x, vn->north_z, vn->height_y+rail_height,
                         &x, &y, &z) ;
          if(it)printf("\n TTTA  x, y, z = %f %f %f :: %i %i  :: %f %f %f : %f \n",
                         (double)x,(double)y,(double)z,
                         vn->tile_east_x, vn->tile_north_z,
                         vn->east_x, vn->height_y, vn->north_z, scale) ;
// mstswagon2local defined in graphics.c
          mstswagon2local(0.0, 0.0, 0.0, tv->x, tv->y, tv->z, a, b, c, scalei,
                         x, y, z, &xr, &yr, &zr) ;

          if(l_time_5s && ip){
            printf(" ==== display.c ====\n") ;
            printf("     x  = %f, y  = %f, z  = %f\n", x, y, z) ;
            printf("     xr = %f, yr = %f, zr = %f\n", xr, yr, zr) ;
            printf(" tv  x  = %f, y  = %f, z  = %f\n", tv->x, tv->y, tv->z) ;
            printf(" eye x  = %f, y  = %f, z  = %f\n", lookat_eye_x, lookat_eye_y, lookat_eye_z) ;
          }
/*
 * Check the wagon is within view - skip if outside view or far away
 */
          iret = check_in_scene( (GLdouble) xr,  (GLdouble) yr, (GLdouble) zr,
                                (GLdouble) 0.05, 0) ;
          if(0==iret) continue ;

          dist = sqrt((xr-lookat_eye_x)*(xr-lookat_eye_x)
                    + (yr-lookat_eye_y)*(yr-lookat_eye_y)
                    + (zr-lookat_eye_z)*(zr-lookat_eye_z) )*plot_scale ;

          glMatrixMode(GL_MODELVIEW) ;
          glPushMatrix() ;
            glTranslated(x, y, z) ;            //  Move vector origin to final position
            glRotated( 90.0, 1.0, 0.0, 0.0 ) ; //  Change from MSTS coordinates of shapes to
            glScaled(scalei,scalei,-scalei) ;  //    normal right-hand coordinates used by ZR
            glRotated(b, 0.0, 1.0, 0.0) ;      //  Rotate track vector about its origin
            glRotated(a, 1.0, 0.0, 0.0) ;      //  The order (roll, pitch, yaw) is unexpected
            glRotated(c, 0.0, 0.0, 1.0) ;      //   but it is the only one that works
            glTranslated(tv->x,tv->y,tv->z) ;  //   and move wagon origin to lie on track vector

            glRotated(-tv->ang_deg,0.0, 1.0, 0.0) ;  //  Start by rotating wagon

            if(ip && l_time_1s){
              printf("\n Disp : TrkVect(x, y, z) = %5.3f %5.3f %5.3f,   Trk_ang = %6.1f\n",
                       x, y, z, b) ;
              printf("      : Wagon(x, y, z)   = %5.1f %5.1f %5.1f,   Wag_ang = %5.1f, train_dir = %i, wagon_dir = %i\n",
                       tv->x, tv->y, tv->z, tv->ang_deg,
                       wagon->train_dir, tv->idirect) ;
            }
/*
 *  Draw location lines
 */
#if 0
            glDisable(GL_LIGHTING) ;
            glDisable(GL_TEXTURE_2D) ;
            glLineWidth(5.0) ;
//  Location of traveller
//        using traveller modelview matrix
            glBegin(GL_LINES) ;
            glColor3f((GLfloat)1.0,(GLfloat)0.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) -1.0, (GLfloat)4.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat)  5.0, (GLfloat)4.0,(GLfloat)0.0) ;

            glColor3f((GLfloat)0.0,(GLfloat)1.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)-1.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat) 5.0,(GLfloat)0.0) ;

            glColor3f((GLfloat)0.0,(GLfloat)0.0,(GLfloat)1.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)4.0,(GLfloat)-1.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)4.0,(GLfloat) 5.0) ;
            glEnd() ;
            glEnable(GL_LIGHTING) ;
            glEnable(GL_TEXTURE_2D) ;
#endif
            dist_level = NULL ;
            for(i=0; i<wagon->shape->n_lod_controls; i++){
              lod_control = &(wagon->shape->lod_control[i]) ;
              for(j=0; j<lod_control->n_dist_levels; j++){
                dist_level = &(lod_control->dist_level[j])  ;
                if(dist < dist_level->dlevel_selection) break ;
              }
              if(j == lod_control->n_dist_levels) continue ;
              display_wshape(wagon, wagon->shape, dist_level) ;
            }
/*
 *  Display load
 */
            r_wagon = wagon->raw_wagon ;
            f_shape = r_wagon->f_shape ;
            if(f_shape != NULL){
              for(i=0; i<f_shape->n_lod_controls; i++){
                lod_control = &(f_shape->lod_control[i]) ;
                for(j=0; j<lod_control->n_dist_levels; j++){
                  dist_level = &(lod_control->dist_level[j])  ;
                  if(dist < dist_level->dlevel_selection) break ;
                }
                if(j == lod_control->n_dist_levels) continue ;
                display_wshape(wagon, f_shape, dist_level) ;
              }
            }
/*
 *  Draw location lines
 */
#if 0
            glDisable(GL_LIGHTING) ;
            glDisable(GL_TEXTURE_2D) ;
            glLineWidth(5.0) ;
//  Location of traveller
//        using traveller modelview matrix
            glBegin(GL_LINES) ;
            glColor3f((GLfloat)1.0,(GLfloat)0.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) -1.0, (GLfloat)4.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat)  5.0, (GLfloat)4.0,(GLfloat)0.0) ;

            glColor3f((GLfloat)0.0,(GLfloat)1.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)-1.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat) 5.0,(GLfloat)0.0) ;

            glColor3f((GLfloat)0.0,(GLfloat)0.0,(GLfloat)1.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)4.0,(GLfloat)-1.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)4.0,(GLfloat) 5.0) ;
            glEnd() ;

            glEnable(GL_LIGHTING) ;
            glEnable(GL_TEXTURE_2D) ;
#endif
          glPopMatrix() ;
        }
      }

      glDisable(GL_BLEND) ;
      glDisable(GL_ALPHA_TEST);
      glDisable(GL_TEXTURE_2D) ;
//  Restore counter-clockwise default for front-facing polygons
      glFrontFace(GL_CCW) ;
//  Disable culling
      glDisable(GL_CULL_FACE) ;
}
#endif
      glFlush() ;     /* Send last buffer to display */
//      printf("  End display of trains\n") ;
/*
 *   Plot default rail tracks.  Used when no track shapes available
 *   Assume LOD method is component additive (flag: COMPONENTADDITIVE).
 *   For the moment this does not check distance
 *
 *   Display lists not normally used.  If they are, remember they are defined
 *   relative to tile_x0, tile_y0 - at the south west of the full layout.
 *
 *   NOTE:  "if 1" now causes a striped landscape!!
 */
#if 0
//    printf("\n  Plot track bed\n") ;
#if 0
      glDisable(GL_LIGHTING) ;
      glShadeModel(GL_FLAT) ;
      glDisable(GL_TEXTURE_2D) ;
      glColor3f((GLfloat)1.0,(GLfloat)1.0,(GLfloat)1.0) ;
#endif

{
int            gl_display_list ;
TrkSector *trk_sec_node   ;

      for(i=0;i<track_db.trk_sections_array_size;i++){
        trk_sec_node = &(track_db.trk_sections_array[i])       ;
        if(trk_sec_node->type_of_node != VECTOR_SECTION) continue ;
#if 1
        glEnable(GL_TEXTURE_2D) ;
        glEnable(GL_BLEND) ;
        glEnable(GL_ALPHA_TEST) ;
        glAlphaFunc(GL_GREATER,0.5);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
#endif
        for(j=0;j<3;j++){      //  Add all 3 components track (sides, top, base).
#if 0
          glColor3f((GLfloat)0.7,(GLfloat)0.0,(GLfloat)0.0) ;
          if(1 == j)glColor3f((GLfloat)0.0,(GLfloat)0.7,(GLfloat)0.0) ;
          if(2 == j)glColor3f((GLfloat)0.0,(GLfloat)0.0,(GLfloat)0.7) ;
#endif
          gl_display_list = trk_sec_node->opengl_display_list[j] ;
          if(i_display<2 && (trk_sec_node->uid == 123)){
             printf("  Call default track display list %i %i :: d list = %i \n",
                      trk_sec_node->uid, j, gl_display_list) ;

          }
          glCallList(gl_display_list) ;
        }
      }
      glDisable(GL_TEXTURE_2D) ;
}
#endif
/*
 *   Plot default roads.   Used when no track shapes available
 *   Display lists not normally used.  If they are, remember they are defined
 *   relative to tile_x0, tile_y0 - at the south west of the full layout.
 */
#if 0
//      printf("  Plot Roads ... %i \n",road_db.trk_sections_array_size) ;
//      glDisable(GL_LIGHTING) ;
//      glColor3f(0.7,0.6,0.1) ;
      for(i=0,k=0;i<road_db.trk_sections_array_size;i++,k++){
        trk_sec_node = &(road_db.trk_sections_array[i])       ;
        if(trk_sec_node->type_of_node != VECTOR_SECTION) continue ;
        glEnable(GL_TEXTURE_2D) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
        for(j=0;j<1;j++){
          gl_display_list = trk_sec_node->opengl_display_list[j] ;
//          printf("  Call display list i = %i, j = %i, display_list = %i\n",i,j,gl_display_list) ;
          glCallList(gl_display_list) ;
        }
        glDisable(GL_TEXTURE_2D) ;
      }
      if(l_pp)printf(" PLOTTING ROADS.  Roads plotted = %i\n",k) ;
#endif
/*
 *  Plot extra information on world and track items near to the
 *  observer
 */
#if 1
#ifdef SHADERS
      glUseProgram(shader_00) ;
      display_extra_data() ;   //  display_info.c
      glUseProgram(0) ;
#else
      display_extra_data() ;   //  display_info.c
#endif
//      printf("  End display of extra info\n") ;
#endif

#endif                 //  End of Normal 3D Display Section
/*
 *==============================================================================
 *  Special Panels for Normal Display
 *==============================================================================
 *  These use gluOrtho to generate a 2D image in front of the main 3D screen
 *  This is scaled so that one unit equals one pixel
 *==============================================================================
 */
#ifdef SHADERS
      glUseProgram(shader_00) ;
#endif
#ifdef _Display_Normal
#if 1
{
GLfloat w = viewport_width ;
GLfloat h = viewport_height ;

      glEnable(GL_BLEND) ;
      glDisable(GL_LIGHTING) ;
      glDisable(GL_TEXTURE_2D) ;

//  Set up projection

      glMatrixMode(GL_PROJECTION) ;
      glPushMatrix() ;
      glLoadIdentity() ;
      gluOrtho2D(0.0,w,0.0,h) ;
      glViewport(0,0,viewport_width,viewport_height) ;
      glMatrixMode(GL_MODELVIEW) ;
      glPushMatrix() ;
      glLoadIdentity() ;

//  Call special 2-D displays

      if(display_help_on)       display_help() ;
      if(player_train){
        if(display_track_info_on) display_track_info(player_train->first->traveller) ;
        if(display_switches_on)   display_switches(player_train) ;
        if(display_train_operations_on) display_train_operations(player_train) ;
      }

//  Restore defaults

      glPopMatrix() ;  //  Pop GL_MODELVIEW
      glMatrixMode(GL_PROJECTION) ;
      glPopMatrix() ;  //  Pop GL_PROJECTION
      glMatrixMode(GL_MODELVIEW) ;

#ifndef display_tracks_and_roads
/*
 *  Sketch tracks routes close to the viewpoint
 */
      if(display_track_info_on){
        if(display_track_info_on & 1)sketch_track_routes(0) ;
        if(display_track_info_on & 2)sketch_track_items(0)  ;
      }else{
        if(show_platforms_and_sidings)display_platforms_and_sidings() ;
      }
#endif
}
#endif
/*
 *   Code to print frames per second in bottom left of window
 *   Switch on/off with shift-z.
 *   It averages over five seconds but takes the first five
 *   seconds to initialise.
 */
#if 1
      glDisable(GL_LIGHTING) ;
      glDisable(GL_TEXTURE_2D) ;

      if(l_fps){

        k = calls_per_second() ;
        if(k>0) fps = k ;
        tile_time_used = 0.98*tile_time_used +  0.02*
                       1000.0*((double)(tile_t_end-tile_t_beg))/CLOCKS_PER_SEC ;
        shape_time_used = 0.95*shape_time_used +  0.05*
                       1000.0*((double)(shape_t_end-shape_t_beg))/CLOCKS_PER_SEC ;
        track_time_used = 0.95*track_time_used +  0.05*
                       1000.0*((double)(track_t_end-track_t_beg))/CLOCKS_PER_SEC ;
        dtrack_time_used = 0.95*dtrack_time_used +  0.05*
                     1000.0*((double)(dtrack_t_end-dtrack_t_beg))/CLOCKS_PER_SEC ;
        sprintf(string,"F.P.S. = %i (%.0f%%) Tiles %i time %7.3f "
                                                "Shapes %i time %7.3f "
                                                "Tracks %i %7.3f %7.3f",
                                          fps, fps_delay,
                                          n_tiles_plotted, tile_time_used,
                                          n_shapes_plotted, shape_time_used,
                                          n_tracks_plotted, track_time_used,
                                          dtrack_time_used) ;
        glColor3f(1.0,1.0,1.0) ;
        print_string_in_window((GLfloat)20.0,(GLfloat)20.0,string) ;

        sprintf(string,"Run time = %9.0f     Speed = %7.2f m/s     %7.2f mph",
                           run_seconds, player_train->speed, 2.236936*player_train->speed );
        print_string_in_window((GLfloat)20.0,(GLfloat)40.0,string) ;
  double  t[4] ;
        zr_clock_gettime(zr_clock_2) ;
        for(i=0;i<4;i++){
          t[i] = (zr_clock_2[i].tv_sec - zr_clock_1[i].tv_sec)
                +(zr_clock_2[i].tv_nsec - zr_clock_1[i].tv_nsec)*0.000000001 ;
          zr_clock_time[i][0] = 0.95*zr_clock_time[i][0] + 0.05*t[i] ;
        }
        sprintf(string," %f %f %f %f :: %i",
                         zr_clock_time[0][0],zr_clock_time[1][0],
                         zr_clock_time[2][0],zr_clock_time[3][0],n_vanodes_d) ;
        print_string_in_window((GLfloat)20.0,(GLfloat)60.0,string) ;
        if(0)printf(" Times AA %f %f %f %f\n",
                         zr_clock_time[0][0],zr_clock_time[1][0],
                         zr_clock_time[2][0],zr_clock_time[3][0]) ;
      }

//      printf("  End display of frames per second\n") ;
#endif
//      printf("  End display of special panels\n") ;
#endif

/*
 *==============================================================================
 *  Routine to print current status of glError flag
 *==============================================================================
 */
#if 1
      display_error() ;
#endif
#ifdef SHADERS
      glUseProgram(0) ;
#endif

#endif
/*
 *  Update train_last variables for each train
 */
      for(train = trainlist_beg; train != NULL ; train=train->next){
        train->last_speed = train->speed ;
      }
/*
 * Clean up at end of routine display
 */
      glFlush() ;     /* Send last buffer to display */
/*
 * Swap double buffers - with SDL2 this is done in the loop which calls display().
 */
#ifndef SDL2
      glutSwapBuffers();
#endif
/*
 *  Process Movie Shot
 */
#ifdef MOVIE
      movie_new_frame() ;
#endif

      {
double t[4] ;
        zr_clock_gettime(zr_clock_2) ;
        for(i=0;i<4;i++){
          t[i] = (zr_clock_2[i].tv_sec - zr_clock_1[i].tv_sec)
                +(zr_clock_2[i].tv_nsec - zr_clock_1[i].tv_nsec)*0.000000001 ;
          zr_clock_time[i][1] = 0.95*zr_clock_time[i][1] + 0.05*t[i] ;
        }
        if(0)printf(" Times BB %f %f %f %f\n\n",
                         zr_clock_time[0][1],zr_clock_time[1][1],
                         zr_clock_time[2][1],zr_clock_time[3][1]) ;

      }

      check_glerror2("Routine 'display'.  OpenGL error detected near end of routine\n") ;
      icount ++ ;
      if(l_pd)printf(" Exit  display()\n");
      l_pp = 0    ;  //  Turn off flag for: new display after position move
      l_disp1 = 0 ;  //                     printing

}                    //  End of //#pragma omp section 2
}                    //  End of //#pragma omp sections
}                    //  End of //#pragma omp parallel

      return ;
}
/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *  Utility routine to plot tile outlines just above nominal height (900 m?)
 *  Note:  The position of each shape is given in metres relative to the
 *         centre of each tile
 *         Grid is plotted just above z=0.0
 *==============================================================================
 */
int plot_tile_outlines(void){

  int   i, j, i1, j1, i2, j2 ;
  GLfloat xx, yy ;

      i1  =  tile_west  - tile_xl ;
      i2  =  tile_east  - tile_xl ;
      j1  =  tile_south - tile_yl ;
      j2  =  tile_north - tile_yl ;

      glDisable(GL_LINE_STIPPLE) ;
      glDisable(GL_LIGHTING) ;
      glDisable(GL_TEXTURE_2D) ;
      glDisable(GL_BLEND) ;
//      printf("  plot_tile_outlines : %i %i :: %i %i\n",i1,i2,j1,j2);
      glLineWidth(1.0) ;
      glColor3f(0.0,0.5,0.5) ;
      glBegin(GL_LINES) ;
      for(i=i1;i<i2+2;i++){
        xx = i  ;
        if(0==i){ glEnd() ; glLineWidth(5.0) ; glBegin(GL_LINES) ; }
        glVertex3f(xx, (GLfloat)j1,       (GLfloat)0.000) ;
        glVertex3f(xx, (GLfloat)(j2 + 1), (GLfloat)0.000) ;
        if(0==i){ glEnd() ; glLineWidth(1.0) ; glBegin(GL_LINES) ; }
      }
      for(j=j1;j<j2+2;j++){
        yy = j  ;
        if(0==j){ glEnd() ; glLineWidth(5.0) ; glBegin(GL_LINES) ; }
        glVertex3f((GLfloat)i1,       yy, (GLfloat)0.000) ;
        glVertex3f((GLfloat)(i2 + 1), yy, (GLfloat)0.000) ;
        if(0==j){ glEnd() ; glLineWidth(1.0) ; glBegin(GL_LINES) ; }
      }
      glEnd();
      return 0 ;
}

/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *  Utility routine to sketch routes of the tracks : 1 m above track base.
 *==============================================================================
 */
int sketch_track_routes(int plot_all_tracks){

  int      i, j, k, ip = 0  ;
  double  scale = 1.0/plot_scale ;
  double  x = 0., y=0.0 , z=0.0, dist ;
  TrkVectorNode   *vec, *v ;
  TrkSector      *trk_sec_node  ;
  WorldItem       *w ;
  DynTrackSect    *d ;
  char     string[256] ;
  char     *my_name = "sketch_track_routes" ;

      if(ip)printf("Enter %s, %i\n",my_name, plot_all_tracks) ;

      glDisable(GL_LIGHTING)   ;
      glDisable(GL_TEXTURE_2D) ;
      glDisable(GL_BLEND)      ;
      glShadeModel(GL_FLAT)     ;
      for(i=0;i<(int)track_db.trk_sections_array_size;i++){
        trk_sec_node = &track_db.trk_sections_array[i] ;
        vec = trk_sec_node->vector ;
        if(0 < trk_sec_node->length_of_vector) {
          k = 0;
          for(j=0;j<(int)trk_sec_node->length_of_vector;j++){
            global2local(tile_xl, tile_yl, tile_h0, tile_size, plot_scale,
                  vec[j].tile_east_x, vec[j].tile_north_z,
                  vec[j].east_x, vec[j].north_z, vec[j].height_y + 1.0,
                  &x, &y, &z );
/*
 * Distances in units of 1024m
 */
            dist = pow(x-lookat_eye_x,2) + pow(y-lookat_eye_y,2) ;
            if(plot_all_tracks == 0 && dist > 0.04)continue ;
/*
 *  Plot lines for each of the vectors.  Note: then last 'vector' of each
 *  series is not plotted.
 */
            ortho_col++ ;
            if(!k++){
              glBegin(GL_LINE_STRIP) ;
              glColor3f(1.0,1.0,1.0) ;
              ortho_col = 0 ;
            }else if(1 == ortho_col)glColor3f(0.8,0.8,0.8) ;
            else if(2 == ortho_col)glColor3f(0.0,0.0,1.0) ;
            else if(3 == ortho_col)glColor3f(0.0,1.0,0.0) ;
            else if(4 == ortho_col)glColor3f(1.0,0.0,0.0) ;
            else if(5 == ortho_col)glColor3f(0.0,1.0,1.0) ;
            else if(6 == ortho_col)glColor3f(1.0,1.0,0.0) ;
            else if(7 == ortho_col){
              glColor3f(1.0,0.0,1.0) ;
              ortho_col = 0;
            }
            glVertex3d((GLfloat) x, (GLfloat) y, (GLfloat) z) ;
//            printf(" %i %i, %f %f %f :: %f \n",k,ortho_col,x,y,z,dist);
          }
          if(k)glEnd();
        }
/*
 *  Code to identify track sections in 3D scene
 */
#if 1
        for(j=0;j<(int)trk_sec_node->length_of_vector;j++){
          v = &vec[j] ;
          w = v->world_item ;
          global2local(tile_xl, tile_yl, tile_h0, tile_size, plot_scale,
                vec[j].tile_east_x, vec[j].tile_north_z,
                vec[j].east_x, vec[j].north_z, vec[j].height_y,
                &x, &y, &z );

          dist = pow(x-lookat_eye_x,2) + pow(y-lookat_eye_y,2) ;
          if(plot_all_tracks == 0 && dist > 0.04)continue ;

          glColor3f(0.8,0.8,0.8) ;
          glBegin(GL_LINES) ;
            glColor3f(0.8,0.8,0.8) ;
            glVertex3d(x, y, z) ;
            z = z + 6.0*scale ;
            glVertex3d(x, y, z) ;
          glEnd() ;
          glColor3f(1.0,1.0,1.0) ;
#if 1
/*
 *  Print track information
 *  Note 'print_string_in_window'
 *    on entry disables GL_LIGHTING, GL_TEXTURE_2D,  enables GL_BLEND
 *    on exit  enables  GL_LIGHTING, GL_TEXTURE_2D and GL_BLEND
 *
 */
          if(v->world_item){
            sprintf(string," TRACK SECT NODE - %i  %i  %i  %i ::F %i %i  ::W  %i  %i\n",
                  trk_sec_node->uid,trk_sec_node->type_of_node,
                  trk_sec_node->length_of_vector,j,v->flag1, v->flag2,
                  v->world_item->uid,v->world_item->iz_off) ;
          }else{
            sprintf(string," TRACK SECT NODE - %i  %i  %i  %i ::F %i %i\n",
                  trk_sec_node->uid,trk_sec_node->type_of_node,
                  trk_sec_node->length_of_vector,j,v->flag1, v->flag2) ;
          }

          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string) ;

          sprintf(string," - TILE %i %i ::XYZ %10.3f  %10.3f  %10.3f\n",
                  vec[j].tile_east_x,vec[j].tile_north_z,
                  vec[j].east_x,vec[j].north_z,vec[j].height_y);
          z = z - 1.0*scale ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);

          if(w){
            if(w->worldtype==306){
              for(k=0;k<5;k++){
                d = &w->u.dyn_track_obj.dyn_trk_sect[k] ;
                if(d->uid==-1)continue ;
                sprintf(string," - %i %i  (%i %.2f %.1f)",v->flag1,v->flag2,d->is_curved, d->param_1,d->param_2);
                z = z - 1.0*scale ;
                glColor3f((GLfloat)v->flag1,0.0,(GLfloat)v->flag2) ;
                print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
                glColor3f(1.0,1.0,1.0) ;
              }
            }else{
                glColor3f(1.0,1.0,1.0) ;
                sprintf(string," - World Type = %i %s",w->worldtype,token_idc[w->worldtype]);
                z = z - 1.0*scale ;
                print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
                glColor3f(1.0,1.0,1.0) ;
            }
          }
          glDisable(GL_LIGHTING)   ;  //Correct for print_string_in_window
          glDisable(GL_TEXTURE_2D) ;
          glDisable(GL_BLEND)      ;
#endif
        }
#endif
      }
      glEnable(GL_LIGHTING)   ;
      glEnable(GL_TEXTURE_2D) ;
      glDisable(GL_BLEND) ;

      if(ip)printf("Exit  %s\n",my_name) ;

      return 0 ;
}
/*
 *    Sketch
 */
/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *  Utility routine to sketch routes of the roads : 1 m above road base.
 *==============================================================================
 */
int sketch_road_routes(void){

  int    i, j ;
  double   scale = 1.0/plot_scale ;
  double   x, y, z ;
  TrkVectorNode   *vec ;
  TrkSector *road_sec_node ;

      glDisable(GL_LIGHTING) ;
      glDisable(GL_TEXTURE_2D) ;
      glDisable(GL_BLEND) ;
      for(i=0;i<(int)road_db.trk_sections_array_size;i++){
        road_sec_node = &road_db.trk_sections_array[i] ;
        if(2 > road_sec_node->length_of_vector) continue ;

        vec = road_sec_node->vector ;
        glShadeModel(GL_FLAT) ;
        for(j=0;j<(int)road_sec_node->length_of_vector;j++){
          global2local(tile_xl, tile_yl, tile_h0, tile_size, plot_scale,
                 vec[j].tile_east_x, vec[j].tile_north_z,
                 vec[j].east_x, vec[j].north_z, vec[j].height_y + 1.0,
                 &x, &y, &z );
          ortho_col++ ;
          if(0 == j){
            glBegin(GL_LINE_STRIP) ;
            glColor3f(1.0,1.0,1.0) ;
            ortho_col = 0 ;
          }else if(1 == j)glColor3f(1.0,1.0,1.0) ;
          else if(1 == ortho_col)glColor3f(0.7,0.7,0.7) ;
          else if(2 == ortho_col)glColor3f(0.4,0.4,1.0) ;
          else if(3 == ortho_col)glColor3f(0.4,1.0,0.4) ;
          else if(4 == ortho_col)glColor3f(1.0,0.4,0.4) ;
          else if(5 == ortho_col)glColor3f(0.4,1.0,1.0) ;
          else if(6 == ortho_col)glColor3f(1.0,1.0,0.4) ;
          else if(7 == ortho_col){
            glColor3f(1.0,0.4,1.0) ;
            ortho_col = 0;
          }
          glVertex3d((GLfloat) x, (GLfloat) y, (GLfloat) z) ;
        }
        glEnd();
/*
 *  Code to identify road sections in 3D scene
 */
#if 0
  char *string[128] ;
        sprintf(string," - ROAD %i\n",road_sec_node->uid);
        glColor3f(0.6,0.6,1.0) ;
//          print_string_in_window((GLfloat) x, (GLfloat) y, string);
        glBegin(GL_LINES) ;
          glVertex3d(x, y, z) ;
          z = z + 50.0*scale ;
          glVertex3d(x, y, z) ;
        glEnd() ;
        print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
        glColor3f(1.0,1.0,1.0) ;
#endif
      }
      return 0 ;
}

/*
 *==============================================================================
 *    Routine to display all the shapes
 *==============================================================================
 */
int display_shapes(void){

  ShapeNode  *snode          ;
  LodControl *lod_control    ;
  DistLevel  *dist_level     ;
  int        gl_display_list ;
  GLfloat    x1, y1,
            x0 = 0, y0 = 0, scale = 1.0/plot_scale ; ;
  int        i, j, ierr, ip = 0 ;
  char       string[128];


//      printf(" PLOTTING  SHAPES :: Routine display_shapes\n") ;
      glEnable(GL_LIGHTING) ;

      sort_shapes(&shapelist_beg) ;

      for(snode=shapelist_beg, i=0, j=0; snode!=NULL; snode=snode->next, i++){
        if(100==i){
          j = j + 1;
          i = 0;
        }
        y1 = y0 + i*150*scale ;
        x1 = x0 + (j*100+ (i%2)*50)*scale ;

        glMatrixMode(GL_MODELVIEW) ;
        glPushMatrix() ;

        glTranslatef(x1,y1,0.0) ;
#ifdef geo_coord
        glScalef(scale,scale,scale) ;
#else
        glRotatef((GLfloat) d_rotate, (GLfloat) 0.0, (GLfloat) 0.0, (GLfloat) 1.0 ) ;
        glRotatef((GLfloat) 90.0, (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0 ) ;
        glScalef(scale,scale,d_reflect*scale) ;  // Change from left hand to right hand axes
#endif

        lod_control = &(snode->lod_control[0]) ;
        dist_level = &(lod_control->dist_level[0])  ;

        glEnable(GL_TEXTURE_2D) ;
        glEnable(GL_BLEND) ;
        glEnable(GL_ALPHA_TEST) ;
        glAlphaFunc(GL_GREATER,0.5);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;
        ierr = glGetError() ;
        if(ierr != GL_NO_ERROR && icount++<100)
                                      printf(" FFF  GL error %i \n",ierr);

#ifdef use_shape_display_list
        gl_display_list = dist_level->gl_display_list ;
        if(ip)printf("  Display: shape = %s, lod = %i, dist = %i, gl_display_list = %i\n",
                  snode->name,i,j,gl_display_list);
        if(0 != gl_display_list) glCallList(gl_display_list) ;
        ierr = glGetError() ;
        if(ierr != GL_NO_ERROR && icount++<100){
                printf(" GGG  GL error        %i \n",ierr);
                printf(" GGG  gl_display_list %i \n",gl_display_list);
        }
#else
        display_shape(NULL, snode, dist_level) ;
        ierr = glGetError() ;
        if(ierr != GL_NO_ERROR && icount++<100){
                printf(" GGG  GL error        %i \n",ierr);
        }
#endif

        glPopMatrix() ;

/*
 *  Add name to shape display group
 */
        glDisable(GL_TEXTURE_2D) ;
        glDisable(GL_LIGHTING) ;
        sprintf(string," - %s",snode->name);
        glColor3f(1.0,1.0,1.0) ;
        print_string_in_window2((GLfloat) x1+0.001, (GLfloat) y1, (GLfloat) 0, string);
        glEnable(GL_LIGHTING) ;
      }
      return 0 ;
}
/*
 *==============================================================================
 *    Routine to display all the wagons in one of the unused tiles
 *    Using the display lists
 *    Note:  Some of the wagons have parts missing if GL_CULL_FACE is
 *           enabled.  This implies that they so not use a strict front
 *           face winding rule (i.e. always anti-clockwise)
 *==============================================================================
 */
int display_wagons(void){

  int        i, j, ip = 0    ;
  ShapeNode  *shape,
            *f_shape        ;
  RawWagonNode *wnode        ;
  WagonNode  dummy,
            *wagon = &dummy ;  // Dummy wagon
  LodControl *lod_control    ;
  DistLevel  *dist_level     ;
  int        gl_display_list ;
  GLfloat    x1, y1, z1, x2, y2, z2,
            x0 = 0, y0 = 0,
            scale = 1.0/plot_scale ;
  char       string[128]     ;
  char       *my_name = "display_wagons" ;

      if(ip){printf("  Enter routine %s\n",my_name) ; fflush(NULL) ; }

      wagon->name = "dummy wagon" ;
      wagon->wheel_angle = 0.0 ;
      wagon->is_engine   = 0   ;

      glEnable(GL_LIGHTING) ;
#ifdef culling_on_for_wagons
      glCullFace(GL_BACK) ;
      glEnable(GL_CULL_FACE) ;
#else
      glDisable(GL_CULL_FACE) ;
#endif
      glFrontFace(GL_CW) ;

      glEnable(GL_BLEND) ;
      glEnable(GL_ALPHA_TEST) ;
      glAlphaFunc(GL_GREATER,0.5);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//  When displaying all the wagons, light modulation is not used
//
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;

//      for(snode=wshapelist_beg, i=0, j=0; snode!=NULL; snode=snode->next, i++){
      for(wnode=rawwagonlist_beg, i=0, j=0; wnode!=NULL; wnode=wnode->next, i++){
        if(ip){printf("  Wagon = %s\n",wnode->name) ; fflush(NULL) ; }
        if(!strcmp(wnode->name,"Default")) continue ;
        if(30==i){
          j = j + 1;
          i = 0;
        }
        y1 = y0 + i*30*scale ;
        x1 = x0 + j*30*scale ;

        glEnable(GL_TEXTURE_2D) ;

        glMatrixMode(GL_MODELVIEW) ;
        glPushMatrix() ;
        glTranslatef(x1,y1,0.0) ;
#ifdef geo_coord
        glScalef(scale,scale,scale) ;
#else
        glRotatef((GLfloat) d_rotate, (GLfloat) 0.0, (GLfloat) 0.0, (GLfloat) 1.0 ) ;
        glRotatef((GLfloat) 90.0, (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0 ) ;
        glScalef(scale,scale,d_reflect*scale) ;  // Change from left hand to right hand axes
#endif

        shape = wnode->shape ;
        lod_control = &(shape->lod_control[0]) ;
        dist_level = &(lod_control->dist_level[0])  ;
        if(ip){printf("  Call display_shape for %s\n",wnode->name) ;fflush(NULL) ; }
        display_wshape(wagon, shape, dist_level) ;

/*
 *  Display load
 */
        f_shape = wnode->f_shape ;
        if(f_shape != NULL){
          lod_control = &(f_shape->lod_control[0]) ;
          dist_level = &(lod_control->dist_level[0])  ;
          if(ip)printf("  Display: i,j = %i,%i load  = %s, lod = %p, dist = %p\n"
                       ,i,j,f_shape->name,(void *)lod_control,
                                          (void *)dist_level) ;
        if(ip){printf("  Call display_shape for load %s\n",f_shape->name) ;fflush(NULL) ; }
          display_wshape(wagon, f_shape, dist_level) ;
        }
/*
 * Plot bounding box
 */
        glDisable(GL_TEXTURE_2D) ;
        glDisable(GL_LIGHTING) ;
        glColor3f(1.0,1.0,1.0) ;
        x1 = shape->esd_bounding_box_xmin ;
        x2 = shape->esd_bounding_box_xmax ;
        y1 = shape->esd_bounding_box_ymin ;
        y2 = shape->esd_bounding_box_ymax ;
        z1 = shape->esd_bounding_box_zmin ;
        z2 = shape->esd_bounding_box_zmax ;
        glBegin(GL_LINE_STRIP) ;
          glVertex3d(x1, y1, z1) ;
          glVertex3d(x2, y1, z1) ;
          glVertex3d(x2, y2, z1) ;
          glVertex3d(x1, y2, z1) ;
          glVertex3d(x1, y1, z1) ;

          glVertex3d(x1, y1, z2) ;
          glVertex3d(x2, y1, z2) ;
          glVertex3d(x2, y2, z2) ;
          glVertex3d(x1, y2, z2) ;
          glVertex3d(x1, y1, z2) ;
        glEnd() ;

        glPopMatrix() ;
/*
 *  Add name
 */
        y1 = y0 + i*30*scale ;
        x1 = x0 + j*30*scale ;
        glColor3f(1.0,1.0,1.0) ;
        sprintf(string," - WAGON    %s",wnode->name);
        print_string_in_window2((GLfloat) x1+0.001, (GLfloat) y1,
                                (GLfloat) 0.0, string);
        sprintf(string," - SHAPE    %s",shape->name);
        print_string_in_window2((GLfloat) x1+0.001, (GLfloat) y1,
                                (GLfloat) -1.0*scale, string);
        if(f_shape){
          sprintf(string," - FREIGHT    %s",f_shape->name);
          print_string_in_window2((GLfloat) x1+0.001, (GLfloat) y1,
                                  (GLfloat) -2.0*scale, string);
        }
        glColor3f(1.0,1.0,0.0) ;
        glEnable(GL_LIGHTING) ;
        glEnable(GL_TEXTURE_2D) ;
      }
      glDisable(GL_BLEND) ;
      glDisable(GL_ALPHA_TEST);

      return 0 ;
}

/*
 *==============================================================================
 *    Routine to display textures near the origin of the tile display
 *==============================================================================
 */
int display_textures(void){

  int          i, j   ;
  int          ip = 0 ;         // debug
  char         string[128]            ;
  GLfloat      x1, x2, y1, y2, x0, y0 ;
  TextureNode  *tx_node ;

      glEnable(GL_TEXTURE_2D) ;
      glEnable(GL_LIGHTING)   ;
      glEnable(GL_ALPHA_TEST) ;
      glAlphaFunc(GL_GREATER,0.5);

      x0 = 0 ;
      y0 = 0 ;
      for(tx_node=texturelist_beg,i=0,j=0;tx_node!=NULL;tx_node=tx_node->next,i++){
        if(10==i){
          j = j + 1;
          i = 0;
        }
        x1 = x0 + i*0.1 ; x2 = x1 + 0.1 ;
        y2 = y0 + j*0.1 ; y1 = y2 + 0.1 ;
        glBindTexture(GL_TEXTURE_2D, tx_node->gl_tex_ref_no) ;
        if(ip)printf(" tx_node = %p, i,j = %i %i,  x1, y1 = %f %f, x2, y2 ="
                     " %f %f :: %i\n",(void *)tx_node,i,j,
                                        x1,y1,x2,y2,tx_node->gl_tex_ref_no);
/*
 * When displaying textures light source now wanted
 */
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;

        glBegin(GL_QUADS) ;
          glTexCoord2f((GLfloat) 0.0,(GLfloat) 0.0) ;
          glVertex3f(x1,y1,(GLfloat) 0.001) ;
          glTexCoord2f((GLfloat) 1.0,(GLfloat) 0.0) ;
          glVertex3f(x2,y1,(GLfloat) 0.001) ;
          glTexCoord2f((GLfloat) 1.0,(GLfloat) 1.0) ;
          glVertex3f(x2,y2,(GLfloat) 0.001) ;
          glTexCoord2f((GLfloat) 0.0,(GLfloat) 1.0) ;
          glVertex3f(x1,y2,(GLfloat) 0.001) ;
        glEnd() ;
/*
 *  Add name
 */
        glDisable(GL_TEXTURE_2D) ;
        glDisable(GL_LIGHTING) ;
        sprintf(string,"   %s",tx_node->name);
        glColor3f(1.0,1.0,1.0) ;
        print_string_in_window2((GLfloat) x1, (GLfloat) y1-0.09, (GLfloat) 0.002, string);
        glColor3f(0.0,0.0,0.0) ;
        print_string_in_window2((GLfloat) x1, (GLfloat) y1-0.08, (GLfloat) 0.002, string);
        glEnable(GL_LIGHTING) ;
        glEnable(GL_TEXTURE_2D) ;
      }
/*
 *==============================================================================
 *    Display Wagon Textures
 *==============================================================================
 */
      x0 =  2 ;
      y0 =  0 ;
      j = 0 ;
      for(tx_node=wtexturelist_beg,i=0,j=0;tx_node!=NULL;tx_node=tx_node->next,i++){
        if(10==i){
          j = j + 1;
          i = 0;
        }
        x1 = x0 + i*0.1 ; x2 = x1 + 0.1 ;
        y2 = y0 + j*0.1 ; y1 = y2 + 0.1 ;

//        ip = !strcmp(tx_node->name,"brake3") ;

        glBindTexture(GL_TEXTURE_2D, tx_node->gl_tex_ref_no) ;
        if(ip)printf(" tx_node = %p, i,j = %i %i,  x1, y1 = %f %f, x2, y2 ="
                     " %f %f :: %i\n",(void *)tx_node,i,j,
                                        x1,y1,x2,y2,tx_node->gl_tex_ref_no);
        glBegin(GL_QUADS) ;
          glTexCoord2f((GLfloat) 0.0,(GLfloat) 0.0) ;
          glVertex3f(x1,y1,(GLfloat) 0.001) ;
          glTexCoord2f((GLfloat) 1.0,(GLfloat) 0.0) ;
          glVertex3f(x2,y1,(GLfloat) 0.001) ;
          glTexCoord2f((GLfloat) 1.0,(GLfloat) 1.0) ;
          glVertex3f(x2,y2,(GLfloat) 0.001) ;
          glTexCoord2f((GLfloat) 0.0,(GLfloat) 1.0) ;
          glVertex3f(x1,y2,(GLfloat) 0.001) ;
        glEnd() ;
/*
 *  Add name
 */
        glDisable(GL_TEXTURE_2D) ;
        glDisable(GL_LIGHTING) ;
        sprintf(string,"   %s",tx_node->name);
        glColor3f(1.0,1.0,1.0) ;
        print_string_in_window2((GLfloat) x1, (GLfloat) y1-0.09, (GLfloat) 0.002, string);
        glColor3f(0.0,0.0,0.0) ;
        print_string_in_window2((GLfloat) x1, (GLfloat) y1-0.08, (GLfloat) 0.002, string);
        glEnable(GL_LIGHTING) ;
        glEnable(GL_TEXTURE_2D) ;
      }
      glDisable(GL_TEXTURE_2D) ;
      glDisable(GL_ALPHA_TEST) ;

/*
 *==============================================================================
 *    Display Mipmaps
 *    Note glDeleteTextures does not appear to recycle Texture names
 *         The original code used a different texName for each mipmap of each
 *         texture.
 *         This version uses the texture mipmap reference number.  With the
 *         base and maximum level it gives the same result.
 *==============================================================================
 */

GLuint  texName[1] ;
int     width, height, size ;
int     k, l ;

      glGenTextures(4, texName) ;
      glEnable(GL_TEXTURE_2D) ;
      glEnable(GL_ALPHA_TEST) ;
//      x0 = ceil(x2+0.1) ;
      x0 = -0.2 ;         //  Plot on left
      y0 =  0.0 ;
      for(tx_node=texturelist_beg,i=0,l=0;tx_node!=NULL;tx_node=tx_node->next,i++){
        if(i<=190)continue ;
        l++ ;
        x1 = x0 - l*0.2 ;  x2 = x1 + 0.1   ;
        width  = tx_node->width  ;
        height = tx_node->height ;
        for(j=0;j<tx_node->n_textures;j++){
          y2 = y0 + j*0.1 ; y1 = y2 + 0.1 ;

          glBindTexture(GL_TEXTURE_2D, tx_node->gl_tex_ref_no) ;
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, j) ;
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  j) ;


          glBegin(GL_QUADS) ;
            glTexCoord2f((GLfloat) 0.0,(GLfloat) 0.0) ;
            glVertex3f(x1,y1,(GLfloat) 0.001) ;
            glTexCoord2f((GLfloat) 1.0,(GLfloat) 0.0) ;
            glVertex3f(x2,y1,(GLfloat) 0.001) ;
            glTexCoord2f((GLfloat) 1.0,(GLfloat) 1.0) ;
            glVertex3f(x2,y2,(GLfloat) 0.001) ;
            glTexCoord2f((GLfloat) 0.0,(GLfloat) 1.0) ;
            glVertex3f(x1,y2,(GLfloat) 0.001) ;
          glEnd() ;
/*
 *  Add name
 */
          glDisable(GL_TEXTURE_2D) ;
          glDisable(GL_LIGHTING) ;
          sprintf(string,"   %s",tx_node->name);
          glColor3f(0.0,0.0,0.0) ;
          print_string_in_window2((GLfloat) x1+0.1, (GLfloat) y1-0.08, (GLfloat) 0.002, string);
          sprintf(string,"   %i %i  %2X  %2X",width,height,tx_node->surface_format,
                                                           tx_node->surface_format_orig);
          glColor3f(0.0,0.0,0.0) ;
          print_string_in_window2((GLfloat) x1+0.1, (GLfloat) y1-0.06, (GLfloat) 0.002, string);
          glEnable(GL_LIGHTING) ;
          glEnable(GL_TEXTURE_2D) ;

          width  = width/2  ;
          height = height/2 ;

        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 10) ;
        if(i>100)break ;
      }
      glDeleteTextures(4, texName) ;
      return 0 ;
}
