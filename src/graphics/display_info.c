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
GLfloat    xx, yy, zz   ;
GLfloat    m2d = 1.0/plot_scale ;  // Convert from metres to display uints
GLfloat    dist, dist0, height0  ;

WorldNode   *wnode ;
WorldItem   *witem ;
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

      glColor3f((GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0) ; //RED
#if 0
      glBegin(GL_LINES) ;
        glVertex3d(xc, yc, (GLfloat) 0.0) ;
        glVertex3d(xc, yc, (GLfloat) 1.0) ;
      glEnd() ;
#else
      tx = tile_x0 + xc ;
      ty = tile_y0 + yc ;
      xx = (xc - ((int)xc) - 0.5)*tile_size ;
      yy = (yc - ((int)yc) - 0.5)*tile_size ;
      zz = tile_h0 + zc*tile_size ;
      glBegin(GL_LINES) ;
        glVertex3d(xc-1.0*m2d, yc, zc) ;
        glVertex3d(xc+1.0*m2d, yc, zc) ;
        glVertex3d(xc, yc-1.0*m2d, zc) ;
        glVertex3d(xc, yc+1.0*m2d, zc) ;
        glVertex3d(xc, yc, zc-1.0*m2d) ;
        glVertex3d(xc, yc, zc+1.0*m2d) ;
      glEnd() ;
#endif
      glColor3f((GLfloat)1.0,(GLfloat)1.0,(GLfloat)1.0) ;
      sprintf(string," - Lookat Point :: %f %f %f :: %i %i : %f %f %f\n",
                            xc,yc,zc,tx,ty,xx,yy,zz);
      print_string_in_window2((GLfloat) xc, (GLfloat) yc, (GLfloat) zc, string);
      glColor3f((GLfloat)1.0,(GLfloat)1.0,(GLfloat)1.0) ;

      for(wnode=worldlist_beg; wnode != NULL; wnode=wnode->next){
        if(0 == wnode->in_use) continue ;
        tx = wnode->tile_x - tile_x0 ;
        ty = wnode->tile_y - tile_y0 ;
//        ip = wnode->tile_x == -6131 && wnode->tile_y == 14888 ;
        for(witem = wnode->world_item ; witem != NULL; witem = witem->next){
          xx = (tx+0.5)*tile_size +  witem->X ;
          yy = (ty+0.5)*tile_size +  witem->Y ;
          zz = witem->Z - tile_h0        ;
          xx = xx*m2d ;
          yy = yy*m2d ;
          zz = zz*m2d ;
          dist = (xx-xc)*(xx-xc) + (yy-yc)*(yy-yc) ;
          if(dist > dist0)continue ;

          glColor3d(0.0,1.0,1.0) ;
          glBegin(GL_LINES) ;
            glVertex3d(xx, yy, zz) ;
            zz = zz + height0      ;
            glVertex3d(xx, yy, zz) ;
          glEnd() ;
          if(witem->uid == 4485){
            glColor3d(1.0,1.0,0.0) ;
          }else{
            glColor3d(1.0,1.0,1.0) ;
          }
          zz = zz + (witem->uid%3)*0.2*m2d ;  //  Reduce chance of overlapping world items
#if 0
          sprintf(string," - WORLD   :: uid = %i, type =  %i"
                         " :: item at :: %f %f %f ",
                         witem->uid,witem->worldtype,xx,yy,zz);
#else
          sprintf(string," - WORLD   :: uid = %i, type =  %i"
                         " :: item at :: %f %f %f :: %i %i :: %s",
                         witem->uid,witem->worldtype,witem->X,witem->Y,witem->Z,
                         wnode->tile_x, wnode->tile_y, witem->filename);
//          if(ip)printf("  World item :: %i %i :: %i :: %s :: %s\n",
//                       wnode->tile_x, wnode->tile_y, witem->uid,
//                       witem->filename, witem->filename2) ;
#endif
          print_string_in_window2((GLfloat) xx, (GLfloat) yy, (GLfloat) zz, string);

          sprintf(string," - TR ITEMS :: %i :: %i %i %i %i :: %i %i %i %i",
                         witem->n_tr_item,
                         witem->tr_item_db[0],witem->tr_item_db[1],
                         witem->tr_item_db[2],witem->tr_item_db[3],
                         witem->tr_item_db_id[0],witem->tr_item_db_id[1],
                         witem->tr_item_db_id[2],witem->tr_item_db_id[3] );
          zz = zz - m2d ;
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

/*
 * Plot grey background for text
 */
      glColor4f(0.8,0.8,0.8,0.9) ;
      glBegin(GL_QUADS) ;
        glVertex2f(  0.0,h - 300.0) ;
        glVertex2f(900.0,h - 300.0) ;
        glVertex2f(900.0,h) ;
        glVertex2f(  0.0,h) ;
      glEnd() ;
/*
 *  Generate Main HELP Panel
 */
      glColor3f(0.0,0.0,0.0) ;

      sprintf(s1," Help Panel - with developer options");
      print_string_in_window3(10.0,h-25.0,s1,12) ;
      sprintf(s1," Keyboard keys:");
      print_string_in_window3(10.0,h-45.0,s1,12) ;
      sprintf(s1," Speed     : 'a' - decrease, 's' - stop,  'd' - increase speed forward");
      print_string_in_window3(10.0,h-70.0,s1,12) ;
      sprintf(s1," Cameras : '1' - Driver, '2' - Front, '3' - Rear, '4' - Trackside, '5' - Passenger, '6' - Coupler, '7' - Yard view, '8' - Default view");
      print_string_in_window3(10.0,h-90.0,s1,12) ;
      sprintf(s1,"              : 'Arrow left' - move Left, 'Arrow Right' - move right, 'Arrow Up' - move forward, 'Arrow Down' - move back");
      print_string_in_window3(10.0,h-110.0,s1,12) ;
      sprintf(s1,"              : 'Page Up' - move up, 'Page Down' - move down");
      print_string_in_window3(10.0,h-130.0,s1,12) ;
      sprintf(s1,"              : 'Alt'/'Shift' plus arrow keys give smaller/larger steps");
      print_string_in_window3(10.0,h-150.0,s1,12) ;
      sprintf(s1,"              : 'Cntl' plus arrow keys turns left, right, up and down");
      print_string_in_window3(10.0,h-170.0,s1,12) ;

      sprintf(s1," Switches : 'F8' - toggle switch display, 'g' - toggle switch in front, 'G' - toggle switch behind, 'F6' - toggle platforms and sidings");
      print_string_in_window3(10.0,h-190.0,s1,12) ;
      sprintf(s1,"               : 'F9' - toggle train operations, 'V' - toggle wipers, 'P' - toggle pantographs, 'p' -  mirrors,  'T' - water column"
      );
      print_string_in_window3(10.0,h-210.0,s1,12) ;

      sprintf(s1," Special Keys - with 'alt'");
      print_string_in_window3(10.0,h-240.0,s1,12) ;
      sprintf(s1," World items : 'n' - toggle, 'm'/'b' increase/decrease distance to marker, 'o'/'p'  radius around marker, 'k'/'l' height of items ");
      print_string_in_window3(10.0,h-260.0,s1,12) ;
      sprintf(s1,"  Trains      : 'F7' - cycle     Track info: 't' - toggle"
                 "     Turntable: 'c'/'C' - rotate"
      );
      print_string_in_window3(10.0,h-280.0,s1,12) ;

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

int           n, ip = 0 ;
GLfloat       w = viewport_width ;
GLfloat       h = viewport_height ;
TrkSector   *tn = t->tn,
              *tn1, *tn2            ;
TrkVectorNode *vn = t->vn ;
char          s1[1024],s2[1024],s3[1024],s4[1024] ;
char         *my_name = "display_track_info" ;

      if(ip)printf("Enter %s\n",my_name) ;

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
      sprintf(s1," Track Data:  Wagon Shape: %s",t->shape->name);
      sprintf(s2," > TN %4i, Pin %i %i :: TN %i %i %i :: ST %i BR %i ",
             tn1->uid,     // Index of 'forward' end node
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
             t->vect_position,       // Position of traveller within 'vector'
             vn->length,             // Length of sub-section
             vn->radius,             // Radius of curved sub-sections
             vn->angle,              // Angle (radian?) subtended by
                                     //                     curved sub-sections
             vn->is_dynamic,         // True if 'dynamic' track
             vn->is_curved) ;        // True if sub-section is curved
      sprintf(s4," < TN %4i, Pin %i %i :: TN %i %i %i :: ST %i BR %i ",
             tn2->uid,     // Index of 'behind' end node
             tn2->type_of_pin[0],    // Number of 'in' nodes
             tn2->type_of_pin[1],    // Number of 'out' nodes
             tn2->pin_to_section[0], // Index of first connecting section
             tn2->pin_to_section[1], // Index of second
             tn2->pin_to_section[2], // Index of third
             tn2->straight,          // Index of straight branch
             tn2->branch) ;          // Index of current switched branch
      print_string_in_window3(10.0,h-25.0,s1,12) ;
      print_string_in_window3(10.0,h-50.0,s2,12) ;
      print_string_in_window3(10.0,h-75.0,s3,12) ;
      print_string_in_window3(10.0,h-100.0,s4,12) ;

      if(ip)printf("Exit  %s\n",my_name) ;

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
 *  Plot line SW
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
 *  Plot line S
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
 *  Plot line SE
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
 *  Plot line NE
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
 *  Plot line N
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
 *  Plot line NW
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
 *   the position of the points in front of and behind the train.
 *
 * =============================================================================
 */

int   display_switches(TrainNode *train){

int           i, n, ii[7]  ;
int           ip = 0       ;         //  Debug
int           iret = 0     ;         //  Return code
uint          index        ;         //  Index of section containing traveller
TravellerNode *tff  = train->first->traveller,
              *tbf  = train->last->traveller,
              tf = *tff,             //  Copy front traveller
              tb = *tbf ;            //  Copy back traveller
TrkSector   *tfn = tf.tn,
              *tbn = tb.tn,
              *tfn1, *tfn2   ;        // Pointers to track sections
TrkVectorNode *vn = tf.vn  ;         // Pointer to current track vector
GLfloat       h = viewport_height ;  // Height of main viewport
double        df, db ;               // lenght of end wagons

/*
 *  Find Junctions or Track Ends in front of and behind
 *  front of front wagon
 */
      for(i=0;i<7;i++)ii[i] = 0 ;                 //  Zero flags

      df = tf.wagon->raw_wagon->length ;
      if(df>0.0) trv_move(&tf, 0.5*df) ;
      tfn = tf.tn ;

      n = tfn->pin_to_section[tf.idirect ? 1 : 0] ;
      tfn1 = &track_db.trk_sections_array[n-1]   ;  // Section in front
      n = tfn->pin_to_section[tf.idirect ? 0 : 1] ;
      tfn2 = &track_db.trk_sections_array[n-1]   ;  // Section behind

      index = tfn->uid ;

      if(ip && l_time_30s){
             printf(" display_switches :: %i : %i %i : %i %i %i : %i %i %i\n",
             index,
             tfn->pin_to_section[tf.idirect ? 1 : 0],
             tfn->pin_to_section[tf.idirect ? 0 : 1],
             tfn1->pin_to_section[0], tfn1->pin_to_section[1],
             tfn1->pin_to_section[2],
             tfn2->pin_to_section[0], tfn2->pin_to_section[1],
             tfn2->pin_to_section[2]);
             printf("    front = %i : %i\n",tfn1->straight,tfn1->branch) ;
             printf("    back  = %i : %i\n",tfn2->straight,tfn2->branch) ;
      }

      if(tfn1->type_of_node == END_SECTION){
        ii[6] = 1 ;
// Approaching base of points
      }else if(index == tfn1->pin_to_section[0]){
        ii[4] = 2 ;
        if(tfn1->straight == 1){        //  Index (1 or 2) of straight branch in junction
          if(tfn1->branch == 1){        //  Index (1 or 2) of current switched branch
            ii[1] = 2 ;  ii[2] = 1 ;
          }else{
            ii[1] = 1 ;  ii[2] = 2 ;
          }
        }else{
          if(tfn1->branch == 1){
            ii[0] = 2 ;  ii[1] = 1 ;
          }else{
            ii[0] = 1 ;  ii[1] = 2 ;
          }
        }
// Approaching left arm of points
      }else if(index == tfn1->pin_to_section[1]){
        ii[4] = 2 ;
        if(tfn1->straight == 1){
          if(tfn1->branch == 1){
            ii[1] = 2 ;  ii[5] = 1 ;
          }else{
            ii[1] = 1 ;  ii[5] = 1 ;
          }
        }else{
          if(tfn1->branch == 1){
            ii[2] = 2 ;  ii[5] = 1 ;
          }else{
            ii[2] = 1 ;  ii[5] = 1 ;
          }
        }
// Approaching right arm of points
      }else if(index == tfn1->pin_to_section[2]){
        ii[4] = 2 ;
        if(tfn1->straight == 1){
          if(tfn1->branch == 1){
            ii[0] = 1 ;  ii[3] = 1 ;
          }else{
            ii[0] = 2 ;  ii[3] = 1 ;
          }
        }else{
          if(tfn1->branch == 1){
            ii[1] = 1 ;  ii[3] = 1 ;
          }else{
            ii[1] = 2 ;  ii[3] = 1 ;
          }
        }
      }else{
       for(i=0;i<6;i++)ii[i] = 1 ;
      }
      if(ip && l_time_30s){
             printf("       indices :: %i %i %i : %i %i %i : %i\n",
               ii[0], ii[1], ii[2], ii[3], ii[4], ii[5], ii[6] ) ;
      }
/*
 *  Call plotting routine for track in front.
 *  This needs further code to allow for small screen sizes
 */
      plot_switch(10.0,h-170.0,ii);

/*
 *  Find Junctions or Track Ends behind back of back wagon
 */
      for(i=0;i<7;i++)ii[i] = 0 ;                 //  Zero flags

      db = tb.wagon->raw_wagon->length ;
      if(db>0.0) trv_move(&tb,-0.5*db) ;
      tbn = tb.tn ;

      n = tbn->pin_to_section[tb.idirect ? 0 : 1] ;
      tfn2 = &track_db.trk_sections_array[n-1]   ;  // Section behind

      index = tbn->uid ;

      if(tfn2->type_of_node == END_SECTION){
        ii[6] = -1 ;
// Leaving base of points
      }else if(index == tfn2->pin_to_section[0]){
        ii[1] = 2 ;
        if(tfn2->straight == 1){
          if(tfn2->branch == 1){
            ii[4] = 2 ;  ii[5] = 1 ;
          }else{
            ii[4] = 1 ;  ii[5] = 2 ;
          }
        }else{
          if(tfn2->branch == 1){
            ii[3] = 2 ;  ii[4] = 1 ;
          }else{
            ii[3] = 1 ;  ii[4] = 2 ;
          }
        }
// Leaving left arm of points
      }else if(index == tfn2->pin_to_section[1]){
        ii[1] = 2 ;
        if(tfn2->straight == 1){
          if(tfn2->branch == 1){
            ii[4] = 2 ;  ii[2] = 1 ;
          }else{
            ii[4] = 1 ;  ii[2] = 2 ;
          }
        }else{
          if(tfn2->branch == 1){
            ii[5] = 2 ;  ii[2] = 1 ;
          }else{
            ii[5] = 1 ;  ii[2] = 1 ;
          }
        }
// Leaving right arm of points
      }else if(index == tfn2->pin_to_section[2]){
        ii[1] = 2 ;
        if(tfn2->straight == 1){
          if(tfn2->branch == 1){
            ii[3] = 1 ;  ii[0] = 1 ;
          }else{
            ii[3] = 2 ;  ii[0] = 1 ;
          }
        }else{
          if(tfn2->branch == 1){
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
      if(ip && l_time_30s){
             printf("       indices :: %i %i %i : %i %i %i : %i\n",
               ii[0], ii[1], ii[2], ii[3], ii[4], ii[5], ii[6] ) ;
      }
      plot_switch(10.0,h-240.0,ii);
      return 0;
}



/*
 * =============================================================================
 *
 *   Routine to check position of train relative to end of track and switches
 *
 *   If the train is approaching and within distance 'dist'
 *    - of the end of track
 *    - of a switch (point) not set for the apth of the train
 *   the train stops
 *
 * =============================================================================
 */

int   check_track(TrainNode *train, double dist){

int           ip = 0       ;  // Debug
int           iret = 0     ;  //  Set to 1 to stopping
TravellerNode tf  ;           //  Copy back traveller
double        df  ;           // lenght of end wagons
char         *my_name="check_track" ;

      if(0.0 == train->speed) return 0 ;

      if(train->speed > 0.0){
        tf = *(train->first->traveller) ;
        df = tf.wagon->raw_wagon->length ;
        if(df>0.0) iret = trv_move(&tf, 0.5*df) ;
        if(iret==0) iret = trv_move(&tf, dist)  ;
      }else{
        tf = *(train->last->traveller) ;
        df = tf.wagon->raw_wagon->length ;
        if(df>0.0) iret = trv_move(&tf, -0.5*df) ;
        if(iret==0) iret = trv_move(&tf, -dist)  ;
      }
      if(iret) train->speed = 0.0 ;
      return iret ;
}

/*
 * =============================================================================
 *
 *   Routine to dislay last graphics error onscreen
 *
 * =============================================================================
 */

void display_error(){

GLenum  n = 0 ;
GLfloat       w = viewport_width ;
GLfloat       h = viewport_height ;
char    string[256] = "GL error = ";
      n = glGetError() ;
      if(n != GL_NO_ERROR){
        gl_errorCode = n ;
        gl_errString = (GLubyte *)gluErrorString(n) ;
      }
//      printf("  OpenGL error = %i, %i :: %s\n",(int)n, (int)gl_errorCode,(char *)gl_errString) ;
      strncat(string,(char *)gl_errString,200) ;
      glColor3f((GLfloat)1.0,(GLfloat)0.0,(GLfloat)0.0) ;
      print_string_in_window3(10.0,h-25.0,string,18) ;

      return ;
}

/*
 * =============================================================================
 *
 *   Routine to display platforms and/or sidings
 *   Key F6 toggles display
 *   Key Shift+F6 cycles through both, platforms/sidings
 *
 * =============================================================================
 */
int display_platforms_and_sidings(){

int         i, type_of_node ;
double      scale = 1.0/plot_scale ;
double      x, y, z, dist   ;
TrkItem *trk_item       ;
char        string[256]     ;

      glDisable(GL_BLEND)      ;
      glShadeModel(GL_FLAT)     ;

      for(i=0;i<(int)track_db.trk_items_array_size;i++){
        trk_item = &track_db.trk_items_array[i] ;
        type_of_node = trk_item->type_of_node ;
        if(PLATFORM != type_of_node && SIDING != type_of_node)continue ;
        if(PLATFORM == type_of_node && 2 == show_platforms_or_sidings)continue ;
        if(SIDING   == type_of_node && 1 == show_platforms_or_sidings)continue ;

        global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
              trk_item->tile_east_x, trk_item->tile_north_z,
              trk_item->east_x, trk_item->north_z, trk_item->height_y,
              &x, &y, &z );
        dist = pow(x-lookat_eye_x,2) + pow(y-lookat_eye_y,2) ;
        if(dist > 0.04)continue ;

        glDisable(GL_LIGHTING)   ;
        glDisable(GL_TEXTURE_2D) ;
        glColor3f(0.8,1.0,0.8) ;
        glBegin(GL_LINES) ;
          glVertex3d(x, y, z) ;
          z = z + 6.0*scale ;
          glVertex3d(x, y, z) ;
        glEnd() ;
        glColor3f(1.0,1.0,1.0) ;

        if(PLATFORM == type_of_node){
          sprintf(string,"%s :: %s :: %i ",trk_item->platform_name,
                  trk_item->platform_data1,trk_item->platform_data2) ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
        }else if(SIDING == type_of_node){
          sprintf(string,"%s :: %s :: %i",trk_item->siding_name,
                  trk_item->siding_data1,trk_item->siding_data2) ;
          print_string_in_window2((GLfloat) x, (GLfloat) y, (GLfloat) z, string);
        }
      glEnable(GL_LIGHTING)   ;
      glEnable(GL_TEXTURE_2D) ;
      glDisable(GL_BLEND) ;
      }
      return 0 ;
}

/*
 * =============================================================================
 *
 *   Routine to display the train operations window
 *   Key F9 toggles display
 *
 * =============================================================================
 */

int display_train_operations(TrainNode *train){

  int    ip = 0 ;
  int      i, n, n_wagons, l_pixels, h_pixels ;
  int      isx, isw ;
  F9_Data  *f9 = &(f9_data) ;
  GLfloat  w = viewport_width ;
  GLfloat  h = viewport_height ;
  WagonNode *wagon ;
  double   x1, x2, y1, y2, xc, y3, y4, y5, y6, xx1, xx2, xx3, yy3 ;
  double   dx ;
  char     string[256]     ;
  char     *my_name = "display_train_operations" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      if(train != f9_data.last_train){
        f9_data.wagon_n0 = 0 ;
        f9_data.n_wagons = 0 ;
        for(wagon = train->first; wagon!=NULL; wagon=wagon->next)
                                                    f9_data.n_wagons++ ;
        f9_data.dx = (f9_data.x3-f9_data.x2)/((double)f9_data.n_wagons) ;
        f9_data.last_train = train ;
      }
      if(train->speed != 0.0){
        if(ip)printf("  Train operations window needs train to be stopped\n") ;
        display_train_operations_on = 0 ;
      }
      x1 = f9->x0 ;
      y1 = f9->y0 ;
      x2 = f9->x1 ;
      y2 = f9->y1 ;
      xc = f9->xc ;
      y3 = f9->y3 ;
      y4 = f9->y2 ;
      y5 = f9->y4 ;
      y6 = f9->y5 ;
      n_wagons = f9_data.n_wagons ;
      if(ip)printf("  Routine %s.  n_wagons = %i\n",my_name, n_wagons) ;
/*
 *   Slider
 */
      if(f9_data.n_wagons <=6){
        f9_data.x4 = f9_data.x2 ;
        f9_data.x5 = f9_data.x4 ;
      }else{
        f9_data.x4 =  f9_data.x2 + f9_data.wagon_n0*f9_data.dx ;
        f9_data.x5 =  f9_data.x4 + 6*f9_data.dx ;
      }
/*
 *  Window title
 */
      glColor4f(0.0,0.0,0.0,0.9) ;
      sprintf(string,"Train Operations") ;
      print_string_in_window3(xc-50, y3+6.0, string, 12) ;
/*
 * Use light grey for lines
 */
      glColor4f(0.4,0.4,0.4,0.9) ;
      glBegin(GL_LINES) ;
        glVertex2f(x1, y3) ;
        glVertex2f(x2, y3) ;
        glVertex2f(x1, y4) ;
        glVertex2f(x2, y4) ;
      glEnd() ;
      glBegin(GL_TRIANGLES) ;
        glVertex2f(x1, 0.5*(y1+y4)) ;
        glVertex2f(x1 + (y4-y1), y1) ;
        glVertex2f(x1 + (y4-y1), y4) ;
        glVertex2f(x2, 0.5*(y1+y4)) ;
        glVertex2f(x2 - (y4-y1), y1) ;
        glVertex2f(x2 - (y4-y1), y4) ;
      glEnd() ;
/*
 *  Plot train
 */
      if(n_wagons<8){
        xx1 = xc - 0.5*(n_wagons*80.0 + (n_wagons-1)*10.0) ;
      }else{
        xx1 = x1 ;
      }
      xx2 = xx1 + 80.0 ;
      n = 0 ;
      i = 0 ;
      h_pixels = 12 ;
      yy3 = y5 + h_pixels/2 ;
      for(wagon = train->first; wagon!=NULL; wagon=wagon->next, i++){
        if(i<f9->wagon_n0)continue ;
        sprintf(string,"%s - %i",train->name,i) ;
        l_pixels = width_string_in_window( string, h_pixels) ;
        xx3 = 0.5*(xx1+xx2-l_pixels) ;

        if(wagon->is_engine)glColor4f(1.0,0.5,0.5,1.0) ;
        else                glColor4f(1.0,1.0,1.0,1.0) ;
        print_string_in_window3(xx3, yy3, string, h_pixels) ;

        glColor4f(0.4,0.4,0.4,0.9) ;
        glBegin(GL_QUADS) ;
          glVertex2f(xx1, y5) ;
          glVertex2f(xx2, y5) ;
          glVertex2f(xx2, y6) ;
          glVertex2f(xx1, y6) ;
        glEnd() ;
        xx1 = xx2 + 10.0 ;
        xx2 = xx1 + 80.0 ;
        n++ ;
        if(n==6)break ;
      }
/*
 *  Plot connections
 */
      if(n_wagons>1){
        if(n_wagons<8){
          xx1 = 0.5*(x1 + x2 - n_wagons*80.0 - (n_wagons-1)*10.0) ;
        }else{
          xx1 = x1 ;
        }
        xx2 = xx1 + 80.0 ;
        i = 1 ;
        glBegin(GL_LINES) ;
          for(wagon = train->first; wagon!=NULL; wagon=wagon->next){
            if(i == n_wagons || i == 6)break ;
            glVertex2f(xx2,      y5) ;
            glVertex2f(xx2+10.0, y6) ;
            glVertex2f(xx2,      y6) ;
            glVertex2f(xx2+10.0, y5) ;
            xx1 = xx2 + 10.0 ;
            xx2 = xx1 + 80.0 ;
            i++ ;
          }
        glEnd() ;
      }
/*
 *  plot slider
 */
      if(n_wagons>6){
#if 0
        isx = f9_data.x2 + (f9_data.wagon_n0*(f9_data.x3 - f9_data.x2))/f9_data.n_wagons ;
        isw = (6*(f9_data.x3 - f9_data.x2))/f9_data.n_wagons ;
        glBegin(GL_QUADS) ;
          glVertex2f(isx+1,   f9_data.y0+3) ;
          glVertex2f(isx+isw, f9_data.y0+3) ;
          glVertex2f(isx+isw, f9_data.y2-2) ;
          glVertex2f(isx+2,   f9_data.y2-2) ;
        glEnd() ;
#else
//        if(l_time_1s)printf("  Slider  %i %i :: %i %i\n",
//                                   f9_data.x4,f9_data.x5,f9_data.y1,f9_data.y2) ;
        glBegin(GL_QUADS) ;
          glVertex2f(f9_data.x4+2, f9_data.y0+3) ;
          glVertex2f(f9_data.x5+4, f9_data.y0+3) ;
          glVertex2f(f9_data.x5+4, f9_data.y2-2) ;
          glVertex2f(f9_data.x4+2, f9_data.y2-2) ;
        glEnd() ;

#endif
      }

/*
 * Plot light grey background for text
 */
      glColor4f(0.8,0.8,0.8,0.9) ;
      glBegin(GL_QUADS) ;
        glVertex2f(x1, y1) ;
        glVertex2f(x2, y1) ;
        glVertex2f(x2, y2) ;
        glVertex2f(x1, y2) ;
      glEnd() ;
      return 0 ;
}
