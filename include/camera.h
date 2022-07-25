/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  camera.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Camera variables used by ZR
 *
 *==============================================================================
 */

int current_camera = -1  ;  //  Current camera (-1 is unset)
int camera_changed =  1  ;  //  If non-zero camera position needs updating
int camera_last    = -1  ;  //  Previous camera position

/*
*  Note: all camera positions are relative to origin of
*  current wagon except for the roaming camera
 */

typedef struct cameranode {
  GLdouble offset_eye_x     ;   // Position of eye (roaming camera)
  GLdouble offset_eye_y     ;   // or offset from shape origin
  GLdouble offset_eye_z     ;   // for cameras connected to train
  GLdouble offset_center_x  ;   // Position looked at (roaming camera)
  GLdouble offset_center_y  ;   // or offset position looked at
  GLdouble offset_center_z  ;   // for cameras connected to train
  int      position         ;   // 0 = absolute, 1=relative to wagon, 2=trackside
} CameraNode ;
//  In MSTS space ::  x    z    y
#if 0
CameraNode  cameras[]  = {
                  {  0.0,  0.0,  0.0, 10.0, 10.0,  0.0, 0 } ,   //  Default/Error
                  { -2.0,  4.0,  -4.0, -2.0, 4.0, 20.0, 1 } ,   //  1 Front cab
                  {  15.0,  15.0,  40.0,  0.0, 0.0,  0.0, 1 } ,   //  2 Front train view
                  { -15.0,  15.0, -40.0,  0.0, 0.0,  0.0, 1 } ,   //  3 Rear train view
                  { 10.0,  1.0,  0.0,  0.0,  1.0,  0.0, 2 } ,   //  4 Trackside view
                  {  0.0,  0.0,  2.0, -1.0,  0.0,  2.0, 1 } ,   //  5 Passenger view
                  {  2.0,  1.5,  2.0,  2.0,  1.5, 22.0, 1 } ,   //  6 Coupler view
                  {  2.0,  5.0,  2.0,  2.0,  5.0, 22.0, 1 } ,   //  7 Yard view
                  {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, 0 } } ; //  8 Free view
#else
//  In ZR space ::  x    y    z
CameraNode  cameras[]  = {
                  {   0.0,   0.0,   0.0,  10.0,   0.0,  10.0, 0 } ,   //  Default/Error
                  {  -2.0,  -4.0,   4.0,  -2.0,  20.0,   4.0, 1 } ,   //  1 Front cab
                  {  15.0,  40.0,  15.0,   0.0, -10.0,   4.0, 1 } ,   //  2 Front train view
                  { -15.0, -40.0,  15.0,   0.0,  10.0,   4.0, 1 } ,   //  3 Rear train view
                  { 10.0,  100.0,   2.5,   0.0,   0.0,   2.5, 1 } ,   //  4 Trackside view
                  { -1.3,   -3.3,   3.0,  10.0,   0.0,   3.0, 1 } ,   //  5 Passenger view
                  {  2.0,    2.0,   1.5,   2.0,  22.0,   1.5, 1 } ,   //  6 Coupler view
                  { 10.0,    0.0,   1.0,   0.0,   0.0,   1.0, 2 } ,   //  7 Yard view
                  {  0.0,    0.0,   0.0,   0.0,   0.0,   0.0, 0 } } ; //  8 Free view
#endif

