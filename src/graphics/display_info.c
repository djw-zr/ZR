/*
 * =============================================================================
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  display_info.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file containing the structures used by ZR to store the data
 *   included in each world file.
 *
 * =============================================================================
 */

int   display_extra_data(){

int        ip = 0  ;
int        tx, ty  ;
GLfloat    xc, yc, zc   ;
GLfloat    vx, vy, vz   ;
GLfloat    xx, yy, zz   ;
GLfloat    m2d = 1.0/plot_scale ;  // Convert from metres to display usints
GLfloat    dist, dist0, height0  ;

WorldNode   *wnode ;
WorldItem   *witem ;
ShapeNode   *snode ;
LodControl  *lod_control ;
DistLevel   *dist_level  ;
char        string[2048] ;

      if(!display_info) return 0 ;
      xc = lookat_center_x ;
      yc = lookat_center_y ;
      zc = lookat_center_z ;

      dist0   = m2d*display_info_radius ;
      height0 = m2d*display_info_height ;
      dist0   = dist0*dist0 ;

      glDisable(GL_LIGHTING) ;
      glDisable(GL_TEXTURE_2D) ;

      glColor3f((GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0) ;
      glBegin(GL_LINES) ;
        glVertex3d(xc, yc, (GLfloat) 0.0) ;
        glVertex3d(xc, yc, (GLfloat) 1.0) ;
      glEnd() ;

      glColor3f((GLfloat)1.0,(GLfloat)1.0,(GLfloat)1.0) ;
      sprintf(string," - Lookat Point  :: %f %f %f ", xc,yc,zc);
      print_string_in_window2((GLfloat) xc, (GLfloat) yc, (GLfloat) zc, string);
      glColor3f((GLfloat)1.0,(GLfloat)1.0,(GLfloat)1.0) ;

      for(wnode=worldlist_beg; wnode != NULL; wnode=wnode->next){
        if(0 == wnode->in_use) continue ;
        tx = wnode->tile_x - tile_x0 ;
        ty = wnode->tile_y - tile_y0 ;
        for(witem = wnode->world_item ; witem != NULL; witem = witem->next){
          xx = (tx+0.5)*tile_size +  witem->X ;
          yy = (ty+0.5)*tile_size +  witem->Y ;
          zz = witem->Z - tile_h0        ;
          xx = xx*m2d ;
          yy = yy*m2d ;
          zz = zz*m2d ;
          dist = (xx-xc)*(xx-xc) + (yy-yc)*(yy-yc) ;
          if(dist > dist0)continue ;

          glColor3f((GLfloat)0.0,(GLfloat)1.0,(GLfloat)1.0) ;
          glBegin(GL_LINES) ;
            glVertex3d(xx, yy, zz) ;
            zz = zz + height0      ;
            glVertex3d(xx, yy, zz) ;
          glEnd() ;
          glColor3f(1.0,1.0,1.0) ;
          sprintf(string," - WORLD   :: uid = %i, type =  %i"
                         " :: item at :: %f %f %f ",
                         witem->uid,witem->worldtype,xx,yy,zz);
          print_string_in_window2((GLfloat) xx, (GLfloat) yy, (GLfloat) zz, string);
        }
      }

      glEnable(GL_LIGHTING) ;

      return 0 ;
}

int  display_help(){

  GLfloat       w = viewport_width ;
  GLfloat       h = viewport_height ;
  char          s1[1024] ;

 //
// Plot grey background for text
//
      glColor4f(0.8,0.8,0.8,0.9) ;
      glBegin(GL_QUADS) ;
        glVertex2f(  0.0,h - 280.0) ;
        glVertex2f(900.0,h - 280.0) ;
        glVertex2f(900.0,h) ;
        glVertex2f(  0.0,h) ;
      glEnd() ;
/*
 *  Generate Main HELP Panel
 */
      glColor3f(0.0,0.0,0.0) ;

      sprintf(s1," Help Panel - with developer options");
      print_string_in_window3(10.0,h-25.0,s1,GLUT_BITMAP_HELVETICA_12) ;
      sprintf(s1," Keyboard keys:");
      print_string_in_window3(10.0,h-45.0,s1,GLUT_BITMAP_HELVETICA_12) ;
      sprintf(s1," Speed     : 'a' - decrease, 's' - stop,  'd' - increase speed forward");
      print_string_in_window3(10.0,h-70.0,s1,GLUT_BITMAP_HELVETICA_12) ;
      sprintf(s1," Cameras : '1' - Driver, '2' - Front, '3' - Rear, '4' - Trackside, '5' - Passenger, '6' - Coupler, '7' - Yard view, '8' - Default view");
      print_string_in_window3(10.0,h-90.0,s1,GLUT_BITMAP_HELVETICA_12) ;
      sprintf(s1,"              : 'Arrow left' - move Left, 'Arrow Right' - move right, 'Arrow Up' - move forward, 'Arrow Down' - move back");
      print_string_in_window3(10.0,h-110.0,s1,GLUT_BITMAP_HELVETICA_12) ;
      sprintf(s1,"              : 'Page Up' - move up, 'Page Down' - move down");
      print_string_in_window3(10.0,h-130.0,s1,GLUT_BITMAP_HELVETICA_12) ;
      sprintf(s1,"              : 'Alt'/'Shift' plus arrow keys give smaller/larger steps");
      print_string_in_window3(10.0,h-150.0,s1,GLUT_BITMAP_HELVETICA_12) ;
      sprintf(s1,"              : 'Cntl' plus arrow keys turns left, right, up and down");
      print_string_in_window3(10.0,h-170.0,s1,GLUT_BITMAP_HELVETICA_12) ;

      sprintf(s1," Switches : 'F8' - toggle display, 'g' - toggle switch in front, 'G' - toggle switch behind");
      print_string_in_window3(10.0,h-190.0,s1,GLUT_BITMAP_HELVETICA_12) ;

      sprintf(s1," Special Keys - with 'alt'");
      print_string_in_window3(10.0,h-220.0,s1,GLUT_BITMAP_HELVETICA_12) ;
      sprintf(s1," World items : 'n' - toggle, 'o'/'p' increase/decrease distance, 'k'/'l' increase/decrease height");
      print_string_in_window3(10.0,h-240.0,s1,GLUT_BITMAP_HELVETICA_12) ;
      sprintf(s1," Track info.   : 't' - toggle");
      print_string_in_window3(10.0,h-260.0,s1,GLUT_BITMAP_HELVETICA_12) ;



      return 0 ;
}

/*
 * =============================================================================
 *
 *   Display information on the track containig a given traveller
 *
 *   This code may be called from the 2-D section of the display routine.
 *   It provides text information on the track section containing the
 *   traveller and on the junction or end sections at either end.
 *
 * =============================================================================
 */

int   display_track_info(TravellerNode *t){

int           n ;
GLfloat       w = viewport_width ;
GLfloat       h = viewport_height ;
TrkNetNode    *tn = t->tn,
              *tn1, *tn2            ;
TrkVectorNode *vn = t->vn ;
char          s1[1024],s2[1024],s3[1024],s4[1024] ;
//
// Plot grey background for text
//
      glColor4f(0.8,0.8,0.8,0.9) ;
      glBegin(GL_QUADS) ;
        glVertex2f(  0.0,h - 115.0) ;
        glVertex2f(1100.0,h - 115.0) ;
        glVertex2f(1100.0,h) ;
        glVertex2f(  0.0,h) ;
      glEnd() ;
/*
 *  Determine track sections (JUNCTION or END_SECTION) at each end
 */
      n = tn->pin_to_section[t->idirect ? 1 : 0] ;
      tn1 = &track_db.trk_sections_array[n-1]   ;  // Section in front
      n = tn->pin_to_section[t->idirect ? 0 : 1] ;
      tn2 = &track_db.trk_sections_array[n-1]   ;  // Section behind
/*
 *  Generate information strings for current track section and the
 *  next JUNCTION or END_SECTION in front of and behind traveller
 */
      glColor3f(0.0,0.0,0.0) ;
      sprintf(s1," Track Data:  Wagon: %s",t->wagon->name);
      sprintf(s2," > TN %4i, Pin %i %i :: TN %i %i %i :: ST %i BR %i ",
             tn1->index_of_node,     // Index of 'forward' end node
             tn1->type_of_pin[0],    // Number of 'in' nodes
             tn1->type_of_pin[1],    // Number of 'out' nodes
             tn1->pin_to_section[0], // Index of first connecting section
             tn1->pin_to_section[1], // Index of second
             tn1->pin_to_section[2], // Index of third
             tn1->straight,          // Index of straight branch
             tn1->branch) ;          // Index of current switched branch
      sprintf(s3," - TN %4i,  Pin %i %i :: TN %i %i %i,   VN %i/%i,  DIR  %i,  Pos  %6.2f,  Len  %6.2f, Rad  %6.2f, Ang  %6.2f,  Dyn  %i, Cur  %i",
             t->itrack,              //  Index of 'current' track section
             tn->type_of_pin[0],     //  Number of 'in' pins (should be 1)
             tn->type_of_pin[1],     //  Number of 'out' pins (should be 1)
             tn->pin_to_section[0],  // Index of first connecting section
             tn->pin_to_section[1],  // Index of second
             tn->pin_to_section[2],  // Index of third  (should be 0)
             t->ivector,             // Index of track sub-section ('vector')
             tn->length_of_vector,   // Number of sub-sections in track section
             t->idirect,             // True if forward direction of traveller
                                     //  is the same as current track section
             t->position,            // Position of traveller within 'vector'
             vn->length,             // Length of sub-section
             vn->radius,             // Radius of curved sub-sections
             vn->angle,              // Angle (radian?) subtended by
                                     //                     curved sub-sections
             vn->is_dynamic,         // True if 'dynamic' track
             vn->is_curved) ;        // True if sub-section is curved
      sprintf(s4," < TN %4i, Pin %i %i :: TN %i %i %i :: ST %i BR %i ",
             tn2->index_of_node,     // Index of 'behind' end node
             tn2->type_of_pin[0],    // Number of 'in' nodes
             tn2->type_of_pin[1],    // Number of 'out' nodes
             tn2->pin_to_section[0], // Index of first connecting section
             tn2->pin_to_section[1], // Index of second
             tn2->pin_to_section[2], // Index of third
             tn2->straight,          // Index of straight branch
             tn2->branch) ;          // Index of current switched branch
      print_string_in_window3(10.0,h-25.0,s1,GLUT_BITMAP_HELVETICA_12) ;
      print_string_in_window3(10.0,h-50.0,s2,GLUT_BITMAP_HELVETICA_12) ;
      print_string_in_window3(10.0,h-75.0,s3,GLUT_BITMAP_HELVETICA_12) ;
      print_string_in_window3(10.0,h-100.0,s4,GLUT_BITMAP_HELVETICA_12) ;

      return 0 ;
      }

/*
 * =============================================================================
 *
 *   Plot diagram of points
 *
 *   This routine is called from routine display_switches within the
 *   2D plotting section of the main graphics routine
 *
 *   It generates a small square image indicating the layout of a single
 *   simple junction or end section of track.  The parameters are:
 *   x1, y1  :: screen location of bottom left hand corner of image
 *   ii[7]   :: array specifying image to plot
 *      If ii[6] > 0  : End section in front (line with red bar at top)
 *               < 0  : End section behind (line with red bar at bottom)
 *      else
 *         if ii[0:6] is non zero plot line starting from cente of image
 *                    = 1 line is grey
 *                    = 2 line is blue
 *         ii[0] corresponds to line to top left (NW), ii[1] to ii[5] then
 *         correspond to N, NE, SE,S and SW
 * =============================================================================
 */

int plot_switch(float x1, float y1, int ii[7]){

GLfloat r0 = 20.0,
        x0 = x1 + 25.0,
        y0 = y1 + 25.0,
        r1 = r0*0.75,
        r2 = r0*0.50   ;
/*
 *  If ii[6] != 0, plot end section
 */
      glColor4f(0.0,0.0,1.0,1.0) ;
      glLineWidth((GLfloat)3.0)  ;
      if(ii[6]!= 0){
        if(ii[6]>0){
          glBegin(GL_LINES) ;
            glVertex2f(x0  , y0-r1) ;
            glVertex2f(x0  , y0+r1   ) ;
          glEnd() ;
          glColor4f(0.7,0.0,0.0,1.0) ;
          glBegin(GL_LINES) ;
            glVertex2f(x0-r1, y0+r1) ;
            glVertex2f(x0+r1, y0+r1) ;
          glEnd() ;
          glColor4f(1.0,1.0,1.0,1.0) ;
        }else if(ii[6]<0){
          glBegin(GL_LINES) ;
            glVertex2f(x0  , y0+r1) ;
            glVertex2f(x0  , y0-r1   ) ;
          glEnd() ;
          glColor4f(0.7,0.0,0.0,1.0) ;
          glBegin(GL_LINES) ;
            glVertex2f(x0-r1, y0-r1) ;
            glVertex2f(x0+r1, y0-r1) ;
          glEnd() ;
          glColor4f(1.0,1.0,1.0,1.0) ;
        }
      }else{
/*
 *  Plot line NW
 */
        if(ii[0]!=0){
          if(ii[0]==2){
            glColor4f(0.0,0.0,1.0,1.0) ;
          }else{
            glColor4f(0.5,0.5,0.5,1.0) ;
          }
          glBegin(GL_LINES) ;
            glVertex2f(x0   , y0   ) ;
            glVertex2f(x0-r1, y0+r1) ;
          glEnd() ;
        }
/*
 *  Plot line N
 */
        if(ii[1]!=0){
          if(ii[1]==2){
            glColor4f(0.0,0.0,1.0,1.0) ;
          }else{
            glColor4f(0.5,0.5,0.5,1.0) ;
          }
          glBegin(GL_LINES) ;
            glVertex2f(x0   , y0   ) ;
            glVertex2f(x0   , y0+r1) ;
          glEnd() ;
        }
/*
 *  Plot line NE
 */
        if(ii[2]!=0){
          if(ii[2]==2){
            glColor4f(0.0,0.0,1.0,1.0) ;
          }else{
            glColor4f(0.5,0.5,0.5,1.0) ;
          }
          glBegin(GL_LINES) ;
            glVertex2f(x0   , y0   ) ;
            glVertex2f(x0+r1, y0+r1) ;
          glEnd() ;
        }
/*
 *  Plot line SE
 */
        if(ii[3]!=0){
          if(ii[3]==2){
            glColor4f(0.0,0.0,1.0,1.0) ;
          }else{
            glColor4f(0.5,0.5,0.5,1.0) ;
          }
          glBegin(GL_LINES) ;
            glVertex2f(x0   , y0   ) ;
            glVertex2f(x0+r1, y0-r1) ;
          glEnd() ;
        }
/*
 *  Plot line S
 */
        if(ii[4]!=0){
          if(ii[4]==2){
            glColor4f(0.0,0.0,1.0,1.0) ;
          }else{
            glColor4f(0.5,0.5,0.5,1.0) ;
          }
          glBegin(GL_LINES) ;
            glVertex2f(x0   , y0   ) ;
            glVertex2f(x0   , y0-r1) ;
          glEnd() ;
        }
/*
 *  Plot line SW
 */
        if(ii[5]!=0){
          if(ii[5]==2){
            glColor4f(0.0,0.0,1.0,1.0) ;
          }else{
            glColor4f(0.5,0.5,0.5,1.0) ;
          }
          glBegin(GL_LINES) ;
            glVertex2f(x0   , y0   ) ;
            glVertex2f(x0-r1, y0-r1) ;
          glEnd() ;
        }
      }
/*
 *  Plot grey square
 *  Note:  If placed at start of routine the code prevents the lines
 *         being plotted.
 */
      glColor4f(0.8,0.8,0.8,0.9) ;
      glBegin(GL_QUADS) ;
        glVertex2f(x1     , y1     ) ;
        glVertex2f(x1+50.0, y1     ) ;
        glVertex2f(x1+50.0, y1+50.0) ;
        glVertex2f(x1     , y1+50.0) ;
      glEnd() ;
      glColor4f(0.5,0.0,0.0,1.0) ;
      glLineWidth((GLfloat)2.0)  ;
      glBegin(GL_LINE_LOOP) ;
        glVertex2f(x1     , y1     ) ;
        glVertex2f(x1+50.0, y1     ) ;
        glVertex2f(x1+50.0, y1+50.0) ;
        glVertex2f(x1     , y1+50.0) ;
      glEnd() ;

      return 0;
}

/*
 * =============================================================================
 *
 *   Plot diagram of switches (UK points)
 *
 *   This routine is called from within the 2D plotting section of the main
 *   graphics routine.  It generates two small 2D images which display
 *   the position of the points in front of and behind the traveller.
 *
 *   Note:  In time it should be modified to generate the images for the
 *          current 'train'.
 * =============================================================================
 */

int   display_switches(TravellerNode *t){

int           i, n, ii[7]  ;
int           ip = 0       ;         // Debug
int           index        ;         //  Index of section containing traveller
TrkNetNode    *tn = t->tn,
              *tn1, *tn2   ;         // Pointers to track sections
TrkVectorNode *vn = t->vn  ;         // Pointer to current track vector
GLfloat       h = viewport_height ;  // Height of main viewport

/*
 *  Find Junctions or Track Ends
 */
      n = tn->pin_to_section[t->idirect ? 1 : 0] ;
      tn1 = &track_db.trk_sections_array[n-1]   ;  // Section in front
      n = tn->pin_to_section[t->idirect ? 0 : 1] ;
      tn2 = &track_db.trk_sections_array[n-1]   ;  // Section behind
/*
 *  Switch ahead of traveller
 */
      for(i=0;i<7;i++)ii[i] = 0 ;
      index = tn->index_of_node ;
      if(ip)printf(" display_switches :: %i : %i %i : %i %i %i : %i %i %i\n",
             index,
             tn->pin_to_section[t->idirect ? 1 : 0],
             tn->pin_to_section[t->idirect ? 0 : 1],
             tn1->pin_to_section[0], tn1->pin_to_section[1], tn1->pin_to_section[2],
             tn2->pin_to_section[0], tn2->pin_to_section[1], tn2->pin_to_section[2]);
      if(tn1->type_of_node == END_SECTION){
        ii[6] = 1 ;
// Approaching base of points
      }else if(index == tn1->pin_to_section[0]){
        ii[4] = 2 ;
        if(tn1->straight == 1){
          if(tn1->branch == 1){
            ii[1] = 2 ;  ii[2] = 1 ;
          }else{
            ii[1] = 1 ;  ii[2] = 2 ;
          }
        }else{
          if(tn1->branch == 1){
            ii[0] = 2 ;  ii[1] = 1 ;
          }else{
            ii[0] = 1 ;  ii[1] = 2 ;
          }
        }
// Approaching left arm of points
      }else if(index == tn1->pin_to_section[1]){
        ii[4] = 2 ;
        if(tn1->straight == 1){
          if(tn1->branch == 1){
            ii[1] = 2 ;  ii[5] = 1 ;
          }else{
            ii[1] = 1 ;  ii[5] = 1 ;
          }
        }else{
          if(tn1->branch == 1){
            ii[2] = 2 ;  ii[5] = 1 ;
          }else{
            ii[2] = 1 ;  ii[5] = 1 ;
          }
        }
// Approaching right arm of points
      }else if(index == tn1->pin_to_section[2]){
        ii[4] = 2 ;
        if(tn1->straight == 1){
          if(tn1->branch == 1){
            ii[0] = 1 ;  ii[3] = 1 ;
          }else{
            ii[0] = 2 ;  ii[3] = 1 ;
          }
        }else{
          if(tn1->branch == 1){
            ii[1] = 1 ;  ii[3] = 1 ;
          }else{
            ii[1] = 2 ;  ii[3] = 1 ;
          }
        }
      }else{
       for(i=0;i<6;i++)ii[i] = 1 ;
      }
/*
 *  Call plotting routine for track in front.
 *  This needs further code to allow for small screen sizes
 */
      plot_switch(10.0,h-170.0,ii);
/*
 *  Switch behind traveller
 */
      for(i=0;i<7;i++)ii[i] = 0 ;
      if(tn2->type_of_node == END_SECTION){
        ii[6] = -1 ;
// Leaving base of points
      }else if(index == tn2->pin_to_section[0]){
        ii[1] = 2 ;
        if(tn2->straight == 1){
          if(tn2->branch == 1){
            ii[4] = 2 ;  ii[5] = 1 ;
          }else{
            ii[4] = 1 ;  ii[5] = 2 ;
          }
        }else{
          if(tn2->branch == 1){
            ii[3] = 2 ;  ii[4] = 1 ;
          }else{
            ii[3] = 1 ;  ii[4] = 2 ;
          }
        }
// Leaving left arm of points
      }else if(index == tn2->pin_to_section[1]){
        ii[1] = 2 ;
        if(tn2->straight == 1){
          if(tn2->branch == 1){
            ii[4] = 2 ;  ii[2] = 1 ;
          }else{
            ii[4] = 1 ;  ii[2] = 2 ;
          }
        }else{
          if(tn2->branch == 1){
            ii[5] = 2 ;  ii[2] = 1 ;
          }else{
            ii[5] = 1 ;  ii[2] = 1 ;
          }
        }
// Leaving right arm of points
      }else if(index == tn2->pin_to_section[2]){
        ii[1] = 2 ;
        if(tn2->straight == 1){
          if(tn2->branch == 1){
            ii[3] = 1 ;  ii[0] = 1 ;
          }else{
            ii[3] = 2 ;  ii[0] = 1 ;
          }
        }else{
          if(tn2->branch == 1){
            ii[4] = 1 ;  ii[0] = 1 ;
          }else{
            ii[4] = 2 ;  ii[0] = 1 ;
          }
        }
      }else{
       for(i=0;i<6;i++)ii[i] = 1 ;
      }
/*
 *  Call plotting routine for track behind.
 *  This needs further code to allow for small screen sizes
 */
      plot_switch(10.0,h-240.0,ii);
      return 0;
}
