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
  nodeType *n1, *n2, *n3, *n4 ;
  char     *name ;
  char     *my_name = "find_signal_script" ;

      if(ip){
        printf("\n===============================================\n") ;
        printf("\n    Routine %s \n",my_name) ;
        printf("\n    Signal %s \n",signal_type_name) ;
      }
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

        if(!strcmp(name,signal_type_name)){
          if(ip || 0)printf("  Routine %s.  Script for '%s' found\n",
                          my_name,signal_type_name) ;
//          signal->sig_script   = n2 ;
//          sig_type->sig_script = n2 ;
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
                  my_name, signal_type_name) ;
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
  TrkItem     *trk_item   ;
  ShapeNode   *snode ;
  DistLevel   *dist_level ;
  nodeType    *sig_script ;


  char      *world_filename     ;
  char      *sig_shape_filename ;
  char      *sig_type_name ;
  char      *string1 ;
  char      *head_name ;
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
      for(k=0;k<(int)witem->u.signal_obj.n_matrices;k++){
        printf("      matrix     = %i, skip = %i, signal = %p, name = %s\n",
                                        k, witem->u.signal_obj.skip[k],
                                        (void *)witem->u.signal_obj.signal[k],
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
        trk_item = &(track_db.trk_items_array[m]) ;
        printf("               Type of node = %i %i  %s\n",
          m,trk_item->type_of_node,token_trackdb[trk_item->type_of_node]) ;
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

int setup_signals(void){

  int ip  = 0  ;
  int ips = 0 ;  // print summary of all signals
  int i, j, k, n, m, it, im ;
  int isignal = 0 ;

  WorldNode   *wnode ;
  WorldItem   *witem ;
  SignalObj   *sig_object  ;
  SignalDB    *signal = NULL;
  SigType     *sig_type   ;    //  Item in list of raw signals
  SigShape    *sig_shape  ;
  SigSubObj   *sig_subobj ;
  TrkItem     *trk_item   ;
  ShapeNode   *snode ;
  DistLevel   *dist_level ;
  nodeType    *sig_script ;


  char      *world_filename     ;
  char      *sig_shape_filename ;
  char      *sig_type_name ;
  char      *string1 ;
  char      *head_name ;

  char *my_name = "setup_signals" ;

      if(ip)printf("   Enter routine %s\n",my_name) ;
/*
 * *****************************************************************************
 *  Loop through the world file, find every signal and add to a list of signals
 * *****************************************************************************
 */
      for(wnode=worldlist_beg; wnode!= NULL; wnode=wnode->next){
        for(witem=wnode->world_item; witem!= NULL; witem=witem->next){
//        ip = (232 == witem->uid) ;  //  Crossing
//          ip = (415 == witem->uid) ;
//          ip = (104 == witem->uid) ;  //USA2
          if(ip){
            printf("  New world item %4i,   type = %-14s",
                    witem->uid, token_idc[witem->worldtype]) ;
            if(witem->snode)printf("   name = %s",witem->snode->name) ;
            printf("\n") ;
          }
          if(witem->worldtype != SIGNAL_ALT) continue ;
          sig_object = &(witem->u.signal_obj) ;
          if(ip)printf("\n  World Item.  Index = %i, Type = %i : %s\n",
                    witem->uid,witem->worldtype,token_idc[witem->worldtype])  ;
/*
 * *****************************************************************************
 *  Allocate space for array of signal pointers matching the shape matrix array
 *  This allows the drawing routine to access the correct signal
 * *****************************************************************************
 */
          n = witem->snode->nmatrices ;
          witem->u.signal_obj.n_matrices = n ;
          if(!witem->u.signal_obj.signal){
            witem->u.signal_obj.signal =
                             (SignalDB **)malloc(n*sizeof(SignalDB *)) ;
            witem->u.signal_obj.skip   = (uint *)malloc(n*sizeof(int)) ;
            for(i=0;i<n;i++){
              witem->u.signal_obj.skip[i]   = 0    ;
              witem->u.signal_obj.signal[i] = NULL ;
            }
            n_signals ++ ;  // Count signals (zr.h)
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

            if(!strcmp(world_filename,sig_shape_filename)){
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
            if(ip)printf("  Found signal shape %s :: %p\n",world_filename,
                                                           (void *) sig_shape) ;
          }
          free(world_filename);
/*
 *  Print Signal Sub-Object data
 */
          if(ip){
            n = sig_shape->n_sig_subobjs ;
            for(i=0;i<n;i++){
              printf("  Signal Shape : Sub-Object = %i\n",i) ;
              sig_subobj = &(sig_shape->sig_subobj[i]) ;
              printf("          index        = %i\n",sig_subobj->index) ;
//            printf("          name         = %p\n",(void *)sig_subobj->name) ;
              printf("          name         = %s\n",sig_subobj->name) ;
//            printf("          sig_substype = %p\n",(void *)sig_subobj->sig_script_type) ;
              printf("          sig_substype = %s\n",sig_subobj->sig_script_type) ;
            }
/*
 *  Print Signal Object Data
 */
            n = sig_object->n_signal_units ;
            for(i=0;i<n;i++){
              printf("  Signal Object = %i %i :: %i\n",i,n,
                                      sig_object->signal_sub_object) ;
              printf("         sub_object       = %i\n",sig_object->sub_object[i]) ;
              printf("         u_data1          = %i\n",sig_object->u_data1[i]) ;
              printf("         tr_item          = %i\n",sig_object->tr_item[i]) ;
              trk_item = &(track_db.trk_items_array[sig_object->tr_item[i]]) ;
              printf("         signal_type_name = %s\n",trk_item->signal_type_name) ;
            }
          }
/*
 *   Print shape file
 */
//           print_shape_file_data(witem->snode);
/*
 * *****************************************************************************
 *  Loop over signals in world item.
 * *****************************************************************************
 */
          n             = sig_object->n_signal_units    ;
          if(ip)printf("\n  SETUP SIGNALS : n_signal_units = %i\n",n) ;
          for(i=0;i<n;i++){
            if(ip){
              printf("\n  SETUP SIGNAL i = %i\n",i) ;
              printf("  sig_object->sub_object[i] =  %i\n",sig_object->sub_object[i]) ;
              printf("  sig_object->u_data1[i]    =  %i\n",sig_object->u_data1[i]) ;
              printf("  sig_object->tr_item[i]    =  %i\n",sig_object->tr_item[i]) ;
/*
 * *****************************************************************************
 *  For each signal object allocate memory for a new signal
 *    and add to the linked list of signals
 * *****************************************************************************
 */
              printf("\n  MALLOC MEMORY FOR SIGNAL\n") ;
            }
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
            fflush(NULL) ;
/*
 * *****************************************************************************
 *  Save signal object, track item and world item
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
            trk_item->signal  = signal      ;
//            ip = (67 == signal->uid) ;  //  Level crossing home
//            ip = (87 == signal->uid) ;  //  Level crossing distant
//            ip = (122 == signal->uid) ; //  Home and distant
/*
 * *****************************************************************************
 *  For each track item find the corresponding Signal Type
 *  and store the link in the signal structure
 * *****************************************************************************
 */
            if(ip)printf("\n  SEARCH FOR SIGNAL TYPE\n") ;
            sig_type_name = trk_item->signal_type_name ;
            signal->sig_type = sig_type = NULL ;
            if(ip)printf("    sig_type_name = %s\n",sig_type_name) ;
            if(ip)printf("    n_sig_types   = %i\n",raw_signal_db->n_sig_types) ;
            for (j=0;j<raw_signal_db->n_sig_types; j++){
              string1 = strdup(raw_signal_db->sig_type[j].name) ;
              string1 = strip_quotes(string1) ;
              if(ip)printf("  Signal Types  %s :: %i %s\n",sig_type_name, j, string1) ;
              if(!strcmp(sig_type_name, string1)){
                signal->sig_type = sig_type = &(raw_signal_db->sig_type[j]) ;
                free(string1) ;
                break ;
              }
              free(string1) ;
            }
            if(!sig_type){
              printf("  ERROR  SigType not found\n") ;
            }
/*
 * *****************************************************************************
 *  For each track item find the signal shape sub-object describing this shape
 * *****************************************************************************
 */
            k = sig_object->sub_object[i] ;
            if(ip){
              printf("\n  SEARCH FOR SIGNAL SUB_OBJECT MATCHING SIGNAL TYPE\n") ;
              printf("    Signal object = %p\n",(void *)sig_object) ;
              printf("    Sub-object    = %p\n",(void *)sig_object->sub_object) ;
              printf("    Index         = %i\n",k) ;
            }
            sig_subobj = &(sig_shape->sig_subobj[k]) ;
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
 *  Setup names
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
          }
/*
 * *****************************************************************************
 *  At this point the signal heads are initialised.
 *  Next loop over the shape matrices.
 *    IF the matrix name starts 'HEAD', set the signal link (if it exists)
 *      OR set skip and continue loop
 *    IF the semaphore flag is TRUE, set the animation link to semaphore_pos.
 * *****************************************************************************
 */
          snode = witem->snode ;
          it = 0 ;
          if(ip)printf("\n  INITIALISE MATRIX AND ANIMATION\n\n") ;
          for(im=0;im<(int)snode->nmatrices;im++){
            if(ip)printf("    Matrix %i, name = %s\n",
                                                 im,snode->matrix[im].name) ;
            if(strncmp_ic(snode->matrix[im].name,"HEAD",4))continue; //  Skip
            if(it >=  witem->u.signal_obj.n_signal_units){
              witem->u.signal_obj.skip[im] = 1 ;
            }else{
              n        = witem->u.signal_obj.tr_item[it++] ;
              trk_item = &(track_db.trk_items_array[n]) ;
              signal   = trk_item->signal ;
              witem->u.signal_obj.signal[im] = signal ;
              if(signal->sig_type->semaphore){
                witem->animations[im]=&(signal->semaphore_pos) ;
              }
            }
            if(ip && signal)print_signal_data(signal) ;
          }
        }
      }
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
 *  drawing each signal
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
  TextureNode  *tnode     ;
  char      *script_file  ;
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
