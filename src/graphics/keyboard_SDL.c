/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  keyboard_SDL.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   SDL keyboard subroutines used by ZR
 *
 *==============================================================================
 */


/*
 *==============================================================================
 *   Routine Keyboard
 *
 *   The include files referencing the keyboard are:
 *
 *   /usr/include/SDL2/SDL_events.h       :  Define SDL_KeyboardEvent
 *   /usr/include/SDL2/SDL_keycode.h      :  List of 'keys'
 *   /usr/include/SDL2/SDL_keyboard.h     :  Define SDL_Keysym structure and
 *                                           keyboard routines
 *
 *   With SDL the basic iformation returned by the event structure is the key
 *   (for which 'a' and 'A' are the same) and the state of the shifl, contl and
 *   alt keys.
 *   However the list of SDLK values indicate that
 *   (a) depressing modifier keys also generates a keypress event
 *   (b) the upper case characters corresponding to non=alphabet chahracters are
 *       represented correctly -but this is not the case.
 *   (c) it looks as if holding doen a key generates repeats - maybe a small delay
 *       and then 10 per second - however there is a way (I thik) to prevent thsi.
 *     No mofifiers: mod & KMOD_NONE
 *     Special keys mod & KMOD_CTRL/SHIFT/ALT/LCRTL/RCTRL/LSHIFT/RSHIFT/LALT/RALT
 *
 *     Event.keyketsym.key:
 *      SDLK_ESCAPE,   'a', 'b'
 *      SDLK_F1,  F2 etc
 *      SDLK_UP, DOWN, LEFT, RIGHT
 *      SDLK_INSERT
 *      SDLK_HOME, END, PAGEUP, PAGEDOWN
 *      SDLK_KP0, KP2, ..., KP9 :: Keypad
 *
 * typedef struct SDL_KeyboardEvent
 *   {
 *      Uint32 type;        : SDL_KEYDOWN or SDL_KEYUP
 *      Uint32 timestamp;   : In milliseconds, populated using SDL_GetTicks()
 *      Uint32 windowID;    : The window with keyboard focus, if any
 *      Uint8 state;        : ::SDL_PRESSED or ::SDL_RELEASED
 *      Uint8 repeat;       : Non-zero if this is a key repeat
 *      Uint8 padding2;
 *      Uint8 padding3;
 *      SDL_Keysym keysym;  : The key that was pressed or released
 *    } SDL_KeyboardEvent;
 *
 *  typedef struct SDL_Keysym
 *  {
 *      SDL_Scancode scancode;  : SDL physical key code - see ::SDL_Scancode for details
 *      SDL_Keycode sym;        : SDL virtual key code - see ::SDL_Keycode for details
 *      Uint16 mod;             : current key modifiers
 *      Uint32 unused;
 *  } SDL_Keysym;
 *
 *  typedef union SDL_Event
 *  {
 *      Uint32 type;               : Event type, shared with all events
 *      SDL_CommonEvent common;    : Common event data
 *      SDL_WindowEvent window;    : Window event data
 *      SDL_KeyboardEvent key;     : Keyboard event data
 *      SDL_TextEditingEvent edit; : Text editing event data
 *      ....
 *      Uint8 padding[56];
 *  } SDL_Event;
 *
 *==============================================================================
 */

int  SDL2_KBD_change_view(SDL_Event *event) ;


int keyboard_sdl(SDL_Event *event){

int  ip = 0                       ;  // 0 = no printing
int  isign = 1                    ;
int  imod, l_shift, l_ctrl, l_alt ;
int  i, n  ;
int  new_camera = 0 ;
double scalei = 1.0/plot_scale     ;
double del_d, del_a, cn, sn        ;
TravellerNode *tr_node  ;
TrkNetNode    *tn1, *tn ;
GLfloat       v4[4]     ;

SDL_Keycode sym ;  //  Part of SDL_Keysym
Uint16      mod ;  //  Ditto
char        my_name[] = "keyboard_sdl" ;
/*
 *==============================================================================
 *  Keyboard should only be entered on SDL_KEYDOWN
 *==============================================================================
 */
      if(ip)printf(" Enter routine %s\n",my_name) ;
      if(SDL_KEYDOWN != event->type)  return 0 ;
      sym = event->key.keysym.sym ;
      mod = event->key.keysym.mod ;
      l_shift = mod & KMOD_SHIFT ;
      l_ctrl  = mod & KMOD_CTRL  ;
      l_alt   = mod & KMOD_ALT   ;

      if(sym == SDLK_ESCAPE) quit_program(0) ;    // Esc
      if(sym == 'c' && l_ctrl) quit_program(1) ;  // Ctrl-C
      if(sym == SDLK_F4 && l_alt) quit_program(2) ;  // Alt-F4
/*
 *==============================================================================
 *  1.  Process keys which change the viewpoint
 *==============================================================================
 */
      if(SDLK_LEFT  == sym || SDLK_UP   == sym || SDLK_PAGEUP   == sym ||
         SDLK_RIGHT == sym || SDLK_DOWN == sym || SDLK_PAGEDOWN == sym){
         SDL2_KBD_change_view(event) ;
         return 0 ;
      }
/*
 *==============================================================================
 *  2.  Keys with alt
 *==============================================================================
 */
      if(l_alt){
        switch (sym) {
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
          case SDLK_F7:
            if(ip)printf("  Skip to next train\n") ;
            if(NULL == player_train->next){
              player_train = trainlist_beg ;
            }else{
              player_train = player_train->next ;
            }
            current_camera = 1    ;
            camera_changed = 1    ;
            camera_new_position() ;
            new_viewpoint = 1     ;
            break ;
          default:
            if(ip)printf(" Keyboard : No action required\n\n");
            return 0 ;
        }
/*
 *==============================================================================
 *  3.  Switch on normal keys
 *==============================================================================
 */
      }else{
        switch (sym){
          case SDLK_F1:
            display_help_on     = !display_help_on ;
            if(ip)printf("  Case F1\n") ;
            break ;
          case SDLK_F8:
            if(ip)printf("  Case F8\n") ;
            display_switches_on = !display_switches_on ;
            break ;
//  Increase speed backwards
          case SDLK_a:
            if(ip)printf("  Case a\n") ;
            player_train->speed -= 2.0 ;
            break ;
//  Increase speed
          case SDLK_d:
            if(ip)printf("  Case d\n") ;
            player_train->speed += 2.0 ;
            break ;
//  Stop
          case SDLK_s:
            if(ip)printf("  Case s\n") ;
            player_train->speed = 0.0 ;
            break ;
          case 'f':
            FullScreen = !FullScreen;
            if (FullScreen){
              SDL_SetWindowFullscreen(Window, WindowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
            }else{
              SDL_SetWindowFullscreen(Window, WindowFlags);
            }
            break;
//  Change Switch/Points in front of traveller
          case SDLK_g:
            if(l_shift){
              if(ip)printf("  Key G  Switch front\n") ;
              tr_node = player_train->last->traveller ;
              tn = tr_node->tn ;
              n = tn->pin_to_section[tr_node->idirect ? 0 : 1] ;
              tn1 = &track_db.trk_sections_array[n-1]   ;  // Section behind
              if(tn1->branch != 0)tn1->branch = (tn1->branch==1) ? 2 : 1 ;
            }else{
              if(ip)printf("  Key g  Switch front\n") ;
              tr_node = player_train->first->traveller ;
              tn = tr_node->tn ;
              n = tn->pin_to_section[tr_node->idirect ? 1 : 0] ;
              tn1 = &track_db.trk_sections_array[n-1]   ;  // Section in front
              if(tn1->branch != 0)tn1->branch = (tn1->branch==1) ? 2 : 1 ;
            }
            break ;
//  Switch frame rate on/off
          case SDLK_z:
            printf(" l_fps = %i\n",l_fps) ;
            if(l_shift)l_fps = !l_fps ;
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
            if(ip)printf("  d_reflect = %f\n",d_reflect) ;
//#endif
            break ;
//  Switch  d_rotate
          case '=':
//#ifdef D_Display_Shapes
            d_rotate = (d_rotate>0.0)? 0.0 : 90.0 ;
            if(ip)printf("  d_rotate = %f\n",d_rotate) ;
//#endif
            break ;

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
          new_viewpoint  = 1              ;
        }
      }
      return 0 ;
}





/*
 *  Print all information about a key event
 */
void PrintKeyInfo( SDL_KeyboardEvent *key ){
        /* Is it a release or a press? */
        if( key->type == SDL_KEYUP )
            printf( "Release:- " );
        else
            printf( "Press:- " );

/* Print the hardware scancode first */
        printf( "Scancode: 0x%02X", key->keysym.scancode );
/* Print the name of the key */
        printf( ", Name: %s", SDL_GetKeyName( key->keysym.sym ) );
#if 0  //  Unicode not supported
        /* We want to print the unicode info, but we need to make */
        /* sure its a press event first (remember, release events */
        /* don't have unicode info                                */
        if( key->type == SDL_KEYDOWN ){
            /* If the Unicode value is less than 0x80 then the    */
            /* unicode value can be used to get a printable       */
            /* representation of the key, using (char)unicode.    */
            printf(", Unicode: " );
            if( key->keysym.unicode < 0x80 && key->keysym.unicode > 0 ){
                printf( "%c (0x%04X)", (char)key->keysym.unicode,
                        key->keysym.unicode );
            }
            else{
                printf( "? (0x%04X)", key->keysym.unicode );
            }
        }
#endif
        printf( "\n" );
        /* Print modifier info */
        PrintModifiers( key->keysym.mod );
    }

    /* Print modifier info */
//    void PrintModifiers( SDLMod mod ){
    void PrintModifiers( Uint16 mod ){
        printf( "Modifers: " );

        /* If there are none then say so and return */
        if( mod == KMOD_NONE ){
            printf( "None\n" );
            return;
        }

        /* Check for the presence of each SDLMod value */
        /* This looks messy, but there really isn't    */
        /* a clearer way.                              */
        if( mod & KMOD_NUM ) printf( "NUMLOCK " );
        if( mod & KMOD_CAPS ) printf( "CAPSLOCK " );
        if( mod & KMOD_LCTRL ) printf( "LCTRL " );
        if( mod & KMOD_RCTRL ) printf( "RCTRL " );
        if( mod & KMOD_RSHIFT ) printf( "RSHIFT " );
        if( mod & KMOD_LSHIFT ) printf( "LSHIFT " );
        if( mod & KMOD_RALT ) printf( "RALT " );
        if( mod & KMOD_LALT ) printf( "LALT " );
        if( mod & KMOD_CTRL ) printf( "CTRL " );
        if( mod & KMOD_SHIFT ) printf( "SHIFT " );
        if( mod & KMOD_ALT ) printf( "ALT " );
        printf( "\n" );
    }

/*
 *  Routine to handle keyboard events which change the viewpoint
 */


int  SDL2_KBD_change_view(SDL_Event *event){

int      ip = 0                       ;  // 0 = no printing
int      imod, l_shift, l_ctrl, l_alt ;
double   scalei = 1.0/plot_scale      ;
double   del_d, del_a, cn, sn         ;
GLfloat  v4[4]                        ;
SDL_Keycode sym                       ;  //  SDL Keyboard Symbol
Uint16      mod                       ;  //  SDL Keyboard Modifier
char   *my_name="SDL2_kbd_change_view" ;

      if(ip){
        printf("  Enter routine %s\n", my_name) ;
        PrintKeyInfo(&(event->key))    ;
      }

      sym = event->key.keysym.sym ;
      mod = event->key.keysym.mod ;
      l_shift = mod & KMOD_SHIFT ;
      l_ctrl  = mod & KMOD_CTRL  ;
      l_alt   = mod & KMOD_ALT   ;
#ifdef kb_dev
//#if 1
// Increment values for while developing code
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
        if(ip)printf("  del_d = %f, del_a = %f\n",del_d,del_a) ;
/*
 *==============================================================================
 *    Code to change camera direction
 *==============================================================================
 */
        if(l_ctrl){
          if(ip)printf(" %s, angle_to_north = %10.6f, angle_to_up = %10.6f\n",
                        my_name,angle_to_north, angle_to_up);
          switch(sym){
            case SDLK_LEFT:
              angle_to_north -= del_a ;
              break ;
            case SDLK_RIGHT:
              angle_to_north += del_a ;
              break ;
            case SDLK_UP:
              angle_to_up    -= del_a ;
              break ;
            case SDLK_DOWN:
              angle_to_up    += del_a ;
              break ;
            default:
              break ;
          }
float c_dist ;  // i.e. 200 m away
          c_dist = sqrt((offset_center_x - offset_eye_x)
                                  *(offset_center_x - offset_eye_x)
                  + (offset_center_y - offset_eye_y)
                                  *(offset_center_y - offset_eye_y)
                  + (offset_center_z - offset_eye_z)
                                  *(offset_center_z - offset_eye_z)) ;

          angle_to_north = nint((double)angle_to_north) ;
          angle_to_up    = nint((double)angle_to_up)    ;
          if(angle_to_up < 2.0)   angle_to_up =   2.0 ;
          if(angle_to_up > 178.0) angle_to_up = 178.0 ;
          if(angle_to_north <   0.0) angle_to_north = angle_to_north + 360.0 ;
          if(angle_to_north > 360.0) angle_to_north = angle_to_north - 360.0 ;
          radian_to_north = radian*angle_to_north ;
          radian_to_up    = radian*angle_to_up    ;

          cn = cos(radian_to_north) ;
          sn = sin(radian_to_north) ;

          offset_center_z = offset_eye_z + c_dist*cos(radian_to_up) ;
          offset_center_x = offset_eye_x +
                            c_dist*sin(radian_to_up)*sin(radian_to_north) ;
          offset_center_y = offset_eye_y +
                            c_dist*sin(radian_to_up)*cos(radian_to_north) ;
/*
 *==============================================================================
 *    Code to change camera position
 *==============================================================================
 */
        }else{
          cn = cos(radian_to_north) ;
          sn = sin(radian_to_north) ;
          switch(sym){
            case SDLK_LEFT:
              offset_eye_x    -= del_d*cn ;
              offset_center_x -= del_d*cn ;
              offset_eye_y    += del_d*sn ;
              offset_center_y += del_d*sn ;
              break ;
            case SDLK_RIGHT:
              offset_eye_x    += del_d*cn ;
              offset_center_x += del_d*cn ;
              offset_eye_y    -= del_d*sn ;
              offset_center_y -= del_d*sn ;
              break ;
            case SDLK_UP:
              offset_eye_x    += del_d*sn ;
              offset_center_x += del_d*sn ;
              offset_eye_y    += del_d*cn ;
              offset_center_y += del_d*cn ;
              break ;
            case SDLK_DOWN:
              offset_eye_x    -= del_d*sn ;
              offset_center_x -= del_d*sn ;
              offset_eye_y    -= del_d*cn ;
              offset_center_y -= del_d*cn ;
              break ;
            case SDLK_PAGEUP:
              offset_eye_z    += del_d    ;
              offset_center_z += del_d    ;
              break ;
            case SDLK_PAGEDOWN:
              offset_eye_z    -= del_d    ;
              offset_center_z -= del_d    ;
#ifndef kb_dev
// Stop the viewpoint dropping too far
//   This should be just above the local land surface level
              if(offset_eye_z < 0.0){
                offset_eye_z    =   0.1*scalei ;  // 0.1 m above baseline
                offset_center_z =   0.1*scalei ;
              }
#endif
               break ;
            default:
              break ;
          }
        }
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

      camera_changed = 1 ;
      camera_new_position() ;
//  Trigger graphics_cull checks (myGlutIdle in graphics.c)
      new_viewpoint = 1 ;

      return 0 ;
}
