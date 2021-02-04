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
 *   The file describes the token format of its contents, including the
 *   format fillowing the tokens "Tracksection ( SectionCurve ( ".
 *
 *   Other tsection.dat files are also found at the top level of the route
 *   directories.  They start with "SIMISA@@@@@@@@@@JINX0T0t______" (note the
 *   'T') and contain Tracksection and SectionCurve tokens, but with a
 *   new format for the tokens following SectionCurve.  In fact the format
 *   is that same as the TRACKSECTION section of the world files - indicating
 *   that the file shoud be read as a world file or a world file include.
 *
 *   The great_zig_zag files are similar, but with an additional tsection.dat
 *   file in the route directory OpenRails.  This is an ascii text file
 *   without an MSTS header.  The first line is blank, the second line:
 *   " include ( "../../../Global/tsection.dat" )"
 *   followed by tokens using the top level format.
 *
 *   For simplicity this routine first reads the top level file, if it exists.
 *   It then reads the OpenRails file, if it exists, ignoring any include
 *   statements.
 *
 * *****************************************************************************
 */


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

  char my_name[] = "init_section_idx" ;

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
FILE    *fp                 ;
MSfile  msfile0             ;
MSfile  *msfile = &msfile0  ;
char    base_name[] = "tsection.dat" ;
char    my_name[]   = "init_tsec_db" ;
char    *token1 = NULL,
        *token2 = NULL,
        *token3 = NULL,
        *string = NULL      ;
TrackSection *track_section = NULL ;
TrackShape   *track_shape   = NULL ;
SectionIdx   *section_idx   = NULL ;
Vector3      *xover         = NULL ;
int          *section       = NULL ;

/*
 *  First find tsection.dat file in the top directory
 */
      for(idir=0;idir<2;idir++){
//  Construct filename
        if(0==idir){
          l = strlen(ORdir) + strlen("Global/") + strlen(base_name) + 1 ;
          string = (char *)malloc(l) ;
          strcpy(string,ORdir) ;
          strcat(string,"Global/") ;
        }else{
          l = strlen(ORroutedir) + strlen("OpenRails/")
                                 + strlen(base_name) + 1 ;
          string = (char *)malloc(l) ;
          strcpy(string,ORroutedir) ;
          strcat(string,"OpenRails/") ;
        }
        strcat(string,base_name) ;
        if(ip)printf("     Full filename = %s\n",string) ;
// Check file exists
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
 *  valgrind --leak-check=full   --show-leak-kinds=all  --track-origins=yes ./zr -p
 *   Cycle through file
 */
        for(;;){
          if(token1 != NULL){free(token1) ; token1 = NULL ;}
          token1 = ctoken(msfile) ;
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
              if(ip)printf(" Enter TrackSections\n") ;

              for(;;){
                if(token2 != NULL){free(token2) ; token2 = NULL ;}
                token2 = ctoken(msfile) ;
                if(is_rbr(token2)) break ;
                SWITCH(token2)
                  CASE ("TrackSection")
                  CASE ("Tracksection")
                    track_section = new_track_section() ;
                    skip_lbr(msfile) ;
                    track_section->index = itoken(msfile) ;
                    for(;;){
                      if(token3 != NULL){free(token3) ; token3 = NULL ;}
                      token3 = ctoken(msfile) ;
                      if(is_rbr(token3)) break ;
                      SWITCH(token3)
                        CASE("SectionSize")
                          skip_lbr(msfile) ;
                          track_section->gauge  = dtoken(msfile)  ;
                          track_section->length = dtoken(msfile) ;
                          skip_rbr(msfile) ;
                          break ;
                        CASE("SectionCurve")
                          skip_lbr(msfile) ;
                          track_section->radius = dtoken(msfile)  ;
                          track_section->angle  = dtoken(msfile) ;
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
                        DEFAULT
                          exit(1) ;
                      END // Switch(token3)
                    }
                    if(token3 != NULL){free(token3) ; token3 = NULL ;}
                    break ;
                  DEFAULT
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
                if(is_rbr(token2)) break ;
                SWITCH(token2)
                  CASE ("TrackShape")
                    track_shape = new_track_shape() ;
                    skip_lbr(msfile) ;
                    track_shape->index = itoken(msfile) ;

                    for(;;){
                      if(token3 != NULL){free(token3) ; token3 = NULL ;}
                      token3 = ctoken(msfile) ;
                      if(is_rbr(token3)) break ;
                      SWITCH(token3)
                        CASE("FileName")
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
                          section_idx->height_y       = dtoken(msfile) ;
                          section_idx->north_z       = dtoken(msfile) ;
                          section_idx->angle       = dtoken(msfile) ;

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
                          exit(1) ;
                      END // Switch(token3)
                    }
                   if(token3 != NULL){free(token3) ; token3 = NULL ;}
                    break ;
                  DEFAULT
                    exit(1) ;
                END // Switch(token2)
              }
              if(token2 != NULL){free(token2) ; token2 = NULL ;}
              break ;
            DEFAULT
              exit(1) ;
          END
        }  // End of main loop
        if(token1 != NULL){free(token1) ; token1 = NULL ;}
        if(token2 != NULL){free(token2) ; token2 = NULL ;}
        if(token3 != NULL){free(token3) ; token3 = NULL ;}
        close_msfile(msfile) ;
        free(string) ;
      }
      return 0 ;
}


