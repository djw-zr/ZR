/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  functions.h
 *   Copyright 2025  David J. Webb
 *
 *   List of ZM menu functions
 *
 *   This file is part of ZR.
 *   Released under licence GPL-3.0-or-later
 *
 *   ZR is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   ZR is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *==============================================================================
 */

int     my_gtk_idle(void) ;
int     init_texture_node(TextureNode *texture) ;
int     load_texture(TextureNode *tnode) ;

int     zm_init(int argc, char *argv[]) ;
int     widgit_init(void) ;

int     init_pdb(void) ;
int     pdb_node_init(PdbNode *pdb)  ;
int     pdb_node_clean(PdbNode *pdb) ;
int     pdb_print(PdbNode *pdb)      ;
GdkPixbuf *zm_load_image_from_ace_file(char *ace_file) ;
char   *convert_print_style_string(char *string1) ;

BTree  *find_top_dirs(void);
BTree  *find_route_dirs(void);
BTree  *zm_find_consists(void) ;
BTree  *zm_find_platforms(void) ;
char   *find_route_name(char *route_dir) ;
int     zm_load_consist_files(void) ;
int     zm_load_route_menus(void) ;
int     zm_set_route_icon_and_text(gint val) ;
int     zm_load_consist_menu(void) ;
int     bt_clear_malloc_data(BTree *b) ;

int  mstswagon2local(double x0, double y0, double z0,
                     double xt, double yt, double zt,
                     double a,  double b,  double c,
                     double scalei,
                     double rx, double ry, double rz,
                     double *x, double *y, double *z) ;
int  global2local(int tile_e0, int tile_n0, double h0, double size, double scale,
                int tile_e,  int tile_n,
                double e,     double n,     double h,
                double *x,    double *y,    double *z ) ;

int  local2msts(int tile_e0, int tile_n0, double h0, double size, double scale,
                double x,     double y,     double z,
                int *tile_e, int *tile_n,
                double *e,    double *n,    double *h) ;
int  print_tsec_section(int section_index);
int  print_tsec_shape(int shape_index);

int  add_distances_to_track_vectors(TrkVectorNode *v, TrkSector *t, int n) ;
int  init_track_section(TrkSector *tsect) ;
static int cmp_dist(const void *p1, const void *p2) ;
void error1(char *routine, char *file, char *datafile) ;
void error2(char *routine, char *file, char *datafile, char *string) ;
void read_track_path(TrkSector *trk_path, MSfile *msfile) ;
void init_track_path(TrkSector *track_path) ;


int     return_bt_count(void) ;
void    print_bt_nodes_with_count_and_index(BTree *p) ;
BTree  *find_bt_node_with_index(BTree *p, int n) ;

int  init_system(void)             ; //  Initialise clocks and freetypr
int  nint(double d)                ; //  Nearest integer
void  str2lc(char *string)         ; //  Convert string to lower case
char *zr_to_upper(char *string)    ; // Return tring as upper case
char *zr_to_lower(char *string)    ; // Return as lower case
int  zr_filename_MS2L(char *fname) ; // replace '\' and '\\' by '/'
char *convert_dir_markers(char *string1) ;//  Convert '\\' to '/'
char *strip_dot_dirs(char* file_1)  ;// Convert '/./' and '/A/../' to '/'

char *strip_quotes(char *string1)   ;// Remove quotes around string
void  zr_str2lcnqne(char *string)   ;// Convert string to lower case without
                                    ;// quotes and file extension

int strcmp_ic(const char *s1,
                    const char *s2) ;//  Compare two string ignoring case
int strncmp_ic(const char *s1,
            const char *s2, int nn) ; // Ditto
int  strcmp_nq(char *s1a, char *s2a)  ; // Compare strings with quotes removed
int  strcmp_nqic(char *s1a, char *s2a); // Ditto ignoring case

char *zr_basename(char *fname)      ;// filename with directories removed
char *zr_basename2(char *fname)     ;// ... and without extension
char *zr_extension(char *fname)     ;// return extension
char *zr_extension_lc(char *fname)  ;// ... as lower case
char *zr_corename(char *fname)      ;// filename without extension
char *zr_parentdir(char *fname)     ;// Return parent directory A/B/C -> B
char *zr_full_parentdir(char *fname);// Return full parent name A/B/C -> A/B
char *zr_filename(char *fname)      ;

int find_msstyle_file(char **pname) ;// Find MS style file
int zr_find_msfile2(char **pname)   ;// Ditto modifying *pname
int zr_find_msdir(char **pname)     ;// Ditto + save directory in dir_master

char *make_filename_3(char *name1, char *name2, char *name3) ;
char *make_filename_5(char *name1, char *name2, char *name3, char *name4, char *name5);
