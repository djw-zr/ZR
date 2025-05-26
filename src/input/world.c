/*
 *******************************************************************************
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File : world.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to handle the loading of binary world data
 *
 *   Once uncompressed the MSTS binary files consist of a series of well
 *   defined blocks.  Each block starts with a 16 byte header made up of:
 *     2-byte unsigned integer  :  enum corresponding to the name of the block,
 *     2-byte unsigned integer  ;  flags,
 *     4-byte unsigned integer  :  length of the remainder of the block.
 *   The remainder of the block is made up of data and sub-blocks with the same
 *   type of header.
 *
 *   The 'length' is usually correct but not always.  However the order of
 *   block names appears to be fixed.  This version of the file therefor
 *   assumes the order is fixed and only uses the block length for the few
 *   cases where the total amount of data in a block is not otherwise clear.
 *
 *******************************************************************************
 */
#include <dirent.h>
#include <stdio.h>
int world_item_init(WorldItem *world_item) ;
void  add_shape_name_to_world_btree(WorldNode *wnode, WorldItem *world_item);


/*
 * *****************************************************************************
 * Routine to create a world node for each world file in directory world
 * *****************************************************************************
 */

int load_world_filenames() {

  int      len1, len2, iret ;
  int      ip = 0 ;                // Control printing : 0 = no printing
  char     *wdir_name ;
  DIR      *wdir;
  char     string7[7] ;
  struct dirent *f_entry;
  WorldNode *world ;
  char      *world1_dir = "/world/" ;
  char      *world2_dir = "/World/" ;

      l_ip = 0 ;

/*
 *  1.  Search for world directory (world, WORLD)
 */
      len1 = strlen(ORroutedir) + strlen(world1_dir) + 1 ;
      wdir_name = (char *)malloc(len1) ;
      strcpy(wdir_name,ORroutedir)     ;
      strcat(wdir_name,world1_dir)     ;
      if(ip)printf(" Trying directory world = %s\n",wdir_name) ;
#if 1
      iret = zr_find_msfile2(&wdir_name);
      if(iret){
        printf(" Unable to find world directory\n") ;
        printf(" Program stopping ... \n") ;
        exit(1) ;
      }
      wdir = opendir(wdir_name) ;
      if(wdir == NULL){
        printf(" Unable to find world directory\n") ;
        printf(" Program stopping ... \n") ;
        exit(1) ;
      }
#else
      wdir = opendir(wdir_name) ;
      if(wdir == NULL){
        strcpy(wdir_name,ORroutedir)    ;
        strcat(wdir_name,world2_dir)      ;
        if(ip)printf(" Trying directory world = %s\n",wdir_name) ;
        wdir = opendir(wdir_name) ;
        if(wdir == NULL){
          printf(" Unable to find world directory\n") ;
          printf(" Program stopping ... \n") ;
          exit(1) ;
        }
      }
#endif
 /*
  *  2.  Loop through files
  */
      while ((f_entry = readdir(wdir)) != NULL) {
        len2 = strlen(f_entry->d_name) ;
        if(len2 != 17 || strcmp(".w",&(f_entry->d_name[15]))!= 0) continue ;
        if(ip)printf("  Found world file = %s\n", f_entry->d_name);
/*
 * Initialise new worldnode
 */
        world = (WorldNode *)malloc(sizeof(WorldNode)) ;
        if(worldlist_beg == NULL){
          worldlist_beg = world       ;
        }else{
          worldlist_end->next = world ;
        }
        worldlist_end = world ;
        world->next   = NULL ;
/*
 *  Save name and filename
 */
        world->name = (char *)malloc(len2-1) ;
        strncpy(world->name,f_entry->d_name,len2-2);
        world->name[len2-2] = '\0' ;
/*
 *  Save full name of world file
 */
        world->wfile = (char *)malloc(len1 + len2 ) ;
        strcpy(world->wfile,wdir_name) ;
        strcat(world->wfile,f_entry->d_name);
/*
 *  Save tile location
 */
        strncpy(string7,&(world->name[2]),6);
        string7[6] = '\0' ;
        world->tile_x = atoi(string7) ;
        if(world->name[1] == '-')world->tile_x = -world->tile_x ;
        strncpy(string7,&(world->name[9]),6);
        string7[6] = '\0' ;
        world->tile_y = atoi(string7) ;
        if(world->name[8] == '-')world->tile_y = -world->tile_y ;
        if(ip){
          printf(" +++++++++++++++++++++++++++++++++++++++\n") ;
          printf("  Tile_X = %i\n",world->tile_x);
          printf("  Tile_Z = %i\n",world->tile_y);
          printf(" +++++++++++++++++++++++++++++++++++++++\n") ;
        }
/*
 *  Initialise other structure variables
 */
        world->tr_watermark = 0 ;
        world->vdbid_count  = 0 ;
        world->vdb_sphere   = NULL ;
        world->world_item   = NULL ;
        world->sound_source = NULL ;
        world->sound_region = NULL ;
        world->shape_tree   = NULL ;
      }
      closedir(wdir) ;
      free(wdir_name) ;

      return 0;
}
/*
 * =============================================================================
 *   Routine to read the world (*.w) files and populate the world database.
 * =============================================================================
 */

int  load_world(WorldNode *wnode){

  int  i, l, iret ;
  int  istoken    ;
  int  is_binary  ;
  enum token_id itoken, itoken2, itoken3, itoken4 ;
  int     ip1 = 0, ip = 0 ;                  //Controls printing
  double  temp ;

  MSfile  msfile0 ;
  MSfile  *msfile = &msfile0 ;
  MSblock *msblock1, *msblock2, *msblock3, *msblock4 ;
  FILE    *fp ;
  char    *ext ;
  char    *tmp_name   ;
  char    *parent_dir ;
  char    *full_parent_dir ;
  char    my_name[] = "load_world" ;


//      ip1 = wnode->tile_x == -6074 && wnode->tile_y == 14922 ;
//      ip1 = wnode->tile_x == 1611 && wnode->tile_y == 10582 ;
      ip = ip1 ;

      if(ip1)printf("\n  Enter routine : %s\n",my_name);

/*
 * =============================================================================
 *  open_msfile reads and checks the first 16 bytes of the file, inflates
 *  compressed files, reads the next 16 bytes and sets the flags.
 * =============================================================================
 */
      l = open_msfile(wnode->wfile, msfile, 0, 0) ;
      if(ip)printf(" Routine %s, file %s, l = %i\n",my_name,wnode->wfile,l);
      if(l!=0){
        printf("\n\n  ERROR : Routine open_msfile failed to open file\n");
        printf(    "  File  = %s\n", wnode->wfile);
        exit(1) ;
      }
      parent_dir = zr_parentdir(wnode->wfile) ;  // Must be freed
      full_parent_dir = zr_full_parentdir(wnode->wfile) ;  // Must be freed

      if(ip){
        printf("        wfile      = %s\n",wnode->wfile) ;
        printf("        parent_dir = %s\n",parent_dir) ;
        if(msfile->text){
          printf("  World file is text.  Filename = %s\n",wnode->wfile) ;
        }
      }
/*
 *  Filename plus flags for utf16le/utf8, binary/text and compressed
 *  This routine has to use the token_bc.c routines to process both
 *  binary and text files.
 */

      if(ip || ip1){
        printf("  File = %c%c%c %s\n",
                  (msfile->utf16le  ? 'u' : 'a'),
                  (msfile->binary   ? 'b' : 't'),
                  (msfile->compress ? 'c' : ' '),wnode->wfile);
      }

      is_binary = msfile->binary ;
      fp = msfile->fp ;
      init_levels(msfile) ;
/*
 * =============================================================================
 *  read blocks
 * =============================================================================
 */
      if(ip)printf("  At start of file ftell = %i\n",(int) ftell(fp));
/*
 *  Level 0 : TR_WORLDFILE
 */
      l = open_named_block(msfile, 0, TR_WORLDFILE) ;
      if(0!=l){
        close_msfile(msfile) ;
        free(parent_dir) ;
        free(full_parent_dir) ;
        return 1;
      }
      if(ip)printf(" Level 0 : token TR_WORLDFILE found \n");
/*
 *   Level 1
 */
      for(;;){
        istoken = open_block(msfile,1) ;
        if(-1==istoken){
          if(ip)printf(" Routine %s.  End of file found at level 1\n",my_name);
          close_msfile(msfile) ;
          free(parent_dir) ;
          free(full_parent_dir) ;
          if(ip)printf(" Routine %s.  At normal exit/return\n",my_name);
          return 1;
        }
        if(istoken == -2){
          if(ip)printf("  Level 1 : world.c : OpenBlock returned -2\n") ;
          break ;
        }
        msblock1 = &(msfile->level[1])  ;
        itoken  = istoken ;

//        ip = (itoken == LEVELCR) ;
//        ip1 = wnode->tile_x == -6158 && wnode->tile_y == 14886 ;

        if(ip || ip1){
          printf("\n  Level 1 : token = %i  %s  : block data length = %i \n",
           itoken,token_idc[itoken], msblock1->length-msblock1->l_label-9) ;
          printf(" +++++++++++++++++++++++++++++++++++++++\n") ;
          printf("  Tile_X = %i\n",wnode->tile_x);
          printf("  Tile_Z = %i\n",wnode->tile_y);
          printf("      ip = %i\n",ip) ;
          printf(" +++++++++++++++++++++++++++++++++++++++\n") ;
      }

        if(itoken == VDBIDCOUNT){
          wnode->vdbid_count = read_uint32_a(msfile) ;
          if(ip)printf("    vdbid_count = %i\n",wnode->vdbid_count) ;
          wnode->vdb_sphere = (VDbSphere *)malloc(wnode->vdbid_count*
                                            sizeof(VDbSphere)) ;
          end_block(msfile,1) ;

  uint    id, lev ;
  MSblock *msblock ;
          lev = 1;

          for(i=0;i<(int)wnode->vdbid_count;i++){
            if(ip)printf("\n") ;
            if(ip)printf("    Open named block : 1 : lev = %i, current = %i\n",
                   lev, current_block_level) ;
            iret = open_named_block(msfile,lev,VIEWDBSPHERE) ;
            itoken = VIEWDBSPHERE ;
            msblock = &(msfile->level[lev])  ;
            if(ip)printf("  i = %i, Level = %i : token = %i  %s  : block data length = %i \n",
            i, lev, itoken,token_idc[itoken], msblock->length-msblock->l_label-9) ;

            lev ++ ;

            if(ip)printf("    Open named block : 2 : lev = %i, current = %i\n",
                   lev, current_block_level) ;
            iret = open_named_block(msfile,lev,VDBID) ;
            itoken = VDBID ;
            msblock = &(msfile->level[lev])  ;
            if(ip)printf("  Level 3 : token = %i  %s  : block data length = %i \n",
            itoken,token_idc[itoken], msblock->length-msblock->l_label-9) ;
            id = read_uint32_a(msfile) ;
            wnode->vdb_sphere[id].vdb_id = id ;
            if(ip)printf("              i = %i, vdb_id   =  %i\n",
                                        i, wnode->vdb_sphere[id].vdb_id);
            end_block(msfile,lev) ;

            if(ip)printf("    Open named block : 3 : lev = %i, current = %i\n",
                   lev, current_block_level) ;
            iret = open_named_block(msfile,lev,POSITION) ;
            itoken = POSITION ;
            msblock3 = &(msfile->level[lev])  ;
            if(ip)printf("  Level %i : token = %i  %s  : block data length = %i \n",
            lev, itoken,token_idc[itoken], msblock3->length-msblock3->l_label-9) ;
            wnode->vdb_sphere[id].Y = read_real32_a(msfile) ;  // MSTS X
            wnode->vdb_sphere[id].Z = read_real32_a(msfile) ;  // MSTS Y
            wnode->vdb_sphere[id].X = read_real32_a(msfile) ;  // MSTS Z
            if(ip)printf("           id, X, Y, Z = %i : %f %f %f\n",
              id,wnode->vdb_sphere[id].X,
              wnode->vdb_sphere[id].Y, wnode->vdb_sphere[id].Z) ;
            end_block(msfile,lev) ;

            if(ip)printf("    Open named block : 4 : lev = %i, current = %i\n",
                   lev, current_block_level) ;
            itoken   = open_named_block(msfile,lev,RADIUS) ;
            itoken = RADIUS ;
            msblock2 = &(msfile->level[lev])  ;
            if(ip)printf("  Level %i : token = %i  %s  : block data length = %i \n",
            lev, itoken, token_idc[itoken], msblock3->length-msblock3->l_label-9) ;
            wnode->vdb_sphere[id].radius = read_real32_a(msfile) ;
            if(ip)printf("              id, radius   = %i : %f\n",
                                             id,wnode->vdb_sphere[id].radius);
            if(ip)printf("    Call end_block 1  : lev = %i, current = %i\n",
                   lev, current_block_level) ;
            end_block(msfile,lev) ;
            lev-- ;

            if(ip)printf("    Call is_end_block : lev = %i, current = %i\n",
                   lev, current_block_level) ;
            if(is_block_end(msfile,lev,0)){
              if(ip)printf("    Call end_block 2  : lev = %i, current = %i\n",
                   lev, current_block_level) ;
              if(lev>1)end_block(msfile,lev) ;
              if(ip)printf("    Return from end_block 2  : lev = %i, current = %i\n",
                   lev, current_block_level) ;
              while(lev>2 && is_block_end(msfile,lev-1,0)){
                if(ip)printf("    Call end_block 3  : lev = %i, current = %i\n",
                    lev, current_block_level) ;
                lev-- ;
                if(lev>1)end_block(msfile,lev) ;
                if(ip)printf("    Return from end_block 3  : lev = %i, current = %i\n",
                    lev, current_block_level) ;
              }
            }else{
              if(ip)printf("    End : 1 : lev = %i, current = %i\n",
                   lev, current_block_level) ;
              lev++ ;
              if(ip)printf("    End : 2 : lev = %i, current = %i\n",
                   lev, current_block_level) ;
            }
          }
          if(ip)printf("    End : 3 : lev = %i, current = %i\n",
                   lev, current_block_level) ;
//          ip = 0 ;
          while(current_block_level>1)end_block(msfile,current_block_level) ;
        }else if(TR_WATERMARK == itoken){
          wnode->tr_watermark = read_int32_a(msfile) ;
/*
 *  Unknown token
 *  301 :  Has contained Copyright comments.  (in Tweed River route)
 */
        }else if(309 == itoken || 300 == itoken || 301 == itoken){
          skip_to_bblock_end_a(msfile,msblock1) ;

/*
 * ***********************************************************************
 *
 *  Level 1 tokens with level 2 subtokens
 *
 * ***********************************************************************
 */
        }else if(STATIC == itoken || COLLIDEOBJECT == itoken
                || TRACKOBJ == itoken
                || itoken == CARSPAWNER || itoken == 357
                || itoken == SIDING_ALT || itoken == 361 || itoken == SIDING2
                || itoken == PLATFORM_ALT || itoken == FOREST
                || itoken == LEVELCR
                || itoken == DYNTRACK   || itoken == 306
                || itoken == TRANSFER   || itoken == 363
                || itoken == GANTRY     || itoken == 356
                || itoken == PICKUP_ALT || itoken == 359 || PICKUP2 == itoken
                || itoken == HAZARD     || itoken == SIGNAL_ALT
                || itoken == SPEEDPOST ){
/*
 *  Crate new world item
 */
  WorldItem *world_item ;
          if(itoken == PICKUP_ALT || itoken == SIDING_ALT ||
             itoken == PICKUP2    || itoken == SIDING2){
            if(ip)printf(" ################ PICKUP_ALT or SIDING_ALT #### %i ###########\n",itoken);
          }
          if(ip)printf("  Create a new world item\n") ;
          world_item = (WorldItem *)malloc(sizeof(WorldItem));
          world_item_init(world_item) ;
          if(ip)printf("  World item created\n") ;

          if(NULL == wnode->world_item){
            wnode->world_item = world_item ;
          }else{
            world_item->next  = wnode->world_item ;
            wnode->world_item = world_item ;
          }
          world_item->worldtype = itoken ;
          world_item->world_node = wnode ;

          if(itoken == CARSPAWNER || itoken == 357){
            if(ip)printf("  Initialise car spawner variables\n") ;
            world_item->u.car_spawn_obj.carspawnerlist_idx =    0 ;
            world_item->u.car_spawn_obj.car_frequency      =  5.0 ;
            world_item->u.car_spawn_obj.car_av_speed       = 20.0 ;
            world_item->u.car_spawn_obj.list_name          = NULL ;
          }else if(itoken == FOREST) {
            world_item->u.forest_obj.tree_texture    = NULL    ;
            world_item->u.forest_obj.scale_range_min = 0.0 ;
            world_item->u.forest_obj.scale_range_max = 0.0 ;
            world_item->u.forest_obj.X               = 0   ;
            world_item->u.forest_obj.Y               = 0   ;
            world_item->u.forest_obj.population      = 0   ;
            world_item->u.forest_obj.width           = 0.0 ;
            world_item->u.forest_obj.height          = 0.0 ;
          }else if(itoken == LEVELCR) {
            world_item->u.levelcr_obj.n_tracks       = 0    ;
            world_item->u.levelcr_obj.track_id       = NULL ;
            world_item->u.levelcr_obj.track_dist     = NULL ;
            world_item->u.levelcr_obj.n_roads        = 0    ;
            world_item->u.levelcr_obj.road_id        = NULL ;
            world_item->u.levelcr_obj.road_dist      = NULL ;
            world_item->u.levelcr_obj.should_be_open = 1    ;
            world_item->u.levelcr_obj.gate_position  = 1.0  ;
          }else if(itoken == PICKUP_ALT || itoken == 359 || PICKUP2 == itoken) {
            world_item->u.pickup_obj.collide_flags         = 0   ;
            world_item->u.pickup_obj.min_mps               = 0.0 ;
            world_item->u.pickup_obj.max_mps               = 0.0 ;
            world_item->u.pickup_obj.pickuptype            = 0   ;
            world_item->u.pickup_obj.pickuptype_2          = 0   ;
            world_item->u.pickup_obj.pickup_options        = 0   ;
            world_item->u.pickup_obj.animation_speed       = 0.0 ;
            world_item->u.pickup_obj.quantity_available_kg = 0.0 ;
            world_item->u.pickup_obj.feed_rate_kgps        = 0.0 ;
          }else if(itoken == PLATFORM_ALT) {
            world_item->u.platform_obj.platform_data   = 0 ;
          }else if(itoken == SIDING_ALT || itoken == 361 || itoken == SIDING2) {
            world_item->u.siding_obj.siding_data       = 0 ;
          }else if(itoken == SIGNAL_ALT) {
            world_item->u.signal_obj.signal_sub_object = 0 ;
            world_item->u.signal_obj.n_signal_units    = 0 ;
            world_item->u.signal_obj.sub_object        = NULL ;
            world_item->u.signal_obj.u_data1           = NULL ;
            world_item->u.signal_obj.tr_item           = NULL ;
            world_item->u.signal_obj.signal            = NULL ;
            world_item->u.signal_obj.n_matrices        = 0    ;
            world_item->u.signal_obj.sm_skip           = NULL ;
            world_item->u.signal_obj.sm_signal         = NULL ;
            world_item->u.signal_obj.skip              = NULL ;
          }else if(itoken == STATIC || COLLIDEOBJECT == itoken){
            world_item->u.static_obj.no_direct_light  = 0 ;
          }else if(itoken == SPEEDPOST) {
            world_item->u.speed_post_obj.speed_digit_tex = NULL ;
            world_item->u.speed_post_obj.shapes_info     = NULL ;
            world_item->u.speed_post_obj.size            = 0.0  ;
            world_item->u.speed_post_obj.dx              = 0    ;
            world_item->u.speed_post_obj.dy              = 0    ;
          }else if(itoken == TRANSFER   || itoken == 363) {
            world_item->u.transfer_obj.width          = 0.0 ;
            world_item->u.transfer_obj.height         = 0.0 ;
          }
/*
 *=====================================================================================
 *   Process level 2 tokens
 *=====================================================================================
 */
          msblock2 = NULL ;
          for(;;){
            if(ip)printf("\n") ;
            if(ip)printf("  Level 2 : world.c : Call open_block\n") ;
            istoken = open_block(msfile,2) ;
            if(ip)printf("  Level 2 : world.c : Token = %i\n",istoken) ;
            if(istoken == -2){
              if(ip)printf("  Level 2 : world.c : OpenBlock returned -2\n") ;
              if(ip)printf("  Level 2 : world.c : Token_unused = %s\n",
                                                             msfile->token_unused) ;
              return_token(")",msfile) ;
              break ;
            }
            itoken2 = istoken ;
            msblock2 = &(msfile->level[2])  ;
            if(ip)printf("  Level 2 : world.c : token = %i  %s  : block data length = %i \n",
              itoken2,token_idc[itoken2], msblock2->length-msblock2->l_label-9) ;
            if(UID == itoken2){
              world_item->uid = read_uint32_a(msfile) ;
//              ip = ip1 & (2542 == world_item->uid || 2544 == world_item->uid) ;
              if(ip)printf("      world_item->uid         = %i\n",world_item->uid);
/*
 *  Process shape filename
 *  This is often a filename relative to the current directory.
 *  Call routine to (1) add shape name to tile/world btree
 *                  (2) check for shape node
 *                  (3) If necessary generate shape node, save full filename
 *                         and add pointer to tile/world btree and full btree
 */
            }else if(FILENAME == itoken2){
// Strings are unicode, headed by two byte length
              tmp_name = read_ucharz_a(msfile) ;
              if(ip || ip1)printf("  raw       filename = %s\n",tmp_name) ;
              str2lc(tmp_name) ;                //  To lower case
              zr_filename_MS2L(tmp_name) ;      //  Convert '\' and "\\" to '/''
              if(ip || ip1)printf("  converted filename = %s\n",tmp_name) ;
/*
 *  If filename starts with a ./ or ../, add parent directory.
 */
              if(tmp_name[0]== '.'){
  char *string ;
#if 1
                string = (char *)malloc(strlen(ORroutedir) + strlen(tmp_name)+ 2) ;
                strcpy(string,ORroutedir) ;
#else
                string = (char *)malloc(strlen(full_parent_dir) + strlen(tmp_name)+ 2) ;
                strcpy(string,full_parent_dir) ;
#endif
                if(ip || ip1)printf("  AA     string = %s\n",string) ;
                strcat(string,"/") ;
                if(ip || ip1)printf("  BB     string = %s\n",string) ;
                strcat(string,tmp_name) ;
                if(ip || ip1)printf("  CC     string = %s\n",string) ;
                free(tmp_name) ;
                tmp_name = strdup(string) ;
                free(string) ;
              }
/*
 *  Remove "/./" and "/../" from string
 */
              if(strstr(tmp_name,"./"))tmp_name = strip_dot_dirs(tmp_name) ;
/*
 *  Add shapes to B-tree
 */
              ext = zr_extension(tmp_name) ;
              if(ip || ip1)printf("  filename = %s,  ext = %s\n",tmp_name,ext) ;
              if(!strcmp(ext,"s")){
                world_item->filename  = tmp_name ;
                add_shape_name_to_world_btree(wnode, world_item) ;
              }else{
                world_item->filename2 = tmp_name ;
#if 0
                printf("+++  Routine %s.  World Item refers to non-shape file\n",
                                                   my_name) ;
                printf("       World Type = %i %s\n", world_item->worldtype,
                                            token_idc[world_item->worldtype]) ;
                printf("       Shape Node = %p\n", (void *)world_item->snode) ;
                printf("       Filename   = %s\n", world_item->filename)      ;
                printf("       Filename2  = %s\n", world_item->filename2)     ;
#endif
              }
              tmp_name = NULL ;
//              if(ip)dump_btree(wnode->shape_tree,0,"X") ;
            }else if(POSITION == itoken2){
              world_item->X = read_real32_a(msfile) ;  // MSTS X
              world_item->Z = read_real32_a(msfile) ;  // MSTS Y
              world_item->Y = read_real32_a(msfile) ;  // MSTS Z
              if(ip || ip1)printf("            POSITION  :: world_item->X, Y, Z     = %f %f %f  :: %i %i\n",
                   world_item->X, world_item->Y,
                   world_item->Z, wnode->tile_x, wnode->tile_y) ;
            }else if(QDIRECTION == itoken2){
              world_item->A = read_real32_a(msfile) ;    //  Direction vector
              world_item->B = read_real32_a(msfile) ;    //    times
              world_item->C = read_real32_a(msfile) ;    //      sin(rotation/2)
              world_item->D = read_real32_a(msfile) ;    //  |vector| times cos(rotation/2)
/*
 *  Convert to direction and angle of rotation as used by OpenGl.
 *  Quaternions use the same axis convention as the 'geographic' axes
 *  of ZR so no other conversion is needed.
 *    See README.QUATERNIONS
 */
              temp = sqrt(world_item->A*world_item->A + world_item->B*world_item->B
                         + world_item->C*world_item->C) ;
              world_item->ANG = 2.0*atan2(temp,world_item->D)*degree ;
              if(temp > 0.00001){
                temp = 1.0/temp ;
                world_item->AX = world_item->A*temp ;
                world_item->AY = world_item->B*temp ;
                world_item->AZ = world_item->C*temp ;
                if(world_item->AX >  1.0) world_item->AX =  1.0 ;
                if(world_item->AX < -1.0) world_item->AX = -1.0 ;
                if(world_item->AY >  1.0) world_item->AY =  1.0 ;
                if(world_item->AY < -1.0) world_item->AY = -1.0 ;
                if(world_item->AZ >  1.0) world_item->AZ =  1.0 ;
                if(world_item->AZ < -1.0) world_item->AZ = -1.0 ;
              }else{
                world_item->ANG = 0.0 ;     // Default
                world_item->AX  = 0.0 ;
                world_item->AY  = 0.0 ;
                world_item->AZ  = 1.0 ;
              }
              if(ip || ip1){
                printf("            QDIRECTION :: world_item->A, B, C, D      = "
                            "%f %f %f : %f\n",
                   world_item->A, world_item->B,world_item->C, world_item->D) ;
                printf("            QDIRECTION :: world_item->AX, AY, AZ, ANG = "
                            "%f %f %f : %f\n",
                   world_item->AX, world_item->AY,
                   world_item->AZ, world_item->ANG) ;
              }
            }else if(VDBID == itoken2){
              world_item->vdb_id = read_lint32_a(msfile) ;
              if(ip)printf("      world_item->vdb_id      = %li\n",world_item->vdb_id);
            }else if(COLLIDEFLAGS == itoken2){
              world_item->collide_flags = read_uint32_a(msfile) ;
              if(ip)printf("      world_item->collide_flags = %i\n",world_item->collide_flags);
            }else if(STATICDETAILLEVEL == itoken2){
              world_item->static_detail_level = read_uint32_a(msfile) ;
              if(ip)printf("      world_item->static_detail_level = %i\n",
                                 world_item->static_detail_level);
            }else if(STATICFLAGS == itoken2){
              world_item->static_flags = read_uint32_a(msfile) ;
              if(ip)printf("      world_item->static_flags = %i\n",world_item->static_flags);
            }else if(TRITEMID == itoken2){
              if(world_item->n_tr_item<=16){
                world_item->tr_item_db[world_item->n_tr_item]      = read_int32_a(msfile) ;
                if(ip)printf("      world_item->n_tr_item       = %i\n",world_item->n_tr_item );
                if(ip)printf("      world_item->tr_item_db[]    = %i\n",world_item->tr_item_db[world_item->n_tr_item] );
                world_item->tr_item_db_id[world_item->n_tr_item] = read_int32_a(msfile) ;
                if(ip)printf("      world_item->tr_item_db_id[] = %i\n",world_item->tr_item_db_id[world_item->n_tr_item] );
                world_item->n_tr_item++ ;
              }else{
                printf(" ERROR Routine %s.  World item: %i %s %s\n", my_name, world_item->uid, token_idc[world_item->worldtype], world_item->filename) ;
                printf("       World item has more than 16 associated track items\n") ;
                read_int32_a(msfile) ; read_int32_a(msfile) ; // Skip track item
              }
            }else if(MAXVISDISTANCE == itoken2){
              world_item->max_vis_distance               = read_real32_a(msfile) ;
              if(ip)printf("      world_item->max_vis_distance = %f\n",world_item->max_vis_distance);

// TRACK OBJECT
            }else if(TRACKOBJ == itoken && SECTIONIDX == itoken2){
              world_item->u.track_obj.section_idx        = read_uint32_a(msfile) ;
              if(ip)printf("      world_item->*.section_idx = %i\n",
                                  world_item->u.track_obj.section_idx);
            }else if(TRACKOBJ == itoken && ELEVATION == itoken2){
              world_item->u.track_obj.elevation          = read_real32_a(msfile) ;
            }else if(TRACKOBJ == itoken && JNODEPOSN == itoken2){
              world_item->u.track_obj.tile_x             = read_int32_a(msfile)  ;
              world_item->u.track_obj.tile_y             = read_int32_a(msfile)  ;
              world_item->u.track_obj.X                  = read_real32_a(msfile) ;
              world_item->u.track_obj.Y                  = read_real32_a(msfile) ;
              world_item->u.track_obj.Z                  = read_real32_a(msfile) ;
            }else if(TRACKOBJ == itoken && COLLIDEFLAGS == itoken2){
              world_item->u.track_obj.collide_flags     = read_uint32_a(msfile)  ;
            }else if(TRACKOBJ == itoken && COLLIDEFUNCTION == itoken2){
              world_item->u.track_obj.collide_function  = read_uint32_a(msfile)  ;
#if 1
//  CARSPAWNER OBJECT
            }else if(itoken == CARSPAWNER || itoken == 357){
               if(CARFREQUENCY == itoken2){
                 world_item->u.car_spawn_obj.car_frequency= read_real32_a(msfile) ;
               }else if(CARAVSPEED == itoken2){
                 world_item->u.car_spawn_obj.car_av_speed = read_real32_a(msfile) ;
               }else if(ORTSLISTNAME == itoken2){
                 world_item->u.car_spawn_obj.list_name    = read_ucharz_a(msfile) ;
               }
#endif
//  FOREST OBJECT
            }else if(FOREST == itoken && TREETEXTURE == itoken2){
              world_item->u.forest_obj.tree_texture     = read_ucharz_a(msfile) ;
            }else if(FOREST == itoken && SCALERANGE == itoken2){
              world_item->u.forest_obj.scale_range_min  = read_real32_a(msfile) ;
              world_item->u.forest_obj.scale_range_max  = read_real32_a(msfile) ;
            }else if(FOREST == itoken && AREA == itoken2){
              world_item->u.forest_obj.X                = read_real32_a(msfile) ;
              world_item->u.forest_obj.Y                = read_real32_a(msfile) ; // MSTS Z
            }else if(FOREST == itoken && POPULATION == itoken2){
              world_item->u.forest_obj.population       = read_int32_a(msfile) ;
            }else if(FOREST == itoken && TREESIZE == itoken2){
              world_item->u.forest_obj.width            = read_real32_a(msfile) ;
              world_item->u.forest_obj.height           = read_real32_a(msfile) ;
//  SPEEDPOST OBJECT
            }else if(SPEEDPOST == itoken && SPEED_DIGIT_TEX == itoken2){
              world_item->u.speed_post_obj.speed_digit_tex  = read_ucharz_a(msfile) ;
            }else if(SPEEDPOST == itoken && SPEED_SIGN_SHAPE == itoken2){
              l = read_int32_a(msfile) ;
              world_item->u.speed_post_obj.shapes_info = (float *)
                                              malloc(l*4*sizeof(float)) ;
              for(i=0;i<l;i++){
                world_item->u.speed_post_obj.shapes_info[i*4+0] = read_real32_a(msfile) ;
                world_item->u.speed_post_obj.shapes_info[i*4+1] = read_real32_a(msfile) ;
                world_item->u.speed_post_obj.shapes_info[i*4+2] =-read_real32_a(msfile) ;
                world_item->u.speed_post_obj.shapes_info[i*4+3] = read_real32_a(msfile) ;
              }
            }else if(SPEEDPOST == itoken && SPEED_TEXT_SIZE == itoken2){
              world_item->u.speed_post_obj.size        = read_real32_a(msfile) ;
              world_item->u.speed_post_obj.dx          = read_real32_a(msfile) ;
              world_item->u.speed_post_obj.dy          = read_real32_a(msfile) ;
//  SIDING  OBJECT
            }else if((SIDING_ALT == itoken || 361 == itoken || SIDING2 == itoken)
                                                        && SIDINGDATA == itoken2){
              world_item->u.siding_obj.siding_data      = read_int32_a(msfile) ;
//  DYN TRACK OBJECT
            }else if((DYNTRACK == itoken|| 306 == itoken) && TRACKSECTIONS == itoken2){
DynTrackSect *dyn_trk_sect ;
              for(i=0;i<5;i++){
                dyn_trk_sect = &(world_item->u.dyn_track_obj.dyn_trk_sect[i]) ;
// SUB BLOCK
                iret = open_named_block(msfile,3,TRACKSECTION) ;
                itoken3 = TRACKSECTION ;
                msblock3 = &(msfile->level[3])  ;
                if(ip)printf("      Level 3 : token = %i  %s  : block data length = %i \n",
                itoken3,token_idc[itoken3], msblock3->length-msblock3->l_label-9) ;

                  iret = open_named_block(msfile,4,SECTIONCURVE) ;
                  itoken4 = SECTIONCURVE ;
                  msblock4 = &(msfile->level[4])  ;
                  if(ip)printf("        Level 4 : token = %i  %s  : "
                               "block data length = %i \n", itoken4,token_idc[itoken4],
                                                  msblock4->length-msblock4->l_label-9) ;
                  dyn_trk_sect->is_curved              = read_uint32_a(msfile) ;
                  if(ip)printf("      dyn_trk_sect->is_curved      = %i\n",
                                     dyn_trk_sect->is_curved);
                  end_block(msfile,4) ;

                dyn_trk_sect->uid                      = read_uint32_a(msfile) ;
                dyn_trk_sect->param_1                  = read_real32_a(msfile) ;
                dyn_trk_sect->param_2                  = read_real32_a(msfile) ;
                dyn_trk_sect->delta_y                  = 0.0             ;
                  if(ip)printf("      dyn_trk_sect->uid, parm1, parm2, delta_y = %i %f %f %f\n",
                                     dyn_trk_sect->uid,dyn_trk_sect->param_1,
                                      dyn_trk_sect->param_2,dyn_trk_sect->delta_y);
                end_block(msfile,3) ;
                if(!dyn_trk_sect->is_curved &&
                  0.0 != dyn_trk_sect->param_2 && 0.0 !=  dyn_trk_sect->param_1){
                  printf("  Routine %s : Dynamic track section has inconsistant values:\n",my_name);
                  printf("    is_curved = %i, param_1 = %f, param_2 = %f, uid = %i\n",
                               dyn_trk_sect->is_curved,dyn_trk_sect->param_1,
                               dyn_trk_sect->param_2,dyn_trk_sect->uid);
                  dyn_trk_sect->is_curved = 1 ;
                  printf("    is_curved set to %i\n",dyn_trk_sect->is_curved) ;
                }
              }
            }else if((DYNTRACK == itoken|| 306 == itoken) && SECTIONIDX == itoken2){
              world_item->u.dyn_track_obj.section_idx      = read_uint32_a(msfile) ;
              if(ip)printf("      section_idx = %i\n",world_item->u.dyn_track_obj.section_idx);
            }else if((DYNTRACK == itoken|| 306 == itoken) && ELEVATION == itoken2){
              world_item->u.dyn_track_obj.elevation        = read_real32_a(msfile) ;
              if(ip)printf("      elevation   = %f\n",world_item->u.dyn_track_obj.elevation);
            }else if((DYNTRACK == itoken|| 306 == itoken) && COLLIDEFLAGS == itoken2){
              world_item->u.dyn_track_obj.collide_flags   = read_uint32_a(msfile) ;
              if(ip)printf("      collide_flags = %i\n",world_item->u.dyn_track_obj.collide_flags);
//  TRANSFER OBJECT
            }else if((itoken == TRANSFER || itoken == 363) && WIDTH == itoken2){
              world_item->u.transfer_obj.width             = read_real32_a(msfile) ;
            }else if((itoken == TRANSFER || itoken == 363) && HEIGHT == itoken2){
              world_item->u.transfer_obj.height            = read_real32_a(msfile) ;
//  SIGNAL
//    SIGNALSUBOBJ is a set of flags.  A signal shape can have up to 32 heads.
//      The set of flags show which heads are active.
            }else if(itoken == SIGNAL_ALT && SIGNALSUBOBJ == itoken2){
              world_item->u.signal_obj.signal_sub_object   = read_uint32_a(msfile) ;
//    SIGNALUNITS starts with the numebr of active heads
            }else if(itoken == SIGNAL_ALT && SIGNALUNITS == itoken2){
              l                                          = read_uint32_a(msfile) ;
              world_item->u.signal_obj.n_signal_units    = l               ;
//       Allocate storage to contain information about the active heads
              world_item->u.signal_obj.sub_object = (int *)malloc(l*sizeof(int)) ;
              world_item->u.signal_obj.u_data1    = (uint *)malloc(l*sizeof(uint)) ;
              world_item->u.signal_obj.tr_item    = (uint *)malloc(l*sizeof(uint)) ;
//       Initialise storage used to speed shape processing
              world_item->u.signal_obj.signal     = (SignalDB **)malloc(l*sizeof(SignalDB *)) ;
              world_item->u.signal_obj.n_matrices = 0    ;  //  Number of matrices used by shape
              world_item->u.signal_obj.skip       = NULL ;  //  Flags to indicate unused matrices

              if(ip){
                printf("  WITEM :: SIGNAL_ALT \n") ;
                printf("  WITEM ::  n_signal_units = %i\n",world_item->u.signal_obj.n_signal_units) ;
              }
//  Loop to obtain information on active heads
              for(i=0;i<l;i++){
                iret    = open_named_block(msfile,3,SIGNALUNIT) ;
                itoken3 = SIGNALUNIT ;
                msblock3 = &(msfile->level[3])  ;
                if(ip)printf("      Level 3 : token = %i  %s  : block data length = %i \n",
//    Sub-Object.  This is the index of the active head in list given in sigcfg.dat
                itoken3,token_idc[itoken3], msblock3->length-msblock3->l_label-9) ;
                world_item->u.signal_obj.sub_object[i]    = read_int32_a(msfile) ;
//    Id of track item corresponding to this active head
//    Variable u_data.  In practice always set to zero.
//                      This may be an index, included originally to allow each head to be connected to
//                      more than one track item, but in practice not needed.
                iret    = open_named_block(msfile,4,TRITEMID) ;
                itoken4 = TRITEMID ;
                msblock4 = &(msfile->level[4])  ;
                if(ip)printf("      Level 4 : token = %i  %s  : block data length = %i \n",
                itoken4,token_idc[itoken4], msblock4->length-msblock4->l_label-9) ;
                world_item->u.signal_obj.u_data1[i]        = read_uint32_a(msfile) ;
                world_item->u.signal_obj.tr_item[i]        = read_uint32_a(msfile) ;
                if(ip){
                  printf("  WITEM :  Sub-Object = %i",i) ;
                  printf("  WITEM ::  sub_object = %i\n",world_item->u.signal_obj.sub_object[i]) ;
                  printf("  WITEM ::  u_data1    = %i\n",world_item->u.signal_obj.u_data1[i]) ;
                  printf("  WITEM ::  tr_item    = %i\n",world_item->u.signal_obj.tr_item[i]) ;
                }
                end_block(msfile,4) ;
                end_block(msfile,3) ;
              }  // End of loop over  active heads

//  LEVELCR OBJECT
            }else if(itoken == LEVELCR && LEVELCRPARAMETERS == itoken2){
              world_item->u.levelcr_obj.warning_time      = read_real32_a(msfile) ;
              world_item->u.levelcr_obj.minimum_distance  = read_real32_a(msfile) ;
              if(ip)printf("      warning_time        = %f\n",world_item->u.levelcr_obj.warning_time);
              if(ip)printf("      minimum_distance    = %f\n",world_item->u.levelcr_obj.minimum_distance);
            }else if(itoken == LEVELCR && CRASHPROBABILITY == itoken2){
              world_item->u.levelcr_obj.crash_probability = read_int32_a(msfile) ;
              if(ip)printf("      crash_probability   = %i\n",world_item->u.levelcr_obj.crash_probability);
            }else if(itoken == LEVELCR && LEVELCRDATA == itoken2){
                world_item->u.levelcr_obj.level_cr_data_1  = read_int32_a(msfile) ;
                world_item->u.levelcr_obj.level_cr_data_2  = read_int32_a(msfile) ;
                world_item->u.levelcr_obj.visible =
                  (world_item->u.levelcr_obj.level_cr_data_1 & 0x1) == 0   ;
                world_item->u.levelcr_obj.silent =
                  !(world_item->u.levelcr_obj.visible) ||
                  (world_item->u.levelcr_obj.level_cr_data_1 & 0x6) == 0x6 ;
              if(ip)printf("      level_cr_data_1     = %i\n",world_item->u.levelcr_obj.level_cr_data_1);
              if(ip)printf("      level_cr_data_2     = %i\n",world_item->u.levelcr_obj.level_cr_data_2);
              if(ip)printf("      visible             = %i\n",world_item->u.levelcr_obj.visible);
              if(ip)printf("      silent              = %i\n",world_item->u.levelcr_obj.silent);
            }else if(itoken == LEVELCR && LEVELCRTIMING == itoken2){
                world_item->u.levelcr_obj.initial_timing   = read_real32_a(msfile) ;
                world_item->u.levelcr_obj.serious_timing   = read_real32_a(msfile) ;
                world_item->u.levelcr_obj.anim_timing      = read_real32_a(msfile) ;
              if(ip)printf("      initial_timing      = %f\n",world_item->u.levelcr_obj.initial_timing);
              if(ip)printf("      serious_timing      = %f\n",world_item->u.levelcr_obj.serious_timing);
              if(ip)printf("      anim_timing         = %f\n",world_item->u.levelcr_obj.anim_timing);
//  PICKUP OBJECT
            }else if((PICKUP_ALT == itoken || 359 == itoken || PICKUP2 == itoken)
                                                          && SPEEDRANGE == itoken2){
              world_item->u.pickup_obj.min_mps             = read_real32_a(msfile) ;
              world_item->u.pickup_obj.max_mps             = read_real32_a(msfile) ;
            }else if((PICKUP_ALT == itoken || 359 == itoken || PICKUP2 == itoken)
                                                          && PICKUPTYPE == itoken2){
              world_item->u.pickup_obj.pickuptype          = read_uint32_a(msfile) ;
              world_item->u.pickup_obj.pickuptype_2        = read_uint32_a(msfile) ;
            }else if((PICKUP_ALT == itoken || 359 == itoken || PICKUP2 == itoken)
                                                      && PICKUPANIMDATA == itoken2){
              world_item->u.pickup_obj.pickup_options      = read_real32_a(msfile) ;
              world_item->u.pickup_obj.animation_speed     = read_real32_a(msfile) ;
              if(0.0 == world_item->u.pickup_obj.animation_speed)
                 world_item->u.pickup_obj.animation_speed = 1.0 ;
            }else if((PICKUP_ALT == itoken || 359 == itoken || PICKUP2 == itoken)
                                                       && PICKUPCAPACITY == itoken2){
              world_item->u.pickup_obj.quantity_available_kg = read_real32_a(msfile) ;
              world_item->u.pickup_obj.feed_rate_kgps        = read_real32_a(msfile) ;
            }else if((PICKUP_ALT == itoken || 359 == itoken || PICKUP2 == itoken)
                                                         && COLLIDEFLAGS == itoken2){
              world_item->u.pickup_obj.collide_flags = read_uint32_a(msfile) ;
//  PLATFORM
            }else if(PLATFORM_ALT == itoken && PLATFORMDATA == itoken2){
              world_item->u.platform_obj.platform_data       = read_uint32_a(msfile) ;
//  STATIC and COLLIDEOBJECT == itoken
            }else if((STATIC == itoken || COLLIDEOBJECT == itoken) && NODIRLIGHT == itoken2){
                world_item->u.static_obj.no_direct_light = 1 ;
            }else if(COLLIDEOBJECT == itoken && COLLIDEFUNCTION == itoken2){
              skip_to_bblock_end_a(msfile,msblock2) ;
            }else if((STATIC == itoken || TRACKOBJ == itoken)
                                       && MATRIX3X3 == itoken2){
              skip_to_bblock_end_a(msfile,msblock2) ;
            }else{
              printf("   ERROR : Routine %s.  File %s\n",my_name,wnode->wfile);
              printf("     Unrecognised level 2 token \n");
              printf("     Level 1 token  = %i %s\n",itoken, token_idc[itoken])  ;
              printf("     Level 2 token  = %i %s\n",itoken2,token_idc[itoken2]) ;
              if(ip){
                printf("     ftell(fp)    = %i\n",(int)ftell(fp)) ;
                printf("     msfile->level[0].byte_end  = %i\n",msfile->level[0].byte_end) ;
                printf("     msfile->level[1].byte_end  = %i\n",msfile->level[1].byte_end) ;
                printf("     msblock1->byte_end         = %i\n",msblock1->byte_end) ;
                printf("     msblock2->byte_end         = %i\n",msblock2->byte_end) ;
              }
              skip_to_bblock_end_a(msfile,msblock2) ;
            }
/*
 *  End of level 2 block
 *  If end of block in a binary file - break out of loop
 *  Otherwise return to top of level 2 loop
 */
            if(ip)printf("    Call end_block 2\n") ;
            if(ip)printf("      token_unused = %s\n",msfile->token_unused) ;
            if(ip)printf("      current_block_level = %i\n",current_block_level) ;
            end_block(msfile,2) ;
            if(ip)printf("      token_unused = %s\n",msfile->token_unused) ;
            if(ip)printf("      current_block_level = %i\n",current_block_level) ;
            if(ip)printf("    Exit end_block 2\n\n") ;

            if(is_binary && ftell(fp) == msfile->level[1].byte_end) break ;
          }
          if(is_binary && msblock2 != NULL){
            for(i=ftell(fp);i<msblock2->byte_end;i++){
              fgetc(fp) ;
            }
          }
          if(ip)printf("\n") ;
          if(ip)printf("  Jumped out of level 2 loop\n\n") ;
          if(!msfile->binary)current_block_level--;
#if 0
          if(!is_binary){
            printf("  Call end_block\n") ;
//            end_block(msfile,2) ;
            printf("  token_unused = %s\n",msfile->token_unused) ;
            printf("  current_block_level = %i\n",current_block_level--) ;

            printf("  current_block_level = %i\n",current_block_level) ;

            printf("  Exit end_block\n") ;
          }
#endif
#if 0
          if(world_item->uid == 50038){
            printf("\n+++++++++++++++++ WORLD ITEM +++++++++++++++++++++++++\n") ;
            list_wfile_item(world_item) ;
            printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n") ;
          }
#endif
/*
 *  End of level 1 procesing
 */
        }else{
          printf(" Level 1 token not recognised.\n   Token = %i %X :: %s"
                 " :: block data length = %i %X.   File = %s\n",
           itoken,itoken,token_idc[itoken], msblock1->length-msblock1->l_label-9,
                                            msblock1->length-msblock1->l_label-9,
                                            zr_basename(wnode->wfile) ) ;
          if(is_binary){
            int j, n = msblock1->byte_end - ftell(fp) + 1;
            char *string ;
            string = (char *)malloc(n) ;
            for(i=ftell(fp), j=0;i<msblock1->byte_end;i++,j++){
              string[j] = fgetc(fp) ;
            }
            string[j] = '\0' ;
            printf("  Data in hexadecimal\n");
            for(j=0; j<n; j++)printf("%2X ",string[j]) ;
            printf("\n") ;
            printf("  Data in ascii\n") ;
            for(j=0; j<n; j++)printf("%c",isalnum(string[j]) ? string[j] : '.') ;
            printf("\n") ;
            free(string) ;
          }
        }
        if(ip)printf("  Level 1 : world.c : token_unused = %s\n",msfile->token_unused) ;
        if(ip)printf("  Level 1 : world.c : At end of level 1 loop.  Call end_block()\n") ;
        end_block(msfile,1) ;
        if(ip)printf("  Level 1 : world.c : token_unused = %s\n",msfile->token_unused) ;
        if(ip)printf("  At end of level 1 loop.  After return from end_block()\n") ;
      }
      if(0 && iret){} ;                         //  Keep the compiler happy
      close_msfile(msfile) ;
      free(parent_dir) ;
      free(full_parent_dir) ;
      if(ip)printf(" Normal return\n");
      return 0 ;
}

int world_item_init(WorldItem *world_item){

int i ;

      world_item->uid           = -1   ;
      world_item->next          = NULL ;
      world_item->world_node    = NULL ;
      world_item->snode         = NULL ;
      world_item->filename      = NULL ;
      world_item->filename2     = NULL ;
      world_item->worldtype     = 0    ;
      world_item->static_flags  = 0    ;
      world_item->collide_flags = 0    ;
      world_item->X             = 0.0  ;
      world_item->Y             = 0.0  ;
      world_item->Z             = 1.0  ;   //  height : MSTS Y Coordinate
      world_item->A             = 0.0  ;
      world_item->B             = 0.0  ;
      world_item->C             = 0.0  ;
      world_item->D             = 0.0  ;
      world_item->AX            = 0.0  ;
      world_item->AY            = 1.0  ;
      world_item->AZ            = 0.0  ;
      world_item->ANG           = 0.0  ;
      world_item->SX            = 1.0  ;
      world_item->SY            = 1.0  ;
      world_item->SZ            = 1.0  ;
      world_item->anim_value    = 0.0  ;
      world_item->vdb_id        = 0    ;
      world_item->iz_off        = 0    ;
      world_item->n_tr_item     = 0    ;
      world_item->max_vis_distance    = 10000.0 ;
      world_item->static_detail_level = 0       ;
      world_item->n_animations  = 0    ;
      world_item->animations    = NULL ;
      for(i=0;i<8;i++){
        world_item->tr_item_db[i]    = 0  ;
        world_item->tr_item_db_id[i] = 0  ;
      }
      return 0;
}

int add_shape_pointers_to_world_items(){

  int  i, k, n ;
  int  ip = 0  ;   // 0 = no printing, -1 = error printing only
  int  id = 0  ;
  int  i_list = 0 ;
  int  n_errors = 0 ;
  WorldNode    *wnode ;
  WorldItem    *witem ;
  ShapeNode    *snode ;
  char         *wname,
               *sname,
               *tname ;
  char         my_name[] = "add_shape_pointers_to_world_items" ;

        if(ip)printf(" Enter routine %s\n",my_name) ;
        if(ip)printf("    worldlist_beg = %p\n",(void *)worldlist_beg) ;
        for(wnode=worldlist_beg; wnode != NULL; wnode=wnode->next){
#if 0
          printf(" ++++++++++++++++++++++++++++++++++++++++++++++++++ \n") ;
          printf(" New world file : wnode = %p\n",(void *)wnode) ;
          printf("                   name = %s\n",wnode->name) ;
          printf("                 witem0 = %p\n",(void *)wnode->world_item) ;
          printf("                   next = %p\n",(void *)wnode->next) ;
//          if(wnode->tile_x != tst_tile_x || wnode->tile_y != tst_tile_y)continue ;
#endif
          for(witem=wnode->world_item; witem != NULL; witem=witem->next){
            if(NULL != witem->filename){
//              ip = id = (witem->uid == 202
//                   && wnode->tile_x == -6131 && wnode->tile_y == 14888) ;
//              ip = id = (witem->uid == 413 ) ;
              tname = strdup(witem->filename) ;
              wname = zr_basename(tname) ;
              free(tname) ;
              n = strlen(wname) ;
              for(k=0;k<n;k++)wname[k] = tolower(wname[k]) ;
/*
 *  Skip hazard (*.haz) files - Add when hazards are implimented properly
 *  Skip ?? (*.ace) files     - Why are they here ??
 */
              if(!strncmp(&wname[n-4],".haz",4))continue ;
              if(!strncmp(&wname[n-4],".ace",4))continue ;
              wname[strlen(wname)-2] = '\0';
              if(ip || id){
                printf(" +++ witem = %p,  uid = %i,  tile_x = %i %i,  tile_y = %i %i\n",
                               (void *)witem, witem->uid,
                               wnode->tile_x, wnode->tile_x-tile_west,
                               wnode->tile_y, wnode->tile_y-tile_south) ;
                printf("     type       = %i   %s\n",
                                witem->worldtype,token_idc[witem->worldtype]) ;
                printf("     shape name = %s\n",wname) ;
              }
//  Gives errors !!!!
// This may be because the btree only relates to one tile.
#if 1
  BTree *btree = NULL ;
//              printf("============================================\n") ;
//              printf(" Calling find_btree\n") ;
//              printf(" wname = %s\n",wname) ;
//              printf("  shape master = %p\n",(void *)shape_master) ;
              btree = find_btree(shape_master,wname) ;
//              printf("  btree = %p\n",(void *)btree) ;
              if(!btree){
                printf("  Routine %s\n", my_name) ;
                printf("    Unable to find shape with name %s\n",wname) ;
              }else{
                snode = (ShapeNode *) btree->data ;
//                printf("  snode = %p\n",(void *)snode) ;
//              printf("============================================\n") ;
                witem->snode  = snode ;
                snode->basic  = 0     ;
                snode->needed = 0     ;
                snode->loaded = 0     ;
                if(ip)printf(" Active list : wnode = %p, witem = %p, add shape: %s\n",
                           (void *)wnode, (void *)witem, witem->snode->name) ;
              }
#else
              witem->snode = NULL ;
              for(snode = shapelist_beg; NULL != snode; snode=snode->next ){
                sname = strdup(snode->name) ;
                for(k=0;k<(int)strlen(sname);k++)sname[k] = tolower(sname[k]) ;
                if(ip)printf("  sname = %s\n",sname) ;
                if(0 == strcmp(wname,sname)){
                  witem->snode  = snode ;
                  snode->basic  = 0     ;
                  snode->needed = 0     ;
                  snode->loaded = 0     ;
                  if(ip)printf("     active list : wnode = %p, witem = %p, add shape: %s  ::  %s \n",(void *)wnode, (void *)witem, sname, witem->snode->name) ;
                  free(sname) ;
                  break ;
                }
                free(sname) ;
              }
#endif
/*
 *  TODO
 *  If shape not found, look for shape file
 *     If successful and to shapelist and load shape
 */
              if(witem->snode == NULL){
                printf("  Routine %s error.  Unable to find shape %s\n",
                       my_name,wname) ;
              }
/*
 *  Initialise variable n_animations and allocate memory for the animation pointers
 */
              snode = witem->snode ;
              if(snode  && snode->lod_control){
                witem->n_animations = snode->lod_control->dist_level->n_hierarchy ; ;
                witem->animations = (double **)malloc(witem->n_animations*sizeof(double **)) ;
                for(k=0;k<witem->n_animations;k++)witem->animations[k] = NULL ;
              }else if(n_errors++<20){
                printf(" Routine %s\n",my_name) ;
                if(!snode->lod_control){
                  printf("  lod_control missing for shape %s needed for %s\n",
                                             snode->name,wname) ;

                }else{
                  printf(" Unable to find shapefile corresponding to %s\n",wname);
                }
                printf(" wname in ascii : ") ;
                for(i=0;i<(int)strlen(wname);i++) printf(" %x ",wname[i]) ;
                printf("\n") ;
                printf("    witem = %p,  uid = %i,  tile_x = %i %i,  tile_y = %i %i\n",
                               (void *)witem, witem->uid,
                               wnode->tile_x, wnode->tile_x-tile_west,
                               wnode->tile_y, wnode->tile_y-tile_south) ;
                printf("    witem->worldtype   = %i, %s\n",witem->worldtype,
                                                       token_idc[witem->worldtype]) ;
                printf("    witem->filename    = %s\n",witem->filename) ;
                printf("    witem->shape       = %p\n",(void *)witem->snode) ;
                if(witem->snode)
                printf("    witem->shape->name = %s",witem->snode->name) ;

                printf("\n") ;
#if 0
                if(i_list++ == 0){
                  for(snode = shapelist_beg; NULL != snode; snode=snode->next ){
//                   if(!strncmp(wname,snode->name,7))
                      printf("  Shape name = %s :: %s\n",snode->name, snode->s_file);
                  }
                }
#endif
                if(0){
                  printf("\n  Routine %s ... \n",my_name) ;
                  printf("  Program ending ...\n") ;
                  exit(0) ;
                }
                if(20 == n_errors)
                  printf("  Missing shapes:  Error reporting limit reached\n") ;
                snode->name = NULL ;
              }else{
                snode->name = NULL ;
              }
              free(wname) ;
            }
          }
        }
  return 0 ;
}

int  list_wfile_item(WorldItem *wi){

  int i ;

      printf("    WorldItem     uid          = %i \n",wi->uid) ;
      printf("    WorldItem     world node   = %p \n",(void *)wi->world_node) ;
      printf("    WorldItem     shape node   = %p \n",(void *)wi->snode) ;
      printf("    WorldItem     shape name   = %s \n",wi->filename) ;
      printf("    WorldItem     type         = %i %s\n",wi->worldtype, token_idc[wi->worldtype]) ;
      printf("    WorldItem     static flags = %8x \n",wi->static_flags) ;
      if(wi->static_flags & SF_RoundShadow)printf("                   Round Shadow\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Rectangular Shadow\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Treeline Shadow\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Dynamic Shadow\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Any Shadow\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Terrain\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Animate\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Global\n");
      printf("    WorldItem     position     = %f %f %f\n",wi->X,wi->Y,wi->Z) ;
      printf("    WorldItem     rotation     = %f %f %f %f\n",wi->AX,wi->AY,wi->AZ,wi->ANG) ;
      printf("    WorldItem     n_tr_item    = %i\n",wi->n_tr_item) ;
      printf("    WorldItem     tr_item_db   = ") ;
      for(i=0;i<8;i++)printf(" %6i", wi->tr_item_db[i]) ;
      printf("\n") ;
      printf("    WorldItem     tr_item_db   = ") ;
      for(i=0;i<8;i++)printf(" %6i", wi->tr_item_db_id[i]) ;
      printf("\n") ;
      return 0;
}

/*
 *  Each world node has a btree containing the names of shapes used
 *  within that world tile.  There is also a master btree.
 *
 *  Process shape filename
 *  This is often a filename relative to the current directory.
 *  Call routine to (1) add shape name to tile/world btree
 *                  (2) check for shape node
 *                  (3) If necessary generate shape node, save full filename
 *                         and add pointer to tile/world btree and full btree
 */

void  add_shape_name_to_world_btree(WorldNode *wnode, WorldItem *world_item){

uint   i ;
char   *name = NULL ;
BTree  *n =  wnode->shape_tree ;
BTree  *btree_node   ;
ShapeNode *shape     ;
char   *my_name = "add_shape_name_to_world_btree" ;

/*  Generate shape name.  This is the filename without directories
 *  and extension, converted to lower case.
 */
      name = zr_basename2(world_item->filename) ;
      for(i=0;i<strlen(name);i++)name[i] = tolower(name[i]) ;
/*
 *  Check world tile btree.
 *  If it is already flagged, return from routine
 */
      if(NULL != n){
        if(find_btree(n,name)){
          free(name) ;      //  Release memory
          return     ;      //  Name already exists
        }
      }
/*
 *  Check shape_master btree
 */
      btree_node = find_btree(shape_master,name) ;
/*
 *  If found insert node with name and data in current world tile btree
 *  The data is a pointer to the ShapeNode
 *
 */
      if(btree_node){
        wnode->shape_tree =
             insert_node(wnode->shape_tree,name,btree_node->data) ;
        return ;
      }
/*
 *  If not found, generate a new ShapeNode and add the name and filename
 *  Then add a new node to the shape_master and world tile btrees.
 */
      shape = (ShapeNode *)malloc(sizeof(ShapeNode)) ;
      init_shape_node(shape) ;
      if(shapelist_beg == NULL){
        shapelist_beg = shape       ;
      }else{
        shapelist_end->next = shape ;
      }
      shapelist_end = shape ;
      shape->next   = NULL ;
/*
 *  Add name and filename to shape
 */
      shape->name   = strdup(name) ;
      shape->s_file = strdup(world_item->filename) ;
/*
 *  Add name and shape to shape_master and world tile btrees
 */
      shape_master      = insert_node(shape_master,     name,(void *)shape) ;
      wnode->shape_tree = insert_node(wnode->shape_tree,name,(void *)shape) ;
      return ;
}

/*
 *  After generating each world tile btree, the shapes it contains are added to
 *  the world_master btree containing all the shapes used by the layout.
 */

void add_world_shapes_to_master(void *b){

BTree *bb = (BTree *)b ;

      if(shape_master){
        if(find_btree(shape_master, bb->index)) return  ;
      }
      shape_master = insert_node(shape_master, bb->index, NULL) ;
      return ;
}

/*
 *   If the corresponding *.ws file is present, read the sound sources
 *   in the file and add to a list held in the world node.
 */
#ifdef OPENAL
int  load_world_soundfile(WorldNode *w){

  int ip = 0 ;
  int n, iret     ;
  char *s_file ;
  char *token1 = NULL ;
  char *token2 = NULL ;
  char *token3 = NULL ;
  FILE    *fp ;
  MSfile  msfile0 ;
  MSfile  *msfile = &msfile0 ;
  SoundObjectNode *so_node ;
  SoundRegionNode *sr_node ;

  char *my_name = "load_world_soundfile" ;

      if(ip){
        printf("  Enter routine '%s'\n",my_name) ;
        printf("  Tile = %i :: %i\n",w->tile_x, w->tile_y) ;
      }

      n = strlen(w->wfile) ;
      s_file = (char *)malloc(n+2) ;
      strcpy(s_file,w->wfile) ;
      s_file[n]   = 's'  ;
      s_file[n+1] = '\0' ;

      fp = fopen(s_file,"r") ;
      if(fp == NULL){
        s_file[n]   = 's'  ;
        fp = fopen(s_file,"r") ;
        if(fp == NULL) return 0 ;  // No *.ws file
      }
      if(ip)printf("  World soundfile found: %s\n",s_file);

      iret = open_msfile(s_file, msfile, 0, 0) ;
      if(iret){
        printf("\n   ERROR in routine %s: \n",my_name);
        printf("     Routine open_msfile failed to open file\n");
        printf("     File = %s\n",s_file) ;
        close_system() ;
      }
      if(ip)printf("    Routine %s.  MSTS file opened\n",my_name) ;

      token1 = ctoken(msfile) ;
      if(strcmp(token1,"Tr_Worldsoundfile")!= 0){
        printf("\n   ERROR in routine %s: \n",my_name);
        printf("     File does not start with 'Tr_Worldsoundfile'.\n");
        close_msfile(msfile) ;
        return 1 ;
      }

      skip_lbr(msfile) ;
      for(;;){
        if(token2)free(token2) ;
        token2 = ctoken(msfile);
        if(ip)printf("      Level 2 AA :: token2 = %s \n",token2) ;
        if(is_rbr(token2)) break ;  // Level 1 end bracket
        if(ip)printf("      Level 2 BB :: token2 = %s \n",token2) ;
        SWITCH(token2)
          CASE("Soundsource")
            skip_lbr(msfile) ;
            so_node = (SoundObjectNode *)malloc(sizeof(SoundObjectNode)) ;
            so_node->uid            = 0 ;
            so_node->source_set     = 0 ;
            so_node->sms_file       = NULL ;
            so_node->sms_node       = NULL ;
            so_node->priority_level = 0 ;
            so_node->n_streams      = 0 ;
            so_node->stream_status  = NULL ;
            so_node->source         = NULL ;
            so_node->sound_on       = NULL ;
            so_node->source_on      = NULL ;
            so_node->last_file      = NULL ;
            so_node->distance_on    = 0 ;
            for(;;){
              if(token3)free(token3) ;
              token3 = ctoken(msfile) ;  // NOTE use before skip_lbr
              if(ip)printf("      Level 3 AA :: token3 = %s \n",token3) ;
              if(is_rbr(token3)) break ; // Level 2 end bracket
              if(ip)printf("      Level 3 BB :: token3 = %s \n",token3) ;
              SWITCH(token3)
                CASE("Position")
                  skip_lbr(msfile) ;
                  so_node->X = dtoken(msfile) ;
                  so_node->Z = dtoken(msfile) ;
                  so_node->Y = dtoken(msfile) ;
                  skip_rbr(msfile);
                  break ;
                CASE("FileName")
                  skip_lbr(msfile) ;
                  so_node->sms_file = ctoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("UiD")
                  skip_lbr(msfile) ;
                  so_node->uid = itoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                DEFAULT
                  printf("\n   ERROR in routine %s: \n",my_name);
                  printf("     Unrecodnised level 3 token.\n");
                  printf("     Token = '%s'\n",token3);
                  printf("     File  = '%s'\n",s_file);
                  free(so_node) ;
                  so_node = NULL ;
                  close_msfile(msfile) ;
                  return 2 ;
              END
            }
            free(token3) ; token3 = NULL ;
           if(ip)printf("    Sound Source.  Filename = %s\n",so_node->sms_file) ;
/*
 *  Add at front of list
 */
            if(so_node != NULL){
              so_node->next = w->sound_source ;
              w->sound_source = so_node ;
            }
            break ;
          CASE("Soundregion")
            skip_lbr(msfile) ;
            sr_node = (SoundRegionNode *)malloc(sizeof(SoundRegionNode)) ;

            sr_node->uid          = 0 ;
            sr_node->trk_item_id0 = 0 ;
            sr_node->trk_item_id  = 0 ;
            sr_node->snd_trk_type = 0 ;
            sr_node->flags        = 0 ;
            sr_node->detail_level = 0 ;
            sr_node->vdbid        = 0 ;
            sr_node->roty         = 0.0  ;
            sr_node->X            = 0.0  ;
            sr_node->Y            = 0.0  ;
            sr_node->Z            = 0.0  ;
            sr_node->Q0           = 0.0  ;
            sr_node->QX           = 0.0  ;
            sr_node->QY           = 0.0  ;
            sr_node->QZ           = 0.0  ;
            sr_node->filename     = NULL ;

            for(;;){
              if(token3)free(token3)   ;
              token3 = ctoken(msfile)  ;
              if(is_rbr(token3)) break ;
              if(ip)printf("      Level 3 :: token3 = %s \n",token3) ;
              SWITCH(token3)
                CASE("UiD")
                  skip_lbr(msfile) ;
                  sr_node->uid = itoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("TrItemId")
                  skip_lbr(msfile) ;
                  sr_node->trk_item_id0 = itoken(msfile) ;
                  sr_node->trk_item_id  = itoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("SoundregionTrackType")
                  skip_lbr(msfile) ;
                  sr_node->snd_trk_type = itoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("StaticFlags")
                  skip_lbr(msfile) ;
                  sr_node->flags = flagtoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("VDbId")
                  skip_lbr(msfile) ;
                  sr_node->flags = atoll(new_tmp_token(msfile)) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("Position")
                  skip_lbr(msfile) ;
                  sr_node->X = dtoken(msfile) ;
                  sr_node->Y = dtoken(msfile) ;
                  sr_node->Z = dtoken(msfile) ;
                  skip_rbr(msfile);
                  break ;
                CASE("QDirection")
                  skip_lbr(msfile) ;
                  sr_node->Q0 = dtoken(msfile) ;
                  sr_node->QX = dtoken(msfile) ;
                  sr_node->QY = dtoken(msfile) ;
                  sr_node->QZ = dtoken(msfile) ;
                  skip_rbr(msfile);
                  break ;
                CASE("SoundregionRoty")
                  skip_lbr(msfile) ;
                  sr_node->roty = dtoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("FileName")
                  skip_lbr(msfile) ;
                  sr_node->filename = ctoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("StaticDetailLevel")
                  skip_lbr(msfile) ;
                  sr_node->detail_level = itoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                DEFAULT
                  printf("\n   ERROR in routine %s: \n",my_name);
                  printf("     Unrecognised level 3 token.\n");
                  printf("     Token = '%s'\n",token3);
                  printf("     File  = '%s'\n",s_file);
                  free(sr_node) ;
                  sr_node = NULL ;
                  close_msfile(msfile) ;
                  return 2 ;
              END
            }
            free(token3) ; token3 = NULL ;
            if(ip)printf("    Sound Region.  Trk type = %i\n",sr_node->snd_trk_type) ;
/*
 *  Add at front of list
 */
            if(sr_node != NULL){
              sr_node->next = w->sound_region ;
              w->sound_region = sr_node ;
            }
            break ;
          DEFAULT
            printf("\n   ERROR in routine %s: \n",my_name);
            printf("     Unrecodnised level 2 token.\n");
            printf("     Token = '%s'\n",token2);
            printf("     File  = '%s'\n",s_file);
            close_msfile(msfile) ;
            return 2 ;
        END
      }
//      skip_rbr(msfile) ;
      if(token3)free(token3) ;
      if(token2)free(token2) ;
      if(token1)free(token1) ;
      close_msfile(msfile) ;
      return 0 ;
}
#endif

#if 0
/data1/djw/Hobbies/OR/Open_Rails_Train_Simulator_-_ZZ/Routes/au_great_zig_zag/world/w+001451+010331.ws
	Soundregion (
		SoundregionRoty ( 1.76021 )
		SoundregionTrackType ( 0 )
		TrItemId ( 0 571 )
		UiD ( 100005 )
		FileName ( IMRegionPoint.s )
		StaticFlags ( 00100000 )
		Position ( 841.457 1035.81 575.188 )
		QDirection ( 0.00845778 -0.77076 -0.008369 0.637014 )
		VDbId ( 4294967295 )
	)
	Soundregion (
		SoundregionRoty ( -1.37112 )
		SoundregionTrackType ( 12 )
		TrItemId ( 0 572 )
		UiD ( 100006 )
		FileName ( IMRegionPoint.s )
		StaticFlags ( 00100000 )
		Position ( 836.461 1035.69 576.17 )
		QDirection ( -0.00753009 0.633062 -0.00921261 0.77401 )
		VDbId ( 4294967295 )
	)

#endif
/*
 *  Routine to print information about a given world item defined
 *  by tile and uid
 */

int  print_world_item_data(int tile_x, int tile_y, int uid){

int       ip = 0 ;
int       item_found = 0 ;
int       i ;
WorldNode *wnode ;
WorldItem *witem ;
SignalObj *sigobj ;

char *my_name = "print_world_item_data" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
      for(wnode = worldlist_beg; wnode != NULL ; wnode = wnode->next){
//        printf("  Tile %i %i %p \n",wnode->tile_x, wnode->tile_y, (void *)wnode->world_item) ;
        if(tile_x != 0 && tile_y != 0 &&
           (wnode->tile_x != tile_x || wnode->tile_y != tile_y)) continue ;
        for(witem = wnode->world_item; witem != NULL ; witem = witem->next){
//          printf("  %i\n",witem->uid) ;
          if(witem->uid != uid) continue ;
          item_found = 1 ;
          printf("\n***************************************************\n") ;
          printf("  World item:  tile_x = %i, tile_y = %i, uid = %i\n",
                                                       tile_x, tile_y, uid) ;
          printf("    Type = %i :: %s\n",witem->worldtype,token_idc[witem->worldtype]) ;
          printf("    shape file = %s\n",witem->filename) ;
          printf("    X  = %f,  Y  = %f, Z  = %f\n",witem->X,witem->Y,witem->Z) ;
          printf("    AX = %f,  AY = %f, AZ = %f, ANG = %f\n",
                                      witem->AX,witem->AY,witem->AZ,witem->ANG) ;
          printf("    n_animations  = %i\n",witem->n_animations) ;
          for(i=0; i<witem->n_animations; i++){
            printf("    animation[%i]  = %f\n",i,
                        (witem->animations[i]) ? *(witem->animations[i]) : 0.0)  ;
          }
          printf("    n_tr_item     = %i\n",witem->n_tr_item)  ;
          for(i=0; i<witem->n_tr_item; i++){
            printf("    tr_item_db[%i]  = %i : %i\n",
                                i,witem->tr_item_db[i],witem->tr_item_db_id[i])  ;
          }

          if(witem->worldtype == SIGNAL_ALT){
            sigobj = &(witem->u.signal_obj) ;
            printf("    signal_sub_object = %i\n",sigobj->signal_sub_object) ;
            printf("    n_signal_units    = %i\n",sigobj->n_signal_units) ;
            for(i=0; i<sigobj->n_signal_units; i++){
              printf("      sub_object = %i,  track item = %i, u_data1 = %i, signal = %i\n",
                         sigobj->sub_object[i], sigobj->tr_item[i],
                         sigobj->u_data1[i], sigobj->signal[i]->uid) ;
            }
          }
        }
        if(item_found == 0){
          printf("  ERROR. Routine %s\n",my_name) ;
          printf("    Track item not found for Tile %i %i,  item %i\n",
                       tile_x, tile_y, uid) ;
        }
      }
      return 0 ;
}
