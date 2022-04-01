/*
 * *****************************************************************************
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File: road_sct_db.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File contains routines for
 *     reading the road database file and
 *     populating the road section database
 *     adding world item pointers to track vector nodes
 *
 * *****************************************************************************
 */

int init_road_db(char * filename)
{
 int    i, j, ip = 0 ;
 char   *datafile ;
 char   *token    ;
 char   *string   ;
 char   myname[] = "init_road_db" ;
 char   ffname[] = "road_sec_db"      ;

 MSfile msfile  ;

      if(ip)printf("  Enter routine %s\n",myname) ;
/*
 * *****************************************************************************
 * Open road database file ; usually *.rdb
 * *****************************************************************************
 */
    datafile = (char *)malloc(strlen(ORroutedir)+strlen(filename)+1);
    strcpy(datafile,ORroutedir);
    strcat(datafile,filename);

    if(open_msfile(datafile, &msfile, 0, 0)!=0) error1(myname,ffname,datafile);
/*
 * Check for 'TrackDB' token
 */
    token = new_tmp_token(&msfile) ;  // Temporaty token !!
    string = (char *)"TrackDB" ;      // Same for both road and rail databases
    if(0 != strcmp(token,string))error2(myname,ffname,datafile,string);
    skip_lbr(&msfile) ;               // skip left bracket
/*
 *  Check for 'Serial' token
 */
    token = new_tmp_token(&msfile) ;  // Temporaty token !!
    string = (char *)"Serial" ;
    if(0 != strcmp(token,string)) error2(myname,ffname,datafile,string);

/*
 *  Read and store track serial number
 */
    skip_lbr(&msfile) ;
    road_db.serial_number = itoken(&msfile) ;
    skip_rbr(&msfile) ;
/*
 * *****************************************************************************
 *  Check for 'TrackNodes' token
 * *****************************************************************************
 */
    token = new_tmp_token(&msfile) ;    // Temporaty token !!
    string = (char *)"TrackNodes" ;
    if(0 != strcmp(token,string)) error2(myname,ffname,datafile,string);
    skip_lbr(&msfile) ;           // skip left bracket
//    printf("\n  New TrackNodes list\n");
/*
 *  Read and store number of road sections
 */
    road_db.trk_sections_array_size = itoken(&msfile) ;
//    printf("  trk_sec_array_size = %i\n",road_db.trk_sections_array_size);
    road_db.trk_sections_array = (TrkSectNode *)
              malloc(road_db.trk_sections_array_size*sizeof(TrkSectNode));
/*
 *   Cycle over the set of road sections
 */
    for(i=0;i<(int)road_db.trk_sections_array_size;i++){
      read_road_path(&road_db.trk_sections_array[i],&msfile) ;
    }
    skip_rbr(&msfile) ;
/*
 * *****************************************************************************
 *  Check for 'TrItemTable' token
 * *****************************************************************************
 */
    token = new_tmp_token(&msfile) ;    // Temporaty token !!
    string = (char *)"TrItemTable" ;
    if(0 != strcmp(token,string)) error2(myname,ffname,datafile,string);
    skip_lbr(&msfile) ;           // skip left bracket
//    printf("\n  New TrItemTable list\n");
/*
 *  Read and store number of track items
 */
    road_db.trk_items_array_size = itoken(&msfile) ;
//    printf("  road_item_array_size = %i\n",road_db.trk_items_array_size);
    road_db.trk_items_array = (TrkItemNode *)
                  malloc(road_db.trk_items_array_size*sizeof(TrkItemNode));
/*
 *   Cycle over the track items
 */
    for(i=0;i<(int)road_db.trk_items_array_size;i++){
      read_road_item(&road_db.trk_items_array[i],&msfile) ;
    }
/*
 * *****************************************************************************
 *  End of file
 *  Look for final ')'
 * *****************************************************************************
 */
    skip_rbr(&msfile) ;           // skip final right bracket


/*
 * *****************************************************************************
 *
 *   Find geographical limits
 *
 * *****************************************************************************
 */
uint            ie = 0, iw = 0, in = 0, is = 0 ;
double          de = 0, dw = 0, dn = 0, ds = 0 ,
                 h0 = 100000.0, h1 = 0.0 ;;
TrkSectNode   *road_sec_node ;
TrkVectorNode    *vec          ;

    for(i=0;i<(int)road_db.trk_sections_array_size;i++){
        road_sec_node = &road_db.trk_sections_array[i];
         vec = road_sec_node->vector ;
         for(j=0;j<(int)road_sec_node->length_of_vector;j++){
#if 0
           printf(" vec = %6i %6i %8.2f %8.2f %8.2f %6i %6i %8.2f %8.2f %8.2f \n",
                                    vec[j].tile_east_x,vec[j].tile_north_z,
                                    vec[j].east_x,     vec[j].north_z,
                                    vec[j].height_y,
                                    vec[j].tile_east_x  - vec[j].wfname_east_x,
                                    vec[j].tile_north_z - vec[j].wfname_north_z,
                                    vec[j].a_east_x,
                                    vec[j].a_north_z,
                                    vec[j].a_height_y
                                    );
#endif
           if(0 == ie){
             ie = iw = vec[j].tile_east_x  ;  in = is = vec[j].tile_north_z ;
             de = dw = vec[j].east_x       ;  dn = ds = vec[j].north_z ;
           }
// West
           if(vec[j].tile_east_x < iw){
             iw = vec[j].tile_east_x ;
             dw = vec[j].east_x ;
           }else{
             if((vec[j].tile_east_x == iw) && (vec[j].east_x < dw))
             dw = vec[j].east_x ;
           }
// East
           if(vec[j].tile_east_x > ie){
             ie = vec[j].tile_east_x ;
             de = vec[j].east_x ;
           }else{
             if((vec[j].tile_east_x == ie) && (vec[j].east_x < de))
             de = vec[j].east_x ;
           }
//South
           if(vec[j].tile_north_z < is){
             is = vec[j].tile_north_z ;
             ds = vec[j].north_z ;
           }else{
             if((vec[j].tile_north_z == is) && (vec[j].north_z < ds))
             ds = vec[j].north_z ;
           }
//North
           if(vec[j].tile_north_z > in){
             in = vec[j].tile_north_z ;
             dn = vec[j].north_z ;
           }else{
             if((vec[j].tile_north_z == in) && (vec[j].north_z < dn))
             dn = vec[j].north_z ;
           }

           if(vec[j].height_y < h0) h0 = vec[j].height_y ;
           if(vec[j].height_y > h1) h1 = vec[j].height_y ;
         }
      }

      printf("    LIMITS \n");
      printf("    iw   = %i    dw = %f\n",iw,dw);
      printf("    ie   = %i    de = %f\n",ie,de);
      printf("    is   = %i    ds = %f\n",is,ds);
      printf("    in   = %i    dn = %f\n",in,dn);
      printf("    h0   = %f\n",h0);
      printf("    h1   = %f\n",h1);

      trk_cell_west  = iw ;
      trk_cell_east  = ie ;
      trk_cell_south = is ;
      trk_cell_north = in ;

      free(datafile);
      close_msfile(&msfile) ;
      if(ip)printf("  Exit  routine %s\n",myname) ;

      return 0;
}

/*
 * ***********************************************************************
 *   Routine to read and store information from one TrackNode block
 *   in a Road file incuding the start 'TrackNode (' and end ')' marks.
 * **********************************************************************
 */

void read_road_path(TrkSectNode *road_path, MSfile *msfile)
{
 int    j, n   ;             // loop integers
 char   *token ;
 char   myname[] = "read_road_path" ;
 char   ffname[] = "__FILE__"      ;
 TrkVectorNode   *trk_vec_node ;  //  Road vectors similar to track vectors
/*
 * Check header
 */
    token = new_tmp_token(msfile) ;    // Temporaty token !!
    if(strcmp(token,"TrackNode") != 0)
                   error2(myname,ffname,msfile->filename,(char *)"TrackNode");;
//  Save node index
    skip_lbr(msfile) ;
    init_road_path(road_path) ;
    road_path->index_of_node = itoken(msfile) ;
/*
 *  Loop through all possible second level TrackNode tokens
 *   TrJunctionNode, TrEndNode, TrVectorNode,  UiD, TrPins,
 */
    for(;;)
    {
      token = new_tmp_token(msfile);
      if( is_rbr(token) ) break ;  // End of Tracknode data

      SWITCH(token)
        CASE("TrEndNode")
          road_path->type_of_node = END_SECTION ;
          skip_lbr(msfile) ;
            road_path->en = itoken(msfile) ;
          skip_rbr(msfile) ;
          break ;
/*
 * Track Vector Node (May not be used for Roads)
 */
        CASE("TrVectorNode")
          road_path->type_of_node = VECTOR_SECTION ;
          skip_lbr(msfile) ;
          for(;;){         // Also Start Inner Switch Block *************
             token = new_tmp_token(msfile);
             if(is_rbr(token)) break ;
             SWITCH(token)
               CASE("TrVectorSections")
                 skip_lbr(msfile);
                 n = itoken(msfile) ;
                 road_path->length_of_vector =  n ;
                 road_path->vector = (TrkVectorNode *)malloc(n*sizeof(TrkVectorNode)) ;
                 for(j=0;j<n;j++){
                   token = new_tmp_token(msfile);
                   if(is_rbr(token)) break ;
                   trk_vec_node = &road_path->vector[j];
                   trk_vec_node->tsec_section_index  = atoi(token) ;
                   trk_vec_node->tsec_shape_index    = itoken(msfile) ;
                   trk_vec_node->wfname_east_x       = itoken(msfile) ;
                   trk_vec_node->wfname_north_z      = itoken(msfile) ;
                   trk_vec_node->worldfileuid        = itoken(msfile) ;
                   trk_vec_node->flag1               = itoken(msfile) ;
                   trk_vec_node->flag2               = itoken(msfile) ;
                   token = new_tmp_token(msfile);
                   trk_vec_node->string[0]           = token[0] ;
                   trk_vec_node->string[1]           = token[1] ;
                   trk_vec_node->tile_east_x         = itoken(msfile) ;
                   trk_vec_node->tile_north_z        = itoken(msfile) ;
                   trk_vec_node->east_x              = dtoken(msfile) ;
                   trk_vec_node->height_y            = dtoken(msfile) ;
                   trk_vec_node->north_z             = dtoken(msfile) ;
                   trk_vec_node->a_east_x            = dtoken(msfile) ;
                   trk_vec_node->a_height_y          = dtoken(msfile) ;
                   trk_vec_node->a_north_z           = dtoken(msfile) ;
                 }
                 skip_rbr(msfile) ;
                 break;
               CASE("TrItemRefs")
                skip_lbr(msfile);
                 n = itoken(msfile);                  // Number of Track Items
                 road_path->trk_item_number = n;
                 road_path->trk_item_vec = (uint *)malloc(n*sizeof(uint));
                 for(j=0;j<n;j++){
                    token = new_tmp_token(msfile);  //  TrItemRef
                    if(is_rbr(token)) break ;
                    skip_lbr(msfile);
                    road_path->trk_item_vec[j] = itoken(msfile) ;
                    skip_rbr(msfile) ;
                 }
                 skip_rbr(msfile) ;
                 break;
               DEFAULT
                 exit(1);
             END
           }              // Also end inner switch Block  ***************
           break ;
/*
 * UiD Node
 */
        CASE("UiD")
          skip_lbr(msfile) ;
          road_path->length_of_vector = 1 ;
          road_path->vector = (TrkVectorNode *)malloc(sizeof(TrkVectorNode)) ;
          trk_vec_node         = road_path->vector;

          trk_vec_node->wfname_east_x  = itoken(msfile) ;
          trk_vec_node->wfname_north_z = itoken(msfile) ;
          trk_vec_node->worldfileuid   = itoken(msfile) ;
          trk_vec_node->flag1          = itoken(msfile) ;   //  WRONG VARIABLE ??
          trk_vec_node->tile_east_x    = itoken(msfile) ;
          trk_vec_node->tile_north_z   = itoken(msfile) ;
          trk_vec_node->east_x         = dtoken(msfile) ;
          trk_vec_node->height_y       = dtoken(msfile) ;
          trk_vec_node->north_z        = dtoken(msfile) ;
          trk_vec_node->a_east_x       = dtoken(msfile) ;
          trk_vec_node->a_height_y     = dtoken(msfile) ;
          trk_vec_node->a_north_z      = dtoken(msfile) ;
          skip_rbr(msfile) ;
          break ;
/*
 * TrPins Node
 */
        CASE("TrPins")
          skip_lbr(msfile) ;
          for(j=0;j<2;j++){
            road_path->type_of_pin[j] = itoken(msfile);
          }
          for(j=0;;j++){
            token = new_tmp_token(msfile);
            if(is_rbr(token)) break ;
            if(0 == strcmp(token,"TrPin")){
              skip_lbr(msfile) ;
              road_path->pin_to_section[j] = itoken(msfile) ;
              road_path->pin_info[j]       = itoken(msfile) ;
              skip_rbr(msfile) ;
            }else{
              printf("   TrPin : token not recognised %s\n",token);
              exit(1) ;
            }
          }
          break;
        DEFAULT
          printf(" Error token not recognised.  Token = %s\n",token);
          exit(1) ;
      END
    }
//  printf(" Exit from routine %s\n",myname);
  return ;
}

void init_road_path(TrkSectNode *road_path)
{
  int  i;
       road_path->index_of_node    = 0;
       road_path->type_of_node     = 0;
       road_path->length_of_vector = 0 ;

       for(i=1;i<3;i++){
         road_path->type_of_pin[0]    = 0 ;
         road_path->pin_to_section[i] = 0 ;
         road_path->pin_info[i]       = 0 ;
       }
}

#if 0
void error1(char *routine, char *file, char *datafile)
{
        printf("\n  ERROR.  Routine: '%s'.  File: '%s'.\n"
                 "          Routine open_msfile failed to open file.\n"
                 "          File name = %s\n",routine, file, datafile);
        printf("\n  Program St0pping\n\n");
        exit(1) ;
}

void error2(char *routine, char *file, char *datafile, char *string)
{
        printf("\n  ERROR.  Routine: '%s'.  File: '%s'.\n"
                 "          Routine failed to find token %s in data file.\n"
                 "          Data file name = %s\n",routine, file, string, datafile);
        printf("\n  Program St0pping\n\n");
        exit(1) ;
}
#endif

