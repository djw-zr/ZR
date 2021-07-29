/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  keyboard.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Keyboard subroutines used by ZR
 *
 *   The graphics code is based on openglut amd OpenGL 2.1
 *
 *==============================================================================
 */


/*
 *==============================================================================
 *   Routine Keyboard
 *   See routine special key for keys moving the viewpoint
 *==============================================================================
 */
void keyboard(unsigned char key, int x, int y){

int  ip = 0                       ;  // 0 = no printing
int  isign = 1                    ;
int  imod, l_shift, l_ctrl, l_alt ;
int  i, n  ;
int  new_camera = 0 ;
double scalei = 1.0/plot_scale ;
CameraNode    *camera           ;
TravellerNode *t = &trav_node_0 ;
TrkNetNode  *tn1, *tn2, *tn = t->tn ;
GLfloat v4[4] ;

      imod = glutGetModifiers();
      if(ip>1){
        printf("  ZR - Modifier %x",imod) ;
        printf("  ZR - Modifier ")       ;
        for(i=0;i<16;i++)printf("%i",((imod >> (15-i)) & 1)) ;
        printf("\n\n");
      }
      l_ctrl  = imod & GLUT_ACTIVE_CTRL  ;
      l_shift = imod & GLUT_ACTIVE_SHIFT ;
      l_alt   = imod & GLUT_ACTIVE_ALT   ;
      if(l_ctrl)             ;  // Keep the compiler happy for now
      if(l_shift) isign = -1 ;

      if(ip)printf(" Routine keyboard.  Key : 0x%x  :%c: \n",key,key);
/*
 *   1.  Keys using 'alt'
 */
      if(l_alt){
        switch (key) {
#ifdef kb_dev
//  Change intensity of ambient, diffusive and specular light0
          case 'a':
          case 'A':
            light0_ambi = zr_fclip(light0_ambi +isign * 0.1, 0.0, 1.0) ;
            zr_setv4(v4,light0_ambi,light0_ambi,light0_ambi,1.0) ;
            printf("  Light_0 ambient intensity = %f\n",(double)light0_ambi) ;
            glLightfv(GL_LIGHT0,GL_AMBIENT,v4);
            break ;
          case 'd':
          case 'D':
            light0_diff = zr_fclip(light0_diff +isign * 0.1, 0.0, 1.0) ;
            zr_setv4(v4,light0_diff,light0_diff,light0_diff,1.0) ;
            printf("  Light_0 diffusive intensity = %f\n",(double)light0_diff) ;
            glLightfv(GL_LIGHT0,GL_SPECULAR,v4);
            break ;
          case 's':
          case 'S':
            light0_spec = zr_fclip(light0_spec +isign * 0.1, 0.0, 1.0) ;
            printf("  Light_0 specular intensity = %f\n",(double)light0_spec) ;
            zr_setv4(v4,light0_spec,light0_spec,light0_spec,1.0) ;
            glLightfv(GL_LIGHT0,GL_SPECULAR,v4);
            break ;
//  Change position of light0
          case 'q':
          case 'Q':
            light0_altde = zr_fclip(light0_altde +isign * 10.0, 0.0, 90.0) ;
            printf("  Light_0 altitude = %f\n",(double)light0_altde)    ;
            zr_setp4(v4,light0_altde,light0_polar) ;
            glLightfv(GL_LIGHT0,GL_POSITION,v4) ;
            break ;
          case 'w':
          case 'W':
            light0_polar = fmod(light0_polar +isign * 15.0, 360.0)   ;
            if(light0_polar<0.0) light0_polar = light0_polar + 360.0 ;
            printf("  Light_0 polar = %f\n",(double)light0_polar)    ;
            zr_setp4(v4,light0_altde,light0_polar) ;
            glLightfv(GL_LIGHT0,GL_POSITION,v4) ;
            break ;
/*
 *  Display world items
 */
          case 'o':
            display_info_radius = display_info_radius/1.3 ;
            break ;
          case 'p':
            display_info_radius = display_info_radius*1.3 ;
            break ;
          case 'k':
            display_info_height = display_info_height/1.3 ;
            break ;
          case 'l':
            display_info_height = display_info_height*1.3 ;
            break ;
          case 'n':
            display_info = !display_info ;
            break ;
          case 'm':
            if(display_info){
              offset_center_x = offset_eye_x + 1.3*(offset_center_x - offset_eye_x) ;
              offset_center_y = offset_eye_y + 1.3*(offset_center_y - offset_eye_y) ;
            }
            break ;
          case 'b':
            if(display_info){
              offset_center_x = offset_eye_x + (offset_center_x - offset_eye_x)/1.3 ;
              offset_center_y = offset_eye_y + (offset_center_y - offset_eye_y)/1.3 ;
            }
            break ;
/*
 *   Track Info
 */
          case 't':
            display_track_info_on = !display_track_info_on ;
            break ;
#endif
        }
      }else{
        switch (key) {
//  Increase speed backwards
          case 'a':
            eng_speed = eng_speed - 2.0 ;
            break ;
//  Increase speed
          case 'd':
            eng_speed = eng_speed + 2.0 ;
            break ;
//  Stop
          case 's':
            eng_speed = 0.0 ;
            break ;
//  Change Switch/Points in front of traveller
          case 'g':
            n = tn->pin_to_section[t->idirect ? 1 : 0] ;
            tn1 = &track_db.trk_sections_array[n-1]   ;  // Section in front
            if(tn1->branch != 0)tn1->branch = (tn1->branch==1) ? 2 : 1 ;
            break ;
//  Change Switch/Points behind traveller
          case 'G':
            n = tn->pin_to_section[t->idirect ? 0 : 1] ;
            tn2 = &track_db.trk_sections_array[n-1]   ;  // Section in front
            if(tn2->branch != 0)tn2->branch = (tn2->branch==1) ? 2 : 1 ;
            break ;
//  Switch frame rate on/off
          case 'Z':
            l_fps = !l_fps ;
            break ;
//  Switch to camera "n"
          case '1':
            new_camera     = 1 ;
            break ;
          case '2':
            new_camera     = 2 ;
            break ;
          case '3':
            new_camera     = 3 ;
            break ;
          case '4':
            new_camera     = 4 ;
            break ;
          case '5':
            new_camera     = 5 ;
            break ;
          case '6':
            new_camera     = 6 ;
            break ;
          case '7':
            new_camera     = 7 ;
            cameras[7].offset_eye_x = -cameras[7].offset_eye_x ;
            offset_eye_x = cameras[7].offset_eye_x ;
            break ;
          case '8':
            new_camera     = 8 ;
            break ;
          case '9':
            new_camera     = 9 ;
            break ;
//  Switch d_reflect
          case '-':
//#ifdef D_Display_Shapes
            d_reflect = -d_reflect ;
            printf("  d_reflect = %f\n",d_reflect) ;
//#endif
            break ;
//  Switch  d_rotate
          case '=':
//#ifdef D_Display_Shapes
            d_rotate = (d_rotate>0.0)? 0.0 : 90.0 ;
            printf("  d_rotate = %f\n",d_rotate) ;
//#endif
            break ;
  /*
  *   'Esc' ends program
  */
          case 27:
            exit(0);
          default:
            break;
        }
        if(new_camera){
          if(ip)printf(" Routine keyboard : New Camera\n") ;
          camera_last    = current_camera ;
          if(new_camera>8)new_camera = 0  ;
          current_camera = new_camera     ;
          camera_changed = 1              ;
          camera_new_position()           ;
        }
      }
}


/*
 *==============================================================================
 *  Special Key
 *
 *  Handles all keys in which modifiers are in use:
 *
 *   It determines which modifiers are being used by calling
 *       int glutgetModifiers(void);
 *   This returns the modifier key state at the time of an input event for a
 *   keyboard, special or mouse callback.  The bitmask components of the
 *   returned integer values are:
 *     GLUT_ACTIVE_SHIFT :  Set if the Shift modifier or Caps Lock is active.
 *     GLUT_ACTIVE_CTRL  :  Set if the Ctrl modifier is active.
 *     GLUT_ACTIVE_ALT   :  Set if the Alt modifier is active.
 *   Glut does not distinguish between left and right Shift, Crtl and Alt keys,
 *   or when the numberpad is used for a special key.
 *
 *   There are also routines:
 *       glutkeyboardfunc(3),  glutkeyboardupfunc(3), glutspecialfunc(3)
 *       glutspecialupfunc(3), glutmotionfunc(3),     glutpassivemotionfunc(3)
 *       glutmousefunc(3),     glutmousewheelfunc(3).
 *   See man pages.
 *
 * Available GLUT_KEY_* special keys are:
 * GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5,
 * GLUT_KEY_F6, GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10,
 * GLUT_KEY_F11,     GLUT_KEY_F12,
 * GLUT_KEY_LEFT,    GLUT_KEY_UP,    GLUT_KEY_RIGHT,  GLUT_KEY_DOWN,
 * GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN,
 * GLUT_KEY_HOME,    GLUT_KEY_END,   GLUT_KEY_INSERT
 *
 *==============================================================================
 */

void  specialkey(int key, int ixm, int iym)
{
  int     i      ;
  int     ip = 0 ;                      // 0 = no printing
  int     imod, l_shift, l_ctrl, l_alt ;
  double  zz1, zz2, c, s ;
  double  del_d,                        // delta distance (plot_scale units)
          del_a ;                       // delta angle (degrees)
  GLfloat v4[4] ;
  double  scalei = 1.0/plot_scale ;
#ifndef kb_dev
  double  cu, su ;
#endif


      if(ip>1){
        printf("\n") ;
        printf("  ZR - special key %x",key) ;
        printf("  ZR - special key ")       ;
        for(i=0;i<16;i++)printf("%i",((key >> (15-i)) & 1)) ;
        printf("\n");
      }

      imod = glutGetModifiers();
      if(ip>1){
        printf("  ZR - Modifier %x",imod) ;
        printf("  ZR - Modifier ")       ;
        for(i=0;i<16;i++)printf("%i",((imod >> (15-i)) & 1)) ;
        printf("\n");
        printf(" offset_eye    = %10.6f  %10.6f  %10.6f\n",
               offset_eye_x, offset_eye_y, offset_eye_z) ;
        printf(" offset_centre = %10.6f  %10.6f  %10.6f\n",
               offset_center_x, offset_center_y, offset_center_z) ;
        printf(" angle to north %10.6f,  to up %10.5f\n",
               angle_to_north, angle_to_up);
      }

//      if(current_camera>0 && current_camera<8)scalei = 1.0 ;

      l_ctrl  = imod & GLUT_ACTIVE_CTRL  ;
      l_shift = imod & GLUT_ACTIVE_SHIFT ;
#ifdef kb_dev
      l_alt   = imod & GLUT_ACTIVE_ALT   ;
// Values for while developing code
      del_d = 32.0*scalei ;     //  Step 32 m
      del_a = 1.0 ;             //  Step 1 degrees
      if(l_shift){ del_d *= 32.0 ; del_a  = 30.0 ; }
      if(l_alt)    del_d /= 32.0 ;
#else
//  MSS values ?
      del_d = 1.0*scalei ;      //  Step 1 m
      del_a = 0.1 ;             //  Step 0.1 degree
      if(l_shift){ del_d *= 10.0 ; del_a  = 10.0 ; }
#endif
/*
 *==============================================================================
 *    Code for Projective (natural) projection
 *==============================================================================
 */
      if(!l_ctrl){
        c = cos(radian_to_north) ;
        s = sin(radian_to_north) ;
#ifndef kb_dev
       cu = cos(radian_to_up) ;
       su = sin(radian_to_up) ;
#endif
      }
/*
 *   Text keys
 *   Used to display extra information and move lookat point
 */
      if(key == GLUT_KEY_LEFT){
        if(l_ctrl){
          angle_to_north = angle_to_north - del_a ;
        }else{
          offset_eye_x    = offset_eye_x    - del_d*c ;
          offset_center_x = offset_center_x - del_d*c ;
          offset_eye_y    = offset_eye_y    + del_d*s ;
          offset_center_y = offset_center_y + del_d*s ;
        }
      }else if(key == GLUT_KEY_RIGHT){
        if(l_ctrl){
          angle_to_north = angle_to_north + del_a ;
        }else{
          offset_eye_x    = offset_eye_x    + del_d*c ;
          offset_center_x = offset_center_x + del_d*c ;
          offset_eye_y    = offset_eye_y    - del_d*s ;
          offset_center_y = offset_center_y - del_d*s ;
        }
      }else if(key == GLUT_KEY_UP){
        if(l_ctrl){
          angle_to_up = angle_to_up - del_a ;
        }else{
#ifdef kb_dev
          offset_eye_x    = offset_eye_x    + del_d*s ;
          offset_center_x = offset_center_x + del_d*s ;
          offset_eye_y    = offset_eye_y    + del_d*c ;
          offset_center_y = offset_center_y + del_d*c ;
#else
          offset_eye_x    = offset_eye_x    + del_d*s*su ;
          offset_center_x = offset_center_x + del_d*s*su ;
          offset_eye_y    = offset_eye_y    + del_d*c*su ;
          offset_center_y = offset_center_y + del_d*c*su ;
          offset_eye_z    = offset_eye_z    + del_d*cu ;
          offset_center_z = offset_center_z + del_d*cu ;
#endif
        }
      }else if(key == GLUT_KEY_DOWN){
        if(l_ctrl){
          angle_to_up = angle_to_up + del_a ;
        }else{
#ifdef kb_dev
          offset_eye_x    = offset_eye_x    - del_d*s ;
          offset_center_x = offset_center_x - del_d*s ;
          offset_eye_y    = offset_eye_y    - del_d*c ;
          offset_center_y = offset_center_y - del_d*c ;
#else
          offset_eye_x    = offset_eye_x    - del_d*s*su ;
          offset_center_x = offset_center_x - del_d*s*su ;
          offset_eye_y    = offset_eye_y    - del_d*c*su ;
          offset_center_y = offset_center_y - del_d*c*su ;
          offset_eye_z    = offset_eye_z    - del_d*cu ;
          offset_center_z = offset_center_z - del_d*cu ;
#endif
        }
      }else if(key == GLUT_KEY_PAGE_UP){
        offset_eye_z    = offset_eye_z    + del_d ;
        offset_center_z = offset_center_z + del_d ;
      }else if(key == GLUT_KEY_PAGE_DOWN){
        offset_eye_z    = offset_eye_z    - del_d ;
        offset_center_z = offset_center_z - del_d ;
#ifndef kb_dev
// Stop the viewpoint dropping too far
//   This should be just above the local land surface level
        if(offset_eye_z < 0.0){
          offset_eye_z    =   0.1*scalei ;  // 0.1 m above baseline
          offset_center_z =   0.1*scalei ;
        }
#endif
      }else if(key == GLUT_KEY_F1){
        display_help_on = !display_help_on ;
        return ;
      }else if(key == GLUT_KEY_F8){
        display_switches_on = !display_switches_on ;
        return ;
      }else{
        if(ip)printf(" Keyboard : No action required\n\n");
        return ;
      }
/*
 *  Update direction variables
 */
      if(ip){
        printf(" New values\n") ;
        printf(" offset_eye    = %10.6f  %10.6f  %10.6f\n",
               offset_eye_x, offset_eye_y, offset_eye_z) ;
        printf(" offset_centre = %10.6f  %10.6f  %10.6f\n",
               offset_center_x, offset_center_y, offset_center_z) ;
      }
#ifdef _Display_Textures
      angle_to_north = 0 ;
      angle_to_up    = 0 ;
      radian_to_north = 0 ;
      radian_to_up    = 0 ;
#else
      if(l_ctrl){
  float c_dist ;  // i.e. 200 m away
        c_dist = sqrt((offset_center_x - offset_eye_x)
                                  *(offset_center_x - offset_eye_x)
                  + (offset_center_y - offset_eye_y)
                                  *(offset_center_y - offset_eye_y)
                  + (offset_center_z - offset_eye_z)
                                  *(offset_center_z - offset_eye_z)) ;
        angle_to_up    = nint((double)angle_to_up) ;
        angle_to_north = nint((double)angle_to_north) ;
        if(angle_to_up < 2.0)   angle_to_up =   2.0 ;
        if(angle_to_up > 178.0) angle_to_up = 178.0 ;
        if(angle_to_north <   0.0) angle_to_north = angle_to_north + 360.0 ;
        if(angle_to_north > 360.0) angle_to_north = angle_to_north - 360.0 ;
        radian_to_north = radian*angle_to_north ;
        radian_to_up    = radian*angle_to_up ;

        offset_center_z = offset_eye_z + c_dist*cos(radian_to_up) ;
        offset_center_x = offset_eye_x +
                          c_dist*sin(radian_to_up)*sin(radian_to_north) ;
        offset_center_y = offset_eye_y +
                          c_dist*sin(radian_to_up)*cos(radian_to_north) ;
      }
#endif
/*
 *  Code to check that the transformations are sensible and to provide
 *  information on the positions of the eye and look_at locations.
 *  The eye and centre positions are relative to the local space with
 *  origin at the SW corner of tile tile_x0, tile_y0 and height tile_y0.
 *  This routine converts the 'centre' position to tile coordinates
 *  i.e the x, y and z coordinates relative to the central position
 *  of the local tile.
 */
#ifdef kb_dev
  int     tile_e, tile_n ;
  float   e, n, h        ;
       if(display_info || ip){
         local2msts(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                   offset_center_x, offset_center_y, offset_center_z,
                   &tile_e, &tile_n, &e, &n, &h );
          printf(" New position : eye = %f %f %f : centre = %f %f %f :"
                 " tile = %i %i : posn = %f %f %f : north, up = %f %f\n",
                  offset_eye_x, offset_eye_y, offset_eye_z,
                  offset_center_x,offset_center_y,offset_center_z,
                  tile_e, tile_n, e, n, h, angle_to_north, angle_to_up) ;
          l_pp = 1 ;     //  Flag new position for debug statements
       }
#endif
/*
 *  Generate new OpenGL Projection and Modelview matrices
 */
      viewport_aspect = (GLfloat)viewport_width/(GLfloat)viewport_height ;
#if defined _Display_Shapes || defined _Display_Wagons
      viewport_near = 1.0/plot_scale ;
      viewport_far  = 4096.0/plot_scale ;
#elif _Display_Textures
      viewport_near =  0.1 ;
      viewport_far  = 10.0 ;
#else
      viewport_near   = viewport_near_m/plot_scale ;
      viewport_far    = viewport_far_m/plot_scale  ;
#endif

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(viewport_fovy,viewport_aspect,viewport_near,viewport_far);
#if 0
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(offset_eye_x,offset_eye_y,offset_eye_z,
               offset_center_x,offset_center_y,offset_center_z,
               offset_up_x,offset_up_y,offset_up_z) ;
//  REQUIRED after MODELVIEW CHANGE
      zr_setp4(v4,light0_altde,light0_polar) ;
      glLightfv(GL_LIGHT0, GL_POSITION, v4);

      initialise_eye_vectors() ;
      initialise_clip_planes(clip_a) ;
//    if(ip) check_clip_planes() ;
#else
      camera_changed = 1 ;
      camera_new_position() ;
#endif
//  Trigger graphics_cull checks (myGlutIdle in graphics.c)
      new_viewpoint = 1 ;
      glutPostRedisplay() ;
}

float zr_fclip(float x, float xmin, float xmax){

  float ans = x ;
      if(xmax<xmin) return ans ;
      if(ans>xmax) ans = xmax ;
      if(ans<xmin) ans = xmin ;
      return ans ;
}

int zr_setv4(GLfloat *v4, float x0, float x1, float x2, float x3){

      v4[0] = x0 ;
      v4[1] = x1 ;
      v4[2] = x2 ;
      v4[3] = x3 ;
      return 0 ;
}

int zr_setp4(float *v4, float altde, float polar) {

      v4[0] = cos(radian*altde)*sin(radian*polar)   ;
      v4[1] = cos(radian*altde)*cos(radian*polar)   ;
      v4[2] = sin(radian*altde) ;
      v4[3] = 0.0 ;
      return 0    ;
}
