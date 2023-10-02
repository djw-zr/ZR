/*
 *******************************************************************************
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File: trk_sct_db.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File contains routines for
 *       reading the track database file (*.tdb) and
 *       populating the TrkDataBase structure (struct.h)
 *         TrkDataBase track_db ; (zr.h)
 *   Based on OpenRails file TrackDatabaseFile.cs
 *
 * *****************************************************************************
 */
int  add_distances_to_track_vectors(TrkVectorNode *v, TrkSector *t, int n) ;
int  init_track_section(TrkSector *tsect) ;
static int cmp_dist(const void *p1, const void *p2) ;
#if 0
static int find_potential_world_items(TrkSector *ts0,int uid0,
                                          int *ix0, int *iy0, int *ifound) ;
#endif
/*
 *   Main read routine
 */

int init_track_db(char * filename){
int    i, j, n, ip = 0 ;
char   *datafile ;
char   *token    ;
char   *string   ;
char   myname[] = "init_track_db" ;
char   ffname[] = "trk_sec_db.c"      ;

MSfile msfile  ;

      if(ip)printf("  Enter routine %s\n",myname) ;
/*
 * *****************************************************************************
 * Open track database file ; usually *.tdb
 * *****************************************************************************
 */
      n = strlen(ORroutedir)+strlen(filename)+1 ;
      datafile = (char *)malloc(n*sizeof(char));
      strcpy(datafile,ORroutedir);
      strcat(datafile,"/");
      strcat(datafile,filename);
      if(ip)printf("  File = %s\n",datafile) ;

      if(open_msfile(datafile, &msfile, 0, 0)!=0) error1(myname,ffname,datafile);
/*
 * Check for 'TrackDB' token
 */
      token = new_tmp_token(&msfile) ;  // Temporaty token !!
      string = (char *)"TrackDB" ;
      if(0 != strcmp(token,string))error2(myname,ffname,datafile,string);
      skip_lbr(&msfile) ;           // skip left bracket
/*
 *  Check for 'Serial' token
 */
      token = new_tmp_token(&msfile) ;    // Temporaty token !!
      string = (char *)"Serial" ;
      if(0 != strcmp(token,string)) error2(myname,ffname,datafile,string);
/*
 *  Read and store track serial number
 */
      skip_lbr(&msfile) ;
      track_db.serial_number = itoken(&msfile) ;
      if(ip)printf("  Track serial number = %i\n", track_db.serial_number ) ;
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
/*
 *  Read and store number of track sections
 */
      track_db.trk_sections_array_size = itoken(&msfile) ;
      track_db.trk_sections_array = (TrkSector *)
              malloc(track_db.trk_sections_array_size*sizeof(TrkSector));
/*
 *   Cycle over the set of track sections
 */
      for(i=0;i<(int)track_db.trk_sections_array_size;i++){
        init_track_section(&track_db.trk_sections_array[i]) ;
        read_track_path(&track_db.trk_sections_array[i],&msfile) ;
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
/*
 *  Read and store number of track items
 */
      track_db.trk_items_array_size = itoken(&msfile) ;
      track_db.trk_items_array = (TrkItem *)
                  malloc(track_db.trk_items_array_size*sizeof(TrkItem));
      printf("  ZXZ ZXZ Track item 242 = %p\n",(void *)&track_db.trk_items_array[242] ) ;

/*
 *   Cycle over the track items
 */
      for(i=0;i<(int)track_db.trk_items_array_size;i++){
//        printf(" READING TRACK ITEM = %i\n",i) ;
        read_track_item(&track_db.trk_items_array[i],&msfile) ;
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
TrkSector   *trk_path_node ;
TrkVectorNode *vec          ;

      for(i=0;i<(int)track_db.trk_sections_array_size;i++){
        trk_path_node = &track_db.trk_sections_array[i];
        vec = trk_path_node->vector ;
        for(j=0;j<(int)trk_path_node->length_of_vector;j++){
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
            if((vec[j].tile_east_x == ie) && (vec[j].east_x > de))
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
            if((vec[j].tile_north_z == in) && (vec[j].north_z > dn))
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
      trk_min_height = h0 ;
      trk_max_height = h1 ;
      tile_h0 = 100.0*floor(trk_min_height/100.0) ;
      if(tile_h0 <0.0) tile_h0 = 0.0 ;

      free(datafile) ;
      close_msfile(&msfile) ;
      if(ip)printf("  Exit  routine %s\n",myname) ;

      return 0;
}

/*
 *  Routine to initialise a Track Section Node
 */
int init_track_section(TrkSector *tsect){

      tsect->next             = NULL ;
      tsect->uid    = UINT_MAX ;
      tsect->type_of_node     = 0    ;

      tsect->jn[0]            = UINT_MAX ;
      tsect->jn[1]            = UINT_MAX ;
      tsect->jn[2]            = UINT_MAX ;
      tsect->en               = UINT_MAX ;
      tsect->length_of_vector = 0    ;
      tsect->vector           = NULL ;
      tsect->trk_item_number  = 0    ;
      tsect->trk_item_list    = NULL ;
      tsect->n_dist_levels    = 0    ;
      tsect->length           = 0.    ;

      return 0 ;
}


/*
 * ***********************************************************************
 *
 *   Routine to read and store information from one TrackNode block
 *   incuding the start 'TrackNode (' and end ')' marks.
 *   Tracknodes are usually in *.trk files
 *
 * **********************************************************************
 */

void read_track_path(TrkSector *trk_path, MSfile *msfile){

int    j, n ;             // loop integers
char   *token    ;
char   myname[] = "read_track_path" ;
char   ffname[] = "__FILE__"      ;
TrkVectorNode  *trk_vec_node ;
/*
 * Check header
 */
      token = new_tmp_token(msfile) ;    // Temporaty token !!
      if(strcmp(token,"TrackNode") != 0)
                    error2(myname,ffname,msfile->filename,(char *)"TrackNode");;
//  Save node index
      skip_lbr(msfile) ;
      init_track_path(trk_path) ;
      trk_path->uid = itoken(msfile) ;
/*
 *  Loop through all possible second level TrackNode tokens
 *   TrJunctionNode, TrEndNode, TrVectorNode,  UiD, TrPins,
 */
      for(;;){
        token = new_tmp_token(msfile);
        if( is_rbr(token) ) break ;  // End of Tracknode data

        SWITCH(token)
          CASE("TrJunctionNode")
            trk_path->type_of_node = JUNCTION ;
            skip_lbr(msfile) ;
              trk_path->jn[0]=itoken(msfile) ;  //  Shape describing this junction ?
              trk_path->jn[1]=itoken(msfile) ;  //  Usually non-zero - the curved path ??
              trk_path->jn[2]=itoken(msfile) ;
            skip_rbr(msfile) ;
            break ;
          CASE("TrEndNode")
            trk_path->type_of_node = END_SECTION ;
            skip_lbr(msfile) ;
              trk_path->en = itoken(msfile) ;
            skip_rbr(msfile) ;
            break ;
/*
 * Track Vector Node
 */
          CASE("TrVectorNode")
            trk_path->type_of_node = VECTOR_SECTION ;
            skip_lbr(msfile) ;
            for(;;){         // Also Start Inner Switch Block *************
              token = new_tmp_token(msfile);
              if(is_rbr(token)) break ;
              SWITCH(token)
                CASE("TrVectorSections")
                  skip_lbr(msfile);
                  n = itoken(msfile) ;
                  trk_path->length_of_vector =  n ;
                  trk_path->vector = (TrkVectorNode *)malloc(n*sizeof(TrkVectorNode)) ;
                  for(j=0;j<n;j++){
                    token = new_tmp_token(msfile);
                    if(is_rbr(token)) break ;
                    trk_vec_node = &trk_path->vector[j];
                    trk_vec_node->tsec_section_index  = atoi(token)    ;
                    trk_vec_node->tsec_shape_index    = itoken(msfile) ;
                    trk_vec_node->wfname_east_x       = itoken(msfile) ;
                    trk_vec_node->wfname_north_z      = itoken(msfile) ;
                    trk_vec_node->worldfileuid        = itoken(msfile) ;
                    trk_vec_node->flag1               = itoken(msfile) ; // 0, 1 if flipped  Sometimes 2
                    trk_vec_node->flag2               = itoken(msfile) ; // 1, 0                       3
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

                    trk_vec_node->world_item = NULL ;
                    trk_vec_node->is_curved  = 0    ;
                    trk_vec_node->is_dynamic = 0    ;
                    trk_vec_node->length     = 0.0  ;
                    trk_vec_node->radius     = 0.0  ;
                    trk_vec_node->angle      = 0.0  ;
                    trk_vec_node->distance0  = 0.0  ;
                  }
                  skip_rbr(msfile) ;
                  break;
                CASE("TrItemRefs")
                  skip_lbr(msfile);
                  n = itoken(msfile);                  // Number of Track Items
                  trk_path->trk_item_number = n;
                  trk_path->trk_item_list = (uint *)malloc(n*sizeof(uint));
                  for(j=0;j<n;j++){
                      token = new_tmp_token(msfile);  //  TrItemRef
                      if(is_rbr(token)) break ;
                      skip_lbr(msfile);
                      trk_path->trk_item_list[j] = itoken(msfile) ;
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
 *  UiD Node : In the input file these are specified for junction and end nodes.
 *             They contain data on the world/shape used to display the
 *             moving junction rails or the track end.
 *
 *             They are not specified for vector nodes but contain similar information
 *             on the position of the node as the track vector node data. For simplicity
 *             their position and other data is therefor stored here in a standard track
 *             vector structure.
 */
          CASE("UiD")
            skip_lbr(msfile) ;
            trk_path->length_of_vector = 1 ;
            trk_path->vector = (TrkVectorNode *)malloc(sizeof(TrkVectorNode)) ;
            trk_vec_node         = trk_path->vector;

            trk_vec_node->wfname_east_x  = itoken(msfile) ;
            trk_vec_node->wfname_north_z = itoken(msfile) ;
            trk_vec_node->worldfileuid   = itoken(msfile) ;
            trk_vec_node->world_item     = NULL           ;
            trk_vec_node->ax             = 0.0            ;
            trk_vec_node->ay             = 0.0            ;
            trk_vec_node->az             = 0.0            ;
            trk_vec_node->ang            = 0.0            ;
            trk_vec_node->flag1          = itoken(msfile) ;   //  WRONG VARIABLE ??
            trk_vec_node->flag1          = 0.0            ;
            trk_vec_node->is_curved      = 0    ;
            trk_vec_node->is_dynamic     = 0    ;
            trk_vec_node->string[0]      = '\0' ;
            trk_vec_node->string[1]      = '\0' ;
            trk_vec_node->tile_east_x    = itoken(msfile) ;
            trk_vec_node->tile_north_z   = itoken(msfile) ;
            trk_vec_node->east_x         = dtoken(msfile) ;
            trk_vec_node->height_y       = dtoken(msfile) ;
            trk_vec_node->north_z        = dtoken(msfile) ;
            trk_vec_node->a_east_x       = dtoken(msfile) ;
            trk_vec_node->a_height_y     = dtoken(msfile) ;
            trk_vec_node->a_north_z      = dtoken(msfile) ;
            trk_vec_node->length         = 0.0  ;
            trk_vec_node->radius         = 0.0  ;
            trk_vec_node->angle          = 0.0  ;
            trk_vec_node->distance0      = 0.0  ;

            skip_rbr(msfile) ;
            break ;
/*
 * TrPins Node
 */
          CASE("TrPins")
            skip_lbr(msfile) ;
            for(j=0;j<2;j++){
              trk_path->type_of_pin[j] = itoken(msfile);
            }
            for(j=0;;j++){
              token = new_tmp_token(msfile);
              if(is_rbr(token)) break ;
              if(0 == strcmp(token,"TrPin")){
                skip_lbr(msfile) ;
                trk_path->pin_to_section[j] = itoken(msfile) ;  // Section connected
                trk_path->pin_info[j]       = itoken(msfile) ;  // Section end (start/end)
                trk_path->trk_sector[j]     =
                             &track_db.trk_sections_array[trk_path->pin_to_section[j]-1] ;
                skip_rbr(msfile) ;
              }else{
                printf("   TrPin : token not recognised %s\n",token);
                exit(1) ;
              }
              if(0 == trk_path->pin_to_section[j] || 0 == trk_path->trk_sector[j] ){
                printf("  ERROR.  Routine %s.  Pin links to section '0'",myname) ;
                printf("               Node uid    = %i\n",trk_path->uid) ;
                printf("               Node type   = %i\n",trk_path->type_of_node) ;
                printf("               Node pin #  = %i\n",j) ;
                printf("               pin_info    = %i\n",trk_path->pin_info[j]) ;
                printf("               pin_to_section  = %i\n",trk_path->pin_to_section[j]) ;
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

void init_track_path(TrkSector *track_path){
int  i;

      track_path->uid    = 0;
      track_path->type_of_node     = 0 ;
      track_path->length_of_vector = 0 ;
      track_path->trk_item_number  = 0 ;
      track_path->straight         = 0 ;
      track_path->branch           = 0 ;
      track_path->mainline         = 0 ;

      for(i=1;i<3;i++){
        track_path->type_of_pin[0]    = 0 ;
        track_path->pin_to_section[i] = 0 ;
        track_path->pin_info[i]       = 0 ;
        track_path->trk_sector[i]     = NULL ;
      }
      return ;
}


void error1(char *routine, char *file, char *datafile){
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

int list_track_section(TrkSector *tnode){

uint          i      ;
TrkVectorNode *vnode ;
WorldNode     *wn    ;
WorldItem     *wi    ;
char          *my_name = "list_track_section";

      printf("  Track section.  Index = %i\n",tnode->uid);
      printf("  Type = %i :: %s\n",tnode->type_of_node,
                                   token_trackdb[tnode->type_of_node]) ;
      printf("  jn    = %i %i %i\n",tnode->jn[0],tnode->jn[1],tnode->jn[2]);
      printf("  en    = %i\n",tnode->en);
      printf("  type of pin     = %3i %3i\n",tnode->type_of_pin[0],
                                             tnode->type_of_pin[1]);
      printf("  pin to section  = %3i %3i %3i\n",tnode->pin_to_section[0],
                        tnode->pin_to_section[1],tnode->pin_to_section[2]) ;
      printf("  pin info        = %3i %3i %3i\n",tnode->pin_info[0],
                                    tnode->pin_info[1],tnode->pin_info[2]) ;
      printf("  length of sectiom = %f\n",tnode->length) ;
      printf("  length of vector  = %i\n",tnode->length_of_vector) ;
      for(i=0;i<tnode->length_of_vector;i++){
        vnode = &tnode->vector[i] ;
        printf("   Vector %3i,  world east = %5i,  north   = %5i\n", i,
                                  vnode->wfname_east_x,vnode->wfname_north_z) ;
        printf("                world_item = %p\n",(void *)vnode->world_item);
        printf("                tile east  = %5i,  north   = %5i",
                                  vnode->tile_east_x,vnode->tile_north_z) ;
        printf("      east_x = %f,  north_z = %f\n",
                                  vnode->east_x,vnode->north_z) ;
        printf("      tsec_section_index    = %i\n", vnode->tsec_section_index) ;
        printf("      tsec_shape_index      = %i\n", vnode->tsec_shape_index)   ;
        printf("      worldfileuid          = %i\n", vnode->worldfileuid)   ;
        printf("      flag1, flag2, string  = %i  %i %2s\n", vnode->flag1, vnode->flag2, vnode->string)   ;
        printf("      length                = %f\n", vnode->length)   ;
        printf("      radius                = %f\n", vnode->radius)   ;
        printf("      angle                 = %f\n", vnode->angle)   ;
        printf("      ---------------------\n")   ;

        if(vnode->tsec_section_index != 0){
          print_tsec_section( vnode->tsec_section_index) ;
        }
        if(vnode->tsec_shape_index != 0){
          print_tsec_shape( vnode->tsec_shape_index) ;
        }
        if(vnode->worldfileuid){
          printf("  Routine %s\n",my_name) ;
          printf("  worldfileuid   = %i\n",vnode->worldfileuid) ;
          printf("  wfname_east_x  = %i\n",vnode->wfname_east_x) ;
          printf("  wfname_north_z = %i\n",vnode->wfname_north_z) ;
        }
        if(vnode->worldfileuid != 0){
          for(wn=worldlist_beg;wn!=NULL;wn=wn->next){
            if(wn->tile_x != vnode->wfname_east_x || wn->tile_y != vnode->wfname_north_z) continue ;
            for(wi=wn->world_item;wi!=NULL;wi=wi->next){
              if(wi->uid != (int)vnode->worldfileuid)continue ;
              printf("  witem found %p\n",(void *)wi);
              list_wfile_item(wi) ;
            }
          }
        }
        printf("\n") ;
      }
      return 0 ;
}

/*
 *   Routine to add world item data to each of track vectors
 *   The UID data of Junction and End nodes is processed in the same way
 *
 *   Additionlly the routine saves data on the length, radius and
 *   angle of each track vector and the length of each track section.
 */

int add_world_item_pointers_to_track_vectors(TrkSector *tnnode) {

int            ip = 0 ;  // Debug
int            iret = 0 ;
int            ifound  ;
uint           nvec ;
int            wtile_x, wtile_y ;
int           w_uid ;
TrkVectorNode  *vec, *vec0 = NULL  ;  // Vector and previous vector
WorldNode      *wnode  ;
WorldItem      *witem  ;
char           my_name[] = "add_world_item_pointers_to_track_vectors" ;
/*
 *  Loop over track vector array.  Note Junction and End node UID data is stored
 *  in a track vector array of length 1.
 */
//      ip = tnnode->uid == 3139 ;

      if(ip)printf(" Enter %s, track node = %i %i %i %p\n",my_name,
                          tnnode->uid,tnnode->type_of_node,
                          tnnode->length_of_vector,(void *)tnnode) ;

      for(nvec=0;nvec<tnnode->length_of_vector;nvec++){
        if(ip)printf("  nvec = %i\n",nvec) ;

        vec = &(tnnode->vector[nvec]) ;
/*
 *  For each vector find world node including shape for this vector
 */
        wtile_x = vec->wfname_east_x  ;
        wtile_y = vec->wfname_north_z ;
        w_uid   = vec->worldfileuid   ;
        if(ip)printf("    vec   X = %i, Y = %i,  UID = %i\n",wtile_x,wtile_y,w_uid) ;
        for(wnode=worldlist_beg;wnode!=NULL;wnode=wnode->next){
          if(ip)printf("    wnode X = %i, Y = %i, World node = %p, First world_item = %p\n",
               wnode->tile_x,wnode->tile_y, (void *)wnode, (void *)wnode->world_item) ;
          if(wnode->tile_x == wtile_x && wnode->tile_y == wtile_y)break ;
        }
        fflush(NULL) ;
        if(wnode->tile_x != wtile_x || wnode->tile_y != wtile_y){
          printf("  ERROR.  Routine %s unable to find world node for %i %i \n",
                                my_name,wtile_x,wtile_y) ;
          exit(0) ;
        }
        if(ip)printf("  AA tile X = %i, Y = %i\n",wnode->tile_x, wnode->tile_y) ;
/*
 *  ... and then world item - i.e. shape for this node.
 */
        fflush(NULL) ;
        ifound = 0 ;
        for(witem=wnode->world_item; witem!=NULL; witem=witem->next){
          if(ip)printf("  uid = %i %i %s\n",w_uid, witem->uid,wnode->name) ;
          if(witem->uid == w_uid){ifound = 1 ; break ;}
          if(ip)printf("  cycle : %p",(void *)witem->next) ;
        }
        if(!ifound){
          printf("  ERROR.  Routine %s cannot find world item\n"
                 "          track section = %i, vector = %i, world item = %i, "
                           "world/location tile = %i %i / %i %i, vec x, y, z = %7.2f %7.2f %7.2f\n",
                 my_name, tnnode->uid, nvec, w_uid, wtile_x, wtile_y,
                 vec->tile_east_x, vec->tile_north_z, vec->east_x, vec->north_z, vec->height_y) ;
#if 0
          find_potential_world_items(tnnode, w_uid, &wtile_x, &wtile_y, &ifound) ;
/*
 *  If found try again
 */
          if(ifound){
            vec->wfname_east_x  = wtile_x ;
            vec->wfname_north_z = wtile_y ;
            for(wnode=worldlist_beg;wnode!=NULL;wnode=wnode->next){
              if(ip)printf("    wnode X = %i, Y = %i, World node = %p, First world_item = %p\n",
                  wnode->tile_x,wnode->tile_y, (void *)wnode, (void *)wnode->world_item) ;
              if(wnode->tile_x == wtile_x && wnode->tile_y == wtile_y)break ;
            }
            ifound = 0 ;
            for(witem=wnode->world_item; witem!=NULL; witem=witem->next){
              if(ip)printf("  uid = %i %i %s\n",w_uid, witem->uid,wnode->name) ;
              if(witem->uid == w_uid){ifound = 1 ; break ;}
              if(ip)printf("  cycle : %p",(void *)witem->next) ;
            }
          }else{
              printf("  World Item still not found !!\n") ;
          }
#endif
        }
        if(ifound){
/*
 *  Save pointer to world item in vector structure
 */
          vec->world_item = witem ;
          vec->ax         = witem->AX ;
          vec->ay         = witem->AZ ;  //  Axes ...
          vec->az         = witem->AY ;  //       ... swapped
          vec->ang        = witem->ANG ;
/*
 *  Save pointer to vector in world item structure
 */

          if(witem->worldtype == TRACKOBJ || witem->worldtype == 306){
            witem->u.track_obj.track_vec = vec ;
            witem->iz_off = 1                  ;  // All track items set to 1
          }else{
            printf(" Possible error.  Track vector %i found world item %i\n", nvec, witem->worldtype);
          }
        }
/*
 *  Add track data on length, radius and angle to track vectors
 */
        if(tnnode->type_of_node != VECTOR_SECTION)continue ;
        add_distances_to_track_vectors(vec,tnnode,nvec) ;
        if(nvec >0){
          vec->distance0 = vec0->length + vec0->distance0 ;
        }
        vec0 = vec ;
      }
/*
 *  Save length of section
 */
      if(tnnode->type_of_node == VECTOR_SECTION){
        vec = &(tnnode->vector[tnnode->length_of_vector-1]) ;
        tnnode->length = vec->length + vec->distance0 ;
      }

      return 0 ;
}

/*
 *  Add track length, radius and angle to each track_vector_node.
 *  The routine uses the world item to determine if the track is dynamic
 *  track.
 */

int  add_distances_to_track_vectors(TrkVectorNode *v, TrkSector *t, int nvec){

int  ip = 0 ;
int  k, kk ;
int  flag1  = v->flag1 ;
int  flag2  = v->flag2 ;
int  is_normal = 0 ;
uint ns     = v->tsec_section_index ;

TrackSection   *ts ;
WorldItem      *w=v->world_item ;
DynTrackObj    *dto ;

static int ndt = 0 ;  //  Number of dynamic track sections to process
static int idt = 0 ;  //  Current dynamic track section being processed
static WorldItem  *ws = NULL ;
char my_name[] = "add_distances_to_track_vectors" ;

      k = 0 ;
//      ip = (t->uid == 3139) ;
/*
 *  Bug fix - needed for example when a track database places the shape
 *  associated with a piece of track in the wrong world tile, so the
 *  corresponding world item cannot be found.
 */
      if(v->world_item != NULL){
        is_normal = v->world_item->worldtype != 306 ;  // Track not dynamic track
      }else{
//        is_normal = v->tsec_section_index != 0 && v->tsec_shape_index != 0 ;
        printf("  ERROR.  Routine %s.  World item pointer is NULL in track sector %i, vector %i.\n"
               "            Assuming normal track, with section index = %i\n",
                 my_name, t->uid, nvec, ns) ;
        is_normal = 1 ;
      }

      if(ip){
        printf("  Routine %s\n",my_name) ;
        printf("    Track node = %4i, vector = %i, flags = %i %i, is_normal = %i, ns = %i, ndt = %i, idt = %i, w = %p\n",t->uid,nvec,flag1,flag2,is_normal,ns,ndt,idt,(void *)w) ;
//      printf("  worldtype = %i\n",v->world_item->worldtype) ;
      }
      if(is_normal){
        if(ndt != 0){
          printf("  ERROR.  Routine %s.  Normal track found while processing dynamic track\n",my_name);
          exit(0) ;
        }
        for(ts = track_section_beg; ts != NULL; ts = ts->next){
          if(ts->index == ns)break ;
        }
        if(!ts || ts->index != ns){
          printf("  ERROR.  Routine %s.  Cannot find track section %i\n",my_name,ns);
          for(ts = track_section_beg, k=1; ts != NULL; ts = ts->next, k++){
            if(k ==0 ) printf(" Track sections : ") ;
            printf(" %i",ts->index) ;
            if(k >= 10){printf("\n") ; k = 0 ; }
          }
          printf("\n") ;
          exit(0) ;
        }
/*
 *  Case flag1 ==2 and flag2 == 0 comes from zig-zag track section 565
 */
#if 1
        v->length = ts->length ;
        v->radius = ts->radius ;
        v->angle  = ts->angle  ;
        if(ip && !( (0 == flag1 && 1 == flag2) || (0 == flag1 && 2 == flag2))){
          printf("  ERROR.  Routine %s.  Unusual sector: flag1 = %i flag2 = %i\n",
                                    my_name, flag1, flag2) ;
        }
#else
        if((0 == flag1 && 1 == flag2) /*|| (2 == flag1 && 0 == flag2)*/){
          v->length = ts->length ;
          v->radius = ts->radius ;
          v->angle  = ts->angle  ;
        }else if((1 == flag1 && 0 == flag2) /*|| (0 == flag1 && 2 == flag2)*/){
          v->length = ts->length ;
          v->radius = ts->radius ;
          v->angle  = -ts->angle  ;
        }else{
          v->length = ts->length ;
          v->radius = ts->radius ;
          v->angle  = -ts->angle  ;
          if(ip || 0)printf(" Routine %s.  Unusual sector: flag1 = %i flag2 = %i\n",
                                    my_name, flag1, flag2) ;
        }
#endif
        v->is_dynamic = 0 ;
        v->is_curved = (v->angle != 0.0) ;
        if(ip)printf("    NORMAL SECTION :: length = %f, radius = %f, angle = %f\n",
                 ts->length,ts->radius,  ts->angle) ;
/*
 *  Dynamic track
 */
      }else{
        w = v->world_item ;
        dto = &(w->u.dyn_track_obj) ;
//  New dynamic track object
        if(ndt == 0){
          for(k=0;k<5;k++){
            if(dto->dyn_trk_sect[k].uid == -1)continue ;
            ndt = ndt + 1 ;
          }
          idt = 0 ;
          ws = w ;
        }
//  Process
        if(ws != w){
          printf("  ERROR.  Routine %s.  New world file while processing dynamic sections\n",my_name);
          exit(0) ;
        }
        if(0 == flag1 && (1 == flag2 || 2 == flag2)){
          kk = 0 ;
          for(k=0;k<5;k++){
            if(dto->dyn_trk_sect[k].uid == -1)continue ;
            if(kk == idt)break ;
            kk = kk + 1 ;
          }
          if(dto->dyn_trk_sect[k].is_curved){
            v->angle  = dto->dyn_trk_sect[k].param_1 ;
            v->radius = dto->dyn_trk_sect[k].param_2 ;
            v->length = fabs(v->angle*v->radius) ;
            v->is_curved  = 1 ;
            v->is_dynamic = 1 ;
          }else{
            v->length = dto->dyn_trk_sect[k].param_1 ;
            v->radius = 0.0 ;
            v->angle  = 0 ;
            v->is_curved  = 0 ;
            v->is_dynamic = 1 ;
          }
        }else if((1 == flag1 || 2 == flag1) && 0 == flag2){
          kk = 0 ;
          for(k=4;k>-1;k--){
            if(dto->dyn_trk_sect[k].uid == -1)continue ;
            if(kk == idt)break ;
            kk = kk + 1 ;
          }
          if(dto->dyn_trk_sect[k].is_curved){
            v->angle  = -dto->dyn_trk_sect[k].param_1 ;
            v->radius = dto->dyn_trk_sect[k].param_2 ;
            v->length = fabs(v->angle*v->radius) ;
            v->is_curved  = 1 ;
            v->is_dynamic = 1 ;
          }else{
            v->length = dto->dyn_trk_sect[k].param_1 ;
            v->radius = 0.0 ;
            v->angle  = 0 ;
            v->is_curved  = 0 ;
            v->is_dynamic = 1 ;
          }
        }else{
          printf("  ERROR.  Routine %s.  Unusual dynamic track sector:\n",my_name) ;
          printf("  Flag1 = %i, Flag2 = %i, Param_1 = %f, Param_2 = %f\n",
                    flag1, flag2, dto->dyn_trk_sect[k].param_1,  dto->dyn_trk_sect[k].param_2) ;
          return 1 ;
        }
        if(ndt>0)ndt = ndt - 1;
        idt = idt + 1 ;
        if(ip)printf("                      v = %p, Flag1 = %i, Flag2 = %i, Length = %f, Radius = %f, Angle = %f\n",
                  (void *)v, flag1, flag2, v->length, v->radius, v->angle) ;
      }

      if(v->length == 0.0 ){
        printf("  ERROR.  Routine %s.  Track length is zero : Track Index = %4i, Type = %i,"
               " vector = %3i, is_normal = %i\n",
               my_name, t->uid, t->type_of_node, nvec, is_normal) ;
//        list_track_section(t) ;
      }
      return 0;
}

/*
 *  Routine find_track_item_posn
 *
 *  Routine called by init_data_structures (data.c) after
 *  reading the track database and world files
 *
 *  Given a road or rail track database it calculates the position of
 *  the track item in terms of the track database by saving the id of the
 *  corresponding track section and the distance of the item along the section
 *
 */

int set_track_items_posn(TrkDataBase *database){

  uint  i, j, k  ;
  int   ip = 0 ;
  int   found ;
  int   trk_item_index ;
  uint  length_of_vector ;

  TrkItem   *trk_item ;
  TrkSector   *trk_sect ;
  TrkVectorNode *trk_vect ;

  double ti_x, ti_y, ti_z ;    //  Track item position
  double tv_x, tv_y, tv_z ;    //  Track vector origin
  double o_x,  o_y,  o_z  ;    //  Centre of track arc
  double e_x,  e_y,  e_z  ;    //  End point of track vector
  double c_x,  c_y,  c_z  ;    //  Closest track point to track item
  double aa,   bb,   cc   ;    //  Rotation angles

  double nx, ny, nz ;          //  Normal to track
  double tx, ty, tz ;          //  Track tangent at start point
  double rx, ry, rz ;          //  Third axis - radial direction
  double vx, vy, vz ;          //  Unit vector from arc centre to track item

  double trk_vector_length,    //  Length of section of track described by vector
         track_distance,       //  Distance to closest point on section of track
         track_radius,         //  Radius of curved track
         alpha,                //  Distance along track to nearest point
         last_error,           //  Previous smallest error
         error,                //  Error at closest point
         sa,                   //  Sign of Curvature
         dott, dotr, temp, s, c  ;
  double alpha1, alpha2 ;

  char   *my_name = "set_track_items_posn" ;

      if(ip)printf("   Enter routine: %s\n",my_name) ;
/*
 *  Loop over track nodes and for VECTOR_SECTION nodes loop over the
 *  track items
 */
      for(i=0;i<database->trk_sections_array_size;i++){
//        ip = (34 == i) || (3447 == i) || (4474 == i)  ;
//        ip = (484 == i)   ;
        trk_sect = &(database->trk_sections_array[i]) ;
        if(trk_sect->type_of_node  != VECTOR_SECTION)continue ;
//        ip = (trk_sect->uid == 356 ) ;
        if(ip){
          printf("\n============================================================\n") ;
          printf("\n  NEW TRACK SECTION : position %i, uid %i,  number of Items %i\n",
                               i,trk_sect->uid,trk_sect->trk_item_number ) ;
          for(j=0;j<trk_sect->trk_item_number;j++){
            printf("  j = %i, trk_item_list[j] = %i\n",j,trk_sect->trk_item_list[j] ) ;
          }
        }
/*
 *  Lopp over track items
 */
        for(j=0;j<trk_sect->trk_item_number;j++){
          trk_item_index = trk_sect->trk_item_list[j] ;
          trk_item = &(database->trk_items_array[trk_item_index]) ;
          trk_item->track_section = i ;            //  Array index of track section - not uid!!
          if(ip){
            printf("\n------------------------------------------------------------\n") ;
            printf("\n    NEW TRACK ITEM j = %i, trk_item_list[j] = %i,"
                       " uid %i, type_of_node %i  %s\n",
                 j,trk_item_index,trk_item->uid,trk_item->type_of_node,
                 token_trackdb[trk_item->type_of_node]) ;
          }
/*
 *  Calculate position of track item
 */
          global2local(tile_x0, tile_y0, tile_h0, tile_size, 1.0,
                            trk_item->tile_east_x, trk_item->tile_north_z,
                            trk_item->east_x, trk_item->north_z, trk_item->height_y,
                            &ti_x, &ti_y, &ti_z) ;
          if(ip){
            printf("   Track Item    : %i  %i\n",trk_item_index, trk_item->type_of_node) ;
            printf("   Tile origin at: %i %i %8.2f\n",tile_x0, tile_y0, tile_h0) ;
            printf("   Track Item at :  %i %i :: %8.2f %8.2f %8.2f\n",
                                   trk_item->tile_east_x, trk_item->tile_north_z,
                                   trk_item->east_x, trk_item->north_z, trk_item->height_y) ;
            printf("   Track Item at :  %8.2f %8.2f %8.2f\n",ti_x, ti_y, ti_z) ;
          }
/*
 *  Loop over track vectors
 */
          found = 0 ;
          last_error = 10000.0 ;
          error      = 10001.1 ;
          for(k=0;k<trk_sect->length_of_vector;k++){
            trk_vect = &(trk_sect->vector[k]) ;
/*
 *  Calculate origin of vector
 */
            global2local(tile_x0, tile_y0, tile_h0, tile_size, 1.0,
                            trk_vect->tile_east_x, trk_vect->tile_north_z,
                            trk_vect->east_x, trk_vect->north_z, trk_vect->height_y,
                            &tv_x, &tv_y, &tv_z) ;
            if(ip)printf("\n    Vector %i origin at:  %8.2f %8.2f %8.2f :: %i :: %8.2f %8.2f %8.2f :: %8.2f\n",
                            k, tv_x, tv_y, tv_z,trk_vect->is_curved,
                            trk_vect->ax,trk_vect->ay,trk_vect->az,trk_vect->ang);

            trk_vector_length = trk_vect->length ;
            aa = degree*trk_vect->a_east_x ;
            bb = degree*trk_vect->a_height_y ;
            cc = degree*trk_vect->a_north_z ;
/*
 * Calculate unit normal and unit tangent
 */
            mstswagon2local(0., 1., 0., 0., 0., 0., aa, bb, cc, 1.,
                                        0., 0., 0., &nx, &ny, &nz) ;
            mstswagon2local(0., 0., 1., 0., 0., 0., aa, bb, cc, 1.,
                                        0., 0., 0., &tx, &ty, &tz) ;
/*
 *  Straight section
 *  The tests on alpha here and later check for distances along the track
 *  greater than -1.0 m.  This is used instead of 0.0 to allow for rounding
 *  errors.  A check for values greater than -0.01 may be sufficient.
 */
            if(!(trk_vect->is_curved)){
              alpha = (ti_x-tv_x)*tx + (ti_y-tv_y)*ty + (ti_z-tv_z)*tz ;
              if((alpha>=-1.0 && alpha<=trk_vector_length) || ip){

                c_x = tx*alpha  + tv_x ;
                c_y = ty*alpha  + tv_y ;
                c_z = tz*alpha  + tv_z ;

                error = sqrt( (ti_x-c_x)*(ti_x-c_x) + (ti_y-c_y)*(ti_y-c_y)
                                                    + (ti_z-c_z)*(ti_z-c_z) ) ;
                if(ip){
                  e_x = tx*trk_vector_length + tv_x ;
                  e_y = ty*trk_vector_length + tv_y ;
                  e_z = tz*trk_vector_length + tv_z ;

                  printf("=+==Straight track\n") ;
                  printf("    Length, Rotate  :  %8.2f %8.2f\n", trk_vector_length, trk_vect->ang) ;
                  printf("    Normal          :  %8.2f %8.2f %8.2f\n",nx, ny, nz) ;
                  printf("    Tangent         :  %8.2f %8.2f %8.2f\n",tx, ty, tz) ;
                  printf("----Closest at      :  %8.2f %8.2f %8.2f :: along track %8.2f ## mis-match %8.2f\n",
                                                    c_x, c_y, c_z, alpha, error);
                  printf("    Vector end at   :  %8.2f %8.2f %8.2f :: %8.2f\n",
                                                    e_x, e_y, e_z, trk_vector_length);
                }
              }
/*
 *  Curved Section
 */
            }else{
              track_radius = trk_vect->radius ;
/*
 *  Calculate bi-normal - unit vector to right of track
 *                        = unit tangent vector X unit normal vector
 */

              mstswagon2local(1., 0., 0., 0., 0., 0., aa, bb, cc, 1.,
                                        0., 0., 0., &rx, &ry, &rz) ;

              sa = (trk_vect->angle > 0.0) ? 1.0 : -1.0 ;
              o_x = tv_x + sa*rx*track_radius ;   //  Centre of arc
              o_y = tv_y + sa*ry*track_radius ;
              o_z = tv_z + sa*rz*track_radius ;
#if 0
              temp = nx*ti_x + ny*ti_y + nz*ti_z ;
              vx = ti_x - temp*nx - o_x ;   //  Unit vector from centre to track item
              vy = ti_y - temp*ny - o_y ;
              vz = ti_z - temp*nz - o_z ;
#else
              vx = ti_x - o_x ;   //  Unit vector from centre to track item
              vy = ti_y - o_y ;
              vz = ti_z - o_z ;
              temp = nx*vx + ny*vy + nz*vz ;
              vx = vx - temp*nx ;  //  After subtracting vertical component
              vy = vy - temp*ny ;
              vz = vz - temp*nz ;
#endif
              temp = 1.0/sqrt(vx*vx + vy*vy + vz*vz) ;
              vx = vx*temp ;        //  Normalise
              vy = vy*temp ;
              vz = vz*temp ;

              dott = vx*tx + vy*ty + vz*tz ;
              dotr = vx*rx + vy*ry + vz*rz ;
              dotr = -sa*dotr ;

              alpha = atan2(dott,dotr)    ;  //  Angle (radian) between vx and rx
              alpha1 = alpha ;
//              if(alpha < 0.0)alpha = alpha + pi ;
//              if(alpha >  0.5*pi)alpha = pi - alpha ;
              alpha2 = alpha ;
              alpha = track_radius*alpha  ;  //  Distance along track
              if((alpha>=-1.0 && alpha<=trk_vector_length+1.0) || ip){

                c_x = o_x + track_radius*vx ;
                c_y = o_y + track_radius*vy ;
                c_z = o_z + track_radius*vz ;

                error = sqrt( (ti_x-c_x)*(ti_x-c_x) + (ti_y-c_y)*(ti_y-c_y)
                                                    + (ti_z-c_z)*(ti_z-c_z) ) ;
                if(ip){
                  s = sin(trk_vect->angle) ;
                  c = cos(trk_vect->angle) ;
                  e_x = o_x - (c*rx - s*tx)*sa*track_radius ; // End position
                  e_y = o_y - (c*ry - s*ty)*sa*track_radius ;
                  e_z = o_z - (c*rz - s*tz)*sa*track_radius ;

                  printf("=+==Curved track\n") ;
                  printf("    Length, Rotate  :  %8.2f %8.2f\n", trk_vector_length, trk_vect->ang) ;
                  printf("    Radius, Angle   :  %8.2f %8.2f :: %8.2f %8.2f %8.2f\n",
                                                  trk_vect->radius, trk_vect->angle, c, s, sa) ;
                  printf("    Normal          :  %8.2f %8.2f %8.2f\n",nx, ny, nz) ;
                  printf("    Tangent         :  %8.2f %8.2f %8.2f\n",tx, ty, tz) ;
                  printf("    Bi-Normal       :  %8.2f %8.2f %8.2f\n",rx, ry, rz) ;
                  printf("    Centre of Arc   :  %8.2f %8.2f %8.2f\n",o_x,  o_y,  o_z)  ;
                  printf("    Track item      :  %8.2f %8.2f %8.2f\n",ti_x, ti_y, ti_z) ;
                  printf("    Centre to t item:  %8.2f %8.2f %8.2f\n",vx, vy, vz) ;
                  printf("    dott, dotr, atan:  %8.2f %8.2f %8.2f\n",dott, dotr, atan2(dott,dotr)) ;
                  printf("    alpha1, alpha2, pi :  %8.2f %8.2f %8.2f\n",alpha1, alpha2, pi) ;
                  printf("----Closest at      :  %8.2f %8.2f %8.2f :: along track %8.2f ## mis-match %8.2f\n",
                                                    c_x, c_y, c_z, alpha, error);
                  printf("    Vector end at   :  %8.2f %8.2f %8.2f :: %8.2f\n",
                                                    e_x, e_y, e_z, trk_vector_length);
                }
              }
            }
/*
 *  Save best match
 */
            if(ip)printf("    Update          :  %8.2f %8.2f :: %8.2f %8.2f :: %i\n",
                            alpha, trk_vector_length, error, last_error, found) ;
            if(alpha>= -1.0 && alpha<=trk_vector_length){
              if(error<last_error){
                trk_item->sect_distance = alpha + trk_vect->distance0 ;
                last_error              = error ;
                found                   = 1     ;
                if(ip && 0){
                  printf("    Update success :  %f  %f  :: %f  :: %f\n",
                                   alpha, trk_vect->distance0, trk_item->sect_distance, last_error) ;
                  printf("    ==============\n") ;
                }
              }
            }
          }     //  Loop over track vectors
          if(!found){
            printf("\n  Routine %s error.",my_name) ;
            printf("  Unable to find vector containing track item\n") ;
            printf("  Track section : position %i, uid %i,  number of Items %i\n",
                               i,trk_sect->uid,trk_sect->trk_item_number ) ;
            printf("    Items in track section\n") ;
            for(j=0;j<trk_sect->trk_item_number;j++){
              printf("    j = %i, trk_item_list[j] = %i\n",j,trk_sect->trk_item_list[j] ) ;
            }
            printf("\n") ;
            printf("   Track Item    : %i  %i  %s\n",trk_item_index,
                   trk_item->type_of_node, token_trackdb[trk_item->type_of_node]) ;
            printf("   Tile origin at: %i %i %8.2f\n",tile_x0, tile_y0, tile_h0) ;
            printf("   Track Item at:  %i %i :: %8.2f %8.2f %8.2f\n",
                                   trk_item->tile_east_x, trk_item->tile_north_z,
                                   trk_item->east_x, trk_item->north_z, trk_item->height_y) ;
            printf("   Track Item at:  %8.2f %8.2f %8.2f\n",ti_x, ti_y, ti_z) ;
            trk_item->sect_distance = 0.0 ;
          }
          if(ip && found){
            printf("  Routine %s : track section %i, track item %i, distance %8.2f, error %8.2f\n\n",
                   my_name, i, trk_item_index, trk_item->sect_distance, last_error) ;
          }
        }       //  Loop over track items
/*
 *  Sort trk_item_list by distance along track section
 *  Then save each track item's own position in trk_item_index
 */

        if(ip){
          printf(" ++++++  trk_sect        = %p\n",(void *) trk_sect) ;
          printf(" ++++++  trk_item_number = %i\n",trk_sect->trk_item_number) ;
        }

        if(trk_sect->trk_item_number){
          if(ip){
            printf(" AA  trk_item_list = ") ;
            for(j=0;j<(trk_sect->trk_item_number);j++){
              printf("  %i", trk_sect->trk_item_list[j]) ;
            }
            printf("\n") ;
          }
          qsort(trk_sect->trk_item_list, trk_sect->trk_item_number, sizeof(int),cmp_dist) ;
          if(ip){
            printf(" BB  trk_item_list = ") ;
            for(j=0;j<(trk_sect->trk_item_number);j++){
              printf("  %i", trk_sect->trk_item_list[j]) ;
            }
            printf("\n") ;
          }
          if(ip)printf(" CAC trk_item_list = %p\n",(void *)trk_sect->trk_item_list) ;
          for(j=0;j<(trk_sect->trk_item_number);j++){
//           printf(" ++++++  j = %i\n", j) ;
            k = trk_sect->trk_item_list[j] ;
            track_db.trk_items_array[k].trk_item_index = j ;
          }
        }
      }         //  Loop over track sections

      return 0 ;
}
/*
 *   Comparion function
 */
static int cmp_dist(const void *p1, const void *p2){

int     i1, i2 ;
double  d1, d2 ;

      i1 = *(int *) p1 ;
      i2 = *(int *) p2 ;
      d1 = track_db.trk_items_array[i1].sect_distance ;
      d2 = track_db.trk_items_array[i2].sect_distance ;
      if(d1>d2) return 1 ;
      if(d2>d1) return -1 ;
      return 0 ;
}

/*
 *  Routine to find potential world item matches when one corresponding
 *  to a section of track cannot be found.
 *    uid0 is the uid of target world item
 *    ix0, iy0 identify the tile where the item was not found.
 *  Assume success if solution found within 1 tile.
 */
#if 0
static int find_potential_world_items(TrkSector *tn0, int uid0,
                                       int *ix0, int *iy0, int *ifound0){
int   ifound = 0 ;
int       ix, iy ;
WorldNode *wnode ;
WorldItem *witem ;


char * my_name = "find_potential_world_items" ;

      for(wnode = worldlist_beg; wnode != NULL; wnode=wnode->next){
        ix = wnode->tile_x ;
        iy = wnode->tile_y ;
        for(witem = wnode->world_item; witem != NULL; witem=witem->next){
          if(witem->uid != uid0)continue ;
          if(0 == ifound){
            printf("    Sector type = %s, Item = %i, Original tile = %i, %i.  Potential matches:\n",
                   token_trackdb[tn0->type_of_node], uid0, *ix0, *iy0) ;
            ifound = 1 ;
          }
          if(ix>*ix0-2 && ix<*ix0+2 && iy>*iy0-2 && iy<*iy0+2){
            printf("  **  ") ;
            *ifound0 = 1     ;
          }else{
            printf("      ") ;
          }
          printf("Tile = %i %i, uid = %i, X, Y, Z = %7.2f %7.2f %7.2f, type = %i, %s\n",
                 ix, iy, uid0, witem->X, witem->Y, witem->Z,
                 witem->worldtype, token_idc[witem->worldtype]) ;
          if(1 == *ifound0){
            printf("****  Missing world item found\n") ;
            return 0 ;                                        //  Success
          }
        }
      }
      printf("####  Missing world item not found\n") ;
      return 0 ;
}
#endif
#if 0

/*
 *  Find distance along vector *i_vec of track section *index
 *                        to the point closest to the track item 'id'
 */
      item = &(database->trk_items_array[id]) ;
/*
 *  Calculate location of track item - metres relative to tile origin
 */
      global2local(tile_x0, tile_y0, tile_h0, tile_size, 1.0,
                         item->tile_east_x, item->tile_north_z,
                         item->east_x, item->north_z, item->height_y,
                         &ti_x, &ti_y, &ti_z) ;
      if(ip){
        printf("   Tile origin at: %i %i %f\n",tile_x0, tile_y0, tile_h0) ;
        printf("   Track Item at:  %f %f %f\n",ti_x, ti_y, ti_z) ;
      }
/*
 *  Loop over section
 *  NOTE:  Track section with index "n" is specified in
 *         trk_sections_array[n-1];
 */

      sect = &(database->trk_sections_array[*section_index-1]) ;
      length_of_vector = sect->length_of_vector ;
      vect = sect->vector ;
      found = 0 ;
      for(i=0;i<length_of_vector;i++){

/*
 *  Calculate origin of vector
 */
        global2local(tile_x0, tile_y0, tile_h0, tile_size, 1.0,
                         vect->tile_east_x, vect->tile_north_z,
                         vect->east_x, vect->north_z, vect->height_y,
                         &tv_x, &tv_y, &tv_z) ;
        if(ip)printf("    Vector origin at:  %f %f %f :: %i :: %f %f %f :: %f\n",
                                   tv_x, tv_y, tv_z,vect->is_curved,
                                   vect->ax,vect->ay,vect->az,vect->ang);

        trk_vector_length = vect->length ;
        aa = degree*vect->a_east_x ;
        bb = degree*vect->a_height_y ;
        cc = degree*vect->a_north_z ;
/*
 * Calculate unit normal and unit tangent
 */
        mstswagon2local(0., 1., 0., 0., 0., 0., aa, bb, cc, 1.,
                                    0., 0., 0., &nx, &ny, &nz) ;
        mstswagon2local(0., 0., 1., 0., 0., 0., aa, bb, cc, 1.,
                                    0., 0., 0., &tx, &ty, &tz) ;
/*
 *  Straight section
 */
        if(!(vect->is_curved)){
          alpha = (ti_x-tv_x)*tx + (ti_y-tv_y)*ty + (ti_z-tv_z)*tz ;
          if((alpha>=0.0 && alpha<=trk_vector_length) || ip){

            c_x = tx*alpha  + tv_x ;
            c_y = ty*alpha  + tv_y ;
            c_z = tz*alpha  + tv_z ;

            error = sqrt( (ti_x-c_x)*(ti_x-c_x) + (ti_y-c_y)*(ti_y-c_y)
                                                + (ti_z-c_z)*(ti_z-c_z) ) ;
            if(ip){
              e_x = tx*trk_vector_length + tv_x ;
              e_y = ty*trk_vector_length + tv_y ;
              e_z = tz*trk_vector_length + tv_z ;

              printf("    Straight track\n") ;
              printf("    Length, Rotate  :  %f %f\n", trk_vector_length, vect->ang) ;
              printf("    Normal          :  %f %f %f\n",nx, ny, nz) ;
              printf("    Tangent         :  %f %f %f\n",tx, ty, tz) ;
              printf("    Closest at      :  %f %f %f :: %f %f\n",
                                                c_x, c_y, c_z, alpha, error);
              printf("    Vector end at   :  %f %f %f :: %f\n",
                                                e_x, e_y, e_z, trk_vector_length);
            }
          }
/*
 *  Curved Section
 */
        }else{
          track_radius = vect->radius ;
/*
 *  Calculate bi-normal - unit vector to right of track
 *                        = unit tangent vector X unit normal vector
 */

          mstswagon2local(1., 0., 0., 0., 0., 0., aa, bb, cc, 1.,
                                    0., 0., 0., &rx, &ry, &rz) ;

          sa = (vect->angle > 0.0) ? 1.0 : -1.0 ;
          o_x = tv_x + sa*rx*track_radius ;   //  Centre of arc
          o_y = tv_y + sa*ry*track_radius ;
          o_z = tv_z + sa*rz*track_radius ;

          temp = nx*ti_x + ny*ti_y + nz*ti_z ;
          vx = ti_x - temp*nx - o_x ;
          vy = ti_y - temp*ny - o_y ;
          vz = ti_z - temp*nz - o_z ;
          temp = 1.0/sqrt(vx*vx + vy*vy + vz*vz) ;
          vx = vx*temp ;             //  Unit vector from centre to track item
          vy = vy*temp ;
          vz = vz*temp ;

          dott = vx*tx + vy*ty + vz*tz ;
          dotr = vx*rx + vy*ry + vz*rz ;

          alpha = atan2(dott,dotr)    ;  //  Angle (radian) between vx and rx
          if(alpha > 0.5*pi)alpha = pi - alpha ;
          alpha = alpha*sa ;
          alpha = track_radius*alpha  ;  //  Distance along track
          if((alpha>=0.0 && alpha<=trk_vector_length) || ip){

            c_x = o_x + track_radius*vx ;
            c_y = o_y + track_radius*vy ;
            c_z = o_z + track_radius*vz ;

            error = sqrt( (ti_x-c_x)*(ti_x-c_x) + (ti_y-c_y)*(ti_y-c_y)
                                                + (ti_z-c_z)*(ti_z-c_z) ) ;
            if(ip){
              s = sin(vect->angle) ;
              c = cos(vect->angle) ;
              e_x = o_x - (c*rx - s*tx)*sa*track_radius ; // End position
              e_y = o_y - (c*ry - s*ty)*sa*track_radius ;
              e_z = o_z - (c*rz - s*tz)*sa*track_radius ;

              printf("=+==Curved track\n") ;
              printf("    Length, Rotate  :  %f %f\n", trk_vector_length, vect->ang) ;
              printf("    Radius, Angle   :  %f %f :: %f %f %f\n",
                                              vect->radius, vect->angle, c, s, sa) ;
              printf("    Normal          :  %f %f %f\n",nx, ny, nz) ;
              printf("    Tangent         :  %f %f %f\n",tx, ty, tz) ;
              printf("    Bi-Normal       :  %f %f %f\n",rx, ry, rz) ;
              printf("    Centre of Arc   :  %f %f %f\n",o_x,  o_y,  o_z)  ;
              printf("    Track item      :  %f %f %f\n",ti_x, ti_y, ti_z) ;
              printf("    Centre to t item:  %f %f %f\n",vx, vy, vz) ;
              printf("    dott, dotr, atan:  %f %f %f\n",dott, dotr, atan2(dott,dotr)) ;
              printf("    Closest at      :  %f %f %f :: %f %f\n",
                                                c_x, c_y, c_z, alpha, error);
              printf("    Vector end at   :  %f %f %f :: %f\n",
                                                e_x, e_y, e_z, trk_vector_length);
            }
          }
        }
/*
 *  Save best match
 */
        if(ip)printf("    Update          :  %f %f :: %f %f :: %i\n",
                         alpha, trk_vector_length, error, last_error, found) ;
        if(alpha>=0.0 && alpha<=trk_vector_length){
          if(error<last_error){
            *vector_index = i  ;
            *distance  = alpha ;
            last_error = error ;
            found = 1          ;
          }
        }
        vect++ ;       //  Move to next vector
      }
      if(!found){
        printf("  Routine %s error:",my_name) ;
        printf("  Unable to find vector containing track item\n") ;
      }
      if(ip)printf("   Track Item %i found at section %i, vector %i\n",
                           id,*section_index,*vector_index);
#endif



#include "junction.c"
