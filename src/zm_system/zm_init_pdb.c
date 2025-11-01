/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File zm_init_pdb.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File contains routines for loading the project data base.  In zm this is
 *   done each time a new route is chosen using the menu.
 *
 *==============================================================================
 */
/*
 *  Read the project (.trk) file and populate the project database
 *     The database is stored in a PdbNode structure
 *     For the moment only one pdb is supported, stored as the
 *     first item of a linked list, and pointed to by pdb_beg
 *
 *  Returns '1' if unable to find pdb file
 *          '0' om success
 */

int init_pdb(void){

  int       ip = 0 ;            // DEBUG
  int       j, l, skip_rb;
  char      *full_filename ;
  char      *token, *token2 ;
  MSfile    msfile ;
  PdbNode   *pdb ;

  char      myname[] = "init_pdb, in file: data.c" ;

      if(ip)printf("\n   Enter init_pdb\n\n");
/*
 *  Create new Project node or clean old node
 */
      if(!project_db){
        project_db= (PdbNode *)malloc(sizeof(PdbNode));
        pdb_node_init(project_db) ;
      }else{
        pdb_node_clean(project_db) ;
      }
      pdb = project_db ;
      printf("GG in zm init_pdb for route %s\n",ORroute);
/*
 *  Generate *.trk filename
 */
      full_filename = (char *)malloc(strlen(ORroutedir)+strlen(ORroute)+6);
      strcpy(full_filename,ORroutedir);
      strcat(full_filename,"/");
      strcat(full_filename,ORroute);
      strcat(full_filename,".trk");
      if(ip)printf("    filename = %s\n",full_filename);
      pdb->filename = full_filename ;
/*
 *  Try opening file
 */
      l = open_msfile(full_filename, &msfile, 0, 0);
      if(l!=0){
        printf("\n\n  ERROR : Routine open_msfile failed to open file\n");
        printf("      File = %s\n",full_filename) ;
        return 1 ;
      }
      if(ip)printf("    msfile pointer  = %p\n\n",(void *)msfile.fp);
/*
 * File is open - read data
 */

      pdb->filetype = ctoken(&msfile) ;
      skip_lbr(&msfile);

      for(;;){
        token = ctoken(&msfile);
        if(ip)printf(" Token = ::%s::%c\n",token,token[0]) ;
        if(')' == token[0])break ;                   // End of data
        skip_rb = 0 ;
        skip_lbr(&msfile);
        SWITCH (token)
          CASE ("RouteID")
              pdb->RouteID = ctoken(&msfile) ;
              break;
          CASE ("Name")
              pdb->Name = ctoken(&msfile) ;
              break ;
          CASE ("Description")
              pdb->Description = ctoken(&msfile) ;
              pdb->Description2 =
                  convert_print_style_string(pdb->Description) ;
              break ;
          CASE ("Graphic")
              pdb->Graphic = strip_quotes(ctoken(&msfile)) ;
              break ;
          CASE ("LoadingScreen")
              pdb->LoadingScreen = strip_quotes(ctoken(&msfile)) ;
              break ;
          CASE ("Filename")
          CASE ("FileName")
              pdb->FileName = strip_quotes(ctoken(&msfile)) ;
              break ;
          CASE ("Electrified")
              pdb->Electrified = itoken(&msfile) ;
              break ;
          CASE ("Mountains")
              pdb->Mountains = itoken(&msfile) ;
              break ;
          CASE ("OverheadWireHeight")
              pdb->OverheadWireHeight = dtoken(&msfile) ;
              break ;
          CASE ("PassengerRuleSet")
              pdb->PassengerRuleSet = itoken(&msfile) ;
              break ;
          CASE ("FreightRuleSet")
              pdb->FreightRuleSet = itoken(&msfile) ;
              break ;
          CASE ("SignalSet")
              pdb->SignalSet = itoken(&msfile) ;
              break ;
          CASE ("GantrySet")
              pdb->GantrySet = itoken(&msfile) ;
              break ;
          CASE ("TrackGauge")
              pdb->TrackGauge = itoken(&msfile) ;
              break ;
          CASE ("Era")
              pdb->Era = itoken(&msfile) ;
              break ;
          CASE ("SpeedLimit")
              pdb->SpeedLimit = dtoken(&msfile)  ;
              break ;
          CASE ("Environment")
              for(j=0;j<12;j++){
                pdb->Environment[j].weather = ctoken(&msfile) ;
                skip_lbr(&msfile);
                pdb->Environment[j].envfile = ctoken(&msfile) ;
                skip_rbr(&msfile);
              }
              break ;
          CASE ("TerrainErrorScale")
              pdb->TerrainErrorScale = dtoken(&msfile) ;
              break ;
          CASE ("RouteStart")
              for(j=0;j<4;j++){
                pdb->RouteStart[j] = itoken(&msfile) ;
              }
              break ;
          CASE ("MilepostUnitsMiles")
              token2 = new_tmp_token(&msfile);  // Temporary token
              if(strcmp(token2,")")==0){
                pdb->MilepostUnitsMiles  = 0 ;
                skip_rb = 1;
                break ;
              }
              pdb->MilepostUnitsMiles    = atof(token2) ;
              break ;
          CASE ("MaxLineVoltage")
              pdb->MaxLineVoltage        = dtoken(&msfile) ;
              break ;
          CASE ("DefaultSignalSMS")
              pdb->DefaultSignalSMS      = ctoken(&msfile) ;
              break ;
          CASE ("DefaultCrossingSMS")
              pdb->DefaultCrossingSMS    = ctoken(&msfile) ;
              break ;
          CASE ("DefaultWaterTowerSMS")
              pdb->DefaultWaterTowerSMS  = ctoken(&msfile) ;
              break ;
          CASE ("DefaultCoalTowerSMS")
              pdb->DefaultCoalTowerSMS   = ctoken(&msfile) ;
              break ;
          CASE ("DefaultDieselTowerSMS")
              pdb->DefaultDieselTowerSMS = ctoken(&msfile) ;
              break ;
          CASE ("TempRestrictedSpeed")
              pdb->TempRestrictedSpeed   = dtoken(&msfile) ;
              break ;
          CASE ("DerailScale")
              pdb->DerailScale           = dtoken(&msfile) ;
              break ;
          CASE ("TimetableTollerance")
              pdb->TimetableTollerance   = dtoken(&msfile) ;
              break ;
          CASE ("GravityScale")
              pdb->GravityScale          = dtoken(&msfile) ;
              break ;
          CASE ("MilepostUnitsKilometers")
              pdb->MilepostUnitsKilometers = 1 ;
              break ;
          CASE ("ORTSUserPreferenceForestClearDistance")
              pdb->ORTSForestClearDistance   = dtoken(&msfile) ;
              break ;
          CASE ("ORTSSwitchSMSNumber")
              pdb->ORTSSwitchSMSNumber   = itoken(&msfile) ;
              break ;
          CASE ("ORTSCurveSMSNumber")
              pdb->ORTSCurveSMSNumber    = itoken(&msfile) ;
              break ;
          CASE ("ORTSCurveSwitchSMSNumber")
              pdb->ORTSCurveSwitchSMSNumber = itoken(&msfile) ;
              break ;

          DEFAULT
              printf("  Routine %s\n",myname) ;
              printf("  Unrecognised token\n") ;
              printf("  Token = %s\n",token) ;
              exit(1) ;
              break ;
        END
        if(skip_rb ==0)skip_rbr(&msfile);
        free(token);
//        pdb_print(pdb_end);
      }
      if(NULL != token) free(token) ;
      close_msfile(&msfile);
      if(ip)printf("\n") ;

      return 0;
}

int pdb_node_init(PdbNode  *pdb){

  int i;

      pdb->filename      = NULL ;
      pdb->filetype      = NULL ;
      pdb->RouteID       = NULL ;
      pdb->Name          = NULL ;
      pdb->Description   = NULL ;
      pdb->Description2  = NULL ;
      pdb->Graphic       = NULL ;
      pdb->LoadingScreen = NULL ;
      pdb->FileName      = NULL ;

      pdb->DefaultSignalSMS      = NULL ;
      pdb->DefaultCrossingSMS    = NULL ;
      pdb->DefaultWaterTowerSMS  = NULL ;
      pdb->DefaultCoalTowerSMS   = NULL ;
      pdb->DefaultDieselTowerSMS = NULL ;

      pdb->Electrified        = 0 ;
      pdb->Mountains          = 0 ;
      pdb->OverheadWireHeight = 0 ;
      pdb->PassengerRuleSet   = 0 ;
      pdb->FreightRuleSet     = 0 ;
      pdb->SignalSet          = 0 ;
      pdb->GantrySet          = 0 ;
      pdb->TrackGauge         = 0 ;
      pdb->Era                = 0 ;
      pdb->MilepostUnitsMiles = 0 ;
      pdb->MilepostUnitsKilometers = 0 ;
      pdb->MaxLineVoltage     = 0 ;

      pdb->ORTSSwitchSMSNumber      = 0 ;
      pdb->ORTSCurveSMSNumber       = 0 ;
      pdb->ORTSCurveSwitchSMSNumber = 0 ;

      pdb->SpeedLimit           = 0.0 ;
      pdb->TerrainErrorScale    = 0.0 ;
      pdb->TempRestrictedSpeed  = 0.0 ;
      pdb->DerailScale          = 0.0 ;
      pdb->TimetableTollerance  = 0.0 ;
      pdb->GravityScale         = 1.0 ;
      pdb->ORTSForestClearDistance = 20.0 ;

      for(i=0;i<12;i++){
        pdb->Environment[i].weather = NULL ;
        pdb->Environment[i].envfile = NULL ;
      }
      for(i=0;i<4;i++){
        pdb->RouteStart[i] = 0 ;
      }

      return 0;
}

int pdb_node_clean(PdbNode  *pdb){

  int i;

      if(pdb->filename)     free(pdb->filename) ;
      if(pdb->filetype)     free(pdb->filetype) ;
      if(pdb->RouteID)      free(pdb->RouteID) ;
      if(pdb->Name)         free(pdb->Name) ;
      if(pdb->Description)  free(pdb->Description) ;
      if(pdb->Description2) free(pdb->Description2) ;
      if(pdb->Graphic)      free(pdb->Graphic) ;
      if(pdb->LoadingScreen)free(pdb->LoadingScreen) ;
      if(pdb->FileName)     free(pdb->FileName) ;

      if(pdb->DefaultSignalSMS)     free(pdb->DefaultSignalSMS) ;
      if(pdb->DefaultCrossingSMS)   free(pdb->DefaultCrossingSMS) ;
      if(pdb->DefaultWaterTowerSMS) free(pdb->DefaultWaterTowerSMS) ;
      if(pdb->DefaultCoalTowerSMS)  free(pdb->DefaultCoalTowerSMS) ;
      if(pdb->DefaultDieselTowerSMS)free(pdb->DefaultDieselTowerSMS) ;

      pdb->filename      = NULL ;
      pdb->filetype      = NULL ;
      pdb->RouteID       = NULL ;
      pdb->Name          = NULL ;
      pdb->Description   = NULL ;
      pdb->Graphic       = NULL ;
      pdb->LoadingScreen = NULL ;
      pdb->FileName      = NULL ;

      pdb->DefaultSignalSMS      = NULL ;
      pdb->DefaultCrossingSMS    = NULL ;
      pdb->DefaultWaterTowerSMS  = NULL ;
      pdb->DefaultCoalTowerSMS   = NULL ;
      pdb->DefaultDieselTowerSMS = NULL ;

      pdb->Electrified        = 0 ;
      pdb->Mountains          = 0 ;
      pdb->OverheadWireHeight = 0 ;
      pdb->PassengerRuleSet   = 0 ;
      pdb->FreightRuleSet     = 0 ;
      pdb->SignalSet          = 0 ;
      pdb->GantrySet          = 0 ;
      pdb->TrackGauge         = 0 ;
      pdb->Era                = 0 ;
      pdb->MilepostUnitsMiles = 0 ;
      pdb->MilepostUnitsKilometers = 0 ;
      pdb->MaxLineVoltage     = 0 ;

      pdb->ORTSSwitchSMSNumber      = 0 ;
      pdb->ORTSCurveSMSNumber       = 0 ;
      pdb->ORTSCurveSwitchSMSNumber = 0 ;

      pdb->SpeedLimit           = 0.0 ;
      pdb->TerrainErrorScale    = 0.0 ;
      pdb->TempRestrictedSpeed  = 0.0 ;
      pdb->DerailScale          = 0.0 ;
      pdb->TimetableTollerance  = 0.0 ;
      pdb->GravityScale         = 1.0 ;
      pdb->ORTSForestClearDistance = 20.0 ;

      for(i=0;i<12;i++){
        pdb->Environment[i].weather = NULL ;
        pdb->Environment[i].envfile = NULL ;
      }
      for(i=0;i<4;i++){
        pdb->RouteStart[i] = 0 ;
      }
      return 0;
}

int pdb_print(PdbNode *pdb){

  int i ;

//  printf("\nTRK_PRINT $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf(" filename = %s\n",pdb->filename);
    printf(" filetype = %s\n",pdb->filetype);
    printf(" RouteID = %s\n",pdb->RouteID);
    printf(" Name = %s\n",pdb->Name);
    printf(" Description  = %s\n",pdb->Description);
    printf(" Description2 = %s\n",pdb->Description2);
    printf(" Graphic = %s\n",pdb->Graphic);
    printf(" LoadingScreen = %s\n",pdb->LoadingScreen);
    printf(" Electrified = %i\n",pdb->Electrified);
    printf(" Mountains = %i\n",pdb->Mountains);
    printf(" OverheadWireHeight = %i\n",pdb->OverheadWireHeight);
    printf(" PassengerRuleSet = %i\n",pdb->PassengerRuleSet);
    printf(" FreightRuleSet = %i\n",pdb->FreightRuleSet);
    printf(" SignalSet = %i\n",pdb->SignalSet);
    printf(" GantrySet = %i\n",pdb->GantrySet);
    printf(" TrackGauge = %i\n",pdb->TrackGauge);
    printf(" Era = %i\n",pdb->Era);
    printf(" SpeedLimit = %f\n",(float)pdb->SpeedLimit);
    for(i=0;i<12;i++){
      printf(" Environment[%i] = %s\n",i,pdb->Environment[i].weather);
      printf(" Environment[%i] = %s\n",i,pdb->Environment[i].envfile);
    }
    printf(" TempRestrictedSpeed = %f\n",(float)pdb->TempRestrictedSpeed);

    return 0;
}

char* find_trk_file(){

  int    ip = 0     ;
  int    l, iret    ;
  char   *route_dir ;
  char   *trk_file = NULL ;
  DIR    *wdir ;
  struct dirent *f_entry ;
  struct stat   *stbuf ;

  char *my_name = "find_trk_file" ;

      route_dir = strdup(ORroutedir) ;
      if(ip)printf("  Routine %s.  Route directory = %s\n",my_name,route_dir) ;
/*
 *  Find route directory
 */
      iret = zr_find_msfile2(&route_dir) ;
      if(iret){
        printf("  Routine %s.  Unable to find directory %s\n",my_name,route_dir);
        exit(0) ;
      }
      if(ip)printf("  Routine %s.  Route directory = %s\n",my_name,route_dir) ;
/*
 *  Open directory
 */
      wdir = opendir(route_dir) ;
      while ((f_entry = readdir(wdir)) != NULL) {
        l = strlen(f_entry->d_name) ;
        if(strcmp_ic(".trk",&(f_entry->d_name[l-4])) != 0) continue ;
/*
 *  Generate full file name
 */
        trk_file = strdup(f_entry->d_name) ;
        if(ip)printf("    Track file name = %s\n", trk_file);
      }
      closedir(wdir) ;
      free(route_dir) ;

      return trk_file ;
}
