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
C:\ZR>gcc zr.c -DSDL2 -Dkb_dev -Dgrid_lines -Duse_vertex_arrays -DMinGW -I"C:\Program Files\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\x86_64-w64-mingw32\include" -IC:\SDL2\include -Iinclude -Isrc/core -Isrc/dynamics -Isrc/graphics -Isrc/input -Isrc/layout -LC:\SDL2\lib -Wall -lopengl32 -lglu32 -lmingw32 -lSDL2main -lSDL2 -lz -o zr -Wno-unused
 *
 */
//#define SDL2
//#define MinGW
/*
 *
 *==============================================================================
 */
#include <stdio.h>
#include "zr.h"
#include "graphics.h"

#ifdef SDL2
  static void quit_program( int code ) ;  //  Used for SDL Quit
#endif

char  my_file[] = "__FILE__" ;
int   l_pd = 0          ;  // l_pd = 1 to print debug information
int   l_pp = 1          ;  // l_pp = 1 to print after a new position

int   n_sig0 = -1      ;
int   n_sig1 = -1         ;
//int   n_sig0 = 3655      ;  // MECoast route York
//int   n_sig1 = 3655      ;  //
//int   n_sig0 = 1570      ;  // NewForestRoute Bug
//int   n_sig1 = 1570      ;  //
int   i_count = 0       ;  // Used to count number of debug print statements
float d_rotate  =  0.0  ;  // Debugug : default rotation angle (0, 90)
float d_reflect = -1.0  ;  // Default reflection value (-1,1)

//char test_shape[] = "A1tPnt6dLft" ;
//char test_shape[] = "Ashphaltplat20m250r" ;
//char test_shape[] = "BBS-SL" ;
//char test_shape[] = "JP1SigGant4" ;
//char test_shape[] = "policePHIL" ;
//char test_shape[] = "JP2grntent" ;
//char test_shape[] = "acelahhl" ;
//char test_shape[] = "amgantry1" ;     //  USA1 signal
//char test_shape[] = "trm9d46" ;     //  New Forest Switch
//char test_shape[] = "ukfs_tcr_r_250m_7d13" ;     //  New Forest Switch
//char test_shape[] = "a1t50mstrt" ;
//char test_shape[] = "N1tEndPnt3d" ;
//char test_shape[] = "1905-S654" ;
//char test_shape[] = "AU_Ash_20m" ;
//char test_shape[] = "BBS-NSW-RoadBridge5span" ;
//char test_shape[] = "BBS-BlockofShops4" ;
//char test_shape[] = "BBS-DerrickCraneBig" ;
//char test_shape[] = "T_Pole_Tall_zz_1tr" ;
//char test_shape[] = "BBS-BlockofShops4" ;
//char test_shape[] = "AU_GumClump2" ;  //  Used to track processing of shape
//char test_shape[] = "NSW_SemLQJct-M-L" ;  //  Used to track error in prim_state
//char test_shape[] = "a1t100mStrt" ;  //  Used to track error in track orientation
//char test_shape[] = "OESignal01" ;
//char test_shape[] = "BBS-NSWGR-60ft-TT-open2_animate" ;
//char test_shape[] = "footbrwmth" ;   //  Footbridge Weymouth : New Forest Route
//char test_shape[] = "rfdowncastshaft" ;   //                   : New Forest Route
//char test_shape[] = "DR_BR_5MT_44917" ;   //                   : New Forest Route
//char test_shape[] = "ukfs_tlm_17m5_9d46" ;   //                   : New Forest Route
//char test_shape[] = "Dock" ;   //                   : New Forest Route
//char test_shape[] = "lymtownstabldg" ;   //                   : New Forest Route
//char test_shape[]   = "seatLSWR2" ;   //                   : New Forest Route
//char test_shape[]   = "corfestation" ;   //                   : New Forest Route
//char test_shape[]   = "latgantry3sigrh" ;   //                   : New Forest Route
//char test_shape[]   = "srjctbsmsbsindic" ;   //                   : New Forest Route
//char test_shape[]   = "ctn_point_indication" ;   //                   : New Forest Route
//char test_shape[]   = "JP1Signal2" ;   //                   : Japan 1
//char test_shape[]   = "nsw_lq-2doll-bracketl" ;   //           : au_great_zig_zag
//char test_shape[]   = "us2bsignal3" ;   //           : au_great_zig_zag
//char test_shape[]   = "srjct-bs-ms-bs" ;   //           : New Forest
//char test_shape[]   = "lswrhfdshortrail" ;   //           : New Forest
//char test_shape[]   = "rt3aspsig" ;   //           : MECoast
//char test_shape[]   = "rifeathers" ;   //           : MECoast
//char test_shape[]   = "ukfs_tt_42ft" ;   //           : Chiltern
char test_shape[] = " " ;  //  Dummy
GLubyte *c_red, *c_green, *c_blue ;
GLubyte *c_rgba ;
int   c_width, c_height ;

/**
 *   Main entry point for ZR.
 */

int main(int argc, char **argv){

int      width, height ;
int      ip = 0        ;
GLfloat  mat_amb[]  = {light0_ambi, light0_ambi, light0_ambi, 1.0} ;  //  Value in shadow
GLfloat  mat_dif[]  = {light0_diff, light0_diff, light0_diff, 1.0} ;  //  Value lighted
GLfloat  mat_spc[]  = {light0_spec, light0_spec, light0_spec, 1.0} ;  //  Highlights
GLfloat  high_shininess[] = { 100.0 };
GLfloat  mid_shininess[]  = {  50.0 };
GLfloat  low_shininess[]  = {  10.0 };
GLfloat  v4[4] ;
char my_name[] = "zr" ;

      printf("  Program %s starting ...\n",my_name);

#ifdef SDL2
      printf("    Flag SDL2 is set\n") ;
#endif
      process_args(argc, argv) ;

      clock_gettime(CLOCK_MONOTONIC, &run_clock0) ; //  Run clock
      start_seconds = clock()/CLOCKS_PER_SEC ;
      process_defaults() ;                          //  Determine route directory etc.
/*
 *    load basic data - i.e. trains/consists/platforms and sidings
 *      used to initialise the system
 */
      load_splash_screen() ;
/*
 *  Start sound
 */
#ifdef OPENAL
      setup_openal(argc, argv) ;
//      alutInit(&argc, argv) ;   //  Sets up OpenAL as well (I think).
#endif

      init_system() ;                               //  Initialise clocks
      printf("  BB ORdir   = %s\n",ORdir) ;
      printf("  ORroute    = %s\n",ORroute) ;
      printf("  ORroutedir = %s\n",ORroutedir) ;
/*
 *  Here I need to call init data structues
 *   in a separate thread, using the pthread library
 *   NOTE:  I could do with specifying trains/consists/platforms and sidings first.
 *  Then start the splash screen etc
 *   then once the thread has finished
 *  Replace the splash screen by the main program
 */
      init_data_structures() ;
/*
 *  Exit if plotting not required
 */
      if(!l_plot){
        printf("\n  Program %s stopping ...\n\n",my_name);
        close_system() ;
      }
/*
 *  Open window and start main loop
 */
#ifdef SDL2
/*
 *  Use SDL2 Interface
 *
 * Initialises the timer, audio, video, joystick,
 *  haptic, gamecontroller and events subsystems
 */
int  iret ;
      printf("  Call SDL_Init\n") ;
      if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("Error initialising SDL: %s\n", SDL_GetError());
        close_system();
      }
      printf("  SDL successfully initialised!\n");

      SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
      SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
      SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
      SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
      SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, -1 );
//  Using accumulation buffer
//      SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 32 );
//      SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 32 );
//      SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 32 );
//      SDL_GL_SetAttribute( SDL_GL_ACCUM_ALPHA_SIZE, 32 );
/*
 *   Open window
 */
      WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE ;
# ifndef _NoMultiSample
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
# endif
      Window = SDL_CreateWindow("OpenGL Test", 100, 100, main_window_width, main_window_height, WindowFlags);
      assert(Window);

      Context = SDL_GL_CreateContext(Window);
      SDL_SetWindowResizable(Window, SDL_TRUE) ;
      SDL_GL_SetSwapInterval(1) ;

const char title[] = "ZR" ;
      SDL_SetWindowTitle(Window,title) ;
      SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER,&sdl2_double_buffer) ;
      printf("  Double buffer set = %i\n",sdl2_double_buffer) ;
/*
 *  Initialise TTF_Font and size
 */
# ifdef USE_SDLTTF
      TTF_Init() ;
#  if 1
      printf("  Use verdana font\n") ;
      ttf_font_f12 = TTF_OpenFont("/home/djw/.zr/fonts/verdana.ttf",12) ;
      ttf_font_f14 = TTF_OpenFont("/home/djw/.zr/fonts/verdana.ttf",14) ;
      ttf_font_f16 = TTF_OpenFont("/home/djw/.zr/fonts/verdana.ttf",16) ;
      ttf_font_f18 = TTF_OpenFont("/home/djw/.zr/fonts/verdana.ttf",18) ;
#  else
      printf("  Use tahoma font\n") ;
      ttf_font_f12 = TTF_OpenFont("/home/djw/.zr/fonts/tahoma.ttf",12) ;
      ttf_font_f14 = TTF_OpenFont("/home/djw/.zr/fonts/tahoma.ttf",14) ;
      ttf_font_f16 = TTF_OpenFont("/home/djw/.zr/fonts/tahoma.ttf",16) ;
      ttf_font_f18 = TTF_OpenFont("/home/djw/.zr/fonts/tahoma.ttf",18) ;
#  endif
# endif
      printf("  GL_VENDOR     = %s\n",glGetString(GL_VENDOR));
      printf("  GL_RENDERER   = %s\n",glGetString(GL_RENDERER));
      printf("  GL_VERSION    = %s\n",glGetString(GL_VERSION));
      printf("  GL_EXTENSIONS = %s\n",glGetString(GL_EXTENSIONS));
      printf("\n") ;
      printf(" GL EXT texture filter anisotropic = %i\n ",
        GL_EXT_texture_filter_anisotropic) ;
      printf("\n") ;
      if(!GL_EXT_texture_filter_anisotropic){
        printf(" Anisotropic texture filter not available\n");
        printf(" a non-zero LOD bias will be used instead\n");
      }

#ifndef _NoMultiSample
      glEnable(GL_MULTISAMPLE_ARB);
GLint  isbuf, isnum;
      glGetIntegerv(GL_SAMPLE_BUFFERS, &isbuf);
      glGetIntegerv(GL_SAMPLES, &isnum);
      printf("\n  Number of sample buffers is %d\n", isbuf);
      printf("  Number of samples is %d\n", isnum);
#endif
/*
 *   Initialise OpenGL graphics
 *   Needs to be placed after SDL opens a OpenGL style window
 *   Otherwise glGenLists() and glGenTexture() do not work
 */
      printf("  Call graphics_init\n") ;
      call flush();
      graphics_init() ;

      zr_setp4(v4,light0_altde,light0_polar) ;
      glLightfv(GL_LIGHT0,GL_POSITION,v4) ;
/*
 *  Light0 materials are  set by graphics init
 */
      if(0){
        glLightfv(GL_LIGHT0, GL_AMBIENT,  mat_amb) ;
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  mat_dif) ;
        glLightfv(GL_LIGHT0, GL_SPECULAR, mat_spc) ;
      }
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);

/*
 *   Initialise the Viewport and GL_PROJECTION
 */
      glViewport( 0, 0, viewport_width, viewport_height);
      viewport_aspect = (GLfloat)viewport_width/(GLfloat)viewport_height    ;
      viewport_near   = 0.002 ;
      viewport_far    = 3.0  ;
      glMatrixMode(GL_PROJECTION) ;
      glLoadIdentity() ;
      gluPerspective(viewport_fovy,viewport_aspect,viewport_near,viewport_far);
      if(ip)printf("  P %f  %f  %f  %f\n",viewport_fovy,viewport_aspect,viewport_near,viewport_far) ;

// Sound
// load WAV file
#if 0
SDL_AudioSpec wavSpec, wavSpec2;
Uint32 wavLength, wavLength2;
Uint8 *wavBuffer, *wavBuffer2;

//SDL_LoadWAV("bird_kookaburra2.wav", &wavSpec, &wavBuffer, &wavLength);
SDL_LoadWAV("sig_gateo_b.wav", &wavSpec, &wavBuffer, &wavLength);

// open audio device

SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

// play audio

int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
//SDL_PauseAudioDevice(deviceId, 0);
#endif

/*
 *  Enter main loop
 */
      while (1){
        SDL_Event Event;
        while (SDL_PollEvent(&Event)){
          if(ip)printf(" SDL_PollEvent :: Event.type = %i :: %i %i %i\n",Event.type,SDL_QUIT,SDL_KEYDOWN, SDL_WINDOWEVENT) ;
          switch(Event.type){
            case SDL_KEYDOWN:
            case SDL_KEYUP:
//          if (Event.type == SDL_KEYDOWN || ){
              if(ip)PrintKeyInfo( &Event.key );
              if(ip)printf(" Call keyboard_sdl\n") ;
              keyboard_sdl(&Event) ;
              if(ip)printf(" Return from keyboard_sdl\n") ;
              break ;
/*
 *  Window flags are defined in SDL2 "sdl_video.h".
 *  Window events are defined in SDL2 "sdl_video.h".
    SDL_WINDOWEVENT_NONE,           Never used
    SDL_WINDOWEVENT_SHOWN,          Window has been shown
    SDL_WINDOWEVENT_HIDDEN,         Window has been hidden
    SDL_WINDOWEVENT_EXPOSED,        Window has been exposed and should be
                                         redrawn
    SDL_WINDOWEVENT_MOVED,          Window has been moved to data1, data2
    SDL_WINDOWEVENT_RESIZED,        Window has been resized to data1xdata2
    SDL_WINDOWEVENT_SIZE_CHANGED,   The window size has changed, either as
                                         a result of an API call or through the
                                         system or user changing the window size.
    SDL_WINDOWEVENT_MINIMIZED,      Window has been minimized
    SDL_WINDOWEVENT_MAXIMIZED,      Window has been maximized
    SDL_WINDOWEVENT_RESTORED,       Window has been restored to normal size
                                         and position
    SDL_WINDOWEVENT_ENTER,          Window has gained mouse focus
    SDL_WINDOWEVENT_LEAVE,          Window has lost mouse focus
    SDL_WINDOWEVENT_FOCUS_GAINED,   Window has gained keyboard focus
    SDL_WINDOWEVENT_FOCUS_LOST,     Window has lost keyboard focus
    SDL_WINDOWEVENT_CLOSE,          The window manager requests that the window be closed
    SDL_WINDOWEVENT_TAKE_FOCUS,     Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore)
    SDL_WINDOWEVENT_HIT_TEST        Window had a hit test that wasn't SDL_HITTEST_NORMAL.
 */
//          }else if(Event.type == SDL_WINDOWEVENT){
            case SDL_WINDOWEVENT:
              if(ip)printf(" SDL_PollEvent :: Event.window.event = %i\n",Event.window.event) ;
              switch (Event.window.event){
                case SDL_WINDOWEVENT_CLOSE:
                  quit_program(3) ;
                  break ;
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                  reshape2(0,0) ;
                  break ;
              }
          }
        }     //  PollEvent

        display() ;
        SDL_GL_SwapWindow(Window);
      }
#else
/*
 *  The movie code needs the Pixel pack alignment defined.
 *  The code may only work with glut but it should only depend
 *  OpenGL
 */
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
/*
 *  Use GLUT Interface
 */
      glutInit(&argc, argv);
      glutInitDisplayMode(GLUT_DOUBLE
#ifndef _NoMultiSample
                        | GLUT_MULTISAMPLE
#endif
                        | GLUT_RGBA | GLUT_DEPTH );
      glutInitWindowPosition(main_window_x0, main_window_y0);
      glutInitWindowSize(main_window_width, main_window_height);
      main_window = glutCreateWindow(argv[0]);

      graphics_init() ;               //  File graphics.c
#ifdef SHADERS
      shaders_init();
#endif
      glutDisplayFunc(display)    ;
      glutIdleFunc(glut_idle)     ;
      glutKeyboardFunc(keyboard)  ;
      glutKeyboardUpFunc(keyboard_up) ;
      glutReshapeFunc(reshape)    ;
      glutSpecialFunc(specialkey) ;
      glutMouseFunc(mouse)        ;
      glutMotionFunc(motion)      ;

      printf("  GL_VENDOR      = %s\n",glGetString(GL_VENDOR));
      printf("  GL_RENDERER    = %s\n",glGetString(GL_RENDERER));
      printf("  GL_VERSION     = %s\n",glGetString(GL_VERSION));
      printf("  GL_EXTENSIONS  = %s\n",glGetString(GL_EXTENSIONS));
//      printf("  GLU_EXTENSIONS = %s\n",glGetString(GLU_EXTENSIONS));  //  Not supported?
      printf(" GL_EXT_texture_filter_anisotropic = %i\n ",
        GL_EXT_texture_filter_anisotropic) ;
      if(!GL_EXT_texture_filter_anisotropic){
        printf(" Anisotropic texture filter not available\n");
        printf(" a non-zero LOD bias will be used instead\n");
      }

/*
 *  Check for texture compression formats
 */
//        printf("  GL_EXT_texture_compression_s3tc = %i\n",GL_EXT_texture_compression_s3tc);
//        printf("  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT = %i\n",GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);

#ifndef _NoMultiSample
      glutSetOption(GLUT_MULTISAMPLE, 4);     //  Available with freeglut, not glut
      glEnable(GL_MULTISAMPLE_ARB);
      glEnable(GL_MULTISAMPLE);
GLint  isbuf, isnum;
      glGetIntegerv(GL_SAMPLE_BUFFERS, &isbuf);
      glGetIntegerv(GL_SAMPLES, &isnum);
      printf("\n  Number of sample buffers is %d\n", isbuf);
      printf("  Number of samples is %d\n", isnum);
#endif
GLfloat ansio, ansio_ext    ;
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY,&ansio) ;
      printf("  Maximum anistrophy is %f\n", ansio);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 4.0);

      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&ansio_ext) ;
      printf("  Maximum anistrophy_ext is %f\n", ansio_ext);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0);

/*
 *  Pixel allignment used for movie output
 */
      glPixelStorei(GL_PACK_ALIGNMENT, 1);

/*
 *   GLUT wondow fram buffer capabilities cannot be changed
 *   https://www.opengl.org/resources/libraries/glut/spec3/node107.html#SECTION000163200000000000000
 */
      printf("GLUT_WINDOW_NUM_SAMPLES = %i\n",GLUT_WINDOW_NUM_SAMPLES) ;
      glutMainLoop();
#endif
      printf("\n  Program %s stopping ...\n\n",my_name);
      return 0;
}

/*
 *==============================================================================
 * Process program arguments
 *
 *   Option -t should be the shortened name of one of the top level directories
 *             defined in the configuraiton file '.zr/config'.
 *          -r should be the name of one of the route directories contained
 *             within the corresponding first level 'Routes' directory.
 *
 *   If not defined here, and there is more than one alternative specified
 *   in the configuraiton file '.zr/config', then they are requested later.
 *
 *==============================================================================
 */

int process_args(int argc, char **argv){
int  ip = 1 ;
int  i      ;
char string[256] ;

      if(argc < 2) return 0 ;

      string[0] = '\0';
      for(i=1;i<argc;i++){
        sscanf(argv[i],"%s",string);
//  Turn on plotting
        if(!strncmp(string,"-p",2)){
          l_plot = 1 ;
        }else if(!strncmp(string,"-t",2)){
          ORdname = strdup(argv[++i]) ;
        }else if(!strncmp(string,"-r",2)){
          ORroute = strdup(argv[++i]) ;
//  Display help message
        }else if(!strncmp(string,"-h",2) || !strncmp(string,"--h",3)){
          printf(" Program zr help\n") ;
          printf("   Options:\n") ;
          printf("   --help   ::  This help message\n") ;
          printf("   -h       ::  This help message\n") ;
          printf("   -t dname ::  Top level directory\n") ;
          printf("   -r rname ::  Name of route\n") ;
          printf("   -p       ::  Load data and display using OpenGL\n") ;
          printf("            ::  Otherwise just load data\n\n") ;
          exit(0);
        }else{
          printf("    Option not recognised.  Argument = %s\n",string) ;
          printf("    Program stopping ...\n") ;
          exit(0) ;
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
/*
 *  Report
 */
      if(ip){
        if(ORdname)printf("  process_args :: Top directory name = %s\n",ORdname) ;
        if(ORroute)printf("  process_args :: Name of route      = %s\n",ORroute) ;
      }
      return 0 ;
}

#ifdef SDL2
static void quit_program( int code )
{
int  ip = 0 ;

    if(ip)printf("  Enter quit_program : %i\n", code) ;
// clean up Sound
//SDL_CloseAudioDevice(deviceId);
//SDL_FreeWAV(wavBuffer);
    SDL_Quit( );
    if(ip)printf("  Enter quit_program - part 2\n") ;
    exit( code );
}
#endif

#include "fcaseopen.c"
#include "system.c"
#if defined MinGW
  #include "system_alt.c"
#endif
#include "btree.c"
#include "getline.c"
#include "process.c"
#include "glfont.c"
#include "filesearch.c"

#include "data.c"
#include "enum.c"
#include "transform.c"
#include "tsection_db.c"
#include "road_itm_db.c"
#include "road_sct_db.c"
#include "track_setup.c"
#include "road_setup.c"
#include "trk_itm_db.c"
#include "trk_sct_db.c"
#include "track_init.c"
#include "setup_level_crossing.c"
#include "dynamic.c"
#include "make_texture_resident.c"
#include "graphics_init.c"
#include "graphics.c"
#include "load_splash_screen.c"
#include "clip.c"
#include "display_info.c"
#include "display.c"
#include "display_shapes.c"
#include "display_wshape.c"
#include "display_water.c"
#include "shape.c"
#include "shape_d.c"
#include "signal_file.c"
#include "setup_signals.c"
#include "signal.c"
#include "switch.c"
#include "topog_dlist.c"
#include "topog_cva.c"
#include "topog_dva.c"
#include "shape_dlist.c"
#include "track_dlist.c"
#include "road_dlist.c"
#include "camera.c"
#include "dynamic_dlist.c"
#include "display_dynamic.c"
#include "texture_alt.c"
#include "texture.c"
#include "terrain.c"
#include "tiles.c"
#include "update_level_crossings.c"
#include "update_signals.c"
#include "world.c"
#include "load_wagon_file.c"
#include "load_wagon_files.c"
#include "load_turntable.c"
#include "load_consist_files.c"
#include "setup_turntables.c"
#include "update_turntables.c"
#include "setup_transfers.c"
#include "find_height.c"
#include "setup_forests.c"
#include "update_transfers.c"
#include "update_trains.c"
#include "signal_scripts.c"
#include "test_sphere.c"
#include "token.c"
#include "token_b.c"
#include "token_tb.c"
#include "train_operations_window.c"

#include "train.c"
#include "traveller.c"
#include "traveller2.c"
#include "traveller3.c"
#include "train_events.c"

#ifdef SDL2
#  include "keyboard_SDL.c"
#else
#  include "graphics_idle.c"
#  include "keyboard.c"
#  include "mouse.c"
#endif

#ifdef USE_FREETYPE
#  include "freetype.c"
#endif

#ifdef OPENAL
#  include "setup_openal.c"
#  include "setup_sounds.c"
#  include "load_sound_files.c"
#  include "load_sound_files2.c"
#  include "update_sounds.c"
#  include "setup_train_sounds.c"
#  include "update_train_sounds.c"
#  include "load_wave_file.c"
#endif

#if defined GLUT || defined USE_ZRGLUT
#  include "zr_glut.c"
#endif

//#if defined FFMPEG || defined LIBPNG || defined PPM
#  include "movie.c"
//#endif


#ifdef SHADERS
#include "shaders.c"
#endif
