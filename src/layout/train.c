/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  train.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for initialising trains, consists, wagons and travellers
 *
 *==============================================================================
 */

int print_train_data(void) ;
int check_train_movement(TrainNode *t) ;

/*
 *  Routine to initialise the trains and thei positions.  The composition of
 *  each train can be done either through a set of subroutines (add_train(),
 *  add_wagon_to_train(), etc) or (to be implimented) via input files and/or
 *  a gui (graphical user interface).
 */

int   trains_init(void){

  int  ip = 0 ;
  char *my_name = "trains_init" ;

/*
 *  Initialise one or more trains
 */
#if defined ROUTE_USA1

#if 1
      add_new_train("T01") ;
      add_wagon_to_train("T01","AcelaHHL", 1) ;
      add_wagon_to_train("T01","AcelaHHLCO", 1) ;
      add_wagon_to_train("T01","AcelaHHLCO", 1) ;
      add_wagon_to_train("T01","AcelaHHLCO", 1) ;
      add_wagon_to_train("T01","AcelaHHLCO", 1) ;
      add_wagon_to_train("T01","AcelaHHLCAFE", 1) ;
      add_wagon_to_train("T01","AcelaHHLCO", 1) ;
      add_wagon_to_train("T01","AcelaHHLCO", 1) ;
      add_wagon_to_train("T01","AcelaHHLCO", 1) ;
      add_wagon_to_train("T01","AcelaHHLBC", 1) ;
      position_train("T01",779,7,1,8.0) ;     //  Station Platform
#endif

#if 1
      add_new_train("T02") ;
      add_wagon_to_train("T02","AcelaHHL", 1) ;
      add_wagon_to_train("T02","AcelaHHLCO", 1) ;
      add_wagon_to_train("T02","AcelaHHLCO", 1) ;
      add_wagon_to_train("T02","AcelaHHLCO", 1) ;
      add_wagon_to_train("T02","AcelaHHLCO", 1) ;
      add_wagon_to_train("T02","AcelaHHLCAFE", 1) ;
      add_wagon_to_train("T02","AcelaHHLCO", 1) ;
      add_wagon_to_train("T02","AcelaHHLCO", 1) ;
      add_wagon_to_train("T02","AcelaHHLCO", 1) ;
      add_wagon_to_train("T02","AcelaHHLBC", 1) ;
      position_train("T02",37,4,0,0.0) ;     //  Station Platform
#endif

#if 1
      add_new_train("T03") ;
      add_wagon_to_train("T03","AcelaHHL", 1) ;
      add_wagon_to_train("T03","AcelaHHLCO", 1) ;
      add_wagon_to_train("T03","AcelaHHLCO", 1) ;
      add_wagon_to_train("T03","AcelaHHLCO", 1) ;
      add_wagon_to_train("T03","AcelaHHLCO", 1) ;
      add_wagon_to_train("T03","AcelaHHLCAFE", 1) ;
      add_wagon_to_train("T03","AcelaHHLCO", 1) ;
      add_wagon_to_train("T03","AcelaHHLCO", 1) ;
      add_wagon_to_train("T03","AcelaHHLCO", 1) ;
      add_wagon_to_train("T03","AcelaHHLBC", 1) ;
      position_train("T03",700,2,0,0.0) ;     //  Siding
#endif


#if 1
      add_new_train("T04") ;
      add_wagon_to_train("T04","AcelaHHL", 1) ;
      add_wagon_to_train("T04","AcelaHHLCO", 1) ;
      add_wagon_to_train("T04","AcelaHHLCO", 1) ;
      add_wagon_to_train("T04","AcelaHHLCO", 1) ;
      add_wagon_to_train("T04","AcelaHHLCO", 1) ;
      add_wagon_to_train("T04","AcelaHHLCAFE", 1) ;
      add_wagon_to_train("T04","AcelaHHLCO", 1) ;
      add_wagon_to_train("T04","AcelaHHLCO", 1) ;
      add_wagon_to_train("T04","AcelaHHLCO", 1) ;
      add_wagon_to_train("T04","AcelaHHLBC", 1) ;
//      position_train("T04",1119,16,0,0.0) ;     //  Station Platform
      position_train("T04",1146,8,0,10.0) ;     //  Station Platform
#endif


#elif defined ROUTE_USA2

#if 1
      add_new_train("T01") ;
      add_wagon_to_train("T01","Dash9", 1) ;

      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;  //  Oil Tanker
      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;
      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;
      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;
      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;
      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;
      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;
      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;
      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;
      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;
      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;
      add_wagon_to_train("T01","US2FullLoggerCar", 1) ;

      position_train("T01",730,1,0,0.0) ;    // Shelby South
#endif

#if 1

      add_new_train("T02") ;
      add_wagon_to_train("T02","Dash9", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1)  ;
      add_wagon_to_train("T02","US2Freightcar3", 1)  ;   //  Oil ??
      add_wagon_to_train("T02","US2EmpLoggerCar", 1) ;   //  Empty logs
      add_wagon_to_train("T02","US2Freightcar4", 1)  ;   //  Trailer on wagon
      add_wagon_to_train("T02","US2BNSFCar", 1)      ;   //  Coal ??
      add_wagon_to_train("T02","US2Freight6", 1)     ;   //  Large green goods
      add_wagon_to_train("T02","US2Freightcar3", 1)  ;   //
      add_wagon_to_train("T02","US2Freight7", 1)     ;   //  Red Gooods - Santa Fe
      add_wagon_to_train("T02","US2Freight8", 1)     ;   //  Large green - Burlington and Northern
      add_wagon_to_train("T02","US2FCarYF2", 1)      ;   //  Large yellow open-sided (closed)
      add_wagon_to_train("T02","US2DbleHgtFrghtRd", 1) ; //  Double stacked red containers
      add_wagon_to_train("T02","US2ChemiCar", 1)     ;   //  Small chemicak car
      add_wagon_to_train("T02","US2FreightcarYel1", 1) ; //  Large yellow open sided (?) goods
      add_wagon_to_train("T02","US2FCarRE2", 1)      ;   //  Empty red open-sided wagon
      add_wagon_to_train("T02","US2FullLoggerCar", 1) ;  //  Full of logs
      add_wagon_to_train("T02","US2WoodChipper", 1)  ;   //  Green open top - Burlington and Northern
      add_wagon_to_train("T02","US2HopperCar", 1)    ;   //  Hopper car (grey)
      add_wagon_to_train("T02","US2GrainCar", 1)     ;   //  Hopper car (brown) grain
      add_wagon_to_train("T02","US2DbleHgtFrghtBl", 1) ; //  Double stacked blue containers
      add_wagon_to_train("T02","US2FCarYE2", 1)      ;   //  Empty yellow open-sided wagon
      add_wagon_to_train("T02","US2Freightcar5", 1)  ;   //  Empty flat freight car
//      position_train("T02",512,4,1,0.0) ;     //  Whitefish siding
      position_train("T02",541,1,1,0.0) ;     //  Whitefish siding
//      position_train("T02",533,11,0,0.0) ;
//      position_train("T02",467,7,0,0.0) ;
#endif

#if 1
      add_new_train("T03") ;
//      add_wagon_to_train("T02","SD40", 1) ;  //  Shape errors ??
      add_wagon_to_train("T03","Dash9", 1) ;

      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;
      add_wagon_to_train("T03","US2Freightcar3", 1) ;

      position_train("T03",431,3,0,0.0) ;    //  Columbia Falls Siding
#endif

#if 1
      add_new_train("T04") ;
      add_wagon_to_train("T04","GP38", 1) ;

      add_wagon_to_train("T04","US2Freight6", 1) ;
      add_wagon_to_train("T04","US2Freight6", 1) ;
      add_wagon_to_train("T04","US2Freight6", 1) ;
      add_wagon_to_train("T04","US2Freight6", 1) ;
      add_wagon_to_train("T04","US2Freight6", 1) ;
      add_wagon_to_train("T04","US2Freight6", 1) ;
      add_wagon_to_train("T04","US2Freight6", 1) ;
      add_wagon_to_train("T04","US2Freight6", 1) ;

      position_train("T04",694,1,0,0.0) ;   //  Kalispell
#endif

#if 1
      add_new_train("T05") ;
      add_wagon_to_train("T05","GP38", 1) ;

      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;
      add_wagon_to_train("T05","US2Freightcar4", 1) ;

      position_train("T05",125,1,1,0.0) ;   // Cutback
#endif

#if 1
      add_new_train("T06") ;
      add_wagon_to_train("T06","Dash9", 1) ;

      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;
      add_wagon_to_train("T06","US2Freight8", 1) ;

      position_train("T06",191,5,1,0.0) ;  // Glacier Park
#endif

#if 1
      add_new_train("T07") ;
//      add_wagon_to_train("T07","SD40", 1) ;
      add_wagon_to_train("T07","Dash9", 1) ;

      position_train("T07",205,2,0,0.0) ;  //  Maria - Baloon
#endif

#if 1
      add_new_train("T08") ;
//      add_wagon_to_train("T07","SD40", 1) ;
      add_wagon_to_train("T08","Dash9", 1) ;
      position_train("T07",449,4,0,0.0) ;  //  Crossing with rising arms
#endif



#elif defined ROUTE_EUROPE1

      add_new_train("T01") ;
      add_wagon_to_train("T01","RoyalScotsClass", 1) ;
      add_wagon_to_train("T01","RoyalScotTender", 1) ;
      add_wagon_to_train("T01","ScotsMailCar", 1) ;
      add_wagon_to_train("T01","ScotsFirstClass", 1) ;
      add_wagon_to_train("T01","ScotsFirstClass", 1) ;
      add_wagon_to_train("T01","ScotsFirstClass", 1) ;
      add_wagon_to_train("T01","ScotsFirstClass", 1) ;
      add_wagon_to_train("T01","ScotsThirdClass", 1) ;
      add_wagon_to_train("T01","ScotsThirdClass", 1) ;
      add_wagon_to_train("T01","ScotsThirdClass", 1) ;
      add_wagon_to_train("T01","ScotsThirdClass", 1) ;
      add_wagon_to_train("T01","ScotsThirdClass", 1) ;
      add_wagon_to_train("T01","ScotsThirdClass", 1) ;
      add_wagon_to_train("T01","ScotsThirdClass", 1) ;
      add_wagon_to_train("T01","ScotsThirdClass", 1) ;
      add_wagon_to_train("T01","ScotsThirdClass", 1) ;

      position_train("T01",325,9,0,0.0) ;

      add_new_train("T02") ;
      add_wagon_to_train("T02","Pendennis", 1) ;
      add_wagon_to_train("T02","PendennisTender", 1) ;
      add_wagon_to_train("T02","GWRFirst", 1) ;
      add_wagon_to_train("T02","GWRFirst", 1) ;
      add_wagon_to_train("T02","GWRFirst", 1) ;
      add_wagon_to_train("T02","GWRFirst", 1) ;
      add_wagon_to_train("T02","GWRRest", 1) ;
      add_wagon_to_train("T02","GWRRest", 1) ;
      add_wagon_to_train("T02","GWRThird", 1) ;
      add_wagon_to_train("T02","GWRThird", 1) ;
      add_wagon_to_train("T02","GWRThird", 1) ;
      add_wagon_to_train("T02","GWRThird", 1) ;
      add_wagon_to_train("T02","GWRThird", 1) ;
      add_wagon_to_train("T02","GWRThird", 1) ;
      add_wagon_to_train("T02","GWRThird", 1) ;
      add_wagon_to_train("T02","GWRThird", 1) ;
      add_wagon_to_train("T02","GWRLuggCar", 1) ;
      add_wagon_to_train("T02","GWRLuggCar", 1) ;

      position_train("T02",329,10,0,30.0) ;


      add_new_train("T03") ;
      add_wagon_to_train("T03","Scotsman", 1) ;
      add_wagon_to_train("T03","ScotsTender", 1) ;
      add_wagon_to_train("T03","ScotsCar1", 1) ;
      add_wagon_to_train("T03","ScotsCar1", 1) ;
      add_wagon_to_train("T03","ScotsCar1", 1) ;
      add_wagon_to_train("T03","ScotsCar3", 1) ;
      add_wagon_to_train("T03","ScotsCar3", 1) ;
      add_wagon_to_train("T03","ScotsCar3", 1) ;
      add_wagon_to_train("T03","ScotsCar3", 1) ;
      add_wagon_to_train("T03","ScotsCar3", 1) ;

      position_train("T03",384,17,0,10.0) ;


#elif defined ROUTE_EUROPE2

      add_new_train("T01") ;
      add_wagon_to_train("T01","310", 1) ;
      add_wagon_to_train("T01","310Tender", 1) ;
      add_wagon_to_train("T01","OESleepCar1", 1) ;
      add_wagon_to_train("T01","OESleepCar2", 1) ;
      add_wagon_to_train("T01","OESleepCar3", 1) ;
      add_wagon_to_train("T01","OESleepCar4", 1) ;
      add_wagon_to_train("T01","OESleepCar5", 1) ;
      add_wagon_to_train("T01","OESleepCar6", 1) ;
      add_wagon_to_train("T01","OESleepCar7", 1) ;
      add_wagon_to_train("T01","OESleepCar8", 1) ;
      add_wagon_to_train("T01","OESleepCar9", 1) ;

      position_train("T01",29,7,1,0.0) ;


#elif defined ROUTE_JAPAN1

      add_new_train("T01") ;
      add_wagon_to_train("T01","KIHA31a", 1) ;
      add_wagon_to_train("T01","KIHA31", 1) ;
      position_train("T01",746,5,0,0.0) ;

      add_new_train("T02") ;
      add_wagon_to_train("T02","KIHA31a", 1) ;
      add_wagon_to_train("T02","KIHA31", 1) ;
      position_train("T02",32,10,0,0.0) ;

#elif defined ROUTE_JAPAN2
      add_new_train("T01") ;
      add_wagon_to_train("T01","KIHA31a", 1) ;
      add_wagon_to_train("T01","KIHA31", 1) ;

      position_train("T01",314,1,0,0.0) ;
#if 1
      add_new_train("T02") ;
      add_wagon_to_train("T02","30000", 1) ;
      add_wagon_to_train("T02","30000", 0) ;
//      add_wagon_to_train("T02","30000", 1) ;
      position_train("T02",282,4,0,0.0) ;

      add_new_train("T03") ;
      add_wagon_to_train("T03","KIHA140", 1) ;
      position_train("T03",309,3,0,0.0) ;

      add_new_train("T04") ;
      add_wagon_to_train("T04","2000", 1) ;
      add_wagon_to_train("T04","2000RearEng", 1) ;
      position_train("T04",255,6,0,0.0) ;

      add_new_train("T05") ;
      add_wagon_to_train("T05","2000", 1) ;
      add_wagon_to_train("T05","2000RearEng", 1) ;
      position_train("T05",266,6,0,35.0) ;


      add_new_train("T06") ;
      add_wagon_to_train("T06","2000", 1) ;
      add_wagon_to_train("T06","2000RearEng", 1) ;
//      add_wagon_to_train("T03","Genesis", 1) ;     //  Amtrack
//      add_wagon_to_train("T03","Series7000Carg", 1) ;
//      add_wagon_to_train("T03","2000ACarriage", 1) ;

      position_train("T06",310,9,0,0.0) ;

      add_new_train("T07") ;
      add_wagon_to_train("T07","2000", 1) ;
      add_wagon_to_train("T07","2000RearEng", 1) ;
//      position_train("T07",62,183,0,0.0) ;
      position_train("T07",11,3,0,0.0) ;

#endif

#elif defined ROUTE_TUTORIAL

      add_new_train("T01") ;
      add_wagon_to_train("T01","Dash9", 1) ;
      position_train("T01",13,2,0,0.0) ;

/*
 *  ZIG ZAG Route
 */
#else

#if 1
      add_new_train("T01") ;
#if 1
      add_wagon_to_train("T01","1905-S654", 1) ;
#else
      add_wagon_to_train("T01","1905-D347", 1) ;
      add_wagon_to_train("T01","1905-D347-t", 1) ;
#endif
      add_wagon_to_train("T01","AU_NSW_Dtruck1_Empty", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck2_Empty", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck3_Empty", 1) ;
#if 0
//      add_wagon_to_train("T01","4W-CV-load", 0) ;
      add_wagon_to_train("T01","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T01","NI4wHopUnb4MT", 1) ;
      add_wagon_to_train("T01","4W-CV-mty", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck3_FlatSheet", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck3_HumpSheet", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck3_Coal", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck3_Ballast", 1) ;
      add_wagon_to_train("T01","JWW_Htype_L", 1) ;
      add_wagon_to_train("T01","JWW_Htype_E", 1) ;

      add_wagon_to_train("T01","AU_NSW_Dtruck3_Coal", 1) ;
      add_wagon_to_train("T01","JWW_Htype_L", 1) ;
      add_wagon_to_train("T01","JWW_Htype_E", 1) ;
      add_wagon_to_train("T01","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T01","NI4wHopUnb4MT", 1) ;
//      position_train("T01",376,1,1,0.0) ;
//      position_train("T01",123,1,1,0.0) ;
//      position_train("T01",377,6,0,0.0) ;
//  The direction here has no ef2fect on the way the wagon is displayed
//  before or after movement - whatever teh train direction
//  The error only occurs when the train direction is reverse - but
//  is corrected as soon as the train moves!!!!!!
      add_wagon_to_train("T01","4W-HG-Brake-Van", 0) ;
      add_wagon_to_train("T01","4W-HG-Brake-Van", 1) ;
      add_wagon_to_train("T01","4W-HG-Brake-Van", 0) ;
      add_wagon_to_train("T01","4W-HG-Brake-Van", 1) ;
#endif
//      add_wagon_to_train("T01","4W-HG-Brake-Van", 1) ;
//      add_wagon_to_train("T01","4W-HG-Brake-Van", 1) ;
// This does afffect position
        position_train("T01",392,1,0,0.0) ;  // Default
//      position_train("T01",356,35,1,0.0) ;
//      position_train("T01",363,1,1,0.0) ;
#endif
#if 1
      add_new_train("T02") ;

      add_wagon_to_train("T02","1905-I103", 1) ;

      add_wagon_to_train("T02","4W-LV-load", 1) ;
      add_wagon_to_train("T02","4W-LV-mty", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_Ash", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_Ballast", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_Coal", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_Empty", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_FlatSheet", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_HumpSheet", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck2_Coal", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck2_Empty", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck2_FlatSheet", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck2_HumpSheet", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck3_Ballast", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck3_Coal", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck3_FlatSheet", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck3_HumpSheet", 1) ;

      add_wagon_to_train("T02","4W-HG-Brake-Van", 1) ;
      position_train("T02",373,1,1,0.0) ;
//      position_train("T02",384,1,1,0.0) ;
#endif
#if 1
      add_new_train("T03") ;
      add_wagon_to_train("T03","1905-A115", 1) ;
      add_wagon_to_train("T03","1905-A115-t", 1) ;
      add_wagon_to_train("T03","1905-J484", 1) ;
      add_wagon_to_train("T03","1905-J484-t", 1) ;
      add_wagon_to_train("T03","1905-AD-Sleeper", 1) ;
      add_wagon_to_train("T03","1905-AD-Sleeper", 1) ;
      add_wagon_to_train("T03","1905-AC-StateCar", 1) ;
      add_wagon_to_train("T03","1905-AB-Diner", 1) ;
      add_wagon_to_train("T03","1905-CBC", 1) ;
      add_wagon_to_train("T03","1905-CBC", 1) ;
      add_wagon_to_train("T03","1905-CBC", 1) ;
      add_wagon_to_train("T03","1905-HO-JWW", 1) ;
//      position_train("T03",384,1,1,0.0) ;
      position_train("T03",356,1,1,0.0) ;
//      position_train("T03",415,1,0,0.0) ;
#endif
#if 1
      add_new_train("T04") ;
      add_wagon_to_train("T04","1905-O449", 1) ;
      add_wagon_to_train("T04","1905-O449-t", 1) ;
      add_wagon_to_train("T04","4W-CV-load", 1) ;
      add_wagon_to_train("T04","4W-CV-mty", 1) ;
      add_wagon_to_train("T04","4W-CW2-LD", 1) ;
      add_wagon_to_train("T04","4W-CW2", 1) ;
      add_wagon_to_train("T04","4W-GSV2-Pigs", 1) ;
      add_wagon_to_train("T04","4W-GSV2-Sheep", 1) ;
      add_wagon_to_train("T04","4W-GSV2", 1) ;
      add_wagon_to_train("T04","4W-GSV2", 1) ;
      add_wagon_to_train("T04","4W-LV-load", 1) ;
      add_wagon_to_train("T04","4W-LV-mty", 1) ;
      add_wagon_to_train("T04","4W-HG-Brake-Van", 1) ;
      position_train("T04",415,1,0,0.0) ;
#endif

#if 1
      add_new_train("T05") ;
      add_wagon_to_train("T05","1905-B319", 1) ;
      add_wagon_to_train("T05","1905-B319-t", 1) ;
      add_wagon_to_train("T05","1905-P708", 1) ;
      add_wagon_to_train("T05","1905-P708-t", 1) ;

      add_wagon_to_train("T05","JWW_Htype_E", 1) ;
      add_wagon_to_train("T05","JWW_Htype_L", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","4W-CV-load", 1) ;
      add_wagon_to_train("T05","4W-CV-load", 1) ;
      add_wagon_to_train("T05","4W-CV-load", 1) ;
      add_wagon_to_train("T05","4W-CV-load", 1) ;
      position_train("T05",242,2,0,0.0) ;
#endif

#if 1
      add_new_train("T06") ;
      add_wagon_to_train("T06","1905-S654", 1) ;
      add_wagon_to_train("T06","1905-C144", 1) ;
      add_wagon_to_train("T06","1905-C144-t", 1) ;
      add_wagon_to_train("T06","4W-CV-load", 1) ;
      add_wagon_to_train("T06","4W-CV-load", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb1LD", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb1MT", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb2LD", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb2MT", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb3LD", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb3MT", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb4LD", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb4MT", 1) ;
/*
 *  These wagons cause a segmentation fault
 */
//      add_wagon_to_train("T05","WVR-4W-AcidTank-9-LD", 1) ;
//      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      position_train("T06",51,12,0,0.0) ;
#endif

#if 1
      add_new_train("T07") ;
      add_wagon_to_train("T07","1905-T524", 1) ;
      add_wagon_to_train("T07","1905-T524-t", 1) ;
      add_wagon_to_train("T07","4W-CV-load", 1) ;
      add_wagon_to_train("T07","1905-AD-Sleeper", 1) ;
      add_wagon_to_train("T07","1905-AB-Diner", 1) ;
      add_wagon_to_train("T07","1905-CBC", 1) ;
      add_wagon_to_train("T07","1905-CBC", 1) ;
      add_wagon_to_train("T07","1905-CBC", 1) ;
      add_wagon_to_train("T07","1905-HO-JWW", 1) ;
      position_train("T07",485,5,0,0.0) ;
#endif

#endif
      mark_textures()              ;
//      load_needed_display_lists()  ;

      clock_gettime(CLOCK_MONOTONIC, &run_clock1) ;
      update_trains() ;  // Update using monotonic time
      update_trains() ;  // Update using monotonic time

      if(ip)print_train_data() ;

      return 0  ;
}

/*
 *  Routine to initialise a WagonNode from the list of raw wagons
 *  Name is name of wagon in rawwagonenode list
 */

WagonNode *wagon_init(char *name){

  int          n    ;
  RawWagonNode *raw ;
  WagonNode    *w   ;
  char         *my_name = "wagon_init" ;

      if(strlen(name) == 0){
        printf("ERROR.  Routine %s.  Parameter name has zero length\n",my_name);
        exit(-1) ;
      }

//  Search for name

      n = 1 ;
      for(raw = rawwagonlist_beg; raw==NULL; raw = raw->next){
        if((n=strcmp(raw->name,name))==0) break ;
      }
      if(n != 0){
         printf(" Routine %s.  Unable to find wagon %s\n",my_name,name);
         exit(-1) ;
      }
/*
 *  Generate new WagonNode
 */
      w = (WagonNode *)malloc(sizeof(WagonNode)) ;
      w->next       = NULL  ;
      w->prev       = NULL  ;
      w->train      = NULL  ;
      w->name       = (char *)malloc(strlen(name)*sizeof(char)) ;
      strcpy(w->name,name)  ;
      w->index      = 0     ;
      n = strlen(raw->file) ;
      w->is_engine  = !strcmp(".eng",&(raw->file[n-4]));
      w->train_dir  = 1     ;     // Initialise as 'true'
      w->n_travel   = 1     ;     // NOTE Change for multi-bogie wagons
      w->dist_front = 10.0  ;     // NOTE Change once sd file read
      w->dist_back  = 10.0  ;
      w->wheel_angle       = 0.0 ;  //  Angle of wheels
      w->driverwheel_angle = 0.0 ;  //  Changes when train moves
      w->traveller  = NULL  ;     //  Initialise once train is generated
      w->shape      = NULL  ;
      w->raw_wagon  = raw   ;
//      w->engine     = NULL  ;

      w->has_wipers      = 0 ;
      w->wipers_on       = 0 ;
      w->wipers_off      = 0 ;
      w->has_mirrors     = 0 ;
      w->mirrors_out     = 0 ;
      w->has_pantographs = 0 ;
      w->pantographs_up  = 0 ;
      w->wipers_ang      = 0.0 ;
      w->mirrors_dist    = 0.0 ;
      w->pantographs_dist= 0.0 ;

      return w ;
}

TrainNode *consist_init(WagonNode *wagon, char *name){

  TrainNode    *c   ;
  char         *my_name = "consist_init" ;

      if(wagon == NULL){
        printf("ERROR.  Routine %s.  Parameter wagon is NULL\n",my_name);
        exit(-1) ;
      }
      if(strlen(name) == 0){
        printf("ERROR.  Routine %s.  Parameter name has zero length\n",my_name);
        exit(-1) ;
      }

      c = (TrainNode *)malloc(sizeof(TrainNode)) ;
      c->next       = NULL ;
      c->name       = (char *)malloc(strlen(name)*sizeof(char)) ;
      strcpy(c->name,name)  ;
      c->speed      = 0.0   ;
      c->first      = wagon ;
      c->last       = wagon ;
      c->motor      = (wagon->is_engine) ? wagon : NULL ;

      return c ;
}

/*
 *==============================================================================
 *  Routine to add one wagon onto the end of a consist
 *==============================================================================
 */

int *consist_add_wagon(TrainNode *c, WagonNode *w){

  WagonNode    *new  ;
  WagonNode    *last  ;
  char         *my_name = "consist_add_wagon" ;

      if(c == NULL){
        printf("ERROR. Routine %s.  Parameter TrainNode is NULL\n",my_name);
        exit(-1) ;
      }
      if(w == NULL){
        printf("ERROR. Routine %s.  Parameter WagonNode is NULL\n",my_name);
        exit(-1) ;
      }
/*
 *  Generate new wagon mode and copy across data
 *  Variables next, prev and traveller must be reinitialised later
 */
      new = (WagonNode *)malloc(sizeof(WagonNode)) ;
      *new = *w ;               //  Copy contents

      last           = c->last         ;
      new->index     = last->index + 1 ;
      new->traveller = NULL            ;
      new->prev      = last            ;
      last->next     = new             ;
      c->last        = new             ;
      if(c->motor == NULL){
        if(new->is_engine) c->motor = new ;
      }
      return 0;
}

/*
 *==============================================================================
 *  Routine to intiialise a train.
 *  This add a train to the train list, gives it a name
 *  and initialises the variables
 *==============================================================================
 */

int  add_new_train(char *name){

int ip   = 0 ;  // Debug
TrainNode *t ;
char      *my_name = "add_new_train" ;

      t = (TrainNode *)malloc(sizeof(TrainNode)) ;
      t->name  = strdup(name) ;
      t->speed = 0.0  ;
      t->next  = NULL ;
      t->motor = NULL ;
      t->first = NULL ;
      t->last  = NULL ;

      if(NULL == trainlist_beg) trainlist_beg = t ;
      if(NULL != trainlist_end){
        trainlist_end->next = t ;
      }
      trainlist_end = t       ;
      if(NULL == player_train) player_train = t ;
      if(ip){
        printf("  Routine %s : train beg    = %s\n",my_name,trainlist_beg->name) ;
        printf("             : train end    = %s\n",trainlist_end->name) ;
        printf("             : player train = %s\n",player_train->name) ;
      }
      return 0 ;
}
/*
 *==============================================================================
 *==============================================================================
 */

 int   add_wagon_to_train(char *train, char *wagon, int idirn){

  int           ip = 0     ;  //  Debug
  TrainNode     *t  = NULL ;
  WagonNode     *w  = NULL ;
  RawWagonNode  *rw = NULL ;
  TravellerNode *tv = NULL ;
  char         *my_name = "add_wagon_to_train" ;

/*
 *==============================================================================
 *  Find train in list of TrainNodes
 *==============================================================================
 */
      if(ip)printf(" Routine %s.  Train = %s,  wagon = %s\n",my_name,train,wagon) ;
      for(t = trainlist_beg; t != NULL ; t=t->next){
        if(ip)printf(" Routine %s.  AA Train = %s\n",my_name,t->name) ;
      }
      for(t = trainlist_beg; t != NULL ; t=t->next){
        if(ip)printf(" Routine %s.  BB Train = %s\n",my_name,t->name) ;
        if(!strcmp(t->name,train)) break ;
      }
      if(t==NULL){
        printf(" Routine %s.\n  Unable to fine train with name %s\n",
               my_name,train) ;
        exit(1) ;
      }
/*
 *==============================================================================
 *  Find wagon in list of raw wagons
 *==============================================================================
 */
      for(rw = rawwagonlist_beg; rw != NULL; rw = rw->next){
        if(ip && 0)printf(" Routine %s, wagon name = %s :: %s\n",my_name,
                    rw->name, rw->shape->name) ;
        if(!strcmp(rw->name,wagon)) break ;
      }
      if(rw==NULL){
        printf(" Routine %s.\n  Unable to find wagon with name %s\n",
               my_name,wagon) ;
        exit(1) ;
      }
/*
 *==============================================================================
 *  Create new WagonNode and initialise
 *==============================================================================
 */
      w  = (WagonNode *)malloc(sizeof(WagonNode)) ;
      tv = (TravellerNode *)malloc(sizeof(TravellerNode)) ;

      w->next        = NULL  ;
      w->prev        = NULL  ;
      w->name        = wagon ;
      w->train       = t     ;
      w->index       = 0     ;
      w->is_engine   = rw->is_engine    ;
      w->train_dir   = idirn ;
      w->n_travel    = 1     ;
      w->dist_front  = rw->length*0.5  ;
      w->dist_back   = w->dist_front  ;
      w->wheel_angle = (360.0*rand())/RAND_MAX ;
      w->driverwheel_angle = (360.0*rand())/RAND_MAX ;
      w->traveller   = tv ;
      w->shape       = rw->shape ;
      w->raw_wagon   = rw        ;
//      w->engine      = NULL      ;

      tv->shape  = w->shape ;
      tv->wagon  = w        ;
      tv->shape->needed = 1 ;
      tv->shape->loaded = 0 ;


      if(t->first == NULL) t->first = w ;
      if(t->last != NULL){
        t->last->next = w ;
        w->prev       = t->last ;
      }
      t->last = w ;
      if(ip)printf("*** Routine %s.  Added wagon %s to train %s\n",my_name,w->name,t->name);

/*
 *==============================================================================
 *  Initialise train main engine if appropiate
 *==============================================================================
 */
      if(w->is_engine && t->motor == NULL){
        t->motor = w ;
//        w->engine = (EngineNode *)malloc(sizeof(EngineNode)) ;
//        if(rw->raw_engine != NULL){
          w->has_wipers      = rw->has_wipers  ;
          w->has_mirrors     = rw->has_mirrors ;
          w->has_pantographs = rw->has_pantographs ;
//        }else{
//          w->has_wipers      = 0  ;
//          w->has_mirrors     = 0 ;
//          w->has_pantographs = 0 ;
//        }

        w->wipers_on  = 0   ;
        w->wipers_off = 0   ;
        w->wipers_ang = 0.0 ;

        w->mirrors_out  = 0   ;
        w->mirrors_dist = 0.0 ;

        w->pantographs_up   = 0   ;
        w->pantographs_dist = 0.0 ;

        if(ip){
          printf("*** Routine %s.  Wagon %s has_wipers      = %i\n",my_name,w->name,w->has_wipers);
          printf("*** Routine %s.  Wagon %s has_mirrors     = %i\n",my_name,w->name,w->has_mirrors);
          printf("*** Routine %s.  Wagon %s has_pantographs = %i\n",my_name,w->name,w->has_pantographs);
        }
/*
 *  Check raw wagon for wipers
 */
ShapeNode *shape  ;
Matrix4x3 *matrix ;
char      *m_name ;
int       i       ;

        shape = rw->shape      ;
        matrix = shape->matrix ;
        for(i=0;i<(int)shape->nmatrices;i++){
          m_name = matrix[i].name ;
          if(!strncmp_ic(m_name,"wiper",5))w->has_wipers = 1  ;
          if(!strncmp_ic(m_name,"mirror",6))w->has_mirrors = 1  ;
          if(!strncmp_ic(m_name,"pantograph",10))w->has_pantographs = 1  ;
        }
        if(ip){
          printf("*** Routine %s.  Wagon %s has_wipers      = %i\n",my_name,w->name,w->has_wipers);
          printf("*** Routine %s.  Wagon %s has_mirrors     = %i\n",my_name,w->name,w->has_mirrors);
          printf("*** Routine %s.  Wagon %s has_pantographs = %i\n",my_name,w->name,w->has_pantographs);
        }

//        printf("  Wagon %s, is_engine = %i, engine = %p, wiper = %i\n",
//               w->name, w->is_engine,(void *)w->engine,w->wiper_on);
      }

      return 0 ;
}

int position_train(char *train, int itrack, int ivector, int idirect, double distance){

  int           ip  = 0    ;  // Debug
  TrainNode     *t  = NULL ;
  WagonNode     *w  = NULL ;
  TravellerNode *tv = NULL ;
  double       dist = distance ;
  char         *my_name = "position_train" ;

      if(ip)printf("  Routine %s :: AA\n",my_name) ;
      fflush(NULL) ;
/*
 *  Find train in TrainNode list
 */
      for(t = trainlist_beg; t != NULL ; t=t->next){
        if(!strcmp(t->name,train)) break ;
      }
      if(ip)printf("  Routine %s :: BB\n",my_name) ;
      fflush(NULL) ;
      if(t==NULL){
        printf(" Routine %s.\n  Unable to fine train with name %s\n",
               my_name,train) ;
        exit(1) ;
      }

      fflush(NULL) ;
/*
 *  Loop over wagons
 */
      for(w = t->last ; w != NULL ; w=w->prev){
        if(ip)printf("  Routine %s :: train = %s, dist = %f, wagon = %s\n",
                                      my_name,t->name,dist,w->name) ;
        fflush(NULL) ;
        tv = w->traveller ;
        trv_initv(tv, itrack, ivector, idirect) ;
        trv_move(tv, dist) ;    //  Initialise tv - even if dist == 0.
        if(w->prev != NULL){
          dist += w->dist_front + w->prev->dist_back ;
        }
      }
      l_disp1 = 1 ;  // Switch on printing DEBUG
      return 0 ;
}

int  copy_traveller_position(TravellerNode *tv, TravellerNode *tv0) {

      tv->tn = tv0->tn ;
      tv->vn = tv0->vn ;
      tv->x  = tv0->x  ;
      tv->y  = tv0->y  ;
      tv->z  = tv0->z  ;
      tv->ang_deg  = tv0->ang_deg  ;
      tv->position = tv0->position ;
      tv->itrack   = tv0->itrack ;
      tv->ivector  = tv0->ivector ;
      tv->idirect  = tv0->idirect ;

      return 0 ;
}


int update_trains(void){

  int         ip = 0 ;

  int       iret = 0 ;  //  Return code
  TrainNode *t       ;
  WagonNode *w       ;
  TravellerNode *tv      ;
  TravellerNode *tv0     ;
  double    time, del_t  ;
  double    dist_moved   ;
  double    dist         ;
  char      *my_name = "update_trains" ;
#if 0
  static double time_last = -1.0   ;

      time = (run_clock1.tv_sec - run_clock0.tv_sec)
           + (run_clock1.tv_nsec - run_clock0.tv_nsec)*0.000000001 ;
      if(time_last<0.0){
        if(ip)printf("  Routine %s, time = %f, time_last = %f\n",
                my_name,time,time_last) ;
        time_last = time ;
        return 0         ;
      }
      del_t = time - time_last ;
#else
      time = run_seconds ;
      del_t = delta_seconds ;
#endif

      if(ip){
        printf("  Enter %s\n",my_name) ;
        print_train_data() ;
      }

/*
 *==============================================================================
 *  Loop over trains
 *==============================================================================
 */
      for(t = trainlist_beg; t != NULL ; t=t->next){
        if(ip)printf("  Routine %s :: train = %s, speed = %f\n",
                                                 my_name,t->name,t->speed) ;
/*
 *  Check train movement
 */
        if(0.0 == t->speed) continue ;

        iret = check_train_movement(t) ;  //  Check for obstructions
        if(iret)continue               ;  //  Train stopped by obstruction

        if(ip)printf("  Routine %s :: AA :: train = %s\n",my_name,t->name) ;
        dist_moved = t->speed*del_t ;
        w  = t->last ;
        if(ip)printf("  Routine %s :: wagon = %s\n",my_name,w->name) ;
        tv0 = w->traveller ;
        iret = trv_move(tv0,dist_moved) ;
/*
*  Check if end of line or attempt to enter junction from the wrong direction
*/
        if(iret){
          t->speed = 0.0 ;
          if(iret == 1) {
            printf("  Train %s derailed at junction.  Switch incorrectly set.\n",
                                                                  t->name) ;
          }else{
            printf("  Train %s crashed into buffers at end of line\n",t->name) ;
          }
        }
        camera_changed = 1 ;
        if(w->train_dir){
          w->wheel_angle = w->wheel_angle +
                          degree*dist_moved*w->raw_wagon->inv_wheelradius ;
        }else{
          w->wheel_angle = w->wheel_angle -
                          degree*dist_moved*w->raw_wagon->inv_wheelradius ;
        }
        w->wheel_angle = fmod((double)w->wheel_angle, (double)360.0) ;
        if(w->wheel_angle<0.0)w->wheel_angle += 360.0 ;
        if(ip){
          printf("  Routine %s AA1 :: first  = %p, last = %p\n",
                                  my_name,(void *)t->first,(void *)t->last) ;
          printf("  Routine %s AA2 :: wagon  = %p, prev = %p\n",
                                        my_name,(void *)w,(void *)w->prev) ;
        }
        if(ip)printf(" Wagon = %s,  Angle = %f\n",w->name,w->wheel_angle ) ;

        if(ip)trv_print(w->traveller) ;
#if 0
//     Model position
        tv = w->traveller ;
        vn = tv->vn ;
        a = degree*vn->a_east_x   ;
        b = degree*vn->a_height_y ;
        c = degree*vn->a_north_z  ;
/*
 *   Convert from MSTS location to coordinates used for graphics
 */
          global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                         vn->tile_east_x, vn->tile_north_z,
                         vn->east_x , vn->north_z, vn->height_y+rail_height,
                         &x0, &y0, &z0) ;
          printf("\n") ;
          printf("  AA %f  %f  %f :: %f %f %f\n",x0,y0,z0,tv->x,tv->y,tv->z)
#endif
/*
 *==============================================================================
 *  Loop over wagons - starting with last and moving forward
 *==============================================================================
 */
        dist = 0.0 ;
        for(w=w->prev; w!=NULL; w=w->prev){
          if(ip)printf("  Routine %s :: wagon = %s\n",my_name,w->name) ;
#if 1
          dist = dist_moved + (w->dist_back + w->next->dist_front) ;
          if(ip){
            printf("  Routine %s BB1 :: train = %s, wagon = %s\n",
                                                  my_name,t->name,w->name) ;
            printf("  Routine %s BB2 :: wagon = %p, next = %p\n",
                                        my_name,(void *)w,(void *)w->next) ;
            printf("  Routine %s BB3 :: train = %s, wagon = %s, dist = %f %f %f\n",
                my_name,t->name,w->name, w->dist_back, w->next->dist_front, dist) ;
          }
          tv = w->traveller ;
          dist = (w->dist_back + w->next->dist_front) ;
          copy_traveller_position(tv,tv0) ;
          iret = trv_move(tv, dist) ;
#else
          dist = dist_moved + (w->dist_back + w->next->dist_front) ;
          iret = trv_move(tv,dist_moved) ;
#endif
          if(ip)trv_print(w->traveller) ;
#if 0
//     Model position
          tv = w->traveller ;
          vn = tv->vn ;
          a = degree*vn->a_east_x   ;
          b = degree*vn->a_height_y ;
          c = degree*vn->a_north_z  ;
/*
 *   Convert from MSTS location to coordinates used for graphics
 */
        global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                        vn->tile_east_x, vn->tile_north_z,
                        vn->east_x , vn->north_z, vn->height_y+rail_height,
                        &x1, &y1, &z1) ;
        printf("  BB %f  %f  %f :: %f %f %f\n",x1,y1,z1,tv->x,tv->y,tv->z)
#endif
/*
 *  Check if end of line or attempt to enter junction from the wrong direction
 */
          if(iret || junction_error){
            t->speed = 0.0 ;
            if(iret)printf("  Train %s reached end of line\n",t->name) ;
            if(junction_error)printf("  Train %s derailed at junction.  Switch incorrectly set.\n",
                                                                          t->name) ;
          }
          tv0 = tv ;
          if(ip){
            printf(" BB Wagon = %s,  DistMoved = %f\n",w->name,dist_moved ) ;
            printf(" BB Wagonw->engine = %s,  Radius    = %f\n",w->name,w->raw_wagon->wheelradius ) ;
            printf(" BB Wagon = %s,  InvRadius = %f\n",w->name,w->raw_wagon->inv_wheelradius ) ;
            printf(" BB Wagon = %s,  Angle     = %f\n",w->name,w->wheel_angle ) ;
          }
/*
 *  Update wheel angle
 */
          if(w->train_dir){
            w->wheel_angle += degree*dist_moved*w->raw_wagon->inv_wheelradius;
          }else{
            w->wheel_angle -= degree*dist_moved*w->raw_wagon->inv_wheelradius;
          }
          w->wheel_angle = fmod((double)w->wheel_angle, (double)360.0) ;
          if(w->wheel_angle<0.0)w->wheel_angle += 360.0 ;
          if(w->wheel_angle<0.0 || w->wheel_angle>360.0)printf(" Train.c :: wheel angle = %f\n",
            w->wheel_angle) ;

        }  //  End loop over wagons
      }    //  End loop over trains
      time_last = time ;

/*
 *==============================================================================
 *  Loop over trains updating other variables
 *==============================================================================
 */
      for(t = trainlist_beg; t != NULL ; t=t->next){
/*
 *==============================================================================
 *  Loop over wagons - starting with last and moving forward
 *==============================================================================
 */
        for(w=t->last; w!=NULL; w=w->prev){
/*
 *  Wipers
 */
          if(w->has_wipers && w->wipers_on){
//            printf("    WIPER ANG = %f, dell_t = %f\n",w->wiper_ang,del_t) ;
            w->wipers_ang = w->wipers_ang
                                           + del_t*180.0 ;    //  Back and forth in 2s
            if(w->wipers_ang > 360.0){
              if(w->wipers_off){                        //  Park wipers
                w->wipers_ang = 0.0 ;
                w->wipers_on  = 0   ;
                w->wipers_off = 0   ;
              }else{
                w->wipers_ang -= 360.0 ;
              }
            }
          }
//          printf("    WIPER ANG = %f, dell_t = %f,  wiper_on = %i\n",
//                  w->wiper_ang,del_t,w->wiper_on) ;
/*
 *  Mirrors
 */
          if(w->has_mirrors){
            if(w->mirrors_out && w->mirrors_dist < 1.0){
              w->mirrors_dist = w->mirrors_dist + del_t ;    //  Open in 1s
              if(w->mirrors_dist > 1.0) w->mirrors_dist = 1.0 ;
            }else if(!w->mirrors_out && w->mirrors_dist > 0.0){
              w->mirrors_dist = w->mirrors_dist - del_t ;    //  Close in 1s
              if(w->mirrors_dist < 0.0) w->mirrors_dist = 0.0 ;
            }
          }
/*
 *  Pantographs
 */
          if(w->has_pantographs){
            if(w->pantographs_up && w->pantographs_dist < 1.0){
              w->pantographs_dist = w->pantographs_dist + del_t ;    //  Open in 1s
              if(w->pantographs_dist > 1.0) w->pantographs_dist = 1.0 ;
            }else if(!w->pantographs_up && w->pantographs_dist > 0.0){
              w->pantographs_dist = w->pantographs_dist - del_t ;    //  Close in 1s
              if(w->pantographs_dist < 0.0) w->pantographs_dist = 0.0 ;
            }
          }
        }
      }

      if(ip)printf("  Exit %s\n",my_name) ;
      return 0 ;
}

/*
 *   Routine to check whether a train is due to hit the end of the
 *   line or to enter a junction from the closed branch.
 *   In both cases the train should derail - however for the moment
 *   the train is only stopped and can be reversed.
 */

int check_train_movement(TrainNode *t){

  int           iret  ;
  double        dist  ;
  WagonNode     *w    ;
  TravellerNode *tn   ;
  TravellerNode tc    ;  //  Tempory traveller
  char          *my_name = "check_wagon_movement" ;

      if(t->speed == 0.0) return 0 ;
      junction_error = 0 ;
      if(t->speed > 0.0){
        w = t->first           ;    //  Front of train
        if(w == NULL) return 0 ;
        tn = w->traveller      ;
        tc = *tn               ;    //  Make temporary traveller
        dist = 1.0 + 0.1*t->speed + 0.5*w->raw_wagon->length ;
        iret = trv_move(&tc,dist) ; //  Move temporary traveller
      }else{
        w = t->last            ;    //  Back of train
        if(w == NULL) return 0 ;
        tn = w->traveller      ;
        tc = *tn               ;     //  Make temporary traveller
        dist = 1.0 + 0.1*t->speed + 0.5*w->raw_wagon->length ;
        iret = trv_move(&tc,-dist) ; //  Move temporary traveller
      }
      if(iret || junction_error){    //  Check for errors
        t->speed = 0.0 ;             //  Stop train
        if(iret)printf("  Train %s reached end of line\n",t->name) ;
        if(junction_error){
          printf("  Train %s derailed at junction.  Switch incorrectly set.\n",
                                                                      t->name) ;
          iret           = 1 ;
          junction_error = 0 ;
        }
      }

      return iret ;
}

int print_train_data(void){
  int       ip = 0 ;
  TrainNode *t     ;
  WagonNode *w     ;
  char      *my_name = "print_train_data" ;

      printf("    Enter %s\n",my_name) ;
      for(t = trainlist_beg; t != NULL ; t=t->next){
        printf("      Train = %s, speed = %f\n",t->name,t->speed) ;
        if(t->motor!= NULL)
          printf("       Motor = %s\n",t->motor->name) ;
        for(w = t->first; w != NULL; w=w->next){
          printf("       Wagon = %s,  Distances = %f,  %f\n",
                     w->name, w->dist_front, w->dist_back) ;
          printf("       prev = %p,  w = %p, next = %p\n",
                 (void *)w->prev, (void *)w, (void *)w->next) ;
        }
      }
      printf("    Exit  %s\n",my_name) ;

      return 0 ;
}
/*
 *  List wagons to file wagonlist.txt
 */

int print_wagon_data(void){

  int          ip = 0 ;
  RawWagonNode *rw    ;
  char         *my_name = "print_wagon_data" ;

      for(rw=rawwagonlist_beg; rw!=NULL; rw=rw->next){
        printf("  Wagon: %-25s :: Type: %-8s :: %s\n",rw->name,rw->type,rw->full_name) ;
      }
      return 0;
}

int print_wagon_data_to_file(char *filename){

  int          ip = 0,
               i      ;
  FILE         *fp    ;
  RawWagonNode *rw    ;
  char         *my_name = "print_wagon_data" ;

      fp = fopen(filename,"w") ;
      fprintf(fp,"\n  List of engine and wagon files generated by ZR routine \"%s()\".\n\n",my_name);
      for(i=0; i<4; i++){
        for(rw=rawwagonlist_beg; rw!=NULL; rw=rw->next){
          if((i == 0 && !strcmp(rw->type,"Engine")   ) ||
             (i == 1 && !strcmp(rw->type,"Tender")   ) ||
             (i == 2 && !strcmp(rw->type,"Carriage") ) ||
             (i == 3 && !strcmp(rw->type,"Freight")   ) )
             fprintf(fp,"  Wagon: %-28s :: File: %-10s :: Type: %-8s :: %s\n",rw->name,rw->s_file, rw->type,rw->full_name) ;
        }
      }
      fclose(fp) ;
      return 0;
}

