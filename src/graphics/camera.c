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
        if(ip)printf(" camera eye = %f %f %f,  center = %f %f %f\n",
               camera->offset_eye_x, camera->offset_eye_x, camera->offset_eye_x,
               camera->offset_center_x, camera->offset_center_y, camera->offset_center_z) ;
        offset_eye_x = scalei*camera->offset_eye_x ;
        offset_eye_y = scalei*camera->offset_eye_y ;
        offset_eye_z = scalei*camera->offset_eye_z ;
        offset_center_x = scalei*camera->offset_center_x ;
        offset_center_y = scalei*camera->offset_center_y ;
        offset_center_z = scalei*camera->offset_center_z ;
//        printf("        eye = %f %f %f,  center = %f %f %f\n",
//               offset_eye_x, offset_eye_x, offset_eye_x,
//               offset_center_x, offset_center_y, offset_center_z) ;

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

/*
 *   Routine to open and read data from file ~/.zr/lookat.txt
 *   to set position of camera.
 *   The file gives the tile location and position of the lookat point
 */

int  read_lookat_file(){

int  n ;
int  tile_x, tile_y ;
double lookat_x, lookat_y, lookat_z ;
size_t slen, nread ;
FILE *fp ;
char *file_name ;
char *look_name = "//lookat.txt" ;
char *line = NULL ;
char *token_a[3]   ;
char *token        ;


      tile_x = tile_y = 9999 ;
      lookat_x = lookat_y = lookat_z = 0.0001;
      token_a[0] = token_a[1] = token_a[2] = NULL ;

      n = strlen(ZRdotdir) + strlen(look_name) + 1 ;
      file_name = (char *) malloc(n) ;
      strcpy(file_name,ZRdotdir)  ;
      strcat(file_name,look_name) ;
      printf("  file_name = %s\n",file_name) ;

      fp = fopen(file_name,"r") ;
      if(fp == NULL){
        printf("  Lookat file '%s' not found\n",file_name) ;
        return 0 ;
      }
#ifdef SDL2
      while ((int)(nread = getline1(&line, &slen, fp)) != -1) {
#else
      while ((int)(nread = getline(&line, &slen, fp)) != -1) {
#endif
        token = strtok(line," \n\r\t");
        printf("  token = %s\n",token) ;
        if((token==NULL))continue ;         // If blank skip line
        if(token[0] == '#')continue;        // If comment ('#') skip line
        if(token_a[0])free(token_a[0]) ;
        token_a[0] = strdup(token) ;        // "strdup" defined in system.c
        printf("  token_a[0] = %s\n",token_a[0]) ;

        token = strtok(NULL," \n\r\t");
        if((token==NULL))continue ;         // If no 2nd token skip line
        printf("  token = %s\n",token) ;
        if(strcmp(token,"="))continue;      // If not '=' skip line
        if(token_a[1])free(token_a[1]) ;
        token_a[1] = strdup(token) ;
        printf("  token_a[1] = %s\n",token_a[1]) ;

        token = strtok(NULL," \n\r\t");
        if((token==NULL))continue ;         //  If no 3rd token skip line
        printf("  token = %s\n",token) ;
        if(token_a[2])free(token_a[2]) ;
        token_a[2] = strdup(token) ;
        printf("  token_a[2] = %s\n",token_a[2]) ;

        if(!strcmp(token_a[0],"tile_x")){
          tile_x = atoi(token_a[2]);
        }
        if(!strcmp(token_a[0],"tile_y")){
          tile_y = atoi(token_a[2]);
        }

        if(!strcmp(token_a[0],"lookat_x")){
          lookat_x = atof(token_a[2]);
        }
        if(!strcmp(token_a[0],"lookat_y")){
          lookat_y = atof(token_a[2]);
        }
        if(!strcmp(token_a[0],"lookat_z")){
          lookat_z = atof(token_a[2]);
        }
      }
/*
 *  Calculate position
 */
      printf("  Position:  Tile = %i %i, tile0 = %i %i, X,Y,Z = %7.2f %7.2f %7.2f\n",
             tile_x, tile_y, tile_x0, tile_y0, lookat_x, lookat_y, lookat_z) ;
      if(0 == tile_x || 0 == tile_y || 0.0001 == lookat_x || 0.0001 == lookat_y || 0.0001 == lookat_z ){
        printf("  Some values unset.  Position not changed\n") ;
        return 0 ;
      }

      cameras[0].offset_center_x = (tile_x - tile_x0 + 0.5)*tile_size + lookat_x ;
      cameras[0].offset_center_y = (tile_y - tile_y0 + 0.5)*tile_size + lookat_y ;
      cameras[0].offset_center_z = (lookat_z - tile_h0) ;

      cameras[0].offset_eye_x = cameras[0].offset_center_x ;
      cameras[0].offset_eye_y = cameras[0].offset_center_y + 30.0;
      cameras[0].offset_eye_z = cameras[0].offset_center_z + 30.0 ;

//        printf(" AA camera eye = %f %f %f,  center = %f %f %f\n",
//               cameras[0].offset_eye_x, cameras[0].offset_eye_x, cameras[0].offset_eye_x,
//               cameras[0].offset_center_x, cameras[0].offset_center_y, cameras[0].offset_center_z) ;
//      current_camera = 0 ;
      camera_changed = 1 ;

      fclose(fp) ;


      return 0;
}
