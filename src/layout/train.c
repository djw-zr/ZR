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
int position_train2(char *location, int iswap, double idirect) ;
int add_wagon_to_train2(char *wagon, RawWagonNode *rw0, int idirn) ;
int add_consist_to_train(char* consist_name) ;
int trains_setup_by_user(void) ;



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
#if   defined NO_ROUTE_DEF
      printf("***************************************\n") ;
      printf(" Call trains_setup_by_user\n") ;
      printf("***************************************\n") ;
      trains_setup_by_user() ;
#elif defined ROUTE_USA1

#if 1
      add_new_train("T01") ;
      add_wagon_to_train("AcelaHHL", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCAFE", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLBC", 1) ;
      position_train(779,7,1,8.0) ;     //  Station Platform
#endif

#if 1
      add_new_train("T02") ;
      add_wagon_to_train("AcelaHHL", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCAFE", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLBC", 1) ;
      position_train(37,4,0,0.0) ;     //  Station Platform
#endif

#if 1
      add_new_train("T02") ;
      add_wagon_to_train("AcelaHHL", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCAFE", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLBC", 1) ;
      position_train(700,2,0,0.0) ;     //  Siding
#endif


#if 1
      add_new_train("T03") ;
      add_wagon_to_train("AcelaHHL", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCAFE", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLCO", 1) ;
      add_wagon_to_train("AcelaHHLBC", 1) ;
//      position_train(1119,16,0,0.0) ;     //  Station Platform
      position_train(1146,8,0,10.0) ;     //  Station Platform
#endif


#elif defined ROUTE_USA2

#if 1
      add_new_train("T01") ;
      add_wagon_to_train("Dash9", 1) ;

      add_wagon_to_train("US2FullLoggerCar", 1) ;  //  Oil Tanker
      add_wagon_to_train("US2FullLoggerCar", 1) ;
      add_wagon_to_train("US2FullLoggerCar", 1) ;
      add_wagon_to_train("US2FullLoggerCar", 1) ;
      add_wagon_to_train("US2FullLoggerCar", 1) ;
      add_wagon_to_train("US2FullLoggerCar", 1) ;
      add_wagon_to_train("US2FullLoggerCar", 1) ;
      add_wagon_to_train("US2FullLoggerCar", 1) ;
      add_wagon_to_train("US2FullLoggerCar", 1) ;
      add_wagon_to_train("US2FullLoggerCar", 1) ;
      add_wagon_to_train("US2FullLoggerCar", 1) ;
      add_wagon_to_train("US2FullLoggerCar", 1) ;

      position_train(730,1,0,0.0) ;    // Shelby South
#endif

#if 1

      add_new_train("T02") ;
      add_wagon_to_train("Dash9", 1) ;
      add_wagon_to_train("US2Freightcar3", 1)  ;
      add_wagon_to_train("US2Freightcar3", 1)  ;   //  Oil ??
      add_wagon_to_train("US2EmpLoggerCar", 1) ;   //  Empty logs
      add_wagon_to_train("US2Freightcar4", 1)  ;   //  Trailer on wagon
      add_wagon_to_train("US2BNSFCar", 1)      ;   //  Coal ??
      add_wagon_to_train("US2Freight6", 1)     ;   //  Large green goods
      add_wagon_to_train("US2Freightcar3", 1)  ;   //
      add_wagon_to_train("US2Freight7", 1)     ;   //  Red Gooods - Santa Fe
      add_wagon_to_train("US2Freight8", 1)     ;   //  Large green - Burlington and Northern
      add_wagon_to_train("US2FCarYF2", 1)      ;   //  Large yellow open-sided (closed)
      add_wagon_to_train("US2DbleHgtFrghtRd", 1) ; //  Double stacked red containers
      add_wagon_to_train("US2ChemiCar", 1)     ;   //  Small chemicak car
      add_wagon_to_train("US2FreightcarYel1", 1) ; //  Large yellow open sided (?) goods
      add_wagon_to_train("US2FCarRE2", 1)      ;   //  Empty red open-sided wagon
      add_wagon_to_train("US2FullLoggerCar", 1) ;  //  Full of logs
      add_wagon_to_train("US2WoodChipper", 1)  ;   //  Green open top - Burlington and Northern
      add_wagon_to_train("US2HopperCar", 1)    ;   //  Hopper car (grey)
      add_wagon_to_train("US2GrainCar", 1)     ;   //  Hopper car (brown) grain
      add_wagon_to_train("US2DbleHgtFrghtBl", 1) ; //  Double stacked blue containers
      add_wagon_to_train("US2FCarYE2", 1)      ;   //  Empty yellow open-sided wagon
      add_wagon_to_train("US2Freightcar5", 1)  ;   //  Empty flat freight car
      position_train(512,4,1,0.0) ;     //  Whitefish siding
//      position_train(541,1,1,0.0) ;     //  Whitefish siding
//      position_train(541,9,1,-30.0) ;     //  Whitefish siding near signals
//      position_train(485,15,1,0.0) ;     //  Whitefish approach
//      position_train(533,11,0,0.0) ;
//      position_train(467,7,0,0.0) ;
#endif

#if 1
      add_new_train("T02") ;
//      add_wagon_to_train("SD40", 1) ;  //  Shape errors ??
      add_wagon_to_train("Dash9", 1) ;

      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;
      add_wagon_to_train("US2Freightcar3", 1) ;

      position_train(431,3,0,0.0) ;    //  Columbia Falls Siding
#endif

#if 1
      add_new_train("T03") ;
      add_wagon_to_train("GP38", 1) ;

      add_wagon_to_train("US2Freight6", 1) ;
      add_wagon_to_train("US2Freight6", 1) ;
      add_wagon_to_train("US2Freight6", 1) ;
      add_wagon_to_train("US2Freight6", 1) ;
      add_wagon_to_train("US2Freight6", 1) ;
      add_wagon_to_train("US2Freight6", 1) ;
      add_wagon_to_train("US2Freight6", 1) ;
      add_wagon_to_train("US2Freight6", 1) ;

      position_train(694,1,0,0.0) ;   //  Kalispell
#endif

#if 1
      add_new_train("T04") ;
      add_wagon_to_train("GP38", 1) ;

      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;
      add_wagon_to_train("US2Freightcar4", 1) ;

      position_train(125,1,1,0.0) ;   // Cutback
#endif

#if 1
      add_new_train("T06") ;
      add_wagon_to_train("Dash9", 1) ;

      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;
      add_wagon_to_train("US2Freight8", 1) ;

      position_train(191,5,1,0.0) ;  // Glacier Park
#endif

#if 1
      add_new_train("T07") ;
//      add_wagon_to_train("SD40", 1) ;
      add_wagon_to_train("Dash9", 1) ;

      position_train(205,2,0,0.0) ;  //  Maria - Baloon
#endif

#if 1
      add_new_train("T08") ;
//      add_wagon_to_train("SD40", 1) ;
      add_wagon_to_train("Dash9", 1) ;
      position_train(449,4,0,0.0) ;  //  Crossing with rising arms
#endif



#elif defined ROUTE_EUROPE1

      add_new_train("T01") ;
      add_wagon_to_train("RoyalScotsClass", 1) ;
      add_wagon_to_train("RoyalScotTender", 1) ;
      add_wagon_to_train("ScotsMailCar", 1) ;
      add_wagon_to_train("ScotsFirstClass", 1) ;
      add_wagon_to_train("ScotsFirstClass", 1) ;
      add_wagon_to_train("ScotsFirstClass", 1) ;
      add_wagon_to_train("ScotsFirstClass", 1) ;
      add_wagon_to_train("ScotsThirdClass", 1) ;
      add_wagon_to_train("ScotsThirdClass", 1) ;
      add_wagon_to_train("ScotsThirdClass", 1) ;
      add_wagon_to_train("ScotsThirdClass", 1) ;
      add_wagon_to_train("ScotsThirdClass", 1) ;
      add_wagon_to_train("ScotsThirdClass", 1) ;
      add_wagon_to_train("ScotsThirdClass", 1) ;
      add_wagon_to_train("ScotsThirdClass", 1) ;
      add_wagon_to_train("ScotsThirdClass", 1) ;

      position_train(325,9,0,0.0) ;

      add_new_train("T02") ;
      add_wagon_to_train("Pendennis", 1) ;
      add_wagon_to_train("PendennisTender", 1) ;
      add_wagon_to_train("GWRFirst", 1) ;
      add_wagon_to_train("GWRFirst", 1) ;
      add_wagon_to_train("GWRFirst", 1) ;
      add_wagon_to_train("GWRFirst", 1) ;
      add_wagon_to_train("GWRRest", 1) ;
      add_wagon_to_train("GWRRest", 1) ;
      add_wagon_to_train("GWRThird", 1) ;
      add_wagon_to_train("GWRThird", 1) ;
      add_wagon_to_train("GWRThird", 1) ;
      add_wagon_to_train("GWRThird", 1) ;
      add_wagon_to_train("GWRThird", 1) ;
      add_wagon_to_train("GWRThird", 1) ;
      add_wagon_to_train("GWRThird", 1) ;
      add_wagon_to_train("GWRThird", 1) ;
      add_wagon_to_train("GWRLuggCar", 1) ;
      add_wagon_to_train("GWRLuggCar", 1) ;

      position_train(329,10,0,30.0) ;


      add_new_train("T02") ;
      add_wagon_to_train("Scotsman", 1) ;
      add_wagon_to_train("ScotsTender", 1) ;
      add_wagon_to_train("ScotsCar1", 1) ;
      add_wagon_to_train("ScotsCar1", 1) ;
      add_wagon_to_train("ScotsCar1", 1) ;
      add_wagon_to_train("ScotsCar3", 1) ;
      add_wagon_to_train("ScotsCar3", 1) ;
      add_wagon_to_train("ScotsCar3", 1) ;
      add_wagon_to_train("ScotsCar3", 1) ;
      add_wagon_to_train("ScotsCar3", 1) ;

      position_train(384,17,0,10.0) ;


#elif defined ROUTE_EUROPE2

      add_new_train("T01") ;
      add_wagon_to_train("310", 1) ;
      add_wagon_to_train("310Tender", 1) ;
      add_wagon_to_train("OESleepCar1", 1) ;
      add_wagon_to_train("OESleepCar2", 1) ;
      add_wagon_to_train("OESleepCar3", 1) ;
      add_wagon_to_train("OESleepCar4", 1) ;
      add_wagon_to_train("OESleepCar5", 1) ;
      add_wagon_to_train("OESleepCar6", 1) ;
      add_wagon_to_train("OESleepCar7", 1) ;
      add_wagon_to_train("OESleepCar8", 1) ;
      add_wagon_to_train("OESleepCar9", 1) ;

      position_train(29,7,1,0.0) ;


#elif defined ROUTE_JAPAN1

      add_new_train("T01") ;
      add_wagon_to_train("KIHA31a", 1) ;
      add_wagon_to_train("KIHA31", 1) ;
      position_train(746,5,0,0.0) ;

      add_new_train("T02") ;
      add_wagon_to_train("KIHA31a", 1) ;
      add_wagon_to_train("KIHA31", 1) ;
      position_train(32,10,0,0.0) ;

#elif defined ROUTE_JAPAN2
      add_new_train("T01") ;
      add_wagon_to_train("KIHA31a", 1) ;
      add_wagon_to_train("KIHA31", 1) ;

      position_train(314,1,0,0.0) ;
#if 1
      add_new_train("T02") ;
      add_wagon_to_train("30000", 1) ;
      add_wagon_to_train("30000", 0) ;
//      add_wagon_to_train("30000", 1) ;
      position_train(282,4,0,0.0) ;

      add_new_train("T02") ;
      add_wagon_to_train("KIHA140", 1) ;
      position_train(309,3,0,0.0) ;

      add_new_train("T03") ;
      add_wagon_to_train("2000", 1) ;
      add_wagon_to_train("2000RearEng", 1) ;
      position_train(255,6,0,0.0) ;

      add_new_train("T04") ;
      add_wagon_to_train("2000", 1) ;
      add_wagon_to_train("2000RearEng", 1) ;
      position_train(266,6,0,35.0) ;


      add_new_train("T06") ;
      add_wagon_to_train("2000", 1) ;
      add_wagon_to_train("2000RearEng", 1) ;
//      add_wagon_to_train("Genesis", 1) ;     //  Amtrack
//      add_wagon_to_train("Series7000Carg", 1) ;
//      add_wagon_to_train("2000ACarriage", 1) ;

      position_train(310,9,0,0.0) ;

      add_new_train("T07") ;
      add_wagon_to_train("2000", 1) ;
      add_wagon_to_train("2000RearEng", 1) ;
//      position_train(62,183,0,0.0) ;
      position_train(11,3,0,0.0) ;

#endif

#elif defined ROUTE_TUTORIAL

      add_new_train("T01") ;
      add_wagon_to_train("Dash9", 1) ;
      position_train(13,2,0,0.0) ;


#elif defined ROUTE_AU_NSW_SW_SS

#if 1
      add_new_train("T01") ;
      add_wagon_to_train("AU_NSW_3028_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3028_tender", 1) ;
      for(i=0;i<30;i++){
        add_wagon_to_train("AU_NSW_Dtruck2_Coal", 1) ;
      }
      add_wagon_to_train("LHG_Brake_Van", 1) ;
      position_train(730,1,1,1.0) ;    // Near Lithgow
#endif
#if 1
      add_new_train("T02") ;
      add_wagon_to_train("AU_NSW_3028_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3028_tender", 1) ;
      position_train(659,7,0,0.0) ;    // Near Mount Victoria

      add_new_train("T02") ;
      add_wagon_to_train("AU_NSW_3028_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3028_tender", 1) ;
      position_train(4321,1,0,1.0) ;    // Near Clyde

      add_new_train("T03") ;
      add_wagon_to_train("AU_NSW_3028_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3028_tender", 1) ;
      position_train(5076,2,1,0.0) ;    //  Redfern

      add_new_train("T04") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(5911,2,0,0.0) ;    // Sydney

      add_new_train("T06") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(1023,2,0,0.0) ;    // Works

      add_new_train("T07") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(1611,2,1,0.0) ;    // Rooty Hill

      add_new_train("T08") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(3262,2,0,0.0) ;    // Cambelltown

      add_new_train("T09") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(760,3,1,0.0) ;    // Lithgow Loop

      add_new_train("T10") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(3161,1,1,0.0) ;    //  Richmond

      add_new_train("T11") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(1701,1,0,0.0) ;    //  Blacktown

      add_new_train("T12") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(2250,3,1,0.0) ;    //  Clyburn

      add_new_train("T13") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(2747,5,0,0.0) ;    //  Chester Hill

      add_new_train("T14") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(5062,3,1,0.0) ;    //  Sydney Raiway Sidings

      add_new_train("T15") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(7706,2,0,0.0) ;    //  Back of beyond /Town no name
                                              // 1423 10258

      add_new_train("T16") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(8051,3,0,0.0) ;    //  Yennora

      add_new_train("T18") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(7495,5,0,0.0) ;    //  Goulburn

      add_new_train("T19") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(7035,3,0,0.0) ;    //  Moss Vale

      add_new_train("T20") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(6683,3,1,0.0) ;    //  Picton

      add_new_train("T21") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(3244,4,0,0.0) ;    //  Cambelltown

      add_new_train("T22") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(4668,3,0,0.0) ;    //  Line to Newcastle

      add_new_train("T23") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(1122,5,1,0.0) ;    //  Mudgee / Wallerwang

      add_new_train("T24") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(1290,2,0,0.0) ;    //  North-west Collery

      add_new_train("T25") ;
      add_wagon_to_train("AU_NSW_3102_C30T", 1) ;
      add_wagon_to_train("AU_NSW_3102_tender", 1) ;
      position_train(1038,2,1,0.0) ;    //  Bowenfeld

#endif

#elif defined ROUTE_NEW_FOREST
#if 0
      add_new_train("T01") ;
      add_wagon_to_train("LJ_ExGW_Railcar", 1) ;
      add_wagon_to_train("35T_ClassB_Tank_Esso_E", 1) ;
      add_wagon_to_train("35T_ClassB_Tank_Esso_Sign_E", 1) ;
      add_wagon_to_train("35T_ClassB_Tank_Esso_Worn_E", 1) ;
      add_wagon_to_train("35T_ClassB_Tank_Esso_L", 1) ;
      add_wagon_to_train("35T_ClassB_Tank_Esso_Sign_L", 1) ;
      add_wagon_to_train("35T_ClassB_Tank_Esso_Worn_L", 1) ;
      add_wagon_to_train("AAC_DEE_MNR_35001", 1) ;
      add_wagon_to_train("AAC_DEE_MNR_35001T", 1) ;
      add_wagon_to_train("AAC_DEE_MNR_35001 discs18", 1) ;
      add_wagon_to_train("AAC_DEE_MNR_35001T", 1) ;
      add_wagon_to_train("BR_BTP_41", 1) ;
      add_wagon_to_train("BR_BTP_41_T", 1) ;
      add_wagon_to_train("BR_BTP_95", 1) ;
      add_wagon_to_train("BR_BTP_95_T", 1) ;
#if 0
      add_wagon_to_train("DR_BR_5MT_44917", 1) ;   //  Textures missing ??
      add_wagon_to_train("DR_BR_5MT_44917T", 1) ;
      add_wagon_to_train("DR_BR_5MT_45440", 1) ;
      add_wagon_to_train("RF_BR_3FT_47190", 1) ;
      add_wagon_to_train("RF_BR_3FT_47191", 1) ;
#endif
      position_train2("MiddleRoad1", 1, 10.0) ;

      add_new_train("T01a") ;
      add_wagon_to_train("AAC_DEE_MNR_35001 discs18", 1) ;
      add_wagon_to_train("AAC_DEE_MNR_35001T", 1) ;
      add_wagon_to_train("BR_BTP_41", 1) ;
      add_wagon_to_train("BR_BTP_95", 1) ;
      add_wagon_to_train("BR_BTP_95_T", 1) ;
      add_wagon_to_train("BR_BTP_41_T", 1) ;
      add_wagon_to_train("BR_BTP_41", 1) ;
      add_wagon_to_train("BR_BTP_95", 1) ;
      add_wagon_to_train("BR_BTP_95_T", 1) ;
      add_wagon_to_train("BR_BTP_41_T", 1) ;
      position_train2("Weymouth3", 1, 10.0) ;
#endif
#if 0
      add_new_train("T01b") ;
      add_wagon_to_train("DEE_D6541_VB", 1) ;
//      add_wagon_to_train("AAC_DEE_MNR_35001 discs18", 1) ;
//      add_wagon_to_train("AAC_DEE_MNR_35001T", 1) ;
#if 0
      add_wagon_to_train("BR_BTP_41", 1) ;
      add_wagon_to_train("BR_BTP_95", 1) ;
      add_wagon_to_train("BR_BTP_95_T", 1) ;
      add_wagon_to_train("BR_BTP_41_T", 1) ;
      add_wagon_to_train("BR_BTP_41", 1) ;
      add_wagon_to_train("BR_BTP_95", 1) ;
      add_wagon_to_train("BR_BTP_95_T", 1) ;
      add_wagon_to_train("BR_BTP_41_T", 1) ;
#endif
      position_train2("Weymouth4", 1, 150.0) ;
//      position_train(4920,5,0,0.0) ;    //  Weymouth arrival
#endif
#if 0
      add_new_train("T02") ;
      add_wagon_to_train("LJ_ExGW_Railcar", 1) ;
      add_consist_to_train("DR_BR_4MT_76007_LE") ;
      add_consist_to_train("LR_Goods") ;
      position_train2("DorchupSiding", 1, 10.0) ;

      add_new_train("T03") ;
      add_consist_to_train("CCW_BR_Q_30548_LC_LE") ;
      position_train2("WarehamDownBay", 1, 10.0) ;

      add_new_train("T03a") ;
      add_consist_to_train("BR 3F 43278 Light") ;
      add_consist_to_train("QuayGds") ;
      position_train2("CorfeCastleDn", 1, 10.0) ;

      add_new_train("T03b") ;
      add_wagon_to_train("AAC_DEE_MNR_35001 discs18", 1) ;
      add_wagon_to_train("AAC_DEE_MNR_35001T", 1) ;
      position_train2("SwanageMain", 0, 0.0) ;

      add_new_train("T04") ;
      add_wagon_to_train("LJ_ExGW_Railcar", 1) ;
      position_train2("PooleQuay East", 1, 10.0) ;

      add_new_train("T05") ;
      add_consist_to_train("80002") ;
      position_train2("BrkYd1", 1, 10.0) ;
#endif
      add_new_train("T06") ;
      add_wagon_to_train("LJ_ExGW_Railcar", 1) ;
//      add_consist_to_train("RL_540WlooWmth70A2WC") ;
    position_train2("Lymington Pier", 1, 10.0) ;
//      position_train(27,16,0,0.0) ;    //  Lymington entrance
#if 0
      add_new_train("T07") ;
      add_consist_to_train("RL_51xxLE") ;
      position_train2("Berths38_41", 1, 10.0) ;

      add_new_train("T08") ;
      add_consist_to_train("RL_4MT_76031D09") ;
      position_train2("RomseyYard1", 1, 10.0) ;

      add_new_train("T09") ;
      add_consist_to_train("RL_4MT_760069D17") ;
      position_train2("ElghUpLoop", 1, 10.0) ;

      add_new_train("T10") ;
      add_consist_to_train("RL_4F44146LE") ;
      position_train2("Bstoke Down carriage", 1, 10.0) ;

      add_new_train("T11") ;
      add_consist_to_train("RL_44411LE") ;
      position_train2("FC1", 1, 10.0) ;

      add_new_train("T12") ;
      add_consist_to_train("RL_43xxgds") ;
      position_train2("MidsomerNortonGds1", 1, 10.0) ;

      add_new_train("T13") ;
      add_consist_to_train("RL_3MT_82014D04") ;
      position_train2("Templecombe Up siding", 1, 10.0) ;

      add_new_train("T14") ;
      add_consist_to_train("RL_std573082") ;
      add_consist_to_train("RL_2323PmthSSYeovil72C482U") ;
      position_train2("EvercreechJct Up", 1, 10.0) ;

      add_new_train("T15") ;
      add_consist_to_train("RL_34046LE") ;
      position_train2("Bath jct", 1, 10.0) ;

//      add_new_train("T16") ;
//      add_wagon_to_train("LJ_ExGW_Railcar", 1) ;
//      position_train2("EvercreechJct", 1, 10.0) ;

#endif
/*
 *  ZIG ZAG Route
 */
#else

      add_new_train("T01") ;
      add_wagon_to_train("1905-I103", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Coal", 1) ;
      position_train2("Goods Siding", 1, 10.0) ;

#if 1
      add_new_train("T02") ;

      add_wagon_to_train("4W-LV-load", 1) ;
      add_wagon_to_train("4W-LV-mty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Ash", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Ballast", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Coal", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_FlatSheet", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_HumpSheet", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck2_Coal", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck2_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck2_FlatSheet", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck2_HumpSheet", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Ballast", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Coal", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_FlatSheet", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_HumpSheet", 1) ;

      add_wagon_to_train("4W-HG-Brake-Van", 1) ;

      position_train2("No 2 Loop Siding", 0, 30.0) ;
//      position_train(408,3,0,0.0) ;     //  Station Bowenfels

//      add_wagon_to_train("4W-HG-Brake-Van", 1) ;
//      position_train(376,1,1,10.0) ;     //  Station Bowenfels
//      position_train(373,1,1,0.0) ;     //  Station Bowenfels
//      position_train(373,1,0,0.0) ;     //  Train Reversed
//      position_train(357,3,0,0.0) ;
//      position_train(384,1,1,0.0) ;
//      position_train(356,33,0,0.0) ;  //  Near Bowenfels Level Crossing 1

      add_new_train("T03") ;
#if 1
      add_consist_to_train("zz_w17_pickup_goods") ;
#else
      add_wagon_to_train("1905-A115", 1) ;
      add_wagon_to_train("1905-A115-t", 1) ;
      add_wagon_to_train("1905-J484", 1) ;
      add_wagon_to_train("1905-J484-t", 1) ;
      add_wagon_to_train("1905-AD-Sleeper", 1) ;
      add_wagon_to_train("1905-AD-Sleeper", 1) ;
      add_wagon_to_train("1905-AC-StateCar", 1) ;
      add_wagon_to_train("1905-AB-Diner", 1) ;
      add_wagon_to_train("1905-CBC", 1) ;
      add_wagon_to_train("1905-CBC", 1) ;
      add_wagon_to_train("1905-CBC", 1) ;
      add_wagon_to_train("1905-HO-JWW", 1) ;
#endif
      position_train(356,1,1,0.0) ;   // Bowenfels End-of-Line
//      position_train(384,1,1,0.0) ;
//      position_train(415,1,0,0.0) ;

      add_new_train("T04") ;

      add_wagon_to_train("1905-O449", 1) ;
      add_wagon_to_train("1905-O449-t", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("4W-HG-Brake-Van", 1) ;

      position_train2("Hermitage Colliery - Arrival Road", 1, 30.0) ;
//      position_train(431,4,0,70.0) ;

      add_new_train("T05") ;

      add_wagon_to_train("1905-B319", 1) ;
      add_wagon_to_train("1905-B319-t", 1) ;
      add_wagon_to_train("1905-P708", 1) ;
      add_wagon_to_train("1905-P708-t", 1) ;

      add_wagon_to_train("JWW_Htype_E", 1) ;
      add_wagon_to_train("JWW_Htype_L", 1) ;
      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("4W-CV-load", 1) ;
      add_wagon_to_train("4W-CV-load", 1) ;
      add_wagon_to_train("4W-CV-load", 1) ;
      add_wagon_to_train("4W-CV-load", 1) ;
      add_wagon_to_train("4W-CV-load", 1) ;
      add_wagon_to_train("4W-CV-mty", 1) ;
      add_wagon_to_train("4W-CW2-LD", 1) ;
      add_wagon_to_train("4W-CW2", 1) ;
      add_wagon_to_train("4W-GSV2-Pigs", 1) ;
      add_wagon_to_train("4W-GSV2-Sheep", 1) ;
      add_wagon_to_train("4W-GSV2", 1) ;
      add_wagon_to_train("4W-GSV2", 1) ;
      add_wagon_to_train("4W-LV-load", 1) ;
      add_wagon_to_train("4W-LV-mty", 1) ;

      position_train2("No 2 Short Deadend", 1, 60.0) ;
//      position_train(242,2,0,0.0) ;

      add_new_train("T06") ;

      add_wagon_to_train("1905-D347", 1) ;
      add_wagon_to_train("1905-D347-t", 1) ;
      add_wagon_to_train("4W-CV-load", 0) ;
      add_wagon_to_train("4W-CV-mty", 1) ;

      add_wagon_to_train("AU_NSW_Dtruck1_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck2_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Empty", 1) ;

      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("NI4wHopUnb4MT", 1) ;
      add_wagon_to_train("NI4wHopUnb4MT", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_FlatSheet", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_HumpSheet", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Coal", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Coal", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck3_Ballast", 1) ;

      add_wagon_to_train("JWW_Htype_E", 1) ;
      add_wagon_to_train("JWW_Htype_L", 1) ;
      add_wagon_to_train("JWW_Htype_L", 1) ;
      add_wagon_to_train("JWW_Htype_E", 1) ;
      add_wagon_to_train("4W-HG-Brake-Van", 1) ;

//      position_train2("No 2 Loop Siding", 0, 60.0) ;
      position_train(189,5,0,0.0) ;

//      position_train(376,1,1,0.0) ;
//      position_train(123,1,1,0.0) ;
//      position_train(377,6,0,0.0) ;
//        position_train(554,4,0,-10.0) ;  // Mt Victoria Turntable
//        position_train(251,3,0,0.0) ;  // Golburn - Train shed line 3
//        position_train(541,5,0,0.0) ;  // Mt Victoria
//        position_train(356,13,1,10.0) ;  // Bowenfels distant
//        position_train(356,33,1,10.0) ;  // Bowenfels level crossing
//        position_train(392,1,0,20.0) ;  // Default Bowenfels siding
//      position_train(363,1,1,0.0) ;

      add_new_train("T07") ;

      add_wagon_to_train("1905-S654", 1) ;
      add_wagon_to_train("1905-C144", 1) ;
      add_wagon_to_train("1905-C144-t", 1) ;
      add_wagon_to_train("4W-CV-load", 1) ;
      add_wagon_to_train("1905-C144", 1) ;
      add_wagon_to_train("1905-C144-t", 1) ;
      add_wagon_to_train("4W-CV-load", 1) ;
      add_wagon_to_train("4W-CV-load", 1) ;
      add_wagon_to_train("NI4wHopUnb1LD", 1) ;
      add_wagon_to_train("NI4wHopUnb1MT", 1) ;
      add_wagon_to_train("NI4wHopUnb2LD", 1) ;
      add_wagon_to_train("NI4wHopUnb2MT", 1) ;
      add_wagon_to_train("NI4wHopUnb3LD", 1) ;
      add_wagon_to_train("NI4wHopUnb3MT", 1) ;
      add_wagon_to_train("NI4wHopUnb4LD", 1) ;
      add_wagon_to_train("NI4wHopUnb4MT", 1) ;

/*
 *  These wagons cause a segmentation fault
 */
//      add_wagon_to_train("WVR-4W-AcidTank-9-LD", 1) ;
//      add_wagon_to_train("WVR-4W-AcidTank-9", 1) ;

//      position_train(51,12,0,0.0) ;
      position_train2("Zig Zag - Top Points - Engine Road", 1, 0.0) ;

      add_new_train("T08") ;

      add_wagon_to_train("1905-T524", 1) ;
      add_wagon_to_train("1905-T524-t", 1) ;
      add_wagon_to_train("4W-CV-load", 1) ;
      add_wagon_to_train("1905-AD-Sleeper", 1) ;
      add_wagon_to_train("1905-AB-Diner", 1) ;
      add_wagon_to_train("1905-CBC", 1) ;
      add_wagon_to_train("1905-CBC", 1) ;
      add_wagon_to_train("1905-CBC", 1) ;
      add_wagon_to_train("1905-HO-JWW", 1) ;

//      position_train(485,5,0,0.0) ;
      position_train2("Mt Victoria - No 1", 1, 20.0) ;

      add_new_train("T09") ;

      add_wagon_to_train("1905-S654", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck2_FlatSheet", 1) ;

      position_train2("No 2 Turntable Road", 0, 30.0) ;
//      position_train(269,10,0,0.0) ;

//  Wagons in Sidings

      add_new_train("T10") ;
      add_wagon_to_train("4W-CV-load", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Empty", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Empty", 1) ;
      position_train2("No 1 Ash Siding", 1, 10.0)  ;
      position_train(213,3,1,10.0) ;

      add_new_train("T11") ;
      add_wagon_to_train("AU_NSW_Dtruck1_Ballast", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Ballast", 1) ;
      position_train2("No 2 Repair Siding", 1, 20.0)  ;
//      position_train(271,2,0,0.0) ;

//  Engines in shed

      add_new_train("T12") ;
      add_wagon_to_train("1905-T524", 1) ;
      add_wagon_to_train("1905-T524-t", 1) ;
      position_train2("No 1 Loco Road", 1, 20.0) ;
//      position_train(251,5,0,20.0) ;

      add_new_train("T13") ;
      add_wagon_to_train("1905-C144", 1) ;
      add_wagon_to_train("1905-C144-t", 1) ;
      position_train2("No 2 Loco Road", 0, 50.0) ;
//      position_train(258,4,1,5.0) ;


//  Trucks only in Bowenfels Siding

      add_new_train("T14") ;
      add_wagon_to_train("AU_NSW_Dtruck1_Ballast", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Ballast", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Ballast", 1) ;
      add_wagon_to_train("AU_NSW_Dtruck1_Empty", 1) ;
      position_train2("Goods Siding", 1, -70.0) ;
//      position_train(392,1,1,30.0) ;     //  Station Bowenfels Siding
#endif

#if 1
//  Tank engine on Mt Victoria Turntable

      add_new_train("T14") ;
      add_wagon_to_train("1905-S654", 1) ;
//      position_train2("Turntable Road", 1, -22.0) ;
//      position_train(554,4,0,-10.0) ;  // Mt Victoria Turntable
      position_train(538,1,0,0.0) ;  // Mt Victoria Entrance
#endif

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
        printf("   Wagon names are:\n") ;
        for(raw = rawwagonlist_beg; raw!=NULL; raw = raw->next){
            printf("   Name = %s :: %s\n",raw->name,raw->full_name) ;
        }
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

int   add_wagon_to_train(char *wagon, int idirn){

  int  iret ;

       iret = add_wagon_to_train2(wagon, NULL, idirn) ;

       return iret ;
}

int   add_wagon_to_train2(char *wagon0, RawWagonNode *rw0, int idirn){

  int           ip = 0     ;  //  Debug
  TrainNode     *t  = NULL ;
  WagonNode     *w  = NULL ;
  RawWagonNode  *rw = rw0  ;
  TravellerNode *z1 = NULL ;
  char          *wagon = wagon0 ;
  char         *my_name = "add_wagon_to_train2" ;

//      ip = ip && (rw0 != NULL) ;
      if(ip)printf("  Routine %s\n",my_name) ;

      t = build_train ;               //  Always use default
      if(!t) return 0 ;               //  No build train defined
/*
 *==============================================================================
 *  If rw0 is not set, find wagon in list of raw wagons
 *  First search through names - then full names
 *==============================================================================
 */

      if(rw == NULL){
        for(rw = rawwagonlist_beg; rw != NULL; rw = rw->next){
          if(ip)printf(" Routine %s, needed wagon = %s, wagon name = %s :: %s\n",
                      my_name, wagon,
                      rw->name, rw->shape->name) ;
          if(!strcmp_ic(rw->name,wagon)) break ;
        }
        if(rw == NULL){
          for(rw = rawwagonlist_beg; rw != NULL; rw = rw->next){
          if(ip)printf(" Routine %s, needed wagon = %s, full wagon name = %s :: %s\n",
                      my_name, wagon,
                      rw->full_name, rw->shape->name) ;
            if(!strcmp_ic(rw->full_name,wagon)) break ;
          }
        }
        if(rw==NULL){
          printf(" Routine %s.\n  Unable to find wagon with name %s\n",
                my_name,wagon) ;
          printf("   Wagon names are:\n") ;
          for(rw = rawwagonlist_beg; rw!=NULL; rw = rw->next){
            printf("   Name = %s :: %s\n",rw->name,rw->full_name) ;
          }
          exit(1) ;
        }
      }else{
        wagon = rw->name ;
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

/*
 *   Add consist to the current train
 */

int add_consist_to_train(char* consist_name){

  int           ip = 0   ;
  ConsistNode   *consist ;
  DLPointerNode *pp_node ;
  RawWagonNode  *raw_wagon ;

  char *my_name = "add_consist_to_train" ;

      if(ip){
        printf("  Enter routine '%s'\n", my_name) ;
        printf("    Consist_name = %s\n",consist_name) ;
      }

      for(consist = consistlist_beg; consist != NULL ; consist = consist->next){
        if(strcmp(consist_name,consist->name) == 0) break ;
      }

      if(!consist){
        printf("  Routine '%s' error.\n", my_name) ;
        printf("  Unable to find consist.  Name = '%s'\n",consist_name) ;
        printf("    Available consists:\n") ;
        for(consist = consistlist_beg; consist != NULL ; consist = consist->next){
          printf("      Consist = '%s'\n",consist->name) ;
        }
        return 1 ;
      }

      for(pp_node = consist->first; pp_node != NULL; pp_node = pp_node->next){
        raw_wagon = (RawWagonNode *)pp_node->pointer ;
        add_wagon_to_train2( NULL, raw_wagon, 1) ;
      }
      return 0 ;
}

int position_train(int itrack, int ivector, int idirect, double distance){

  int           ip = 0     ;  // Debug
  int           n          ;
  TrainNode     *t  = NULL ;
  WagonNode     *w  = NULL ;
  TravellerNode *z1 = NULL ;
  double       dist = distance ;
  char         *my_name = "position_train" ;

      if(ip)printf("  Routine %s\n",my_name) ;

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
        if(ip){
          printf("  Traveller position for n = %i\n",n) ;
          trv_position(z1) ;
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
 *  Routine to place the default train at a given platform or siding
 *  location.  The default is to place the back of train at the
 *  "ffff0000" end of the platform or siding.
 *
 *  l_dir = 1 :  back of the train at the "ffff0000" track item
 *  distance  :  forward movement of train from default position
 */

int position_train2(char *location, int l_dir, double distance){

  int           ip = 0     ;  //  Debug
  uint          i, k, n    ;
  uint          itrack     ;  //  Track section uid
  uint          l_trk      ;
  double        dist       ;
  TrainNode     *t  = build_train ;
  WagonNode     *w  = NULL ;
  TravellerNode *z0 = NULL ;
  TravellerNode *z1 = NULL ;
  TrkItem       *ti, *ti0 = NULL, *ti1 = NULL ;
  TrkSector     *ts0, *ts1 ;
  char         *my_name = "position_train2" ;

      if(ip)printf("  Enter routine %s.\n",my_name) ;

/*
 *  Loop over track items
 */
      k = 0 ;
      for(i=0; i<track_db.trk_items_array_size; i++){
        ti = &(track_db.trk_items_array[i]) ;
        if(ti->type_of_node == PLATFORM){
          if(!strcmp_nq(location,ti->platform_name)){
            k = ti->platform_data2 ;
            if(ti->platform_data1[0]=='f'){
              ti1 = ti;
              ti0 = &(track_db.trk_items_array[k]) ;
            }else{
              ti0 = ti;
              ti1 = &(track_db.trk_items_array[k]) ;
            }
            break ;
          }
        }else if(ti->type_of_node == SIDING){
          if(!strcmp_nq(location,ti->siding_name)){
            k = ti->siding_data2 ;
            if(ti->siding_data1[0]=='f'){
              ti1 = ti;
              ti0 = &(track_db.trk_items_array[k]) ;
            }else{
              ti0 = ti;
              ti1 = &(track_db.trk_items_array[k]) ;
            }
            break ;
          }
        }
      }
      if(!ti0 || !ti1){
        printf("  Routine %s error.\n",my_name) ;
        printf("  Unable to find either or both ends of platform"
                " or siding '%s'.\n",location) ;
        k = sqrt(-1.0) ;
        close_system() ;
      }
/*
 *  At this stage
 *   ti0 is at leaving end of platform or siding
 *   ti1 is at buffer or trainling end
 */
      if(ip){
        printf("  location = %s :: %i %i\n",location,i,k) ;
        printf("  Track Item ti0 = %p :: Item type %i %s :: Item uid = %i : data1 = %s\n",
                              (void *)ti0,  ti0->type_of_node,
                              token_trackdb[ti0->type_of_node],
                              ti0->uid, ti0->siding_data1) ;
        printf("  Track Item ti1 = %p :: Item type %i %s :: Item uid = %i : data1 = %s\n",
                              (void *)ti1,  ti1->type_of_node,
                              token_trackdb[ti1->type_of_node],
                              ti1->uid, ti1->siding_data1) ;
        printf("  Track Section : ts0 index = %i, ts1 index = %i\n",
               ti0->track_section, ti1->track_section) ;
        ts0 = &track_db.trk_sections_array[ti0->track_section]   ;
        ts1 = &track_db.trk_sections_array[ti1->track_section]   ;
        printf("  Track Sector ts0 : %p : uid %i, type = %i %s\n",
        (void *)ts0, ts0->uid, ts0->type_of_node, token_trackdb[ts0->type_of_node]) ;
        printf("  Track Sector ts1 : %p : uid %i, type = %i %s\n",
        (void *)ts1, ts1->uid, ts0->type_of_node, token_trackdb[ts0->type_of_node]) ;
      }
/*
 *  Check the two track items are on the same track section
 */
      itrack = ti0->track_section ;
      if(itrack != ti1->track_section){
        printf("  Routine %s error.\n",my_name) ;
        printf("  Routine needs both limits of the Platform or Siding"
                " to lie in the same track section.\n") ;
        printf("  Start and end limits are in sections %i and %i\n",
                  itrack,ti1->track_section) ;
        k = sqrt(-1.0) ;
        close_system() ;
      }
/*
 *  Position of end of train
 */
      l_trk = (ti0->sect_distance - ti1->sect_distance) >= 0.0    ;
      dist  = (l_dir ? ti1->sect_distance : ti0->sect_distance)  ;

      if(ip){
        printf("  location = %s, l_dir = %i, distance = %f\n",
                  location, l_dir, distance) ;
        printf("  section distances = %f :: %f\n",
                  ti0->sect_distance, ti1->sect_distance) ;
        printf("  itrack+1 = %i, l_trk = %i, dist = %f\n",itrack+1,l_trk,dist) ;
      }
/*
 *  Loop over wagons
 */
      for(w = t->last, n=0 ; w != NULL ; w=w->prev, n++){
        if(ip)printf("  Routine %s :: train = %s, dist = %f, wagon = %s\n",
                                      my_name,t->name,dist,w->name) ;
        z0 = w->traveller ;
        if(w == t->last){
          trv_initv(z0, itrack+1, 0, 1) ;
          trv_move(z0, dist)  ;
          if(l_trk){
            z0->idirect = l_dir ;
          }else{
            z0->idirect = !l_dir ;
          }
          trv_move(z0, distance)  ;
        }else{
          copy_traveller_position(z0, z1) ;
          dist = w->dist_back + w->next->dist_front ;
          trv_move(z0, dist) ;
        }
        z1 = z0 ;
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
  int       ipp = 0 ;

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


      ipp = ip && i_zrt ;
      if(ipp){
        printf("\n  Enter %s.  Run seconds = %f :: %i %i %i\n",my_name,run_seconds,
               ip,i_zrt,ipp) ;
//        print_train_data() ;
      }
      time = run_seconds ;
/*
 *==============================================================================
 *  Loop over trains
 *==============================================================================
 */
      for(t0 = trainlist_beg; t0 != NULL ; t0=t0->next){

        ip = ipp && (t0->speed != 0.0) ;
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
          gz0  = *w->traveller   ;    //  Copy to temporary traveller
          gz0.wagon = NULL       ;
          if(ip)printf(" Routine %s AA call trv_move\n",my_name) ;
          iret = trv_move(&gz0, 0.5*w->raw_wagon->length) ;
        }else{
          w = t0->last           ;    //  Back of train
          if(w == NULL) return 0 ;
          gz0 = *w->traveller    ;    //  Copy to temporary traveller
          gz0.wagon = NULL       ;
          if(ip)printf(" Routine %s BB call trv_move\n",my_name) ;
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

       if(ip)printf(" Routine %s CC call trv_move\n",my_name) ;
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
              if(ip)printf("    Check for train collision\n") ;
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
              if(ip)printf("  Routine %s.  Z0 :: %f %f :: %f\n",my_name,
                                        d0, d1, z0->sect_distance ) ;
              if(ip)printf("  Routine %s.  Z1 :: %f %f :: %f\n",my_name,
                                        d0, d1, z1->sect_distance ) ;
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
          if(ip)printf(" Routine %s DD call trv_move\n",my_name) ;
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
        if(ip)printf(" Routine %s EE call trv_move\n",my_name) ;
        trv_move(&(t0->front), 0.5*t0->first->raw_wagon->length) ;
        copy_traveller_position(&(t0->back), t0->last->traveller) ;
        if(ip)printf(" Routine %s FF call trv_move\n",my_name) ;
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

/*
 *   Routine to allow user to set up trains
 */


int trains_setup_by_user(){

int  ip = 0 ;
int  i, n, m, itrain, iret ;
int    n_engines   ;
int    n_tenders   ;
int    n_carriages ;
int    n_wagons    ;
int    n_consists  ;
int    n_platforms ;
int    n_sidings   ;

BTree  *engines   = NULL ;
BTree  *tenders   = NULL ;
BTree  *carriages = NULL ;
BTree  *wagons    = NULL ;
BTree  *consists  = NULL ;
BTree  *platform_list = NULL ;
BTree  *siding_list   = NULL ;
BTree  *found     = NULL ;
RawWagonNode *rw ;
ConsistNode  *consist ;
TrkItem      *ti ;
char         tname[10] ;




char  *my_name = "trains_setup_by_user" ;
/*
 *  Create Btrees containing engiens wagons, consists, Platforms, Sidings
 */

      printf("  PROCESS WAGONS\n") ;

      for(rw=rawwagonlist_beg; rw!=NULL; rw=rw->next){
        if(!strcmp(rw->type,"Engine")){
          engines = insert_node(engines,rw->name,(void *)rw);
        }else if(!strcmp(rw->type,"Tender")){
          tenders = insert_node(tenders,rw->name,(void *)rw);
        }else if(!strcmp(rw->type,"Carriage")){
          carriages = insert_node(carriages,rw->name,(void *)rw);
        }else if(!strcmp(rw->type,"Freight")){
          wagons = insert_node(wagons,rw->name,(void *)rw);
        }else{
          printf("  Type unknown:  %s :: %s\n",rw->type, rw->name) ;
        }
      }
      n_engines   = count_bt_nodes(engines)   ;
      n_tenders   = count_bt_nodes(tenders)   ;
      n_carriages = count_bt_nodes(carriages) ;
      n_wagons    = count_bt_nodes(wagons)    ;

      printf("  PROCESS CONSISTS\n") ;

      for(consist = consistlist_beg; consist != NULL ; consist = consist->next){
        consists = insert_node(consists,consist->name,(void *)consist);
      }
      n_consists   = count_bt_nodes(consists)   ;

      printf("  PROCESS TRACK\n") ;

      for(i=0; i<(int)track_db.trk_items_array_size; i++){
        ti = &(track_db.trk_items_array[i]) ;
        if(ti->type_of_node == PLATFORM && ti->platform_name
                                        && ti->platform_data1[0] == '0'){
          platform_list = insert_node(platform_list,ti->platform_name,(void *)ti);
        }else if(ti->type_of_node == SIDING && ti->siding_name
                                        && ti->siding_data1[0] == '0'){
          siding_list = insert_node(siding_list,ti->siding_name,(void *)ti);
        }
      }
      n_platforms  = count_bt_nodes(platform_list) ;
      n_sidings    = count_bt_nodes(siding_list)   ;

      for(itrain=0;;itrain++){

        sprintf(tname,"T%4.4i",itrain) ;

        add_new_train(tname) ;

        printf("\n  ENGINES\n\n");
        print_bt_nodes_with_count_and_index(engines) ;
        n = 0 ;
        while(n<1 || n>n_engines){
          printf("  Enter index of engine (or 0 to skip):\n") ;
          scanf("%i",&n) ;
          if(n>n_engines)continue ;
          if(n<1) break ;
          found = find_bt_node_with_index(engines,n) ;
          rw = (RawWagonNode *)found->data ;
          add_wagon_to_train(rw->name,1) ;
          printf("  Engine added.  Name = %s\n",rw->name) ;
        }

        printf("\n  TENDERS\n\n");
        print_bt_nodes_with_count_and_index(tenders) ;
        n = 0 ;
        while(n<1 || n>n_tenders){
          printf("  Enter index of tender (or 0 to skip):\n") ;
          scanf("%i",&n) ;
          if(n>n_tenders)continue ;
          if(n<1) break ;
          found = find_bt_node_with_index(tenders,n) ;
          rw = (RawWagonNode *)found->data ;
          add_wagon_to_train(rw->name,1) ;
          printf("  Tender added.  Name = %s\n",rw->name) ;
        }

        printf("\n  CARRIAGES\n\n");
        print_bt_nodes_with_count_and_index(carriages) ;
        n = 0 ;
        while(n<1 || n>n_carriages){
          printf("  Enter index of carriage (or 0 to skip):\n") ;
          scanf("%i",&n) ;
          if(n>n_carriages)continue ;
          if(n<1) break ;
          found = find_bt_node_with_index(carriages,n) ;
          rw = (RawWagonNode *)find_bt_node_with_index(carriages,n) ;
          add_wagon_to_train(rw->name,1) ;
          printf("  Carriage added.  Name = %s\n",rw->name) ;
        }

        printf("\n  FREIGHT\n\n");
        print_bt_nodes_with_count_and_index(wagons) ;
        n = 0 ;
        while(n<1 || n>n_wagons){
          printf("  Enter index of wagon (or 0 to skip):\n") ;
          scanf("%i",&n) ;
          if(n>n_wagons)continue ;
          if(n<1) break ;
          found = find_bt_node_with_index(wagons,n) ;
          rw = (RawWagonNode *)found->data ;
          add_wagon_to_train(rw->name,1) ;
          printf("  Wagon added.  Name = %s\n",rw->name) ;
        }

        printf("\n  CONSISTS\n\n");
        print_bt_nodes_with_count_and_index(consists) ;
        n = 0 ;
        while(n<1 || n>n_consists){
          printf("  Enter index of consist (or 0 to skip):\n") ;
          scanf("%i",&n) ;
          if(n>n_consists)continue ;
          if(n<1) break ;
          found = find_bt_node_with_index(consists,n) ;
          consist = (ConsistNode *)found->data ;
          add_consist_to_train(consist->name) ;
          printf("  Consist added.  Name = %s\n",consist->name) ;
        }

        m = 0 ;
        printf("\n  PLATFORMS\n\n");
        print_bt_nodes_with_count_and_index(platform_list) ;
        n = 0 ;
        while(n<1 || n>n_platforms){
          printf("  Enter index of platform (or 0 to skip):\n") ;
          scanf("%i",&n) ;
          if(n>n_platforms) continue ;
          if(n<1) break ;
          found = find_bt_node_with_index(platform_list,n) ;
          ti = (TrkItem *)found->data ;
          position_train2(ti->platform_name,1,0.0) ;
          printf("  Position at platform.  Name = %s\n",ti->platform_name) ;
          m = 1 ;
          break ;
        }
        if(m == 0){
          printf("\n  SIDINGS\n\n");
          print_bt_nodes_with_count_and_index(siding_list) ;
          n = 0 ;
          while(n<1 || n>n_sidings){
            printf("  Enter index of siding (or 0 to skip):\n") ;
            scanf("%i",&n) ;
            if(n>n_sidings) continue ;
            if(n<1) break ;
            found = find_bt_node_with_index(siding_list,n) ;
            ti = (TrkItem *)found->data ;
            position_train2(ti->siding_name,1,0.0) ;
            printf("  Position at siding.  Name = %s\n",ti->siding_name) ;
          }
        }
        printf(" Enter 1 for additional train or 0 to continue:\n");
        iret = 0 ;
        scanf("%i",&iret) ;
        if(!iret) return 0 ;

      }
      return 0 ;
}
