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

  int      len1, len2 ;
  int      ip = 0 ;                // Control printing : 0 = no printing
  char     *wdir_name ;
  DIR      *wdir;
  char     string7[7] ;
  struct dirent *f_entry;
  WorldNode *world ;

/*
 *  1.  Search for world directory (world, WORLD)
 */
      len1 = strlen(ORroutedir) + strlen("world/") + 1 ;
      wdir_name = (char *)malloc(len1) ;
      strcpy(wdir_name,ORroutedir)    ;
      strcat(wdir_name,"world/")      ;
      if(ip)printf(" Trying directory world = %s\n",wdir_name) ;
      wdir = opendir(wdir_name) ;
      if(wdir == NULL){
        strcpy(wdir_name,ORroutedir)    ;
        strcat(wdir_name,"World/")      ;
        if(ip)printf(" Trying directory world = %s\n",wdir_name) ;
        wdir = opendir(wdir_name) ;
        if(wdir == NULL){
          printf(" Unable to find world directory\n") ;
          printf(" Program stopping ... \n") ;
          exit(1) ;
        }
      }
 /*
  *  2.  Loop through files
  */
      while ((f_entry = readdir(wdir)) != NULL) {
        len2 = strlen(f_entry->d_name) ;
        if(strcmp(".w",&(f_entry->d_name[len2-2]))!= 0) continue ;
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
  int  istoken ;
  enum token_id itoken, itoken2, itoken3, itoken4 ;
  int     ip1 = 0, ip = 0 ;                  //Controls printing
  double  temp ;

  MSfile  msfile0 ;
  MSfile  *msfile = &msfile0 ;
  MSblock *msblock1, *msblock2, *msblock3, *msblock4 ;
  FILE    *fp ;
  char    my_name[] = "load_world" ;


//      ip1 = wnode->tile_x == 1448 && wnode->tile_y == 10332 ;
      ip = ip1 ;

      if(ip)printf("\n  Enter routine : %s\n",my_name);
      if(ip || ip1 )printf("  File = %s\n",wnode->wfile);

/*
 * =============================================================================
 *  open_msfile reads and checks the first 16 bytes of the file, inflates
 *  compressed files, reads the next 16 bytes and sets the flags.
 * =============================================================================
 */
      l = open_msfile(wnode->wfile, msfile, 0, 0) ;
//      printf(" Routine %s, file %s, l = %i\n",my_name,wnode->wfile,l);
      if(l!=0){
        printf("\n\n  ERROR : Routine open_msfile failed to open file\n\n");
        exit(1) ;
      }
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
      if(0!=l)return 1;
      if(ip)printf(" Level 0 : token TR_WORLDFILE found \n");
/*
 *   Level 1
 */

      for(;;){
        istoken = open_block(msfile,1) ;
        if(-1==istoken){
          if(ip)printf(" Routine %s.  End of file found at level 1\n",my_name);
          close_msfile(msfile) ;
          return 1;
        }
        msblock1 = &(msfile->level[1])  ;
        itoken  = istoken ;

        if(ip)printf("\n  Level 1 : token = %i  %s  : block data length = %i \n",
           itoken,token_idc[itoken], msblock1->length-msblock1->l_label-9) ;
        if(itoken == VDBIDCOUNT){
          wnode->vdbid_count = read_uint32(fp) ;
          if(ip)printf("    vdbid_count = %i\n",wnode->vdbid_count) ;
          wnode->vdb_sphere = (VDbSphere *)malloc(wnode->vdbid_count*
                                            sizeof(VDbSphere)) ;
        }else if(itoken == VIEWDBSPHERE){

          iret = open_named_block(msfile,2,VDBID) ;
          itoken = VDBID ;
          msblock2 = &(msfile->level[2])  ;
          if(ip)printf("    Level 2 : token = %i  %s  : block data length = %i \n",
           itoken,token_idc[itoken], msblock2->length-msblock2->l_label-9) ;
          wnode->vdb_sphere[0].vdb_id = read_uint32(fp) ;
          if(ip)printf("              vdb_id = %i\n",wnode->vdb_sphere[0].vdb_id) ;
          end_block(msfile,2) ;

          iret = open_named_block(msfile,2,POSITION) ;
          itoken = POSITION ;
          msblock2 = &(msfile->level[2])  ;
          if(ip)printf("    Level 2 : token = %i  %s  : block data length = %i \n",
           itoken,token_idc[itoken], msblock2->length-msblock2->l_label-9) ;
          wnode->vdb_sphere[0].Y = read_real32(fp) ;  // MSTS X
          wnode->vdb_sphere[0].Z = read_real32(fp) ;  // MSTS Y
          wnode->vdb_sphere[0].X = read_real32(fp) ;  // MSTS Z
          if(ip)printf("              X, Y, Z = %f %f %f\n",wnode->vdb_sphere[0].X,
                 wnode->vdb_sphere[0].Y, wnode->vdb_sphere[0].Z) ;
          end_block(msfile,2) ;

          itoken   = open_named_block(msfile,2,RADIUS) ;
          itoken = RADIUS ;
          msblock2 = &(msfile->level[2])  ;
          if(ip)printf("    Level 2 : token = %i  %s  : block data length = %i \n",
           itoken,token_idc[itoken], msblock2->length-msblock2->l_label-9) ;
          wnode->vdb_sphere[0].radius = read_real32(fp) ;
          if(ip)printf("              radius   = %f\n",wnode->vdb_sphere[0].radius);
          end_block(msfile,2) ;

          for(i=1;i<(int)wnode->vdbid_count;i++){
            iret = open_named_block(msfile,2,VIEWDBSPHERE) ;
            itoken = VIEWDBSPHERE ;
            msblock2 = &(msfile->level[2])  ;
            if(ip)printf("  Level 2 : token = %i  %s  : block data length = %i \n",
            itoken,token_idc[itoken], msblock2->length-msblock2->l_label-9) ;

            iret = open_named_block(msfile,3,VDBID) ;
            itoken = VDBID ;
            msblock3 = &(msfile->level[3])  ;
            if(ip)printf("  Level 3 : token = %i  %s  : block data length = %i \n",
            itoken,token_idc[itoken], msblock3->length-msblock3->l_label-9) ;
            wnode->vdb_sphere[i].vdb_id = read_uint32(fp) ;
            if(ip)printf("              i, vdb_id   = %i : %i\n",
                                             i,wnode->vdb_sphere[0].vdb_id);
            end_block(msfile,3) ;

            iret = open_named_block(msfile,3,POSITION) ;
            itoken = POSITION ;
            msblock3 = &(msfile->level[3])  ;
            if(ip)printf("  Level 3 : token = %i  %s  : block data length = %i \n",
            itoken,token_idc[itoken], msblock3->length-msblock3->l_label-9) ;
            wnode->vdb_sphere[i].Y = read_real32(fp) ;  // MSTS X
            wnode->vdb_sphere[i].Z = read_real32(fp) ;  // MSTS Y
            wnode->vdb_sphere[i].X = read_real32(fp) ;  // MSTS Z
            if(ip)printf("           i, X, Y, Z = %i : %f %f %f\n",i,wnode->vdb_sphere[i].X,
                 wnode->vdb_sphere[i].Y, wnode->vdb_sphere[i].Z) ;
            end_block(msfile,3) ;

            itoken   = open_named_block(msfile,3,RADIUS) ;
            itoken = RADIUS ;
            msblock2 = &(msfile->level[3])  ;
            if(ip)printf("  Level 3 : token = %i  %s  : block data length = %i \n",
            itoken,token_idc[itoken], msblock3->length-msblock3->l_label-9) ;
            wnode->vdb_sphere[i].radius = read_real32(fp) ;
            if(ip)printf("              i, radius   = %i : %f\n",
                                             i,wnode->vdb_sphere[0].radius);
            end_block(msfile,3) ;
            end_block(msfile,2) ;
          }
        }else if(TR_WATERMARK == itoken){
          wnode->tr_watermark = read_int32(fp) ;
/*
 *  Unknown token
 */
        }else if(309 == itoken){
          skip_to_bblock_end(fp,msblock1) ;

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
                || itoken == SIDING_ALT || itoken == 361
                || itoken == PLATFORM_ALT || itoken == FOREST
                || itoken == LEVELCR
                || itoken == DYNTRACK   || itoken == 306
                || itoken == TRANSFER   || itoken == 363
                || itoken == GANTRY     || itoken == 356
                || itoken == PICKUP_ALT || itoken == 359
                || itoken == HAZARD     || itoken == SIGNAL_ALT
                || itoken == SPEEDPOST ){

/*
 *  Crate new world item
 */
  WorldItem *world_item ;
          if(itoken == PICKUP_ALT || itoken == SIDING_ALT){
            printf(" ################ PICKUP_ALT or SIDING_ALT #### %i ###########\n",itoken);
          }
          world_item = (WorldItem *)malloc(sizeof(WorldItem));
          world_item_init(world_item) ;

          if(NULL == wnode->world_item){
            wnode->world_item = world_item ;
          }else{
            world_item->next  = wnode->world_item ;
            wnode->world_item = world_item ;
          }
          world_item->worldtype = itoken ;
#if 1
          if(itoken == CARSPAWNER || itoken == 357){
            if(ip)printf("  Initialise car spawner variables\n") ;
            world_item->u.car_spawn_obj.carspawnerlist_idx =    0 ;
            world_item->u.car_spawn_obj.car_frequency      =  5.0 ;
            world_item->u.car_spawn_obj.car_av_speed       = 20.0 ;
            world_item->u.car_spawn_obj.list_name          = NULL ;
          }else if(itoken == STATIC || COLLIDEOBJECT == itoken){
            world_item->u.static_obj.no_direct_light  = 0 ;
          }
#endif
/*
 *   Process level 3 tokens
 */
          for(;;){
            itoken2 = open_block(msfile,2) ;
            msblock2 = &(msfile->level[2])  ;
            if(ip)printf("    Level 2 : token = %i  %s  : block data length = %i \n",
            itoken2,token_idc[itoken2], msblock2->length-msblock2->l_label-9) ;
            if(UID == itoken2){
              world_item->uid = read_uint32(fp) ;
              if(ip)printf("      world_item->uid         = %i\n",world_item->uid);
            }else if(FILENAME == itoken2){
// Strings are unicode, headed by two byte length
              world_item->filename = read_ucharz(fp) ;
              add_shape_name_to_world_btree(wnode, world_item) ;
              if(ip || ip1)printf("  filename = %s\n",world_item->filename) ;
            }else if(POSITION == itoken2){
              world_item->X = read_real32(fp) ;  // MSTS X
              world_item->Z = read_real32(fp) ;  // MSTS Y
              world_item->Y = read_real32(fp) ;  // MSTS Z
              if(ip1)printf("            POSITION  :: world_item->X, Y, Z     = %f %f %f\n",
                   world_item->X, world_item->Y,
                   world_item->Z) ;
            }else if(QDIRECTION == itoken2){
              world_item->A = read_real32(fp) ;    //  Direction vector
              world_item->B = read_real32(fp) ;    //    times
              world_item->C = read_real32(fp) ;    //      sin(rotation/2)
              world_item->D = read_real32(fp) ;    //  |vector| times cos(rotation/2)
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
              if(ip1)printf("            QDIRECTION :: world_item->AX, AY, AZ, ANG = "
                            "%f %f %f : %f\n",
                   world_item->AX, world_item->AY,
                   world_item->AZ, world_item->ANG) ;
            }else if(VDBID == itoken2){
              world_item->vdb_id = read_uint32(fp) ;
              if(ip)printf("      world_item->vdb_id      = %i\n",world_item->vdb_id);
            }else if(COLLIDEFLAGS == itoken2){
              world_item->collide_flags = read_uint32(fp) ;
              if(ip)printf("      world_item->collide_flags = %i\n",world_item->collide_flags);
            }else if(STATICDETAILLEVEL == itoken2){
              world_item->static_detail_level = read_uint32(fp) ;
              if(ip)printf("      world_item->static_detail_level = %i\n",
                                 world_item->static_detail_level);
            }else if(STATICFLAGS == itoken2){
              world_item->static_flags = read_uint32(fp) ;
              if(ip)printf("      world_item->static_flags = %i\n",world_item->static_flags);
            }else if(TRITEMID == itoken2){
              world_item->tr_item_db[world_item->n_tr_item]      = read_int32(fp) ;
              world_item->tr_item_db_id[world_item->n_tr_item++] = read_int32(fp) ;
            }else if(MAXVISDISTANCE == itoken2){
                world_item->max_vis_distance             = read_real32(fp) ;

// TRACK OBJECT
            }else if(TRACKOBJ == itoken && SECTIONIDX == itoken2){
              world_item->u.track_obj.section_idx        = read_uint32(fp) ;
              if(ip)printf("      world_item->*.section_idx = %i\n",
                                  world_item->u.track_obj.section_idx);
            }else if(TRACKOBJ == itoken && ELEVATION == itoken2){
              world_item->u.track_obj.elevation          = read_real32(fp) ;
            }else if(TRACKOBJ == itoken && JNODEPOSN == itoken2){
              world_item->u.track_obj.tile_x             = read_int32(fp)  ;
              world_item->u.track_obj.tile_y             = read_int32(fp)  ;
              world_item->u.track_obj.X                  = read_real32(fp) ;
              world_item->u.track_obj.Y                  = read_real32(fp) ;
              world_item->u.track_obj.Z                  = read_real32(fp) ;
            }else if(TRACKOBJ == itoken && COLLIDEFLAGS == itoken2){
              world_item->u.track_obj.collide_flags     = read_uint32(fp)  ;
            }else if(TRACKOBJ == itoken && COLLIDEFUNCTION == itoken2){
//  Skip to end of block
              for(i=ftell(fp);i<msblock2->byte_end;i++){
                fgetc(fp) ;
              }
#if 1
//  CARSPAWNER OBJECT
            }else if(itoken == CARSPAWNER || itoken == 357){
               if(CARFREQUENCY == itoken2){
                 world_item->u.car_spawn_obj.car_frequency = read_real32(fp) ;
               }else if(CARAVSPEED == itoken2){
                 world_item->u.car_spawn_obj.car_av_speed  = read_real32(fp) ;
               }
#endif
//  FOREST OBJECT
            }else if(FOREST == itoken && TREETEXTURE == itoken2){
              world_item->u.forest_obj.tree_texture     = read_ucharz(fp) ;
            }else if(FOREST == itoken && SCALERANGE == itoken2){
              world_item->u.forest_obj.scale_range_min  = read_real32(fp) ;
              world_item->u.forest_obj.scale_range_max  = read_real32(fp) ;
            }else if(FOREST == itoken && AREA == itoken2){
              world_item->u.forest_obj.X                = read_real32(fp) ;
              world_item->u.forest_obj.Y                = read_real32(fp) ; // MSTS Z
            }else if(FOREST == itoken && POPULATION == itoken2){
              world_item->u.forest_obj.population       = read_int32(fp) ;
            }else if(FOREST == itoken && TREESIZE == itoken2){
              world_item->u.forest_obj.width            = read_real32(fp) ;
              world_item->u.forest_obj.height           = read_real32(fp) ;
//  SPEEDPOST OBJECT
            }else if(SPEEDPOST == itoken && SPEED_DIGIT_TEX == itoken2){
              world_item->u.speed_post_obj.speed_digit_tex  = read_ucharz(fp) ;
            }else if(SPEEDPOST == itoken && SPEED_SIGN_SHAPE == itoken2){
              l = read_int32(fp) ;
              world_item->u.speed_post_obj.shapes_info = (float *)
                                              malloc(l*4*sizeof(float)) ;
              for(i=0;i<l;i++){
                world_item->u.speed_post_obj.shapes_info[i*4+0] = read_real32(fp) ;
                world_item->u.speed_post_obj.shapes_info[i*4+1] = read_real32(fp) ;
                world_item->u.speed_post_obj.shapes_info[i*4+2] =-read_real32(fp) ;
                world_item->u.speed_post_obj.shapes_info[i*4+3] = read_real32(fp) ;
              }
            }else if(SPEEDPOST == itoken && SPEED_TEXT_SIZE == itoken2){
              world_item->u.speed_post_obj.size        = read_real32(fp) ;
              world_item->u.speed_post_obj.dx          = read_real32(fp) ;
              world_item->u.speed_post_obj.dy          = read_real32(fp) ;
//  SIDING  OBJECT
            }else if((SIDING_ALT == itoken || 361 == itoken) && SIDINGDATA == itoken2){
              world_item->u.siding_obj.siding_data      = read_int32(fp) ;
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
                  dyn_trk_sect->is_curved              = read_uint32(fp) ;
                  if(ip)printf("      dyn_trk_sect->is_curved      = %i\n",
                                     dyn_trk_sect->is_curved);
                  end_block(msfile,4) ;

                dyn_trk_sect->uid                      = read_uint32(fp) ;
                dyn_trk_sect->param_1                  = read_real32(fp) ;
                dyn_trk_sect->param_2                  = read_real32(fp) ;
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
              world_item->u.dyn_track_obj.section_idx      = read_uint32(fp) ;
              if(ip)printf("      section_idx = %i\n",world_item->u.dyn_track_obj.section_idx);
            }else if((DYNTRACK == itoken|| 306 == itoken) && ELEVATION == itoken2){
              world_item->u.dyn_track_obj.elevation        = read_real32(fp) ;
              if(ip)printf("      elevation   = %f\n",world_item->u.dyn_track_obj.elevation);
            }else if((DYNTRACK == itoken|| 306 == itoken) && COLLIDEFLAGS == itoken2){
              world_item->u.dyn_track_obj.collide_flags   = read_uint32(fp) ;
              if(ip)printf("      collide_flags = %i\n",world_item->u.dyn_track_obj.collide_flags);
//  TRANSFER OBJECT
            }else if((itoken == TRANSFER || itoken == 363) && WIDTH == itoken2){
              world_item->u.transfer_obj.width             = read_real32(fp) ;
            }else if((itoken == TRANSFER || itoken == 363) && HEIGHT == itoken2){
              world_item->u.transfer_obj.width             = read_real32(fp) ;
//  SIGNAL OBJECT
            }else if(itoken == SIGNAL_ALT && SIGNALSUBOBJ == itoken2){
              world_item->u.signal_obj.signal_sub_object   = read_uint32(fp) ;
            }else if(itoken == SIGNAL_ALT && SIGNALUNITS == itoken2){
              l                                          = read_uint32(fp) ;
              world_item->u.signal_obj.n_signal_units    = l               ;
              world_item->u.signal_obj.sub_object = (int *)malloc(l*sizeof(int)) ;
              world_item->u.signal_obj.u_data1    = (uint *)malloc(l*sizeof(uint)) ;
              world_item->u.signal_obj.tr_item    = (uint *)malloc(l*sizeof(uint)) ;
              for(i=0;i<l;i++){
// SUB BLOCK
                iret    = open_named_block(msfile,3,SIGNALUNIT) ;
                itoken3 = SIGNALUNIT ;
                msblock3 = &(msfile->level[3])  ;
                if(ip)printf("      Level 3 : token = %i  %s  : block data length = %i \n",
                itoken3,token_idc[itoken3], msblock3->length-msblock3->l_label-9) ;
                world_item->u.signal_obj.sub_object[i]    = read_int32(fp) ;

                iret    = open_named_block(msfile,4,TRITEMID) ;
                itoken4 = TRITEMID ;
                msblock4 = &(msfile->level[4])  ;
                if(ip)printf("      Level 4 : token = %i  %s  : block data length = %i \n",
                itoken4,token_idc[itoken4], msblock4->length-msblock4->l_label-9) ;
                world_item->u.signal_obj.u_data1[i]        = read_uint32(fp) ;
                world_item->u.signal_obj.tr_item[i]        = read_uint32(fp) ;
                end_block(msfile,4) ;
                end_block(msfile,3) ;
// ENDOF SUB BLOCK
              }
//  LEVELCR OBJECT
            }else if(itoken == LEVELCR && LEVELCRPARAMETERS == itoken2){
              world_item->u.levelcr_obj.warning_time      = read_real32(fp) ;
              world_item->u.levelcr_obj.minimum_distance  = read_real32(fp) ;
            }else if(itoken == LEVELCR && CRASHPROBABILITY == itoken2){
              world_item->u.levelcr_obj.crash_probability = read_int32(fp) ;
            }else if(itoken == LEVELCR && LEVELCRDATA == itoken2){
                world_item->u.levelcr_obj.level_cr_data_1  = read_int32(fp) ;
                world_item->u.levelcr_obj.level_cr_data_2  = read_int32(fp) ;
                world_item->u.levelcr_obj.visible =
                  (world_item->u.levelcr_obj.level_cr_data_1 & 0x1) == 0   ;
                world_item->u.levelcr_obj.silent =
                  !(world_item->u.levelcr_obj.visible) ||
                  (world_item->u.levelcr_obj.level_cr_data_1 & 0x6) == 0x6 ;
            }else if(itoken == LEVELCR && LEVELCRTIMING == itoken2){
                world_item->u.levelcr_obj.initial_timing   = read_real32(fp) ;
                world_item->u.levelcr_obj.serious_timing   = read_real32(fp) ;
                world_item->u.levelcr_obj.anim_timing      = read_real32(fp) ;
//  PICKUP OBJECT
            }else if((PICKUP_ALT == itoken || 359 == itoken) && SPEEDRANGE == itoken2){
              world_item->u.pickup_obj.min_mps             = read_real32(fp) ;
              world_item->u.pickup_obj.max_mps             = read_real32(fp) ;
            }else if((PICKUP_ALT == itoken || 359 == itoken) && PICKUPTYPE == itoken2){
              world_item->u.pickup_obj.pickuptype          = read_uint32(fp) ;
              world_item->u.pickup_obj.pickuptype_2        = read_uint32(fp) ;
            }else if((PICKUP_ALT == itoken || 359 == itoken) && PICKUPANIMDATA == itoken2){
              world_item->u.pickup_obj.pickup_options      = read_real32(fp) ;
              world_item->u.pickup_obj.animation_speed     = read_real32(fp) ;
              if(0.0 == world_item->u.pickup_obj.animation_speed)
                 world_item->u.pickup_obj.animation_speed = 1.0 ;
            }else if((PICKUP_ALT == itoken || 359 == itoken) && PICKUPCAPACITY == itoken2){
              world_item->u.pickup_obj.quantity_available_kg = read_real32(fp) ;
              world_item->u.pickup_obj.feed_rate_kgps        = read_real32(fp) ;
            }else if((PICKUP_ALT == itoken || 359 == itoken) && COLLIDEFLAGS == itoken2){
              world_item->u.pickup_obj.collide_flags         = read_uint32(fp) ;
//  PLATFORM
            }else if(PLATFORM_ALT == itoken && PLATFORMDATA == itoken2){
              world_item->u.platform_obj.platform_data       = read_uint32(fp) ;
//  STATIC and COLLIDEOBJECT == itoken
            }else if((STATIC == itoken || COLLIDEOBJECT == itoken) && NODIRLIGHT == itoken2){
                world_item->u.static_obj.no_direct_light = 1 ;
            }else if(COLLIDEOBJECT == itoken && COLLIDEFUNCTION == itoken2){
              skip_to_bblock_end(fp,msblock2) ;
            }else if(STATIC == itoken && MATRIX3X3 == itoken2){
              skip_to_bblock_end(fp,msblock2) ;
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
//  Skip to end of block
//              for(i=ftell(fp);i<msblock2->byte_end;i++){
//                fgetc(fp) ;
//              }
              skip_to_bblock_end(fp,msblock2) ;
            }
            end_block(msfile,2) ;
            if(ftell(fp) == msfile->level[1].byte_end) break ;
          }
          for(i=ftell(fp);i<msblock2->byte_end;i++){
            fgetc(fp) ;
          }
/*
 *  End of level 2 procesing
 */
        }else{
           printf(" Level 1 token not recognised.\n   Token = %i  %s  : block data length = %i \n",
           itoken,token_idc[itoken], msblock1->length-msblock1->l_label-9) ;
          for(i=ftell(fp);i<msblock1->byte_end;i++){
            fgetc(fp) ;
          }
        }
        end_block(msfile,1) ;
      }
      if(0 && iret){} ;                         //  Keep the compiler happy
      close_msfile(msfile) ;
      printf(" Normal return\n");
      return 0 ;
}

int world_item_init(WorldItem *world_item){

      world_item->next          = NULL ;
      world_item->filename      = NULL ;
      world_item->snode         = NULL ;
      world_item->worldtype     = 0    ;
      world_item->uid           = 0    ;
      world_item->static_flags  = 0    ;
      world_item->collide_flags = 0    ;
      world_item->X             = 0.0  ;
      world_item->Y             = 0.0  ;
      world_item->Z             = 0.0  ;
      world_item->A             = 0.0  ;
      world_item->B             = 0.0  ;
      world_item->C             = 0.0  ;
      world_item->D             = 0.0  ;
      world_item->vdb_id        = 0    ;
      world_item->iz_off        = 0    ;
      world_item->n_tr_item     = 0    ;
      world_item->max_vis_distance    = 10000.0 ;
      world_item->static_detail_level = 0       ;

      return 0;
}

int add_shape_pointers_to_world_items(){

  int  k      ;
  int  ip = 0 ;   // 0 = no printing, -1 = error printing only
  int  id = 0 ;
  WorldNode    *wnode ;
  WorldItem    *witem ;
  ShapeNode    *snode ;
  char         *wname,
               *sname  ;
  char         my_name[] = "add_shape_pointers_to_world_items" ;

        if(ip)printf(" Enter routine %s\n",my_name) ;
//        printf("    wnode0 = %p\n",(void *)worldlist_beg) ;
        for(wnode=worldlist_beg; wnode != NULL; wnode=wnode->next){
//          printf(" ++++++++++++++++++++++++++++++++++++++++++++++++++ \n") ;
//          printf(" New world file : wnode = %p\n",(void *)wnode) ;
//          printf("                   name = %s\n",wnode->name) ;
//          printf("                 witem0 = %p\n",(void *)wnode->world_item) ;
//          printf("                   next = %p\n",(void *)wnode->next) ;
//          if(wnode->tile_x != tst_tile_x || wnode->tile_y != tst_tile_y)continue ;
          for(witem=wnode->world_item; witem != NULL; witem=witem->next){
            if(NULL != witem->filename){
//              id = (witem->uid == 4485) ;
              wname = strdup(witem->filename) ;
              for(k=0;k<(int)strlen(wname);k++)wname[k] = tolower(wname[k]) ;
              wname[strlen(wname)-2] = '\0';
              if(ip || id){
                printf("     witem = %p,  uid = %i,  tile_x = %i %i,  tile_y = %i %i\n",
                               (void *)witem, witem->uid,
                               wnode->tile_x, wnode->tile_x-tile_west,
                               wnode->tile_y, wnode->tile_y-tile_south) ;
                printf("     type       = %i   %s\n",
                                witem->worldtype,token_idc[witem->worldtype]) ;
                printf("     shape name = %s\n",wname) ;
              }
              for(snode = shapelist_beg; NULL != snode; snode=snode->next ){
                sname = strdup(snode->name) ;
                for(k=0;k<(int)strlen(sname);k++)sname[k] = tolower(sname[k]) ;
                if(0 == strcmp(wname,sname)){
                  witem->snode  = snode ;
                  snode->basic  = 0     ;
                  snode->needed = 0     ;
                  snode->loaded = 0     ;
                  if(ip)printf(" Active list : wnode = %p, witem = %p, add shape: %s  ::  %s \n",(void *)wnode, (void *)witem, sname, witem->snode->name) ;
                  free(sname) ;
                  break ;
                }
                free(sname) ;
              }
              if(NULL == witem->snode && ip){
                printf(" Unable to find shapefile corresponding to %s\n",wname);
              }
              free(wname) ;
            }
          }
        }
  return 0 ;
}

int  list_wfile_item(WorldItem *wi){

      printf("   WorldItem     uid   = %i \n",wi->uid) ;
      printf("   WorldItem     type  = %i %s\n",wi->worldtype, token_idc[wi->worldtype]) ;
      printf("   WorldItem     static flags = %8x \n",wi->static_flags) ;
      if(wi->static_flags & SF_RoundShadow)printf("                   Round Shadow\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Rectangular Shadow\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Treeline Shadow\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Dynamic Shadow\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Any Shadow\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Terrain\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Animate\n");
      if(wi->static_flags & SF_RoundShadow)printf("                   Global\n");
      printf("   WorldItem     position = %f %f %f\n",wi->X,wi->Y,wi->Z) ;
      printf("   WorldItem     rotation = %f %f %f %f\n",wi->AX,wi->AY,wi->AZ,wi->ANG) ;


      return 0;
}

void  add_shape_name_to_world_btree(WorldNode *wnode, WorldItem *world_item){

uint   i ;
char   *name = NULL ;
BTree  *n =  wnode->shape_tree ;
char   *my_name = "add_shape_name_to_world_btree" ;

//  Make copy of filename without directories and extension
//  and convert to lower case.

      name = zr_basename2(world_item->filename) ;
      for(i=0;i<strlen(name);i++)name[i] = tolower(name[i]) ;

//  Compare

      if(NULL != n){
        if(find_btree(n,name)){
          free(name) ;
          return     ;      // Name already exists
        }
      }

//  Add new item

      wnode->shape_tree =
           insert_node(wnode->shape_tree,name,NULL) ;
      return ;
}

void add_world_shapes_to_master(void *b){

BTree *bb = (BTree *)b ;

      if(shape_master){
        if(find_btree(shape_master, bb->index)) return  ;
      }
      shape_master = insert_node(shape_master, bb->index, NULL) ;
      return ;
}


