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


int plot_tile_outlines(void)  ;  // Plot outlines of all tiles
int sketch_track_routes(void) ;  // Sketch routes of tracks
int sketch_road_routes(void)  ;  // Sketch routes of roads
int display_shapes(void)      ;  // Plot all shapes on a grid
int display_wagons(void)      ;  // Plot all wagons on a grid
int display_textures(void)    ;  // Plot all textures on a grid

int    i_display = 0  ;
double x_last, y_last ;

/*
 *==============================================================================
 *   Main display routine
 *==============================================================================
 */
void display(void){

int      iret ;
int      i, j, k  ;
int      ip = 0   ;  // DEBUG
char     string[128];
TileListNode    *tl_node ;

clock_t       tile_t_beg,  tile_t_end  ;
clock_t       shape_t_beg, shape_t_end ;
clock_t       track_t_beg, track_t_end ;
clock_t       dtrack_t_beg, dtrack_t_end ;
static double tile_time_used, shape_time_used, track_time_used,
              dtrack_time_used  ;
int           n_tiles_plotted, n_shapes_plotted, n_tracks_plotted ;
char          *my_name = "display" ;

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

      zr_clock_gettime(zr_clock_1) ;
#ifdef _Display_Normal
      update_trains(zr_clock_1[1]) ;  // Update using monotonic time
#endif
/*
 *  If position has changed - update viewpoint
 */
      if(camera_changed)camera_new_position() ;
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
      glEnable(GL_NORMALIZE) ;
      glDisable(GL_LINE_STIPPLE) ;
      glDisable(GL_TEXTURE_2D) ;

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
      glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS, mid_shininess) ;
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
 *   Display Wagons
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
      sketch_track_routes() ;
      sketch_road_routes() ;
      track_t_end = clock() ;
      n_tracks_plotted = 1 ;
#endif

/*
 *==============================================================================
 *   Call the topography Display List for each of the tiles
 *==============================================================================
 */
#if 1
      tile_t_beg = clock() ;
      n_tiles_plotted = 0  ;

      glShadeModel(GL_SMOOTH) ;
//      glShadeModel(GL_FLAT) ;
      glEnable(GL_LIGHTING);

      glCullFace(GL_BACK) ;
      glEnable(GL_CULL_FACE) ;
      if(land_texture){
        glEnable(GL_TEXTURE_2D) ;
        glDisable(GL_BLEND) ;

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;
      }

#ifdef use_vertex_arrays
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
        if(0==check_topog_in_scene(tl_node)) continue ;
#ifdef use_vertex_arrays
        display_tile_vertex_array(tl_node) ;
#else
        glCallList((GLuint) tl_node->gl_display_list) ;
#endif
        n_tiles_plotted++ ;
      }
/*
 *   Disable Cleint States.  Reset Lights and Material Properties
 */
#ifdef use_vertex_arrays
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
      glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS, mid_shininess) ;
#endif
//      glFinish() ; // Clear GPU
      tile_t_end   = clock() ;
      if(0 && l_pp)printf(" PLOT TOPOGRAPHY.  Tiles plotted = %i\n",j) ;
      check_glerror2("My routine 'display', after plotting topography\n") ;
#endif
/*
 *==============================================================================
 *  End of topography
 *==============================================================================
 */


/*
 *==============================================================================
 *  Loop over the world files
 *  For each shape encountered:
 *      1.  set transformation
 *      2.  call  display list
 *==============================================================================
 */
#if 1
      {                 //  New block
WorldNode    *wnode ;
WorldItem    *witem ;
ShapeNode    *snode ;
LodControl  *lod_control    ;
DistLevel   *dist_level     ;

int         gl_display_list ;
int         l_here = 0      ;
double      dist            ;
double      scale= 1.0/plot_scale ;

double   x, y, z ;
GLfloat a, b, c, d ;

      glEnable(GL_LIGHTING);
      glEnable(GL_BLEND) ;
//  Define front face polygons as going clockwise
//  Needed for MSTS shapes after transforms
      glFrontFace(GL_CW) ;
//  Cull back faces but this should not be used for wagons as some parts
//  are then missing.
      glCullFace(GL_BACK) ;
      glEnable(GL_CULL_FACE) ;
      glEnable(GL_RESCALE_NORMAL) ;
//      glEnable(GL_NORMALIZE) ;

//      glFlush() ;
//      glFinish() ; // Clear GPU
      shape_t_beg = clock() ;
      for(wnode=worldlist_beg; wnode != NULL; wnode=wnode->next){
        if(0 == wnode->in_use) continue ;
        if(0 && l_pp)printf(" New tile tile_x, tile_y, in_use = %i %i %i\n",
                           wnode->tile_x, wnode->tile_y,  wnode->in_use ) ;
        for(witem=wnode->world_item; witem != NULL; witem=witem->next){
          snode = witem->snode ;
/*
 *  Shape nodes causing errors !!
 *  To be investigated ... .
 */
          if(NULL == snode)continue ;
          if(NULL == snode->name)continue ;
          if(FOREST == witem->worldtype)continue ;
          if(DYNTRACK == witem->worldtype)continue ;
          if(306 == witem->worldtype)continue ;


//  Debug printing if new position and and first time with debug statements
//  String 'test_shape' is defined in "zr.c".

          ip = 0 && (l_pp || i_display<2) && l_here &&
                                      !strncmp(snode->name,test_shape,12)  ;
          if(ip)printf(" New world item : shape name  = %s\n",snode->name) ;
/*
 *   Convert from MSTS location to coordinates used for graphics
 */
            global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                         wnode->tile_x,wnode->tile_y,
                         witem->X, witem->Y, witem->Z, &x, &y, &z) ;
/*
 *  Check if item is within window
 *  At this point the program is using the MSTS axis convention
 */
            iret = check_in_scene( (GLdouble) x,  (GLdouble) y, (GLdouble) z,
                                  (GLdouble) 0.05, (GLdouble) 2.5) ;

            if(0==iret) continue ;
            if(ip)printf(" New world item : shape is in view\n") ;
            n_shapes_plotted++ ;

            glMatrixMode(GL_MODELVIEW) ;
            glPushMatrix() ;
/*
 *  Move to shape final orientation and then position in geographic
 *  coordinates.
 *  Note apparent reversal of order of transformations
 */
            glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z) ;
//            glEnable(GL_NORMALIZE) ;
# if 0
            if(wnode->tile_x == 1448 && wnode->tile_y == 10332
              && (witem->uid == 6 || witem->uid == 7)){
              printf(" GGG tile_x = %i, tile_y = %i, uid = %i, shape = %s\n",
                     wnode->tile_x, wnode->tile_y, witem->uid, witem->filename) ;
              printf(" HHH  X, Y, Z = %f %f %f, ANG : AX AY AZ = %f : %f %f %f\n",
                     witem->X, witem->Y, witem->Z, witem->ANG, witem->AX, witem->AY, witem->AZ) ;
            }
#endif
            a = witem->ANG ;
            b = witem->AX ;
            c = witem->AY ;
            d = witem->AZ ;
            glRotatef(a,b,d,c) ;    // Move to final position and rotate
/*
 *  Convert from MSTS axes (x-east, y-up, z-north) used in shape files
 *    to geographical axes (x-east, y-north, z-up)
 */
#ifdef geo_coord
            glDisable(GL_CULL_FACE) ;
            glScalef(scale,scale,scale) ;
#else
            glRotatef((GLfloat) 90.0, (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0 ) ;
            glScalef(scale,scale,-scale) ;  // Change from left hand to right hand axes
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
              for(j=0; j<lod_control->n_dist_levels; j++){
                if(ip)printf("    distance level index = %i\n",j) ;
                dist_level = &(lod_control->dist_level[j])  ;
                if(dist < dist_level->dlevel_selection) break ;
              }
              if(j == lod_control->n_dist_levels){
//                glPopMatrix() ;
                continue ;
              }

//  Keep the 'optimised' compiler happy
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
              gl_display_list = dist_level->gl_display_list ;
#pragma GCC diagnostic pop
              if(gl_display_list <= 0) continue ;
              if(ip && i_display<2)
              printf("  Display: shape = %s, lod = %i, dist = %i, gl_display_list = %i\n",
                                         snode->name,i,j,gl_display_list);
              glEnable(GL_TEXTURE_2D) ;
              glEnable(GL_BLEND) ;
              glEnable(GL_ALPHA_TEST) ;
              glAlphaFunc(GL_GREATER,0.5);
              glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

              glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;

              glCallList(gl_display_list) ;

              glDisable(GL_BLEND) ;
              glDisable(GL_ALPHA_TEST);
              glDisable(GL_TEXTURE_2D) ;
            }
/*
 *  Print position of shape on screen
 */
#if 0
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
            glEnable(GL_LIGHTING) ;
#endif

            glPopMatrix() ;
            if(ip)l_here = 0 ;  // Turn off local logic once example processed
          }
        }
      }   // END of plot shapes block
//      glFlush() ;
//      glFinish() ; // Clear GPU
      shape_t_end = clock() ;
      glDisable(GL_RESCALE_NORMAL) ;
//  Disable culling
      glDisable(GL_CULL_FACE) ;
//  Restore counter-clockwise default for front-facing polygons
      glFrontFace(GL_CCW) ;

#endif
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
      dtrack_t_beg = clock() ;
      display_dynamic_tracks() ;
//      glFlush() ;
//      glFinish() ; // Clear GPU
      dtrack_t_end   = clock() ;
#endif

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
  double   x, y, z, dist  ;
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

#ifdef culling_off_for_wagons
      glDisable(GL_CULL_FACE) ;
#else
      glCullFace(GL_BACK) ;
      glEnable(GL_CULL_FACE) ;
#endif
      glFrontFace(GL_CW) ;

      glEnable(GL_TEXTURE_2D) ;
      glEnable(GL_BLEND) ;
      glEnable(GL_ALPHA_TEST) ;
      glAlphaFunc(GL_GREATER,0.1);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;

      for(train = trainlist_beg; train != NULL ; train=train->next){
        for(wagon = train->first; wagon != NULL; wagon = wagon->next){

          tv = wagon->traveller ;
          vn = tv->vn ;
          a = degree*vn->a_east_x   ;
          b = degree*vn->a_height_y ;
          c = degree*vn->a_north_z  ;
/*
 *   Convert from MSTS location to coordinates used for graphics
 */
          global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                         vn->tile_east_x, vn->tile_north_z,
                         vn->east_x , vn->north_z, vn->height_y+rail_height,
                         &x, &y, &z) ;
          if(it)printf("\n TTTA  x, y, z = %f %f %f :: %i %i  :: %f %f %f : %f \n",
                         (double)x,(double)y,(double)z,
                         vn->tile_east_x, vn->tile_north_z,
                         vn->east_x, vn->height_y, vn->north_z, scale) ;
/*
 * Check the wagon is within view - skip if outside view or far away
 */
          iret = check_in_scene( (GLdouble) x,  (GLdouble) y, (GLdouble) z,
                                (GLdouble) 0.05, (GLdouble) 2.5) ;

          if(0==iret) continue ;

          dist = sqrt((x-lookat_eye_x)*(x-lookat_eye_x)
                    + (y-lookat_eye_y)*(y-lookat_eye_y)
                    + (z-lookat_eye_z)*(z-lookat_eye_z) )*plot_scale ;

          glMatrixMode(GL_MODELVIEW) ;
          glPushMatrix() ;
            glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z) ;
            glRotatef((GLfloat) 90.0, (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0 ) ;
            glScalef(scalei,scalei,-scalei) ;
            glRotatef((GLfloat)b,(GLfloat)0.0,(GLfloat)1.0,(GLfloat)0.0) ;
            glRotatef((GLfloat)a,(GLfloat)1.0,(GLfloat)0.0,(GLfloat)0.0) ;
            glRotatef((GLfloat)c,(GLfloat)0.0,(GLfloat)0.0,(GLfloat)1.0) ;

            glTranslatef((GLfloat)(tv->x),(GLfloat)(tv->y),(GLfloat)(tv->z)) ;
#if 0
            if(0==strcmp(wagon->name,"4W-HG-Brake-Van")){
              printf("  Display :: Wagon %s, w->train_dir = %i, tv->ang_deg = %f\n",
                        wagon->name, wagon->train_dir,tv->ang_deg) ;
            }
#endif
            if(wagon->train_dir){
              glRotatef((GLfloat)(-tv->ang_deg),(GLfloat)0.0,(GLfloat)1.0,(GLfloat)0.0) ;
            }else{
              glRotatef((GLfloat)(180.0-tv->ang_deg),
                        (GLfloat)0.0,(GLfloat)1.0,(GLfloat)0.0) ;
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
            glColor3f((GLfloat)1.0,(GLfloat)0.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) -1.0, (GLfloat)4.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat)  5.0, (GLfloat)4.0,(GLfloat)0.0) ;

            glColor3f((GLfloat)0.0,(GLfloat)1.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)-1.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat) 5.0,(GLfloat)0.0) ;

            glColor3f((GLfloat)0.0,(GLfloat)0.0,(GLfloat)1.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)4.0,(GLfloat)-1.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)4.0,(GLfloat) 5.0) ;

            glEnable(GL_LIGHTING) ;
            glEnable(GL_TEXTURE_2D) ;
#endif
            dist_level = NULL ;
            for(i=0; i<wagon->shape->n_lod_controls; i++){
              lod_control = &(wagon->shape->lod_control[i]) ;

              for(j=0; j<lod_control->n_dist_levels; j++){
                if(ip)printf("    distance level index = %i\n",j) ;
                dist_level = &(lod_control->dist_level[j])  ;
                if(dist < dist_level->dlevel_selection) break ;
              }
              if(j == lod_control->n_dist_levels) continue ;

              display_shape(wagon, wagon->shape, lod_control, dist_level) ;
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
                  if(ip)printf("    distance level index = %i\n",j) ;
                  dist_level = &(lod_control->dist_level[j])  ;
                  if(dist < dist_level->dlevel_selection) break ;
                }
                if(j == lod_control->n_dist_levels) continue ;

                display_shape(wagon, f_shape, lod_control, dist_level) ;
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
            glColor3f((GLfloat)1.0,(GLfloat)0.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) -1.0, (GLfloat)4.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat)  5.0, (GLfloat)4.0,(GLfloat)0.0) ;

            glColor3f((GLfloat)0.0,(GLfloat)1.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)-1.0,(GLfloat)0.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat) 5.0,(GLfloat)0.0) ;

            glColor3f((GLfloat)0.0,(GLfloat)0.0,(GLfloat)1.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)4.0,(GLfloat)-1.0) ;
            glVertex3f((GLfloat) 0.0, (GLfloat)4.0,(GLfloat) 5.0) ;

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


/*
 *   Plot default rail tracks.  Used when no track shapes available
 *   Assume LOD method is component additive (flag: COMPONENTADDITIVE).
 *   For the moment this does not check distance
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
TrkNetNode *trk_sec_node   ;

      for(i=0;i<track_db.trk_sections_array_size;i++){
        trk_sec_node = &(track_db.trk_sections_array[i])       ;
        if(trk_sec_node->type_of_node != VECTOR_SECTION) continue ;
#if 1
        glEnable(GL_TEXTURE_2D) ;
        glEnable(GL_BLEND) ;
        glEnable(GL_ALPHA_TEST) ;
        glAlphaFunc(GL_GREATER,0.5);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
          if(i_display<2 && (trk_sec_node->index_of_node == 123)){
             printf("  Call default track display list %i %i :: d list = %i \n",
                      trk_sec_node->index_of_node, j, gl_display_list) ;

          }
          glCallList(gl_display_list) ;
        }
      }
      glDisable(GL_TEXTURE_2D) ;
}
#endif
/*
 *   Plot default roads.   Used when no track shapes available
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
      display_extra_data() ;   //  display_info.c
#endif

#endif                 //  End of Normal 3D Display Section
/*
 *   Code to print frames per second in bottom left of window
 *   Switch on/off with shift-z.
 *   It averages over five seconds but takes the first five
 *   seconds to initialise.
 */
#ifdef _Display_Normal
      if(l_fps){
        glDisable(GL_LIGHTING) ;
        glDisable(GL_TEXTURE_2D) ;
        glEnable(GL_BLEND) ;

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
//        sprintf(string,"Frames per second = %i.  Tiles %i, time %7.3f  "
//                                               " Shapes %i, time %7.3f  "
//                                               " Dy Track %i, time %7.3 ms.",
        sprintf(string,"Frames per second = %i.  Tiles %i time %7.3f "
                                                "Shapes %i time %7.3f "
                                                "Tracks %i %7.3f %7.3f",
                                     fps, n_tiles_plotted, tile_time_used,
                                          n_shapes_plotted, shape_time_used,
                                          n_tracks_plotted, track_time_used,
                                          dtrack_time_used
               ) ;
        glColor3f(1.0,1.0,1.0) ;
        print_string_in_window((GLfloat)20.0,(GLfloat)20.0,string) ;

        sprintf(string,"Run time = %9.2f     Speed = %7.2f",
                           run_seconds, player_train->speed );
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
#endif
/*
 *  Special Panels for Normal Display
 *  These use gluOrtho to generate a 2D image in front of the main 3D screeen
 *  This is scaled so that one unit equals one pixel
 */
#ifdef _Display_Normal
{
GLfloat w = viewport_width ;
GLfloat h = viewport_height ;

      glDisable(GL_LIGHTING) ;
      glDisable(GL_TEXTURE_2D) ;
      glEnable(GL_BLEND) ;

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
      if(display_track_info_on) display_track_info(player_train->first->traveller) ;
      if(display_help_on)       display_help() ;
      if(display_switches_on)   display_switches(player_train) ;
//  Restore defaults

      glPopMatrix() ;  //  Pop GL_MODELVIEW
      glMatrixMode(GL_PROJECTION) ;
      glPopMatrix() ;  //  Pop GL_PROJECTION
      glMatrixMode(GL_MODELVIEW) ;

      glEnable(GL_LIGHTING) ;
      glEnable(GL_TEXTURE_2D) ;
}
#endif
/*
 * Clean up at end of routine display
 */
//      glFlush() ;
//      glFinish();     /* Wait until drawing done  */

#ifndef SDL2
      glutSwapBuffers();
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

      check_glerror2("My routine 'display', at end returns GL error\n") ;
      icount ++ ;
      if(l_pd)printf(" Exit  display()\n");
      l_pp = 0    ;  //  Turn off flag for: new display after position move
      l_disp1 = 0 ;  //                     printing
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

      i1  =  tile_west  - tile_x0 ;   //  Should be zero
      i2  =  tile_east  - tile_x0 ;
      j1  =  tile_south - tile_y0 ;   //  Should be zero
      j2  =  tile_north - tile_y0 ;

      glDisable(GL_LINE_STIPPLE) ;
      glDisable(GL_LIGHTING);
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
int sketch_track_routes(void){

int      i, j, k    ;
double  scale = 1.0/plot_scale ;
double  x = 0., y=0.0 , z=0.0 ;
TrkVectorNode   *vec, *v ;
TrkNetNode      *trk_sec_node  ;
WorldItem       *w ;
DynTrackSect    *d ;
char     string[256] ;

      for(i=0;i<(int)track_db.trk_sections_array_size;i++){
        trk_sec_node = &track_db.trk_sections_array[i] ;
        vec = trk_sec_node->vector ;
        if(0 < trk_sec_node->length_of_vector) {
          glShadeModel(GL_FLAT) ;
          for(j=0;j<(int)trk_sec_node->length_of_vector;j++){
            global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                  vec[j].tile_east_x, vec[j].tile_north_z,
                  vec[j].east_x, vec[j].north_z, vec[j].height_y + 1.0,
                  &x, &y, &z );
            ortho_col++ ;
            if(0 == j){
              glBegin(GL_LINE_STRIP) ;
              glColor3f(1.0,1.0,1.0) ;
              ortho_col = 0 ;
            }else if(1 == j)glColor3f(1.0,1.0,1.0) ;
            else if(1 == ortho_col)glColor3f(0.5,0.5,0.5) ;
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
          }
          glEnd();
        }
/*
 *  Code to identify track sections in 3D scene
 */
#if 1
        for(j=0;j<(int)trk_sec_node->length_of_vector;j++){
          v = &vec[j] ;
          w = v->world_item ;
//          if(v->flag1<2 && v->flag2<2) continue ;
          global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                vec[j].tile_east_x, vec[j].tile_north_z,
                vec[j].east_x, vec[j].north_z, vec[j].height_y,
                &x, &y, &z );
          glColor3f(0.8,0.8,0.8) ;
          glBegin(GL_LINES) ;
            glVertex3d(x, y, z) ;
            z = z + 6.0*scale ;
            glVertex3d(x, y, z) ;
          glEnd() ;
          glColor3f(1.0,1.0,1.0) ;
#if 1
          glEnable(GL_BLEND) ;
          glDisable(GL_LIGHTING)   ;
          glDisable(GL_TEXTURE_2D) ;
          sprintf(string," - %i  %i  %i  %i ::F %i %i\n",
                  trk_sec_node->index_of_node,trk_sec_node->type_of_node,
                  trk_sec_node->length_of_vector,j,v->flag1, v->flag2) ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string) ;

          sprintf(string," - TRACK %i %i :: %10.3f  %10.3f  %10.3f\n",
                  vec[j].tile_east_x,vec[j].tile_north_z,
                  vec[j].east_x,vec[j].north_z,vec[j].height_y);
          z = z - 1.0*scale ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);

          if(w->worldtype==306){
            for(k=0;k<5;k++){
              d = &w->u.dyn_track_obj.dyn_trk_sect[k] ;
              if(d->uid==-1)continue ;
              sprintf(string," - %i %i  (%i %.2f %.1f)",v->flag1,v->flag2,d->is_curved, d->param_1,d->param_2);
              z = z - 1.0*scale ;
              glColor3f((GLfloat)v->flag1/3.0,0.0,(GLfloat)v->flag2/3.0) ;
              print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
              glColor3f(1.0,1.0,1.0) ;
            }
          }else{
              glColor3f(0.6,0.6,0.6) ;
              sprintf(string," - World Type = %i %s",w->worldtype,token_idc[w->worldtype]);
              z = z - 1.0*scale ;
              print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
              glColor3f(1.0,1.0,1.0) ;
          }
          glEnable(GL_LIGHTING)   ;
          glEnable(GL_TEXTURE_2D) ;
          glDisable(GL_BLEND) ;
#endif
        }
#endif
      }
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
TrkNetNode  *road_sec_node ;

      for(i=0;i<(int)road_db.trk_sections_array_size;i++){
        road_sec_node = &road_db.trk_sections_array[i] ;
        if(2 > road_sec_node->length_of_vector) continue ;

        vec = road_sec_node->vector ;
        glShadeModel(GL_FLAT) ;
        for(j=0;j<(int)road_sec_node->length_of_vector;j++){
#if 0
           x = (vec[j].tile_east_x  - tile_x0 - 0.5)*plot_scale + vec[j].east_x  ;
           y = (vec[j].tile_north_z - tile_y0 - 0.5)*plot_scale + vec[j].north_z ;
           z = vec[j].height_y - tile_h0 + 1.0 ;
           x = x*scale ;
           y = y*scale ;
           z = z*scale ;
#else
          global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                 vec[j].tile_east_x, vec[j].tile_north_z,
                 vec[j].east_x, vec[j].north_z, vec[j].height_y + 1.0,
                 &x, &y, &z );
#endif
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
        sprintf(string," - ROAD %i\n",road_sec_node->index_of_node);
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
 *    Routine to display all the shapes in tile (0,0)
 *    Using the display lists
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

      for(snode=shapelist_beg, i=0, j=0; snode!=NULL; snode=snode->next, i++){
        if(30==i){
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
# if 1
        glRotatef((GLfloat) d_rotate, (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0 ) ;
        glScalef(scale,scale,d_reflect*scale) ;  // Change from left hand to right hand axes
#else
        glRotatef((GLfloat) 90.0, (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0 ) ;
        glScalef(scale,scale,-scale) ;  // Change from left hand to right hand axes
# endif
#endif


        lod_control = &(snode->lod_control[0]) ;
        dist_level = &(lod_control->dist_level[0])  ;
        gl_display_list = dist_level->gl_display_list ;

        if(ip)printf("  Display: shape = %s, lod = %i, dist = %i, gl_display_list = %i\n",
                  snode->name,i,j,gl_display_list);

        glEnable(GL_TEXTURE_2D) ;
        glEnable(GL_BLEND) ;
        glEnable(GL_ALPHA_TEST) ;
        glAlphaFunc(GL_GREATER,0.5);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        ierr = glGetError() ;
        if(ierr != GL_NO_ERROR && icount++<100)
                                      printf(" FFF  GL error %i \n",ierr);
        if(0 != gl_display_list) glCallList(gl_display_list) ;
        ierr = glGetError() ;
        if(ierr != GL_NO_ERROR && icount++<100){
                printf(" GGG  GL error        %i \n",ierr);
                printf(" GGG  gl_display_list %i \n",gl_display_list);
        }
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

ShapeNode  *shape,
           *f_shape         ;
RawWagonNode *wnode        ;
WagonNode  *wagon = trainlist_beg->first ;  // Dummy wagon
LodControl *lod_control    ;
DistLevel  *dist_level     ;
int        gl_display_list ;
GLfloat    x1, y1, z1, x2, y2, z2,
           x0 = 0, y0 = 0,
           scale = 1.0/plot_scale ;
int        i, j, ierr, ip = 0 ;
char       string[128]     ;

      glEnable(GL_LIGHTING) ;

#ifdef culling_off_for_wagons
      glDisable(GL_CULL_FACE) ;
#else
      glCullFace(GL_BACK) ;
      glEnable(GL_CULL_FACE) ;
#endif
      glFrontFace(GL_CW) ;

      glEnable(GL_BLEND) ;
      glEnable(GL_ALPHA_TEST) ;
      glAlphaFunc(GL_GREATER,0.5);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;

//      for(snode=wshapelist_beg, i=0, j=0; snode!=NULL; snode=snode->next, i++){
      for(wnode=rawwagonlist_beg, i=0, j=0; wnode!=NULL; wnode=wnode->next, i++){
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
        glRotatef((GLfloat) 90.0, (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0 ) ;
        glScalef(scale,scale,-scale) ;  // Change from left hand to right hand axes
#endif

        shape = wnode->shape ;
        lod_control = &(shape->lod_control[0]) ;
        dist_level = &(lod_control->dist_level[0])  ;
#if 0
        gl_display_list = dist_level->gl_display_list ;
        if(ip)printf("  Display: i,j = %i,%i wagon = %s, lod = %i, dist = %i, "
              " gl_display_list = %i\n", i,j,shape->name,i,j,gl_display_list);
        glCallList(gl_display_list) ;
        printf(" FF ") ;
#else
        display_shape(wagon, shape, lod_control, dist_level) ;
#endif


/*
 *  Display load
 */
        f_shape = wnode->f_shape ;
        if(f_shape != NULL){
          lod_control = &(f_shape->lod_control[0]) ;
          dist_level = &(lod_control->dist_level[0])  ;
#if 0
          gl_display_list = dist_level->gl_display_list ;
          if(ip)printf("  Display: i,j = %i,%i load  = %s, lod = %i, dist = %i"
                       "\n",i,j,f_shape->name,i,j,
                         gl_display_list);
          glCallList(gl_display_list) ;
#else
          if(ip)printf("  Display: i,j = %i,%i load  = %s, lod = %p, dist = %p\n"
                       ,i,j,f_shape->name,(void *)lod_control,
                                          (void *)dist_level) ;
          display_shape(wagon, f_shape, lod_control, dist_level) ;
#endif
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
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;

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
        glBindTexture(GL_TEXTURE_2D, tx_node->gl_tex_ref_no) ;
        if(ip)printf(" tx_node = %p, i,j = %i %i,  x1, y1 = %f %f, x2, y2 ="
                     " %f %f :: %i\n",(void *)tx_node,i,j,
                                        x1,y1,x2,y2,tx_node->gl_tex_ref_no);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;

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
      return 0 ;
}


