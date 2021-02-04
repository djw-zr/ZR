/*
 *******************************************************************************
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File data.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 * *****************************************************************************
 *   File contains routines for reading the data files
 *   and populating the data structures.
 * *****************************************************************************
 *
 *   This code presently only works with the Australian Zig-Zag Route
 *
 *   Note : Because the word 'track' can have many meanings and
 *          the accronym for Route Data Base (rdb) can be confused
 *          with Road Data Base, the term Project Data Base (pdb)
 *          is used.
 * *****************************************************************************
 */

int init_data_structures(){

char pdb_file[] = "au_great_zig_zag.trk" ;   // Project file : Name, Description, Route start
char tdb_file[] = "au_great_zig_zag.tdb" ;   // Track data base : nodes, crossings etc.
char rdb_file[] = "au_great_zig_zag.rdb" ;   // Road data base  : nodes etc.
//char ace_file[] = "au_great_zig_zag.ace" ;   // Splash Image
                                             // not unicode, compress or ascii.

int            i      ;
int            ip = 0  ;  //  0 = no printing
TileListNode   *tlnode ;
ShapeNode      *snode  ;
WorldNode      *wnode  ;
WorldItem      *witem  ;
int            wtype   ;
TextureNode    *tnode  ;
DynProfile     *dnode  ;
char           my_name[] = "init_data_structures" ;

      printf("***********************************************************\n") ;
      printf(" Enter %s\n",my_name) ;
/*
 * *****************************************************************************
 *  First read the project files in the route directory. In the
 *  great zig-zag set of tiles this is directory
 *       Routes/au_great_zig_zag
 * *****************************************************************************
 *  The project files are:
 *  1.  The project data base file (*.trk).
 *      The data is stored in a PdbNode structure, the first node
 *      in a linked list pointed to by 'pdb_beg' (zr.h).
 */
      printf("***********************************************************\n") ;
      printf(" PROCESS SYSTEM FILES\n") ;
      printf("   Read OR project file\n");
      init_pdb(pdb_file) ;
//   Check that all files have been close
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;

/*
 *  2.  The track database (*.tdb)
 *      The data is stored in track_db (zr.h), a structure of
 *      type TrkDataBase, which contains 1-D arrays of track sections
 *      (positions of individual sections of track) and track items
 *      (positions of sidings, speedposts, crossovers, platforms etc.)
 */
      printf("   Read track database file\n");
      init_track_db(tdb_file);
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *  3.  The road database (*.rdb)
 *      The data is stored in road_db (zr.h), a structure of
 *      type TrkDataBase.  Contains 1-D arras of road sections
 *      and road items
 */
      printf("   Read road database file\n");
      init_road_db(rdb_file);
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *  Open tsection.dat file
 */
      init_tsec_db() ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 * *****************************************************************************
 *  Then process the tiles (sub-directory 'TILES')
 * *****************************************************************************
 *
 *   1.  Create a list of tiles (*.t and *_y.raw)
 *       Use limits of track to identify tile files and store valid
 *       filenames in a linked list of TileListNode structures
 *       pointed to by tilelist_head (zr.h).
 */
      printf("***********************************************************\n") ;
      printf(" PROCESS TILES\n") ;
      printf("   Create list of tile filenames\n");
      tilelist_init2();

//      tilelist_init(trk_cell_west,trk_cell_east,
//                    trk_cell_south,trk_cell_north);
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *   2.   Read read and process the tile data
 *        The *.t files contain data about the textures used for
 *        the terrain.  This is stored in the TerrainData structure
 *        part of each TileListNode.
 *        The *_y.raw files contain topography.  This is stored in
 *        the elevations array, part of the TerrainData structure.
 */
      printf("   Read and load tile files\n") ;
      for(tlnode = tilelist_head;tlnode!=NULL;tlnode=tlnode->next){
        if(0 == tlnode->t_found) continue ;
        read_ms_terrain(tlnode)    ;
        read_elevations(tlnode)    ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 * *****************************************************************************
 *  Process the world files (sub-directory 'world')
 * *****************************************************************************
 *
 *   1.   Create a list of world files (*.w) in directory world
 *        Each file contains data for a separate tile.
 *        The filenames are stored in a linked list of WorldNode
 *        structures, the first pointed to by worldlist_begin (zr.h).
 */
      printf("   Create list of world filenames\n") ;
      load_world_filenames() ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *   2.   Read and process the world file of each WorldNode.
 *        The files contain information on all fixed structures, both
 *        active and passive (buildings, signals etc.), in the model
 *        world. The data is stored in a linked list of WorldItem
 *        structures within each WorldNode
 */
      printf("   Read and load world files\n") ;
      for(wnode=worldlist_beg; wnode!=NULL; wnode=wnode->next){
        load_world(wnode) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 * *****************************************************************************
 *  Process the files describing engines and wagons.
 * *****************************************************************************
 *
 *    The files describing engines and wagons are found in directories
 *    below XX/Trains/Trainsets.  Here XX can be any of the top level
 *    directories:
 *       MSTSdir    // Top directory for MSTS files
 *       ORdir      // Top directory for OR routes
 *       ORroutedir // Top directory of current route
 *    In the present version of the routines only ORdir is searched.
 *
 *   1.   Generate a lists of wagon files and texture files
 *
 *   a)   Walk file structure and for each *.eng and *.wag
 *        file found generate a new WagonNode structure and add it
 *        to the linked list pointed to by wagonlist_beg (zr.h).
 *        Add *.jpg (side image), *.s (3-D shape) and *.sd
 *        (bounding box etc) files with the same name to the
 *        WagonNode structure.
 *   b)   For each *.ace file found generate a TextureNode
 *        structure and add it to the linked list pointed to by
 *        wtexturelist_beg (zr.h).
 *        Note:  Many wagons will also use other MSTS or route textures.
 * *****************************************************************************
 */
      printf("***********************************************************\n") ;
      printf(" PROCESS WAGONS\n") ;
      printf("   Generate lists of wagon files\n");
      generate_wagon_list() ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *   2.  Load the Shape (*.s) and Additional data (*.sd) files
 */
      printf("   Read and load wagon shape files\n");
      for(snode=wshapelist_beg; snode!=NULL; snode=snode->next){
        if(ip)printf(" Call load_shape :: shape name = %s\n",snode->name) ;
        load_shape(snode) ;
        if(ip)printf(" Call load_shape_d :: sdfile = %s\n",snode->sdfile) ;
        load_shape_d(snode) ;
//      if(!strcmp(snode->name,"4W-CV-load"))print_shape_file_data(snode) ;
//      if(!strcmp(snode->name,"4W-CW2"))print_shape_file_data(snode) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *   3.  Load wagon textures
 *       These are textures found in the
 *       The texture data is loaded into the TextureNode structure
 *       ready for use.
 */
      printf("   Read and load wagon texture files\n");
      for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
//        if(strncmp(tnode->name,"AU_Ash",6))continue ;
        load_texture(tnode) ;
        if(ip)printf(" Texture name = %s\n",tnode->name) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *    4.  Add wagon texture pointers to wagon structures
 *        Each WagonNode structure contains a list of requied
 *        textures.  After this point it also contains pointers to
 *        the corresponding texture structures in memory.
 */
      printf("   Add links to textures needed by each wagon shape\n");
      for(snode=wshapelist_beg; snode!=NULL; snode=snode->next){
        add_texture_pointers_to_wagon_shapes(snode) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 * *****************************************************************************
 *  Process the files describing the 3-D structures in the world files
 * *****************************************************************************
 *
 *      The files describing fixed 'shapes' are found in directories
 *      with names 'SHAPES", "Shapes" etc.  The present version of the
 *      routines searches:
 *            'ORdir'/Global/Shapes/        MSTS Shapes??
 *            'ORroutedir'/SHAPES/          Locally generated shapes
 *
 *      Shapes in directory Global/Shapes may be original MSTS files.
 *
 *      The data from each shape file is placed in a ShapeNode
 *      structure.  This is in a linked list pointed to by
 *      'shapelist_beg' (zr.h).  If a *.sd file is present its name
 *      is added to the structure.
 *
 *   1.  Generate a list of shape files (*.s)
 *
 * *****************************************************************************
 */
      printf("***********************************************************\n") ;
      printf(" PROCESS SHAPES\n") ;
      printf("   Create list of shape files \n") ;
      load_shape_filenames() ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *   2.   Load shape data from *.s and *.sd files
 *        Populate the ShapeNode structures.
 */
      printf("   Read and load shape files\n");
      for(snode=shapelist_beg; snode!=NULL; snode=snode->next){
        if(0)printf(" data :: shape name = %s\n",snode->name) ;
        load_shape(snode) ;
        load_shape_d(snode) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *    3.  Add shape pointers to the world files.
 *        Each world node includes the name of a 3-D structure.
 *        After this stage it also contains a pointer to the
 *        structure's ShapeNode in memory.
 */
      printf("    Add links to shape items used by each world tile\n");
      add_shape_pointers_to_world_items() ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 * *****************************************************************************
 *  Process the files containing the textures needed by the world
 *  shapes (structures).
* *****************************************************************************
 *      The files textures are found in directories
 *      with names 'TEXTURES", "Textures" etc.  The present version of the
 *      routines searche:
 *            'ORdir'/Global/Textures/        MSTS Textures??
 *            'ORroutedir'/TEXTURES/          Locally generated shapes
 *
 *      Textures in directory 'Global/Textures/' may be original MSTS files.
 * *****************************************************************************
 *
 *     1.  Create list of texture files
 *        For each file initialise a new TextureNode, part of
 *        a linked list pointed at by texturelist_beg (zr.h).
 *
 * *****************************************************************************
 */
      printf("   Create list of texture filenames \n") ;
      load_texture_filenames() ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *     2.  Read texture files
 *         The data is added to the TextureNode
 */
      printf("   Read and load texture files\n");
      for(tnode=texturelist_beg; tnode!=NULL; tnode=tnode->next){
//        if(strncmp(tnode->name,"AU_Ash",6))continue ;
        load_texture(tnode) ;
//  Check for interim land texture
        if(strcmp_ic(tnode->name,land_texture_default))continue;
        tnode->basic = 1     ;    //  Must be loaded
        land_texture = tnode ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *      3.  In each ShapeNode add pointers to the required textures.
 */
      printf("    Add links to textures needed by each shape\n");
      for(snode=shapelist_beg; snode!=NULL; snode=snode->next){
        add_texture_pointers_to_shape_items(snode) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 * *****************************************************************************
 *    If rail and road profiles are not provided then generate them
 * *****************************************************************************
 */
      if(zr_road_profile == NULL || zr_road_profile == NULL){
        printf("**********************************************************\n") ;
        printf(" GENERATE NEEDED RAIL AND ROAD PROFILES\n") ;
        if(zr_rail_profile == NULL){
          printf("   Make default track profile\n");
          make_default_track_profile() ;                      // track_setup.c
//          printf("  rail_profile = %p %p\n",(void *)zr_rail_profile, (void *)&zr_rail_profile);
        }
        if(zr_road_profile == NULL){
          printf("   Make default road profile\n");
          make_default_road_profile() ;
        }
      }
/*
 * *****************************************************************************
 *    Add texture pointers to the profiles
 * *****************************************************************************
 */
      add_texture_pointers_to_track_profiles(zr_rail_profile) ;
      add_texture_pointers_to_track_profiles(zr_road_profile) ;
/*
 * *****************************************************************************
 *    Option to generate a complete set of rail and road shapes
 *           based in the rail (track) and road databases.
 * *****************************************************************************
 */
      if(0){
        printf("***********************************************************\n") ;
        printf("   Create default track shapes\n") ;
        make_track_shapes(zr_rail_profile) ;
        make_road_shapes(zr_road_profile)  ;
        if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
      }
/*
 * *****************************************************************************
 *    Print summary data
 * *****************************************************************************
 */
      printf("***********************************************************\n") ;
      printf(" PRINT SUMMARY DATA\n") ;
// List tiles
#if 0
TileListNode *tlist ;
int   nn, ni ;
      printf("  List of tiles\n") ;
      printf("      tile_west,  tile_east  = %i %i\n",
                                        tile_west, tile_east) ;
      printf("      tile_south, tile_north = %i %i\n",
                                        tile_south,tile_north) ;
      ni = tile_east + 1 - tile_west ;
      for(j=tile_south;j<tile_north+1;j++){
        for(i=tile_west;i<tile_east+1;i++){
          nn = (j-tile_south)*ni + i - tile_west ;
          if(tile_array[nn]==NULL) continue ;
          printf("    Tile %i %i\n",i,j)    ;
        }
      }
#endif
// List track sections
#if 0
      printf("   List track sections\n");
      for(i=0;i<(int)track_db.trk_sections_array_size;i++){
        printf("  Track section %3i,  type of item = %2i :: %s\n", i,track_db.trk_sections_array[i].type_of_node,
         token_trackdb[track_db.trk_sections_array[i].type_of_node]) ;
        if(1){
         list_track_section(&track_db.trk_sections_array[i]) ;
        }
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
#endif
// List track items
#if 0
      printf("   List track items\n");
      for(i=0;i<(int)track_db.trk_items_array_size;i++){
        printf("    Track item %3i,  type of item = %2i :: %s\n", i,track_db.trk_items_array[i].type_of_node,
         token_trackdb[track_db.trk_items_array[i].type_of_node]) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
#endif
//   List and/or print shape data
//   Shape 'test_shape' is defined in file 'zr.h'.
#if 0
      printf("   List textures\n");
      for(snode=shapelist_beg,i=0; snode!=NULL; snode=snode->next,i++){
        printf("   Shape : %3i : %s\n",i,snode->name);
        if(0 == strcmp_ic(snode->name,test_shape)){
          printf("\n  Shape file data:\n") ;
          print_shape_file_data(snode)  ;
        }
      }
#endif
/*
 *   List texture data
 */
#if 0
      printf("   List textures\n");
      for(tnode=texturelist_beg,i=0; tnode!=NULL; tnode=tnode->next,i++){
        printf("     Texture : %3i : %s\n",i,tnode->name);
      }
      printf("   List wagon texture files\n");
      for(tnode=wtexturelist_beg,i=0; tnode!=NULL; tnode=tnode->next,i++){
        printf("     Texture : %3i : %s\n",i,tnode->name);
      }
#endif
/*
 * List Tile data
 */
#if 0
      if(ip){
        printf(" List tile data\n") ;
        for(tlnode = tilelist_head;tlnode!=NULL;tlnode=tlnode->next){
          if(tlnode->t_found==0) continue ;
          printf("\n");
          printf("  File      = %s\n",tlnode->t_file);
          printf("    Ybuffer = %s\n",tlnode->terrain_data.ybuffer);
          for(i=0;i<tlnode->terrain_data.number_of_shaders;i++){
            printf("    Shader %i,  Name = %s\n",i,tlnode->terrain_data.tile_shader[i].name);
          }

          n = tlnode->terrain_data.terrain_nsamples ;
          kp = tlnode->terrain_data.elevations      ;
          kmin = kmax = *kp ;
#if 1
          for(j=0;j<n;j++){
          for(i=0;i<n;i++,kp++){
            k = *kp ;
            if(k<kmin)kmin = k ;
            if(k>kmax)kmax = k ;
//          if(in==1 && j<10 && i<10)printf(" %6.6i ",k);
//          if(in==1 && j<10 && i==10)printf("\n");
//          if(in == 0)printf(" %6.6i ",k) ;
          }
//          if(in == 0)printf("\n") ;
          }
#endif
          printf("  kmin = %i\n",(int)kmin);
          printf("  kmax = %i\n",(int)kmax);
//        in = 1 ;


          i = tlnode->tilex - tile_west  ;
          j = tlnode->tiley - tile_south ;
          n = j*(tile_east + 1 - tile_west) + i ;
          printf(" i, j, n = %i %i %i\n",i,j,n);
          printf(" Check node1 = %p\n",(void *)tlnode);
          printf(" Check node2 = %p\n",(void *)tile_array[n]);
        }
      }
#endif

      printf("  tile_west  = %i\n",tile_west) ;
      printf("  tile_east  = %i\n",tile_east) ;
      printf("  tile_north = %i\n",tile_north) ;
      printf("  tile_south = %i\n",tile_south) ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
      return 0 ;
}

/*
 *  Read the project (.trk) file and populate the project database
 *     The database is stored in a PdbNode structure
 *     For the moment only one pdb is supported, stored as the
 *     first item of a linked list, adn pointed to by pdb_beg
 */

int init_pdb(char *filename)
{
int       j, l, skip_rb;
int       ip = 0 ;            // 0 = no printing
char      *full_filename ;
char      *token, *token2 ;
MSfile    msfile ;
PdbNode   *pdb ;
char      myname[] = "init_pdb, in file: data.c" ;

      if(ip)printf("\n  Enter init_pdb\n\n");

      full_filename = (char *)malloc(strlen(ORroutedir)+strlen(filename)+1);
      strcpy(full_filename,ORroutedir);
      strcat(full_filename,filename);
      if(ip)printf("    file_in_name = %s\n",full_filename);

      l = open_msfile(full_filename, &msfile, 0, 0);
      if(l!=0){
        printf("\n\n  ERROR : Routine open_msfile failed to open file\n\n");
        exit(1) ;
      }
      if(ip)printf(" msfile pointer  = %p\n\n",(void *)msfile.fp);
//
//  Create new trknode
//
      pdb = (PdbNode *)malloc(sizeof(PdbNode));
      pdb_init(pdb) ;

      if(NULL == pdb_end){
        pdb_beg = pdb ;
      }else{
        pdb_end->next = pdb ;
      }
      pdb_end = pdb ;

      pdb->filename = full_filename ;

      pdb->filetype = ctoken(&msfile) ;
      skip_lbr(&msfile);

      for(;;){
        token = ctoken(&msfile);
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
              break ;
          CASE ("Graphic")
              pdb->Graphic = ctoken(&msfile) ;
              break ;
          CASE ("LoadingScreen")
              pdb->LoadingScreen = ctoken(&msfile) ;
              break ;
          CASE ("FileName")
              pdb->FileName = ctoken(&msfile) ;
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

          DEFAULT
              printf("Routine %s\n",myname) ;
              printf("in DEFAULT &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
              exit(1) ;
              break ;
        END
        if(skip_rb ==0)skip_rbr(&msfile);
        free(token);
//        pdb_print(pdb_end);
      }
      if(NULL != token) free(token) ;
      close_msfile(&msfile);

      return 0;
}

int pdb_init(PdbNode  *pdb){
int i;
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
      pdb->MaxLineVoltage     = 0 ;

      pdb->SpeedLimit           = 0.0 ;
      pdb->TerrainErrorScale    = 0.0 ;
      pdb->TempRestrictedSpeed  = 0.0 ;

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

    printf("\nTRK_PRINT $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf(" filename = %s\n",pdb->filename);
    printf(" filetype = %s\n",pdb->filetype);
    printf(" RouteID = %s\n",pdb->RouteID);
    printf(" Name = %s\n",pdb->Name);
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
