/*
 * =============================================================================
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  display_info.h
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

int display_info = 0 ;      //  When set display extra information on world
                            //    and track items.
GLfloat display_info_radius = 100.0 ;
GLfloat display_info_height =  3.0 ;

int display_track_info_on = 0 ; // When set display extra track data, see:
                                //  'display_track_info' (display_info.c)
                                // Toggle using 'alt'-'t'

typedef struct f9structure {
  TrainNode *last_train ;
  int       wagon_n0 ; // Number of wagons outside left of window
  int       n_wagons ; // Number of wagons
  int       x0 ;    //  Origin (bottom-lefto corner f window
  int       y0 ;
  int       x1 ;    //  Top right corner
  int       y1 ;

  int       xc ;    //  Centre
  int       x2 ;    //  Left arrow limit
  int       x3 ;    //  Right arrow limit
  int       x4 ;    //  Left slider limit
  int       x5 ;    //  Right slider limit
  int       y2 ;    //  Bottom Line
  int       y3 ;    //  Top Line
  int       y4 ;    //  Wagon bottom
  int       y5 ;    //  Wagon top
  int       yc ;    //  Wagon centre
  int       x_off ;
  int       y_off ;
  int       wagon_n1 ;  // Value when button pressed
  int       width ;
  int       height ;
  double    dx    ;  // Width of slider unit
} F9_Data ;

F9_Data f9_data ;


