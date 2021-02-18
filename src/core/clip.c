/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  clip.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to determine the current window clip planes and use the result.
 *
 *==============================================================================
 */

/*
 * *****************************************************************************
 *   Reshape - called when window size is changed
 *         also called when the graphics screen is opened
 * *****************************************************************************
 */

void matmult(GLfloat *res, GLfloat *a, GLfloat *b) ;

int  initialise_clip_planes(GLfloat clipv[6][4]){

int     i, j, m ;
int     ip = 0 ;         // Debug
double  temp ;
GLfloat modelview[16]  ;
GLfloat projection[16] ;
GLfloat prodmat[16]    ;

      for(m=0;m<1;m++){
        glGetFloatv(GL_MODELVIEW_MATRIX, modelview)  ;
        glGetFloatv(GL_PROJECTION_MATRIX, projection) ;

        if(ip){
          printf("\n Model view matrix\n") ;
          printf("    %10.6f  %10.6f  %10.6f  %10.6f \n",
                    modelview[ 0],modelview[ 4],modelview[ 8],modelview[12]);
          printf("    %10.6f  %10.6f  %10.6f  %10.6f \n",
                    modelview[ 1],modelview[ 5],modelview[ 9],modelview[13]);
          printf("    %10.6f  %10.6f  %10.6f  %10.6f \n",
                    modelview[ 2],modelview[ 6],modelview[10],modelview[14]);
          printf("    %10.6f  %10.6f  %10.6f  %10.6f \n",
                    modelview[ 3],modelview[ 7],modelview[11],modelview[15]);

          printf("\n Projection matrix\n") ;
          printf("    %10.6f  %10.6f  %10.6f  %10.6f \n",
                    projection[ 0],projection[ 4],projection[ 8],projection[12]);
          printf("    %10.6f  %10.6f  %10.6f  %10.6f \n",
                    projection[ 1],projection[ 5],projection[ 9],projection[13]);
          printf("    %10.6f  %10.6f  %10.6f  %10.6f \n",
                    projection[ 2],projection[ 6],projection[10],projection[14]);
          printf("    %10.6f  %10.6f  %10.6f  %10.6f \n",
                    projection[ 3],projection[ 7],projection[11],projection[15]);
          printf("\n") ;
        }
/*
 *  Calculate M*P
 */

        if(0==m){
          matmult(prodmat, modelview, projection) ;
        }else{
          glPushMatrix();
          glLoadMatrixf(projection);
          glMultMatrixf(modelview);
          glGetFloatv(GL_MODELVIEW_MATRIX, prodmat);
          glPopMatrix();
        }

        clipv[0][0] = prodmat[ 3]-prodmat[ 0] ;  //  Right clip plane
        clipv[0][1] = prodmat[ 7]-prodmat[ 4] ;
        clipv[0][2] = prodmat[11]-prodmat[ 8] ;
        clipv[0][3] = prodmat[15]-prodmat[12] ;

        clipv[1][0] = prodmat[ 3]+prodmat[ 0] ;  //  Left clip plane
        clipv[1][1] = prodmat[ 7]+prodmat[ 4] ;
        clipv[1][2] = prodmat[11]+prodmat[ 8] ;
        clipv[1][3] = prodmat[15]+prodmat[12] ;

        clipv[2][0] = prodmat[ 3]-prodmat[ 1] ;  //  Top clip plane
        clipv[2][1] = prodmat[ 7]-prodmat[ 5] ;
        clipv[2][2] = prodmat[11]-prodmat[ 9] ;
        clipv[2][3] = prodmat[15]-prodmat[13] ;

        clipv[3][0] = prodmat[ 3]+prodmat[ 1] ;  //  Bottom clip plane
        clipv[3][1] = prodmat[ 7]+prodmat[ 5] ;
        clipv[3][2] = prodmat[11]+prodmat[ 9] ;
        clipv[3][3] = prodmat[15]+prodmat[13] ;

        clipv[4][0] = prodmat[ 3]-prodmat[ 2] ;  //  Far clip plane
        clipv[4][1] = prodmat[ 7]-prodmat[ 6] ;
        clipv[4][2] = prodmat[11]-prodmat[10] ;
        clipv[4][3] = prodmat[15]-prodmat[14] ;

        clipv[5][0] = prodmat[ 3]+prodmat[ 2] ;  //  Near clip plane
        clipv[5][1] = prodmat[ 7]+prodmat[ 6] ;
        clipv[5][2] = prodmat[11]+prodmat[10] ;
        clipv[5][3] = prodmat[15]+prodmat[14] ;
/*
 *  Normalise clip vectors
 */
        for(i=0;i<6;i++){
          temp = 1.0/sqrt(clipv[i][0]*clipv[i][0] + clipv[i][1]*clipv[i][1]
                                                  + clipv[i][2]*clipv[i][2]) ;
          for(j=0;j<4;j++){
            clipv[i][j] = clipv[i][j]*temp ;
          }
        }
        if(ip){
          printf(" Product matrix clipping planes:\n") ;
          for(i=0;i<6;i++){
            printf("    %10.6f  %10.6f  %10.6f  %10.6f \n",clipv[i][0],clipv[i][1],clipv[i][2],clipv[i][3]);
          }
          printf("\n") ;
        }
      }
      return 0 ;
}

void matmult(GLfloat *res, GLfloat *a, GLfloat *b) {

  int i, j, k ;
      for(i=0;i<4;i++) {
        for(j=0;j<4;j++) {
          res[i*4+j] = 0.0;
          for (int k = 0; k < 4; k++) {
            res[i*4+j] += a[i*4+k] * b[k*4+j];
          }
        }
      }
      return ;
}
/*
 *   Routine to check clipping planes
 *   The eye viewpoint should satisby the clipping equation
 *   The lookat point should give positive values
 */
void  check_clip_planes(){

int      i, j, k ;
GLfloat  x, y, z, sum[6] ;

      x = lookat_eye_x ;
      y = lookat_eye_y ;
      z = lookat_eye_z ;

      printf("\n") ;
      for(k=0;k<6;k++){
        sum[k] = x*clip_a[k][0] + y*clip_a[k][1] + z*clip_a[k][2] + clip_a[k][3] ;
        printf("  check_clip_planes : eye    :: k = %i,  sum = %9.6f\n",k,(double)sum[k]);
      }

      x = lookat_centre_x ;
      y = lookat_centre_y ;
      z = lookat_centre_z ;

      printf("\n") ;
      for(k=0;k<6;k++){
        sum[k] = x*clip_a[k][0] + y*clip_a[k][1] + z*clip_a[k][2] + clip_a[k][3] ;
        printf("  check_clip_planes : lookat :: k = %i,  sum = %9.6f\n",k,(double)sum[k]);
      }

      return ;
}


