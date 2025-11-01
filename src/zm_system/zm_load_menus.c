/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  zm_load_menus.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Menu routines used by ZM, the front end to ZR.
 *
 *==============================================================================
 */

int zm_load_route_menus(void){

  int    ip = 0 ;
  int    i      ;
  BTree *bt1    ;
  char  *my_name = "zm_load_route_menus" ;

      if(ip)printf(" Enter routine %s\n",my_name) ;
      if(ip){
        printf("  ORdir      = %s\n",ORdir) ;
        printf("  ORroute    = %s\n",ORroute) ;
        printf("  ORroutedir = %s\n",ORroutedir) ;
        printf("  MSTSdir    = %s\n",MSTSdir) ;
      }
/*
 *  generate Btree of routes and consists
 */
      bt_clear_malloc_data(ORroutes) ;    ORroutes    = NULL ;
      bt_clear_malloc_data(ORconsists) ;  ORconsists  = NULL ;
      bt_clear_malloc_data(ORplatforms) ; ORplatforms = NULL ;

     if(ip){
        printf("  ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ\n") ;
        printf("  ORdirs      = %p\n", (void *)ORdirs) ;
        print_bt_nodes_with_count_and_index(ORdirs);
        printf("  ORroutes    = %p\n", (void *)ORroutes) ;
        print_bt_nodes_with_count_and_index(ORroutes);
        printf("  ORconsists  = %p\n", (void *)ORconsists) ;
        print_bt_nodes_with_count_and_index(ORconsists);
        printf("  ORplatforms = %p\n", (void *)ORplatforms) ;
        print_bt_nodes_with_count_and_index(ORplatforms);
     }
      ORroutes = find_route_dirs() ;
      n_orroutes = return_bt_count() ;
      ORconsists = zm_find_consists() ;
      n_consists = return_bt_count() ;
/*
 *  platforms can only be listed once one of the routes has been chosen
 *  This may occur once the route menu has been setup and the initial
 *  displayed route set or late when a route is displayed.
 */
      if(ip || 1){
        printf("  Number of ORroute directories = %i\n",n_orroutes) ;
        printf("  ORroutes = %p\n",(void *)ORroutes) ;
        printf("    List of routes:\n") ;
        print_bt_nodes_with_count_and_index(ORroutes) ;
        printf("  Number of consists = %i\n",n_consists) ;
        printf("  ORconsists = %p\n",(void *)ORconsists) ;
        printf("    List of consists:\n") ;
        print_bt_nodes_with_count_and_index(ORconsists) ;
        printf("  Number of platforms = %i\n",n_platforms) ;
        printf("  ORplatforms = %p\n",(void *)ORplatforms) ;
        printf("    List of platforms:\n") ;
        print_bt_nodes_with_count_and_index(ORplatforms) ;
      }
/*
 *   Fill combo box 2 - this is the list of routes
 */
      combo2_active = 0 ;
      gtk_combo_box_text_remove_all((GtkComboBoxText*)combo2) ;
      for(i=1;i<n_orroutes+1;i++){
        bt1 = find_bt_node_with_index(ORroutes, i) ;
        if(bt1) gtk_combo_box_text_append_text ((GtkComboBoxText*) combo2,
                                                   (const gchar*) bt1->index ) ;
      }
      gtk_combo_box_set_active ((GtkComboBox*) combo2, (gint) 0) ;
      combo2_active = 1 ;
      if(ip)printf("  ZM End of combo2 - route menu setup\n") ;
/*
 *  Set route icon and route text for first route
 */
      zm_set_route_icon_and_text((gint)0) ;
/*
 *  Set platforms for first route
 */
      ORplatforms = zm_find_platforms();
      n_platforms = return_bt_count() ;
        printf("  Number of platforms = %i\n",n_platforms) ;
        printf("  ORplatforms = %p\n",(void *)ORplatforms) ;
        printf("    List of platforms:\n") ;
        print_bt_nodes_with_count_and_index(ORplatforms) ;
/*
 *   Fill combo box 6 - this is the list of platforms
 */
      combo6_active = 0 ;
      gtk_combo_box_text_remove_all((GtkComboBoxText*)combo6) ;
      for(i=1;i<n_platforms+1;i++){
        bt1 = find_bt_node_with_index(ORplatforms, i) ;
        if(bt1) gtk_combo_box_text_append_text ((GtkComboBoxText*) combo6,
                                                   (const gchar*) bt1->index ) ;
      }
      gtk_combo_box_set_active ((GtkComboBox*) combo6, (gint) 0) ;
      combo6_active = 1 ;
      bt1 = find_bt_node_with_index(ORplatforms, 1) ;
      ORplatform = bt1->index ;
      if(ip)printf("  ZM End of combo6 - route menu setup\n") ;

      return 0;
}

/*
 *   Routine to load and display the icons and data for a chosen route
 */

int zm_set_route_icon_and_text(gint val){

  int    ip = 0 ;
  int    n      ;
  BTree *bt1    ;
  GdkPixbuf *pixbuf1, *pixbuf2 ;

  char *my_name = "zm_set_route_icon_and_text" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;

      bt1 = find_bt_node_with_index(ORroutes, (int) val+1) ;
      if(ip){
        printf("  New top route = %s\n",bt1->index) ;
        printf("  New top route = %s\n",bt1->data) ;
      }
      if(ORroute)    free (ORroute) ;
      if(ORrouteace) free (ORrouteace) ;
      ORroutedir = bt1->data  ;             // Do not free(dt1->data) !!
      ORroute    = find_route_name(ORroutedir);
      if(ip){
        printf(" ORroutedir = %s\n", ORroutedir) ;
        printf(" ORroute    = %s\n", ORroute) ;
      }
/*
 *==============================================================================
 * Read the project (*.trk) file for the route and
 * populate the project database structure
 *==============================================================================
 */
      init_pdb() ;
      if(ip){
        printf("  Route = %s\n", ORroute) ;
        pdb_print(project_db) ;
      }

      n = strlen(ORroutedir) + strlen(project_db->Graphic) + 2 ;
      ORrouteace = (char *) malloc(n) ;
      strcpy(ORrouteace, ORroutedir) ;
      strcat(ORrouteace, "/")     ;
      strcat(ORrouteace, project_db->Graphic) ;
      pixbuf1 = zm_load_image_from_ace_file(ORrouteace) ;

//    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf1, 177, 142, GDK_INTERP_NEAREST) ;
      pixbuf2 = gdk_pixbuf_scale_simple(pixbuf1, 177, 142, GDK_INTERP_BILINEAR) ;
      image1 = gtk_image_new_from_pixbuf (pixbuf2);
//      gtk_container_add (GTK_CONTAINER (fixed1), image1);
//      gtk_widget_show(image1);
      gtk_container_add (GTK_CONTAINER (fixed1), image1);
      gtk_widget_show(image1);

      gtk_fixed_move (GTK_FIXED(fixed1), image1, 360, 60);

      gtk_text_buffer_set_text(TextBuffer,
                         (const gchar *) project_db->Description2, (gint) -1);
/*
 *  Set the platforms
 * BTree   *ORdirs     = NULL ;       //  BTree containing top directories
BTree   *ORroutes   = NULL ;       //  BTree containing routes
BTree   *ORconsists = NULL ;       //  BTree containing consists
BTree   *ORplatforms = NULL ;      //  BTree containing platforms

 */
      if(ip){
        printf("  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n") ;
        printf("  ORdirs      = %p\n", (void *)ORdirs) ;
        print_bt_nodes_with_count_and_index(ORdirs);
        printf("  ORroutes    = %p\n", (void *)ORroutes) ;
        print_bt_nodes_with_count_and_index(ORroutes);
        printf("  ORconsists  = %p\n", (void *)ORconsists) ;
        print_bt_nodes_with_count_and_index(ORconsists);
        printf("  ORplatforms = %p\n", (void *)ORplatforms) ;
        print_bt_nodes_with_count_and_index(ORplatforms);
      }
      ORplatforms = zm_find_platforms();
      n_platforms = return_bt_count() ;
      if(ip){
        printf("  BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n") ;
        printf("  ORdirs      = %p\n", (void *)ORdirs) ;
        print_bt_nodes_with_count_and_index(ORdirs);
        printf("  ORroutes    = %p\n", (void *)ORroutes) ;
        print_bt_nodes_with_count_and_index(ORroutes);
        printf("  ORconsists  = %p\n", (void *)ORconsists) ;
        print_bt_nodes_with_count_and_index(ORconsists);
        printf("  ORplatforms = %p\n", (void *)ORplatforms) ;
        print_bt_nodes_with_count_and_index(ORplatforms);
      }
      if(ip)printf("  Exit  routine %s\n",my_name) ;

      return 0 ;
}

/*
 *   Fill combo box 5 (Consist)
 */

int zm_load_consist_menu(void){

  int   ip = 0 ;
  ConsistNode *consist ;
  char *my_name = "zm_load_consist_menu" ;

      if(consistlist_beg)consistlist_beg = NULL ;
      zm_load_consist_files() ;
      if(ip){
        for(consist = consistlist_beg; consist!= NULL; consist = consist->next){
          printf("  Consist name = %s\n",consist->name) ;
        }
      }
      combo5_active = 0 ;
      gtk_combo_box_text_remove_all((GtkComboBoxText*)combo5) ;
      for(consist = consistlist_beg; consist!= NULL; consist=consist->next){
        gtk_combo_box_text_append_text ((GtkComboBoxText*) combo5,
                                            (const gchar*) consist->name ) ;
      }
      gtk_combo_box_set_active ((GtkComboBox*) combo5, (gint) 0) ;
      combo5_active = 1 ;

      return 0 ;
}
