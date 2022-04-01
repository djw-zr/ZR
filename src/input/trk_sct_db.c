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
int  add_distances_to_track_vectors(TrkVectorNode *v, TrkSectNode *t, int n) ;

int init_track_db(char * filename){
int    i, j, ip = 0 ;
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
      datafile = (char *)malloc(strlen(ORroutedir)+strlen(filename)+1);
      strcpy(datafile,ORroutedir);
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
      track_db.trk_sections_array = (TrkSectNode *)
              malloc(track_db.trk_sections_array_size*sizeof(TrkSectNode));
/*
 *   Cycle over the set of track sections
 */
      for(i=0;i<(int)track_db.trk_sections_array_size;i++){
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
      track_db.trk_items_array = (TrkItemNode *)
                  malloc(track_db.trk_items_array_size*sizeof(TrkItemNode));
/*
 *   Cycle over the track items
 */
      for(i=0;i<(int)track_db.trk_items_array_size;i++){
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
TrkSectNode   *trk_path_node ;
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

      free(datafile) ;
      close_msfile(&msfile) ;
      if(ip)printf("  Exit  routine %s\n",myname) ;

      return 0;
}

/*
 * ***********************************************************************
 *
 *   Routines to read and store information from one TrackNode block
 *   incuding the start 'TrackNode (' and end ')' marks.
 *   Tracknodes are usually in *.trk files
 *
 * **********************************************************************
 */

void read_track_path(TrkSectNode *trk_path, MSfile *msfile){

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
      trk_path->index_of_node = itoken(msfile) ;
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
              trk_path->jn[1]=itoken(msfile) ;
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
                  }
                  skip_rbr(msfile) ;
                  break;
                CASE("TrItemRefs")
                  skip_lbr(msfile);
                  n = itoken(msfile);                  // Number of Track Items
                  trk_path->trk_item_number = n;
                  trk_path->trk_item_vec = (uint *)malloc(n*sizeof(uint));
                  for(j=0;j<n;j++){
                      token = new_tmp_token(msfile);  //  TrItemRef
                      if(is_rbr(token)) break ;
                      skip_lbr(msfile);
                      trk_path->trk_item_vec[j] = itoken(msfile) ;
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
                trk_path->pin_to_section[j] = itoken(msfile) ;
                trk_path->pin_info[j]       = itoken(msfile) ;
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

void init_track_path(TrkSectNode *track_path){
int  i;

      track_path->index_of_node    = 0;
      track_path->type_of_node     = 0;
      track_path->length_of_vector = 0 ;

      for(i=1;i<3;i++){
        track_path->type_of_pin[0]    = 0 ;
        track_path->pin_to_section[i] = 0 ;
        track_path->pin_info[i]       = 0 ;
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

int list_track_section(TrkSectNode *tnode){

uint          i      ;
TrkVectorNode *vnode ;
WorldNode     *wn    ;
WorldItem     *wi    ;
      printf("  Track section.  Index = %i\n",tnode->index_of_node);
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
      printf("  length of vector = %i\n",tnode->length_of_vector) ;
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

        if(vnode->tsec_section_index != 0){
          print_tsec_section( vnode->tsec_section_index) ;
        }
        if(vnode->tsec_shape_index != 0){
          print_tsec_shape( vnode->tsec_shape_index) ;
        }
        if(vnode->worldfileuid != 0){
          for(wn=worldlist_beg;wn!=NULL;wn=wn->next){
            if(wn->tile_x != vnode->wfname_east_x || wn->tile_y != vnode->wfname_north_z) continue ;
            for(wi=wn->world_item;wi!=NULL;wi=wi->next){
              if(wi->uid != (int)vnode->worldfileuid)continue ;
              list_wfile_item(wi) ;
            }
          }
        }
        printf("\n") ;
      }
      return 0 ;
}

int add_world_item_pointers_to_track_vectors(TrkSectNode *tnnode) {

int            ip = 0 ;  // Debug
int            iret = 0 ;
uint           nvec ;
int            wtile_x, wtile_y ;
int           w_uid ;
TrkVectorNode  *vec ;
WorldNode      *wnode  ;
WorldItem      *witem  ;
char           my_name[] = "add_world_item_pointers_to_track_vectors" ;
/*
 *  Loop over track vectors
 */
      if(ip)printf(" Enter %s, track node = %i %i %i %p\n",my_name,
                          tnnode->index_of_node,tnnode->type_of_node,
                          tnnode->length_of_vector,(void *)tnnode) ;
      for(nvec=0;nvec<tnnode->length_of_vector;nvec++){
        vec = &(tnnode->vector[nvec]) ;
/*
 *  For each vector find world Node
 */
        wtile_x = vec->wfname_east_x  ;
        wtile_y = vec->wfname_north_z ;
        w_uid   = vec->worldfileuid   ;
        for(wnode=worldlist_beg;wnode!=NULL;wnode=wnode->next){
          if(wnode->tile_x == wtile_x && wnode->tile_y == wtile_y)break ;
        }
        fflush(NULL) ;
        if(wnode->tile_x != wtile_x || wnode->tile_y != wtile_y){
          printf("  ERROR. Routine %s unable to find world node for %i %i\n",
                                my_name,wtile_x,wtile_y) ;
          exit(0) ;
        }
/*
 *  ... and then world item
 */
        fflush(NULL) ;
        for(witem=wnode->world_item;witem!=NULL;witem=witem->next){
          if(witem->uid == w_uid)break ;
        }
        if(witem->uid != w_uid){
          printf("  ERROR. Routine %s unable to find world item %i\n",
                                my_name,w_uid) ;
          exit(0) ;
        }
/*
 *  Save pointer to world item
 */
        vec->world_item = witem ;
        vec->ax         =witem->AX ;
        vec->ay         =witem->AZ ;  //  Axes ...
        vec->az         =witem->AY ;  //       ... swapped
        vec->ang        =witem->ANG ;
/*
 *  Save pointer to vector
 */
        if(witem->worldtype == TRACKOBJ || witem->worldtype == 306){
          witem->u.track_obj.track_vec = vec ;
          witem->iz_off = 1                  ;  // All track items set to 1
        }else{
          printf(" Possible error.  Track vector %i found world item %i\n", nvec, witem->worldtype);
        }

        if(iret)continue ;
        if(tnnode->type_of_node == VECTOR_SECTION){
          iret = add_distances_to_track_vectors(vec,tnnode,nvec) ;
        }
      }
      return 0 ;
}

/*
 *  Add track length, radius and angle to each track_vector_node.
 */

int  add_distances_to_track_vectors(TrkVectorNode *v, TrkSectNode *t, int nvec){

int  ip = 0 ;
int  k, kk ;
int  flag1  = v->flag1 ;
int  flag2  = v->flag2 ;
int  is_normal = v->world_item->worldtype != 306 ;
uint ns     = v->tsec_section_index ;

TrackSection   *ts ;
WorldItem      *w=v->world_item ;
DynTrackObj    *dto ;

static int ndt = 0 ;  //  Number of dynamic track sections to process
static int idt = 0 ;  //  Current dynamic track section being processed
static WorldItem  *ws = NULL ;
char my_name[] = "add_distances_to_track_vectors" ;

      k = 0 ;

      if(ip)printf("  Add distances :  track node = %4i, vector = %i, flags = %i %i, is_normal = %i, ns = %i, ndt = %i, idt = %i, w = %p, ws = %p\n",t->index_of_node,nvec,flag1,flag2,is_normal,ns,ndt,idt,(void *)w,(void *)ws) ;
//      printf("  worldtype = %i\n",v->world_item->worldtype) ;
      if(is_normal){
        if(ndt != 0){
          printf("  ERROR in %s.  Normal track found while processing dynamic track\n",my_name);
          exit(0) ;
        }
        for(ts = track_section_beg; ts != NULL; ts = ts->next){
          if(ts->index == ns)break ;
        }
        if(ts->index != ns){
          printf("  ERROR in %s.  Cannot find track section %i\n",my_name,ns);
          exit(0) ;
        }
/*
 *d  Case flag1 ==2 and flag2 == 0 comes from zig-zag track section 565
 */
#if 1
        v->length = ts->length ;
        v->radius = ts->radius ;
        v->angle  = ts->angle  ;
        if(ip && !( (0 == flag1 && 1 == flag2) || (0 == flag1 && 1 == flag2))){
          printf(" Routine %s.  Unusual sector:\n",my_name) ;
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
          if(ip || 0)printf(" Routine %s.  Unusual sector:\n",my_name) ;
        }
#endif
        v->is_dynamic = 0 ;
        v->is_curved = (v->angle != 0.0) ;
        if(ip)printf("  NORMAL SECTION :: length = %f, radius = %f, angle = %f\n",
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
          printf("  ERROR in %s.  New world file while processing dynamic sections\n",my_name);
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
          printf(" Routine %s.  Unusual dynamic track sector:\n",my_name) ;
          printf("  Flag1 = %i, Flag2 = %i, Param_1 = %f, Param_2 = %f\n",
                    flag1, flag2, dto->dyn_trk_sect[k].param_1,  dto->dyn_trk_sect[k].param_2) ;
          return 1 ;
        }
        if(ndt>0)ndt = ndt - 1;
        idt = idt + 1 ;
        if(ip)printf("                      v = %p, Flag1 = %i, Flag2 = %i, Length = %f, Radius = %f, Angle = %f\n",
                  (void *)v, flag1, flag2, v->length, v->radius, v->angle) ;
      }

      if(v->length == 0.0){
        printf("  %s, LENGTH ERROR :  Track Index = %4i, Type = %i,"
               " vector = %3i, is_normal = %i\n",
               my_name, t->index_of_node, t->type_of_node, nvec, is_normal) ;
      }
      return 0;
}

#include "junction.c"
