/*
 * *****************************************************************************
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File: road_itm_db.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File contains routines for reading the track items
 *   and populating the data structures
 *
 * *****************************************************************************
 */

int read_road_item(TrkItem *road_item, MSfile *msfile)
{
char   myname[] = "read_road_item" ;
char *token = NULL ;

/*
 * ***********************************************************************
 *  Read Item Type
 * ***********************************************************************
 */
      token = new_tmp_token(msfile) ;
      if (0 != strcmp(token,"LevelCrItem"))
        road_item-> type_of_node = LEVEL_CROSSING ;
      else if (0 != strcmp(token,"EmptyItem"))
        road_item-> type_of_node = NONE           ;
      else if (0 != strcmp(token,"CarSpawnerItem"))
        road_item-> type_of_node = CAR_SPAWNER    ;
      else{
        printf("  Routine %s, file %s :  Unrecognised track item.\n",
                                                     myname,"__FILE__");
        printf("    Track item is %s\n", token );
        printf("    Program stopping ... \n");
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
            road_item->uid = itoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("TrItemRData")
            skip_lbr(msfile) ;
            road_item->east_x        = dtoken(msfile) ;
            road_item->height_y      = dtoken(msfile) ;
            road_item->north_z       = dtoken(msfile) ;
            road_item->tile_east_x   = itoken(msfile) ;
            road_item->tile_north_z  = itoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("TrItemPData")
            skip_lbr(msfile) ;
            road_item->p_east_x        = dtoken(msfile) ;
            road_item->p_north_z       = dtoken(msfile) ;
            road_item->p_tile_east_x   = itoken(msfile) ;
            road_item->p_tile_north_z  = itoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          CASE("TrItemSData")
            skip_lbr(msfile) ;
            road_item->s_data1         = itoken(msfile) ;
            road_item->s_data2         = ctoken(msfile) ;
            skip_rbr(msfile) ;
            break;

          DEFAULT
            printf("  Routine %s, file %s :  Unrecognised track sub-item.\n",
                                                        myname,"__FILE__");
            printf("    Track sub-item is %s\n", token );
            printf("  Program stopping ... \n");
            exit(1) ;
        END

      }
      return 0;
}
