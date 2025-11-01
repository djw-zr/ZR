/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+
#+     Glade / Gtk Programming
#+
#+     Copyright (C) 2019 by Kevin C. O'Kane
#+
#+     Kevin C. O'Kane
#+     kc.okane@gmail.com
#+     https://www.cs.uni.edu/~okane
#+     http://threadsafebooks.com/
#+
#+ This program is free software; you can redistribute it and/or modify
#+ it under the terms of the GNU General Public License as published by
#+ the Free Software Foundation; either version 2 of the License, or
#+ (at your option) any later version.
#+
#+ This program is distributed in the hope that it will be useful,
#+ but WITHOUT ANY WARRANTY; without even the implied warranty of
#+ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#+ GNU General Public License for more details.
#+
#+ You should have received a copy of the GNU General Public License
#+ along with this program; if not, write to the Free Software
#+ Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#+
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define USE_ZM_INIT

#include "zm.h"

int  zr_flag = 0 ;        //  Set to '1' to request zr

bool application_activate (GtkApplication* app, gpointer user_data);
void box_add              (GtkWidget *parent_box, char *class, char *text);
void css_add              (char *css);
void label_add            (GtkWidget *parent_box, char *text);

char  *pingtotal; // pointer shared memory
int   *pingFlag;
void  on_destroy();

int   countDownMax, countCurrent;
int   currentRow;
char  rows[5][10] =
            { "row 1", "row 2", "row 3", "row 4", "row 5" };

char  tmp[1024]; // general use

gboolean timer_handler(GtkWidget *);

void  on_changed_text(GtkTextBuffer *t);

int main(int argc, char *argv[]) {

  int    ip = 1 ;
  int    i, n_dirs ;
  BTree *bt1 ;
  char  *my_name = "ZM  main routine" ;

      if(ip){
        printf("***********************************************************\n") ;
        printf(" Enter %s\n",my_name) ;
      }
#ifdef   USE_ZM_INIT
      zm_init(argc, argv);
#else
      gtk_init(&argc, &argv); // init Gtk

//---------------------------------------------------------------------
// establish contact with xml code used to adjust widget settings
//---------------------------------------------------------------------

      builder = gtk_builder_new_from_file ("ZG.glade");

      window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

      g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

      gtk_builder_connect_signals(builder, NULL);

/*
 *   Add the css information
 */
  {
    /* CSS */
    GError *error = NULL;
    GtkStyleContext *context;
    GtkCssProvider *provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_data (provider, css, strlen (css), &error);
    if (error != NULL)
        {
        fprintf (stderr, "CSS: %s\n", error->message);
        }
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
  }

      fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));

#if 1
      button1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
      button2 = GTK_WIDGET(gtk_builder_get_object(builder, "button2"));
      button3 = GTK_WIDGET(gtk_builder_get_object(builder, "button3"));
      button4 = GTK_WIDGET(gtk_builder_get_object(builder, "button4"));
#endif
      label1 = GTK_WIDGET(gtk_builder_get_object(builder, "label1"));
      label2 = GTK_WIDGET(gtk_builder_get_object(builder, "label2"));
//      label3 = GTK_WIDGET(gtk_builder_get_object(builder, "label3"));

      radio1 = GTK_WIDGET(gtk_builder_get_object(builder, "radio1"));
      radio2 = GTK_WIDGET(gtk_builder_get_object(builder, "radio2"));
#if 0
      radio3 = GTK_WIDGET(gtk_builder_get_object(builder, "radio3"));
      check1 = GTK_WIDGET(gtk_builder_get_object(builder, "check1"));
      toggle1 = GTK_WIDGET(gtk_builder_get_object(builder, "toggle1"));
      switch1 = GTK_WIDGET(gtk_builder_get_object(builder, "switch1"));
#endif
      combo1 = GTK_WIDGET(gtk_builder_get_object(builder, "combo1"));
      entry1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry1"));
      combo2 = GTK_WIDGET(gtk_builder_get_object(builder, "combo2"));
      entry2 = GTK_WIDGET(gtk_builder_get_object(builder, "entry2"));
      combo3 = GTK_WIDGET(gtk_builder_get_object(builder, "combo3"));
      entry3 = GTK_WIDGET(gtk_builder_get_object(builder, "entry3"));
      combo4 = GTK_WIDGET(gtk_builder_get_object(builder, "combo4"));
      entry4 = GTK_WIDGET(gtk_builder_get_object(builder, "entry4"));
      combo5 = GTK_WIDGET(gtk_builder_get_object(builder, "combo5"));
      entry5 = GTK_WIDGET(gtk_builder_get_object(builder, "entry5"));
      combo6 = GTK_WIDGET(gtk_builder_get_object(builder, "combo6"));
      entry6 = GTK_WIDGET(gtk_builder_get_object(builder, "entry6"));
      combo7 = GTK_WIDGET(gtk_builder_get_object(builder, "combo7"));
      entry7 = GTK_WIDGET(gtk_builder_get_object(builder, "entry7"));
      combo8 = GTK_WIDGET(gtk_builder_get_object(builder, "combo8"));
      entry8 = GTK_WIDGET(gtk_builder_get_object(builder, "entry8"));
      combo9 = GTK_WIDGET(gtk_builder_get_object(builder, "combo9"));
      entry9 = GTK_WIDGET(gtk_builder_get_object(builder, "entry9"));
      combo10 = GTK_WIDGET(gtk_builder_get_object(builder, "combo10"));
      entry10 = GTK_WIDGET(gtk_builder_get_object(builder, "entry10"));
      combo11 = GTK_WIDGET(gtk_builder_get_object(builder, "combo11"));
      entry11 = GTK_WIDGET(gtk_builder_get_object(builder, "entry11"));
      combo12 = GTK_WIDGET(gtk_builder_get_object(builder, "combo12"));
      entry12 = GTK_WIDGET(gtk_builder_get_object(builder, "entry12"));

      stack1  = GTK_WIDGET(gtk_builder_get_object(builder, "stack1"));
      stkfxd1 = GTK_WIDGET(gtk_builder_get_object(builder, "stkfxd1"));
      stkfxd2 = GTK_WIDGET(gtk_builder_get_object(builder, "stkfxd2"));


      frame5 = GTK_WIDGET(gtk_builder_get_object(builder, "frame5"));
      ali5   = GTK_WIDGET(gtk_builder_get_object(builder, "ali5"));
      fixed5 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed5"));

      frame6 = GTK_WIDGET(gtk_builder_get_object(builder, "frame6"));

      gtk_frame_set_shadow_type(GTK_FRAME(frame5),GTK_SHADOW_IN) ;
      gtk_frame_set_shadow_type(GTK_FRAME(frame6),GTK_SHADOW_IN) ;

#if 0
      spin1 = GTK_WIDGET(gtk_builder_get_object(builder, "spin1"));
      color1 = GTK_WIDGET(gtk_builder_get_object(builder, "color1"));
      font1 = GTK_WIDGET(gtk_builder_get_object(builder, "font1"));
      file1 = GTK_WIDGET(gtk_builder_get_object(builder, "file1"));
      volume1 = GTK_WIDGET(gtk_builder_get_object(builder, "volume1"));
      frame1 = GTK_WIDGET(gtk_builder_get_object(builder, "frame1"));
      frame2 = GTK_WIDGET(gtk_builder_get_object(builder, "frame2"));
      frame3 = GTK_WIDGET(gtk_builder_get_object(builder, "frame3"));
      frame4 = GTK_WIDGET(gtk_builder_get_object(builder, "frame4"));
      scroll1 = GTK_WIDGET(gtk_builder_get_object(builder, "scroll1"));
      image2 = GTK_WIDGET(gtk_builder_get_object(builder, "image2"));
      timer = GTK_WIDGET(gtk_builder_get_object(builder, "timer"));
      menu1 = GTK_WIDGET(gtk_builder_get_object(builder, "menu1"));
      new = GTK_WIDGET(gtk_builder_get_object(builder, "new"));
      vc1 = GTK_WIDGET(gtk_builder_get_object(builder, "vc1"));
      prog1 = GTK_WIDGET(gtk_builder_get_object(builder, "prog1"));
      start1 = GTK_WIDGET(gtk_builder_get_object(builder, "start1"));
      PingTime = GTK_WIDGET(gtk_builder_get_object(builder, "PingTime"));
      PingTimeDisplay = GTK_WIDGET(gtk_builder_get_object(builder, "PingTimeDisplay"));
      ScrollWindow = GTK_WIDGET(gtk_builder_get_object(builder, "ScrollWindow"));
      ViewPort = GTK_WIDGET(gtk_builder_get_object(builder, "ViewPort"));
      TextView = GTK_WIDGET(gtk_builder_get_object(builder, "TextView"));
      saveText = GTK_WIDGET(gtk_builder_get_object(builder, "saveText"));
#endif
      liststore1 = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));
      adjustment2 = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "adjustment2"));
#if 0
//      Note include of <sys/mman.h> at beginning

        pingtotal = mmap(NULL, 512, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0); // shared memory

      strcpy(pingtotal,"Tot Play Time: <empty>");

        pingFlag = mmap(NULL, 32, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0); // shared memory

      *pingFlag = 1;
#endif

      /*******************
      struct GtkAdjustment {
                    gdouble lower,
                    gdouble upper,
                    gdouble step_increment,
                    gdouble page_increment,
                    gdouble page_size};
      ****************************/

/* alter the range of the scroll bar */
#if 0
        gtk_range_set_range (GTK_RANGE(scroll1), 0, 100);
#endif
        GdkColor color; // default background color
        color.red = 0xcccc;
        color.green = 0xcccc;
        color.blue = 0xe000;
        gtk_widget_modify_bg(GTK_WIDGET(window), GTK_STATE_NORMAL, &color);

//        gtk_container_set_border_width (GTK_CONTAINER(frame5), 5) ;

        color.red = 0xeeee;
        color.green = 0xeeee;
        color.blue = 0xaaaa;

//        gtk_widget_modify_bg(frame5, GTK_STATE_NORMAL, &color);
#if 0
        color.red = 0xaaaa;
        color.green = 0xaaaa;
        color.blue = 0xeeee;
        gtk_widget_modify_bg(frame1, GTK_STATE_NORMAL, &color);

        color.red = 0xaaaa;
        color.green = 0xeeee;
        color.blue = 0xaaaa;
        gtk_widget_modify_bg(frame2, GTK_STATE_NORMAL, &color);

        color.red = 0xeeee;
        color.green = 0xaaaa;
        color.blue = 0xaaaa;
        gtk_widget_modify_bg(frame3, GTK_STATE_NORMAL, &color);

        color.red = 0xeeee;
        color.green = 0xeeee;
        color.blue = 0xaaaa;

        gtk_widget_modify_bg(frame4, GTK_STATE_NORMAL, &color);

      image1 = NULL;

//      The following function returns an ID of the event that can be
//      used to destroy it. Not used here.

      guint ID = g_timeout_add_seconds(1, (GSourceFunc) timer_handler, timer);

//      Time units smaller than seconds are possible (higher overhead):
//      guint ID = g_timeout_add(250, (GSourceFunc) timer_handler, data_pointer);

      countCurrent = countDownMax = 0;

//      label3 & button4 scroll and background setup

      currentRow = 0;
      gtk_label_set_text (GTK_LABEL(label3), (const gchar* ) rows[currentRow]);

        color.red = 0xdddd;
        color.green = 0xcccc;
        color.blue = 0xd900;
        gtk_widget_modify_bg(label3, GTK_STATE_NORMAL, &color); // label background

      gtk_widget_set_events(button4, GDK_SCROLL_MASK); // allowed events.
                                              // see GdkEventScroll data
                                          // structure for valid elements

//-----------------------------------

      TextBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(TextView));

      g_signal_connect(TextBuffer, "changed", G_CALLBACK(on_changed_text), NULL);

//      line of text from the GTK online manual:

      strcpy(tmp,"Because of this, iterators can't be used to preserve positions across buffer modifications.\
 \n\nTo preserve a position, the GtkTextMark object is ideal. \n\nYou can think of a mark as an invisible cursor \
or insertion point; it floats in the buffer, saving a position. \n\nIf the text surrounding the mark is deleted, \
the mark remains in the position the text once occupied; if text is inserted at the mark, the mark ends up \
either to the left or to the right of the new text, depending on its gravity. \n\nThe standard text cursor in \
left-to-right languages is a mark with right gravity, because it stays to the right of inserted text.");

      gtk_text_buffer_set_text (TextBuffer, (const gchar *) tmp, (gint) -1);

      gtk_widget_hide(saveText);
#endif
//-----------------------------------
#endif
/*
 * Add my 'extras'
 */
/*
 *  Call routine to initialise top level directory menu (combo1)
 */
      ORdirs = find_top_dirs();
      n_ordirs = return_bt_count() ;
      if(ip){
        printf("  Number of ORdir directories = %i\n",n_ordirs) ;
        printf("    List of directories:\n") ;
        print_bt_nodes_with_count_and_index(ORdirs) ;
      }
/*
 *   Fill combo box 1.
 */
      gtk_combo_box_text_remove_all ((GtkComboBoxText*)combo1) ;
      for(i=1;i<n_ordirs+1;i++){
        bt1 = find_bt_node_with_index(ORdirs, i) ;
        if(bt1) gtk_combo_box_text_append_text ((GtkComboBoxText*) combo1,
                                                   (const gchar*) bt1->index ) ;
        if(i==1){
          ORdname = bt1->index ;
          ORdir   = bt1->data  ;
        }
      }
      gtk_combo_box_set_active ((GtkComboBox*) combo1, (gint) 0) ;
      printf("  ORdname = %s\n", ORdname) ;
      printf("  ORdir   = %s\n", ORdir)   ;
#if 1
      zm_load_route_menus() ;
      zm_set_route_icon_and_text((gint)0) ;
      zm_load_consist_menu() ;
#else
/*
 *  generate Btree of routes and consists
 */
      ORroutes = find_route_dirs() ;
      n_orroutes = return_bt_count() ;
      ORconsists = zm_find_consists() ;
      n_consists = return_bt_count() ;
      if(ip){
        printf("  Number of ORroute directories = %i\n",n_orroutes) ;
        printf("  ORroutes = %p\n",(void *)ORroutes) ;
        printf("    List of routes:\n") ;
        print_bt_nodes_with_count_and_index(ORroutes) ;
        printf("  Number of consists = %i\n",n_consists) ;
        printf("  ORconsists = %p\n",(void *)ORconsists) ;
        printf("    List of consists:\n") ;
        print_bt_nodes_with_count_and_index(ORconsists) ;
      }
/*
 *   Fill combo box 2.
 */
      gtk_combo_box_text_remove_all((GtkComboBoxText*)combo2) ;
      for(i=1;i<n_orroutes+1;i++){
        bt1 = find_bt_node_with_index(ORroutes, i) ;
        if(bt1) gtk_combo_box_text_append_text ((GtkComboBoxText*) combo2,
                                                   (const gchar*) bt1->index ) ;
      }
      gtk_combo_box_set_active ((GtkComboBox*) combo2, (gint) 0) ;
      if(ip)printf("  ZM End of combo2 setup\n") ;
/*
 *  Set route icon and route text for first route
 */
gint  val = 0 ;
int   n = 0 ;
GdkPixbuf *pixbuf1, *pixbuf2 ;

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
 *   Fill combo box 5 (Consist)
 */
      if(consistlist_beg)consistlist_beg = NULL ;
      zm_load_consist_files() ;
ConsistNode *consist ;
      for(consist = consistlist_beg; consist!= NULL; consist = consist->next){
        printf("  Consist name = %s\n",consist->name) ;
      }
      combo5_active = 0 ;
      gtk_combo_box_text_remove_all((GtkComboBoxText*)combo5) ;
      for(consist = consistlist_beg; consist!= NULL; consist=consist->next){
        gtk_combo_box_text_append_text ((GtkComboBoxText*) combo5,
                                            (const gchar*) consist->name ) ;
      }
      gtk_combo_box_set_active ((GtkComboBox*) combo5, (gint) 0) ;
#endif

      combo1_active = 1 ;
      combo2_active = 1 ;
      combo5_active = 1 ;
      if(ip)printf("  ZM End of combo setup\n") ;

      gtk_button_set_label ((GtkButton *)button2, (const gchar* )"START");
      gtk_widget_show_all(window);

      gtk_main();

      return EXIT_SUCCESS;
      }

//......................................................................................
#ifndef USE_ZM_INIT
void      on_radio1_toggled(GtkRadioButton *b) {
      gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
    if (T) {
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) "Radio 1 Active");
      gtk_stack_set_visible_child(GTK_STACK(stack1), stkfxd1) ;
    }else{ gtk_label_set_text (GTK_LABEL(label2), (const gchar* ) "Radio 1 Not Active");
    }
      }

void      on_radio2_toggled(GtkRadioButton *b) {
      gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
    if (T) {
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) "Radio 2 Active");
      gtk_stack_set_visible_child(GTK_STACK(stack1), stkfxd2) ;
    }else{ gtk_label_set_text (GTK_LABEL(label2), (const gchar* ) "Radio 2 Not Active");
    }
      }

void      on_button1_clicked (GtkButton *b) {
      char tmp[128];
      gdouble val = gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin1));
      sprintf(tmp, "spin=%d", (int) val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      }

void      on_radio3_toggled(GtkRadioButton *b) {
      gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
      if (T) gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) "Radio 3 Active");
      else gtk_label_set_text (GTK_LABEL(label2), (const gchar* ) "Radio 3 Not Active");
      }

void      on_check1_toggled(GtkCheckButton *b) {
      gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
      if (T) gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) "Check 1 Active");
      else gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) "Check 1 Not Active");
      }

void      on_toggle1_toggled(GtkToggleButton *b) {
      gboolean T = gtk_toggle_button_get_active(b);
      if (T) gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) "Toggle 1 Active");
      else gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) "Toggle 1 Not Active");
      }

void      on_switch1_state_set (GtkSwitch *s) {
      gboolean T = gtk_switch_get_active(s);
      if (T) gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) "Switch 1 Active");
      else gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) "Switch 1 Not Active");
      }

void      on_combo1_changed(GtkComboBox *c) {
  int  i;
  char tmp[128] ;
  BTree *bt1 ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 1 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo1 changed.  val = %i\n",(int)val);
      if(val<0) return ;
      bt1 = find_bt_node_with_index(ORdirs, (int) val+1) ;
      printf("  New top directory = %s\n",bt1->index) ;
      printf("  New top directory = %s\n",bt1->data) ;

      ORdname = bt1->index ;
      ORdir   = bt1->data  ;
/*
 *  Call routine to initialise route menu (combo2)
 */
#if 1
     zm_load_route_menus() ;
#else
      ORroutes = find_route_dirs() ;
      n_orroutes = return_bt_count() ;
      printf("  Number of ORroute directories = %i\n",n_orroutes) ;
      printf("  ORroutes = %p\n",(void *)ORroutes) ;
      printf("    List of routes:\n") ;
      print_bt_nodes_with_count_and_index(ORroutes) ;
/*
 *   Fill combo box 2.
 */
      gtk_combo_box_text_remove_all ((GtkComboBoxText*)combo2) ;
      for(i=1;i<n_orroutes+1;i++){
        bt1 = find_bt_node_with_index(ORroutes, i) ;
        if(bt1) gtk_combo_box_text_append_text ((GtkComboBoxText*) combo2,
                                                   (const gchar*) bt1->index ) ;
      }
      gtk_combo_box_set_active ((GtkComboBox*) combo2, (gint) 0) ;
#endif
      return ;
}

void      on_combo2_changed(GtkComboBox *c) {
      char tmp[128] ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 2 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo2 changed entered\n");
      }

void      on_combo3_changed(GtkComboBox *c) {
      char tmp[128] ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 3 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo3 changed entered\n");
      }

void      on_combo4_changed(GtkComboBox *c) {
      char tmp[128] ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 4 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo4 changed entered\n");
      }

void      on_combo5_changed(GtkComboBox *c) {
      char tmp[128] ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 5 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo5 changed entered\n");
      }

void      on_combo6_changed(GtkComboBox *c) {
      char tmp[128] ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 6 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo6 changed entered\n");
      }

void      on_combo7_changed(GtkComboBox *c) {
      char tmp[128] ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 7 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo7 changed entered\n");
      }

void      on_combo8_changed(GtkComboBox *c) {
      char tmp[128] ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 8 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo8 changed entered\n");
      }

void      on_combo9_changed(GtkComboBox *c) {
      char tmp[128] ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 9 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo9 changed entered\n");
      }

void      on_combo10_changed(GtkComboBox *c) {
      char tmp[128] ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 10 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo10 changed entered\n");
      }

void      on_combo11_changed(GtkComboBox *c) {
      char tmp[128] ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 11 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo11 changed entered\n");
      }

void      on_combo12_changed(GtkComboBox *c) {
      char tmp[128] ;
      gint val = gtk_combo_box_get_active(c);
      sprintf(tmp, "combo 12 choice = %d", val);
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      printf("combo12 changed entered\n");
      }

void      on_link1_activate_link(GtkLinkButton *c) {
      char *tmp1 ;
      char  tmp[1024] ;
      tmp1 = (char *)gtk_link_button_get_uri(c);
      sprintf(tmp, "link1 activated:");
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      sprintf(tmp, "%s",tmp1);
      gtk_label_set_text (GTK_LABEL(label2), (const gchar* ) tmp);
      printf("link1 activated\n");
      }

void      on_color1_color_set (GtkColorButton *c) {
      GdkRGBA color;
      gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER(c), &color);
      printf("red %f\n", color.red);
      printf("green %f\n", color.green);
      printf("blue %f\n", color.blue);
      printf("alpha %f\n\n", color.alpha);
      /* just for fun, lets change the scrollbar background */
      gtk_widget_override_background_color (scroll1, GTK_STATE_FLAG_NORMAL, &color);
      }

void      on_file1_file_set(GtkFileChooserButton *f) {
      printf("file name = %s\n", gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(f)) );
      printf("folder name = %s\n", gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(f)) );
      }

void      on_font1_font_set(GtkFontButton *f1) {
      printf("font name = %s\n", gtk_font_button_get_font_name (f1) );
      }

void      on_volume1_value_changed (GtkVolumeButton *v1) {

      /*******************************************************************
      The function:
            void gtk_scale_button_set_value (GtkScaleButton *, gdouble)
      can be used to set the volume control to a particular value.
      ********************************************************************/

      printf("volume scale = %f\n", gtk_scale_button_get_value(GTK_SCALE_BUTTON(v1)) );
      }

void    on_scroll1_value_changed(GtkRange *r) {
        gdouble x = gtk_range_get_value (r);
      printf("scroll = %d\n", (int) x );
        }

void      on_entry1_changed(GtkEntry *e) {
      char tmp[128];
      sprintf(tmp, "entry=%s", gtk_entry_get_text(e));
      gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) tmp);
      }

void      on_button2_clicked(GtkButton *b) {
      printf("  Button 2 clicked\n") ;
      system("../../ZR/zr -p") ;
      printf("  Button 2 after echo\n") ;
/*
 *   gtk_signal_connect (GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (callback), (gpointer) "cool button");
 *
 *  See https://sites.cc.gatech.edu/data_files/public/doc/gtk/tutorial/gtk_tut-6.html
 */
//        gtk_editable_delete_text (GTK_EDITABLE(entry1), 0, -1);
      }

/*------ see below - this is commented out -------------------------------------------
void      on_button3_clicked(GtkButton *b) {
      char filename[] = "prr.jpg";
      int hor = 150, ver = 1;
      if (image1)
            gtk_container_remove (GTK_CONTAINER (fixed1), image1); // remove old slide
      gtk_widget_hide(image2);
        image1 = gtk_image_new_from_file (filename);
        gtk_container_add (GTK_CONTAINER (fixed1), image1);
        gtk_widget_show(image1);
        gtk_fixed_move (GTK_FIXED(fixed1), image1, hor, ver);
      }
-------------------------------------------------*/

void      on_button3_clicked(GtkButton *b) {
      char filename[] = "nya.jpg";
      char cmd[2048];
      FILE *f1;
      int j, h, v, hor = 10, ver = 30;

      if (image1)
            gtk_container_remove (GTK_CONTAINER (fixed1), image1); // remove old slide
      gtk_widget_hide(image2);

//      ---------------------------------------------------------------
//      get dimensions of image. identify() is part of ImageMagick
//      ---------------------------------------------------------------

      sprintf(cmd, "identify -format %%wx%%h \"%s\"\n", filename);

      f1 = popen(cmd, "r");

      strcpy(cmd,"");
      fgets(cmd, 512, f1);
      fclose (f1);

      h = v = 1;

      if (strlen(cmd)) {
            for (j=0; j<strlen(cmd)-1; j++) if (cmd[j] == 'x') break;
            if (cmd[j] == 'x') { // x is a delimiter between horizontal and vertical
                  cmd[j] = 0;
                  sscanf(cmd,"%d",&h);
                  sscanf(&cmd[j+1], "%d", &v);
                  }
            }

      if (h < 100 || v < 100 ) {
            printf("**** questionable image: %s\n",filename);
            return;      // probably a bad image
            }

//      ---------------------------------------------------------------
//      RESIZE image.  convert() is part of ImageMagick
//      ---------------------------------------------------------------

      int width = 420;
      int height = 250;

      sprintf(cmd, "convert \"%s\" -resize %dx%d tmp.jpg", filename, width, height);
      system(cmd);

      strcpy(filename,"tmp.jpg");

//      ---------------------------------------------------------------
//      Get new image dimensions  - actual dimensions will be in the
//      range of the conversion. Conversion will maintain ratio.
//      ---------------------------------------------------------------

      sprintf(cmd, "identify -format %%wx%%h \"%s\"\n", filename);
      f1 = popen(cmd, "r");
      strcpy(cmd,"");
      fgets(cmd, 512, f1);
      fclose (f1);
      h = v = 1;

      if (strlen(cmd)) {
            for (j=0; j<strlen(cmd)-1; j++) if (cmd[j] == 'x') break;
            if (cmd[j] == 'x') {
                  cmd[j] = 0;
                  sscanf(cmd,"%d",&h);
                  sscanf(&cmd[j+1], "%d", &v);
                  }
            }

//      ---------------------------------------------------------------
//      h and v are now the actual new dimensions which you can use
//      to better center your image in the GUI - not used here
//      ---------------------------------------------------------------

      image1 = gtk_image_new_from_file (filename);

      gtk_container_add (GTK_CONTAINER (fixed1), image1);
      gtk_widget_show(image1);

      gtk_fixed_move (GTK_FIXED(fixed1), image1, hor, ver);

      system("rm tmp.jpg");

      }

gboolean      timer_handler(GtkWidget *timer) {

            time_t t = time(0);
            gtk_label_set_text(GTK_LABEL(timer), ctime(&t));    // update time of day

            if (countCurrent) {
                  countCurrent = countCurrent - 1;
                  gtk_progress_bar_set_fraction
                        (GTK_PROGRESS_BAR(prog1), (gdouble) countCurrent/countDownMax );
                  }

//            Extract contents of string showing total play time.
//            This string is set in a forked process and the pointer
//            pingtotal is in shared memory

            gtk_label_set_text (GTK_LABEL(PingTimeDisplay), (const gchar* ) pingtotal);

            return TRUE; // FALSE kills the timer
            }

void      on_new_activate(GtkMenuItem *m) {
      printf("New activated\n");
      }

void      on_vc1_activate(GtkMenuItem *m) {
      printf("Vc1 activated\n");
      }

void      on_new_select(GtkMenuItem *m) {
      printf("New selected\n");
      }

void      on_new_deselect(GtkMenuItem *m) {
      printf("New deselected\n");
      }

void      on_start1_clicked (GtkButton *b) {
      countDownMax = (int) gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin1));
      if (countDownMax <= 0 ) countCurrent = countDownMax = 0;
      else {
            countCurrent = countDownMax;
            gtk_progress_bar_set_fraction
                  (GTK_PROGRESS_BAR(prog1), (gdouble) 1.00 );
            }
      }

gboolean on_start1_button_press_event (GtkButton *btn, GdkEventButton *event) {

      printf("event button value: %d\n", event->button);

        if (event->button < 3 ) return FALSE; // left or center click - continue processing

      countDownMax = countCurrent = 0;
      gtk_progress_bar_set_fraction
            (GTK_PROGRESS_BAR(prog1), (gdouble) 0.00 );
        return TRUE; // stop processing of this click
        }

gboolean on_button4_scroll_event(GtkButton *b, GdkEventScroll *e) {

                // see docs: GdkEventScroll data structure for valid elements
                // see above: gtk_widget_set_events(button4,GDK_SCROLL_MASK);

                if (e->direction == 1) { // down
                  if (currentRow < 4) {
                        currentRow ++;
                        gtk_label_set_text
                              (GTK_LABEL(label3), (const gchar* ) rows[currentRow]);
                        return TRUE;
                        }
                  return TRUE;
                        }
                else { // up
                  if (currentRow > 0) {
                        currentRow --;
                        gtk_label_set_text
                              (GTK_LABEL(label3), (const gchar* ) rows[currentRow]);
                        return TRUE;
                        }
                  return TRUE;
                        }
                return TRUE;
                }

void      on_PingTime_clicked (GtkButton *b) {

      pid_t i = fork();
      if (i) return; // parent task returns

      FILE *f1 = popen("ping -c 10 www.cs.uni.edu", "r");

      if (f1 == NULL) exit(1);

      while (1) {
            char name[512];
            char * p1;

            if (*pingFlag == 0) {
                  fclose(f1);
                  exit(0);
                  }

            if (fgets(name, 512, f1) == NULL) {
                  strcpy(pingtotal, "done");
                  fclose(f1);
                  exit(0);
                  }

            p1 = strstr(name, "time=");
            if (p1 == NULL) continue; // first line has no "time="

            printf("ping result=%s\n", p1);
            strcpy(pingtotal, p1);
            }
      }

void      on_saveText_clicked(GtkButton *b) {

      GtkTextIter begin, end;
      gchar * text;

      gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(TextBuffer), &begin, (gint) 0);
      gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(TextBuffer), &end, (gint) -1);

      text = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(TextBuffer), &begin, &end, TRUE);
      printf("-------\n%s\n--------\n",text);
      gtk_widget_hide(saveText);
      }

void      on_destroy() {
//            *pingFlag = 0; // note dereference operator
            gtk_main_quit();
            }

void      on_changed_text(GtkTextBuffer *t) {
      printf("*** text changed\n");
      gtk_widget_show(saveText);

      }
#endif

/*
 *  Routines used to keep track of open files
 *  Not needed once program fully debugged - if ever!
 */

FILE *gopen(char* fname, char *ftype){

FILE *f ;
      f = fopen(fname,ftype) ;
      return f ;
}

FILE *gcaseopen(char* fname, char *ftype){

FILE *f ;
      f = fcaseopen(fname,ftype) ;
      return f ;
}

int gclose(FILE *f){
int  iret ;
      iret = fclose(f) ;
      return iret ;
}



#include "token.c"
#include "token_b.c"
#include "fcaseopen.c"

#include "zm_system.c"
#include "zm_btree.c"
#include "zm_init.c"
#include "zm_init_pdb.c"
#include "zm_trk_itm_db.c"
#include "zm_trk_sct_db.c"
#include "zm_load_menus.c"
#include "zm_load_image.c"
#include "zm_find_routes.c"
#include "zm_find_platforms.c"
#include "zm_filesearch.c"
#include "zm_texture.c"
#include "zm_load_consist_files.c"
#include "zm_graphics.c"
#include "tsection_db.c"
#include "junction.c"
//#include "world.c"
#include "enum.c"
