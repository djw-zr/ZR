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

static double key_release_seconds = 0.0 ;

/*
 *==============================================================================
 *   Routine Keyboard
 *   See routine 'specialkey' for F1 type keys and the arrow keys used with ZR
 *   to move the viewpoint
 *==============================================================================
 */
void keyboard(unsigned char key, int x, int y){

int  ip = 0                       ;  //  0 = no printing
int  isign = 1                    ;  //  1 or -1
int  imod, l_shift, l_ctrl, l_alt ;
int  i, n, iret     ;
int  new_camera = 0 ;
double        df ;               // lenght of end wagons
TravellerNode *tff = NULL ,
              *tbf = NULL ,
              tf, tb           ;
TrkSector   *tn1 = NULL,
              *tn2 = NULL,
              *tfn = NULL,
              *tbn = NULL      ;
WorldItem     *witem           ; // World item for junctions
GLfloat v4[4] ;
char    *my_name="keyboard" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

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
      if(l_shift) isign = -1 ;

      if(ip){
        printf("    Routine keyboard.  Key : 0x%x  :%c: C=%i S=%i A=%i\n",
               key,key, l_ctrl, l_shift, l_alt);
        fflush(NULL);
        if(l_ctrl){i = x && y ; }            // Keep the compiler happy
      }
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
 *  Display world items (alt_left + ..)
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
              lookat_center_x = lookat_eye_x + 1.3*(offset_center_x - offset_eye_x) ;
              lookat_center_y = lookat_eye_y + 1.3*(offset_center_y - offset_eye_y) ;
            }
            break ;
          case 'b':
            if(display_info){
              offset_center_x = offset_eye_x + (offset_center_x - offset_eye_x)/1.3 ;
              offset_center_y = offset_eye_y + (offset_center_y - offset_eye_y)/1.3 ;
              lookat_center_x = lookat_eye_x + (offset_center_x - offset_eye_x)/1.3 ;
              lookat_center_y = lookat_eye_y + (offset_center_y - offset_eye_y)/1.3 ;
            }
            break ;
/*
 *   Track Info Window(left alt + t)
 */
          case 't':
            if(++display_track_info_on >3) display_track_info_on = 0;
            break ;
#if 0
/*
 *  Debug display of engines and trucks
 */
          case 'z':
            id_shape-- ;
            break ;
          case 'x':
            id_shape = -1 ;
            break ;
          case 'c':
            id_shape++ ;
            break ;
#endif
/*
 *  glutIgnoreKeyRepeat appears to turn it off permanently
 */
          case 'c':
            if(key_release_seconds == 0.0){
              key_release_seconds=run_seconds;
            }else if(run_seconds-key_release_seconds < 0.2){
              break ;
            }
            glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF) ;
//          printf(" glutSetKeyRepeat(GLUT_KEY_REPEAT_ON) \n") ;
            turntable_rotating = rotate_tt_clockwise() ;
            break ;
          case 'C':
            if(key_release_seconds == 0.0){
              key_release_seconds=run_seconds;
            }else if(run_seconds-key_release_seconds < 0.2){
              break ;
            }
            glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF) ;
//          printf(" glutSetKeyRepeat(GLUT_KEY_REPEAT_ON) \n") ;
            turntable_rotating = rotate_tt_anticlockwise() ;
            break ;
/*
 *  Switches under keyboard control
 *  For the moment may be used to debug code
 *  Alt-'0' : read lookat position from file ~/.zr/lookat.txt
 */
          case '0':
            i_zra = 0 ;
            if(ip)printf(" Switch i_zra = %i\n",i_zra) ;
            read_lookat_file() ;
            break ;
          case '1':
            i_zra = 1 ;
            if(ip)printf(" Switch i_zra = %i\n",i_zra) ;
            break ;
          case '2':
            i_zra = 2 ;
            if(ip)printf(" Switch i_zra = %i\n",i_zra) ;
            break ;
          case '3':
            i_zra = 3 ;
            if(ip)printf(" Switch i_zra = %i\n",i_zra) ;
            break ;
          case '4':
            i_zra = 4 ;
            if(ip)printf(" Switch i_zra = %i\n",i_zra) ;
            break ;
          case '9':
            i_zrt = !i_zrt ;
            if(ip || 1)printf(" Toggle i_zrt = %i\n",i_zrt) ;
            break ;
#endif
        }
/*
 *   Modifier keys not used
 */
      }else{
        switch (key) {
//  Break or Increase speed backwards
          case 'a':
            if(player_train->motor){
              if(player_train->speed< 0.2){ player_train->speed -= 0.1 ; }
              else                        { player_train->speed -= 0.2 ; }
              if(fabs(player_train->speed) < 0.1)player_train->speed = 0.0 ;
            }
            break ;
//  Break or Increase speed
          case 'd':
              if(player_train->speed>-0.2){ player_train->speed += 0.1 ; }
              else                        { player_train->speed += 0.2 ; }
              if(fabs(player_train->speed) < 0.1)player_train->speed = 0.0 ;
            break ;
//  Emergency Stop
          case 's':
            player_train->speed = 0.0 ;
            break ;
//  Change Switch/Points in front of traveller
          case 'g':
            change_forward_switch_for_player_train() ;
            clear_track_beyond_next_switch() ;
            break ;
//  Change Switch/Points behind traveller
          case 'G':
            change_rearward_switch_for_player_train() ;
            clear_track_beyond_previous_switch() ;
            break ;
//  Toggle mirrors out or in
          case 'P':
            if(player_train && player_train->motor
                            && player_train->motor->has_mirrors){
              player_train->motor->mirrors_out =
                        !player_train->motor->mirrors_out ;
            }
            break ;
//  Toggle pantographs up or down
          case 'p':
            if(player_train && player_train->motor
                            && player_train->motor->has_pantographs){
              player_train->motor->pantographs_up =
                         !player_train->motor->pantographs_up ;
            }
            break ;
//  Toggle wipers on or off
          case 'V':
            if(player_train && player_train->motor
                            && player_train->motor->has_wipers){
              if(player_train->motor->wipers_on){
                player_train->motor->wipers_off = 1 ;  // Park at end of sweep
              }else{
                player_train->motor->wipers_on  = 1 ;
              }
            }
            break ;
//  Trigger Water Column for Steam Trains
          case 'T':
            if(key_release_seconds == 0.0){
              key_release_seconds=run_seconds;
            }else if(run_seconds-key_release_seconds < 0.2){
              break ;
            }
            glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF) ;
            start_transfer() ;
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
            cameras[4].offset_eye_x = -cameras[4].offset_eye_x ;
            offset_eye_x = cameras[4].offset_eye_x ;
            break ;
          case '5':
            new_camera     = 5 ;
            break ;
          case '6':
            new_camera     = 6 ;
            break ;
          case '7':
            new_camera     = 7 ;
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
            d_rotate = (d_rotate>=269.0)? 0.0 : d_rotate + 90.0 ;
            printf("  d_rotate = %f\n",d_rotate) ;
//#endif
            break ;
  /*
  *   'Esc' ends program
  */
          case 27:
            close_system() ;
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
      if(ip){
        printf("  Exit keyboard\n") ;
        fflush(NULL) ;
      }
}

/*
 * With key repeat on - the normal keyboard software option
 * the program thinks that they are released every 1/10 second approx
 * and pressed down again.
 */

void keyboard_up(unsigned char key, int x, int y){

int  ip = 0                       ;  //  0 = no printing
int  i                            ;
int  imod, l_shift, l_ctrl, l_alt ;
char *my_name="keyboard_up" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      imod = glutGetModifiers();
      l_ctrl  = imod & GLUT_ACTIVE_CTRL  ;
      l_shift = imod & GLUT_ACTIVE_SHIFT ;
      l_alt   = imod & GLUT_ACTIVE_ALT   ;

      if(ip>1){
        printf("    Routine %s.  Key : 0x%x  :%c: C=%i S=%i A=%i\n",
               my_name,key,key, l_ctrl, l_shift, l_alt);
        printf("  Routine %s.  Modifier %x",my_name, imod) ;
        printf("  Routine %s.  Modifier ", my_name)        ;
        for(i=0;i<16;i++)printf("%i",((imod >> (15-i)) & 1)) ;
        printf("\n\n");
      }
/*
 *  Detect 'c' key release when turntable is rotating.
 *  Detecting release of <Alt>+C does not work if <Alt> released first.
 */
      if(turntable_rotating){
        switch (key) {
          case 'c':
          case 'C':
            end_tt_rotation() ;
            turntable_rotating = 0;
            key_release_seconds = run_seconds ;
            break ;
          default :
// Keep the compiler happy
            if(0 && l_ctrl && l_shift && l_alt){i = x && y ; }
            break ;
        }
        glutSetKeyRepeat(GLUT_KEY_REPEAT_ON) ;
/*
 *   Transfer:  water tower, coal stage etc.
 */
      }else if(current_transfer && current_transfer->on){
        switch (key) {
          case 'T':
          case 't':
            stop_transfer() ;
            key_release_seconds = run_seconds ;
           break ;
          default:
            break ;
        }
        glutSetKeyRepeat(GLUT_KEY_REPEAT_ON) ;
      }
      return ;
}

/*
 *==============================================================================
 *  Special Key
 *
 *  Handles keyboard special keys (F1, F2, .. arrow keys, ... etc):
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
 *
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
  double  c, s ;
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
        i = ixm && iym ;  // Keep the compiler happy
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
        offset_eye_z    = offset_eye_z    + del_d*0.25 ;
        offset_center_z = offset_center_z + del_d*0.25 ;
      }else if(key == GLUT_KEY_PAGE_DOWN){
        offset_eye_z    = offset_eye_z    - del_d*0.25 ;
        offset_center_z = offset_center_z - del_d*0.25 ;
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
      }else if(key == GLUT_KEY_F6){
        if(l_shift){
          if(++show_platforms_or_sidings == 3)
            show_platforms_or_sidings = 0 ;
        }else{
          show_platforms_and_sidings = !show_platforms_and_sidings ;
          show_platforms_or_sidings = 0 ;
        }
        return ;
      }else if(key == GLUT_KEY_F7 && l_alt){
        do{
          if(NULL == player_train->next){
            player_train = trainlist_beg ;
          }else{
            player_train = player_train->next ;
          }
        }while(!player_train->motor) ; //  Skip 'trains' without engines

        current_camera = 1    ;
        camera_changed = 1    ;
        camera_new_position() ;
        new_viewpoint = 1     ;
        return ;
      }else if(key == GLUT_KEY_F8){
        display_switches_on = !display_switches_on ;
        return ;
      }else if(key == GLUT_KEY_F9){
        display_train_operations_on = !display_train_operations_on ;
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
  double  e, n, h        ;
       if(display_info || ip){
         local2msts(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                   offset_center_x, offset_center_y, offset_center_z,
                   &tile_e, &tile_n, &e, &n, &h );
          printf(" New position : eye = %f %f %f : centre = %f %f %f :"
                 " tile = %i %i : posn = %f %f %f : north, up = %f %f\n",
                  offset_eye_x, offset_eye_y, offset_eye_z,
                  offset_center_x,offset_center_y,offset_center_z,
                  tile_e, tile_n, e, n, h, angle_to_north, angle_to_up) ;
          printf("              : eye = %f %f %f : centre = %f %f %f\n",
                  (tile_e-tile_west)  + offset_eye_x,
                  (tile_n-tile_south) + offset_eye_y, offset_eye_z,
                  (tile_e-tile_west)  + offset_center_x,
                  (tile_n-tile_south) + offset_center_y, offset_center_z) ;
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
#if 1
      sprintf(string," - Lookat Point :: %f %f %f :: %f %f %f ",
                         lookat_eye_x, lookat_eye_y, lookat_eye_z,
                         lookat_center_x, lookat_center_y, lookat_center_z);
#endif
      glutPostRedisplay() ;
}
