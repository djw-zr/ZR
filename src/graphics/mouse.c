/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  Mouse.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Mouse subroutines used by ZR
 *
 *   The graphics code is based on openglut amd OpenGL 2.1
 *
 *   The grahics has teh origin in the bottom left corner, but the mouse
 *   origin is at the top left
 *
 *==============================================================================
 */

/*
 *   Mouse button handling routine
 *   button = GLUT_LEFT_BUTTON or GLUT_MIDDLE_BUTTON of GLUT_RIGHT_BUTTON
 *   state  = GLUT_DOWN or GLUT_UP
 *       A GLUT_UP follows a GLUT_DOWN even if the mouse has moved
 *       outside the window.
 */

static int mouse_subw ;  //  Sub window handling mouse movement

void mouse(int button, int state, int x, int ym){

  int y = viewport_height - ym ;

#if 0
      printf("  Mouse  %i %i :: %i %i\n",button, state, x, y) ;
      printf("  Mouse  :: %i %i :: %i %i :: %i\n",
             f9_data.x0, f9_data.x1, f9_data.y0, f9_data.y1,
             display_train_operations_on) ;
#endif
/*
 *  Middle and right buttons not used (yet!)
 */
      if(button != GLUT_LEFT_BUTTON) return ;
/*
 *  Button UP
 */
      if(state == GLUT_UP){
        mouse_subw = 0 ;
        return ;
      }
/*
 *  Train operations window.
 *  Mouse actions to move window                     subw = 1
 *                   change part of train displayed  subw = 2 if slider used
 *                   uncouple section of train
 */
      if(display_train_operations_on){
        if(x >= f9_data.x0 && x <= f9_data.x1 &&
           y >= f9_data.y0 && y <= f9_data.y1){
              mouse_subw = train_operations_mouse(button,state,x,y);
        }
        return ;
      }
      return ;
}


void motion(int x, int ym){

  int y = viewport_height - ym ;

      if(!mouse_subw)return ;
//      printf("  Motion  %i :: %i %i\n", mouse_subw, x, y) ;
/*
 *  Respond to mouse movements with left button down
 */
      switch(mouse_subw){
        case 1:
          move_train_operations_window(x,y) ;
          break ;
        case 2:
          change_train_operations_view(x, y) ;
          break ;
        default:
          break ;
      }
      return ;
}
