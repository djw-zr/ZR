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
char* find_trk_file() ;

int init_data_structures(){

/*
 *  Define route files - [Eventually these should be searched for]
 */

#if defined ROUTE_MSTS  || defined ROUTE_AU_NSW_SW_SS
  char *pdb_file0 = msts_pdb_file ;
  char *tdb_file0 = msts_tdb_file ;
  char *rdb_file0 = msts_rdb_file ;
#elif defined ROUTE_NEW_FOREST
// Project file : Name, Description, Route start
  char pdb_file0[] = "Watersnake.trk" ;
// Track data base : nodes, crossings etc.
  char tdb_file0[] = "Watersnake.tdb" ;
// Road data base  : nodes etc.
  char rdb_file0[] = "Watersnake.rdb" ;
#else
// Project file : Name, Description, Route start
  char pdb_file0[] = "au_great_zig_zag.trk" ;
// Track data base : nodes, crossings etc.
  char tdb_file0[] = "au_great_zig_zag.tdb" ;
// Road data base  : nodes etc.
  char rdb_file0[] = "au_great_zig_zag.rdb" ;
// Splash Image
//  char spl_file[] = "au_great_zig_zag.ace" ;
#endif
  char spl_file[] = "au_great_zig_zag.ace" ;

char *pdb_file ;
char *tdb_file ;
char *rdb_file ;

uint           i, n, iret   ;
uint           ip = 0  ;  //  0 = no printing
TileListNode   *tl_node ;
ShapeNode      *snode  ;
WorldNode      *wnode  ;
WorldItem      *witem  ;
TrkSector      *tnnode ;
TrkItem        *tinode ;
int            wtype   ;
TextureNode    *tx_node ;
DynProfile     *dnode  ;
RawWagonNode   *rwnode ;
SigScriptFile  *sig_script ;
SignalDB       *signal ;
BTree          *btree  ;
char           my_name[] = "init_data_structures" ;

      printf("***********************************************************\n") ;
      printf(" Enter %s\n",my_name) ;
      enum_btree_init() ;
      printf("  ORdir      = %s\n",ORdir) ;
      printf("  ORroute    = %s\n",ORroute) ;
      printf("  ORroutedir = %s\n",ORroutedir) ;
      printf("  MSTSdir    = %s\n",MSTSdir) ;

#if 0
      pdb_file = pdb_file0 ;
      tdb_file = tdb_file0 ;
      rdb_file = rdb_file0 ;
#else
      printf("  Construct names.  ORroute = %s\n",ORroute) ;
      n = strlen(ORroute) + 5 ;
      pdb_file = (char *)malloc(n) ;
      strcpy(pdb_file,ORroute) ;
      strcat(pdb_file,".trk") ;
#endif
      iret = zr_find_msfile2(&pdb_file) ;
      if(iret){
        free(pdb_file) ;
        pdb_file = find_trk_file() ;
        if(!pdb_file){
          printf("   Track file (*.trk) for route not found.\n") ;
          exit(0) ;
        }
      }
#if 1
      printf("   pdb_file = %s\n",pdb_file) ;
#endif
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
      printf("***********************************************************\n") ;
      printf("   Read OR project file\n");
      init_pdb(pdb_file) ;
      n = strlen(pdb_beg->FileName) + 5;
      tdb_file = (char *)malloc(n) ;
      rdb_file = (char *)malloc(n) ;
      strcpy(tdb_file,pdb_beg->FileName) ; strcat(tdb_file,".tdb") ;
      strcpy(rdb_file,pdb_beg->FileName) ; strcat(rdb_file,".rdb") ;
      printf("   tdb_file = %s\n",tdb_file) ;
      printf("   rdb_file = %s\n",rdb_file) ;

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
      printf("   Read track section database file\n");
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
      printf("***********************************************************\n") ;
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
      for(tl_node = tilelist_head;tl_node!=NULL;tl_node=tl_node->next){
//        printf("  Tile : %i %i :: %i\n",tl_node->tilex, tl_node->tiley,
//                                        tl_node->t_found) ;
        if(0 == tl_node->t_found) continue ;
        read_ms_terrain(tl_node)    ;
        read_elevations(tl_node)    ;
      }
//double height ;
//      find_height(1.4,10.4,&height) ;
//      printf("  Height = %f\n",height) ;
//      exit(0) ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
#if 1
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
      printf("\n   Create list of world filenames\n") ;
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
        bt_walk_a2z(wnode->shape_tree, add_world_shapes_to_master) ;
#ifdef OPENAL
        load_world_soundfile(wnode) ;  //  Load SMS file
#endif
      }
      if(ip)dump_btree(shape_master,0,"X") ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *   Link each world sound source and sound region to its
 *   Sound Management System node.
 */
#ifdef OPENAL
      link_world_sounds_to_sms_nodes() ;
#endif
/*
 *  3.   Link track vector nodes to their matching world item
 *       The  add vector length, angle and radius to each track vector node
 */
      printf("   Link world files to track vectors\n") ;
      for(i=0;i<track_db.trk_sections_array_size;i++){
        tnnode = &(track_db.trk_sections_array[i]) ;
        if(ip)printf("  Routine %s, Track Section Node %i, Index %i, Type %i\n",
            my_name, i, tnnode->uid, tnnode->type_of_node) ;
        add_world_item_pointers_to_track_vectors(tnnode) ;
      }
/*
 *  4.   Set junction data : left/right handed and index of branch node
 */
      printf("   Set junction paths\n") ;
      for(i=0;i<track_db.trk_sections_array_size;i++){
        tnnode = &(track_db.trk_sections_array[i]) ;
        if(tnnode->type_of_node == JUNCTION)set_junction_path(tnnode) ;
      }
/*
 *  5.   Set location of track items as distance along track section
 */
      printf("   Set track item positions\n") ;
      set_track_items_posn(&track_db) ;
#endif
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
      printf("***********************************************************\n") ;
      printf("   Create list of shape files \n") ;
      load_shape_filenames() ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *   2.   Load shape data from *.s and *.sd files
 *        Populate the ShapeNode structures.
 */
      printf("   Read and load shape files\n");
      for(snode=shapelist_beg; snode!=NULL; snode=snode->next){
        if(ip || 0)printf(" data :: shape name = %s :: %s\n",snode->name, snode->s_file) ;
        if(!strcmp(snode->name,"JP1SigGant4"))continue ;
        if(!strncmp(snode->name,"DFu",3))continue ;  //  Chiltern DFu2/3
        if(!strncmp(snode->name,"DAr",3))continue ;  //  Chiltern DArbiter2
        if(load_shape(snode) == 0){
          load_shape_d(snode) ;
          correct_shape(snode) ;
        }
//        if(!strcmp(snode->name,"underground_marker"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"A1tPnt6dLft"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"UKCrossGate_fix"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"Jp1CarCrossing"))print_shape_file_data(snode) ;
//        if(!strcmp_ic(snode->name,"US2BSignal3"))print_shape_file_data(snode) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *    3.  Add shape pointers to the world files.
 *        Each world node includes the name of a 3-D structure.
 *        After this stage it also contains a pointer to the
 *        structure's ShapeNode in memory.
 */
      printf("   Add links to shape items used by each world tile\n");
      add_shape_pointers_to_world_items() ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *    4.  Process forests
 */
      setup_forests() ;
/*
 * *****************************************************************************
 *  Process the signal files
 *  These are read here so that the signal texture file(s) are known.  The files
 *  sigcfg.dat and sigscr.data should be in the main "Route" directory
 *  (ORRoutedir))
 * *****************************************************************************
 */
      printf("***********************************************************\n") ;
      printf(" READ SIGNAL FILES\n") ;
      printf("***********************************************************\n") ;
      printf("   Read signal description file \n");
      read_signals_db() ;
      printf("   Read signal script file \n");
//  Sometimes there are more than one signal script files.  This assumes that
//  only one is valid as in the AU_NSW_SW_SS route
      for(sig_script = raw_signal_db->script_file; sig_script != NULL;
                                                sig_script = sig_script->next){
        load_sigscr_file(ORroutedir,sig_script->scriptfile_name) ;
      }

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
      printf("***********************************************************\n") ;
      printf(" READ TEXTURE FILES\n") ;
      printf("***********************************************************\n") ;
      printf("   Create list of texture filenames \n") ;
      load_texture_filenames() ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *     2.  Read texture files
 *         The data is added to the TextureNode
 */
      printf("   Read and load texture files\n");
      if(ip)printf("   land_texture_default = %s\n",land_texture_default) ;
      for(tx_node=texturelist_beg; tx_node!=NULL; tx_node=tx_node->next){
//        if(!strncmp(tx_node->name,"JP2concwarehse",14))continue ;    //  USA1
        if(load_texture(tx_node))continue ;
        convert_texture(tx_node) ;
/*
 *  Add to btree
 */
          texture_tree = insert_node(texture_tree,tx_node->name,(void *) tx_node) ;
      }
/*
 *  Look for the default terrain.  If missing look for a suitable alternative.
 */
//      dump_btree(texture_tree,0,"X") ;
      btree = NULL ;
      if(land_texture_default)btree = find_btree(texture_tree,land_texture_default) ;
      if(!btree)btree = find_btree(texture_tree,"HW_c1") ;
      if(!btree)btree = find_btree(texture_tree,"BrownScrub") ;
      if(!btree)btree = find_btree(texture_tree,"ACleanTrackBase") ;
      if(!btree)btree = find_btree(texture_tree,"US2ScrubTrans") ;
      if(!btree)btree = find_btree(texture_tree,"terrain") ;
      if(!btree)btree = find_btree(texture_tree,"microtex") ;

      if(!btree){
        printf(" ERROR : Land texture not found\n") ;
        exit(0) ;
      }else{
        land_texture = (TextureNode *)btree->data ;
        printf("   land_texture name = %s\n",land_texture->name) ;
        land_texture->basic = 1 ;
        land_texture->needed = 1 ;
        if(!(land_texture->loaded)){
          load_texture(land_texture) ;
          convert_texture(land_texture) ;
        }
        printf("   land_texture->loaded 2 = %i\n",land_texture->loaded) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *      3.  In each ShapeNode add pointers to the required textures.
 */
      printf("   Add links to textures needed by each shape\n");
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
        printf("***********************************************************\n") ;
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
      printf("   Add textures to track profiles\n");
      add_texture_pointers_to_track_profiles(zr_rail_profile) ;
      printf("   Add textures to road profiles\n");
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
        printf("***********************************************************\n") ;
        make_track_shapes() ;
        make_road_shapes()  ;
        if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
      }
/*
 * *****************************************************************************
 *  Initialise the signal structures.
 * *****************************************************************************
 */
      printf("***********************************************************\n") ;
      printf("  INITIALSE SIGNALS\n") ;
      printf("***********************************************************\n") ;
      printf("   Add links to textures needed by the signals\n");
      add_texture_pointers_to_signals() ;
      printf("   Initialise other raw signal pointers\n");
      setup_raw_signals() ;
//      printf("   ====  sTree = %p\n",(void *)sTree) ;
      printf("   Initialise individual signals\n");
      setup_signals() ;
//      printf("   ====  sTree = %p\n",(void *)sTree) ;
/*
 * *****************************************************************************
 *    Read turntable file and generate a list of turntables
 * *****************************************************************************
 */
      printf("***********************************************************\n") ;
      printf("   INITIALISE TURNTABLES \n");
      printf("***********************************************************\n") ;
      read_turntable_file() ;
      printf("   Setup turntables \n");
      setup_turntables()    ;
      printf("   Setup transfers \n");
      setup_transfers()     ;
/*
 * *****************************************************************************
 *    For each world tile generate a list of level crossings
 * *****************************************************************************
 */
      printf("***********************************************************\n") ;
      printf("   INITIALISE LEVEL CROSSINGS \n");
      printf("***********************************************************\n") ;
      setup_level_crossings() ;
/*
 * *****************************************************************************
 *    Process Environment Files
 * *****************************************************************************
 */
//          NOT DONE YET
//          SOME SHAPE TEXTURES DEPEND ON NIGHT, SEASON, SNOW ETC.
/*
 * *****************************************************************************
 *    Process Sound Files for scenery
 * *****************************************************************************
 */
#ifdef OPENAL
      printf("***********************************************************\n") ;
      printf("  INITIALSE SOUNDS - EXCEPT WAGON SOUNDS\n") ;
      printf("***********************************************************\n") ;
      printf("    Collect and Initialise Sounds\n");
      load_sound_files() ;
#endif
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
 *        file found generate a new RawWagonNode structure and add it
 *        to the linked list pointed to by rawwagonlist_beg (zr.h).
 *        Add *.jpg (side image), *.s (3-D shape) and *.sd
 *        (bounding box etc) files with the same name to the
 *        WagonNode structure.
 *   b)   For each *.ace file found generate a TextureNode
 *        structure and add it to the linked list pointed to by
 *        wtexturelist_beg (zr.h).
 *        Note:  Many wagons will also use other MSTS or route textures.
 * *****************************************************************************
 */
#if 1
      printf("***********************************************************\n") ;
      printf(" PROCESS WAGONS\n") ;
      printf("***********************************************************\n") ;
      printf("   Scan for wagon (*.wag, *.eng) files and the associated  \n") ;
      printf("   shape (*.s, *.sd) and texture files and initialise the  \n") ;
      printf("   wagon and shape structures                              \n") ;
/*
 * *****************************************************************************
 *  1.  Scan for wagons and wagon textures
 *      Add nodes to raw wagon list and wagon texture list
 * *****************************************************************************
 */
      scan_for_wagon_files() ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 * *****************************************************************************
 *  2.  Read wagon files
 *      Create shape data structures
 * *****************************************************************************
 */
      for(rwnode = rawwagonlist_beg; rwnode != NULL; rwnode = rwnode->next){
        create_wagon_shape_node(rwnode) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 * *****************************************************************************
 *   3.  Read consist files
 * *****************************************************************************
 */
      load_consist_files() ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 * *****************************************************************************
 *  4.  Use wagon and consist data to initialise trains
 *      and flag needed wagons
 * *****************************************************************************
 */
      trains_init() ;
/*
 * *****************************************************************************
 *   5.  Load the Shape (*.s) and Additional data (*.sd) files
 * *****************************************************************************
 */
      printf("   Read and load wagon shape files\n");
      for(snode=wshapelist_beg; snode!=NULL; snode=snode->next){
        if(ip){
          printf(" Call load_shape :: shape name = %s\n",snode->name) ;
          printf(" Call load_shape :: shape      = %p\n",(void *)snode) ;
        }
        if(!snode->needed) continue ;
        load_shape(snode) ;
        if(ip)printf(" Call load_shape_d :: sd_file = %s\n",snode->sd_file) ;
        load_shape_d(snode) ;
        mark_needed_wagon_textures(snode) ;
//        if(!strcmp(snode->name,"acelahhl"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"UKRoyalScot"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"dash9"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"4W-CV-load"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"4W-CW2"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"1905-I103"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->nametexturelist_beg,"1905-S654"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"AU_NSW_Dtruck3"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"SMRcrew"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"AAC_DEE_MNR_35001"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"DR_BR_5MT_44839"))print_shape_file_data(snode) ;
//        if(!strcmp(snode->name,"33020"))print_shape_file_data(snode) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 * *****************************************************************************
 *   6.  Load wagon textures
 *       These are textures found in the
 *       The texture data is loaded into the TextureNode structure
 *       ready for use.
 * *****************************************************************************
 */
      printf("   Read and load wagon texture files\n");
      for(tx_node=wtexturelist_beg; tx_node!=NULL; tx_node=tx_node->next){
//        if(!strncmp(tx_node->name,"JP2concwarehse",14))continue ;    //  USA1
        if(!(tx_node->needed)) continue ;
        load_texture(tx_node)    ;
        convert_texture(tx_node) ;
        if(ip)printf(" Texture name = %s\n",tx_node->name) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *    6a.  Sort list
 */
      printf("   Sort textures\n");
      if(ip)printf("   wtexturelist_beg = %p\n",(void *)&wtexturelist_beg) ;
      sort_textures(&wtexturelist_beg) ;
/*
 * *****************************************************************************
 *    7.  Add wagon texture pointers to wagon structures
 *        Each WagonNode structure contains a list of requied
 *        textures.  After this point it also contains pointers to
 *        the corresponding texture structures in memory.
 * *****************************************************************************
 */
      printf("   Add links to textures needed by each wagon shape\n");
      for(snode=wshapelist_beg; snode!=NULL; snode=snode->next){
        if(!(snode->needed)) continue ;
        add_texture_pointers_to_wagon_shapes(snode) ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *   5.  Check wheel radii
 */
#if 0
      printf("   Check wheel radii\n");
      for(rwnode = rawwagonlist_beg; rwnode != NULL ; rwnode=rwnode->next){
        printf(" data :: AA %p\n",(void *)rwnode) ;
        check_wheel_radii(rwnode) ;
        printf(" data :: BB\n") ;
      }
        printf(" data :: CC\n") ;
#endif
#endif

/*
 * *****************************************************************************
 *    Print summary data
 * *****************************************************************************
 */
      printf("***********************************************************\n") ;
      printf("  PRINT SUMMARY DATA\n") ;
      printf("***********************************************************\n") ;
// List tiles
#if 0
{
TileListNode *tlist ;
int   i, j, nn, ni ;
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
}
#endif
/*
 *   List world item
 */
#if 0
      printf("***********************************************************\n") ;
      printf("  List Shapes\n");
      printf("***********************************************************\n") ;
      print_world_item_data(-6074, 14922,2542) ;
      print_world_item_data(-6074, 14922,2544) ;
//      print_world_item_data(0, 0,2109) ;
#endif
// List track sections
//   NOTE node N is in trk_sections_array[N-1]
#if 0
      printf("   List track sections\n");
      for(i=0;i<track_db.trk_sections_array_size;i++){
uint j ;
TrkSector *ts ;
TrkItem *ti ;
//        if(130 != i+1)continue ;
//        if(565 != i+1)continue ;
//        if(377 != i+1)continue ;
        if(534 != i+1)continue ;
        ts = &track_db.trk_sections_array[i] ;

        printf("  Track section %3i :: %3i,  type of item = %2i :: %s\n",
               i,ts->uid,ts->type_of_node,token_trackdb[ts->type_of_node]) ;
        printf("        Junction Node data = %i,%i,%i\n",
                 ts->jn[0],ts->jn[1],ts->jn[2]) ;
        printf("        End Node data       = %i\n",ts->en) ;
        printf("        Number of vectors   = %i,\n",ts->length_of_vector) ;
        printf("        Vector1, World name = %s\n",ts->vector->world_item->filename) ;
        printf("        Number of items     = %i,\n",ts->trk_item_number)  ;
        for(j=0;j<ts->trk_item_number;j++){
          ti = &(track_db.trk_items_array[ts->trk_item_list[j]])    ;
          printf("        Item     = %i : %i :: %i %i\n",
                 j,ts->trk_item_list[j],ti->uid, ti->type_of_node) ;
        }
        printf("        Pin types and links = %i %i :: %i %i %i :: %i %i %i\n",
                  ts->type_of_pin[0],ts->type_of_pin[1],
                  ts->pin_to_section[0],ts->pin_to_section[1],ts->pin_to_section[2],
                  ts->pin_info[0],ts->pin_info[1],ts->pin_info[2]) ;
        printf("        Dyn Node profile    = %p\n",(void *)ts->profile);
        printf("        LOD method          = %i\n",ts->lod_method) ;

#if 0
        if(track_db.trk_sections_array[i].type_of_node == 1){
uint          j, k, uid ;
TrkVectorNode *v ;
WorldItem     *w ;
DynTrackSect  *d ;
TrkSector     *t = &(track_db.trk_sections_array[i]) ;
          for(j=0;j<t->length_of_vector;j++){
            v = &t->vector[j] ;
            w = v->world_item ;
            printf("      %3i :: %5i  %5i  :: %i %i :: %i %i :: %9.3f %9.3f %9.3f :: %i %i :: %9.3f %9.3f %9.3f :: %4i %3i :: %s",j,
                   v->tsec_section_index,v-> tsec_shape_index,
                   v->flag1, v->flag2,
                   v->tile_east_x,v->tile_north_z,
                   v->east_x,v->north_z,v->height_y,
                   v->wfname_east_x,v->wfname_north_z,
                   (double)w->X,(double)w->Y,(double)w->Z,
                   w->uid,w->worldtype,w->filename) ;
             if(w->worldtype==306){
               for(k=0;k<5;k++){
                 d = &w->u.dyn_track_obj.dyn_trk_sect[k] ;

                 printf(" %i",d->uid);
                 if(d->uid!=-1)printf(" (%i %.2f %.1f)",d->is_curved, d->param_1,d->param_2);
               }
             }
             printf("\n") ;
          }
  #if 0
          printf("\n==========================================\n");
          for(j=0;j<t->length_of_vector;j++){
            v = &t->vector[j] ;
            w = v->world_item ;
            printf("      %3i :: %5i  %5i  :: %i %i : %i %i :: %c %c :: len ="
                   "  %8.3f,  rad = %8.3f, ang = %6.3f :: %4i %3i :: %s",j,
                   v->tsec_section_index,v-> tsec_shape_index,
                   v->flag1, v->flag2, v->is_curved, v->is_dynamic,
                   v->string[0],v->string[1],
                   v->length,v->radius,v->angle,
                   w->uid,w->worldtype,w->filename) ;
             if(w->worldtype==306){
               for(k=0;k<5;k++){
                 d = &w->u.dyn_track_obj.dyn_trk_sect[k] ;
                 printf(" %i",d->uid);
                 if(d->uid!=-1)printf(" (%i %.2f %.1f)",d->is_curved, d->param_1,d->param_2);
               }
             }
             printf("\n") ;
           }
 #endif
        }
#else
         list_track_section(&track_db.trk_sections_array[i]) ;
#endif
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
#endif
// List track items
#if 0
      printf("   List track items\n");
      for(i=0;i<track_db.trk_items_array_size;i++){
        tinode = &track_db.trk_items_array[i] ;
        printf("    Track item %3i,  type of item = %2i :: %s",
               i,tinode->type_of_node,token_trackdb[tinode->type_of_node]) ;
        if(tinode->type_of_node == PICKUP)printf(" :: %i  %s",
                                tinode->pickup_data1,tinode->pickup_data2) ;
        printf("\n") ;
      }
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
#endif
/*
 *   List and/or print shape data
 *   Shape 'test_shape' is defined in file 'zr.c'.
 */
#if 0
uint       j, k;
LodControl *lod_control ;
DistLevel  *dist_level  ;
      printf("***********************************************************\n") ;
      printf("  List Shapes\n");
      printf("***********************************************************\n") ;
      printf("       Test shape = %s\n",test_shape);
      for(snode=shapelist_beg,i=0; snode!=NULL; snode=snode->next,i++){
        printf("   Shape : %3i : %s\n",i,snode->name);
# if 0
        printf("        nvolumes = %3i, n_lod_controls = %3i\n",
             snode->nvolumes,snode->n_lod_controls) ;
        for(i=0;i<snode->nvolumes;i++){
          printf("    %i  radius = %f,  position = %f %f %f\n",i,
              snode->shape_vol[i].radius,snode->shape_vol[i].vec.X,
              snode->shape_vol[i].vec.Y,snode->shape_vol[i].vec.Z) ;
        }
        for(i=0;i<snode->n_lod_controls;i++){
          lod_control = &snode->lod_control[i] ;
          printf("    %i,  n_dist_levels = %i\n",i,lod_control->n_dist_levels) ;
          dist_level = lod_control->dist_level ;
          for(j=0;j<lod_control->n_dist_levels;j++){
            printf("    %i, %i  n_sub_objects = %i\n",i, j, dist_level[j].n_sub_objects) ;
            for(k=0;k<dist_level[j].n_sub_objects;k++){
              printf("    %i, %i, %i vol_idx = %i\n",i,j,k,dist_level[j].sub_object[k].vol_idx) ;
            }
          }
        }
# endif
        if(snode->name && 0 == strcmp_ic(snode->name,test_shape)){
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
      for(tx_node=texturelist_beg,i=0; tx_node!=NULL; tx_node=tx_node->next,i++){
        printf("     Texture : %3i : %s\n",i,tx_node->name);
      }
      printf("   List wagon texture files\n");
      for(tx_node=wtexturelist_beg,i=0; tx_node!=NULL; tx_node=tx_node->next,i++){
        printf("     Texture : %3i : %24s : %2x %2x : %2i : %i %i %i : %4i %4i : %4i %4i %i\n",
               i,tx_node->name, tx_node->surface_format, tx_node->surface_format_orig,
               tx_node->n_textures, tx_node->is_alpha, tx_node->is_mask, tx_node->alpha_is_mask,
               tx_node->width, tx_node->height,
               tx_node->gl_mem_format, tx_node->gl_mem_packing, tx_node->gl_tex_ref_no) ;
      }
#endif
/*
 *  List Platforms and Sidings
 */
#if 0
      printf("***********************************************************\n") ;
      printf("  List Platforms and Sidings\n");
      printf("***********************************************************\n") ;
      list_platforms_and_sidings() ;
#endif
/*
 *  List consists
 */
#if 0
      {
ConsistNode *consist ;

        printf("***********************************************************\n") ;
        printf("  List Consists\n");
        printf("***********************************************************\n") ;
        for(consist = consistlist_beg; consist != NULL; consist = consist->next){
          printf("    Consist = %s\n",consist->name) ;
        }
      }
#endif
/*
 *  List sidings
 */
#if 0
      {
uint     i ;
TrkItem *ti ;
        printf("***********************************************************\n") ;
        printf("  List Platforms and Sidings\n");
        printf("***********************************************************\n") ;
        for(i=0; i<track_db.trk_items_array_size; i++){
          ti = &(track_db.trk_items_array[i]) ;
          if(ti->type_of_node == PLATFORM  && ti->platform_data1[0]!='f'){
            printf("    Platform name = '%s'\n",ti->platform_name) ;
          }else if(ti->type_of_node == SIDING  && ti->siding_data1[0]!='f'){
            printf("    Siding name   = '%s'\n",ti->siding_name) ;
          }
        }
      }
#endif
/*
 * List Signal data
 */
#if 0
    for(signal = signallist_beg; signal != NULL; signal = signal->next){
      print_signal_data(signal) ;
    }
#endif
/*
 * List Tile data
 */
#if 0
      ip = 0 ;
      printf("***********************************************************\n") ;
      printf("  List Tiles\n");
      printf("***********************************************************\n") ;
      if(ip){
  int   i, j, k, kmin, kmax, n ;
  unsigned short  *kp ;
        printf(" List tile data\n") ;
        for(tl_node = tilelist_head;tl_node!=NULL;tl_node=tl_node->next){
          if(tl_node->t_found==0) continue ;
          printf("\n");
          printf("  File      = %s\n",tl_node->t_file);
          printf("    Ybuffer = %s\n",tl_node->terrain_data.ybuffer);
          for(i=0;i<tl_node->terrain_data.number_of_shaders;i++){
            printf("    Shader %i,  Name = %s\n",i,tl_node->terrain_data.tile_shader[i].name);
          }

          n = tl_node->terrain_data.terrain_nsamples ;
          printf("    nsamples = %i\n",n) ;
          kp = tl_node->terrain_data.elevations      ;
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


          i = tl_node->tilex - tile_west  ;
          j = tl_node->tiley - tile_south ;
          n = j*(tile_east + 1 - tile_west) + i ;
          printf(" i, j, n = %i %i %i\n",i,j,n);
          printf(" Check node1 = %p\n",(void *)tl_node);
          printf(" Check node2 = %p\n",(void *)tile_array[n]);
        }
      }
#endif
/*
 * List BTree Data
 */
#if 0
      printf("***********************************************************\n") ;
      printf("  List BTree Data\n");
      printf("***********************************************************\n") ;
      printf("  BTree 'dir_master'  %p\n",(void *)dir_master);
      dump_btree(dir_master,0,"0") ;
#endif
      printf("    tile_west  = %i\n",tile_west) ;
      printf("    tile_east  = %i\n",tile_east) ;
      printf("    tile_north = %i\n",tile_north) ;
      printf("    tile_south = %i\n",tile_south) ;

      printf("    Number of signals    = %i\n", n_signals)    ;
      printf("    Number of transfers  = %i\n", n_transfers)  ;
      printf("    Number of turntables = %i\n", n_turntables) ;
      printf("    Number of level crossings = %i\n", n_levelcrossings) ;
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *  List wagons
 */

//      print_wagon_data() ;
      print_wagon_data_to_file("wagonlist.txt") ;

      printf("  Exit from %s\n",my_name) ;
      fflush(NULL) ;

      return 0 ;
}

/*
 *  Read the project (.trk) file and populate the project database
 *     The database is stored in a PdbNode structure
 *     For the moment only one pdb is supported, stored as the
 *     first item of a linked list, and pointed to by pdb_beg
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

      if(ip)printf("\n   Enter init_pdb\n\n");

      full_filename = (char *)malloc(strlen(ORroutedir)+strlen(filename)+2);
      strcpy(full_filename,ORroutedir);
      strcat(full_filename,"/");
      strcat(full_filename,filename);
      if(ip)printf("    filename = %s\n",full_filename);

      l = open_msfile(full_filename, &msfile, 0, 0);
      if(l!=0){
        printf("\n\n  ERROR : Routine open_msfile failed to open file\n");
        printf("      File = %s\n",full_filename) ;
        exit(1) ;
      }
      if(ip)printf("    msfile pointer  = %p\n\n",(void *)msfile.fp);
/*
 *  Create new trknode
 */
//      pdb = (PdbNode *)malloc(sizeof(PdbNode));
      pdb = &project_db ;
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
              break ;
          CASE ("Graphic")
              pdb->Graphic = ctoken(&msfile) ;
              break ;
          CASE ("LoadingScreen")
              pdb->LoadingScreen = ctoken(&msfile) ;
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
      return trk_file ;
}

