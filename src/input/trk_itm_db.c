/*
 *==============================================================================
 *   File: trk_itm_db.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File contains routines for reading the track items
 *   and populating the data structures
 *
 *==============================================================================
 */

int init_track_item(TrkItem *trk_item) ;

int read_track_item(TrkItem *trk_item, MSfile *msfile)
{
int  j, k   ;
int  ip = 0 ;                                    // Debug printing
char my_name[] = "read_track_item" ;
char *token = NULL ;
/*
 * ***********************************************************************
 *  Initialise Item Type
 * ***********************************************************************
 */
      init_track_item(trk_item) ;

/*
 * ***********************************************************************
 *  Read Item Type
 * ***********************************************************************
 */
      token = new_tmp_token(msfile) ;
      if(ip)printf("  Routine %s, token = %s\n",my_name,token) ;
      if(0 == strcmp(token,"CrossoverItem"))
//        trk_item-> type_of_node = trackdb(CROSSOVER);
//      enum trackdb  trk_item->type_of_node = CROSSOVER    ;
        trk_item->type_of_node = CROSSOVER ;
//        type_of_node = CROSSOVER ;
      else if (0 == strcmp(token,"HazzardItem"))
        trk_item-> type_of_node = HAZZARD      ;
      else if (0 == strcmp(token,"LevelCrItem"))
        trk_item-> type_of_node = LEVEL_CROSSING ;
      else if (0 == strcmp(token,"PickupItem"))
        trk_item-> type_of_node = PICKUP       ;
      else if (0 == strcmp(token,"PlatformItem"))
        trk_item-> type_of_node = PLATFORM     ;
      else if (0 == strcmp(token,"SidingItem"))
        trk_item-> type_of_node = SIDING       ;
      else if (0 == strcmp(token,"SignalItem"))
        trk_item-> type_of_node = SIGNALS      ;
      else if (0 == strcmp(token,"SoundRegionItem"))
        trk_item-> type_of_node = SOUND_REGION ;
      else if (0 == strcmp(token,"SpeedPostItem"))
        trk_item-> type_of_node = SPEED_POST   ;
      else if (0 == strcmp(token,"EmptyItem"))
        trk_item-> type_of_node = EMPTY_ITEM   ;
      else{
        printf("  Routine %s, file %s :  Unrecognised track item.\n",
                                                     my_name,"__FILE__");
        printf("    Track item is %s\n", token );
        printf("  Program stopping ... \n");
        exit(1) ;
      }
      skip_lbr(msfile) ;              // Opening bracket
/*
 * ***********************************************************************
 *  Loop over item fields
 * ***********************************************************************
 */
      for(;;){
        token = new_tmp_token(msfile) ;
        if(ip)printf("  Routine %s,   sub-token = %s\n",my_name,token) ;
        if(')' == token[0])break   ;     // Closing bracket
        SWITCH(token)
          CASE("TrItemId")
            skip_lbr(msfile) ;
            trk_item->uid = itoken(msfile) ;
            skip_rbr(msfile) ;
            if(ip)printf("  Routine %s,   index of node = %i\n",
                                      my_name,trk_item->uid) ;
            break;

          CASE("TrItemRData")
            skip_lbr(msfile) ;
            trk_item->east_x        = dtoken(msfile) ;  // location in world tile
            trk_item->height_y      = dtoken(msfile) ;  // location in world tile
            trk_item->north_z       = dtoken(msfile) ;  // location in world tile
            trk_item->tile_east_x   = itoken(msfile) ;  // x value world tile (tracknode tile)
            trk_item->tile_north_z  = itoken(msfile) ;  // z value world tile
            trk_item->tritemrdata   = 1 ;               // R data read
            skip_rbr(msfile) ;
            break;

          CASE("TrItemPData")
            skip_lbr(msfile) ;
            trk_item->p_east_x        = dtoken(msfile) ;
            trk_item->p_north_z       = dtoken(msfile) ;
            trk_item->p_tile_east_x   = itoken(msfile) ;
            trk_item->p_tile_north_z  = itoken(msfile) ;
            trk_item->tritempdata     = 1 ;               // P data read
            skip_rbr(msfile) ;
            break;

          CASE("TrItemSData")
            skip_lbr(msfile) ;
            trk_item->s_data1         = dtoken(msfile) ;  // Extra data
            trk_item->s_data2         = ctoken(msfile) ;
            trk_item->tritemsdata     = 1 ;               // S data read
            skip_rbr(msfile) ;
            break;

          CASE("TrItemSRData")
            skip_lbr(msfile) ;
            trk_item->sr_data1        = itoken(msfile) ;
            trk_item->sr_data2        = itoken(msfile) ;
            trk_item->sr_data3        = dtoken(msfile) ;
            trk_item->tritemsrdata    = 1 ;               // SR data read
            skip_rbr(msfile) ;
            break;

          CASE("SidingTrItemData")
            skip_lbr(msfile) ;
            trk_item->siding_data1    = ctoken(msfile) ;
            trk_item->siding_data2    = itoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("SidingName")
            skip_lbr(msfile) ;
            trk_item->siding_name     = strip_quotes(ctoken(msfile)) ;
            skip_rbr(msfile) ;
            break;

          CASE("TrSignalType")
            skip_lbr(msfile) ;
            trk_item->signal_data1     = ctoken(msfile) ;   // Flags1  00000001 if junction link set
            trk_item->signal_direction = itoken(msfile) ;   // Direction 0/1
            trk_item->signal_data3     = dtoken(msfile) ;   // Float ??
            trk_item->signal_type_name = ctoken(msfile) ;   // SignalType
            skip_rbr(msfile) ;
            break;

          CASE("TrSignalDirs")
            skip_lbr(msfile) ;
            trk_item->signal_num_dirs = itoken(msfile) ;   //NoSigDirs
            if(trk_item->signal_num_dirs>4){
              printf(" Signal : signal_num_dirs>4\n");
              printf("  Program stopping ... \n");
              exit(1) ;
            }
            for(j=0;j<(int)trk_item->signal_num_dirs;j++){
              new_tmp_token(msfile) ;                   // TrSignalDirs
              skip_lbr(msfile);                   // '('
              for(k=0;k<4;k++){
                trk_item->signal_dirs[j][k] = itoken(msfile) ;  // uint 0 = TrackNode  Junction track node
                                                                // uint 1 = Sd1        May indicate link is a junction
                                                                // uint 2 = LinkLRPath Index of junction 1 = left
                                                                // uint 3 = Sd3        0/1 Used with junction
              }
              skip_rbr(msfile);                   // ')'
            }
            skip_rbr(msfile) ;                   // ')'
            break;

          CASE("SpeedpostTrItemData")
            skip_lbr(msfile) ;
            trk_item->speedpost_flags  = itoken(msfile) ;
            trk_item->is_warning       = trk_item->speedpost_flags & 0x01 ;
            trk_item->is_limit         = trk_item->speedpost_flags & 0x02 ;
            trk_item->is_milepost      = !trk_item->is_warning && !trk_item->is_limit ;
            trk_item->is_resume    = 0 ;
            trk_item->is_passanger = 0 ;
            trk_item->is_freight   = 0 ;
            trk_item->is_mph       = 0 ;
            trk_item->show_number  = 0 ;
            trk_item->show_dot     = 0 ;
            trk_item->speed_ind    = 0 ;
            trk_item->display_number = 0.0 ;
            trk_item->angle          = 0.0 ;
            if(!trk_item->is_milepost){
              if(trk_item->is_warning && trk_item->is_limit){
                trk_item->is_warning = 0 ;
                trk_item->is_resume  = 1 ;
              }

              trk_item->is_passanger = trk_item->speedpost_flags & (1<<5) ;
              trk_item->is_freight   = trk_item->speedpost_flags & (1<<6) ;
              if(trk_item->speedpost_flags & (1<<7)){
                trk_item->is_passanger = 1 ;
                trk_item->is_freight   = 1 ;
              }
              trk_item->is_mph       = trk_item->speedpost_flags & (1<<8) ;
              if(trk_item->speedpost_flags & (1<<4)){
                 trk_item->show_number = 1 ;
                 trk_item->show_dot    = trk_item->speedpost_flags & (1<<9) ;
              }
            }

            if(!trk_item->is_resume){
              if(trk_item->is_milepost && !(trk_item->speedpost_flags & (1<<9)) ){
                trk_item->speed_ind = dtoken(msfile) ;    //   Convert MPH ??
              }else{
                trk_item->speed_ind = dtoken(msfile) ;
              }
            }

            if(trk_item->show_number){
              trk_item->display_number = itoken(msfile) ;
            }
            trk_item->angle          = dtoken(msfile) ;

            skip_rbr(msfile) ;
            break;

          CASE("CrossoverTrItemData")
            skip_lbr(msfile) ;
            trk_item->crossover_data1  = itoken(msfile) ;  // TrackNode
            trk_item->crossover_data2  = itoken(msfile) ;  // ShapeID
            skip_rbr(msfile) ;
            break;

          CASE("PickupTrItemData")
            skip_lbr(msfile) ;
            trk_item->pickup_data1     = itoken(msfile) ;
            trk_item->pickup_data2     = ctoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("PlatformTrItemData")
            skip_lbr(msfile) ;
            trk_item->platform_data1     = ctoken(msfile) ;
            trk_item->platform_data2     = itoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("PlatformName")
            skip_lbr(msfile) ;
            trk_item->platform_name      = strip_quotes(ctoken(msfile)) ;
            skip_rbr(msfile) ;
            break;

          CASE("Station")
            skip_lbr(msfile) ;
            trk_item->station            = ctoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("PlatformMinWaitingTime")
            skip_lbr(msfile) ;
            trk_item->platform_wait_time = itoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("PlatformNumPassengersWaiting")
            skip_lbr(msfile) ;
            trk_item->platform_passengers = itoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          DEFAULT
            printf("  Routine %s, file %s :  Unrecognised track sub-item.\n",
                                                          my_name,"__FILE__");
            printf("    Track type of node is %s\n",
                                      token_trackdb[trk_item->type_of_node]-1);
            printf("    Track sub-item is %s\n", token );
            printf("  Program stopping ... \n");
            exit(1) ;
        END

      }
      if(ip)printf("  Exit routine %s\n\n",my_name) ;
      return 0;
}


int init_track_item(TrkItem *trk_item){

      trk_item->uid    = UINT_MAX ;
      trk_item->type_of_node     = 0    ;
      trk_item->track_section    = 0    ;

      trk_item->trk_item_index   = 0    ;
      trk_item->sect_distance    = 0.0 ;

      trk_item->tritemrdata      = 0 ;
      trk_item->tritempdata      = 0 ;
      trk_item->tritemsdata      = 0 ;
      trk_item->tritemsrdata     = 0 ;

      trk_item->tile_east_x      = 0    ;
      trk_item->tile_north_z     = 0    ;
      trk_item->p_tile_east_x    = 0    ;
      trk_item->p_tile_north_z   = 0    ;
      trk_item->east_x           = 0.0  ;
      trk_item->height_y         = 0.0  ;
      trk_item->north_z          = 0.0  ;
      trk_item->p_east_x         = 0.0  ;
      trk_item->p_north_z        = 0.0  ;

      trk_item->s_data1          = 0.0  ;
      trk_item->s_data2          = NULL ;

      trk_item->sr_data1          = 0    ;
      trk_item->sr_data2          = 0    ;
      trk_item->sr_data3          = 0.0  ;

      trk_item->siding_data1      = NULL ;
      trk_item->siding_data1      = 0    ;
      trk_item->siding_name       = NULL ;

      trk_item->platform_name     = NULL ;

      trk_item->signal_data1      = 0    ;
      trk_item->signal_direction  = 0    ;
      trk_item->signal_data3      = 0.0  ;
      trk_item->signal_type_name  = NULL ;
      trk_item->signal            = NULL ;
      trk_item->signal_num_dirs   = 0    ;

      return 0 ;
}

int list_platforms_and_sidings(){

int     i ;
int     type_of_node ;
TrkItem *trk_item    ;

      printf("  Number of items = %i\n",(int)track_db.trk_items_array_size) ;
      for(i=0;i<(int)track_db.trk_items_array_size;i++){
        trk_item = &track_db.trk_items_array[i] ;
        type_of_node = trk_item->type_of_node ;
        if(PLATFORM == type_of_node){
          printf("  %i  Platform = %s\n",i, trk_item->platform_name) ;
        }else if(SIDING == type_of_node){
          printf("  %i  Siding   = %s\n",i, trk_item->siding_name) ;
        }
      }
      printf("  End of List\n") ;
      return 0 ;
}
