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

//double   x_scale, y_scale, z_scale, x_last, y_last ;
double   x_last, y_last ;

int i_display = 0 ;


/*
 *==============================================================================
 *   Main display routine
 *==============================================================================
 */
void display(void){

int      ierr ;
int      i, j, k  ;
int      ip = 1   ;  // 0 = no printing
char     string[128];
TileListNode    *tnode ;

GLfloat  mat_amb[]  = {light0_ambi, light0_ambi, light0_ambi, 1.0} ;  //  Value in shadow
GLfloat  mat_dif[]  = {light0_diff, light0_diff, light0_diff, 1.0} ;  //  Value lighted
GLfloat  mat_spc[]  = {light0_spec, light0_spec, light0_spec, 1.0} ;  //  Highlights
GLfloat  high_shininess[] = { 100.0 };
GLfloat  mid_shininess[]  = {  50.0 };
GLfloat  low_shininess[]  = {  10.0 };
GLfloat  v4[4] ;


//      printf(" ++++++++++++++++++++++++++++++++++++++++++++++++++\n");
//      printf(" Enter display :: %i\n",i_display++) ;
//      printf(" ++++++++++++++++++++++++++++++++++++++++++++++++++\n");
      i_display++ ;
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

/*
 *  checks on the clip planes
 */
      glClipPlane(GL_CLIP_PLANE0, clip_0);
      if(iclip_on){
        if(ip)printf(" clip_0 = %f  %f  %f  %f\n",(float)clip_0[0],(float)clip_0[1],(float)clip_0[2],(float)clip_0[3]);
        glEnable(GL_CLIP_PLANE0);
      }else{
        glDisable(GL_CLIP_PLANE0);
      }
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
      sketch_track_routes() ;
      sketch_road_routes() ;
#endif
/*
 *   Call the topography Display List for each of the tiles
 */
      glShadeModel(GL_SMOOTH) ;
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
      for(tnode = tilelist_head; tnode != NULL; tnode=tnode->next){
        if(0!=tnode->gl_display_list)
                     glCallList((GLuint) tnode->gl_display_list) ;
      }
      if(0 && l_pp)printf(" PLOT TOPOGRAPHY.  Tiles plotted = %i\n",j) ;
      check_glerror2("My routine 'display', after plotting topography\n") ;
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
double      distance        ;
double      scale= 1.0/plot_scale ;

float   x, y, z ;
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
 *   Translate ModelView to origin of tile
 */
            global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                         wnode->tile_x,wnode->tile_y,
                         witem->X, witem->Y, witem->Z, &x, &y, &z) ;
/*
 *  Check if item is within window
 *  At this point the program is using MSTS axis convention
 */
  int  iret ;
            iret = check_in_scene( (GLdouble) x,  (GLdouble) y, (GLdouble) z,
                                  (GLdouble) 0.05, (GLdouble) 2.5) ;

            if(0==iret) continue ;
            if(ip)printf(" New world item : shape is in view\n") ;
            glMatrixMode(GL_MODELVIEW) ;
            glPushMatrix() ;
/*
 *  Move to shape final orientation and then position in geographic
 *  coordinates.
 *  Note apparent reversal of order of transformations
 */
            glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z) ;
            glEnable(GL_NORMALIZE) ;
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
            distance = 0.0 ;                              // NOTE
            for(i=0; i<snode->n_lod_controls; i++){
              lod_control = &(snode->lod_control[i]) ;
              if(ip)printf(" lod_control index = %i\n",i) ;
              for(j=0; j<lod_control->n_dist_levels; j++){
                if(ip)printf("    distance level index = %i\n",j) ;
                dist_level = &(lod_control->dist_level[j])  ;
                if(distance < dist_level->dlevel_selection) break ;
              }
              if(j == lod_control->n_dist_levels) continue ;
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
//  Disable culling
      glDisable(GL_CULL_FACE) ;
//  Restore counter-clockwise default for front-facing polygons
      glFrontFace(GL_CCW) ;

#endif
/*
 *   Display dynamic tracks
 */
#if 1
      display_dynamic_tracks() ;
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
TrkSectionNode *trk_sec_node   ;

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
      display_extra_data() ;   //  display_info.c

#endif                 //  End of Normal 3D Display Section
/*
 *   Code to print frames per second in bottom left of window
 *   Switch on/off with shift-z.
 *   It averages over five seconds but takes the first five
 *   seconds to initialise.
 */
      if(l_fps){
        glDisable(GL_LIGHTING) ;
        glDisable(GL_TEXTURE_2D) ;
        k = calls_per_second() ;
        if(k>0) fps = k ;
        sprintf(string,"Frames per second = %i",fps) ;
        glColor3f(1.0,1.0,1.0) ;
        print_string_in_window((GLfloat)20.0,(GLfloat)20.0,string) ;
      }
/*
 * Clean up at end of routine display
 */
      glutSwapBuffers();
      glFinish();     /* Wait until drawing done  */

      check_glerror2("My routine 'display', at end returns GL error\n") ;
      icount ++ ;
      if(l_pd)printf(" Exit  display()\n");
      l_pp = 0 ;  //  Turn off flag for: new display after position move
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

int      i, j    ;
double   scale = 1.0/plot_scale ;
GLfloat  x = 0., y=0.0 , z=0.0 ;
TrkVectorNode   *vec ;
TrkSectionNode  *trk_sec_node  ;
char     string[256] ;

      for(i=0;i<(int)track_db.trk_sections_array_size;i++){
         trk_sec_node = &track_db.trk_sections_array[i] ;

         if(2 > trk_sec_node->length_of_vector) continue ;

         vec = trk_sec_node->vector ;
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
          glVertex3d(x, y, z) ;
        }
        glEnd();
/*
 *  Code to identify track sections in 3D scene
 */
#if 0
        glColor3f(0.8,0.8,0.8) ;
        glBegin(GL_LINES) ;
          glVertex3d(x, y, z) ;
          z = z + 5.0*scale ;
          glVertex3d(x, y, z) ;
        glEnd() ;
        glColor3f(1.0,1.0,1.0) ;

        j = 1 ;
        sprintf(string," - %i  %i  %i  %i\n",trk_sec_node->index_of_node,
               trk_sec_node->type_of_node, trk_sec_node->length_of_vector, j ) ;
        print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string) ;

        sprintf(string," - TRACK %i %i :: %f %f\n",
                vec[j-1].tile_east_x,vec[j-1].tile_north_z,
                vec[j-1].east_x,vec[j-1].north_z);
        z = z - 2.0*scale ;
        print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
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
GLfloat  x, y, z ;
TrkVectorNode   *vec ;
TrkSectionNode  *road_sec_node ;

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
          glVertex3d(x, y, z) ;
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
        glRotatef((GLfloat) 90.0, (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0 ) ;
       glScalef(scale,scale,-scale) ;  // Change from left hand to right hand axes
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

ShapeNode  *snode          ;
LodControl *lod_control    ;
DistLevel  *dist_level     ;
int        gl_display_list ;
GLfloat    x1, y1,
          x0 = 0, y0 = 0,
          scale = 1.0/plot_scale ;
int        i, j, ierr, ip = 0 ;
char       string[128]     ;

      glFrontFace(GL_CW) ;
//  Cull back faces
      glCullFace(GL_BACK) ;
      glEnable(GL_CULL_FACE) ;

      glEnable(GL_BLEND) ;
      glEnable(GL_ALPHA_TEST) ;
      glAlphaFunc(GL_GREATER,0.5);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;

      for(snode=wshapelist_beg, i=0, j=0; snode!=NULL; snode=snode->next, i++){
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

        lod_control = &(snode->lod_control[0]) ;
        dist_level = &(lod_control->dist_level[0])  ;
        gl_display_list = dist_level->gl_display_list ;
        if(ip)printf("  Display: i,j = %i,%i wagon = %s, lod = %i, dist = %i, "
              " gl_display_list = %i\n", i,j,snode->name,i,j,gl_display_list);

        glCallList(gl_display_list) ;

        glPopMatrix() ;
/*
 *  Add name
 */
        glDisable(GL_TEXTURE_2D) ;
        glDisable(GL_LIGHTING) ;
        sprintf(string," - WAGON %s",snode->name);
        glColor3f(1.0,1.0,1.0) ;
        print_string_in_window2((GLfloat) x1+0.001, (GLfloat) y1, (GLfloat) 0.0, string);
        glEnable(GL_LIGHTING) ;
        glEnable(GL_TEXTURE_2D) ;
      }
      glDisable(GL_BLEND) ;
      glDisable(GL_ALPHA_TEST);

      return 0 ;
}

/*
 *==============================================================================
 *    Routine to display all the wagons in one of the unused tiles
 *    Using the display lists
 *==============================================================================
 */
int display_textures(void){

int          i, j   ;
int          ip = 0 ;         // debug
char         string[128]            ;
GLfloat      x1, x2, y1, y2, x0, y0 ;
TextureNode  *tnode ;

      glEnable(GL_TEXTURE_2D) ;
      glEnable(GL_LIGHTING)   ;
      glEnable(GL_ALPHA_TEST) ;
      glAlphaFunc(GL_GREATER,0.5);

      x0 = 0 ;
      y0 = 0 ;
      for(tnode=texturelist_beg,i=0,j=0;tnode!=NULL;tnode=tnode->next,i++){
        if(10==i){
          j = j + 1;
          i = 0;
        }
        x1 = x0 + i*0.1 ; x2 = x1 + 0.1 ;
        y2 = y0 + j*0.1 ; y1 = y2 + 0.1 ;
        glBindTexture(GL_TEXTURE_2D, tnode->gl_tex_ref_no) ;
        if(ip)printf(" tnode = %p, i,j = %i %i,  x1, y1 = %f %f, x2, y2 ="
                     " %f %f :: %i\n",(void *)tnode,i,j,
                                        x1,y1,x2,y2,tnode->gl_tex_ref_no);
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
        sprintf(string,"   %s",tnode->name);
        glColor3f(1.0,1.0,1.0) ;
        print_string_in_window2((GLfloat) x1, (GLfloat) y1-0.09, (GLfloat) 0.002, string);
        glColor3f(0.0,0.0,0.0) ;
        print_string_in_window2((GLfloat) x1, (GLfloat) y1-0.08, (GLfloat) 0.002, string);
        glEnable(GL_LIGHTING) ;
        glEnable(GL_TEXTURE_2D) ;
      }

      x0 =  2 ;
      y0 =  0 ;
      for(tnode=wtexturelist_beg,i=0,j=0;tnode!=NULL;tnode=tnode->next,i++){
        if(10==i){
          j = j + 1;
          i = 0;
        }
        x1 = x0 + i*0.1 ; x2 = x1 + 0.1 ;
        y2 = y0 + j*0.1 ; y1 = y2 + 0.1 ;
        glBindTexture(GL_TEXTURE_2D, tnode->gl_tex_ref_no) ;
        if(ip)printf(" tnode = %p, i,j = %i %i,  x1, y1 = %f %f, x2, y2 ="
                     " %f %f :: %i\n",(void *)tnode,i,j,
                                        x1,y1,x2,y2,tnode->gl_tex_ref_no);
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
        sprintf(string,"   %s",tnode->name);
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


