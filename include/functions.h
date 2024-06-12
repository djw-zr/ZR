/*
 * =============================================================================
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  function.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file containing the function specifications used by ZR
 *
 * =============================================================================
 */

// system.c routines

int  strcmp_ic(const char *s1, const char *s2) ;  // strcmp ignoring case.
int  strncmp_ic(const char *s1, const char *s2, int nn) ;

void str2lc(char *string)          ;  // string to lower case
void str2lcnqne(char *strint)      ;  // rmove quotes and extension as well
char *zr_basename(char *fname)     ;  // basename replacement
char *zr_extension(char *fname)    ;  // Return filename extension
char *zr_corename(char *fname)     ;  // Return filename without extension
char *strip_dot_dirs(char *fnane)  ;  // Strip '/../' and '/./' from string
char *zr_filename(char *fname)     ;  // Convert to standard ZR filename format
int  zr_filename_MS2L(char *pname) ;  // Replace '\' and '\\' by '/' in filenames
int  find_msstyle_file(char **pname) ;  // Alternative route to ...
int  zr_find_msfile2(char **pname)  ;  //  ...
int  zr_find_msdir(char **pname)  ;  //  ...
int  is_eof(char *token)           ;  // Return 1 if "End of File"
char *make_filename_3(char *n1, char *n2, char *n3) ;
char *make_filename_5(char *n1, char *n2, char *n3, char *n4, char *n5) ;


int  calls_per_sec(void)               ;  // Frames_per_second timing function
int  nint(double d)                ;  // Nearest integer
double interpolate_array(int nn, double xx, double *xa, double *ya) ;



int  process_args(int argc, char **argv) ;
void process_defaults(void)                  ;
int  init_system(void) ;
int  init_data_structures(void) ;
int  init_pdb(char *filename) ;
FILE *gopen(char* fname, char *ftype) ;
int  gclose(FILE *f) ;
int  close_system(void) ;

int   check_glerror(void) ;
int   check_glerror2(char *string) ;
void  display_error(void) ;

int   pdb_init(PdbNode *pdb) ;          // Read project database file (*.trk)
int   pdb_print(PdbNode *pdb) ;         // Print project data base structure

void  error1(char *routine, char *file, char *datafile);
void  error2(char *routine, char *file, char *datafile, char*string);
void  error3(char *routine, char *file, char *datafile, char *string, int n);
void  error4(char *routine, char *file, char *datafile, char *string);

//  Camera routines

int   cameras_init(void) ;
int   camera_new_position(void) ;
int   update_camera_offsets(TrainNode *player_train) ;


//  Track routines

int   init_track_db(char *filename) ;
void  read_track_path(TrkSector *pnode, MSfile *ms) ;
void  init_track_path(TrkSector *pnode) ;
int   list_track_section(TrkSector *pnode) ;
int   list_platforms_and_sidings() ;
int   add_world_item_pointers_to_track_vectors(TrkSector *tnnode) ;
int   set_junction_path(TrkSector *tnnode) ;
int   set_track_items_posn(TrkDataBase *d) ;
int   find_track_item_posn(TrkDataBase *, int id,
          uint *sec_index, uint *vec_index, double *distance) ;

//  Track sections

int   init_tsec_db(void) ;
int   print_tsec_section(int section_index) ;
int   print_tsec_shape(int shape_index) ;


int   print_world_item_data(int tile_x, int tile_y, int uid) ;

int   make_tile_vertex_arrays(void)                    ;
int   make_tile_vertex_array(TileListNode *tl_node)    ;
int   display_tile_vertex_array(TileListNode *tl_node) ;
void  display_topog_water(TileListNode *tl_node)       ;

int   make_texture_resident(TextureNode *tx_node) ;
int   make_needed_track_road_shapes(void) ;
int   make_track_shapes(void) ;
int   make_track_shape(TrkSector *tracknode, DynProfile *profile) ;
int   initialise_track_shape(ShapeNode *shapenode) ;
int   add_sub_nodes_to_track(TrkSector *track_section, DynProfile *profile) ;
int   add_texture_pointers_to_track_profiles(DynProfile *dnode) ;
int   make_track_display_lists(void) ;
int   create_dynamic_track_node(WorldItem *world_item) ;

/*  Initialise summary structures */

int   setup_level_crossings(void)   ;
int   read_turntable_file(void)     ;
int   setup_turntables(void)        ;
int   update_turntable(TurnTable *tt) ;
int   rotate_tt_clockwise(void)     ;
int   rotate_tt_anticlockwise(void) ;
int   end_tt_rotation(void)         ;
TurnTable *find_tt_in_window(void)  ;
int   water_column(int i)           ;
double find_witem_in_scene(WorldItem *witem) ;
int   setup_transfers(void)         ;
int   start_transfer(void)          ;
int   stop_transfer(void)           ;
int   update_transfer(void)         ;


int   read_tr_items_files(char *filename) ;
int   read_tr_items(MSfile *fp, char *header);
int   load_sigscr_file(char *dir, char *file) ;
char  *sigscr_token_string(int i) ;
int   process_signal_script(SignalDB *signal) ;
int   add_texture_pointers_to_signals(void) ;

int  find_next_signal_mr(SignalDB *signal, int itype, int itype2) ;
int  print_signal_data(SignalDB *signal) ;

int   init_road_db(char *filename) ;
void  read_road_path(TrkSector *road_path, MSfile *msfile) ;
void  init_road_path(TrkSector *road_path) ;
int   make_default_road_profile(void) ;
int   make_road_shapes(void) ;
int   make_road_display_lists(void) ;

int   read_ms_texture(char *filename) ;
int   read_ms_terrain(TileListNode *tile) ;
int   read_elevations(TileListNode *tile) ;

int   tilelist_init(int tile_w, int tile_e, int tile_s, int tile_n) ;
int   tilelist_init2(void) ;
char  *TileToFileName(int tilex, int tiley, int zoom);

int   read_signals_db(void)  ;
int   setup_signals(void)    ;
int   setup_raw_signals(void) ;
SignalDB *find_signal(char *name) ;

//  Forests
int  setup_forests(void) ;
int find_height(int tile_x1, int tile_y1, double x1, double y1, double *height) ;
int  check_rail_mask(int tile_x, int tile_y, double xx, double yy) ;


//  Train and traveller routines

int   load_consist_files() ;
int   trains_init(void) ;
int   add_new_train(char *name) ;
int   add_wagon_to_train(char *wagon, int idirn) ;
int   position_train(int itrack, int ivector, int idirect, double d) ;
int   trv_next(TravellerNode *t) ;
int   trv_prev(TravellerNode *t) ;
int   trv_position(TravellerNode *t) ;
int   trv_initv(TravellerNode *tn, int itrack, int ivector, int idirect) ;
int   test_trav(void)       ;
int   init_trav(TravellerNode *t) ;
int   copy_traveller_position(TravellerNode *t_copy, TravellerNode *t) ;
int   trv_move(TravellerNode *t, double dist) ;
int   search_for_track_position(TravellerNode *t_beg, TravellerNode *t_end,
                              uint tr_sect, double tr_dist) ;
int   set_traveller_posn(WagonNode *w) ;
int   update_trains(void) ;
void  crash_train(TrainNode *t, int i_crash) ;
void  crash_trains(TrainNode *t0, TrainNode *t1, int i_collide) ;
void  join_trains(TrainNode *t0, TrainNode *t1, int i_collide) ;

int   create_wagon_shape_node(RawWagonNode *w) ;
int   scan_for_wagon_files(void) ;
int   initialise_wagon_openal_variables() ;
// Sounds

int   link_world_sounds_to_sms_nodes(void) ;
int   load_sound_files(void) ;
int   load_sound_sms_file(SMS_Node *sms_node, char *filename) ;
int   update_sounds(void) ;
int   add_snd_trigger_to_list(TrgLstNode **s0, int itrigger) ;
int   remove_snd_trigger_from_list(TrgLstNode **s0, int itrigger) ;
int   add_trigger_to_motor(TrainNode *t, int trigger) ;
int   clear_wagon_sound_trigger_list(WagonNode *w) ;
int   initialise_sms_node(SMS_Node  *sms_node) ;
int   load_wave_file(char* filename, char **data, ALenum *format, ALsizei *size, ALsizei *frequency) ;
int   print_sms_node(RawWagonNode *rw) ;
int   set_sms_group(RawWagonNode *rw) ;

#ifdef OPENAL
int  setup_train_sms_nodes(void) ;
int  initial_train_sounds(void) ;
int  update_train_sounds(void) ;
char *al_error_code(ALenum iret) ;
#endif

int   update_level_crossings(void) ;
int   update_signals(void) ;
int   print_wagon_data(void) ;
int   print_wagon_data_to_file(char *filename) ;
int   check_wheel_radii(RawWagonNode *w) ;

void  split_train_at_wagon(TrainNode *train, int wagon_number) ;
void  trigger_wagon_event(TrainNode *train, int wagon_number)  ;
void  train_search(void) ;


//  Graphics routines
void  display(void) ;
int   display_shape(WorldItem *witem, ShapeNode *shape, DistLevel *dist_level) ;
int   display_wshape(WagonNode *wagon, ShapeNode *shape, DistLevel *dist_level) ;

//  Msfile routines (token.c)

int   open_msfile(char *filename, MSfile *msfile, int texture, int iprint) ;
int   close_msfile(MSfile *msfile)   ;

//  Text routines (token.c)

int   is_string_int(char *s)         ;
int   is_string_decimal(char *s)     ;
int   is_string_alnum(char *s)       ;
int   is_lbr(char *token)            ;
int   is_rbr(char *token)            ;

int   skip_lbr(MSfile *msfile)       ;
int   skip_rbr(MSfile *msfile)       ;
int   skip_rbr_1(MSfile *msfile)     ;
int   skippast_rbr(MSfile *msfile)     ;
int   return_token(char *token, MSfile *msfile) ;


char  *new_tmp_token(MSfile *msfile) ;
char  *ctoken(MSfile *msfile)        ;
char  *txt_token(MSfile *msfile)     ;
int   itoken(MSfile *msfile)         ;
unsigned int flagtoken(MSfile *msfile) ;
long  ltoken(MSfile *msfile)         ;
double dtoken(MSfile *msfile)        ;
unsigned int uitoken(MSfile *msfile)   ;
unsigned int flagtoken(MSfile *msfile) ;
long long lltoken_16(MSfile *msfile)   ;
double convert_unit(char *t, char *s)  ;

// Binary read routine (token_tb.c)

int   open_block(MSfile *msfile, int i_level)                   ;
int   open_named_block(MSfile *msfile, int i_level, int itoken) ;
int   is_block_end(MSfile *msfile, int i_level, int i_print)    ;
void  end_block(MSfile *msfile, int i_level)                    ;
void  init_levels(MSfile *msfile)                               ;

uint  read_uint8_a(MSfile *m)   ;
int   read_int16_a(MSfile *m)   ;
uint  read_uint16_a(MSfile *m)  ;
int   read_int32_a(MSfile *m)   ;
uint  read_uint32_a(MSfile *m)  ;
long  read_lint32_a(MSfile *m)  ;
float read_real32_a(MSfile *m)  ;
int   read_int64_a(MSfile *msfile) ;
char  *read_ucharz_a(MSfile *m) ;
void  skip_to_bblock_end_a(MSfile *m, MSblock *block) ;

// Original binary read routine (token_tb.c)

uint  read_uint8(FILE *fp)      ;
int   read_int16(FILE *fp)      ;
uint  read_uint16(FILE *fp)     ;
int   read_int32(FILE *fp)      ;
uint  read_uint32(FILE *fp)     ;
float read_real32(FILE *fp)     ;
int   read_int64(FILE *fp)      ;
char  *read_ucharz(FILE *fp)    ;
void  skip_to_bblock_end(FILE *fp, MSblock *block) ;


int   load_shape_d(ShapeNode *snode) ;
int   init_shape_node(ShapeNode *snode) ;
int   print_shape_file_data(ShapeNode *snode) ;
int   add_shape_pointers_to_world_items(void) ;
void  add_world_shapes_to_master(void *b) ;
int   correct_shape(ShapeNode *snode) ;

int   generate_shape_display_list(ShapeNode *snode) ;


int   add_texture_pointers_to_shape_items(ShapeNode *snode) ;
int   sort_textures(TextureNode **t) ;
int   convert_texture(TextureNode *t) ;
int   texture_print(char *name) ;
int   add_texture_pointers_to_wagon_shapes(ShapeNode *snode) ;
int   mark_needed_wagon_textures(ShapeNode *snode) ;

ShapeNode *find_wagon(char *name)                             ;
char  *find_wagon_wave_file(RawWagonNode *rw, char *wav_file) ;

int   trv_print(TravellerNode *tn)            ;
int   trk_next(TravellerNode *t, int inext)   ;
int   trk_next_p(TravellerNode *t, int inext) ;



int   global2local(int tile_e0, int tile_n0, double h0, double size, double scale,
                  int tile_e,  int tile_n,  double e,  double n,    double h,
                  double *x,    double *y,    double *z) ;
int   local2msts(int tile_e0, int tile_n0, double h0, double size, double scale,
                 double x,     double y,     double z,  int *tile_e, int *tile_n,
                 double *e,    double *n,    double *h) ;
int   traveller2modelview(TravellerNode *t, GLdouble *mview) ;

int   rotate_axis_angle(double x,   double y,   double z,
                        double ax,  double ay,  double az,  double ang,
                        double *xx, double *yy, double *zz) ;
int   rotate_axis_angle_010(double ax,  double ay,  double az,  double ang,
                            double *xx, double *yy, double *zz) ;
int   mstswagon2local(double x0, double y0, double z0,
                      double xt, double yt, double zt,
                      double a,  double b,  double c,
                      double scalei,
                      double rx, double ry, double rz,
                      double *x, double *y, double *z);
int   zr_print_modelview(void) ;

int   test_transforms(void) ;

void  zr_clock_gettime(struct timespec clock[4]) ;

//  Freetype

int    freetype_init(void) ;

//OpenAL
#ifdef OPENAL
int  setup_openal(int argc, char **argv) ;
#endif
