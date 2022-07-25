/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  camera.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Camera routines used by ZR
 *
 *==============================================================================
 */


int cameras_init(){

CameraNode *camera ;

      current_camera = -1 ;   //  Unset
      camera_last    = -1 ;   //  Unset
/*
 *  Initialise default camera '0' from the 'offset' variables.  The  initial
 *  values of these variables are defined in file 'graphics.h'.
 */
      camera = &cameras[0] ;
      camera->offset_eye_x    = plot_scale*offset_eye_x ;
      camera->offset_eye_y    = plot_scale*offset_eye_y ;
      camera->offset_eye_z    = plot_scale*offset_eye_z ;
      camera->offset_center_x = plot_scale*offset_center_x ;
      camera->offset_center_y = plot_scale*offset_center_y ;
      camera->offset_center_z = plot_scale*offset_center_z ;


      return 0 ;
}

/*
 *==============================================================================
 *  Routine camera_new_position.
 *
 *  Thus updates the absolute and relative positions and associated
 *  variables before calling GluLookAt to reset the OpenGL
 *  modelview matrix.
 *
 *  It is called after choosing a new camera, after moving the
 *  traveller of choosing a new one, or after the window has been
 *  resized.
 *
 *==============================================================================
 */

int camera_new_position(){

  int         j ;
  int         ip = 0 ;         //  Debug
  double      scalei=1.0/plot_scale ;
  GLdouble ea[4], et[4], ca[4], ct[4], mm[4][4]  ;
  CameraNode  *camera  ;
  GLfloat     v4[4]  ;
  TravellerNode *t   ;
  char        *my_name = "camera_new_position" ;
      if(ip)printf(" Enter %s\n",my_name) ;
/*
 *==============================================================================
 *  Code for normal preprocessor options (i.e. not Texture or Shapes)
 *==============================================================================
 */
#ifdef _Display_Normal
      if(ip){
        printf(" Enter %s _Display_Normal\n",my_name) ;
        printf(" camera_changed =  %i\n",camera_changed) ;
        printf(" current_camera =  %i\n",current_camera) ;
        printf(" camera_last =  %i\n",camera_last) ;
      }

      if(!camera_changed)return 0 ;
      camera_changed = 0 ;
      camera   = &cameras[current_camera] ;
/*
 *==============================================================================
 *  If the camera has been changed initialise offsets from
 *  origin (origin can be absolute or the position of a traveller)
 *  If camera 4 is called more then once the position swaps to the other
 *  side of the tracks.
 *==============================================================================
 */
      if((current_camera != camera_last) || current_camera == 4){
        offset_eye_x = scalei*camera->offset_eye_x ;
        offset_eye_y = scalei*camera->offset_eye_y ;
        offset_eye_z = scalei*camera->offset_eye_z ;
        offset_center_x = scalei*camera->offset_center_x ;
        offset_center_y = scalei*camera->offset_center_y ;
        offset_center_z = scalei*camera->offset_center_z ;

//  Other graphic variables

        radian_to_north = atan2(offset_center_x-offset_eye_x,
                                offset_center_y-offset_eye_y) ;
        radian_to_up    = 0.5*M_PI - atan2(offset_center_z-offset_eye_z,
                                sqrt(pow(offset_center_x-offset_eye_x, 2)
                                    +pow(offset_center_y-offset_eye_y, 2)) ) ;
        angle_to_north  = degree*radian_to_north  ;
        angle_to_up     = degree*radian_to_up  ;
        if(ip)printf(" %s, angle_to_north = %10.6f, angle_to_up = %10.6f\n",
               my_name, angle_to_north, angle_to_up);
      }
/*
 *==============================================================================
 *  If camera position is absolute copy offset coordinates to lookat
 *  otherwise calculate new lookat (absolute) values
 *==============================================================================
 */
      if(current_camera == 0 || current_camera == 8){
        lookat_eye_x    = offset_eye_x    ;
        lookat_eye_y    = offset_eye_y    ;
        lookat_eye_z    = offset_eye_z    ;
        lookat_center_x = offset_center_x ;
        lookat_center_y = offset_center_y ;
        lookat_center_z = offset_center_z ;
      }else{
        if(current_camera == 3){
          t     = player_train->last->traveller ;
        }else{
          t     = player_train->first->traveller ;
          if(current_camera == 6 && current_camera != camera_last){
            offset_eye_y =  player_train->first->raw_wagon->length*0.5 - 2.0;
            if(offset_eye_y< 3)offset_eye_y = 10.0 ;
            offset_eye_y = offset_eye_y/plot_scale ;
          }
        }
/*
 *==============================================================================
 *   Call routine to return OpenGl modelview matrix for the traveller position.
 *
 *   Note the returned matrix includes the transformation from metres
 *   to the basic model grid scale (usually 2048 m)
 *
 *   Todo:  Use extra parameter to skip this transform
 *==============================================================================
 */
        traveller2modelview(t, &mm[0][0]) ;

        if(ip){
          printf(" Routine %s\n",my_name) ;
          printf(" Matrix mm\n");
          for(j=0;j<4;j++){
            if(ip)printf("    %10.6f %10.6f %10.6f %10.6f\n",
                              mm[0][j],mm[1][j],mm[2][j],mm[3][j]) ;
          }
        }
/*
 *==============================================================================
 *  Calculate offsets in metres
 *
 *  Todo:  Modify this section and matrix_product routine to use pointers
 *==============================================================================
 */
        ea[0] = offset_eye_x*plot_scale ;
        ea[1] = offset_eye_y*plot_scale ;
        ea[2] = offset_eye_z*plot_scale ;
        ea[3] = 1.0          ;
        ca[0] = offset_center_x*plot_scale ;
        ca[1] = offset_center_y*plot_scale ;
        ca[2] = offset_center_z*plot_scale ;
        ca[3] = 1.0             ;
/*
 *==============================================================================
 *  Use matrix to covert from position relative to the traveller
 *  (measured in metres) to the absolute positions in model
 *  grid units
 *==============================================================================
 */
        zr_gl_matrix_prod(&et[0],&mm[0][0],&ea[0]) ;
        zr_gl_matrix_prod(&ct[0],&mm[0][0],&ca[0]) ;

        if(ip){
        printf(" Vector e\n") ;
        printf("    %10.6f %10.6f %10.6f %10.6f\n",ea[0],ea[1],ea[2],ea[3]) ;
        printf("    %10.6f %10.6f %10.6f %10.6f\n",ea[0]*plot_scale,ea[1]*plot_scale,ea[2]*plot_scale,ea[3]*plot_scale) ;
        printf(" Vector et\n") ;
        printf("    %10.6f %10.6f %10.6f %10.6f\n",et[0],et[1],et[2],et[3]) ;
        printf("    %10.6f %10.6f %10.6f %10.6f\n",et[0]*plot_scale,et[1]*plot_scale,et[2]*plot_scale,et[3]*plot_scale) ;
        printf(" Vector c\n") ;
        printf("    %10.6f %10.6f %10.6f %10.6f\n",ca[0],ca[1],ca[2],ca[3]) ;
        printf("    %10.6f %10.6f %10.6f %10.6f\n",ca[0]*plot_scale,ca[1]*plot_scale,ca[2]*plot_scale,ca[3]*plot_scale) ;
        printf(" Vector ct\n") ;
        printf("    %10.6f %10.6f %10.6f %10.6f\n",ct[0],ct[1],ct[2],ct[3]) ;
        printf("    %10.6f %10.6f %10.6f %10.6f\n",ct[0]*plot_scale,ct[1]*plot_scale,ct[2]*plot_scale,ct[3]*plot_scale) ;
        }
        lookat_eye_x    = et[0] ;
        lookat_eye_y    = et[1] ;
        lookat_eye_z    = et[2] ;
        lookat_center_x = ct[0] ;
        lookat_center_y = ct[1] ;
        lookat_center_z = ct[2] ;
      }
/*
 *==============================================================================
 *  Set camera last
 *==============================================================================
 */
      camera_last = current_camera ;
/*
 *==============================================================================
 *  Reset tile containing viewpoint
 *==============================================================================
 */
      tile_eye_x0 = lookat_eye_x ;
      tile_eye_y0 = lookat_eye_y ;
      if(lookat_eye_x<0.0) tile_eye_x0 = tile_eye_x0 - 1 ;
      if(lookat_eye_y<0.0) tile_eye_y0 = tile_eye_y0 - 1 ;
      if(ip)printf(" Camera AA :: Tile %i %i :: Eye %f %f :: Last %i %i\n",
                  tile_eye_x0, tile_eye_y0, lookat_eye_x, lookat_eye_y,
                  last_eye_x0, last_eye_y0) ;
/*
 *==============================================================================
 *  If viewpoint tile has changed cull shapes etc.
 *==============================================================================
 */
      if(((tile_eye_x0 != last_eye_x0) || (tile_eye_y0 != last_eye_y0))){
        if(ip)printf(" Camera :: Tile %i %i :: Eye %f %f :: Last %i %i\n",
                  tile_eye_x0, tile_eye_y0, lookat_eye_x, lookat_eye_y,
                  last_eye_x0, last_eye_y0) ;
        graphics_reset() ;
        last_eye_x0 = tile_eye_x0 ;
        last_eye_y0 = tile_eye_y0 ;
        new_viewpoint = 0 ;
      }
/*
 *==============================================================================
 *  Setup Modelview matrix from scratch
 *==============================================================================
 */
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      if(ip)printf(" Camera BB gluLookAt %f %f %f : %f %f %f : %f %f %f\n",
                lookat_eye_x,lookat_eye_y,lookat_eye_z,
                lookat_center_x,lookat_center_y,lookat_center_z,
                lookat_up_x,lookat_up_y,lookat_up_z) ;

      gluLookAt(lookat_eye_x,lookat_eye_y,lookat_eye_z,
                lookat_center_x,lookat_center_y,lookat_center_z,
                lookat_up_x,lookat_up_y,lookat_up_z) ;
//  REQUIRED after MODELVIEW CHANGE
      if(ip)printf(" Final processing %s\n",my_name) ;

      zr_setp4(v4,light0_altde,light0_polar) ;
      glLightfv(GL_LIGHT0, GL_POSITION, v4);
      initialise_eye_vectors() ;
      initialise_clip_planes(clip_a) ;
      check_topographic_blocks() ;
/*
 *==============================================================================
 *  Section when displaying textures or shapes  (Preprocessor options)
 *==============================================================================
 */
#else
      if(!camera_changed)return 0 ;
      camera_changed = 0 ;

      lookat_eye_x    = offset_eye_x    ;
      lookat_eye_y    = offset_eye_y    ;
      lookat_eye_z    = offset_eye_z    ;
      lookat_center_x = offset_center_x ;
      lookat_center_y = offset_center_y ;
      lookat_center_z = offset_center_z ;
#ifdef _Display_Textures
      lookat_up_x     = 0.0 ;
      lookat_up_y     = 1.0 ;
      lookat_up_z     = 0.0 ;
      angle_to_north = 0 ;
      angle_to_up    = 0 ;
      radian_to_north = 0 ;
      radian_to_up    = 0 ;
#else
      lookat_up_x     = 0.0 ;
      lookat_up_y     = 0.0 ;
      lookat_up_z     = 1.0 ;
//  Other graphic variables
      radian_to_north = atan2(offset_center_x-offset_eye_x,
                              offset_center_y-offset_eye_y) ;
      radian_to_up    = 0.5*M_PI - atan2(offset_center_z-offset_eye_z,
                              sqrt(pow(offset_center_x-offset_eye_x, 2)
                                  +pow(offset_center_y-offset_eye_y, 2)) ) ;
      angle_to_north  = degree*radian_to_north  ;
      angle_to_up     = degree*radian_to_up  ;
      if(ip)printf(" %s, angle_to_north = %10.6f, angle_to_up = %10.6f\n",
              my_name,angle_to_north, angle_to_up);
#endif
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      if(ip)printf(" BB gluLookAt\n") ;
      gluLookAt(lookat_eye_x,lookat_eye_y,lookat_eye_z,
                lookat_center_x,lookat_center_y,lookat_center_z,
                lookat_up_x,lookat_up_y,lookat_up_z) ;
//  REQUIRED after MODELVIEW CHANGE
      if(ip)printf(" Final processing %s\n",my_name) ;

      zr_setp4(v4,light0_altde,light0_polar) ;
      glLightfv(GL_LIGHT0, GL_POSITION, v4);
      initialise_eye_vectors() ;
      initialise_clip_planes(clip_a) ;
      graphics_reset() ;

#endif
/*
 *  Exit
 */
      if(ip){
        printf(" Exit %s\n",my_name) ;
        fflush(NULL) ;
      }
      return 0;
}
