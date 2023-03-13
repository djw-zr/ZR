/*
 * =============================================================================
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  train_operations_window.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   The file includes the routines used to initialise and later modify
 *   the train operations window.
 *
 *   Key F9 toggles the display of this window.  The display routine
 *   'display_train_operations', which contains all the OpenGL routine calls
 *   is in file 'display_info.c'.
 *
 * =============================================================================
 */

int init_train_operations_window(int first_time){

  int      width  = 530 ;
  int      height =  80 ;
  int      band_height = 17 ;
  int      wagon_width = 20 ;
  F9_Data  *f9 = &(f9_data)    ;
  GLfloat  w = viewport_width  ;
  GLfloat  h = viewport_height ;

      if(first_time){
        f9->xc = 0.5*w            ;
        f9->x0 = f9->xc - width/2 ;
        f9->y1 = h - 40           ;
        f9->y0 = f9->y1 - height  ;
        f9->width  = width        ;
        f9->height = height       ;
      }else{
        f9->xc = f9->x0 + width/2 ;
        f9->y1 = f9->y0 + height  ;
      }

      f9->last_train = NULL ;

      f9->x1 = f9->x0 + width ;
      f9->x2 = f9->x0 + band_height ;
      f9->x3 = f9->x1 - band_height ;

      f9->y2 = f9->y0 + band_height ;
      f9->y3 = f9->y1 - band_height ;

      f9->yc = 0.5*(f9->y2 + f9->y3) ;
      f9->y4 = f9->yc - wagon_width/2 ;
      f9->y5 = f9->yc + wagon_width/2 ;

      f9->x4 = 0 ;
      f9->x5 = 0 ;

      return 0 ;
}

/*
 *  Routine to process mouse button actions associated with the train
 *  operations window
 */

int train_operations_mouse(int button,int state,int x, int y){

  int  ip = 0 ;
  int  n, xx, xx0, xx1 ;

      if(ip)printf(" Train Operations %i %i %i %i %p\n",
                button, state, x, y, (void *)f9_data.last_train) ;
      if(f9_data.last_train == NULL) return 0 ;
      if(ip)printf("     %i %i %i\n",f9_data.y2,f9_data.x2,f9_data.x3) ;
/*
 * Arrow buttons or slider clicked : Move displayed part of train
 */
      if(y<f9_data.y2){
        if(ip)printf(" AA\n") ;
// Left arrow region
        if(x<f9_data.x2){
          if(f9_data.wagon_n0 > 0)f9_data.wagon_n0-- ;
          return 0 ;
// Right arrow region
        }else if(x>f9_data.x3){
          if(f9_data.n_wagons > f9_data.wagon_n0 + 6)f9_data.wagon_n0++ ;
          return 0 ;
        }else{
// Slider region
          f9_data.x_off    = x - f9_data.x0   ;
          f9_data.wagon_n1 = f9_data.wagon_n0 ;
          return 2 ;
        }
      }
// Sub-window header - used to move sub-window
      if(y>f9_data.y3){
        if(ip)printf(" BB\n") ;
        f9_data.x_off = f9_data.x0 - x ;
        f9_data.y_off = f9_data.y0 - y ;
        return 1 ;
      }
/*
 *  Train selection
 */
      if(y>f9_data.y4 && y<f9_data.y5){
        if(ip)printf("  Click in wagon line\n") ;
        if(f9_data.n_wagons<8){
          xx  = 0.5*( f9_data.n_wagons*80 + (f9_data.n_wagons-1)*10 ) ;
          xx0 = f9_data.xc - xx ;
          xx1 = f9_data.xc + xx ;
        }else{
          xx0 =f9_data.x0 ;
          xx1 =f9_data.x1 ;
        }
        if(x<=xx0 || x>=xx1) return 0 ;
        n = (x - xx0)/90.0 ;
        if(x > (xx0 + n*90 + 80) ){
          split_train_at_wagon(f9_data.last_train, n+f9_data.wagon_n0) ;
        }else{
          trigger_wagon_event(f9_data.last_train, n+f9_data.wagon_n0) ;
        }
        return 0 ;
      }
      if(ip)printf(" CC  y = %i, y4 = %i, y5 = %i\n",y,f9_data.y4,f9_data.y5) ;
      return 0 ;
}

void  move_train_operations_window(int x, int y){

  GLfloat  w = viewport_width ;
  GLfloat  h = viewport_height ;

      f9_data.x0 = x + f9_data.x_off ;
      if(f9_data.x0<0)f9_data.x0 = 0 ;
      else if(f9_data.x0+f9_data.width  > w)f9_data.x0 = w - f9_data.width  ;

      f9_data.y0 = y + f9_data.y_off ;
      if(f9_data.y0<0)f9_data.y0 = 0 ;
      else if(f9_data.y0+f9_data.height > h)f9_data.y0 = h - f9_data.height ;

      init_train_operations_window(0) ;
      return ;
}

void  change_train_operations_view(int x, int y){

  int    ix ;
  double dx ;

      if(f9_data.n_wagons<=6){
        f9_data.wagon_n0 = 0 ;
        return ;
      }
      ix = f9_data.wagon_n1 + 0.5 +
                           (x - f9_data.x0 - f9_data.x_off)/f9_data.dx ;

      if(ix<0) ix = 0 ;
      if(ix>f9_data.n_wagons-6) ix = f9_data.n_wagons-6 ;

      f9_data.wagon_n0 = ix ;
      return ;
      if(1) ix = y ;   //  Dummy use of 'y' - keep the compiler happy
}

