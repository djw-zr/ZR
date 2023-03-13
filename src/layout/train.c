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

static TrainNode  *build_train ;

/*
 *  Routine to initialise the trains and thei positions.  The composition of
 *  each train can be done either through a set of subroutines (add_train(),
 *  add_wagon_to_train(), etc) or (to be implimented) via input files and/or
 *  a gui (graphical user interface).
 */

int   trains_init(void){

  int  ip = 0 ;
  int  i ;
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
      position_train("T02",700,2,0,0.0) ;     //  Siding
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
//      position_train("T03",1119,16,0,0.0) ;     //  Station Platform
      position_train("T03",1146,8,0,10.0) ;     //  Station Platform
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
      position_train("T02",512,4,1,0.0) ;     //  Whitefish siding
//      position_train("T02",541,1,1,0.0) ;     //  Whitefish siding
//      position_train("T02",541,9,1,-30.0) ;     //  Whitefish siding near signals
//      position_train("T02",485,15,1,0.0) ;     //  Whitefish approach
//      position_train("T02",533,11,0,0.0) ;
//      position_train("T02",467,7,0,0.0) ;
#endif

#if 1
      add_new_train("T02") ;
//      add_wagon_to_train("T02","SD40", 1) ;  //  Shape errors ??
      add_wagon_to_train("T02","Dash9", 1) ;

      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;
      add_wagon_to_train("T02","US2Freightcar3", 1) ;

      position_train("T02",431,3,0,0.0) ;    //  Columbia Falls Siding
#endif

#if 1
      add_new_train("T03") ;
      add_wagon_to_train("T03","GP38", 1) ;

      add_wagon_to_train("T03","US2Freight6", 1) ;
      add_wagon_to_train("T03","US2Freight6", 1) ;
      add_wagon_to_train("T03","US2Freight6", 1) ;
      add_wagon_to_train("T03","US2Freight6", 1) ;
      add_wagon_to_train("T03","US2Freight6", 1) ;
      add_wagon_to_train("T03","US2Freight6", 1) ;
      add_wagon_to_train("T03","US2Freight6", 1) ;
      add_wagon_to_train("T03","US2Freight6", 1) ;

      position_train("T03",694,1,0,0.0) ;   //  Kalispell
#endif

#if 1
      add_new_train("T04") ;
      add_wagon_to_train("T04","GP38", 1) ;

      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;
      add_wagon_to_train("T04","US2Freightcar4", 1) ;

      position_train("T04",125,1,1,0.0) ;   // Cutback
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
      position_train("T08",449,4,0,0.0) ;  //  Crossing with rising arms
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


      add_new_train("T02") ;
      add_wagon_to_train("T02","Scotsman", 1) ;
      add_wagon_to_train("T02","ScotsTender", 1) ;
      add_wagon_to_train("T02","ScotsCar1", 1) ;
      add_wagon_to_train("T02","ScotsCar1", 1) ;
      add_wagon_to_train("T02","ScotsCar1", 1) ;
      add_wagon_to_train("T02","ScotsCar3", 1) ;
      add_wagon_to_train("T02","ScotsCar3", 1) ;
      add_wagon_to_train("T02","ScotsCar3", 1) ;
      add_wagon_to_train("T02","ScotsCar3", 1) ;
      add_wagon_to_train("T02","ScotsCar3", 1) ;

      position_train("T02",384,17,0,10.0) ;


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

      add_new_train("T02") ;
      add_wagon_to_train("T02","KIHA140", 1) ;
      position_train("T02",309,3,0,0.0) ;

      add_new_train("T03") ;
      add_wagon_to_train("T03","2000", 1) ;
      add_wagon_to_train("T03","2000RearEng", 1) ;
      position_train("T03",255,6,0,0.0) ;

      add_new_train("T04") ;
      add_wagon_to_train("T04","2000", 1) ;
      add_wagon_to_train("T04","2000RearEng", 1) ;
      position_train("T04",266,6,0,35.0) ;


      add_new_train("T06") ;
      add_wagon_to_train("T06","2000", 1) ;
      add_wagon_to_train("T06","2000RearEng", 1) ;
//      add_wagon_to_train("T02","Genesis", 1) ;     //  Amtrack
//      add_wagon_to_train("T02","Series7000Carg", 1) ;
//      add_wagon_to_train("T02","2000ACarriage", 1) ;

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


#elif defined ROUTE_AU_NSW_SW_SS

#if 1
      add_new_train("T01") ;
      add_wagon_to_train("T01","AU_NSW_3028_C30T", 1) ;
      add_wagon_to_train("T01","AU_NSW_3028_tender", 1) ;
      for(i=0;i<30;i++){
        add_wagon_to_train("T01","AU_NSW_Dtruck2_Coal", 1) ;
      }
      add_wagon_to_train("T01","LHG_Brake_Van", 1) ;
      position_train("T01",730,1,1,1.0) ;    // Near Lithgow
#endif
#if 1
      add_new_train("T02") ;
      add_wagon_to_train("T02","AU_NSW_3028_C30T", 1) ;
      add_wagon_to_train("T02","AU_NSW_3028_tender", 1) ;
      position_train("T02",659,7,0,0.0) ;    // Near Mount Victoria

      add_new_train("T02") ;
      add_wagon_to_train("T02","AU_NSW_3028_C30T", 1) ;
      add_wagon_to_train("T02","AU_NSW_3028_tender", 1) ;
      position_train("T02",4321,1,0,1.0) ;    // Near Clyde

      add_new_train("T03") ;
      add_wagon_to_train("T03","AU_NSW_3028_C30T", 1) ;
      add_wagon_to_train("T03","AU_NSW_3028_tender", 1) ;
      position_train("T03",5076,2,1,0.0) ;    //  Redfern

      add_new_train("T04") ;
      add_wagon_to_train("T04","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T04","AU_NSW_3102_tender", 1) ;
      position_train("T04",5911,2,0,0.0) ;    // Sydney

      add_new_train("T06") ;
      add_wagon_to_train("T06","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T06","AU_NSW_3102_tender", 1) ;
      position_train("T06",1023,2,0,0.0) ;    // Works

      add_new_train("T07") ;
      add_wagon_to_train("T07","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T07","AU_NSW_3102_tender", 1) ;
      position_train("T07",1611,2,1,0.0) ;    // Rooty Hill

      add_new_train("T08") ;
      add_wagon_to_train("T08","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T08","AU_NSW_3102_tender", 1) ;
      position_train("T08",3262,2,0,0.0) ;    // Cambelltown

      add_new_train("T09") ;
      add_wagon_to_train("T09","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T09","AU_NSW_3102_tender", 1) ;
      position_train("T09",760,3,1,0.0) ;    // Lithgow Loop

      add_new_train("T10") ;
      add_wagon_to_train("T10","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T10","AU_NSW_3102_tender", 1) ;
      position_train("T10",3161,1,1,0.0) ;    //  Richmond

      add_new_train("T11") ;
      add_wagon_to_train("T11","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T11","AU_NSW_3102_tender", 1) ;
      position_train("T11",1701,1,0,0.0) ;    //  Blacktown

      add_new_train("T12") ;
      add_wagon_to_train("T12","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T12","AU_NSW_3102_tender", 1) ;
      position_train("T12",2250,3,1,0.0) ;    //  Clyburn

      add_new_train("T13") ;
      add_wagon_to_train("T13","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T13","AU_NSW_3102_tender", 1) ;
      position_train("T13",2747,5,0,0.0) ;    //  Chester Hill

      add_new_train("T14") ;
      add_wagon_to_train("T14","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T14","AU_NSW_3102_tender", 1) ;
      position_train("T14",5062,3,1,0.0) ;    //  Sydney Raiway Sidings

      add_new_train("T15") ;
      add_wagon_to_train("T15","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T15","AU_NSW_3102_tender", 1) ;
      position_train("T15",7706,2,0,0.0) ;    //  Back of beyond /Town no name
                                              // 1423 10258

      add_new_train("T16") ;
      add_wagon_to_train("T16","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T16","AU_NSW_3102_tender", 1) ;
      position_train("T16",8051,3,0,0.0) ;    //  Yennora

      add_new_train("T18") ;
      add_wagon_to_train("T18","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T18","AU_NSW_3102_tender", 1) ;
      position_train("T18",7495,5,0,0.0) ;    //  Goulburn

      add_new_train("T19") ;
      add_wagon_to_train("T19","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T19","AU_NSW_3102_tender", 1) ;
      position_train("T19",7035,3,0,0.0) ;    //  Moss Vale

      add_new_train("T20") ;
      add_wagon_to_train("T20","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T20","AU_NSW_3102_tender", 1) ;
      position_train("T20",6683,3,1,0.0) ;    //  Picton

      add_new_train("T21") ;
      add_wagon_to_train("T21","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T21","AU_NSW_3102_tender", 1) ;
      position_train("T21",3244,4,0,0.0) ;    //  Cambelltown

      add_new_train("T22") ;
      add_wagon_to_train("T22","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T22","AU_NSW_3102_tender", 1) ;
      position_train("T22",4668,3,0,0.0) ;    //  Line to Newcastle

      add_new_train("T23") ;
      add_wagon_to_train("T23","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T23","AU_NSW_3102_tender", 1) ;
      position_train("T23",1122,5,1,0.0) ;    //  Mudgee / Wallerwang

      add_new_train("T24") ;
      add_wagon_to_train("T24","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T24","AU_NSW_3102_tender", 1) ;
      position_train("T24",1290,2,0,0.0) ;    //  North-west Collery

      add_new_train("T25") ;
      add_wagon_to_train("T25","AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("T25","AU_NSW_3102_tender", 1) ;
      position_train("T25",1038,2,1,0.0) ;    //  Bowenfeld

#endif


/*
 *  ZIG ZAG Route
 */
#else

      add_new_train("T01") ;
      add_wagon_to_train("T01","1905-I103", 1) ;
      position_train("T01",387,3,0,-50.0) ;     //  Station Bowenfels

      add_new_train("T02") ;

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

      position_train("T02",408,3,0,0.0) ;     //  Station Bowenfels

//      add_wagon_to_train("T02","4W-HG-Brake-Van", 1) ;
//      position_train("T02",376,1,1,10.0) ;     //  Station Bowenfels
//      position_train("T02",373,1,1,0.0) ;     //  Station Bowenfels
//      position_train("T02",373,1,0,0.0) ;     //  Train Reversed
//      position_train("T02",357,3,0,0.0) ;
//      position_train("T02",384,1,1,0.0) ;
//      position_train("T02",356,33,0,0.0) ;  //  Near Bowenfels Level Crossing 1

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

      position_train("T03",356,1,1,0.0) ;   // Bowenfels End-of-Line
//      position_train("T03",384,1,1,0.0) ;
//      position_train("T03",415,1,0,0.0) ;

      add_new_train("T04") ;

      add_wagon_to_train("T04","1905-O449", 1) ;
      add_wagon_to_train("T04","1905-O449-t", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T04","4W-HG-Brake-Van", 1) ;

      position_train("T04",431,4,0,70.0) ;

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
      add_wagon_to_train("T05","4W-CV-load", 1) ;
      add_wagon_to_train("T05","4W-CV-mty", 1) ;
      add_wagon_to_train("T05","4W-CW2-LD", 1) ;
      add_wagon_to_train("T05","4W-CW2", 1) ;
      add_wagon_to_train("T05","4W-GSV2-Pigs", 1) ;
      add_wagon_to_train("T05","4W-GSV2-Sheep", 1) ;
      add_wagon_to_train("T05","4W-GSV2", 1) ;
      add_wagon_to_train("T05","4W-GSV2", 1) ;
      add_wagon_to_train("T05","4W-LV-load", 1) ;
      add_wagon_to_train("T05","4W-LV-mty", 1) ;
      position_train("T05",242,2,0,0.0) ;

      add_new_train("T06") ;

      add_wagon_to_train("T06","1905-D347", 1) ;
      add_wagon_to_train("T06","1905-D347-t", 1) ;
      add_wagon_to_train("T06","4W-CV-load", 0) ;
      add_wagon_to_train("T06","4W-CV-mty", 1) ;

      add_wagon_to_train("T06","AU_NSW_Dtruck1_Empty", 1) ;
      add_wagon_to_train("T06","AU_NSW_Dtruck2_Empty", 1) ;
      add_wagon_to_train("T06","AU_NSW_Dtruck3_Empty", 1) ;

      add_wagon_to_train("T06","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T06","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb4MT", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb4MT", 1) ;
      add_wagon_to_train("T06","AU_NSW_Dtruck3_FlatSheet", 1) ;
      add_wagon_to_train("T06","AU_NSW_Dtruck3_HumpSheet", 1) ;
      add_wagon_to_train("T06","AU_NSW_Dtruck3_Coal", 1) ;
      add_wagon_to_train("T06","AU_NSW_Dtruck3_Coal", 1) ;
      add_wagon_to_train("T06","AU_NSW_Dtruck3_Ballast", 1) ;

      add_wagon_to_train("T06","JWW_Htype_E", 1) ;
      add_wagon_to_train("T06","JWW_Htype_L", 1) ;
      add_wagon_to_train("T06","JWW_Htype_L", 1) ;
      add_wagon_to_train("T06","JWW_Htype_E", 1) ;
      add_wagon_to_train("T06","4W-HG-Brake-Van", 1) ;

      position_train("T06",189,5,0,0.0) ;

//      position_train("T06",376,1,1,0.0) ;
//      position_train("T06",123,1,1,0.0) ;
//      position_train("T06",377,6,0,0.0) ;
//        position_train("T06",554,4,0,-10.0) ;  // Mt Victoria Turntable
//        position_train("T06",251,3,0,0.0) ;  // Golburn - Train shed line 3
//        position_train("T06",541,5,0,0.0) ;  // Mt Victoria
//        position_train("T06",356,13,1,10.0) ;  // Bowenfels distant
//        position_train("T06",356,33,1,10.0) ;  // Bowenfels level crossing
//        position_train("T06",392,1,0,20.0) ;  // Default Bowenfels siding
//      position_train("T06",363,1,1,0.0) ;

      add_new_train("T07") ;

      add_wagon_to_train("T07","1905-S654", 1) ;
      add_wagon_to_train("T07","1905-C144", 1) ;
      add_wagon_to_train("T07","1905-C144-t", 1) ;
      add_wagon_to_train("T07","4W-CV-load", 1) ;
      add_wagon_to_train("T07","1905-C144", 1) ;
      add_wagon_to_train("T07","1905-C144-t", 1) ;
      add_wagon_to_train("T07","4W-CV-load", 1) ;
      add_wagon_to_train("T07","4W-CV-load", 1) ;
      add_wagon_to_train("T07","NI4wHopUnb1LD", 1) ;
      add_wagon_to_train("T07","NI4wHopUnb1MT", 1) ;
      add_wagon_to_train("T07","NI4wHopUnb2LD", 1) ;
      add_wagon_to_train("T07","NI4wHopUnb2MT", 1) ;
      add_wagon_to_train("T07","NI4wHopUnb3LD", 1) ;
      add_wagon_to_train("T07","NI4wHopUnb3MT", 1) ;
      add_wagon_to_train("T07","NI4wHopUnb4LD", 1) ;
      add_wagon_to_train("T07","NI4wHopUnb4MT", 1) ;

/*
 *  These wagons cause a segmentation fault
 */
//      add_wagon_to_train("T04","WVR-4W-AcidTank-9-LD", 1) ;
//      add_wagon_to_train("T04","WVR-4W-AcidTank-9", 1) ;

      position_train("T07",51,12,0,0.0) ;

      add_new_train("T08") ;

      add_wagon_to_train("T08","1905-T524", 1) ;
      add_wagon_to_train("T08","1905-T524-t", 1) ;
      add_wagon_to_train("T08","4W-CV-load", 1) ;
      add_wagon_to_train("T08","1905-AD-Sleeper", 1) ;
      add_wagon_to_train("T08","1905-AB-Diner", 1) ;
      add_wagon_to_train("T08","1905-CBC", 1) ;
      add_wagon_to_train("T08","1905-CBC", 1) ;
      add_wagon_to_train("T08","1905-CBC", 1) ;
      add_wagon_to_train("T08","1905-HO-JWW", 1) ;

      position_train("T08",485,5,0,0.0) ;

      add_new_train("T09") ;

      add_wagon_to_train("T09","1905-S654", 1) ;
      add_wagon_to_train("T09","AU_NSW_Dtruck1_Empty", 1) ;
      add_wagon_to_train("T09","AU_NSW_Dtruck2_FlatSheet", 1) ;

      position_train("T09",269,10,0,0.0) ;

//  Wagons in Sidings

      add_new_train("T10") ;
      add_wagon_to_train("T10","4W-CV-load", 1) ;
      add_wagon_to_train("T10","AU_NSW_Dtruck1_Empty", 1) ;
      add_wagon_to_train("T10","AU_NSW_Dtruck1_Empty", 1) ;
      position_train("T10",213,3,1,10.0) ;

      add_new_train("T11") ;
      add_wagon_to_train("T11","AU_NSW_Dtruck1_Ballast", 1) ;
      add_wagon_to_train("T11","AU_NSW_Dtruck1_Ballast", 1) ;
      position_train("T11",271,2,0,0.0) ;

//  Engines in shed

      add_new_train("T12") ;
      add_wagon_to_train("T12","1905-T524", 1) ;
      add_wagon_to_train("T12","1905-T524-t", 1) ;
      position_train("T12",251,5,0,20.0) ;

      add_new_train("T13") ;
      add_wagon_to_train("T13","1905-C144", 1) ;
      add_wagon_to_train("T13","1905-C144-t", 1) ;
      position_train("T13",258,4,1,5.0) ;


//  Trucks only in Bowenfels Siding

      add_new_train("T14") ;
      add_wagon_to_train("T14","AU_NSW_Dtruck1_Ballast", 1) ;
      add_wagon_to_train("T14","AU_NSW_Dtruck1_Ballast", 1) ;
      add_wagon_to_train("T14","AU_NSW_Dtruck1_Ballast", 1) ;
      add_wagon_to_train("T14","AU_NSW_Dtruck1_Empty", 1) ;
      position_train("T14",392,1,1,30.0) ;     //  Station Bowenfels Siding

#endif

      mark_textures()              ;
      clock_gettime(CLOCK_MONOTONIC, &run_clock1) ;
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
      w->is_engine  = raw->is_engine ;
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
      init_trav(&t->front) ;
      init_trav(&t->back) ;


      if(NULL == trainlist_beg) trainlist_beg = t ;
      if(NULL != trainlist_end){
        trainlist_end->next = t ;
      }

      trainlist_end = build_train = t ;
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
 *
 *==============================================================================
 */

 int   add_wagon_to_train(char *train, char *wagon, int idirn){

  int           ip = 0     ;  //  Debug
  TrainNode     *t  = NULL ;
  WagonNode     *w  = NULL ;
  RawWagonNode  *rw = NULL ;
  TravellerNode *z1 = NULL ;
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
        printf(" Routine %s.\n  Unable to find train with name %s\n",
               my_name,train) ;
      }
      t = build_train ;               //  Always use default
      if(!t) return 0 ;               //  No build train defined

/*
 *==============================================================================
 *  Find wagon in list of raw wagons
 *==============================================================================
 */
      for(rw = rawwagonlist_beg; rw != NULL; rw = rw->next){
        if(ip && 1)printf(" Routine %s, wagon = %s, wagon name = %s :: %s\n",
                    my_name, wagon,
                    rw->name, rw->shape->name) ;
        if(!strcmp_ic(rw->name,wagon)) break ;
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
      z1 = (TravellerNode *)malloc(sizeof(TravellerNode)) ;

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
      w->traveller   = z1 ;
      w->shape       = rw->shape ;
      w->raw_wagon   = rw        ;
      w->has_wipers  = 0 ;
      w->has_mirrors     = rw->has_mirrors ;
      w->has_pantographs = rw->has_pantographs ;
      w->wipers_on  = 0   ;
      w->wipers_off = 0   ;
      w->wipers_ang = 0.0 ;

      w->mirrors_out  = 0   ;
      w->mirrors_dist = 0.0 ;

      w->pantographs_up   = 0   ;
      w->pantographs_dist = 0.0 ;

      z1->shape  = w->shape ;
      z1->wagon  = w        ;
      z1->shape->needed = 1 ;
      z1->shape->loaded = 0 ;


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
        w->has_wipers      = rw->has_wipers  ;
        w->has_mirrors     = rw->has_mirrors ;
        w->has_pantographs = rw->has_pantographs ;

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
  int           n          ;
  TrainNode     *t  = NULL ;
  WagonNode     *w  = NULL ;
  TravellerNode *z1 = NULL ;
  double       dist = distance ;
  char         *my_name = "position_train" ;

      if(ip)printf("  Routine %s :: AA\n",my_name) ;
/*
 *  Find train in TrainNode list
 */
      for(t = trainlist_beg; t != NULL ; t=t->next){
        if(!strcmp(t->name,train)) break ;
      }
      if(ip)printf("  Routine %s :: BB\n",my_name) ;
      if(t==NULL){
        printf(" Routine %s.\n  Unable to fine train with name %s\n",
               my_name,train) ;
      }
      t = build_train ;  //  Always assume default
      if(!t) return 0 ;
/*
 *  Loop over wagons
 */
      for(w = t->last, n=0 ; w != NULL ; w=w->prev, n++){
        if(ip)printf("  Routine %s :: train = %s, dist = %f, wagon = %s\n",
                                      my_name,t->name,dist,w->name) ;
        z1 = w->traveller ;
        trv_initv(z1, itrack, ivector, idirect) ;
        trv_move(z1, dist) ;    //  Initialise z1 - even if dist == 0.
        if(w->prev != NULL){
          dist += w->dist_front + w->prev->dist_back ;
        }
      }
      t->n_wagons = n ;
/*
 *  Position front and back of train
 */
      copy_traveller_position(&(t->front), t->first->traveller) ;
      trv_move(&(t->front), 0.5*t->first->raw_wagon->length) ;
      copy_traveller_position(&(t->back), t->last->traveller) ;
      trv_move(&(t->back), -0.5*t->last->raw_wagon->length) ;

      build_train = NULL ;

      return 0 ;
}

/*
 *  Routine copy traveller used to copy second traveller position without
 *  affecting name or shape variables
 */

int  copy_traveller_position(TravellerNode *z1, TravellerNode *z0) {

      z1->tn = z0->tn ;
      z1->vn = z0->vn ;
      z1->x  = z0->x  ;
      z1->y  = z0->y  ;
      z1->z  = z0->z  ;
      z1->ang_deg  = z0->ang_deg  ;
      z1->vect_position = z0->vect_position ;
      z1->sect_distance = z0->sect_distance ;
      z1->itrack   = z0->itrack ;
      z1->ivector  = z0->ivector ;
      z1->idirect  = z0->idirect ;

      return 0 ;
}

/*
 *   Routine to update train positions
 */

static TravellerNode *gzl = NULL ;

int update_trains(void){

  int       ip = 0 ;

  int       iret = 0 ;  //  Return code
  int       i_crash   = 0 ;
  int       i_collide = 0 ;
  int       idirect  ;
  int       isect    ;
  TrainNode *t0, *t1 = NULL ;
  WagonNode *w           ;
  TravellerNode *z0, *z1, gz0, gz1 ;
  TrkSector     *ts      ;
  double    d0, d1, d2   ;
  double    length       ;
  double    time, del_t  ;
  double    dist_to_move ;
  double    dist_remain, dist_moved   ;
  char      *my_name = "update_trains" ;


      if(ip){
        printf("\n  Enter %s.  Run seconds = %f\n",my_name,run_seconds) ;
//        print_train_data() ;
      }
      time = run_seconds ;
/*
 *==============================================================================
 *  Loop over trains
 *==============================================================================
 */
      for(t0 = trainlist_beg; t0 != NULL ; t0=t0->next){

        if(ip)printf("  Routine %s. Main Loop :: train = %s, speed = %f\n",
                                                 my_name,t0->name,t0->speed) ;
        del_t = delta_seconds ;
        if(0.0 == t0->speed) continue ;    //  Nothing to be done
        if(ip)printf("\n  Routine %s.  Train is moving\n",my_name) ;
        camera_changed = 1 ;               //  If connected to this train
        junction_error = 0 ;
/*
 *  First move to limit of train in direction of motion
 */
        if(t0->speed > 0.0){
          w = t0->first          ;    //  Front of train
          if(w == NULL) return 0 ;
          gz0  = *w->traveller   ;    //  Make temporary traveller
          iret = trv_move(&gz0, 0.5*w->raw_wagon->length) ;
        }else{
          w = t0->last            ;    //  Back of train
          if(w == NULL) return 0 ;
          gz0 = *w->traveller    ;     //  Make temporary traveller
          iret = trv_move(&gz0,-0.5*w->raw_wagon->length) ;
        }
        if(iret || junction_error){
          printf("  Routine %s.  Train overlaps buffers or incorrectly"
                                                    " set junction\n",my_name) ;
          trv_print(&gz0) ;
          exit (0) ;
        }
        gz1  = gz0             ;    //  Make a copy
/*
 *  Next check for train collision with buffers or incorrectly set junction
 *  during the current timestep
 */
        dist_to_move = del_t*t0->speed ;
        dist_remain  = dist_to_move ;

        iret = trv_move(&gz1, dist_to_move) ; //  Move temporary traveller
/*
 *  If crash detected, print error and change timestep for this train
 *  Unfortunately dist_remain is not changed (yet).
 */
        i_crash = (iret || junction_error) ;
        if(i_crash){
          del_t = (fabs(dist_to_move) - fabs(dist_remain))/fabs(t0->speed) ;
          if(iret){
            printf("  Train %s crashed into end of line.\n",t0->name) ;
          }else{
            printf("  Train %s derailed at junction.\n",t0->name) ;
          }
/*
 *  If the train has not crashed, user traveller gz0 to check for
 *  collisions with other trains.  .
 */
        }else{
          dist_moved  = 0.0 ;
          dist_remain = dist_to_move ;
          do{
            idirect = gz0.idirect  ;
            isect   = gz0.itrack   ;
            ts      = &track_db.trk_sections_array[isect-1] ; //  Remember '-1'!
            if(ip)printf("\n  Track section = %i, idirect = %i, speed = %f, name = %s"
                   "   dist remain = %f\n",
                        isect, idirect, t0->speed,
                        (gz0.wagon) ? gz0.wagon->name : "(null)", dist_remain ) ;

            if(gz0.idirect && t0->speed>0.0){
              d0 = gz0.sect_distance ;
              d1 = d0 + dist_remain ;
              if(d1 > ts->length){
                d1 = ts->length ;
                dist_remain  -= (ts->length - d0) ;
              }else{
                dist_remain = 0.0 ;
              }
            }else if(gz0.idirect && t0->speed<0.0){
              d1   = gz0.sect_distance ;
              d0   = d1 + dist_remain  ;
              if(d0 < 0.0){
                d0 = 0 ;
                dist_remain += d1 ;
              }else{
                dist_remain = 0.0 ;
              }
            }else if(!gz0.idirect && t0->speed>0.0){
              d1   = gz0.sect_distance ;
              d0   = d1 - dist_remain  ;
              if(d0 < 0.0){
                d0 = 0 ;
                dist_remain -= d1 ;
              }else{
                dist_remain = 0.0 ;
              }
            }else{
              d0 = gz0.sect_distance ;
              d1 = d0 - dist_remain ;
              if(d1 > ts->length){
                d1 = ts->length ;
                dist_remain  -= (ts->length - d0) ;
              }else{
                dist_remain = 0.0 ;
              }
            }
            if(ip)printf("  TT  d0 = %f, d1 = %f, d_remain = %f\n",d0, d1, dist_remain) ;
/*
*  Loop over all other trains searching for collisions.
*  This is linited to collisions between the front and end of trains
*  Collisions with wagons at crossovers and junctions are not covered.
*/
            i_collide = 0 ;
            for(t1 = trainlist_beg; t1!=NULL; t1=t1->next){
              if(t1 == t0)continue ;
              z0 = &(t1->front) ;
              z1 = &(t1->back)  ;
              if(ip)printf("    Train0 = %s, Front = %i %f, Back = %i %f  "
                                "Motor = %i %f\n",
                    t0->name, t0->front.itrack,
                              t0->front.sect_distance,
                              t0->back.itrack,
                              t0->back.sect_distance,
                    (t0->motor) ?t0->motor->traveller->itrack : 0,
                    (t0->motor) ?t0->motor->traveller->sect_distance : 0.0) ;

              if(ip)printf("    Train1 = %s, Front = %i %f, Back = %i %f  "
                           "Motor = %i %f  ::   %i %f :: %i %f %f\n",
                    t1->name, z0->itrack, z0->sect_distance,
                              z1->itrack, z1->sect_distance,
                    (t1->motor) ?t1->motor->traveller->itrack : 0,
                    (t1->motor) ?t1->motor->traveller->sect_distance : 0.0,                              gz0.itrack, gz0.sect_distance, isect, d0, d1) ;
  /*
  *  Does either end of the second train lie in the same track section
  *              and within the distance moved by the ?
  */
              if(ip)printf("  Z0 :: %f %f :: %f\n",d0, d1, z0->sect_distance ) ;
              if(ip)printf("  Z1 :: %f %f :: %f\n",d0, d1, z1->sect_distance ) ;
              if(isect == (int) z0->itrack){
                if(z0->sect_distance > d0 && z0->sect_distance < d1){
                  i_collide = 1 ;   //  Collision with front of train t1
                  if(ip)printf("  COLLIDE with front of train t1\n") ;
                  dist_moved += (z0->idirect) ? (z0->sect_distance - d0)
                                              : (d1 - z0->sect_distance) ;
                  break ;
                }
              }
              if(isect == (int)z1->itrack){
                if(z1->sect_distance > d0 && z1->sect_distance < d1){
                  i_collide = -1 ;   //  Collision with back of train t1
                  if(ip)printf("  COLLIDE with back of train t1\n") ;
                  dist_moved += (z0->idirect) ? (z1->sect_distance - d0)
                                              : (d1 - z1->sect_distance) ;
                  break ;
                }
              }
            }
            if(i_collide) break ;
/*
 *  If dist_remain != 0.0, move ghost traveller to next section.
 *  This also checks for possible crashes.
 */
            if(dist_remain == 0.0)break ;

            if( idirect == (dist_remain > 0.0) ){
              i_crash = trk_next(&gz0,1) ;
            }else{
              i_crash = trk_next(&gz0,-1) ;
            }
            if(i_crash){
              printf("  Routine %s error.  End of line or junction error"
                                " while checking for train collisions\n", my_name) ;
              printf("   idirect = %i\n",idirect) ;
              trv_print(&gz0) ;
              exit(1) ;
            }
          }while(dist_remain != 0.0) ;
/*
 *  In the case of train collisions, set the timestep sufficient to move the
 *  train to the collision point.  The coupling togeather of trains approaching
 *  each other at low speed is treated near the end.
 */
         if(i_collide){
            del_t = fabs(dist_moved/t0->speed) ;
            if(ip)printf(" Collide. Dist moved = %f, speed = %f, del_t = %f\n",
                          dist_moved, t0->speed, del_t) ;
          }
        }
/*
 *  End of checks for current train.
 *  Next update each of the engines and wagons in the train
 */
        for(w=t0->last; w!=NULL; w=w->prev){
          if(w == t0->last){
            dist_moved = del_t*t0->speed ;
          }else{
            dist_moved = (w->dist_back + w->next->dist_front)        ;
            copy_traveller_position(w->traveller,w->next->traveller) ;
          }
          iret = trv_move(w->traveller, dist_moved) ;
/*
 *  Update wheels on wagon
 */
          dist_moved = del_t*t0->speed ;
          if(w->train_dir){
            w->wheel_angle += degree*dist_moved*w->raw_wagon->inv_wheelradius ;
          }else{
            w->wheel_angle -= degree*dist_moved*w->raw_wagon->inv_wheelradius ;
          }
          while(w->wheel_angle > 360.0) w->wheel_angle -= 360.0 ;
          while(w->wheel_angle <   0.0) w->wheel_angle += 360.0 ;

          if(w->is_engine){
            if(w->train_dir){
              w->driverwheel_angle +=
                              degree*dist_moved*w->raw_wagon->inv_driverwheelradius ;
            }else{
              w->driverwheel_angle -=
                              degree*dist_moved*w->raw_wagon->inv_driverwheelradius ;
            }
            while(w->driverwheel_angle > 360.0) w->driverwheel_angle -= 360.0 ;
            while(w->driverwheel_angle <   0.0) w->driverwheel_angle += 360.0 ;
          }
        }  //  End loop over wagons
/*
 *  Update train's front and back travellers
 */
        copy_traveller_position(&(t0->front), t0->first->traveller) ;
        trv_move(&(t0->front), 0.5*t0->first->raw_wagon->length) ;
        copy_traveller_position(&(t0->back), t0->last->traveller) ;
        trv_move(&(t0->back), -0.5*t0->last->raw_wagon->length) ;
/*
 *  Current train in final position ;  Process any train collision.
 */
        if(i_collide != 0){
          printf("  TRAINS COLLIDE   Speed = %f\n", t0->speed) ;
          if(!t1){
            printf("  Routine %s error."
                   "  Train pointer t1 set to NULL after crash.\n",my_name) ;
          }
          if(fabs(t0->speed) < 2.0){               //  A bit high!
            join_trains(t0,t1,i_collide) ;
          }else{
            crash_trains(t0,t1,i_collide) ;
          }
        }else if(i_crash != 0){
          crash_train(t0,i_crash) ;
        }
      }    //  End loop over trains
/*
 *==============================================================================
 *  Second loop over trains and wagons
 *  Update variables which do not involve train movement using full del_t
 *==============================================================================
 */
      del_t = delta_seconds ;

      for(t0 = trainlist_beg; t0 != NULL ; t0=t0->next){
        for(w=t0->last; w!=NULL; w=w->prev){
/*
 *  Engine Wipers
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
 *  Engine Mirrors
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
 *  Engine Pantographs
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
/*
 *  End loop over trains
 */
      }
      time_last = time ;
      if(ip)printf("  Exit %s\n",my_name) ;
      return 0 ;
}

#if 0
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
#endif

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

