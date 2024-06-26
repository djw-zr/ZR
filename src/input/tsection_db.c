/*
 *******************************************************************************
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File tsection_db.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   This file contains routines for reading the tsection.dat files
 *   and populating the nodes of the tsec_sections and tsec_shapes databases.
 *
 * *****************************************************************************
 *
 *   Notes on tsection.dat
 *
 *   MSTS has a tsection.dat file in the top level Global directory.  This
 *   starts with the line "SIMISA@@@@@@@@@@JINX0F0t______"
 *   which is also the start line of 'forest.dat' in the Route directories.
 *
 *   Global tsection.dat
 *
 *   The file describes the token format of its contents, including the
 *   format fillowing the tokens: Tracksection, TrackSection, SectionSize
 *   and SectionCurve.
 *
 *   The file also contains a list of TrackShapes.  Each of these includes
 *   the file name of a track shape (one of the displayed shapes) and the
 *   number of TrackSections used to represent the shape.  This the NumPaths
 *   is usually 1 but may be 2 at a junction or crossing.  There is then
 *   a series of SectionIdx tokens in order leftmost to rightmost.  This
 *   gives the number of sections used for this path, the offset of the start
 *   start section from the shape origin, the angle of section start from the
 *   shape origin and then a list of TrackShapes.
 *
 *   Route tsection.dat
 *
 *   In the MSTS directory structure other tsection.dat files are also found
 *   at the top level of the route directories.  They start with
 *    "SIMISA@@@@@@@@@@JINX0T0t______" (note the 'T').
 *
 *   They contain Tracksection and SectionCurve tokens, but with a new format
 *   for the tokens following SectionCurve.  In fact the format
 *   is that same as the TRACKSECTION section of the world files - indicating
 *   that the file might be read as a world file or a world file include.
 *
 *   The files also include a top level SectionIdx token, followed by a set
 *   of TrackPath Tokens.  These have the form
 *      TrackPath (263 1 376)
 *      TrackPath (267 2 384 386)
 *   where the last two numbers refer to TrackSections at the start of the file.
 *   What I do not understand is what the first number refers to.
 *
 *   The great_zig_zag files are similar, but with an additional tsection.dat
 *   file in the route directory OpenRails.  This is an ascii text file
 *   without an MSTS header.  The first line is blank, the second line:
 *   " include ( "../../../Global/tsection.dat" )"
 *   followed by tokens using the top level format.
 *
 *   For simplicity this routine first reads the global/tsection.dat file, if
 *   it exists.   It then reads the route/OpenRails/tsection.dat file, if it
 *   exists, ignoring any include statements.
 *
 *   It does not read the route/tsection.dat.  So far this has not caused a
 *   problem.
 *
 *   I wonder whether the SectionIdx data is really needed.  As I remember it,
 *   ZR gets all the information it needs to move the trains along each section
 *   of track from the track database file.
 *
 * *****************************************************************************
 */

int read_route_tsection_dat(MSfile *msfile) ;

TrackSection *new_track_section(){
TrackSection *track_section ;
      track_section = (TrackSection *)malloc(sizeof(TrackSection)) ;
      track_section->next       = NULL ;
      track_section->index      = -1   ;
      track_section->gauge      = 0.0  ;
      track_section->length     = 0.0  ;
      track_section->radius     = 0.0  ;
      track_section->angle      = 0.0  ;
      if(track_section_beg == NULL){
        track_section_beg       = track_section ;
      }else{
        track_section_end->next = track_section ;
      }
      track_section_end = track_section ;
      return track_section ;
}

int   init_section_idx(SectionIdx *section_idx){

  int  ip = 0 ;
  char my_name[] = "init_section_idx" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      if(section_idx == NULL){
        printf(" ERROR : Routine %s.  Parameter section_idx == NULL\n",my_name);
        exit(1) ;
      }

      section_idx->num_sections = 0   ;
      section_idx->east_x       = 0.0 ;
      section_idx->height_y     = 0.0 ;
      section_idx->north_z      = 0.0 ;
      section_idx->east_x       = 0.0 ;
      section_idx->section      = NULL ;
      return 0 ;
}

TrackShape *new_track_shape(){
TrackShape *track_shape ;
      track_shape = (TrackShape *)malloc(sizeof(TrackShape)) ;
      track_shape->next          = NULL ;
      track_shape->filename      = NULL ;
      track_shape->num_paths     = 0    ;
      track_shape->main_route    = 0    ;
      track_shape->clearance_dist= 0.0  ;
      track_shape->section_idx   = NULL ;
      track_shape->tunnel_shape  = 0    ;
      track_shape->road_shape    = 0    ;
      track_shape->manual_junction_shape = 0 ;
      track_shape->crossover_shape = 0  ;

      if(track_shape_beg == NULL){
        track_shape_beg       = track_shape ;
      }else{
        track_shape_end->next = track_shape ;
      }
      track_shape_end = track_shape ;
      return track_shape ;
}

/*
 *
 *  Routine to initialise the two tsection databases
 *
 */

int  init_tsec_db(){

int     ip = 0              ;     // Debug printing
int     idir, i, l, n, iret ;
int     ts_error = 0        ;     // TrackSection/TrackShape error
FILE    *fp                 ;
MSfile  msfile0             ;
MSfile  *msfile = &msfile0  ;
char    base_name[] = "tsection.dat" ;
char    my_name[]   = "init_tsec_db" ;
char    *token1 = NULL,
        *token2 = NULL,
        *token3 = NULL,
        *string = NULL      ;
char    *global = "/Global/" ;
char    *open_r = "/OpenRails/" ;
TrackSection *track_section = NULL ;
TrackShape   *track_shape   = NULL ;
SectionIdx   *section_idx   = NULL ;
Vector3      *xover         = NULL ;
int          *section       = NULL ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
 *  The file tsection.data should be in the top Global
 *  or GLOBAL directory.  There may be extra definitions in
 *  directory Openrails within the Route directory
 */
      for(idir=0;idir<3;idir++){
        printf("  idir = %i\n",idir) ;
        printf("  ORroutedir = %s\n",ORroutedir) ;
        printf("  ORdir      = %s\n",ORdir) ;
//  Construct filename
        if(0==idir){
          l = strlen(ORdir) + strlen(global) + strlen(base_name) + 1 ;
          if(ip)printf(" l = %i, %i\n",l,idir);
          string = (char *)malloc(l) ;
//          if(ip)printf(" string = %s\n",string);
          strcpy(string,ORdir) ;
          if(ip)printf(" string = %s\n",string);
          strcat(string,global) ;
        }else if(1==idir){
          l = strlen(ORroutedir) + strlen(open_r)
                                 + strlen(base_name) + 1 ;
          if(ip)printf(" l = %i, %i\n",l,idir);
          string = (char *)malloc(l) ;
          strcpy(string,ORroutedir) ;
          if(ip)printf(" string = %s\n",string);
          strcat(string,open_r) ;
        }else{
          l = strlen(ORroutedir) + strlen(base_name) + 2 ;
          if(ip)printf(" l = %i, %i\n",l,idir);
          string = (char *)malloc(l) ;
          strcpy(string,ORroutedir) ;
          strcat(string,"/") ;
          if(ip)printf(" string = %s\n",string);
        }
        if(ip)printf(" string = %s\n",string);
        strcat(string,base_name) ;
        if(ip)printf("     Full filename = %s\n",string) ;
// Check file exists
        iret = zr_find_msfile2(&string) ;
        if(iret){
          if(idir == 0){
            printf("    Routine %s.  Top level 'tsection.dat' file not found\n",
                    my_name) ;
            printf("    Filename = %s\n",string)  ;
            continue ;
          }
          printf("    Routine %s.  'tsection.dat' file not found\n",
                    my_name) ;
          printf("    Filename = %s\n",string)  ;
          continue ;
        }
        if(ip)printf("  Routine %s.  Opening file %s\n",my_name,string) ;
        fp = gopen(string,"r") ;
        if(idir == 0 && fp == NULL){
          printf("  Unable to find tsection.dat file\n") ;
          printf("     Full filename = %s\n",string) ;
          printf(" Program stopping ...\n") ;
          exit(1) ;
        }
        gclose(fp) ;
/*
 *  Open as a msfile tagged file
 *  This may not have an MSTS header, so use flag '-1'/
 */
        iret = open_msfile(string, msfile, -1, 0) ;
        if(iret){
          printf(" Program stopping ...\n") ;
          exit(1) ;
        }
        if(ip)printf(" File found: %s\n", string) ;
/*
 *  Call a separate routine to read the tsection.dat file in the
 *  route directory
 */
        if(2==idir){
          if(ip)printf("  Call routine read_route_tsection_dat\n") ;
          read_route_tsection_dat(msfile) ;
          close_msfile(msfile) ;
          free(string) ;
          continue ;
        }
/*
 *  valgrind --leak-check=full   --show-leak-kinds=all  --track-origins=yes ./zr -p
 *   Cycle through file
 */
        for(;;){
          if(token1 != NULL){free(token1) ; token1 = NULL ;}
          token1 = ctoken(msfile) ;
          if(ip)printf("  Token 1 = %s\n",token1) ;
          if(is_rbr(token1)) break ;
          if(!strcmp(token1,eof_mark)) break ;

          skip_lbr(msfile) ;
          SWITCH(token1)
            CASE ("include")
              new_tmp_token(msfile) ;
              skip_rbr(msfile) ;
//                token2 = ctoken(msfile) ;
//                printf(" Token 2 %s\n",token2) ;  free(token2) ; token2 = NULL;
//                token2 = ctoken(msfile) ;
//                printf(" Token 3 %s\n",token2) ;  free(token2) ; token2 = NULL;
              break ;
            CASE ("TrackSections")
              n = itoken(msfile) ;     //  maximum index used in this file
              if(ip)printf(" Routine %s.  Enter TrackSections\n",my_name) ;

              for(;;){
                if(token2 != NULL){free(token2) ; token2 = NULL ;}
                token2 = ctoken(msfile) ;
                if(ip)printf("  Token 2 = %s\n",token2) ;
                if(is_rbr(token2)) break ;
                SWITCH(token2)
                  CASE ("TrackSection")
                  CASE ("Tracksection")
                    track_section = new_track_section() ;
                    skip_lbr(msfile) ;
                    track_section->index = itoken(msfile) ;
                    if(ip)printf("  Track Section : %i\n",track_section->index) ;
                    for(;;){
                      if(token3 != NULL){free(token3) ; token3 = NULL ;}
                      token3 = ctoken(msfile) ;
                      if(ip)printf("  Token 3 = %s\n",token3) ;
                      if(is_rbr(token3)) break ;
                      SWITCH(token3)
                        CASE("SectionSize")
                          skip_lbr(msfile) ;
                          track_section->gauge  = dtoken(msfile) ;
                          track_section->length = dtoken(msfile) ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("SectionCurve")
                          skip_lbr(msfile) ;
                          track_section->radius = dtoken(msfile) ;
// Store angle in radians
                          track_section->angle  = radian*dtoken(msfile) ;
                          track_section->length =
                            fabs(track_section->radius*track_section->angle) ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("SectionSkew")
                          skip_lbr(msfile) ;
                          track_section->skew = dtoken(msfile)  ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("WaterScoop")
                          skip_lbr(msfile) ;
                          track_section->water_scoop  = 1  ;
                          skip_rbr(msfile) ;
                          break ;
/*
 *  Some versions of the tsection.dat file have TrackSection which are
 *  be mixtures of TrackSection and TrackShape sub-items.  In this case
 *  the TrackShape variables are just ignored.
 */
                        CASE("FileName")
                        CASE("Filename")
                        CASE("NumPaths")
                        CASE("SectionIdx")
                          skip_lbr(msfile) ;
                          skippast_rbr(msfile) ;
                          break ;
                        DEFAULT
                          printf("  ERROR  Routine %s, \n",my_name) ;
                          printf("         Level 3 processing TrackSection\n") ;
                          printf("         Token not recognised.  Token = %s\n",token3);
                          printf("         Input file = %s\n",string) ;
                          printf("         Index      = %i\n",track_shape->index) ;
                          exit(1) ;
                      END // Switch(token3)
                    }
                    if(token3 != NULL){free(token3) ; token3 = NULL ;}
                    break ;
                  DEFAULT
                    printf("  ERROR  Routine %s, \n",my_name) ;
                    printf("         Level 2 processing TackSections\n") ;
                    printf("         Token not recognised.  Token = %s\n",token2);
                    printf("         Input file = %s\n",string) ;
                    exit(1) ;
                END // Switch(token2)
              }
              if(token2 != NULL){free(token2) ; token2 = NULL ;}
              break ;
            CASE("TrackShapes")
              n = itoken(msfile) ;      //  maximum index used in this file

              for(;;){
                if(token2 != NULL){free(token2) ; token2 = NULL ;}
                token2 = ctoken(msfile) ;
                if(ip)printf("  Token 2 = %s\n",token2) ;
                if(is_rbr(token2)) break ;
                SWITCH(token2)
                  CASE ("TrackShape")
                    track_shape = new_track_shape() ;
                    skip_lbr(msfile) ;
                    track_shape->index = itoken(msfile) ;

                    for(;;){
                      if(token3 != NULL){free(token3) ; token3 = NULL ;}
                      token3 = ctoken(msfile) ;
                      if(ip)printf("  Token 3 = %s\n",token3) ;
                      if(is_rbr(token3)) break ;
                      SWITCH(token3)
                        CASE("FileName")
                        CASE("Filename")
                          skip_lbr(msfile) ;
                          track_shape->filename  = ctoken(msfile)  ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("MainRoute")
                          skip_lbr(msfile) ;
                          track_shape->main_route  = itoken(msfile)  ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("ClearanceDist")
                          skip_lbr(msfile) ;
                          track_shape->clearance_dist  = dtoken(msfile)  ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("TunnelShape")
                          skip_lbr(msfile) ;
                          track_shape->tunnel_shape  = 1  ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("RoadShape")
                          skip_lbr(msfile) ;
                          track_shape->road_shape  = 1  ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("ManualJunctionShape")
                          skip_lbr(msfile) ;
                          track_shape->manual_junction_shape  = 1  ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("CrossoverShape")
                          skip_lbr(msfile) ;
                          track_shape->crossover_shape  = 1  ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("NumPaths")
                          skip_lbr(msfile) ;
                          n                        = itoken(msfile) ;
                          track_shape->num_paths   = n ;
                          section_idx = (SectionIdx *)malloc(n*sizeof(SectionIdx)) ;
                          track_shape->section_idx = section_idx ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("SectionIdx")
                          skip_lbr(msfile) ;
                          init_section_idx(section_idx) ;

                          n = itoken(msfile) ;
                          section = (int *)malloc(n*sizeof(int)) ;
                          section_idx->num_sections = n ;
                          section_idx->section      = section ;

                          section_idx->east_x       = dtoken(msfile) ;
                          section_idx->height_y     = dtoken(msfile) ;
                          section_idx->north_z      = dtoken(msfile) ;
                          section_idx->angle        = dtoken(msfile) ;

                          for(i=0;i<n;i++){
                            section[i] = itoken(msfile) ;
                          }
                          skip_rbr(msfile) ;
                          section_idx++ ;
                          break ;
                        CASE("XoverPts")
                          skip_lbr(msfile) ;
                          n = itoken(msfile) ;
                          xover = (Vector3 *)malloc(n*sizeof(Vector3)) ;
                          track_shape->num_xover = n     ;
                          track_shape->xover     = xover ;
                          for(i=0;i<n;i++){
                            new_tmp_token(msfile) ;  // XoverPt
                            skip_lbr(msfile) ;
                            xover[i].X = dtoken(msfile) ;
                            xover[i].Y = dtoken(msfile) ;
                            xover[i].Z = dtoken(msfile) ;
                            skip_rbr(msfile) ;
                          }
                          skip_rbr(msfile) ;
                          break ;
                        DEFAULT
                          printf("  ERROR  Routine %s, \n",my_name) ;
                          printf("         Level 3 processing TrackShape\n") ;
                          printf("         Token not recognised.  Token = %s\n",token3);
                          printf("         Input file = %s\n",string) ;
                          printf("         Index      = %i\n",track_shape->index) ;
                          exit(1) ;
                      END // Switch(token3)
                    }
                   if(token3 != NULL){free(token3) ; token3 = NULL ;}
                    break ;
                  DEFAULT
                    printf("  ERROR  Routine %s, \n",my_name) ;
                    printf("         Level 2 processing TackShape2\n") ;
                    printf("         Token not recognised.  Token = %s\n",token2);
                    printf("         Input file = %s\n",string) ;
                    exit(1) ;
                END // Switch(token2)
              }
              if(token2 != NULL){free(token2) ; token2 = NULL ;}
              break ;
            DEFAULT
              printf("  ERROR  Routine %s, \n",my_name) ;
              printf("         Level 1 Top level\n") ;
              printf("         Token not recognised.  Token = %s\n",token1);
              printf("         Input file = %s\n",string) ;
              exit(1) ;
          END
        }  // End of main loop
        if(token1 != NULL){free(token1) ; token1 = NULL ;}
        if(token2 != NULL){free(token2) ; token2 = NULL ;}
        if(token3 != NULL){free(token3) ; token3 = NULL ;}
        close_msfile(msfile) ;
        free(string) ;
        printf("  Routine %s.  End of main loop. idir = %i\n",my_name,idir) ;
      }
      return 0 ;
}

int read_route_tsection_dat(MSfile *msfile){

int     ip = 0              ;     // Debug printing
int     idir, i, l, n, iret ;
char    *token1 = NULL,
        *token2 = NULL,
        *token3 = NULL,
        *string = NULL      ;
TrackSection *track_section = NULL ;
TrackShape   *track_shape   = NULL ;
SectionIdx   *section_idx   = NULL ;
Vector3      *xover         = NULL ;
int          *section       = NULL ;
char    my_name[]   = "read_route_tsection_dat" ;

      if(ip)printf(" Enter routine %s\n",my_name) ;
/*
 *  valgrind --leak-check=full   --show-leak-kinds=all  --track-origins=yes ./zr -p
 *   Cycle through file
 */
      for(;;){
        if(token1 != NULL){free(token1) ; token1 = NULL ;}
        token1 = ctoken(msfile) ;
        if(ip)printf("  Token 1 = %s\n",token1) ;
        if(is_rbr(token1)) break ;
        if(!strcmp(token1,eof_mark)) break ;

        skip_lbr(msfile) ;
        SWITCH(token1)
          CASE ("TrackSections")
            if(ip)printf(" Routine %s. Enter TrackSections\n",my_name) ;
            n = itoken(msfile) ;     //  maximum index used in this file
            for(;;){
              if(token2 != NULL){free(token2) ; token2 = NULL ;}
              token2 = ctoken(msfile) ;
              if(ip)printf("  Token 2 = %s\n",token2) ;
              if(is_rbr(token2)) break ;
              SWITCH(token2)
                CASE ("TrackSection")
                CASE ("Tracksection")
                  track_section = new_track_section() ;
                  skip_lbr(msfile) ;
/*
 * Read section curve
 */
                  if(token3 != NULL){free(token3) ; token3 = NULL ;}
                  token3 = ctoken(msfile) ;
                  if(ip)printf("  Token 3 = %s\n",token3) ;
                  if(strcmp(token3,"SectionCurve")){
                    printf(" Routine %s error\n",my_name) ;
                    printf(" Token 'SectionCurve' missing in file "
                            " route_directory/tsection.dat\n") ;
                    printf("  Program stopping ...\n") ;
                    exit(0) ;
                  }
                  skip_lbr(msfile) ;
                  i = itoken(msfile) ;
                  skip_rbr(msfile) ;
                  track_section->index = itoken(msfile) ;
                  if(0==i){
                    track_section->length = dtoken(msfile) ;
                    dtoken(msfile) ;
                  }else{
                    track_section->angle  = dtoken(msfile) ;
                    track_section->radius = dtoken(msfile) ;
                  }
                  skip_rbr(msfile) ;
                  break ;
                DEFAULT
                    printf("  Routine %s error\n",my_name) ;
                    printf("    Token 2 not recognised\n") ;
                    printf("    Token 2 is %s\n",token2) ;
                    printf("  Program stopping ...\n") ;
                    exit(0) ;
              END
            }
            break ;
          CASE ("SectionIdx")
            if(ip)printf(" Routine %s.  Enter SectionIdx\n",my_name) ;
            n = itoken(msfile) ;     //  maximum index used in this file
            for(;;){
              if(token2 != NULL){free(token2) ; token2 = NULL ;}
              token2 = ctoken(msfile) ;
              if(ip)printf("  Token 2 = %s\n",token2) ;
              if(is_rbr(token2)) break ;
              SWITCH(token2)
                CASE ("TrackPath")
                  skip_lbr(msfile) ;
                  i = itoken(msfile) ;
                  n = itoken(msfile) ;
                  for(i=0;i<n;i++)itoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                DEFAULT
                    printf("  Routine %s error\n",my_name) ;
                    printf("    Token 2 not recognised\n") ;
                    printf("    Token 2 is %s\n",token2) ;
                    printf("  Program stopping ...\n") ;
                    exit(0) ;
              END
            }
            break ;
          DEFAULT
            printf("  Routine %s error\n",my_name) ;
            printf("    Token 1 not recognised\n") ;
            printf("    Token 1 is %s\n",token1) ;
            printf("  Program stopping ...\n") ;
            exit(0) ;
        END
      }
      return 0;
}

int print_tsec_section(int section_index){

int          found = 0 ;
TrackSection *t ;

      for(t = track_section_beg; t!=NULL;t=t->next){
        if(section_index != (int)t->index)continue ;
          found = 1 ;
          printf("    TrkSection:  gauge  = %f\n",t->gauge)  ;
          printf("    TrkSection:  length = %f\n",t->length) ;
          printf("    TrkSection:  radius = %f\n",t->radius) ;
          printf("    TrkSection:  angle  = %f\n",t->angle)  ;
          printf("    TrkSection:  skew   = %f %i\n",t->skew,t->water_scoop) ;
      }
      if(0 == found){
        printf("    TrkSection:  Shape index %i, not found.\n",section_index);
      }
      return 0 ;
}

int print_tsec_shape(int shape_index){

uint          i, found = 0 ;
TrackShape *t ;
SectionIdx *s ;

      for(t = track_shape_beg; t!=NULL;t=t->next){
        if(shape_index != t->index)continue ;
        found = 1;
        printf("    TrkShape:    num_paths  = %i\n",t->num_paths)  ;
        printf("    TrkShape:    filename   = %s\n",t->filename) ;
        printf("    TrkShape:    main_route = %i\n",t->main_route) ;
        printf("    TrkShape:    clearance  = %f\n",t->clearance_dist)  ;
        printf("    TrkShape:    shapes     = %i %i %i %i\n",t->tunnel_shape,t->road_shape,
                                              t->manual_junction_shape, t->crossover_shape) ;
        printf("    TrkShape:    num_xover  = %i\n",t->num_xover) ;
        s = t->section_idx ;
        printf("      SectionIdx : num_sections = %i\n",s->num_sections) ;
        printf("                   position, x, y, z, ang = %f %f %f %f\n",
                                                          s->east_x,s->north_z,s->east_x,s->angle);
        if(s->num_sections>0){
          printf("                   Sections:  ");
          for(i=0;i<s->num_sections;i++){
            printf("  %i",s->section[i]) ;
          }
          printf("\n") ;
        }
      }
      if(0 == found){
        printf("    TrkShape:    Shape index %i, not found.\n",shape_index);
      }
      return 0 ;
}

