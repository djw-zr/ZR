/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  zr.c
 *   Copyright 2021  David J. Webb
 *
 *   ZR main program
 *
 *   This file is part of ZR.
 *   Released under licence GPL-3.0-or-later
 *
 *   ZR is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   ZR is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *==============================================================================
 */
#include "zr.h"
#include "graphics.h"

char  my_file[] = "__FILE__" ;
int   l_pd = 0              ;  // l_pd = 1 to print debug information
int   l_pp = 0              ;  // l_pp = 1 to print after a new position
int   i_count = 0           ;  // Used to count number of debug print statements
float d_rotate  = 90.0      ;  // Deug : default rotation angle (0, 90)
float d_reflect = -1.0      ;  // Default reflection value (-1,1)
char test_shape[] = "1905-S654" ;
//char test_shape[] = "AU_Ash_20m" ;
//char test_shape[] = "BBS-NSW-RoadBridge5span" ;
//char test_shape[] = "BBS-BlockofShops4" ;
//char test_shape[] = "BBS-DerrickCraneBig" ;
//char test_shape[] = "T_Pole_Tall_zz_1tr" ;
//char test_shape[] = "BBS-BlockofShops4" ;
//char test_shape[] = "AU_GumClump2" ;  //  Used to track processing of shape
//char test_shape[] = "NSW_SemLQJct-M-L" ;  //  Used to track error in prim_state
//char test_shape[] = "a1t100mStrt" ;  //  Used to track error in track orientation
//char test_shape[] = " " ;  //  Dummy
GLubyte *c_red, *c_green, *c_blue ;
GLubyte *c_rgba ;
int   c_width, c_height ;

/**
 *   Main entry point for ZR.
 */

int main(int argc, char **argv){

char my_name[] = "zr" ;

      printf(" Program %s starting ...\n",my_name);
      process_args(argc, argv) ;

      clock_gettime(CLOCK_MONOTONIC, &run_clock0) ; //  Run clock
      process_defaults() ;
      init_data_structures() ;

      if(l_plot){
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glDepthRange(0.0,1.0);
        glutInitWindowPosition(main_window_x0, main_window_y0);
        glutInitWindowSize(main_window_width, main_window_height);
        main_window = glutCreateWindow(argv[0]);

        graphics_init() ;               //  File graphics.c

        glutDisplayFunc(display);
//      printf( "GLUI version: %3.2f\n", GLUI_Master.get_version() );
        printf("  GL_VENDOR     = %s\n",glGetString(GL_VENDOR));
        printf("  GL_RENDERER   = %s\n",glGetString(GL_RENDERER));
        printf("  GL_VERSION    = %s\n",glGetString(GL_VERSION));
        printf("  GL_EXTENSIONS = %s\n",glGetString(GL_EXTENSIONS));
//        printf("  GL_EXT_texture_compression_s3tc = %i\n",GL_EXT_texture_compression_s3tc);
//        printf("  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT = %i\n",GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);

        glutIdleFunc( glut_idle );
        glutKeyboardFunc( keyboard );
        glutReshapeFunc( reshape );
        glutSpecialFunc( specialkey );
        glutMouseFunc( NULL );

        glutMainLoop();

      }

      printf("\n  Program %s stopping ...\n\n",my_name);
      return 0;
}

/*
 *==============================================================================
 * Process program arguments
 *==============================================================================
 */

int process_args(int argc, char **argv){
int  i ;
char string[256] ;

      if(argc < 2) return 0 ;

      string[0] = '\0';
      for(i=0;i<argc;i++){
        sscanf(argv[i],"%s",string);
//  Turn on plotting
        if(!strncmp(string,"-p",2)){
          l_plot = 1 ;
//  Display help message
        }else if(!strncmp(string,"-h",2) || !strncmp(string,"--h",3)){
          printf(" Program zr help\n") ;
          printf("   Options:\n") ;
          printf("   --help  ::  This help message\n") ;
          printf("   -h      ::  This help message\n") ;
          printf("   -p      ::  Load data and display using OpenGL\n") ;
          printf("           ::  Otherwise just load data\n\n") ;
          exit(0);
        }
      }
/*
 *  Check compiler options
 */
#if (defined _Display_Wagons && defined _Display_Shapes) \
 || (defined _Display_Wagons && defined _Display_Textures) \
 || (defined _Display_Shapes && defined _Display_Textures)
      printf("  Error.  More than one compiler option provided from group:\n") ;
      printf("    -D_Display_Wagons,  -D_Display_Shapes, -D_Display_Textures\n");
      printf("  Program stopping ...\n") ;
      exit(1) ;
#endif
      return 0 ;
}

#include "system.c"
#include "process.c"
#include "data.c"
#include "transform.c"
#include "tsection_db.c"
#include "road_itm_db.c"
#include "road_sct_db.c"
#include "track_setup.c"
#include "road_setup.c"
#include "trk_itm_db.c"
#include "trk_sct_db.c"
#include "dynamic.c"
#include "make_texture_resident.c"
#include "freetype.c"
#include "graphics_init.c"
#include "graphics_idle.c"
#include "graphics.c"
#include "clip.c"
#include "display_info.c"
#include "display.c"
#include "display_shapes.c"
#include "keyboard.c"
#include "shape.c"
#include "shape_d.c"
#include "topog_dlist.c"
#include "topog_cva.c"
#include "topog_dva.c"
#include "shape_dlist.c"
#include "track_dlist.c"
#include "road_dlist.c"
#include "camera.c"
#include "dynamic_dlist.c"
#include "display_dynamic.c"
#include "texture.c"
#include "terrain.c"
#include "tiles.c"
#include "world.c"
#include "load_wagon_file.c"
#include "load_wagon_files.c"
#include "test_sphere.c"
#include "token.c"
#include "token_b.c"
#include "token_tb.c"

#include "train.c"
#include "traveller.c"
#include "traveller2.c"

