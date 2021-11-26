/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  graphics_idle.c
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
 *  Idle function for use with glut
 *    Eventually any non-graphics code needs to be placed in a separate
 *    thread to reduce the load on the graphics thread.
 */

void glut_idle( void )
{
int ip = 0                ;  // = 1 for debug printing
int        tile_eye_x = lookat_eye_x ;  //  Integer value of lookat_eye_x
int        tile_eye_y = lookat_eye_y ;
static int last_eye_x = 0 ;
static int last_eye_y = 0 ;
static double dist_tot = 0.0 ;
char        my_name[] = "glut_idle" ;

      if(ip)printf("  Enter %s\n",my_name) ;

/*
 * According to the GLUT specification, the current window is
 * undefined during an idle callback.  So we need to explicitly change
 *it if necessary
 */
        glutPostRedisplay();
        if(ip>1)printf(" Exit myGlutIdle\n");
        return ;
}

