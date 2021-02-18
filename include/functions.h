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

// system.c
int  strcmp_ic(const char *s1, const char *s2) ;  // strcmp ignoring case.
char *zr_basename(char *fname)     ;  // basename replacement
char *zr_extension(char *fname)    ;  // Return filename extension
char *zr_corename(char *fname)     ;  // Return filename without extension
int zr_filename_MS2L(char *fname)  ;  // Convert all '\' in filename to '/'
int  calls_per_sec()               ;  // Frames_per_second timing function

int  process_args(int argc, char **argv) ;
void process_defaults()                  ;
int  init_data_structures() ;
int  init_pdb(char *filename) ;
FILE *gopen(char* fname, char *ftype) ;
int  gclose(FILE *f) ;

int   check_glerror() ;
int   check_glerror2(char *string) ;

int   pdb_init(PdbNode *pdb) ;          // Read project database file (*.trk)
int   pdb_print(PdbNode *pdb) ;         // Print project data base structure

void  error1(char *routine, char *file, char *datafile);
void  error2(char *routine, char *file, char *datafile, char*string);
void  error3(char *routine, char *file, char *datafile, char *string, int n);
void  error4(char *routine, char *file, char *datafile, char *string);

int   init_track_db(char *filename) ;
void  read_track_section(TrkSectionNode *tracknode, MSfile *ms) ;
void  init_track_section(TrkSectionNode *tracknode) ;
int   list_track_section(TrkSectionNode *tnode) ;

int   init_tsec_db() ;

int   make_tile_vertex_arrays()       ;
int   make_tile_vertex_array(TileListNode *tnode)       ;
int   display_tile_vertex_array(TileListNode *tnode) ;

int   make_needed_track_road_shapes() ;
int   make_track_shapes() ;
int   make_track_shape(TrkSectionNode *tracknode, DynProfile *profile) ;
int   initialise_track_shape(ShapeNode *shapenode) ;
int   add_sub_nodes_to_track(TrkSectionNode *track_section, DynProfile *profile) ;
int   add_texture_pointers_to_track_profiles(DynProfile *dnode) ;
int   make_track_display_lists() ;
int   create_dynamic_track_node(WorldItem *world_item) ;

int   read_tr_items_files(char *filename) ;
int   read_tr_items(MSfile *fp, char *header);

int   init_road_db(char *filename) ;
void  read_road_section(TrkSectionNode *road_section, MSfile *msfile) ;
void  init_road_section(TrkSectionNode *road_node) ;
int   make_default_road_profile() ;
int   make_road_shapes() ;
int   make_road_display_lists() ;

int   read_ms_texture(char *filename) ;
int   read_ms_terrain(TileListNode *tile) ;
int   read_elevations(TileListNode *tile) ;

int   tilelist_init(int tile_w, int tile_e, int tile_s, int tile_n) ;
int   tilelist_init2() ;
char  *TileToFileName(int tilex, int tiley, int zoom);

//  Msfile routines (token.c)

int  open_msfile(char *filename, MSfile *msfile, int texture, int iprint) ;
int   close_msfile(MSfile *msfile)   ;

//  Text routines (token.c)

int   is_string_int(char *s)         ;
int   is_string_decimal(char *s)     ;
int   is_string_alnum(char *s)       ;
int   is_rbr(char *token)            ;

int   skip_lbr(MSfile *msfile)       ;
int   skip_rbr(MSfile *msfile)       ;

char  *new_tmp_token(MSfile *msfile) ;
char  *ctoken(MSfile *msfile)        ;
int   itoken(MSfile *msfile)         ;
long  ltoken(MSfile *msfile)         ;
double dtoken(MSfile *msfile)        ;
unsigned int uitoken(MSfile *msfile) ;
long long lltoken_16(MSfile *msfile) ;

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
float read_real32_a(MSfile *m)  ;
int   read_int64_a(MSfile *msfile) ;
char  *read_ucharz_a(MSfile *m) ;

// Original binary read routine (token_tb.c)

uint  read_uint8(FILE *fp)      ;
int   read_int16(FILE *fp)      ;
uint  read_uint16(FILE *fp)     ;
int   read_int32(FILE *fp)      ;
uint  read_uint32(FILE *fp)     ;
float read_real32(FILE *fp)     ;
int   read_int64(FILE *fp)      ;
char  *read_ucharz(FILE *fp)    ;


int   load_shape_d(ShapeNode *snode) ;
int   init_shape_node(ShapeNode *snode) ;
int   print_shape_file_data(ShapeNode *snode) ;
int   add_shape_pointers_to_world_items() ;

int generate_shape_display_list(ShapeNode *snode) ;
int add_texture_pointers_to_shape_items(ShapeNode *snode) ;

int  generate_wagon_list()                                  ;
int  add_texture_pointers_to_wagon_shapes(ShapeNode *snode) ;

int  global2local(int tile_e0, int tile_n0, float h0, float size, float scale,
                 int tile_e,  int tile_n,  float e,  float n,    float h,
                 float *x,    float *y,    float *z) ;
int  local2msts(int tile_e0, int tile_n0, float h0, float size, float scale,
                float x,     float y,     float z,  int *tile_e, int *tile_n,
                float *e,    float *n,    float *h) ;
int  test_transforms() ;

