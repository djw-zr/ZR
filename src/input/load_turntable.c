/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  load_turntable.c
 *   Copyright 2023  David J. Webb
 *
 *   File contains one or more routines for reading the turntable file.
 *
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

int init_turntable(TurnTable *tt) ;

/*
 *  If it exists, the OpenRail turntable file "turntables.dat" is placed in
 *  directory <route/OpenRails/ where <route> is the current route directory.
 *  The file should start with a blank line followed by a line containing
 *  one integer giving the number of turntables.  The file then follows the
 *  standard Kuju/MSTS UTF-16 file format.
 */

int read_turntable_file(void){

  int         n, iret ;
  int         ip = 0 ;
  struct stat sb ;
  MSfile      msfile0   ;
  MSfile      *msfile   ;
  TurnTable   *tt ;
  char        *or_dir_name ;
  char        *tt_name ;
  char        *token1 = NULL,
              *token2 = NULL,
              *string = NULL,
              string7[7] ;
  char *open_r  = "/OpenRails"          ;
  char *t_table = "/turntables.dat"     ;
  char *my_name = "read_turntable_file" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      n_turntables = 0 ;
      turntablelist_beg = NULL ;
      turntablelist_end = NULL ;
      msfile = &msfile0 ;

      n = strlen(ORroutedir) + strlen(open_r) + 1 ;
      or_dir_name = (char *)malloc(n*sizeof(char));
      strcpy(or_dir_name,ORroutedir) ;
      strcat(or_dir_name,open_r) ;

      iret = stat(or_dir_name,&sb) ;
      if(iret){
        if(ip){
          printf("   Routine %s.  OpenRails directory not found \n",my_name) ;
          printf("     Full directory name should be %s\n",or_dir_name) ;
        }
        return 0 ;
      }

      n = strlen(or_dir_name) + strlen(t_table) + 1 ;
      tt_name = (char *)malloc(n) ;
      strcpy(tt_name,or_dir_name) ;
      strcat(tt_name,t_table)     ;

      iret = stat(tt_name,&sb) ;
      if(iret){
        if(ip){
          printf("   Routine %s.  Turntable file not found \n",my_name) ;
          printf("     Full file name should be %s\n",tt_name) ;
        }
        return 0 ;
      }
/*
 *  Open MSTS style text data file without MSTS header (parameter 3 => -1)
 */
      iret = open_msfile(tt_name, msfile, -1, 0) ;
      if(iret){
        printf("\n   ERROR in routine %s: \n",my_name);
        printf("     Routine open_msfile failed to open file\n");
        printf("     File = %s\n",tt_name) ;
        exit(1) ;
      }

      if(ip)printf("    Routine %s.  MSTS file opened\n",my_name) ;

      n_turntables = itoken(msfile) ;
      if(ip)printf("     n_turntables = %i\n",n_turntables) ;
      for(;;){
        if(token1)free(token1) ;
        token1 = ctoken(msfile) ;
        if(!strcmp(token1,eof_mark)) break ;
        if(ip)printf("  AA  token1 = %s \n",token1) ;
        SWITCH(token1)
/*
 *  Level 1 tokens
 */
          CASE("Comment")
          CASE("comment")
            skip_lbr(msfile) ;
            skippast_rbr(msfile) ;
            break ;
          CASE("Turntable")
            skip_lbr(msfile) ;
            tt = (TurnTable *)malloc(sizeof(TurnTable)) ;
            init_turntable(tt) ;
            if(turntablelist_beg== NULL)
              turntablelist_beg = tt ;
            else{
              turntablelist_end->next = tt ;
            }
            turntablelist_end = tt ;
            tt->next = NULL ;
/*
 *  Read turntable data
 */
            for(;;){
              if(token2)free(token2) ;
              token2 = ctoken(msfile) ;
              if(is_rbr(token2)) break ;
              if(ip)printf("    BB  token2 = %s \n",token2) ;
/*
 *  Level 2 tokens
 */
              SWITCH(token2)
                CASE("Comment")
                CASE("comment")
                  skip_lbr(msfile) ;
                  skippast_rbr(msfile) ;
                  break ;
                CASE("WFile")            //  Name within "quotes"!
                  skip_lbr(msfile) ;
                  string = new_tmp_token(msfile) ;  // Temporary string
                  strncpy(string7,&(string[3]),6);
                  string7[6] = '\0' ;
                  tt->tile_x = atoi(string7) ;
                  if(string[1] == '-')tt->tile_x = -tt->tile_x ;
                  strncpy(string7,&(string[10]),6);
                  string7[6] = '\0' ;
                  tt->tile_y = atoi(string7) ;
                  if(string[8] == '-')tt->tile_y = -tt->tile_y ;
                  if(ip){
                    printf(" +++++++++++++++++++++++++++++++++++++++\n") ;
                    printf("  Tile_X = %i\n",tt->tile_x);
                    printf("  Tile_Z = %i\n",tt->tile_y);
                    printf(" +++++++++++++++++++++++++++++++++++++++\n") ;
                  }
                  tt->trk_vector0.tile_east_x  = tt->tile_x ;
                  tt->trk_vector0.tile_north_z = tt->tile_y ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("UiD")
                  skip_lbr(msfile) ;
                  tt->w_uid = itoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("XOffset")
                  skip_lbr(msfile) ;
                  tt->x_offset = dtoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("YOffset")
                  skip_lbr(msfile) ;
                  tt->y_offset = dtoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("ZOffset")
                  skip_lbr(msfile) ;
                  tt->z_offset = dtoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("TrackShapeIndex")
                  skip_lbr(msfile) ;
                  tt->trackshapeindex = dtoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("Animation")
                  skip_lbr(msfile) ;
                  tt->animation = ctoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("Diameter")
                  skip_lbr(msfile) ;
                  tt->diameter = dtoken(msfile) ;
                  tt->trk_vector0.length = tt->diameter ;
                  skip_rbr(msfile) ;
                  break ;
                DEFAULT
                  printf("    Routine %s.  Unrecognised token at level 2\n",
                                                                   my_name) ;
                  printf("      Token2 = %s\n",token2) ;
                  break ;
              END
            }
            break ;
          DEFAULT
            printf("    Routine %s.  Unrecognised token at level 1\n",my_name) ;
            printf("      Token1 = %s\n",token1) ;
            break ;
        END
      }
      if(token1)free(token1) ;
      if(token2)free(token2) ;
      return 0 ;
}

int init_turntable(TurnTable *tt){

  TrkSector      *ts ;
  TrkVectorNode  *tv ;

      tt->next = NULL ;
      tt->w_uid  = 0  ;
      tt->tile_x = 0  ;
      tt->tile_y = 0  ;
      tt->x_offset   = 0.0 ;
      tt->y_offset   = 0.0 ;
      tt->z_offset   = 0.0 ;
      tt->trackshapeindex = 0 ;
      tt->animation  = NULL ;
      tt->diameter   = 0.0  ;
      tt->angle      = 0.0  ;
      tt->n_paths    = 0    ;
      tt->f_path     = NULL ;
      tt->path       = NULL ;
      ts = &tt->trk_sector0 ;
      tv = &tt->trk_vector0 ;

      ts->next = NULL ;
      ts->uid  = 0 ;
      ts->type_of_node = END_SECTION ;
      ts->type_of_pin[0] = 2 ;
      ts->type_of_pin[1] = 0 ;
      ts->trk_sector[0] = &end_sector0;
      ts->trk_sector[1] = &end_sector0;

      tv->next         = NULL ;
      tv->flag1        = 0    ;
      tv->flag2        = 0.0  ;
      tv->is_curved    = 0    ;
      tv->is_dynamic   = 0    ;
      tv->string[0]    = '\0' ;
      tv->string[1]    = '\0' ;

      tv->tile_east_x  = 0.0  ;
      tv->tile_north_z = 0.0  ;

      tv->east_x     = 0.0 ;
      tv->height_y   = 0.0 ;
      tv->north_z    = 0.0 ;

      tv->a_east_x   = 0.0 ;
      tv->a_height_y = 0.0 ;
      tv->a_north_z  = 0.0 ;

      tv->length     = 0.0  ;
      tv->radius     = 0.0  ;
      tv->angle      = 0.0  ;
      tv->distance0  = 0.0  ;

      return 0 ;
}
