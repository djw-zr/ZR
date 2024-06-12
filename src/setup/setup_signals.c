/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  setup_signals.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for initialising signals and related data structues
 *
 *==============================================================================
 */


/*
 *  Routine to intialise the date structures needed to represent each
 *  of the signals in the layout.
 *
 *   This routine is called after all the signal descriptiogn file and
 *   the many world files have been read.
 */

#include "y.tab.h"

int init_signal_db(SignalDB *s) ;

/*
 * *****************************************************************************
 *  Routine to search through the signal script tree for the script with the
 *  given name.  It returns a pointer to the start of the script in memory.
 * *****************************************************************************
 */

nodeType  *find_signal_script(char *signal_type_name){
  int      ip = 0 ;
  int      ifound = 0 ;
  int      itry, iret ;
  nodeType *n1, *n2, *n3, *n4 ;
  char     *name ;
  char     *my_name = "find_signal_script" ;

      if(ip){
        printf("\n===============================================\n") ;
        printf("\n    Routine %s \n",my_name) ;
        printf("\n    Signal %s \n",signal_type_name) ;
      }
      for(itry = 0; itry < 2; itry++){
        name = NULL ;
        n1 = sTree ;
        n1 = n1->opr.op[0] ;
        n2 = NULL ;

        for(;;){
          if(!n1)break ;
          if(ip){
            printf("\n================================================\n") ;
            printf("   n1 = %p\n", (void *)n1 ) ;
            printf("   n1  type = %i\n",
                      n1->type) ;
            printf("   n1  type = %i, oper = %i \n",
                      n1->type, n1->opr.oper) ;
            printf("   n1  %p type = %i, oper = %i %s\n", (void *) n1,
                      n1->type, n1->opr.oper, sigscr_token_string(n1->opr.oper)) ;
          }
/*
*  n1 points to the top of the part of the tree not searched.
*  If it is a SC_SCRIPT_LIST, the right hand [1] branch should be a SC_SCRIPT
*  Alternatively it should be a SC_SCRIPT (the right hand branch at the bottom
*  of the tree).
*/
          if(n1->type == typeOpr && n1->opr.oper == SC_SCRIPT_LIST){
            n2 = n1->opr.op[1] ;
          }else if(n1->type == typeOpr && n1->opr.oper == SC_SCRIPT){
            n2 = n1 ;
          }else{
            printf("  Routine %s. Search for SC-SCRIPT or SC-SCRIPT_LIST failed\n",
                      my_name) ;
            n1 = NULL ; break ;
          }
          if(ip)printf("     n2  %p type = %i, oper = %i %s\n", (void *) n2,
                            n2->type, n2->opr.oper, sigscr_token_string(n2->opr.oper)) ;
/*
*  n2 should now point to a SC_SCRIPT
*  in which case its left hand [0] branch should be the name of a signal type
*/
          if(n2->type == typeOpr && n2->opr.oper == SC_SCRIPT){
            n3 = n2->opr.op[0] ;
          }else{
            printf("  Routine %s. Search for SC-SCRIPT failed\n",
                      my_name) ;
            n1 = NULL ; break ;
          }
          if(ip)printf("     n3  %p type = %i, name = %s\n", (void *) n3,
                            n3->type, n3->str.name) ;
/*
*  Check that it is a name (string)
*/
          if(n3->type == typeStr){
            name = n3->str.name ;
            if(ip)printf("  n3 name = %s, signal_type_name = %s\n",name, signal_type_name) ;
          }else{
            printf("  Routine %s. Search for 'name' failed.\n",
                      my_name) ;
            printf("  Pointers: n1 = %p, n2 = %p, n3 = %p\n",
                              (void *)n1, (void *)n2, (void *)n3) ;
            printf("  Types n1 = %i, n2 = %i, n3 = %i\n",
                              n1->type, n2->type, n3->type) ;
            printf("  Oper n1 = %i, n2 = %i\n",
                              n1->opr.oper, n2->opr.oper) ;
            n1 = NULL ; break ;
          }
/*
*  Check that it is the right name.
*/
          if(itry == 0){
            iret = !strcmp(name,signal_type_name) ;
          }else{
            iret = !strcmp_ic(name,signal_type_name) ;
          }
          if(iret){
            if(ip || 0)printf("  Routine %s.  Script for '%s' found\n",
                            my_name,signal_type_name) ;
            ifound = 1;
            break ;
          }
/*
*  The name has not been found.  If n1 is at a  SC_SCRIPT_LIST
*  then continue along the left hand branch to the next node
*  If n1 == n2, the final SC_SCRIPT has just been processed and
*  there are no more nodes
*/
          if(ip)printf("  Loop n1 = %p, n2 = %p\n",(void *)n1, (void *)n2) ;
          if(n1 != n2){
            n1 = n1->opr.op[0] ;
          }else{
            break ;
          }
        }
        if(ifound)break ;
      }
      if(!ifound){
        printf("  AAAR Routine '%s', failed to find script for signal '%s'\n",
                  my_name, signal_type_name) ;
        return NULL ;
      }
      return n2 ;
}


int print_signal_data(SignalDB *signal){

  int ip  = 0  ;
  int ips = 0 ;  // print summary of all signals
  int i, j, k, n, m, it, im ;
  int isignal = 0 ;

  WorldNode   *wnode ;
  WorldItem   *witem ;
  SignalObj   *sig_object  ;
  SigType     *sig_type   ;    //  Item in list of raw signals
  SigShape    *sig_shape  ;
  SigSubObj   *sig_subobj ;
  TrkItem     *trk_item,
              *trk_item2  ;
  ShapeNode   *snode ;
  DistLevel   *dist_level ;
  nodeType    *sig_script ;


  char      *world_filename     ;
  char      *sig_shape_filename ;
  char      *sig_type_name ;
  char      *string1 ;
  char      *head_name ;
  char      *sm_name   ;            //  Name of shape matrix
  char      *name      ;
  char      *my_name = "print_signal_data";

      witem = signal->witem ;
      wnode = witem->world_node ;
      sig_object = signal->sig_object ;

      printf("\n****************************************************\n") ;
      printf("  Routine %s.\n",my_name) ;
      printf("  Signal. uid = %i\n",signal->uid) ;
      printf("****************************************************\n\n") ;

      world_filename = zr_basename2(witem->filename);
      str2lc(world_filename) ;
      printf("    Signal shape name = %s  position = %i %i :: %f %f %f\n",
                world_filename, wnode->tile_x, wnode->tile_y,
                witem->X, witem->Y, witem->Z) ;
      free(world_filename) ;
      printf("    n_tr_item = %i\n",witem->n_tr_item)     ;
      printf("      tr_item_db    = %i, %i, %i, %i\n",
                            witem->tr_item_db[0],   witem->tr_item_db[1],
                            witem->tr_item_db[2],   witem->tr_item_db[3]   ) ;
      printf("      tr_item_db_id = %i, %i, %i, %i\n",
                            witem->tr_item_db_id[0],witem->tr_item_db_id[1],
                            witem->tr_item_db_id[2],witem->tr_item_db_id[3]) ;
      printf("    signal_sub_object = %i\n", witem->u.signal_obj.signal_sub_object);
      printf("    n_signal_units    = %i\n", witem->u.signal_obj.n_signal_units) ;
      for(k=0;k<witem->u.signal_obj.n_signal_units;k++){
        printf("      sub_object = %i, u_data1 = %i, track_item = %i\n",
                witem->u.signal_obj.sub_object[k],witem->u.signal_obj.u_data1[k],
                witem->u.signal_obj.tr_item[k]) ;
      }
      printf("    n_matrices     = %i\n", witem->u.signal_obj.n_matrices) ;
      sig_object = &(witem->u.signal_obj) ;
      for(k=0;k<(int)sig_object->n_matrices;k++){
        printf("      matrix     = %i, skip = %i, signal = %p, name = %s\n",
                             k, sig_object->sm_skip ? sig_object->sm_skip[k] : 0,
                                sig_object->sm_signal ? (void *)sig_object->sm_signal[k] : NULL,
                                         witem->snode->matrix[k].name) ;
      }

      printf("\n  Signal item variables:\n") ;
      trk_item = signal->trk_item ;
      printf("    Track item = %p,  uid = %i, Signal_Type_Name = %s\n",
              (void *)trk_item, trk_item->uid, trk_item->signal_type_name) ;
      printf("                 Position = %i %i :: %f %f %f\n",
                  trk_item->tile_east_x, trk_item->tile_north_z,
                  trk_item->east_x, trk_item->north_z, trk_item->height_y) ;
      printf("                 Type of node = %i  %s\n",
                  trk_item->type_of_node,token_trackdb[trk_item->type_of_node]) ;
      printf("                 Signal   flags = %s,  direction = %i,  data3 = %f\n",
            trk_item->signal_data1, trk_item->signal_direction, trk_item->signal_data3) ;
      printf("                 Signal dirs = %i ::",trk_item->signal_num_dirs) ;
      for(k=0;k<(int)trk_item->signal_num_dirs;k++){
            printf("           TrackNode = %i",(int)trk_item->signal_dirs[k][0]) ;
            printf("  Sd1 = %i",(int)trk_item->signal_dirs[k][1]) ;
            printf("  LinkLRPath = %i",(int)trk_item->signal_dirs[k][2]) ;
            printf("  Sd3 = %i",(int)trk_item->signal_dirs[k][3]) ;
      }
      printf("\n") ;
      for(k=0;k<(int)trk_item->signal_num_dirs;k++){
        m = trk_item->signal_dirs[k][0] ;
        trk_item2 = &(track_db.trk_items_array[m]) ;
        n = trk_item2->type_of_node ;
        if(n < 0 || n > 1000){
          printf("  Error in routine %s",my_name) ;
          printf("  trk_item2->type_of_node = %i,.  Out of range",n) ;
        }else{
          printf("               Type of node = %i %i  %s\n",
            m,trk_item2->type_of_node,token_trackdb[trk_item2->type_of_node]) ;
        }
      }
      sig_type = signal->sig_type ;
      printf("\n    Signal Type.  Name = %s.  Semaphore = %i, No gantry = %i\n",
                  sig_type->name, sig_type->semaphore, sig_type->no_gantry) ;
      printf("                  n_sig_lights = %i, n_sig_dstates = %i, n_sig_aspects = %i\n",
                  sig_type->n_sig_lights, sig_type->n_sig_dstates, sig_type->n_sig_aspects) ;
      sig_shape = signal->sig_shape ;
      printf("    Signal Shape.  Name = %s, n_sig_subobjs = %i\n",
                            sig_shape->name, sig_shape->n_sig_subobjs) ;
      sig_subobj = signal->sig_subobj ;
      printf("    Signal Shape Sub-Object.  Name = %s, %s, subs_type = %s\n", sig_subobj->name, sig_subobj->full_name, sig_subobj->sig_script_type) ;
      printf("    Signal flags:  ");
      if(sig_subobj->sig_signal_head || sig_subobj->sig_gradient_plate || sig_subobj->sig_number_plate){
        if(sig_subobj->sig_signal_head)  printf("SIGNAL_HEAD ");
        if(sig_subobj->sig_gradient_plate)printf("GRADIENT_PLATE ");
        if(sig_subobj->sig_number_plate) printf("NUMBER_PLATE ");
        printf("  ::  ") ;
      }
      if(sig_subobj->sig_default)    printf("DEFAULT ");
      if(sig_subobj->sig_jn_link)    printf("JN_LINK (%i) ", sig_subobj->sig_jn_link);
      if(sig_subobj->sig_back_facing)printf("BACK_FACING ");
      if(sig_subobj->sig_optional)   printf("OPTIONAL ");
      printf("\n") ;

      if(sig_shape->n_sig_subobjs>1){
        printf("\n    Full list of Signal Sub-Objects\n") ;
        for(k=0;k<sig_shape->n_sig_subobjs;k++){
          sig_subobj = &(sig_shape->sig_subobj[k]) ;
          printf("      Sub-Object.  k = %i, Name = %s, %s, subs_type = %s\n",
                  k, sig_subobj->name, sig_subobj->full_name, sig_subobj->sig_script_type) ;
          printf("      Signal flags:  ");
          if(sig_subobj->sig_signal_head || sig_subobj->sig_gradient_plate || sig_subobj->sig_number_plate){
            if(sig_subobj->sig_signal_head)  printf("SIGNAL_HEAD ");
            if(sig_subobj->sig_gradient_plate)printf("GRADIENT_PLATE ");
            if(sig_subobj->sig_number_plate) printf("NUMBER_PLATE ");
            printf("  ::  ") ;
          }
          if(sig_subobj->sig_default)    printf("DEFAULT ");
          if(sig_subobj->sig_jn_link)    printf("JN_LINK (%i)  ", sig_subobj->sig_jn_link);
          if(sig_subobj->sig_back_facing)printf("BACK_FACING ");
          if(sig_subobj->sig_optional)   printf("OPTIONAL ");
          printf("\n") ;
          printf("      Link = %i\n",sig_subobj->n_sig_jn_lnk) ;
          for(m=0;m<sig_subobj->n_sig_jn_lnk;m++){
            printf("      Link %i = %i\n",m,sig_subobj->sig_jn_lnk_a[m]) ;
          }
        }
      }
      sig_script = signal->sig_script ;
      printf("\n    Signal Script pointer = %p\n", (void *) sig_script) ;
      printf("                  name    = %s\n", sig_script->opr.op[0]->str.name) ;

      printf("\n    SignalObj = %i, n_signal_units = %i\n",
                      sig_object->signal_sub_object,
                      sig_object->n_signal_units    ) ;
      for(k=0;k<sig_object->n_signal_units;k++){
        printf("        Sub_object = %i, u_data1 = %i, tr_item = %i\n",
            sig_object->sub_object[k], sig_object->u_data1[k],
            sig_object->tr_item[k])     ;
      }

      printf("  World Item.   UID = %i, Shape File  = %s\n",
                   witem->uid, witem->filename) ;
      n = witem->n_animations ;
      printf("                n_animations = %i\n",n) ;
      for(k=0;k<n;k++){
        printf("                animations   = %i %p\n",
                                    k,(void *)witem->animations[k]) ; ;
      }
      printf("****************************************************\n\n") ;
      return 0 ;
}

/*
 *  Routine to loop over all world tiles and
 *  setup individual signals.
 */

int setup_signals(void){

  int ip = 0  ;
  int ips = 0 ;  // print summary of all signals
  int i, j, k, n, m, im ;
  int isignal = 0 ;
  int n_signal_units ;
  int n_matrices ;

  WorldNode   *wnode ;
  WorldItem   *witem ;
  ShapeNode   *snode ;
  SignalObj   *sig_object  ;
  SignalDB    *signal = NULL;
  SigType     *sig_type   ;    //  Item in list of raw signals
  SigShape    *sig_shape  ;
  SigSubObj   *sig_subobj ;
  TrkItem     *trk_item   ;
  DistLevel   *dist_level ;
  nodeType    *sig_script ;
  int         *hierarchy  ;


  char      *world_filename     ;
  char      *sig_shape_filename ;
  char      *sig_type_name ;
  char      *string1 ;
  char      *head_name ;
  char      *sm_name  = NULL ;
  char      *sm_name2 = NULL ;            //  Name of shape matrix

  char *my_name = "setup_signals" ;

      if(ip)printf("   Enter routine %s\n",my_name) ;
/*
 * *****************************************************************************
 *  Loop through the world file, find every signal and add to a list of signals
 * *****************************************************************************
 */
      for(wnode=worldlist_beg; wnode!= NULL; wnode=wnode->next){
        for(witem=wnode->world_item; witem!= NULL; witem=witem->next){
//          ip = (232 == witem->uid) ;  //  Crossing
//          ip = (415 == witem->uid) ;
//          ip = (104 == witem->uid) ;  //USA2
//          ip = (1876 == witem->uid) ;  // New Forest route - Weymouth platform 4
//          ip = (1909 == witem->uid) ;  // New Forest route - Weymouth approach
//          ip = 413 == witem->uid ;
/*
 *  Three light signal at York 440  MECoast route
 */
//            ip =  590 == witem->uid
//         &&   -6073 == witem->world_node->tile_x && 15048 == witem->world_node->tile_y ;
/*
 *  Platform signal at Mount Victoria - Zig-Zag Route
 */
//           ip =  410 == witem->uid
//                 &&  1454 == witem->world_node->tile_x
//                 && 10325 == witem->world_node->tile_y ;
/*
 *  Approach signal at Mount Victoria - Zig-Zag Route
 */
//           ip =  (414 == witem->uid || 415 == witem->uid)
//                 &&  1454 == witem->world_node->tile_x
//                 && 10325 == witem->world_node->tile_y ;
/*
 *  Signal outside Philidophis :  Route USA1
 */
//           ip =  4353 == witem->uid
//                 && -11009 == witem->world_node->tile_x
//                 &&  14317 == witem->world_node->tile_y ;
          if(ip){
            printf("\n ****************************************************\n") ;
            printf(" ****** Signals : New WorldItem %5i****************\n\n",
                                                                    witem->uid) ;
            printf("  Tile %i %i.  World item %4i,   type = %-14s,",
                   wnode->tile_x, wnode->tile_y,
                   witem->uid, token_idc[witem->worldtype] ) ;
            if(witem->snode)printf("   name = %s",witem->snode->name) ;
            printf("\n") ;
          }
/*
 *   Skip processing if this is not a signal
 *   otherwise load pointer to signal object in world item
 */
          if(witem->worldtype != SIGNAL_ALT) continue ;
          snode      = witem->snode ;
          sig_object = &(witem->u.signal_obj) ;
//          ip =  1930 == witem->uid ;
          if(ip){
            printf("\n ****************************************************\n") ;
            printf("\n  World Item.  Index = %i, Type = %i : %s\n",
                    witem->uid,witem->worldtype,token_idc[witem->worldtype])  ;
            printf("    nmatrices = %i\n",snode->nmatrices) ;
          }
/*
 * *****************************************************************************
 *  For each active head:
 *    allocate space for a signal pointer and
 *    initialise.
 * *****************************************************************************
 */
          n = n_signal_units = sig_object->n_signal_units ;
          if(n>0){
            sig_object->signal = (SignalDB **)malloc(n*sizeof(SignalDB *)) ;
            for(i=0;i<n;i++){
              sig_object->signal[i] = NULL ;
              n_signals ++ ;  // Count number of signal structures (zr.h)
            }
          }
/*
 * *****************************************************************************
 *  Allocate space used for each of the shape matrices and initialise
 * *****************************************************************************
 */
          n = n_matrices = snode->nmatrices ;
          sig_object->n_matrices = n ;
          if(n>0){
            sig_object->sm_signal = (SignalDB **)malloc(n*sizeof(SignalDB *)) ;
            sig_object->sm_skip   = malloc(n*sizeof(int)) ;
            for(i=0;i<n;i++){
              sig_object->sm_signal[i] = NULL ;
              sig_object->sm_skip[i]   = 0    ;
            }
          }
/*
 * *****************************************************************************
 *  Find  signal shape in list of raw signals
 *
 *   Signal shapes are stored as an array in the raw signal database.
 *   The name of the signal shape is the same as the name of the world
 *   item.  Its name is bounded by '"'
 *   The world item name has the quotes missing..
 * *****************************************************************************
 */
          world_filename = zr_basename2(witem->filename) ;
          sig_shape = NULL ;
          if(ip){
            printf("\n  FIND SIGNAL SHAPE IN LIST OF RAW SIGNALS\n") ;
            printf("  Number of raw signal shapes = %i\n",
                                          raw_signal_db->n_sig_shapes) ;
          }
          for(i=0;i<raw_signal_db->n_sig_shapes;i++){
            string1 = strdup(raw_signal_db->sig_shape[i].shapefile) ;
            string1 = strip_quotes(string1) ;
            sig_shape_filename = zr_basename2(string1) ;
            if(ip)printf("  Search %i for signal name %s :: %s :: %i\n",
                        i,world_filename,  sig_shape_filename,
                        strcmp(world_filename,sig_shape_filename ) ) ;

            if(!strcmp_ic(world_filename,sig_shape_filename)){
              sig_shape = &(raw_signal_db->sig_shape[i]);
            }
            free(string1) ;
            free(sig_shape_filename) ;
            if(sig_shape)break ;
          }
          if(!sig_shape){
            printf("  Routine %s ERROR.\n",my_name) ;
            printf("  Unable to find signal shape %s\n",world_filename) ;
          }else{
            if(ip)printf("  Found signal shape %s :: %p\n",
                                    world_filename, (void *) sig_shape) ;
          }
          free(world_filename);
/*
 *  Print Signal Sub-Object data from Signal Configuration File
 */
          if(ip){
            n = sig_shape->n_sig_subobjs ;
            printf("\n  Signal Configuration : Number of Shape Sub-Objects = %i\n",n) ;
            for(i=0;i<n;i++){
              printf("    Sub-Object         = %i\n",i) ;
              sig_subobj = &(sig_shape->sig_subobj[i]) ;
              printf("          index        = %i\n",sig_subobj->index) ;
              printf("          name         = %s\n",sig_subobj->name) ;
              printf("          sig_substype = %s\n",sig_subobj->sig_script_type) ;
            }
/*
 *  Print Signal Object Data from World/Tile File
 */
            n = n_signal_units ;
            printf("\n  World item : Number of Signal Units = %i\n",n) ;
            for(i=0;i<n;i++){
              printf("    Signal Unit           = %i %i :: %i\n",i,n,
                                      sig_object->signal_sub_object) ;
              printf("         sub_object       = %i\n",sig_object->sub_object[i]) ;
              printf("         u_data1          = %i\n",sig_object->u_data1[i]) ;
              printf("         tr_item          = %i\n",sig_object->tr_item[i]) ;
              trk_item = &(track_db.trk_items_array[sig_object->tr_item[i]]) ;
              printf("         signal_type_name = %s\n",trk_item->signal_type_name) ;
            }
          }
/*
 *   Print matrix and other data from the  Shape (*.s) File
 */
//           if(ip)print_shape_file_data(witem->snode);
/*
 * *****************************************************************************
 *  Loop over signal units in world item
 * *****************************************************************************
 */
          n = n_signal_units    ;
          if(ip)printf("\n  SETUP SIGNALS : n_signal_units = %i\n",n) ;
          for(i=0;i<n;i++){
            if(ip){
              printf("\n  SETUP SIGNAL i = %i\n",i) ;
              printf("  sig_object->sub_object[i] =  %i\n",sig_object->sub_object[i]) ;
              printf("  sig_object->u_data1[i]    =  %i\n",sig_object->u_data1[i]) ;
              printf("  sig_object->tr_item[i]    =  %i\n",sig_object->tr_item[i]) ;
              printf("\n  MALLOC MEMORY FOR SIGNAL\n") ;
            }
/*
 * *****************************************************************************
 *  For each signal unit allocate memory for a new signal structure (SignalDB)
 *    and add to the linked list of signals
 * *****************************************************************************
 */
            signal = (SignalDB *)malloc(sizeof(SignalDB)) ;
            if(signallist_beg == NULL){
              signallist_beg = signal ;
            }else{
              signallist_end->next = signal;
              isignal ++ ;
            }
            signallist_end = signal ;

            init_signal_db(signal)  ;
            signal->uid = isignal   ;
            if(ip)printf("\n  ****** SIGNAL UID = %i ******\n",isignal) ;
/*
 *  Add pointer to world item
 */
            sig_object->signal[i] = signal ;
/*
 * *****************************************************************************
 *  Initialise signal (SignalDB) structure
 * *****************************************************************************
 */
            trk_item          = &(track_db.trk_items_array[sig_object->tr_item[i]]) ;
            signal->witem      = witem      ;
            signal->sig_shape  = sig_shape  ;
            signal->sig_object = sig_object ;
            signal->sig_obj_i  = i          ;
            signal->trk_item   = trk_item   ;
            signal->state      = 0          ;
            signal->draw_state = 0          ;
            signal->enabled    = 1          ;
            signal->semaphore_pos = 0.0     ;
            signal->zrv1       = 0.0        ;
            signal->zrv2       = 0.0        ;
            signal->zrv3       = 0.0        ;
            signal->zrv4       = 0.0        ;
            signal->zrv5       = 0.0        ;
            signal->zrv6       = 0.0        ;
            signal->zrv7       = 0.0        ;
            signal->zrv8       = 0.0        ;
            trk_item->signal   = signal     ;
//            ip = (67 == signal->uid) ;  //  Level crossing home
//            ip = (87 == signal->uid) ;  //  Level crossing distant
//            ip = (122 == signal->uid) ; //  Home and distant
/*
 * *****************************************************************************
 *   Search the list of signal types for the 'signal_type_name'
 *     specified by the track_item linked to the signal.
 *   Save pointer to the signal type structure (SigType)
 * *****************************************************************************
 */
            if(ip)printf("\n  SEARCH FOR SIGNAL TYPE\n") ;
            sig_type_name = trk_item->signal_type_name ;
            if(sig_type_name == NULL){
              printf("  ERROR.  Routine %s, sig_type_name = NULL\n",my_name) ;
              printf("  Track item :: trk_item->uid  = %i\n", trk_item->uid) ;
              printf("  Track item :: type_of_node   = %i, %s\n", trk_item->type_of_node,
                                                token_trackdb[trk_item->type_of_node]) ;
              printf("  Track item :: track_section  = %i\n", trk_item->track_section) ;
              printf("  Track item :: trk_item_index = %i\n", trk_item->trk_item_index) ;
              printf("  Track item :: signal_data1   = %s\n", trk_item->signal_data1) ;
              printf("  Track item :: signal_direction = %i\n",
                                                         trk_item->signal_direction) ;
              printf("  Track item :: trk_item_index = %i\n", trk_item->trk_item_index) ;
            }
            signal->sig_type = sig_type = NULL ;
            if(ip)printf("    sig_type_name = %s\n",sig_type_name) ;
            if(ip)printf("    n_sig_types   = %i\n",raw_signal_db->n_sig_types) ;
            for (j=0;j<raw_signal_db->n_sig_types; j++){
              string1 = strdup(raw_signal_db->sig_type[j].name) ;
              string1 = strip_quotes(string1) ;
              if(ip)printf("  Signal Types  %s :: %i %s\n",sig_type_name, j, string1) ;
              if(!strcmp_ic(sig_type_name, string1)){
                signal->sig_type = sig_type = &(raw_signal_db->sig_type[j]) ;
                free(string1) ;
                break ;
              }
              free(string1) ;
            }
            if(!sig_type){
              printf("  ERROR  SigType not found\n") ;
              printf("  sig_type_name = %s\n",sig_type_name) ;
            }
/*
 * *****************************************************************************
 *  For each track item find the signal shape sub-object describing this shape
 * *****************************************************************************
 */
            k = sig_object->sub_object[i] ;  // World item specifies head index
            if(ip){
              printf("\n  SEARCH FOR SIGNAL SUB_OBJECT MATCHING SIGNAL TYPE\n") ;
              printf("    Signal object = %p\n",(void *)sig_object) ;
              printf("    Sub-object    = %p\n",(void *)sig_object->sub_object) ;
              printf("    Index         = %i\n",k) ;
            }
            sig_subobj = &(sig_shape->sig_subobj[k]) ;  // Head in signals data
            signal->sig_subobj = sig_subobj ;
/*
 * *****************************************************************************
 *  Setup initial state of signal
 * *****************************************************************************
 */
            signal->state = -1 ;
            for(j=0;j<8;j++){
              if(sig_type->sig_draw_a[j]){
                signal->state = j;
                break;
              }
            }
/*
 * *****************************************************************************
 *  Add names to SignalDB structure for this signal
 * *****************************************************************************
 */
            signal->world_name = signal->witem->filename ;
            signal->shape_name = signal->sig_shape->name ;
            signal->type_name  = signal->sig_type->name  ;
/*
 *  Find root of signal script
 */
            if(ip)printf("\n  FIND ENTRY TO SIGNAL SCRIPT\n") ;

            if(sig_type->sig_script){
              signal->sig_script = sig_type->sig_script ;
            }else{
              signal->sig_script = sig_type->sig_script =
                  find_signal_script(signal->trk_item->signal_type_name) ;
            }
/*
 * *****************************************************************************
 *  SPECIAL ACTIONS
 *  The USA1 signal type AM14Light needs to be drawn without culling
 * *****************************************************************************
 */
            if(!strcmp_nqic(sig_type->name,"AM14Light")){
              witem->snode->no_culling = 1 ;
            }
          }  //  End Loop over Active Heads specified in World Item
/*
 * *****************************************************************************
 *  At this point the signal heads are initialised.
 *  Next loop over the shape matrices.
 *    IF the matrix name starts 'HEAD', set the signal link (if it exists)
 *      OR set skip and continue loop
 *    IF the semaphore flag is TRUE, set the animation link to semaphore_pos.
 *    Need to strip any trailing test after HEAD##
 * *****************************************************************************
 */
          if(ip){
            printf("\n  INITIALISE MATRIX AND ANIMATION\n\n") ;
            printf("  n_matrices = %i\n",n_matrices) ;
          }
#if 1
/*
 *  Revised HEAD processing
 *
 *  1. Loop over shape matrices and process valid HEAD matrices:
 *          HEAD, HEADR, HEADX, HEADXX, HEADRX, HEADRXX
 *     where X is an integer.
 *    The loop has to include matrix[0] because some signals (i.e. USA 1)
 *    do not start with a SIGNAL matrix but instead start with the
 *    HEAD1 matrix.
 */
int  is_r ;                                          //  Reverse head
int  *hierarchy = snode->lod_control->dist_level[0].hierarchy ;
char *tail = NULL, head[8] ;

          for(i=0;i<n_matrices;i++){                 //  Loop over matrices
            sm_name = snode->matrix[i].name ;
            if(ip)printf(" JJJ Matrix %i, name = %s\n",i,sm_name) ;
/*
 *  Check name is "HEAD" or "HEADR", followed by nothing or an integer
 */
            if(strncmp_ic(sm_name,"HEAD",4)) continue ;
            is_r = (sm_name[4] == 'R') ;
            n = strlen(sm_name) ;
            m = strcspn(sm_name,"0123456789") ;
/*
 *  Check matrix name is of correct form
 */
            if(m-is_r != 4) continue ;
            if(n>m+2)       continue ;
            if(n>m   && !isdigit(sm_name[m]))   continue ;
            if(n>m+1 && !isdigit(sm_name[m+1])) continue ;
/*
 *  Search shape sub-objects looking for a matching HEAD
 */
            im = -1 ;
            m = sig_shape->n_sig_subobjs ;
            for(j=0;j<m;j++){
              if(ip)printf("  SEARCH  matrix (i) = %i,  head (j) = %i,"
                            " name = %s :: %s\n",
                            i, j, sig_shape->sig_subobj[j].name, sm_name) ;
              if(!strcmp_nqic(sig_shape->sig_subobj[j].name, sm_name)){
                im = j;
                if(ip)printf("  FOUND, signal sub object %i :: name = %s\n",
                                          im,sig_shape->sig_subobj[j].name) ;
                break ;
              }
            }
/*
 *  Sometimes the shape (*.s) file includes possible HEADs which are not used
 *  in the Signal Shape (sigcfg.data) definitions.
 *  In this case the HEAD is ignored.
 */
            if(-1==im){
//              printf("  ERROR. Routine %s\n",my_name) ;
//              printf("  Unable to find head '%s' for signal %s\n",
//                     sm_name, sig_shape->name) ;
//              exit(0) ;
              continue ;  //  Skip this HEAD matrix in the shape file
            }
/*
 *  Loop over heads defined in world item.
 *  If the current head is not used, set sm_skip to '1' for this matrix
 *  Otherwise clear sm_skip and set both sm_signal and animation.
 */
            sig_object->sm_skip[i] = 1 ;   //  Set default to skip
            m = sig_object->n_signal_units ;
            if(ip)printf("\n  INITIALISE HEAD %i IN WORLD ITEM\n\n",i) ;
            for(j=0;j<m;j++){
              if(im == sig_object->sub_object[j]){
                signal = sig_object->signal[j] ;
                sig_object->sm_signal[i] = signal ;
                sig_object->sm_skip[i]   = 0      ;
                witem->animations[i]     = &(signal->semaphore_pos) ;
                if(ip)printf("    World item %i set to signal sub-object %i\n",i,j) ;
                break ;
              }
            }
          }
/*
 *  Loop over matrices looking for parents.
 *  If a parent is present and not skipped,
 *       unset skip flag and set up animation link
 */
          if(ip)printf("\n  INITIALISE DEPENDENT MATRICES\n\n") ;
          for(i=1;i<n_matrices;i++){                 //  Loop over matrices
            if(ip)printf("    Process matrix %i : hierarchy = %i\n",i, hierarchy[i]) ;
            j = hierarchy[i]    ;
            if(j == 0) continue ;                    //  No parent
            if(sig_object->sm_skip[j]) continue ;    // Parent not needed
            sig_object->sm_skip[i] = 0      ;
            witem->animations[i]   = witem->animations[j] ;
            if(ip)printf("  sm_signal for dependent matrix %i set to HEAD matrix %i\n", i, j) ;
          }
/*
 *  Loop over matrices looking for non-HEAD matrices with no parent
 *  and where the world item has sm_skip set.
 *  Unset sm_skip if there is no matching head or the matching head
 *  also has sm_skip unset.
 */
          if(ip)printf("\n  INITIALISE REMAINING MATRICES\n\n") ;
          for(i=1;i<n_matrices;i++){                  //  Loop over matrices
            if(ip)printf("    Process matrix %i  : hierarchy = %i : sm_skip = %i\n",
                                i, hierarchy[i], sig_object->sm_skip[i] ) ;
            if(sig_object->sm_skip[i] == 0)continue ; //  Already processed
            j = hierarchy[i]    ;
            if(j != 0) continue ;                     //  Has parent
            sm_name = snode->matrix[i].name ;
            if(!strncmp_ic(sm_name,"HEAD",4)) continue ; // Is HEAD
/*
 *  Search for valid tail
 */
            sm_name = snode->matrix[i].name ;
            n = strlen(sm_name) ;
            tail = &(sm_name[n]) ;
            if(n>0 && isdigit(sm_name[n-1])) n-- ;
            if(n>0 && isdigit(sm_name[n-1])) n-- ;
            if(n>0 && sm_name[n-1] == 'R')   n-- ;
            tail = &(sm_name[n]) ;
            if(n == 0) continue ;
/*
 *  Does HEAD exist?
 */
            strcpy((char *)&head,"HEAD") ;
            strcat((char *)&head,tail) ;
            printf("    tail = %s, head = %s\n",(char *)&head,tail) ;
            for(j=1;i<n_matrices;i++){                  //  Loop over matrices
              if(strcmp(head, snode->matrix[j].name)) continue ;
              if(sig_object->sm_skip[j] == 0)
                   sig_object->sm_skip[i] = 0;
            }
          }
#else
/*
 *   Loop over matrices in world item looking for heads
 */
          for(i=0;i<n_matrices;i++){                 //  Loop over matrices
            sm_name = snode->matrix[i].name ;
            if(ip)printf(" JJJ Matrix %i, name = %s\n",i,sm_name) ;
/*
 *  Check name is of format HEADXX where X is a number
 */
            if(strncmp_ic(sm_name,"HEAD",4)) continue ;
            n = strlen(sm_name) ;
            m = strcspn(sm_name,"0123456789") ;
            if(m != 4) continue ;
/*
 *  Search shape sub-objects looking for a matching HEAD
 */
            im = -1 ;
            m = sig_shape->n_sig_subobjs ;
            for(j=0;j<m;j++){
              if(ip)printf("  SEARCH  matrix (i) = %i,  head (j) = %i, name = %s :: %s\n",
                     i, j, sig_shape->sig_subobj[j].name, sm_name) ;
              if(!strcmp_nqic(sig_shape->sig_subobj[j].name, sm_name)){
                im = j;
                break ;
              }
            }
/*
 *  Sometimes the shape (*.s) file includes possible HEADs which are not used
 *  in the Signal Shape (sigcfg.data) definitions.  In this case the HEAD is
 *  ignored.
 */
            if(-1==im){
//              printf("  ERROR. Routine %s\n",my_name) ;
//              printf("  Unable to find head '%s' for signal %s\n",
//                     sm_name, sig_shape->name) ;
//              exit(0) ;
              continue ;         //  DEBUG !!  Solution to Japan1 problem??
            }
/*
 *  Check with heads defined by world item.  Is the head associated
 *  with the current matrix needed?  If it is not-needed set
 *      WorldItem->sig_object->sm_skip to '1'.
 *  Otherwise set it to '0' and initialise:
 *      WorldItem->sig_object->sm_signal
 *      WorldItem->animations[i]
 */
            sig_object->sm_skip[i] = 1 ;   //  Set default to skip
            m = sig_object->n_signal_units ;
            for(j=0;j<m;j++){
              if(im == sig_object->sub_object[j]){
                signal = sig_object->signal[j] ;
                sig_object->sm_signal[i] = signal ;
                sig_object->sm_skip[i]   = 0      ;
                witem->animations[i]     = &(signal->semaphore_pos) ;
                break ;
              }
            }
          }
/*
 *  Set skip flag for child matrices not used when displaying this shape
 */
          k = 1;
          dist_level = snode->lod_control->dist_level;
          hierarchy  = dist_level->hierarchy ;

          while(k){
            k = 0 ;
            for(i=1;i<n_matrices;i++){
              if(sig_object->sm_skip[i]) continue ;
              j = hierarchy[i] ;
              if(sig_object->sm_skip[j]){
                sig_object->sm_skip[i] = 1 ;
                k++ ;
              }
            }
          }
#endif
/*
 *  Print matrix summary
 */
          if(ip){
            printf(" World Item :: %i %i :: %i\n",wnode->tile_x, wnode->tile_y,
                                                         witem->uid);
            printf(" Active Heads :: %i\n", n_signal_units) ;
            for(i=0;i<n_signal_units;i++){
              printf(" Head index %i : Track item %i : Signal %p\n",
                     i, sig_object->sub_object[i], (void *)sig_object->signal[i] ) ;
              if(sig_object->signal[i]){
                printf(" Head index %i : Track item %i : Signal : %i\n",
                     i, sig_object->sub_object[i], sig_object->signal[i]->uid ) ;
              }
            }
            printf("\n") ;
            printf(" Matrices :: %i\n", n_matrices) ;
            for(i=0;i<n_matrices;i++){
              printf(" Matrix %i : sm_skip %i : sm_signal %p : name %s\n",
                     i, sig_object->sm_skip[i], (void *)sig_object->sm_signal[i],
                     snode->matrix[i].name ) ;
              if(sig_object->sm_signal[i]){
                printf(" Matrix %i : sm_skip %i : sm_signal : %i\n",
                       i, sig_object->sm_skip[i], sig_object->sm_signal[i]->uid ) ;
              }
            }
            printf("\n") ;
          }

        }  // End loop over world items in tile
      }  // End loop over tiles
      if(ip)printf("   Exit routine %s\n",my_name) ;
      return 0 ;
}
#if 0
#if 0
/*
 *  Find the shape matrix corresponding to the sub-object name (HEAD1, HEAD2 etc)
 */
            head_name = strdup(sig_subobj->name) ;
            head_name = strip_quotes(head_name) ;
            printf("\n  Find shape matrix matching head name %s\n",head_name) ;
            printf("  Full name  = %s\n",sig_subobj->full_name) ;
            printf("       name  = %s\n",sig_subobj->name) ;
            snode     = witem->snode ;
            m = -1 ;
            for(j=0;j<(int)snode->nmatrices;j++){
              if(ip)printf(" +++ Shape %s, head_name = '%s', j = %i, matrix name = '%s'\n",
                       snode->name, head_name, j, snode->matrix[j].name) ;
              if(!strcmp_ic(head_name,snode->matrix[j].name)){
                m = j;
                break ;
              }
            }
            if(m < 0){
              printf("  Routine %s, unable to find shape matrix\n",my_name) ;
              printf("  World item uid = %i\n", witem->uid) ;
              printf("  Shape name     = %s\n", snode->name) ;
              printf("  Signal object  = %i\n", i) ;
              printf("  Head name      = %s\n", head_name) ;
              for(j=0;j<(int)snode->nmatrices;j++){
                printf(" Matrix %i, name = '%s'\n", j, snode->matrix[j].name) ;
              }
              printf("  Program stopping ...\n\n") ;
              exit(0) ;
            }
            witem->u.signal_obj.signal[m] = signal ;
            signal->sig_subobj = NULL ;
            for(j=0;j<sig_shape->n_sig_subobjs;j++){
              printf("  AAA j = %i :: %i\n",j,sig_shape->n_sig_subobjs) ;
              printf("  AAA sig_shape->sig_subobj = %p\n", (void *)sig_shape->sig_subobj ) ;
              printf("  AAA sig_shape->sig_subobj[j].sig_script_type = %s\n",
                              sig_shape->sig_subobj[j].sig_script_type) ;
              string1 =  sig_shape->sig_subobj[j].sig_script_type ;
              if(ip)printf("  Compare %s :: %s\n", sig_type->name,string1) ;
              if(string1 && !strcmp(sig_type->name, string1)){
                printf(" AA\n") ;
                signal->sig_subobj_i = j ;
                signal->sig_subobj = &(sig_shape->sig_subobj[j]) ;
                break ;
              }
                printf(" CC\n") ;
            }
            printf(" DD\n") ;
            if(!signal->sig_subobj){
              printf("  ERROR  Signal Shape Sub-object not found\n") ;
            }
/*
 *  For each world item, set up the animation variables
 */
            printf("\n  SETUP ANIMATION\n") ;
            head_name = strdup(signal->sig_subobj->name) ;
            head_name = strip_quotes(head_name) ;
            printf("\n  Find shape matrix matching head name %s\n",head_name) ;
            snode     = witem->snode ;
            m = -1 ;
            for(j=0;j<(int)snode->nmatrices;j++){
              if(ip)printf(" +++ Shape %s, head_name = '%s', j = %i, matrix name = '%s'\n",
                       snode->name, head_name, j, snode->matrix[j].name) ;
              if(!strcmp_ic(head_name,snode->matrix[j].name)){
                m = j;
                break ;
              }
            }
/*
 *   Use hierarchy array to find dependent matrices
 */
            printf("\n  FIND DEPENDENT MATRICES\n") ;
            if(m == -1){
              printf("  Unable to find head '%s' for signal in shape '%s'\n",
                                                     head_name,snode->name) ;
            }else{
              dist_level = snode->lod_control->dist_level;
              if(ip)printf("  Size of hierarchy array = %i\n",dist_level->n_hierarchy );
              for(j=0;j<(int)dist_level->n_hierarchy;j++){
                if(ip)printf("  hierarchy %i = %i\n", j,dist_level->hierarchy[j] );
                if(dist_level->hierarchy[j] == m){
                  witem->animations[j]=&(signal->semaphore_pos) ;
                  if(ip)printf("  Pointer set for matrix %s\n",snode->matrix[j].name) ;
                }
              }
            }
            free(head_name) ;
#endif
/*
 *  Find root of signal script
 */
            printf("\n  FIND ENTRY TO SIGNAL SCRIPT\n") ;

            if(sig_type->sig_script){
              signal->sig_script = sig_type->sig_script ;
            }else{
int      ip = 0 ;
nodeType *n1, *n2, *n3, *n4 ;
char     *name ;
              if(ip){
                printf("\n===============================================\n") ;
                printf("\n    Signal %s \n",sig_type_name) ;
              }
              name = NULL ;
              n1 = sTree ;
              n1 = n1->opr.op[0] ;
              for(;;){
                if(!n1)break ;
                if(ip){
                  printf("\n================================================\n") ;
                  printf("   n1 = %p\n", (void *)n1 ) ;
                  printf("   n1  type = %i\n",
                            n1->type) ;
                  printf("   n1  type = %i, oper = %i \n",
                            n1->type, n1->opr.oper) ;
                  printf("   n1  %p type = %i, oper = %i %s\n", (void *) n1,
                            n1->type, n1->opr.oper, sigscr_token_string(n1->opr.oper)) ;
                }
                if(n1->type == typeOpr && n1->opr.oper == SC_SCRIPT_LIST){
                  n2 = n1->opr.op[1] ;
                }else if(n1->type == typeOpr && n1->opr.oper == SC_SCRIPT){
                  n2 = n1 ;
                }else{
                  printf("  Routine %s. Search for SC-SCRIPT or SC-SCRIPT_LIST failed\n",
                            my_name) ;
                  n1 = NULL ; break ;
                }

                if(ip)printf("     n2  %p type = %i, oper = %i %s\n", (void *) n2,
                                  n2->type, n2->opr.oper, sigscr_token_string(n2->opr.oper)) ;
                if(n2->type == typeOpr && n2->opr.oper == SC_SCRIPT){
                  n3 = n2->opr.op[0] ;
                }else{
                  printf("  Routine %s. Search for SC-SCRIPT failed\n",
                            my_name) ;
                  n1 = NULL ; break ;
                }

                if(ip)printf("     n3  %p type = %i, name = %s\n", (void *) n3,
                                  n3->type, n3->str.name) ;
                if(n3->type == typeStr){
                  name = n3->str.name ;
                  if(ip)printf("  n3 name = %s\n",name) ;
                }else{
                  printf("  Routine %s. Search for 'name' failed.\n",
                            my_name) ;
                  printf("  Pointers: n1 = %p, n2 = %p, n3 = %p\n",
                                    (void *)n1, (void *)n2, (void *)n3) ;
                  printf("  Types n1 = %i, n2 = %i, n3 = %i\n",
                                    n1->type, n2->type, n3->type) ;
                  printf("  Oper n1 = %i, n2 = %i\n",
                                    n1->opr.oper, n2->opr.oper) ;
                  n1 = NULL ; break ;
                }

                if(!strcmp(name,sig_type_name)){
                  if(ip || 0)printf("  Routine %s.  Script for '%s' found\n",
                                  my_name,sig_type_name) ;
                  signal->sig_script   = n2 ;
                  sig_type->sig_script = n2 ;
                  break ;
                }
                if(ip)printf("  Loop n1 = %p, n2 = %p\n",(void *)n1, (void *)n2) ;
                if(n1 != n2){
                  n1 = n1->opr.op[0] ;
                }else{
                  break ;
                }
              }
              if(!n1){
                printf("  Routine '%s', failed to find script for signal '%s'\n",
                          my_name, sig_type_name) ;
              }
            }
/*
 *  Print the signal information
 */
//            ips = (witem->uid == 503) ;
//            ips = (90 == signal->uid) || (91 == signal->uid) ;
//            ips = 1;

            if(ip | ips){
            }
            printf("  New Loop i = %i, n = %i\n",i,n) ;
          }
            printf("  Loop ended\n") ;
        }
        printf("  New world node\n");
      }
#endif



/*
 *  Routine to add useful indices to raw signal node.
 *
 *  The signal configuration file refers to texture file to use
 *  for any lights, the grouping of lights to be drawn and the
 *  colours of the light by names(i.e."Red", "Red Light").
 *
 *  Here pointers are added to speed up the process of updating and
 *  drawing each signal.
 */

int setup_raw_signals(void){

  int       i, j, k, n, m ;
  int       ip = 0 ;
  LightTex  *light_tex    ;
  LightTab  *light_tab    ;
  SigLight  *sig_light    ;
  SigType   *sig_type     ;
  SigShape  *sig_shape    ;
  SigDraw   *sig_draw     ;
  SigAspect *sig_aspect   ;
  ShapeNode *sig_snode    ;
  TextureNode  *tnode     ;
  char      *script_file  ;
  char      *filename ;        // File name of the signal shape
  char      *string1, *string2 ;
  char      *my_name = "setup_raw_signals" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
#if 0
/*
 *  Locate the texture structures containing the textures
 */
      if(ip)printf("  n_light_tex = %i\n",raw_signal_db->n_light_tex) ;
//      for(i=0;i<raw_signal_db->n_light_tex;i++){
//        light_tex = &(raw_signal_db->light_tex[i]) ;
      for(light_tex=raw_signal_db->light_tex; light_tex!=NULL; light_tex=light_tex->next){
        string2   = strdup(light_tex->ace_file) ;
        string2   = strip_quotes(string2) ;
        string1   = zr_corename(string2)  ;
        if(string2)free(string2) ;
        light_tex->tnode = NULL ;
        for(tnode=texturelist_beg; tnode!=NULL; tnode=tnode->next){
          string2 = tnode->name ;
          if(ip)printf(" Routine %s, AA :: string1 = %s, string2 = %s\n",my_name,string1,string2) ;
          if(!strcmp_ic(string1,string2)){
            light_tex->tnode = tnode;
            break ;
          }
        }
        if(!light_tex->tnode){
          printf("  Routine %s.  Unable to find light texture %s\n",my_name,light_tex->ace_file) ;
        }
        free(string1) ;
      }
#endif
/*
 * Loop over Signal Types (SigType)
 */
      if(ip)printf("   n_sig_types  = %i\n", raw_signal_db->n_sig_types) ;
      fflush(NULL) ;
      for (i=0;i<raw_signal_db->n_sig_types; i++){
        sig_type = &(raw_signal_db->sig_type[i]) ;
        if(ip)printf("  Signal %i, Type = %s\n",i,sig_type->name) ;
/*
 *  Signal type structure:
 *  Convert name of light texture 'light_tex_ref' to
 *  a pointer to the light texture 'light_tex_node'
 */
        sig_type->light_tex_node = NULL ;
        string1 = sig_type->light_tex_ref ;
//        for(j=0;j<raw_signal_db->n_light_tex;j++){
//          light_tex = &(raw_signal_db->light_tex[j]) ;
        for(light_tex=raw_signal_db->light_tex; light_tex!=NULL; light_tex=light_tex->next){
          string2 = light_tex->name ;
          if(ip)printf(" Routine %s, BB :: string1 = %s, string2 = %s\n",my_name,string1,string2) ;
          if(!strcmp(string1,string2)){
            sig_type->light_tex_node = light_tex->tnode ;
          }
        }
        if(!sig_type->light_tex_node){
          printf("  Routine %s.  Unable to find light texture ref %s\n",my_name,string1) ;
        }
/*
 *  Signal type structure
 *
 *  Convert names of signal lights 'sig_light[i]->name'
 *  to pointer to signal light in table 'sig_light[i]->light_tab'
 */
        if(ip)printf("   n_sig_lights  = %i\n", sig_type->n_sig_lights) ;

        for(j=0;j<sig_type->n_sig_lights;j++){
          sig_light = &(sig_type->sig_light[j]) ;
          sig_light->light_tab = NULL ;
          string1 = sig_light->name ;
          if(!string1){
            sig_light->light_tab = NULL ;
            continue ;
          }
          for(k=0;k<raw_signal_db->n_light_tab;k++){
            light_tab = &(raw_signal_db->light_tab[k]) ;
            string2 = light_tab->name ;
            if(!strcmp(string1,string2)){
              sig_light->light_tab = light_tab ;
            }
          }
          if(!sig_light->light_tab){
            printf("  Routine %s.  Unable to find light table for %s\n",my_name,string1) ;
          }
        }
/*
 *  Signal type structure
 *
 *  In signal draw state structure convert indices of lights
 *     sig_draw->draw_light_i[k]
 *  to pointers to the SigLight light structure belonging to the
 *  same signal type
 *     sig_draw->draw_light[k]
 */
        if(ip)printf("   n_sig_dstates = %i\n", sig_type->n_sig_dstates) ;

        for(j=0;j<sig_type->n_sig_dstates;j++){
          sig_draw = &(sig_type->sig_draw[j]) ;
          n = sig_draw->n_draw_lights ;
          sig_draw->draw_light = (SigLight **)malloc(n*sizeof(SigLight)) ;
          for(k=0;k<n;k++){
            m = sig_draw->draw_light_i[k] ;
//            printf("  Loop  i, k, m = %i %i %i\n",i,k,m) ;
            if(m<sig_type->n_sig_lights){
              sig_draw->draw_light[k] = &(sig_type->sig_light[m]) ;
            }else{
              sig_draw->draw_light[k] = NULL ;
            }
          }
        }
/*
 *  Signal type structure
 *
 *  Each signal has a number of supported signal aspects corresponding to
 *   some of the standard signsl 'state's - STOP, CLEAR_2 etc
 *  For each aspect the corresponding draw state names is converted into
 *    a pointer to the correct draw state structure
 *  Also one array, whose indices correspond to the allowed set of
 *    signal states, becomes a set of pointers to the corresponding
 *    draw state structure or NULL if there is no corresponding
 *    structure
 *  A similar array is a set of pointers to the corresponding aspect
 *    structures.  This may contain additional information, i.e. speed
 *    limits.
 */
        for(j=0;j<8;j++){
          sig_type->sig_draw_a[j] = NULL ;
          sig_type->sig_draw_ia[j] = -1   ;
          sig_type->sig_aspect_a[j] = NULL ;
          sig_type->sig_aspect_ia[j] = -1   ;
        }

        if(ip)printf("   n_sig_aspects = %i\n", sig_type->n_sig_aspects) ;

        for(j=0;j<sig_type->n_sig_aspects;j++){
          sig_aspect = &(sig_type->sig_aspect[j]) ;
          sig_aspect->sig_draw = NULL ;
          m = sig_aspect->status ;                     //  SIG_STOP etc
          string1 = sig_aspect->draw_state_name ;
          for(k=0;k<sig_type->n_sig_dstates;k++){
            sig_draw = &(sig_type->sig_draw[k]) ;
            string2 = sig_draw->name ;
            if(!strcmp(string1,string2)){
              sig_aspect->sig_draw       = sig_draw   ;
              sig_type->sig_draw_a[m]    = sig_draw   ;
              sig_type->sig_draw_ia[m]   = k          ;
              sig_type->sig_aspect_a[m]  = sig_aspect ;
              sig_type->sig_aspect_ia[m] = j          ;
            }
          }
          if(!sig_aspect->sig_draw){
            printf("  Routine %s.  Unable to find signal draw for aspect  %s\n",my_name,string1) ;
          }
        }
        if(ip){
          printf(" Signal draw array   = ") ;
          for(j=0;j<8;j++)printf("  %2i %p",sig_type->sig_draw_ia[j],
                                    (void *)sig_type->sig_draw_a[j] ) ;
          printf("\n") ;
          printf(" Signal aspect array = ") ;
          for(j=0;j<8;j++)printf("  %2i %p",sig_type->sig_aspect_ia[j],
                                    (void *)sig_type->sig_aspect_a[j] ) ;
          printf("\n") ;
        }
/*
 *  End loop over Signal Types
 */
      }

/*
 * Loop over Signal Shapes
 */
      if(ip)printf("   n_sig_shapes  = %i\n", raw_signal_db->n_sig_shapes) ;
      for (i=0;i<raw_signal_db->n_sig_shapes; i++){
        sig_shape = &(raw_signal_db->sig_shape[i]) ;
        if(ip)printf("  Signal Type [%i] = %s  :: %s\n",
                i,sig_shape->name,sig_shape->shapefile) ;
/*
 *  Find signal shape
 */
        filename = strdup(sig_shape->shapefile) ;
        zr_str2lcnqne(filename) ;
        sig_snode = (ShapeNode *)find_btree_ic(shape_master,filename) ;
/*
 *  shape_master only stores names mentioned by the world (file) files
 */
        if(!sig_snode){
          for(sig_snode = shapelist_beg; sig_snode != NULL; sig_snode=sig_snode->next){
            if(sig_snode->name == NULL) continue;
            if(!strcmp_ic(sig_snode->name,filename)) break ;
          }
        }
        if(!sig_snode){
//          printf("  Routine %s.  UNABLE to find shape node for signal :: %s\n",my_name, filename) ;
          continue ;
        }else{
          if(ip)printf("\n  Routine %s.  FOUND shape node for signal :: %s\n",my_name, filename) ;

        }
/*
 *  Set heirarchy
 *
 *  Signals with signal arms may also have counterweights and other moving
 *  parts accciated with each signal head.  However sometimes the heirarchy
 *  array is not initialised, so that the standard way used to connect
 *  elements such as engine driving wheels and the steam gear does not work.
 *
 *  In these cases OpenRails appears to ignore the problem by not displaying
 *  the part.  Here part names ending with a single integer are connected to
 *  the corresponding HEAD.  Thus if 'Arm1' is missing a heirarchy link, it
 *  is linked to matrix 'Head1', if that exists.
 *
 *  Double integers are ignored.  This is because 'Arm12', may refer to 'Head1'
 *  or 'Head12'.    (See:  Creation of a Complex Shape.doc).  Such cases could
 *  be allowed for, but if the shape file has been so badly set up, this is
 *  likely to lead to furtehr errors.
 *
 *  NOTE:  Added.  The scheme worked but in the test case (New Forest Route,
 *  signal SRJct-BS-MS-BS.s), but the vertical offset of the balance arms above
 *  the signal base was added to vertical offset of the signals heads - putting
 *  the balance arms floating in mid-air.
 *
 *  Maybe this is the reason the shape hierarchy array was set to zero.  Anyway
 *  the code is now switched off.
 *
 */
#if 0
int iag, ian, ilc, idl, ihy, ihy1, iret, l1, l2 ;

int n_matrices ;
int n_animations  ;
int n_anim_nodes  ;
int n_controllers ;

int n_lod_controls ;
int n_dist_levels  ;
int n_hierarchy  ;
char *name       ;
int  *heada      ;

Animation  *animation ;
AnimNode   *anim_node ;
LodControl *lod_control ;
DistLevel  *dist_level ;

        n_matrices   = sig_snode->nmatrices ;
        n_animations = sig_snode->n_animations ;

        for(iag=0; iag< n_animations; iag++){
          animation = &(sig_snode->animation[iag]) ;
          n_anim_nodes = animation->n_anim_nodes ;
          if(n_matrices != n_anim_nodes  || 1){
            printf("  Routine %s.  Shape %s :: animation = %i, n_anim_nodes = %i,  n_matrices = %i.\n",
                  my_name, sig_snode->name, iag, n_anim_nodes, n_matrices) ;
          }
          for(ian=0; ian<n_anim_nodes; ian++){
            anim_node = &(animation->anim_node[ian]) ;
            n_controllers = anim_node->n_controllers ;
            printf("  Animation : group = %i, node = %2i, controllers = %i, matrix = %s\n",
              iag, ian, n_controllers, sig_snode->matrix[ian].name) ;
          }
        }

        if(n_animations == 0)continue ;
/*
 *  Setup array heada - contains indices of any head matrices
 */
        heada = (int *)malloc(n_matrices*sizeof(int)) ;
        for(k=0;k<n_matrices;k++){
          heada[k] = 0 ;
          name = sig_snode->matrix[k].name ;
          iret = strncmp_ic("HEAD",name,4);
          if(!iret){
            l1 = strlen(name) ;
            if(l1 == 5 && isdigit((int)name[4]) ){
              heada[k] = name[4] - '0' ;
            }
          }
        }
        printf("\n  Array heada:\n") ;
        for(k=0;k<n_matrices;k++) printf(" %i ",heada[k]) ;
        printf("\n") ;
/*
 *  Loop over levels containing hierarchy arrays
 *  Compare with single (?) animation structure
 */
        animation = &(sig_snode->animation[0]) ;

        n_lod_controls = sig_snode->n_lod_controls ;
        for(ilc=0; ilc<n_lod_controls; ilc++){
          lod_control = &(sig_snode->lod_control[ilc]) ;
          n_dist_levels = lod_control->n_dist_levels ;

          for(idl=0;idl<n_dist_levels;idl++){
            dist_level  = &(lod_control->dist_level[idl]) ;
            n_hierarchy = dist_level->n_hierarchy ;
            printf("\n  Hierarchy : level of detail = %i, distance level = %i, hierarchy = %i\n",
                   ilc, idl,n_hierarchy) ;
/*
 *  Check for error
 */
            if(n_hierarchy != n_matrices){
              printf("  ERROR.  Routine %s.  Shape %s. Level of detail = %i, Distance level = %i",
                     my_name, sig_snode->name, ilc, idl) ;
              printf("          Number of hierarchies = %i, do not match number of matrices %i\n",
                     n_hierarchy, n_matrices) ;
              continue ;
            }
/*
 * Loop over hierarchy array - missing hierarchy[0].
 */
            for(ihy=1; ihy<n_hierarchy; ihy++){
/*
 *  Test for missing heirarchy value
 */
              name = sig_snode->matrix[ihy].name ;
              if(dist_level->hierarchy[ihy] == 0 && animation->anim_node[ihy].n_controllers != 0
                && strncmp_ic("HEAD",name,4)){
                printf("  **** Index = %2i,  hierarchy = %2i, n_controllers = %2i, matrix = %s\n",ihy,
                     dist_level->hierarchy[ihy], animation->anim_node[ihy].n_controllers, sig_snode->matrix[ihy].name) ;

                l1 = strlen(name) ;
                l2 = strcspn(name,"0123456789");
                printf("  name = %s,  l1 = %i, l2 = %i\n",name,l1,l2) ;
                if(l2 == l1-1){
                  n = name[l2] - '0' ;
                  printf(" name[l2] = %c. n = %i\n",name[l2],n) ;
                  for(k=0;k<n_matrices;k++){
                    if(n == heada[k]){
                      dist_level->hierarchy[ihy] = k ;
                      break ;
                    }
                  }
                }
              }else{
                printf("       Index = %2i,  hierarchy = %2i, n_controllers = %2i, matrix = %s\n",ihy,
                     dist_level->hierarchy[ihy], animation->anim_node[ihy].n_controllers, sig_snode->matrix[ihy].name) ;
              }
            }
            printf("\n  SUMMARY\n") ;
            for(ihy=0; ihy<n_matrices; ihy++){
              printf("    Index = %i, hierarchy = %i, matrix = %s\n",ihy,
                     dist_level->hierarchy[ihy], sig_snode->matrix[ihy].name) ;
            }
          }
        }
        if(heada){free(heada) ; heada = NULL ; }
#endif

      } //  Loop over raw signal shapes
      return 0 ;
}

int init_signal_db(SignalDB *s){

      s->next        = NULL ;
      s->uid         = -1 ;
      s->world_name  = NULL ;
      s->shape_name  = NULL ;
      s->type_name   = NULL ;
      s->witem       = NULL ;
      s->sig_object  = NULL ;
      s->sig_obj_i   = -1   ;
      s->trk_item    = NULL ;
      s->sig_shape_i = -1 ;
      s->sig_subobj_i= -1 ;
      s->sig_type_i  = -1 ;
      s->sig_shape   = NULL ;
      s->sig_subobj  = NULL ;
      s->sig_type    = NULL ;
      s->sig_script  = NULL ;
      s->sig_snode   = NULL ;
      s->enabled     =  1 ;
      s->state       =  0 ;
      s->draw_state  =  0 ;
      s->aspect      =  0 ;
      s->semaphore_pos = 0.0 ;
      s->light_pos     = 0.0 ;

      return 0 ;
}

int add_texture_pointers_to_signals(void){

  int         ip = 0 ;
  int         i ;
  LightTex    *light_tex ;
  TextureNode *tnode ;
  char        *name  ;
  char        *my_name = "add_texture_pointers_to_signals" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      for(light_tex=raw_signal_db->light_tex; light_tex!=NULL; light_tex=light_tex->next){
        if(ip){
          printf("  Routine %s. Texture name  = %s\n",my_name,light_tex->name) ;
          printf("  Routine %s. Ace file name = %s\n",my_name,
                                                          light_tex->ace_file) ;
        }
        name = light_tex->ace_file ;
/*
 *  Loop through list of textures
 *  Texture names are set to NULL if there were problems reading the texture
 */
        for(tnode=texturelist_beg; tnode!=NULL; tnode=tnode->next){
          if(ip)printf("  Textures ::  %s\n",tnode->name) ;
          if(!tnode->name || strcmp_ic(name,tnode->name))continue ;
          light_tex->tnode = tnode ;          // save link
          tnode->basic        = 1  ;
          tnode->needed       = 1  ;
          if(ip>1)printf("  Texture found    = %s,  %s\n",tnode->name, tnode->filename) ;
          break ;
        }
        if(ip && light_tex->tnode == NULL){
          printf("  Routine %s :: LightTex %s, texture %s not found\n",
                 my_name,name,name) ;
/*
 *  If texture is not found substitute 'BLANK'
 */
          for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
            if(strcmp_ic("BLANK",tnode->name))continue ;
            light_tex->tnode = tnode ;          // save link
            tnode->basic     = 1     ;
            tnode->needed    = 1     ;
            printf("            Substitute used = %s\n",tnode->name) ;
            break ;
          }
          if(ip && light_tex->tnode == NULL){
            printf("       Substitute 'BLANK not found \n") ;
          }
        }
      }
      return 0 ;
}
