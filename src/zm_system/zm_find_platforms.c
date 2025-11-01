/*
 *******************************************************************************
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File zm_init_platforms.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 * *****************************************************************************
 */

/*
 *  Platforms are specified in the track database file.  This is in the
 *  main directory of each route and has a name of the form 'name.tdb'.
 *  The 'name' part if defined in the project file (the line with the
 *  token FileName = name).  This is usually just the anme of the route
 *  even in the standard MSTS files this is not always the case.
 *
 *  The routine first opens and loads the project file and then loads
 *  the track database file.  Finally it scans the resulting structure
 *  for the filenames.
 *
 *  A bit long-winded ... .
 */

BTree  *zm_find_platforms(void){

  uint  ip = 0   ;  //  0 = no printing
  uint  i, n        ;
  int   iret,
        type_of_node ;
  char *tdb_file ;
  char *rdb_file ;
  TrkItem *trk_item ;
  BTree *bt_plat = ORplatforms ;

  char *my_name = "zm_find_platforms" ;

      if(ip){
        printf("***********************************************************\n") ;
        printf(" Enter %s\n",my_name) ;
        printf("  ORdirs      = %p\n", (void *)ORdirs) ;
        print_bt_nodes_with_count_and_index(ORdirs);
        printf("  ORroutes    = %p\n", (void *)ORroutes) ;
        print_bt_nodes_with_count_and_index(ORroutes);
        printf("  ORconsists  = %p\n", (void *)ORconsists) ;
        print_bt_nodes_with_count_and_index(ORconsists);
        printf("  ORplatforms = %p\n", (void *)ORplatforms) ;
        print_bt_nodes_with_count_and_index(ORplatforms);
      }
      enum_btree_init() ;
      if(ip){
        printf("  ORdir      = %s\n",ORdir) ;
        printf("  ORroute    = %s\n",ORroute) ;
        printf("  ORroutedir = %s\n",ORroutedir) ;
        printf("  MSTSdir    = %s\n",MSTSdir) ;
      }
/*
 *  Read project data base into structure 'project_db'
 */
      printf("   Read OR project file\n");
      init_pdb() ;
/*
 *  construct name of track database file
 */
      n = strlen(project_db->FileName) + 5;
      tdb_file = (char *)malloc(n) ;
      rdb_file = (char *)malloc(n) ;
      strcpy(tdb_file,project_db->FileName) ; strcat(tdb_file,".tdb") ;
      strcpy(rdb_file,project_db->FileName) ; strcat(rdb_file,".rdb") ;
      printf("   tdb_file = %s\n",tdb_file) ;
      printf("   rdb_file = %s\n",rdb_file) ;

//   Check that all files have been close
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;
/*
 *  Read the track database (*.tdb)
 *
 *      The data is stored in track_db (zr.h), a structure of
 *      type TrkDataBase, which contains 1-D arrays of track sections
 *      (positions of individual sections of track) and track items
 *      (positions of sidings, speedposts, crossovers, platforms etc.)
 */
      printf("   Read track database file\n");
      zm_init_track_db(tdb_file);
      if(n_open_files>0)printf("    n_open_files = %i\n",n_open_files) ;

//  Make list of platforms
      reset_bt_count() ;
      bt_plat = NULL ;
      for(i=0;i<(int)track_db.trk_items_array_size;i++){
        trk_item = &track_db.trk_items_array[i] ;
        type_of_node = trk_item->type_of_node ;
        if(PLATFORM != type_of_node)continue ;
        if(!bt_plat || !find_btree(bt_plat,trk_item->platform_name)){
            bt_plat = insert_node(bt_plat,strdup(trk_item->platform_name),NULL) ;
            increment_bt_count((void *)bt_plat) ;
          }
      }
      if(ip){
        printf("  ORdirs      = %p\n", (void *)ORdirs) ;
        print_bt_nodes_with_count_and_index(ORdirs);
        printf("  ORroutes    = %p\n", (void *)ORroutes) ;
        print_bt_nodes_with_count_and_index(ORroutes);
        printf("  ORconsists  = %p\n", (void *)ORconsists) ;
        print_bt_nodes_with_count_and_index(ORconsists);
        printf("  ORplatforms = %p\n", (void *)ORplatforms) ;
        print_bt_nodes_with_count_and_index(ORplatforms);
      }
      return bt_plat ;
}
