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

#if 0
/*
 * If viewpoint has moved to a new tile, check for changes in
 * the required shapes and textures
 */
      if(lookat_eye_x<0) tile_eye_x-- ;  //  Correct integer value if negative
      if(lookat_eye_y<0) tile_eye_y-- ;
      if(new_viewpoint && ((tile_eye_x != last_eye_x)
                        || (tile_eye_y != last_eye_y))){
        if(ip)printf(" glut_idle :: Tile %i %i :: Eye %f %f :: Last %i %i\n",
                  tile_eye_x, tile_eye_y, lookat_eye_x, lookat_eye_y,
                  last_eye_x, last_eye_y) ;
        graphics_cull() ;
        last_eye_x = tile_eye_x ;
        last_eye_y = tile_eye_y ;
        new_viewpoint = 0 ;
      }
#endif
/*
 *  Determine the time interval since the routine last called.
 */
#ifdef _Display_Normal
      clock_gettime(CLOCK_MONOTONIC, &run_clock1)    ;  //  Run clock
      run_seconds = (run_clock1.tv_sec - run_clock0.tv_sec)
                  + (run_clock1.tv_nsec - run_clock0.tv_nsec)*0.000000001 ;
      if(eng_seconds0<0) eng_seconds0 = run_seconds ;
/*
 *  If time >~ 1/30 second update dynamics
 */
      if(run_seconds - eng_seconds0>0.03){
//      if(run_seconds - eng_seconds0>1.0){
double dist ;
        dist = eng_speed*(run_seconds - eng_seconds0) ;
        dist_tot = dist_tot + dist ;
        if(ip>1 || (ip == 1 && dist != 0.0)){
          printf("\n====================================================\n");
          printf("  In routine %s\n",my_name) ;
          printf("  run sec = %f :: eng sec = %f :: speed = %f :: dist = %f"
             " :: dist_tot = %f\n",
             eng_seconds0,run_seconds,eng_speed,dist, dist_tot) ;
        }
        if(dist!=0.0){
          if(ip)printf("  Routine %s :: Call trv_move\n",my_name) ;
          trv_move(&trav_node_0,dist) ;
          if(ip)printf("  Routine %s :: Return from trv_move\n",my_name) ;
          camera_changed = 1 ;
        }
        eng_seconds0 = run_seconds ;
      }
#endif
/*
 * According to the GLUT specification, the current window is
 * undefined during an idle callback.  So we need to explicitly change
 *it if necessary
 */
        glutPostRedisplay();
        if(ip>1)printf(" Exit myGlutIdle\n");
        return ;
}

void  zr_clock_gettime(struct timespec clock[4]){

//      clock_gettime(CLOCK_REALTIME,           &clock[0] );
      clock_gettime(CLOCK_REALTIME_COARSE,    &clock[0] );
//      clock_gettime(CLOCK_MONOTONIC,          &clock[1] );
      clock_gettime(CLOCK_MONOTONIC_COARSE,   &clock[1] );
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &clock[2] );
      clock_gettime(CLOCK_THREAD_CPUTIME_ID,  &clock[3] );

      return ;
}
