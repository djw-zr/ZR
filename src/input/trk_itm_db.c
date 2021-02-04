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

int read_track_item(TrkItemNode *trk_item, MSfile *msfile)
{
int  j, k   ;
int  ip = 0 ;                                    // Debug printing
char my_name[] = "read_track_item" ;
char *token = NULL ;

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
        if(')' == token[0])break   ;     // Closing bracket
        SWITCH(token)
          CASE("TrItemId")
            skip_lbr(msfile) ;
            trk_item->index_of_node = itoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("TrItemRData")
            skip_lbr(msfile) ;
            trk_item->east_x        = dtoken(msfile) ;
            trk_item->height_y      = dtoken(msfile) ;
            trk_item->north_z       = dtoken(msfile) ;
            trk_item->tile_east_x   = itoken(msfile) ;
            trk_item->tile_north_z  = itoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("TrItemPData")
            skip_lbr(msfile) ;
            trk_item->p_east_x        = dtoken(msfile) ;
            trk_item->p_north_z       = dtoken(msfile) ;
            trk_item->p_tile_east_x   = itoken(msfile) ;
            trk_item->p_tile_north_z  = itoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("TrItemSData")
            skip_lbr(msfile) ;
            trk_item->s_data1         = itoken(msfile) ;
            trk_item->s_data2         = ctoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("TrItemSRData")
            skip_lbr(msfile) ;
            trk_item->sr_data1        = itoken(msfile) ;
            trk_item->sr_data2        = itoken(msfile) ;
            trk_item->sr_data3        = dtoken(msfile) ;
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
            trk_item->siding_name     = ctoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("TrSignalType")
            skip_lbr(msfile) ;
            trk_item->signal_data1    = ctoken(msfile) ;
            trk_item->signal_data2    = itoken(msfile) ;
            trk_item->signal_data3    = dtoken(msfile) ;
            trk_item->signal_name     = ctoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("TrSignalDirs")
            skip_lbr(msfile) ;
            trk_item->signal_num_dirs = itoken(msfile) ;
            if(trk_item->signal_num_dirs>4){
              printf(" Signal : signal_num_dirs>4\n");
              printf("  Program stopping ... \n");
              exit(1) ;
            }
            for(j=0;j<(int)trk_item->signal_num_dirs;j++){
              new_tmp_token(msfile) ;                   // TrSignalDirs
              skip_lbr(msfile);                   // '('
              for(k=0;k<4;k++){
                trk_item->signal_dirs[j][k] = itoken(msfile) ;
              }
              skip_rbr(msfile);                   // ')'
            }
            skip_rbr(msfile) ;                   // ')'
            break;

          CASE("SpeedpostTrItemData")
            skip_lbr(msfile) ;
            trk_item->speedpost_data1  = itoken(msfile) ;
            trk_item->speedpost_data2  = itoken(msfile) ;
            trk_item->speedpost_data3  = dtoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("CrossoverTrItemData")
            skip_lbr(msfile) ;
            trk_item->crossover_data1  = itoken(msfile) ;
            trk_item->crossover_data2  = itoken(msfile) ;
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
            trk_item->platform_name      = ctoken(msfile) ;
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
            printf("    Track sub-item is %s\n", token );
            printf("  Program stopping ... \n");
            exit(1) ;
        END

      }
      return 0;
}
