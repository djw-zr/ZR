/*
 * *****************************************************************************
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File : shape.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to handle the loading of binary shape data
 *
 *   Once uncompressed the MSTS binary files consist of a series of well
 *   defined blocks.  Each block starts with a 16 byte header made up of:
 *     2-byte unsigned integer  :  enum corresponding to the name of the block,
 *     2-byte unsigned integer  ;  flags,
 *     4-byte unsigned integer  :  length of the remainder of the block.
 *   The remainder of the block is made up of data and sub-blocks with the same
 *   type of header.
 *   The 'length' is usually correct but not always.  However the order of
 *   block names appears to be fixed.  This version of the file therefor
 *   assumes the order is fixed and only uses the block length for the few
 *   cases where the total amount of data in a block is not otherwise clear.
 *
 * *****************************************************************************
 */
#include <dirent.h>
#include <stdio.h>

void error_s1(char *d)  ;
void init_sub_object(SubObject *s)  ;
void init_tri_list(TriList *t)      ;

/*
 *  Function to test a directory entry for ".s" filename extension
 *  (*.h, *.ace etc.)
 */


int file_is_shape( const struct dirent *p){

  int len = strlen(p->d_name);
      if(len<3) return 0 ;
      if(strcmp_ic((char *)&p->d_name[len-2],".s")== 0) return 1;
      return 0;
}

/*
 * Routine to create a shape node for each shape file in directory SHAPES
 */

int load_shape_filenames() {

int    len1, len2, len3, idir, i, k, n, iret ;
int    ip = 0 ;                    // 0 = no printing
char   *name      = NULL ;
char   *sdir_name = NULL ;
DIR    *sdir      = NULL ;
FILE   *f         = NULL ;
char   my_name[] = "load_shape_filenames" ;
struct dirent *f_entry   = NULL ;
struct dirent **namelist = NULL ;
ShapeNode *shape     ;
char   *shape_dir   = "/SHAPES/" ;
char   *g_shape_dir = "/Global/Shapes/" ;
BTree  *btree_node   ;
/*
 *   Process shape files in directories
 *      ROUTE/SHAPES and GLOBAL/SHAPES
 */
      for(idir=0;idir<2;idir++){
        if(idir==0){
          len1 = strlen(ORroutedir) + strlen(shape_dir) + 1 ;
          sdir_name = (char *)malloc(len1) ;
          strcpy(sdir_name,ORroutedir)    ;
          strcat(sdir_name,shape_dir)      ;
        }else{
          len1 = strlen(ORdir) + strlen(g_shape_dir) + 1 ;
          sdir_name = (char *)malloc(len1) ;
          strcpy(sdir_name,ORdir)    ;
          strcat(sdir_name,g_shape_dir)      ;
        }
        if(ip)printf(" Directory SHAPES = %s\n",sdir_name) ;
        iret = zr_find_msfile2(&sdir_name) ;
        if(iret || sdir_name == NULL){
          printf(" Routine %s : ERROR : Unable to open SHAPES directory\n",
                                                                my_name) ;
          printf("    Directory name = %s\n", sdir_name) ;
          printf("    Program stopping ...\n") ;
          exit(1) ;
        }

        sdir = opendir(sdir_name) ;
        if(sdir == NULL) error_s1(sdir_name) ;
        closedir(sdir) ;

//        while ((f_entry = readdir(sdir)) != NULL) {
//          len2 = strlen(f_entry->d_name) ;
//          if(strcmp(".s",&(f_entry->d_name[len2-2]))!= 0 &&
//            strcmp(".S",&(f_entry->d_name[len2-2]))!= 0) continue ;
#ifdef MinGW
        n = scandir(sdir_name, &namelist, file_is_shape, NULL);
#else
        n = scandir(sdir_name, &namelist, file_is_shape, versionsort);
#endif
        if (n == -1) {
          perror("scandir");
          exit(EXIT_FAILURE);
        }
        printf("     Shapes from directory %s\n",sdir_name );
//        printf("  n = %i, namelist = %p :: %p\n",n,(void *)namelist,(void *)namelist[0]) ;
#if 0
        if(ip){
          for(i=0;i<n;i++){
            char *string = namelist[i]->d_name ;
            printf(" %s  name = %i,  %s\n",my_name,i,string) ;
            printf(" Ascii = ") ;
            for(k=0;k<(int)strlen(string);k++){
              unsigned char c = string[k] ;
              printf(" %c %x ",c,c) ;
            }
            printf("\n") ;
          }
        }
#endif
/*
 *  Process shape files
 */
        for(i=0;i<n;i++){
//  Determine core name and convert to lower case
          if(name){free(name); name = NULL;}
          name = zr_basename2(namelist[i]->d_name) ;
          str2lc(name) ;
/*
 *  Skip unless shape is needed
 */
          btree_node = find_btree(shape_master,name) ;
//          if(0)printf("  Routine %s.  Btree node = %p.  Shape is %s\n",
//                                                 my_name, (void *)btree_node, name) ;
#if 0
          if(btree_node){
            if(btree_node->data){
              shape = (ShapeNode *) btree_node->data ;
              printf("  Dir contains shape file %s,  btree_node = %p, data = %p, name = %s, "
                     "file = %s\n", name, (void *)btree_node,
                     (void *)btree_node->data, shape->name, shape->s_file);
            }else{
              printf("  Dir contains shape file %s,  btree_node = %p, data = %p\n",
                   name, (void *)btree_node, (void *)btree_node->data);
            }
          }else{
            printf("  Dir contains shape file %s,  btree_node = %p\n", name, (void *)btree_node);
          }
#endif
          if(!btree_node){
            free(namelist[i]) ;
            continue ;  //  Shape  is not needed
          }
/*
 *  Initialise new ShapeNode if not initialised
 *  Otherwise process as normal
 *  TODO This only initialises normal shapes
 *       add code to initialise btree->shapes in world.c with filename
 *       and to initialise to initialise btree->shapes not processed here.
 *  NOTE:  Each Tile/World Node has its own btree!!
 *
 */
          if(!(btree_node->data)){
            shape = (ShapeNode *)malloc(sizeof(ShapeNode)) ;
            init_shape_node(shape) ;
            btree_node->data = (void *)shape ;

            if(shapelist_beg == NULL){
              shapelist_beg = shape       ;
            }else{
              shapelist_end->next = shape ;
            }
//         printf(" CC i = %i\n",i) ;
            shapelist_end = shape ;
            shape->next   = NULL ;
            if(ip)printf("  Routine %s.  shapelist_end name added = %s :: %s\n",
                           my_name, name, namelist[i]->d_name);
          }else{
            shape = (ShapeNode *)btree_node->data ;
            if(ip)printf("  Routine %s.  btree already contains   = %s :: %s\n",
                           my_name, name, namelist[i]->d_name);
          }
/*
 *  Save name and filename
 */
//         printf(" DD i = %i\n",i) ;
          len2 = strlen(namelist[i]->d_name) ;
          shape->name = (char *)malloc(len2-1) ;
          strncpy(shape->name,namelist[i]->d_name,len2-2);
          shape->name[len2-2] = '\0' ;
          if(ip)printf(" Found shape file : %s\n",shape->name) ;
//          if(!strncmp(shape->name,"ukfs_t",6)){
//            printf("  FOUND shape = %s",shape->name) ;
//          }
/*
 *  Save full name of s file
 */
          shape->s_file = (char *)malloc(len1 + len2 ) ;
          strcpy(shape->s_file,sdir_name) ;
          strcat(shape->s_file,namelist[i]->d_name);

          f=gopen(shape->s_file,"r") ;
          if(f!=NULL ){
            if(ip)printf(" File opened: \n") ;
            gclose(f);
          }else{
            if(ip)printf(" File not opened \n") ;
          }
/*
 *  Save full name of sd file if present or NULL
 */
          len3 = len1 + len2 + 1 ;
          shape->sd_file = (char *)malloc(len3) ;
          strncpy(shape->sd_file, shape->s_file, len3) ;
//          shape->sd_file[len3-3] = 's' ;
          if(shape->sd_file[len3-3]=='s'){
            shape->sd_file[len3-2] = 'd' ;
          }else{
            shape->sd_file[len3-2] = 'D' ;
          }
          shape->sd_file[len3-1] = '\0' ;
          if(ip)printf("  AA  Full name of sd file : %s\n",shape->sd_file) ;
/*
 *  Check that sd file exists
 */
          if((f=gopen(shape->sd_file,"r"))==NULL ){
/*
 *  If sd file was not found, try using routine fcaseopen.
 *  This searches for the filename - ignoring the case of each character.
 */
#if 1
            if((f=gcaseopen(shape->sd_file,"r"))==NULL ){
/*
 *  If the file is still not found, skip MSTS track shapes as these do (may?) not
 *  have shape description (*.sd) files.  Otherwise print an error message.
 */
              if((shape->name[0] != 'a' && shape->name[0] != 'A') ||
                  (shape->name[1] != '1' && shape->name[1] != '2')){
                printf("       Routine '%s'.  Unable to find sd file "
                  "corresponding to shape : %s\n",my_name, shape->name)   ;
              }
//              printf("  shapefile *.s  = %s\n",shape->s_file) ;
//              printf("  shapefile *.sd = %s\n",shape->sd_file) ;
              free(shape->sd_file);
              shape->sd_file = NULL ;
            }
#else

            shape->sd_file[len3-2] = 'D' ;
            if((f=gopen(shape->sd_file,"r"))==NULL ){
              shape->sd_file[len3-3] = 'S' ;
              shape->sd_file[len3-2] = 'd' ;
              if((f=gopen(shape->sd_file,"r"))==NULL ){
                shape->sd_file[len3-2] = 'D' ;
                if((f=gopen(shape->sd_file,"r"))==NULL ){
//  Ignore MSTS track shapes
                  if((shape->name[0] != 'a' && shape->name[0] != 'A') ||
                     (shape->name[1] != '1' && shape->name[1] != '2')){
                    printf("       Routine '%s'.  Unable to find sd file "
                      "corresponding to shape : %s\n",my_name, shape->name)   ;
//                  printf("   Full name of s  file : %s\n",   shape->s_file)  ;
//                  printf("  Full name of sd file : %s\n",    shape->sd_file) ;
                  }
                  free(shape->sd_file);
                  shape->sd_file = NULL ;
                }
              }
            }
#endif
          }
          if(f != NULL)gclose(f) ;
          free(namelist[i]) ;
        }
        if(namelist)free(namelist);
        if(sdir_name)free(sdir_name) ;
      }
      if(name)free(name) ;
      return 0;
}

/*
 *  Initialise Shapenode
 */
int  init_shape_node(ShapeNode *shape){


          shape->name              = NULL ;
          shape->next              = NULL ;
          shape->s_file            = NULL ;
          shape->sd_file           = NULL ;
//          shape->gl_display_list   = 0 ;  // not a valid display list number
          shape->basic             = 0 ;  //  = 1 always needed
          shape->needed            = 0 ;  //  Needed for current scene
          shape->loaded            = 0 ;  //  Loaded in graphics card
          shape->flags1            = 0 ;
          shape->flags2            = 0 ;
          shape->nvolumes          = 0 ;
          shape->nshaders          = 0 ;
          shape->nfilters          = 0 ;
          shape->npoints           = 0 ;
          shape->nnormals          = 0 ;
          shape->nuvpoints         = 0 ;
          shape->nsort_vectors     = 0 ;
          shape->ncolors           = 0 ;
          shape->nmatrices         = 0 ;
          shape->n_textures        = 0 ;
          shape->use_texture       = 0 ;
          shape->n_texlevel_low    = 0 ;
          shape->nlight_materials  = 0 ;
          shape->nlight_model_cfgs = 0 ;
          shape->n_vtx_states      = 0 ;
          shape->n_prim_states     = 0 ;
          shape->dlevel_selection  = 0 ;
          shape->nhierarchy        = 0 ;
          shape->n_lod_controls    = 0 ;
          shape->n_animations      = 0 ;
          shape->shape_vol         = NULL ;
          shape->shader            = NULL ;
          shape->filter            = NULL ;
          shape->point             = NULL ;
          shape->normal            = NULL ;
          shape->uvpoint           = NULL ;
          shape->sort_vector       = NULL ;
          shape->color             = NULL ;
          shape->matrix            = NULL ;
          shape->texture_name      = NULL ;
          shape->texture           = NULL ;
          shape->texlevel_low      = NULL ;
          shape->light_material    = NULL ;
          shape->light_model_cfg   = NULL ;
          shape->vtx_state         = NULL ;
          shape->prim_state        = NULL ;
          shape->hierarchy         = NULL ;
          shape->hierarchy_flag    = NULL ;
          shape->lod_control       = NULL ;
          shape->animation         = NULL ;

          shape->esd_detail_level          = 0 ;
          shape->esd_alternative_texture   = 0 ;
          shape->esd_tunnel_length         = 0.0 ;
          shape->esd_tunnel_f1             = 0.0 ;
          shape->esd_tunnel_f2             = 0.0 ;
          shape->esd_tunnel_i1             = 0 ;
          shape->esd_tunnel_i2             = 0 ;
          shape->esd_no_visual_obstruction = 0 ;
          shape->esd_snapable = 0 ;
          shape->esd_sound_file_name       = NULL ;
          shape->esd_bell_animation_fps    = 0.0 ;
          shape->esd_bounding_box_xmin     = 0.0 ;
          shape->esd_bounding_box_xmax     = 0.0 ;
          shape->esd_bounding_box_ymin     = 0.0 ;
          shape->esd_bounding_box_ymax     = 0.0 ;
          shape->esd_bounding_box_zmin     = 0.0 ;
          shape->esd_bounding_box_zmax     = 0.0 ;
          shape->n_esd_complex_box         = 0 ;
          shape->esd_complex_box           = NULL ;

          shape->no_culling         = 0 ;

      return 0;
}



/*
 * =============================================================================
 *   Routine to read the shape (*.s) files and populate the shape database.
 *   The code is based on OR file ShapeFile.cs
 * =============================================================================
 */



int load_shape(ShapeNode *snode ) {

  uint    ip = 0 ;                  //  Debug printing when ip == 1
  uint    i, j, k, l, m, n, itoken;
  int     iret ;
  MSfile  msfile0 ;
  MSfile  *msfile = &msfile0 ;
  FILE    *fp ;
  char    *string ;
  char    my_name[] = "load_shape" ;

//      ip = ip && !strcmp_ic(snode->name,test_shape) ;
//      ip = !strncmp(snode->name,"mm_",3) || !strncmp(snode->name,"MM_",3) ;
      if(ip || 0){
        printf("\n  Enter routine : %s : %i\n",my_name, ip);
        printf("    Shape name = %s\n",snode->name);
        printf("    File       = %s\n",snode->s_file);
      }
/*
 * =============================================================================
 *  open_msfile reads and checks the first 16 bytes of the file, inflates
 *  compressed files, reads the next 16 bytes and sets the flags.
 * =============================================================================
 */
      if(ip)printf("  AA\n");
      zr_filename_MS2L(snode->s_file) ;          //  Convert '\' to '/'
      if(ip)printf("  Routine %s, snode->s_file = %s\n",my_name,snode->s_file) ;
      iret = zr_find_msfile2(&(snode->s_file)) ;
/*
 *  If the shape is not found - print message and set name to NULL
 *  This shape is then skipped by the display routine
 */
      if(iret){
        printf("\n\n  ERROR in routine %s\n",my_name);
        printf("      zr_find_msfile2 failed to find shape file\n");
        printf("      Shape name = %s\n",snode->name);
        printf("      Shape file = %s\n",snode->s_file) ;
        free(snode->name) ;
        snode->name = NULL ;
        return 1 ;
      }
      if(ip)printf("  Routine %s,        File = %s\n",my_name,snode->s_file) ;
      l = open_msfile(snode->s_file, msfile, 0, ip) ;
      if(l!=0){
        printf("\n\n  ERROR : Routine open_msfile failed to open file\n\n");
        free(snode->name) ;
        snode->name = NULL ;
        return 1 ;
      }
      fp = msfile->fp      ;
      init_levels(msfile) ;   // Initialise system to track data blocks
/*
 * =============================================================================
 *  read blocks
 * =============================================================================
 */
      if(ip)printf("  At start of file ftell = %i\n",(int) ftell(fp));
// 0 SHAPE
      l = open_named_block(msfile, 0, SHAPE) ;
      if(1==l){
        close_msfile(msfile) ;
        return 1;
      }
      if(ip)printf(" Token SHAPE found \n");
/*
 * 1  SHAPE_HEADER
 */
        open_named_block(msfile, 1, SHAPE_HEADER) ;
        if(ip)printf(" Token SHAPE_HEADER found \n");
        snode->flags1 = read_uint32_a(msfile) ;
        if(!is_block_end(msfile, 1, 0)){
          snode->flags2 = read_uint32_a(msfile) ;
        }
        end_block(msfile, 1) ;
/*
 * =============================================================================
 * 1  VOLUMES
 * =============================================================================
 */
        open_named_block(msfile, 1, VOLUMES) ;
        if(ip)printf(" Token VOLUMES found \n");
        snode->nvolumes = read_uint32_a(msfile) ;
        if(ip)printf(" read_shape : snode->nvolumes = %i\n", snode->nvolumes);
        snode->shape_vol = (ShapeVolume *)malloc(snode->nvolumes
                                                 * sizeof(ShapeVolume)) ;
        for(i=0;i<snode->nvolumes;i++){
// 2 VOL_SPHERE
          open_named_block(msfile, 2, VOL_SPHERE) ;
          if(ip)printf(" Token VOL_SPHERE found \n");
// 3 VECTOR
          {
            open_named_block(msfile, 3, VECTOR) ;
            if(ip)printf(" Token VECTOR found \n");
// Read the MSTS shape coordinates X, Y and Z (where Y is up)
//      but place as geographic coordinates Y, Z and X
// A wagon will usually have an MSTS length along X and width to the right
//   along Z.  It is now stored with length along Y and a width along X
            snode->shape_vol[i].vec.Y = read_real32_a(msfile) ; // MSTS X
            snode->shape_vol[i].vec.Z = read_real32_a(msfile) ; // MSTS Y
            snode->shape_vol[i].vec.X = read_real32_a(msfile) ; // MSTS Z
            end_block(msfile,3) ;
          }
          snode->shape_vol[i].radius = read_real32_a(msfile) ;
          end_block(msfile,2) ;
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 SHADER_NAMES
 * =============================================================================
 */
        open_named_block(msfile, 1, SHADER_NAMES) ;
        if(ip)printf(" Token SHADER_NAMES found \n");
        snode->nshaders = read_uint32_a(msfile) ;
        snode->shader = (char **)malloc(snode->nshaders*sizeof(char *));
        for(i=0;i<snode->nshaders;i++){
// 2 NAMED_SHADER
          open_named_block(msfile, 2, NAMED_SHADER) ;
          if(ip)printf(" Token NAMED_SHADER found \n");
          snode->shader[i] = read_ucharz_a(msfile);
          if(ip)printf(" NAMED_SHADER = %s\n",snode->shader[i]);
          end_block(msfile,2) ;
        }
        end_block(msfile,1) ;
/*
 * 1 TEXTURE_FILTER_NAMES
 */
        open_named_block(msfile, 1, TEXTURE_FILTER_NAMES) ;
        if(ip)printf(" Token TEXTURE_FILTER_NAMES found \n");
        snode->nfilters = read_uint32_a(msfile) ;
        snode->filter = (char **)malloc(snode->nfilters*sizeof(char *));
        for(i=0;i<snode->nfilters;i++){
// 2 NAMED_FILTER_MODE
          open_named_block(msfile, 2, NAMED_FILTER_MODE) ;
          if(ip)printf(" Token NAMED_FILTER_MODE found \n");
          snode->filter[i] = read_ucharz_a(msfile);
          if(ip)printf(" NAMED_FILTER_MODE = %s\n",snode->filter[i]);
          end_block(msfile,2) ;
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 POINTS   (Given as ZR_POINTS in enum.h to bypass window script)
 * =============================================================================
 */
        open_named_block(msfile, 1, ZR_POINTS) ;
        snode->npoints = read_uint32_a(msfile) ;
        if(ip)printf(" Token POINTS found.  npoints = %i \n",snode->npoints);
/*
 *  Check npoints is correct
 */
        if(msfile->binary){
          n = msfile->level[1].length - 13 ;
          m = n/21 ;
          if(m!=snode->npoints){
            printf("     Routine %s error.\n",my_name) ;
            printf("       Shape name = %s\n",snode->name) ;
            printf("       Number of points reported = %i\n", snode->npoints) ;
            printf("       Number of points present  = %i\n", m) ;
            if(m>snode->npoints)snode->npoints = m ;
            printf("       Number of points stored   = %i\n", snode->npoints) ;
          }
        }
        snode->point = (Vector3 *)malloc(snode->npoints*sizeof(Vector3));
        for(i=0;i<snode->npoints;i++){
// 2 POINT   (Changed to ZR_POINT to bypass Windows script)
          open_named_block(msfile, 2, ZR_POINT) ;
          if(ip)printf(" Token ZR_POINT found. i = %i   ",i);
#ifdef geo_coord
          snode->point[i].X = read_real32_a(msfile);
          snode->point[i].Z = read_real32_a(msfile);
          snode->point[i].Y = read_real32_a(msfile);
#else
          snode->point[i].X = read_real32_a(msfile);
          snode->point[i].Y = read_real32_a(msfile);
          snode->point[i].Z = read_real32_a(msfile);
#endif
          if(ip)printf("  %f   %f   %f\n",snode->point[i].X,snode->point[i].Y,snode->point[i].Z) ;
          is_block_end(msfile,2,ip) ;
          end_block(msfile,2) ;
        }
        is_block_end(msfile,1,ip) ;
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 UV_POINTS
 * =============================================================================
 */
        open_named_block(msfile, 1, UV_POINTS) ;
        snode->nuvpoints = read_uint32_a(msfile) ;
        if(ip)printf(" Token UV_POINTS found.  nuvpoints = %i \n",snode->nuvpoints);
        snode->uvpoint = (UVVector2 *)malloc(
                                      snode->nuvpoints*sizeof(UVVector2));
        for(i=0;i<snode->nuvpoints;i++){
// 2 UV_POINT
          open_named_block(msfile, 2, UV_POINT) ;
          if(ip)printf(" Token UV_POINT found. i = %i   ",i);
          snode->uvpoint[i].U = read_real32_a(msfile);
          snode->uvpoint[i].V = read_real32_a(msfile);
          if(ip)printf("  %f   %f\n",snode->uvpoint[i].U,snode->uvpoint[i].V) ;
          end_block(msfile,2) ;
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 NORMALS
 * =============================================================================
 */
        open_named_block(msfile, 1, NORMALS) ;
        snode->nnormals = read_uint32_a(msfile) ;
        if(ip)printf(" Token NORMALS found.  nnormals = %i \n",snode->nnormals);
        snode->normal = (Vector3 *)malloc(snode->nnormals*sizeof(Vector3));
        for(i=0;i<snode->nnormals;i++){
// 2 VECTOR
float  X, Y, Z, R ;
          open_named_block(msfile, 2, VECTOR) ;
          if(ip)printf(" Token normal VECTOR found. i = %i   ",i);
          X = read_real32_a(msfile);
          Y = read_real32_a(msfile);
          Z = read_real32_a(msfile);
          R = 1.0/sqrt(X*X+Y*Y+Z*Z) ;
// Normalise
#ifdef geo_coord
          snode->normal[i].X = X*R ;
          snode->normal[i].Z = Y*R ;
          snode->normal[i].Y = Z*R ;
#else
          snode->normal[i].X = X*R ;
          snode->normal[i].Y = Y*R ;
          snode->normal[i].Z = Z*R ;
#endif
          if(ip)printf("  %f   %f   %f\n",snode->normal[i].X,snode->normal[i].Y,snode->normal[i].Z) ;

          end_block(msfile,2) ;
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 SORT_VECTORS
 * =============================================================================
 */
        open_named_block(msfile, 1, SORT_VECTORS) ;
        if(ip)printf(" Token SORT_VECTORS found \n");
        snode->nsort_vectors = read_uint32_a(msfile) ;
        if(ip){
          printf("  nsort_vectors = %i\n",snode->nsort_vectors);
          printf("  position      = %i\n",(int)ftell(msfile->fp));
          printf("  block_end     = %i\n",msfile->level[1].byte_end);
        }
        if(0 != snode->nsort_vectors){
          snode->sort_vector = (Vector3 *)
                          malloc(snode->nsort_vectors*sizeof(Vector3));
          for(i=0;i<snode->nsort_vectors;i++){
// 2 VECTOR
            open_named_block(msfile, 2, VECTOR) ;
            if(ip)printf(" Token sort VECTOR found.  i = %i  ",i);
#ifdef geo_coord
            snode->sort_vector[i].X = read_real32_a(msfile);
            snode->sort_vector[i].Z = read_real32_a(msfile);
            snode->sort_vector[i].Y = read_real32_a(msfile);
#else
            snode->sort_vector[i].X = read_real32_a(msfile);
            snode->sort_vector[i].Y = read_real32_a(msfile);
            snode->sort_vector[i].Z = read_real32_a(msfile);
#endif
            if(ip)printf("  %f   %f   %f\n",snode->sort_vector[i].X,snode->sort_vector[i].Y,snode->sort_vector[i].Z) ;
            end_block(msfile,2) ;
          }
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 COLOURS
 * =============================================================================
 */
        open_named_block(msfile, 1, COLOURS) ;
        if(ip)printf(" Token COLOURS found \n");
        snode->ncolors = read_uint32_a(msfile) ;
        if(ip)printf("  ncolours = %i\n",snode->ncolors);
        if(0 != snode->ncolors){
           snode->color = (RGBA *)malloc(snode->ncolors*sizeof(RGBA));
          for(i=0;i<snode->ncolors;i++){
// 2 COLOUR
            open_named_block(msfile, 2, COLOUR) ;
            if(ip)printf(" Token COLOUR found \n");
            snode->color[i].A = read_real32_a(msfile);
            snode->color[i].R = read_real32_a(msfile);
            snode->color[i].G = read_real32_a(msfile);
            snode->color[i].B = read_real32_a(msfile);
            end_block(msfile,2) ;
          }
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 MATRICES
 * =============================================================================
 */
        open_named_block(msfile, 1, MATRICES) ;
        if(ip)printf(" Token MATRICES found \n");
        snode->nmatrices = read_uint32_a(msfile) ;
        if(ip)printf("  nmatrices = %i\n",snode->nmatrices);
        snode->matrix = NULL ;
        if(0 != snode->nmatrices) {
          snode->matrix = (Matrix4x3 *)
                              malloc(snode->nmatrices*sizeof(Matrix4x3));
          for(i=0;i<snode->nmatrices;i++){
// 2 MATRIX
            open_named_block(msfile, 2, MATRIX) ;
            snode->matrix[i].name = strdup(msfile->level[2].label) ;
            if(ip)printf(" Token MATRIX found.  i = %i,  name = %s\n",i,snode->matrix[i].name);
#if defined geo_coord
            snode->matrix[i].AX = read_real32_a(msfile);
            snode->matrix[i].AZ = read_real32_a(msfile);
            snode->matrix[i].AY = read_real32_a(msfile);
            snode->matrix[i].CX = read_real32_a(msfile);
            snode->matrix[i].CZ = read_real32_a(msfile);
            snode->matrix[i].CY = read_real32_a(msfile);
            snode->matrix[i].BX = read_real32_a(msfile);
            snode->matrix[i].BZ = read_real32_a(msfile);
            snode->matrix[i].BY = read_real32_a(msfile);
            snode->matrix[i].DX = read_real32_a(msfile);
            snode->matrix[i].DZ = read_real32_a(msfile);
            snode->matrix[i].DY = read_real32_a(msfile);
#else
            snode->matrix[i].AX = read_real32_a(msfile);
            snode->matrix[i].AY = read_real32_a(msfile);
            snode->matrix[i].AZ = read_real32_a(msfile);
            snode->matrix[i].BX = read_real32_a(msfile);
            snode->matrix[i].BY = read_real32_a(msfile);
            snode->matrix[i].BZ = read_real32_a(msfile);
            snode->matrix[i].CX = read_real32_a(msfile);
            snode->matrix[i].CY = read_real32_a(msfile);
            snode->matrix[i].CZ = read_real32_a(msfile);
            snode->matrix[i].DX = read_real32_a(msfile);
            snode->matrix[i].DY = read_real32_a(msfile);
            snode->matrix[i].DZ = read_real32_a(msfile);
#endif
            end_block(msfile,2) ;
/*
 *  Determine matrix type (no-op, translation, rotation)
 */
            snode->matrix[i].type = check_matrix4x3(&(snode->matrix[i])) ;
/*
 *  Determine corresponding animation class
 */
            if(!strncmp_ic(snode->matrix[i].name,"wheels",6)){
              if(7 == strlen(snode->matrix[i].name)){
                snode->matrix[i].anim = MAT_DRIVER_WHEEL ;
              }else{
                snode->matrix[i].anim = MAT_WHEEL ;
              }
            }else if(!strncmp_ic(snode->matrix[i].name,"rod",3)){
              snode->matrix[i].anim = MAT_ROD ;
            }else if(!strncmp_ic(snode->matrix[i].name,"bogie",5)){
              snode->matrix[i].anim = MAT_BOGIE ;
            }else if(!strncmp_ic(snode->matrix[i].name,"wiper",5)){
              snode->matrix[i].anim = MAT_WIPER ;
            }else if(!strncmp_ic(snode->matrix[i].name,"mirror",6)){
              snode->matrix[i].anim = MAT_MIRROR ;
            }else if(!strncmp_ic(snode->matrix[i].name,"pantograph",10)){
              snode->matrix[i].anim = MAT_PANTOGRAPH ;
            }else{
              snode->matrix[i].anim = MAT_NONE ;
            }
          }
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 IMAGES
 *   These are the names of the texture files
 * =============================================================================
 */
        open_named_block(msfile, 1, IMAGES) ;
        if(ip)printf(" Token IMAGES found \n");
        snode->n_textures = read_uint32_a(msfile) ;
        if(ip)printf("  nimages = %i\n",snode->n_textures);
        snode->texture_name = (char **)malloc(snode->n_textures*sizeof(char *));
        snode->texture = (TextureNode **)
                                malloc(snode->n_textures*sizeof(TextureNode *));
        for(i=0;i<snode->n_textures;i++){
          snode->texture[i] = NULL ;         // Set once textures have been read
// 2 IMAGE
          open_named_block(msfile, 2, IMAGE) ;
          snode->texture_name[i] = read_ucharz_a(msfile);
          zr_filename_MS2L(snode->texture_name[i]) ;
          if(ip)printf(" Token IMAGE found.  i = %i,  name = %s\n",i, snode->texture_name[i]);
          end_block(msfile,2) ;
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 *
 *  Token TEXTURE refers to a low level texture structure containing
 *  the index of an element in the list of texture images used by the shape
 *  plus flags to be used in displaying the texture.
 *
 *  The openrails code gives the structure the name Textures.  However this name
 *  is used by OpenGL and other graphics systems to describe the texture image.
 *  In ZR the alternative name TexLevelLow is used
 *
 *  Openrails provides the information:
 *    texture   ==> :uint,ImageIdx :uint,FilterMode
 *                                 :float,MipMapLODBias [:dword,BorderColor] .
 *    Provides attributes for each image e.g. texture ( 1 0 -3 ff000000 )
 *    MipMapLODBias  -3  fixes blurring, 0  can cause some texture blurring
 *
 * =============================================================================
 * 1 TEXTURES
 * =============================================================================
 */
        open_named_block(msfile, 1, TEXTURES) ;
        if(ip)printf(" Token TEXTURES found \n");
        snode->n_texlevel_low = read_uint32_a(msfile) ;
        if(ip)printf("  n_texlevel_low = %i\n",snode->n_texlevel_low);
        if(0 != snode->n_texlevel_low) {
          snode->texlevel_low = (TexLevelLow *)
                                   malloc(snode->n_texlevel_low*sizeof(TexLevelLow));
          for(i=0;i<snode->n_texlevel_low;i++){
// 2 TEXTURE
            open_named_block(msfile, 2, TEXTURE) ;
            if(ip)printf(" Token TEXTURE found,  i + %i\n",i);
            snode->texlevel_low[i].iImage        = read_int32_a(msfile);
            snode->texlevel_low[i].FilterMode    = read_int32_a(msfile);
            snode->texlevel_low[i].MipMapLODBias = read_real32_a(msfile);
            snode->texlevel_low[i].BorderColor   = read_uint32_a(msfile);
            end_block(msfile,2) ;
          }
        }
        end_block(msfile,1) ;
/*
 * *****************************************************************************
 *    light_material   ==> :
 *        dword,flags :uint,DiffColIdx :uint,AmbColIdx :uint,SpecColIdx :uint,EmissiveColIdx :float,SpecPower .
 *      Never seen it used eg light_materials ( 0 )
 * *****************************************************************************
 */
/*
 * =============================================================================
 * 1 LIGHT_MATERIALS
 * =============================================================================
 */
        open_named_block(msfile, 1, LIGHT_MATERIALS) ;
        if(ip)printf(" Token LIGHT_MATERIALS found \n");
        snode->nlight_materials = read_uint32_a(msfile) ;
        if(ip)printf("  nlight_materials = %i\n",snode->nlight_materials);
        if(0 != snode->nlight_materials) {
          snode->light_material = (LightMaterial *)
                        malloc(snode->nlight_materials*sizeof(LightMaterial));
          for(i=0;i<snode->nlight_materials;i++){
// 2 LIGHT_MATERIAL
            open_named_block(msfile, 2, LIGHT_MATERIAL) ;
            if(ip)printf(" Token LIGHT_MATERIAL found \n");
            snode->light_material[i].flags          = read_int32_a(msfile);
            snode->light_material[i].DiffColIdx     = read_int32_a(msfile);
            snode->light_material[i].AmbColIdx      = read_int32_a(msfile);
            snode->light_material[i].SpecColIdx     = read_int32_a(msfile);
            snode->light_material[i].EmissiveColIdx = read_int32_a(msfile);
            snode->light_material[i].SpecPower      = read_real32_a(msfile);
            end_block(msfile,2) ;
          }
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 LIGHT_MODEL_CFGS
 * =============================================================================
 */
        open_named_block(msfile, 1, LIGHT_MODEL_CFGS) ;
        if(ip)printf(" Token LIGHT_MODEL_CFGS found \n");
        snode->nlight_model_cfgs = read_uint32_a(msfile) ;
        if(ip)printf("  nlight_model_cfgs = %i\n",snode->nlight_model_cfgs);
        if(0 != snode->nlight_model_cfgs) {
          snode->light_model_cfg = (LightModelCfg *)
                     malloc(snode->nlight_model_cfgs*sizeof(LightModelCfg));
          for(i=0;i<snode->nlight_model_cfgs;i++){
// 2 LIGHT_MODEL_CFG
            LightModelCfg *light_model_cfg = &(snode->light_model_cfg[i]) ;
            open_named_block(msfile, 2, LIGHT_MODEL_CFG) ;
            if(ip)printf(" Token LIGHT_MODEL_CFG found \n");
            light_model_cfg->flags = read_uint32_a(msfile) ;
            {
// 3 UV_OPS
              open_named_block(msfile, 3, UV_OPS) ;
              if(ip)printf(" Token UV_OPS found \n");
              light_model_cfg->n_uv_ops = read_uint32_a(msfile) ;
              if(ip)printf("  n_uv_ops = %i\n",light_model_cfg->n_uv_ops);
              light_model_cfg->uv_ops = (UVOps *)
                     malloc(light_model_cfg->n_uv_ops*sizeof(UVOps));
              for(j=0;j<light_model_cfg->n_uv_ops;j++){
                UVOps *uv_ops = &(light_model_cfg->uv_ops[j]) ;
// 4 Open block - name one of a group
                itoken = open_block(msfile,4) ;
                switch(itoken) {
                  case(UV_OP_COPY) :
                    if(ip)printf(" Token UV_OP_COPY found \n");
                    uv_ops->type           = UV_OP_COPY    ;
                    uv_ops->tex_addr_mode  = read_int32_a(msfile);
                    uv_ops->src_uv_idx     = read_int32_a(msfile);
//                    printf(" UV_OP_COPY  uv_ops->tex_addr_mode = %i\n",uv_ops->tex_addr_mode);
//                    printf(" UV_OP_COPY  uv_ops->src_uv_idx    = %i\n",uv_ops->src_uv_idx);
                    break ;
                  case(UV_OP_REFLECTMAPFULL) :
                    if(ip)printf(" Token UV_OP_REFLECTMAPFULL found : %i\n",UV_OP_REFLECTMAPFULL);
                    uv_ops->type           = UV_OP_REFLECTMAPFULL    ;
                    uv_ops->tex_addr_mode  = read_int32_a(msfile);
                    break ;
                  case(UV_OP_REFLECTMAP) :
                    if(ip)printf(" Token UV_OP_REFLECTMAP found : %i\n",UV_OP_REFLECTMAP);
                    uv_ops->type           = UV_OP_REFLECTMAP    ;
                    uv_ops->tex_addr_mode  = read_int32_a(msfile);
                    break ;
                  case(UV_OP_UNIFORMSCALE) :
                    if(ip)printf(" Token UV_OP_UNIFORMSCALE found : %i\n",UV_OP_UNIFORMSCALE);
                    uv_ops->type            = UV_OP_UNIFORMSCALE    ;
                    uv_ops->tex_addr_mode   = read_int32_a(msfile);
                    uv_ops->src_uv_idx      = read_int32_a(msfile);
                    uv_ops->unknown_param_3 = read_real32_a(msfile);
                    break ;
                  case(UV_OP_NONUNIFORMSCALE) :
                    if(ip)printf(" Token UV_OP_NONUNIFORMSCALE found : %i\n",UV_OP_NONUNIFORMSCALE);
                    uv_ops->type            = UV_OP_NONUNIFORMSCALE    ;
                    uv_ops->unknown_param_3 = read_real32_a(msfile);
                    uv_ops->unknown_param_4 = read_real32_a(msfile);
                    break ;
                  default :
                    if(ip)printf("  Token not recognised.  itoken = %i, %s\n",itoken,token_idc[itoken]) ;
                    exit(1) ;
                    break ;
                }
                end_block(msfile,4);
              }
              end_block(msfile,3);
            }
            end_block(msfile,2);
          }
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 VTX_STATES
 * =============================================================================
 */
        open_named_block(msfile, 1, VTX_STATES) ;
        if(ip)printf(" Token VTX_STATES found \n");
        snode->n_vtx_states = read_uint32_a(msfile) ;
        if(ip)printf("  n_vtx_states = %i\n",snode->n_vtx_states);
        if(0 != snode->n_vtx_states) {
          snode->vtx_state = (VTXState *)
                        malloc(snode->n_vtx_states*sizeof(VTXState));
          for(i=0;i<snode->n_vtx_states;i++){
            VTXState *vtx_state = &(snode->vtx_state[i]) ;
// 2 VTX_STATE
            open_named_block(msfile, 2, VTX_STATE) ;
            if(ip)printf(" Token VTX_STATE found \n");
            vtx_state->flags          = read_uint32_a(msfile);
            vtx_state->imatrix        = read_int32_a(msfile);
            vtx_state->light_mat_idx  = read_int32_a(msfile);
            vtx_state->light_cfg_idx  = read_int32_a(msfile);
            vtx_state->light_flags    = read_uint32_a(msfile);
            if(!is_block_end(msfile,2,ip))
              vtx_state->imatrix2        = read_int32_a(msfile);
            else
              vtx_state->imatrix2        = 0.0           ;
            end_block(msfile,2) ;
          }
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 PRIM_STATES
 * =============================================================================
 */
        open_named_block(msfile, 1, PRIM_STATES) ;
        if(ip)printf(" Token PRIM_STATES found \n");
        snode->n_prim_states = read_uint32_a(msfile) ;
        if(ip)printf("  n_vtx_states = %i\n",snode->n_prim_states);
        if(0 != snode->n_prim_states) {
          snode->prim_state = (PrimState *)
                        malloc(snode->n_prim_states*sizeof(PrimState));
          for(i=0;i<snode->n_prim_states;i++){
            PrimState *prim_state = &(snode->prim_state[i]) ;
// 2 VTX_STATE
            open_named_block(msfile, 2, PRIM_STATE) ;
            if(ip)printf(" Token PRIM_STATE found \n");
            prim_state->name           = msfile->level[2].label ;
            if(ip)printf(" Token PRIM_STATE name = %s\n",msfile->level[2].label);
            msfile->level[2].label = NULL ;
            prim_state->flags          = read_uint32_a(msfile);
            prim_state->ishader        = read_int32_a(msfile);
            {
// 3 TEX_IDXS
               open_named_block(msfile, 3, TEX_IDXS) ;
               if(ip)printf(" Token TEX_IDXS found \n");
               prim_state->n_tex_idxs = read_int32_a(msfile);
               if(ip)printf("  n_tex_idxs = %i\n",prim_state->n_tex_idxs) ;
               if(0 != prim_state->n_tex_idxs) {
                 prim_state->tex_idx = (uint *)
                                 malloc(prim_state->n_tex_idxs*sizeof(uint)) ;
                 for(j=0;j<prim_state->n_tex_idxs;j++)
                   prim_state->tex_idx[j] = read_int32_a(msfile);
               }
               end_block(msfile,3) ;
            }
            prim_state->zbias           = read_real32_a(msfile);
            prim_state->ivtx_state      = read_int32_a(msfile);
            prim_state->alpha_test_mode = read_int32_a(msfile);
            prim_state->light_cfgidx    = read_int32_a(msfile);
            prim_state->zbuf_mode       = read_int32_a(msfile);
            end_block(msfile,2) ;
          }
        }
        end_block(msfile,1) ;
/*
 * =============================================================================
 * 1 LOD_CONTROLS
 * =============================================================================
 */
        open_named_block(msfile, 1, LOD_CONTROLS) ;
        if(ip)printf(" Token LOD_CONTROLS found \n");
        snode->n_lod_controls = read_uint32_a(msfile) ;
        if(ip)printf("  n_lod_controls = %i\n",snode->n_lod_controls);
        if(0 != snode->n_lod_controls) {
          snode->lod_control = (LodControl *)
                        malloc(snode->n_lod_controls*sizeof(LodControl));
          for(i=0;i<snode->n_lod_controls;i++){
            LodControl *lod_control = &(snode->lod_control[i]) ;
// 2 VTX_STATE
            open_named_block(msfile, 2, LOD_CONTROL) ;
            if(ip)printf(" Token LOD_CONTROL found \n");
// 3 DISTANCE_LEVELS_HEADER
            {
               open_named_block(msfile, 3, DISTANCE_LEVELS_HEADER) ;
               if(ip)printf(" Token DISTANCE_LEVELS_HEADER found \n");
                 lod_control->dlev_bias = read_int32_a(msfile) ;
               end_block(msfile,3) ;
            }
// 3 DISTANCE_LEVELS
            {
              open_named_block(msfile, 3, DISTANCE_LEVELS) ;
              if(ip)printf(" Token DISTANCE_LEVELS found \n");
              lod_control->n_dist_levels = read_int32_a(msfile) ;
              if(0 != lod_control->n_dist_levels) {
                lod_control->dist_level = (DistLevel *)
                          malloc(lod_control->n_dist_levels*sizeof(DistLevel));

                for(j=0;j<lod_control->n_dist_levels;j++){
                  DistLevel *dist_level = &(lod_control->dist_level[j]) ;
//  Initialise
                  dist_level->index            = j ;
                  dist_level->dlevel_selection = 0.0 ;
                  dist_level->n_hierarchy      = 0 ;
                  dist_level->n_sub_objects    = 0 ;
                  dist_level->gl_display_list  = 0 ;
                  dist_level->hierarchy        = NULL ;
                  dist_level->sub_object       = NULL ;
// 4 DISTANCE_LEVEL
                  open_named_block(msfile, 4, DISTANCE_LEVEL) ;
                  if(ip)printf(" Token DISTANCE_LEVEL found \n");
// 5 DISTANCE_LEVEL_HEADER
                  {
                    open_named_block(msfile, 5, DISTANCE_LEVEL_HEADER) ;
                    if(ip)printf(" Token DISTANCE_LEVEL_HEADER found \n");
// 6 DLEVEL_SELECTION
                    {
                      open_named_block(msfile, 6, DLEVEL_SELECTION) ;
                      if(ip)printf(" Token DLEVEL_SELECTION found \n");
                        dist_level->dlevel_selection = read_real32_a(msfile) ;
                      end_block(msfile,6) ;
                    }
// 6 HIERARCHY
                    {
                      open_named_block(msfile, 6, HIERARCHY) ;
                      if(ip)printf(" Token HIERARCHY found \n");
                        dist_level->n_hierarchy = read_int32_a(msfile) ;
                        if(0!=dist_level->n_hierarchy){
                          dist_level->hierarchy = (int *)malloc(
                            dist_level->n_hierarchy*sizeof(int)) ;
                          for(k=0;k<dist_level->n_hierarchy;k++)
                            dist_level->hierarchy[k] = read_int32_a(msfile) ;
                        }
                      end_block(msfile,6) ;
                    }
                    end_block(msfile,5) ;
                  }
// 5 SUB_OBJECTS
                  {
                    open_named_block(msfile, 5, SUB_OBJECTS) ;
                    if(ip)printf(" Token SUB_OBJECTS found \n");
                    dist_level->n_sub_objects = read_int32_a(msfile) ;
                    if(0!=dist_level->n_sub_objects){
                      dist_level->sub_object = (SubObject *)malloc(
                        dist_level->n_sub_objects*sizeof(SubObject)) ;
                      for(k=0;k<dist_level->n_sub_objects;k++){
                        SubObject *sub_object = &(dist_level->sub_object[k]) ;
// 6 SUB_OBJECT
                        init_sub_object(sub_object) ;
                        open_named_block(msfile, 6, SUB_OBJECT) ;
                        if(ip)printf(" Token SUB_OBJECT found \n");
// 7 SUB_OBJECT_HEADER
                        {
                          open_named_block(msfile, 7, SUB_OBJECT_HEADER) ;
                          if(ip)printf(" Token SUB_OBJECT_HEADER found \n");
                          sub_object->flags             = read_uint32_a(msfile) ;
                          sub_object->sort_vector_idx   = read_int32_a(msfile) ;
                          sub_object->vol_idx           = read_int32_a(msfile) ;
                          sub_object->src_vtx_fmt_flags = read_uint32_a(msfile) ;
                          sub_object->dst_vtx_fmt_flags = read_uint32_a(msfile) ;
// 8 GEOMETRY_INFO
                          {
                            open_named_block(msfile, 8, GEOMETRY_INFO) ;
                            if(ip)printf(" Token GEOMETRY_INFO found \n");
                            sub_object->face_normals         = read_int32_a(msfile) ;
                            sub_object->tx_light_cmds        = read_int32_a(msfile) ;
                            sub_object->nodex_tri_list_idxs  = read_int32_a(msfile) ;
                            sub_object->tri_list_idxs        = read_int32_a(msfile) ;
                            sub_object->linelist_idxs        = read_int32_a(msfile) ;
                            sub_object->nodex_tri_list_idxs2 = read_int32_a(msfile) ;
                            sub_object->tri_lists            = read_int32_a(msfile) ;
                            sub_object->line_lists           = read_int32_a(msfile) ;
                            sub_object->pt_lists             = read_int32_a(msfile) ;
                            sub_object->nodex_tri_lists      = read_int32_a(msfile) ;
// 9 GEOMETRY_NODES
                            {
                              open_named_block(msfile, 9, GEOMETRY_NODES) ;
                              if(ip)printf(" Token GEOMETRY_NODES found \n");
                              sub_object->n_geom_nodes = read_int32_a(msfile) ;
                              if(0!=sub_object->n_geom_nodes){
                                sub_object->geom_node = (GeomNode *)malloc(
                                  sub_object->n_geom_nodes*sizeof(GeomNode)) ;
                                for(l=0;l<sub_object->n_geom_nodes;l++){
                                  GeomNode *geom_node = &(sub_object->geom_node[l]) ;
// 10 GEOMETRY_NODE
                                  open_named_block(msfile, 10, GEOMETRY_NODE) ;
                                  if(ip)printf(" Token GEOMETRY_NODE found \n");
                                  geom_node->tx_light_cmds       = read_int32_a(msfile) ;
                                  geom_node->nodex_tx_light_cmds = read_int32_a(msfile) ;
                                  geom_node->tri_lists           = read_int32_a(msfile) ;
                                  geom_node->line_lists          = read_int32_a(msfile) ;
                                  geom_node->pt_lists            = read_int32_a(msfile) ;
// 11 CULLABLE_PRIMS
                                  {
                                    open_named_block(msfile, 11, CULLABLE_PRIMS) ;
                                    if(ip)printf(" Token CULLABLE_PRIMS found \n");
                                      geom_node->num_prims         = read_int32_a(msfile) ;
                                      geom_node->num_flat_sections = read_int32_a(msfile) ;
                                      geom_node->num_prim_idxs     = read_int32_a(msfile) ;
                                    end_block(msfile,11) ;
                                  }
                                  end_block(msfile,10) ;
                                }                // FOR
                              }                  // if
                              end_block(msfile,9); // GEOMETRY_NODES
                            }
 // 9 GEOMETRY_NODE_MAP
                            {
                              open_named_block(msfile, 9, GEOMETRY_NODE_MAP) ;
                              if(ip)printf(" Token GEOMETRY_NODE_MAP found \n");
                              sub_object->n_node_maps = read_int32_a(msfile) ;
                              if(ip)printf(" n_node_maps = %i\n",sub_object->n_node_maps) ;
                              if(0!=sub_object->n_node_maps){
                                sub_object->node_map = (uint *)malloc(
                                   sub_object->n_node_maps*sizeof(uint)) ;
                                for(l=0;l<sub_object->n_node_maps;l++)
                                  sub_object->node_map[l] = read_int32_a(msfile) ;
                              }
                              end_block(msfile,9) ;  // GEOMETRY_NODE_MAP
                            }
                            end_block(msfile,8);     // GEOMETRY_INFO
                          }
/*
 *   SUB_OBJECT_HEADER has three optional sections
 *   7.1.  shaders
 */
                          if(ip)printf(" SUB_OBJECT_HEADER Section 1 \n");
                          if(!is_block_end(msfile,7,ip)){
//  8  SUBOBJECT_SHADERS
                            open_named_block(msfile, 8, SUBOBJECT_SHADERS) ;
                            if(ip)printf(" Token SUBOBJECT_SHADERS found \n");
                            sub_object->n_shaders = read_int32_a(msfile);
                            if(ip)printf(" n_shaders = %i\n",sub_object->n_shaders) ;
                            if(0!=sub_object->n_shaders){
                              sub_object->shader = (uint *)malloc(
                                    sub_object->n_shaders*sizeof(uint)) ;
                              for(l=0;l<sub_object->n_shaders;l++){
                                sub_object->shader[l] = read_int32_a(msfile) ;
                              }
                            }
                            end_block(msfile,8) ;
                          }
//   7.2.  light_cfgs
                          if(ip)printf(" SUB_OBJECT_HEADER Section 2 \n");
                          if(!is_block_end(msfile,7,ip)){
//  8  SUBOBJECT_LIGHT_CFGS
                            open_named_block(msfile, 8, SUBOBJECT_LIGHT_CFGS) ;
                            if(ip)printf(" Token SUBOBJECT_LIGHT_CFGS found \n");
                            sub_object->n_light_cfgs = read_int32_a(msfile);
                            if(ip)printf(" n_light_cfgs = %i\n",sub_object->n_light_cfgs) ;
                            if(0!=sub_object->n_light_cfgs){
                              sub_object->light_cfg = (uint *)malloc(
                                    sub_object->n_light_cfgs*sizeof(uint)) ;
                              for(l=0;l<sub_object->n_light_cfgs;l++){
                                sub_object->light_cfg[l] = read_int32_a(msfile) ;
                              }
                            }
                            end_block(msfile,8) ;
                          }
//   3.Unknown
                          if(ip)printf(" SUB_OBJECT_HEADER Section 3 \n");
                          if(!is_block_end(msfile,6,0)){
                            read_int32_a(msfile);
                          }
                          end_block(msfile,7) ;     // SUB_OBJECT_HEADER
                        }
/*
 *  each SUB_OBJECT also has vertices, vertex sets and primitives
 */
// 7 VERTICES
                        {
                          open_named_block(msfile, 7, VERTICES) ;
                          if(ip)printf(" Token VERTICES found \n");
                          sub_object->n_vertices = read_int32_a(msfile);
                          if(ip)printf(" n_vertices = %i\n",sub_object->n_vertices) ;
                          if(0!=sub_object->n_vertices){
                            sub_object->vertex = (Vertex *)malloc(
                                sub_object->n_vertices*sizeof(Vertex));
                            for(l=0;l<sub_object->n_vertices;l++){
                              Vertex *vertex  = &(sub_object->vertex[l]) ;
// 8  VERTEX
                              open_named_block(msfile, 8, VERTEX) ;
                              if(ip)printf(" Token VERTEX found.  l = %i\n",l);
                              vertex->flags   = read_uint32_a(msfile) ;
                              vertex->ipoint  = read_int32_a(msfile) ;
                              vertex->inormal = read_int32_a(msfile) ;
                              vertex->color1  = read_int32_a(msfile) ;
                              vertex->color2  = read_int32_a(msfile) ;
                              {
                                open_named_block(msfile, 9, VERTEX_UVS) ;
                                if(ip)printf(" Token VERTEX_UVS found \n");
                                vertex->n_vertex_uvs = read_int32_a(msfile);
                                if(ip)printf(" n_vertex_uvs = %i\n",vertex->n_vertex_uvs) ;
                                if(0!=vertex->n_vertex_uvs){
                                  vertex->vertex_uv = (uint *)malloc(
                                      vertex->n_vertex_uvs*sizeof(uint)) ;
                                  for(m=0;m<(uint)(vertex->n_vertex_uvs);m++)
                                      vertex->vertex_uv[m] = read_int32_a(msfile) ;
                                }
                                end_block(msfile,9) ;
                              }
                              end_block(msfile,8) ;
                            }
                          }
                          end_block(msfile,7) ;
                        }
// 7 VERTEX_SETS
                        {
                          open_named_block(msfile, 7, VERTEX_SETS) ;
                          if(ip)printf(" Token VERTEX_SETS found \n");
                          sub_object->n_vertex_sets = read_int32_a(msfile);
                          if(ip)printf(" n_vertex_sets = %i\n",sub_object->n_vertex_sets) ;
                          if(0!=sub_object->n_vertex_sets){
                            sub_object->vertex_set = (VertexSet *)malloc(
                                sub_object->n_vertex_sets*sizeof(VertexSet));
                            for(l=0;l<sub_object->n_vertex_sets;l++){
                              VertexSet *vertex_set  = &(sub_object->vertex_set[l]) ;
                              open_named_block(msfile, 8, VERTEX_SET) ;
                              if(ip)printf(" Token VERTEX_SET found \n");
                              vertex_set->vtx_state_idx   = read_uint32_a(msfile) ;
                              vertex_set->start_vtx_idx   = read_uint32_a(msfile) ;
                              vertex_set->vtx_count       = read_uint32_a(msfile) ;
                              end_block(msfile,8) ;
                            }
                          }
                          end_block(msfile,7) ;
                        }
// 7 PRIMITIVES
                        {
                          int  last_prim_state_idx = 0 ;
                          int  token  ;
                          uint icount ;
                          TriList  *tri_list ;
                          open_named_block(msfile, 7, PRIMITIVES) ;
 //                         sub_object->n_primitives = read_int32_a(msfile);
                          icount = read_int32_a(msfile);
                          sub_object->n_tri_lists = icount ;
                          if(ip)printf(" Open block PRIMITIVES at level 7.  Number = %i\n",icount);
                          if(0!=icount){
                            sub_object->tri_list = (TriList *)malloc(icount*sizeof(TriList));
                            for(m=0;m<icount;m++)init_tri_list(&(sub_object->tri_list[m])) ;
                            for(l=0,m=0;l<icount;l++){
                              if(ip)printf("    Open block at level 8\n") ;
                              token = open_block(msfile,8) ;
                              switch(token) {
                                case(PRIM_STATE_IDX):
                                  last_prim_state_idx = read_int32_a(msfile) ;
                                  if(ip)printf("     Token PRIM_STATE_IDX found.  last_prim_state_idx = %i\n",last_prim_state_idx);
                                  break ;
                                case(INDEXED_TRILIST):
                                  if(ip)printf("     Token INDEXED_TRILIST found \n");
                                  tri_list = &(sub_object->tri_list[++m]) ;
                                  tri_list->prim_state_trilist = last_prim_state_idx ;
                                  {
//                                    open_named_block(msfile, 9, INDEXED_TRILIST) ;
//                                    if(ip)printf(" Token INDEXED_TRILIST found \n");
                                    {
                                      if(ip)printf("     Search for block: VERTEX_IDXS \n");
                                      open_named_block(msfile, 9, VERTEX_IDXS) ;
                                      if(ip)printf("     Token VERTEX_IDXS found.");
                                      tri_list->n_vertex_idxs = read_int32_a(msfile)/3 ;
                                      if(ip)printf("     n_vertex_idxs = %i\n",tri_list->n_vertex_idxs) ;
                                      if(0!=tri_list->n_vertex_idxs){
                                        tri_list->vertex_idx = (VertexIdx *)malloc(
                                          tri_list->n_vertex_idxs*sizeof(VertexIdx)) ;
                                        for(n=0;n<tri_list->n_vertex_idxs;n++){
                                          tri_list->vertex_idx[n].a = read_int32_a(msfile);
                                          tri_list->vertex_idx[n].b = read_int32_a(msfile);
                                          tri_list->vertex_idx[n].c = read_int32_a(msfile);
                                        }
                                      }
                                      end_block(msfile,9) ;

                                      if(ip)printf("     Search for block: NORMAL_IDXS \n");
                                      open_named_block(msfile, 9, NORMAL_IDXS) ;
                                      if(ip)printf("     Token NORMAL_IDXS found.");
                                      tri_list->n_normal_idxs = read_int32_a(msfile);
                                      if(ip)printf("     n_normal_idxs = %i\n",tri_list->n_normal_idxs) ;
                                      if(0!=tri_list->n_normal_idxs){
                                        tri_list->normal_idx = (uint *)malloc(
                                          tri_list->n_normal_idxs*sizeof(uint)) ;
                                        for(n=0;n<tri_list->n_normal_idxs;n++){
                                          tri_list->normal_idx[n] = read_int32_a(msfile) ;
                                          read_int32_a(msfile) ;
                                        }
                                      }
                                      end_block(msfile,9) ;

                                      if(ip)printf("     Search for block: FLAGS \n");
                                      open_named_block(msfile, 9, FLAGS) ;
                                      if(ip)printf("     Token FLAGS found.");
                                      tri_list->n_flags = read_int32_a(msfile);
                                      if(ip)printf("     n_flags = %i\n",tri_list->n_flags) ;
                                      if(0!=tri_list->n_flags){
                                        tri_list->flag = (uint *)malloc(
                                                    tri_list->n_flags*sizeof(uint)) ;
                                        for(n=0;n<tri_list->n_flags;n++)
                                                    tri_list->flag[n] = read_int32_a(msfile) ;
                                      }
                                      end_block(msfile,9) ;
                                    }
                                  }
                                  break ;
                              }
                              end_block(msfile,8) ;
                              if(ip)printf("    Close block at level 8\n") ;
                            }
                          }
                          end_block(msfile,7) ;
                          if(ip)printf("  Close block at level 7\n") ;
                        }
                        end_block(msfile,6) ;    //SUB_OBJECT
                      }                        // for
                    }                          // if
                    end_block(msfile,5) ;    //SUB_OBJECTS
                  }
                  end_block(msfile,4) ;  // DISTANCE_LEVEL
                }                        // for j
              }                          // if
              end_block(msfile,3) ;      // DISTANCE_LEVELS
            }
            end_block(msfile,2) ;        // LOD_CONTROL
          }                              // for i
        }                                // if
        end_block(msfile,1) ;            // LOD_CONTROLS
/*
 * =============================================================================
 * 1 ANIMATIONS
 *    n_animations -> array of ANIMATION structures
 *        n_anim_nodes -> array of AMIN_NODE structues
 *            n_controllers ->  array of CONTROLLER structures
 *                n_keys          ->  array of KEY structures
 * =============================================================================
 */

        if(!is_block_end(msfile, 0, 0)){

          if(ip)printf("  ANIMATIONS in file : %s\n",snode->name) ;

          open_named_block(msfile, 1, ANIMATIONS)  ;
          if(ip)printf(" Token ANIMATIONS found \n") ;
          snode->n_animations = read_int32_a(msfile) ;
          if(ip)printf(" n_animations = %i\n",snode->n_animations);
          if(0<snode->n_animations){
            snode->animation = (Animation *)malloc(
                                          snode->n_animations*sizeof(Animation)) ;
            for(i=0;i<snode->n_animations;i++){
              Animation *animation = &(snode->animation[i]) ;
//  2  ANIMATION
              open_named_block(msfile, 2, ANIMATION)  ;
              if(ip)printf(" Token ANIMATION found \n") ;
              animation->frame_count = read_int32_a(msfile) ;
              animation->frame_rate  = read_int32_a(msfile) ;
//  3  ANIM_NODES
              {
                open_named_block(msfile, 3, ANIM_NODES)  ;
                if(ip)printf(" Token ANIM_NODES found \n") ;
                animation->n_anim_nodes = read_int32_a(msfile) ;
                if(ip)printf(" n_anim_nodes = %i\n",animation->n_anim_nodes);
                if(0<animation->n_anim_nodes){
                  animation->anim_node = (AnimNode *)malloc(
                                  animation->n_anim_nodes*sizeof(AnimNode)) ;
                  for(j=0;j<animation->n_anim_nodes;j++){
                    AnimNode *anim_node = &(animation->anim_node[j]);
//  4  ANIM_NODE
                    open_named_block(msfile, 4, ANIM_NODE)  ;
                    if(ip)printf(" Token ANIM_NODE found \n") ;
                    anim_node->name = msfile->level[4].label ;
                    msfile->level[4].label = NULL ;
                    {
//  5 CONTROLLERS
                      open_named_block(msfile, 5, CONTROLLERS)  ;
                      if(ip)printf(" Token CONTROLLERS found \n") ;
                      anim_node->n_controllers = read_int32_a(msfile) ;
                      if(ip)printf(" n_controllers = %i\n",anim_node->n_controllers);
                      if(0<anim_node->n_controllers){
                        anim_node->controller = (AnimController *)malloc(
                            anim_node->n_controllers*sizeof(AnimController)) ;
                        for(k=0;k<anim_node->n_controllers;k++){
                          AnimController *controller = &(anim_node->controller[k]) ;
//  6  CONTROLLER (SWITCH)
                          itoken = open_block(msfile,6) ;
                          switch(itoken){
                            case(LINEAR_POS) :
                              controller->type = LINEAR_POS;
                              controller->n_anim_keys = read_int32_a(msfile) ;
                              if(ip)printf(" n_anim_keys = %i\n",controller->n_anim_keys);
                              if(0!=controller->n_anim_keys){
                                controller->anim_key = (AnimKey *)malloc(
                                    controller->n_anim_keys*sizeof(AnimKey)) ;
                                for(l=0;l<controller->n_anim_keys;l++){
                                  AnimKey *anim_key = &(controller->anim_key[l]) ;
//  7  LINEAR_KEY
                                  open_named_block(msfile, 7, LINEAR_KEY)  ;
                                  if(ip)printf(" Token LINEAR_KEY found \n") ;
                                  anim_key->type  = LINEAR_KEY ;
                                  anim_key->frame = read_int32_a(msfile) ;
                                  anim_key->X     = read_real32_a(msfile) ;
                                  anim_key->Y     = read_real32_a(msfile) ;
                                  anim_key->Z     = read_real32_a(msfile) ;
                                  end_block(msfile,7) ;
                                }
                              }
                              break;
                            case(TCB_POS) :
                              controller->type = TCB_POS;
                              controller->n_anim_keys = read_int32_a(msfile) ;
                              if(ip)printf(" n_anim_keys = %i\n",controller->n_anim_keys);
                              if(0!=controller->n_anim_keys){
                                controller->anim_key = (AnimKey *)malloc(
                                    controller->n_anim_keys*sizeof(AnimKey)) ;
                                for(l=0;l<controller->n_anim_keys;l++){
                                  AnimKey *anim_key = &(controller->anim_key[l]) ;
//  7  TCB_KEY
                                  open_named_block(msfile, 7, TCB_KEY)  ;
                                  if(ip)printf(" Token TCB_KEY found \n") ;
                                  anim_key->type      = TCB_KEY ;
                                  anim_key->frame     = read_int32_a(msfile) ;
                                  anim_key->X         = read_real32_a(msfile) ;
                                  anim_key->Y         = read_real32_a(msfile) ;
                                  anim_key->Z         = read_real32_a(msfile) ;
                                  anim_key->W         = read_real32_a(msfile) ;
                                  anim_key->tension   = read_real32_a(msfile) ;
                                  anim_key->continuity = read_real32_a(msfile) ;
                                  anim_key->bias      = read_real32_a(msfile) ;
                                  anim_key->in        = read_real32_a(msfile) ;
                                  anim_key->out       = read_real32_a(msfile) ;
                                  end_block(msfile,7) ;
                                }
                              }
                              break;
                            case(TCB_ROT):
                              controller->type = TCB_ROT;
                              controller->n_anim_keys = read_int32_a(msfile) ;
                              if(ip)printf(" n_anim_keys = %i\n",controller->n_anim_keys);
                              if(0!=controller->n_anim_keys){
                                controller->anim_key = (AnimKey *)malloc(
                                    controller->n_anim_keys*sizeof(AnimKey)) ;
                                for(l=0;l<controller->n_anim_keys;l++){
                                  AnimKey *anim_key = &(controller->anim_key[l]) ;
//  7  KEY (SWITCH)
                                  itoken = open_block(msfile,7) ;
                                  switch(itoken){
                                    case(SLERP_ROT):
//                                      open_named_block(msfile, 8, SLERP_ROT) ;
                                      if(ip)printf(" Token SLERP_ROT found \n") ;
                                      anim_key->type  = SLERP_ROT ;
                                      anim_key->frame = read_int32_a(msfile) ;
                                      anim_key->X     = read_real32_a(msfile) ;
                                      anim_key->Y     = read_real32_a(msfile) ;
                                      anim_key->Z     = read_real32_a(msfile) ;
                                      anim_key->W     = read_real32_a(msfile) ;
//                                      end_block(msfile,8) ;
                                      break ;
                                    case(TCB_KEY):
//                                      open_named_block(msfile, 8, TCB_KEY) ;
                                      if(ip)printf(" Token TCB_KEY found \n") ;
                                      anim_key->type      = TCB_KEY ;
                                      anim_key->frame     = read_int32_a(msfile) ;
                                      anim_key->X         = read_real32_a(msfile) ;
                                      anim_key->Y         = read_real32_a(msfile) ;
                                      anim_key->Z         = read_real32_a(msfile) ;
                                      anim_key->W         = read_real32_a(msfile) ;
                                      anim_key->tension   = read_real32_a(msfile) ;
                                      anim_key->continuity = read_real32_a(msfile) ;
                                      anim_key->bias      = read_real32_a(msfile) ;
                                      anim_key->in        = read_real32_a(msfile) ;
                                      anim_key->out       = read_real32_a(msfile) ;
//                                      end_block(msfile,8) ;
                                      break ;
                                    default:
                                      if(ip)printf(" Unexpected animation key\n") ;
                                      if(ip)printf("   Token = %i, %s\n",itoken,token_idc[itoken]);
                                      if(ip)printf("   Program stopping ... \n\n");
                                      exit(1) ;
                                  }
                                  end_block(msfile,7) ;
                                }
                              break;
                            default:
                              if(ip)printf(" Unexpected animation \n") ;
                              if(ip)printf("   Token = %i, %s\n",itoken,token_idc[itoken]);
                              if(ip)printf("   Program stopping ... \n\n");
                              exit(1) ;
                          }
                        }
                        end_block(msfile,6) ;
                      }
                    }
                    end_block(msfile,5) ;
                  }
                  end_block(msfile,4) ;
                }
              }
              end_block(msfile,3) ;
            }
            end_block(msfile,2) ;
          }
        }
        end_block(msfile,1) ;
      }
      close_msfile(msfile);

/*
 *  EXIT
 */
      if(ip){
        printf(" *****************************************************************\n") ;
        printf(" *   EXIT  ROUTINE  %s\n",my_name) ;
        printf(" *****************************************************************\n") ;
      }
      return 0 ;
}

/*
 *  Routine to print data 'gleaned' from the shape file
 */

int  print_shape_file_data(ShapeNode *snode){

  uint  i, j, k, l, m, ll, nn;
  uint  ip = 0 ; // 1 = Printing of trilists
  char *my_name = "print_shape_file_data" ;

      printf("\n *****************************************************************\n") ;
      printf(" *   ENTER  ROUTINE  %s\n",my_name) ;
      printf(" *****************************************************************\n") ;
      printf( "  shape   = %s\n",snode->name ) ;
      printf( "  s_file  = %s\n",snode->s_file ) ;
      printf( "  sd_file = %s\n",snode->sd_file ) ;


#if 1
      printf( "  basic     = %i\n",snode->basic  ) ;
      printf( "  needed    = %i\n",snode->needed ) ;
      printf( "  loaded    = %i\n",snode->loaded ) ;
      printf( "  flags1    = %x\n",snode->flags1 ) ;
      printf( "  flags2    = %x\n",snode->flags2 ) ;
      printf( "  nvolumes  = %i\n",snode->nvolumes ) ;
      for(i=0;i<snode->nvolumes;i++){
        printf( "      volume %i radius = %f\n",i,snode->shape_vol[i].radius) ;
        printf( "                vector = %f %f %f\n",
                         snode->shape_vol[i].vec.X,
                         snode->shape_vol[i].vec.Y,
                         snode->shape_vol[i].vec.Z) ;
      }
      printf( "  nshaders  = %i\n",snode->nshaders ) ;
      for(i=0;i<snode->nshaders;i++)
        printf( "      shader   %i = %s\n",i,snode->shader[i] ) ;
      printf( "  nfilters  = %i\n",snode->nfilters ) ;
      for(i=0;i<snode->nfilters;i++)
        printf( "      filter   %i = %s\n",i,snode->filter[i] ) ;
      printf( "  npoints   = %i\n",snode->npoints ) ;
      printf( "  nnormals  = %i\n",snode->nnormals ) ;
      printf( "  nuvpoints = %i\n",snode->nuvpoints ) ;
      printf( "  nsort_vectors      = %i\n",snode->nsort_vectors ) ;
      printf( "  ncolors            = %i\n",snode->ncolors ) ;
      for(i=0;i<snode->ncolors;i++)printf("  Colour %i :: R G B = %f %f %f :: A = %f\n",
                                          i, snode->color[i].R, snode->color[i].G,
                                             snode->color[i].B, snode->color[i].A) ;
      printf( "  nmatrices          = %i\n",snode->nmatrices ) ;
      for(i=0;i<snode->nmatrices;i++){
        printf("  Matrix      = %i\n",i) ;
        printf("  Matrix name = %s\n",snode->matrix[i].name) ;
        printf("  AX, AY, AZ  = %f %f %f\n",
                 snode->matrix[i].AX,snode->matrix[i].AY,snode->matrix[i].AZ) ;
        printf("  BX, BY, BZ  = %f %f %f\n",
                 snode->matrix[i].BX,snode->matrix[i].BY,snode->matrix[i].BZ) ;
        printf("  CX, CY, CZ  = %f %f %f\n",
                 snode->matrix[i].CX,snode->matrix[i].CY,snode->matrix[i].CZ) ;
        printf("  DX, DY, DZ  = %f %f %f\n",
                 snode->matrix[i].DX,snode->matrix[i].DY,snode->matrix[i].DZ) ;
      }
      printf( "  n_textures         = %i\n",snode->n_textures ) ;
      for(i=0;i<snode->n_textures;i++){
        printf("    texture name %i = %s, pointer = %p\n",
                          i,snode->texture_name[i],(void *)snode->texture[i]) ;
      }
      printf( "  n_texlevel_low     = %i\n",snode->n_texlevel_low ) ;
      for(i=0;i<snode->n_texlevel_low;i++){
TexLevelLow *tl_low;
        tl_low = &(snode->texlevel_low[i]) ;
        printf("    iImage        %i = %i\n",i,tl_low->iImage) ;
        printf("    FilterMode    %i = %i\n",i,tl_low->FilterMode) ;
        printf("    MipMapLODBias %i = %f\n",i,(double)tl_low->MipMapLODBias) ;
        printf("    BorderColor   %i = %i\n",i,tl_low->BorderColor) ;
      }
      printf( "  nlight_materials   = %i\n",snode->nlight_materials ) ;
      printf( "  nlight_model_cfgs  = %i\n",snode->nlight_model_cfgs ) ;
      printf( "  n_vtx_states       = %i\n",snode->n_vtx_states ) ;
      printf( "  n_prim_states      = %i\n",snode->n_prim_states ) ;
      printf( "  dlevel_selection   = %i\n",snode->dlevel_selection ) ;
      printf( "  nhierarchy         = %i\n",snode->nhierarchy ) ;
      printf( "  n_lod_controls     = %i\n",snode->n_lod_controls ) ;
      printf( "  n_animations       = %i\n",snode->n_animations ) ;

      printf( "\n  npoints   = %i\n",snode->npoints ) ;
      nn = snode->npoints ;
//      for(i=0;i<nn;i++)
      for(i=0;i<(nn>20 ? 20 : nn);i++)
        printf( "  point   %4i = %10f  %10f %10f \n",i,snode->point[i].X,snode->point[i].Z,snode->point[i].Y ) ;

      printf( "\n  nnormals  = %i\n",snode->nnormals ) ;
      nn = snode->nnormals ;
//      for(i=0;i<nn;i++)
      for(i=0;i<(nn>20 ? 20 : nn);i++)
        printf( "  normal   %4i = %10f  %10f %10f \n",i,snode->normal[i].X,snode->normal[i].Z,snode->normal[i].Y ) ;

      printf( "\n  nuvpoints = %i\n",snode->nuvpoints ) ;
      nn = snode->nuvpoints ;
      for(i=0;i<(nn>20 ? 20 : nn);i++)
        printf( "  uvpoint   %4i = %10f  %10f \n",i,snode->uvpoint[i].U,snode->uvpoint[i].V ) ;

      printf( "\n  nsort_vectors      = %i\n",snode->nsort_vectors ) ;
      nn = snode->nsort_vectors ;
      for(i=0;i<(nn>20 ? 20 : nn);i++)
        printf( "  sort_vector   %4i = %10f  %10f %10f \n",i,snode->sort_vector[i].X,snode->sort_vector[i].Z,snode->sort_vector[i].Y ) ;

      printf( "\n  nlight_model_cfgs      = %i\n",snode->nlight_model_cfgs ) ;
      nn = snode->nlight_model_cfgs ;
      for(i=0;i<(nn>20 ? 20 : nn);i++){
        printf( "  light_model_cfg   %4i,  flags = %X, n_uv_ops = %i\n",
                i,snode->light_model_cfg[0].flags,snode->light_model_cfg[0].n_uv_ops) ;
        for(j=0; j<snode->light_model_cfg[0].n_uv_ops; j++){
UVOps *uv_ops = &(snode->light_model_cfg[0].uv_ops[j]) ;
          printf(" { %i, %i, %i, %f, %f } \n", uv_ops->type, uv_ops->tex_addr_mode, uv_ops->src_uv_idx, uv_ops->unknown_param_3, uv_ops->unknown_param_4 ) ;
        }
      }




      printf( "\n  n_vtx_states       = %i\n",snode->n_vtx_states ) ;
      nn = snode->n_vtx_states ;
      for(i=0;i<(nn>40 ? 40 : nn);i++){
        printf( "    flags           %i = %i\n",i,snode->vtx_state[i].flags ) ;
        printf( "      imatrix       %i = %i\n",i,snode->vtx_state[i].imatrix ) ;
        if(snode->matrix && i<snode->nmatrices){
          printf( "         AX, AY, AZ = %9f %9f %9f\n",
                         snode->matrix[i].AX,snode->matrix[i].AY,snode->matrix[i].AZ) ;
          printf( "         BX, BY, BZ = %9f %9f %9f\n",
                         snode->matrix[i].BX,snode->matrix[i].BY,snode->matrix[i].BZ) ;
          printf( "         CX, CY, CZ = %9f %9f %9f\n",
                         snode->matrix[i].CX,snode->matrix[i].CY,snode->matrix[i].CZ) ;
          printf( "         DX, DY, DZ = %9f %9f %9f\n",
                         snode->matrix[i].DX,snode->matrix[i].DY,snode->matrix[i].DZ) ;
        }
        printf( "      light_mat_idx %i = %i\n",i,snode->vtx_state[i].light_mat_idx ) ;
        printf( "      light_cfg_idx %i = %i\n",i,snode->vtx_state[i].light_cfg_idx ) ;
        printf( "      light_flags   %i = %i\n",i,snode->vtx_state[i].light_flags ) ;
        printf( "      imatrix2      %i = %i\n",i,snode->vtx_state[i].imatrix2 ) ;
      }
      printf( "\n  n_prim_states       = %i\n",snode->n_prim_states ) ;
      nn = snode->n_prim_states ;
      for(i=0;i<nn;i++){
         printf( "    PrimState name    %i = %s\n",i,snode->prim_state[i].name ) ;
         printf( "      flags           %i = %i\n",i,snode->prim_state[i].flags ) ;
         printf( "      ishader         %i = %i\n",i,snode->prim_state[i].ishader ) ;
         printf( "      n_tex_idxs      %i = %i\n",i,snode->prim_state[i].n_tex_idxs ) ;
         for(j=0;j<snode->prim_state[i].n_tex_idxs;j++){
           printf("      tex_idx         %i %i = %i\n",i,j, snode->prim_state[i].tex_idx[j]);
         }
         printf( "      zbias           %i = %f\n",i,snode->prim_state[i].zbias ) ;
         printf( "      ivtx_state      %i = %i\n",i,snode->prim_state[i].ivtx_state ) ;
         printf( "      alpha_test_mode %i = %i\n",i,snode->prim_state[i].alpha_test_mode ) ;
         printf( "      light_cfgidx    %i = %i\n",i,snode->prim_state[i].light_cfgidx ) ;
         printf( "      zbuf_mode       %i = %i\n",i,snode->prim_state[i].zbuf_mode ) ;
      }

      printf("==============================================================================\n");
      printf("=====  Start LOD                          ====================================\n");
      printf("==============================================================================\n");

      printf( "\n  n_lod_controls     = %i\n",snode->n_lod_controls ) ;
      for(i=0;i<snode->n_lod_controls;i++){
        printf( "    lod_control        = %i\n",i ) ;
        printf( "    dlev_bias          = %i\n",snode->lod_control[i].dlev_bias ) ;
        printf( "      n_dist_levels      = %i\n",snode->lod_control[i].n_dist_levels ) ;
        for(j=0;j<snode->lod_control[i].n_dist_levels;j++){
DistLevel *dist_level = &(snode->lod_control[i].dist_level[j]) ;
        printf("==============================================================================\n");
        printf("=======  Start dist_level                 ====================================\n");
        printf("==============================================================================\n");
          printf( "      dist_level        = %i\n",j ) ;
          printf( "      dlevel_selection  = %f\n",dist_level->dlevel_selection ) ;
          printf( "      n_hierarchy        = %i\n",dist_level->n_hierarchy ) ;
          printf( "      n_sub_objects      = %i\n",dist_level->n_sub_objects ) ;

          for(k=0;k<dist_level->n_hierarchy;k++)
            printf("               Heirarchy %3i = %3i\n",k, dist_level->hierarchy[k]);
          printf("..............................................................\n") ;

          printf("==============================================================================\n");
          printf("=========  Start Sub-Objects              ====================================\n");
          printf("==============================================================================\n");
          printf( "      Number of Sub-Objects = %i\n", dist_level->n_sub_objects) ;

          for(k=0;k<dist_level->n_sub_objects;k++){
SubObject *sub_object = &(dist_level->sub_object[k]) ;
            printf("==============================================================================\n");
            printf("=========  Start New SubObject            ====================================\n");
            printf("==============================================================================\n");
            printf( "      Sub Object index      = %i\n", k) ;
            printf( "      flags                 = %i\n",sub_object->flags ) ;
            printf( "      sort_vector_idx       = %i\n",sub_object->sort_vector_idx ) ;
            printf( "      vol_idx               = %i\n",sub_object->vol_idx ) ;
            printf( "      src_vtx_fmt_flags     = %i\n",sub_object->src_vtx_fmt_flags ) ;
            printf( "      dst_vtx_fmt_flags     = %i\n",sub_object->dst_vtx_fmt_flags ) ;
            printf( "      face_normals          = %i\n",sub_object->face_normals ) ;
            printf( "      tx_light_cmds         = %i\n",sub_object->tx_light_cmds ) ;
            printf( "      nodex_tri_list_idxs   = %i\n",sub_object->nodex_tri_list_idxs ) ;
            printf( "      tri_list_idxs         = %i\n",sub_object->tri_list_idxs ) ;
            printf( "      linelist_idxs         = %i\n",sub_object->linelist_idxs ) ;
            printf( "      nodex_tri_list_idxs2  = %i\n",sub_object->nodex_tri_list_idxs2 ) ;
            printf( "      tri_lists             = %i\n",sub_object->tri_lists ) ;
            printf( "      line_lists            = %i\n",sub_object->line_lists ) ;
            printf( "      pt_lists              = %i\n",sub_object->pt_lists ) ;
            printf( "      nodex_tri_lists       = %i\n",sub_object->nodex_tri_lists ) ;
            printf( "      n_geom_nodes          = %i\n",sub_object->n_geom_nodes ) ;
            printf( "      n_node_maps           = %i\n",sub_object->n_node_maps ) ;
            for(l=0;l<sub_object->n_node_maps;l++)
              printf( "        node_map       %i     = %i\n",l,sub_object->node_map[l] ) ;
            printf( "      n_shaders             = %i\n",sub_object->n_shaders ) ;
            for(l=0;l<sub_object->n_shaders;l++)
              printf( "        shaders        %i     = %i\n",l,sub_object->shader[l] ) ;

            printf( "      n_light_cfgs          = %i\n",sub_object->n_light_cfgs ) ;
            for(l=0;l<sub_object->n_light_cfgs;l++)
              printf( "        light_cfg      %i     = %i\n",l,sub_object->light_cfg[l] ) ;

            printf( "      n_vertices            = %i\n",sub_object->n_vertices ) ;

            ll = (300<sub_object->n_vertices ? 300 : sub_object->n_vertices);
#if 0
            for(l=0;l<ll;l++){
              printf( "          flags         %i     = %x\n",l,sub_object->vertex[l].flags ) ;
              printf( "          ipoint        %i     = %i\n",l,sub_object->vertex[l].ipoint ) ;
              printf( "          inormal       %i     = %i\n",l,sub_object->vertex[l].inormal ) ;
              printf( "          color1        %i     = %x\n",l,sub_object->vertex[l].color1 ) ;
              printf( "          color2        %i     = %x\n",l,sub_object->vertex[l].color2 ) ;
              printf( "          n_vertex_uvs  %i     = %i\n",l,sub_object->vertex[l].n_vertex_uvs ) ;
              for(m=0;m<sub_object->vertex[l].n_vertex_uvs;m++)
              printf( "            vertex_uv   %i     = %i\n",m,sub_object->vertex[l].vertex_uv[m] ) ;
            }
#else
            printf("     l    flags    ipoint    inormal     color1      color2   n_vertexuvs   [0]\n");
            for(l=0;l<ll;l++){
Vertex        *vertex = &(sub_object->vertex[l]) ;
              if(0 != vertex->n_vertex_uvs){
                printf("   { %3i, %4i, %4i, 0x%8x, 0x%8x, %2i, %3i},\n",
                          vertex->flags,vertex->ipoint,vertex->inormal,
                          vertex->color1,vertex->color2,
                          vertex->n_vertex_uvs, vertex->vertex_uv[0] );
              }else{
                printf("   %3i      %3i      %4i       %4i    %8x    %8x\n",
                         l, vertex->flags,vertex->ipoint,vertex->inormal,
                          vertex->color1,vertex->color2);
              }
            }
#endif


            printf( "      n_vertex_sets         = %i\n",sub_object->n_vertex_sets ) ;
            ll = (30<sub_object->n_vertex_sets ? 30 : sub_object->n_vertex_sets);
            for(l=0;l<ll;l++){
              printf( "\n        vtx_state_idx  %i     = %i\n",l,sub_object->vertex_set[l].vtx_state_idx ) ;
              printf( "        start_vtx_idx  %i     = %i\n",l,sub_object->vertex_set[l].start_vtx_idx ) ;
              printf( "        vtx_count      %i     = %i\n",l,sub_object->vertex_set[l].vtx_count     ) ;
            }

            printf("==============================================================================\n");
            printf("=====      Start Tri-Lists                ====================================\n");
            printf("==============================================================================\n");


            printf( "      Number of Tri-Lists     = %i\n",sub_object->n_tri_lists ) ;
            ll =  sub_object->n_tri_lists;
            for(l=0;l<ll;l++){
              printf("==============================================================================\n");
              printf("=====      Start New Tri-Lists            ====================================\n");
              printf("==============================================================================\n");
              printf( "\n        prim_state_idx     %i     = %i\n",l,sub_object->tri_list[l].prim_state_trilist ) ;
              printf(   "        n_vertex_idxs      %i     = %i\n",l,sub_object->tri_list[l].n_vertex_idxs ) ;
              if(ip){
                for(m=0;m<sub_object->tri_list[l].n_vertex_idxs;m++){
//                  printf(" %i (%i %i %i),",m, sub_object->tri_list[l].vertex_idx[m].a,sub_object->tri_list[l].vertex_idx[m].b,sub_object->tri_list[l].vertex_idx[m].c) ;
                  printf(" {%i, %i, %i},", sub_object->tri_list[l].vertex_idx[m].a,sub_object->tri_list[l].vertex_idx[m].b,sub_object->tri_list[l].vertex_idx[m].c) ;
                }
                printf("\n") ;
              }
              printf(   "        n_normal_idxs      %i     = %i\n",l,sub_object->tri_list[l].n_normal_idxs     ) ;
              if(ip){
                for(m=0;m<sub_object->tri_list[l].n_normal_idxs;m++){
//                  printf(" %i (%i),",m, sub_object->tri_list[l].normal_idx[m]) ;
                  printf(" %i,", sub_object->tri_list[l].normal_idx[m]) ;
                }
                printf("\n") ;
              }
              printf(   "        n_flags            %i     = %i\n",l,sub_object->tri_list[l].n_flags     ) ;
              if(ip){
                for(m=0;m<sub_object->tri_list[l].n_vertex_idxs;m++){
                  printf(" %i,",(int)sub_object->tri_list[l].flag[m]) ;
                }
                printf("\n") ;
              }
            }
          }
        }
      }

      printf("==============================================================================\n");
      printf("=====  Start Animations                   ====================================\n");
      printf("==============================================================================\n");

      printf( "\n  n_animations     = %i\n",snode->n_animations) ;
      for(i=0;i<snode->n_animations;i++){
Animation *animation ;
        animation = &(snode->animation[i]) ;
        printf("\n  animation : i = %i\n",i) ;
        printf("    frame_count  = %i\n",animation->frame_count)  ;
        printf("    frame_rate   = %i\n",animation->frame_rate)   ;
        printf("    n_anim_nodes = %i\n",animation->n_anim_nodes) ;

        for(j=0;j<animation->n_anim_nodes;j++){
AnimNode *anim_node ;
          anim_node = &(animation->anim_node[j]) ;
          printf("\n    anim_node : j = %i\n",j) ;
          printf("      n_controllers = %i\n",anim_node->n_controllers) ;

          for(k=0;k<anim_node->n_controllers;k++){
AnimController *controller ;
            controller = &(anim_node->controller[k]) ;
            printf("\n    controller : k = %i : Type = %i : %s\n",
                         k,controller->type,token_idc[controller->type]) ;
            printf("      n_anim_keys = %i\n",controller->n_anim_keys) ;
            if(controller->type == LINEAR_POS){
              printf("        anim_key  type  frame  X         Y         Z\n") ;
            }else if(controller->type == TCB_POS){
              printf("        anim_key  type  frame  X         Y         Z         W\n") ;
            }else{
              printf("        anim_key  type  frame  X         Y         Z         W        "
                     " tension   continuity bias     in        out\n") ;
            }

            for(l=0;l<controller->n_anim_keys;l++){
AnimKey *anim_key ;
              anim_key = &(controller->anim_key[l]) ;
#if 0
              printf("\n      anim_key : l = %i\n",l) ;
              printf("        type     = %i\n",anim_key->type)  ;
              printf("        frame    = %i\n",anim_key->frame) ;
              printf("        X        = %f\n",anim_key->X)     ;
              printf("        Y        = %f\n",anim_key->Y)     ;
              printf("        Z        = %f\n",anim_key->Z)     ;
              printf("        W        = %f\n",anim_key->W)     ;
              printf("        tension  = %f\n",anim_key->tension) ;
              printf("        continuity  = %f\n",anim_key->continuity) ;
              printf("        bias     = %f\n",anim_key->bias)  ;
              printf("        in       = %f\n",anim_key->in  )  ;
              printf("        out      = %f\n",anim_key->out )  ;
#else
              if(anim_key->type == SLERP_ROT){
                printf( "         %i,       %i,    %i,   %8.5f, %8.5f, %8.5f, %8.5f : %s\n",
                    l, anim_key->type, anim_key->frame, anim_key->X, anim_key->Y, anim_key->Z, anim_key->W,token_idc[anim_key->type]) ;
              }else if(anim_key->type == LINEAR_KEY){
                printf( "         %i,       %i,    %i,   %8.5f, %8.5f, %8.5f : %s\n",
                    l, anim_key->type, anim_key->frame, anim_key->X, anim_key->Y, anim_key->Z, token_idc[anim_key->type]) ;
              }else{
                printf( "         %i,       %i,    %i,   %8.5f, %8.5f, %8.5f, %8.5f, "
                      "%8.5f, %8.5f, %8.5f, %8.5f, %8.5f : %s\n",
                    l, anim_key->type, anim_key->frame, anim_key->X, anim_key->Y, anim_key->Z, anim_key->W, anim_key->tension, anim_key->continuity, anim_key->bias, anim_key->in, anim_key->out,token_idc[anim_key->type]) ;
              }
#endif
            } //  "End l AnimKey"
          }   //  "End k AnimController"
        }     //  "End j AnimNode "
      }       //  "End i Animation"


      printf("\n\n*****************************************************\n\n\n");
#endif
//      if(ip)printf(" Stopping after reading shape %s\n",snode->name);
//      if(ip)exit(1) ;

      return 0;
}



void error_s1(char *d) {

      printf("  Error in routine 'load_shape_filenames'.\n")   ;
      printf("  Directory not found.  \n  Directory = %s\n",d) ;
      printf("  Program stopping ... \n") ;
      exit(1) ;
}

void init_sub_object(SubObject *s){

      s->flags     = 0 ;
      s->sort_vector_idx     = 0 ;
      s->vol_idx     = 0 ;
      s->src_vtx_fmt_flags = 0 ;
      s->dst_vtx_fmt_flags = 0 ;
      s->face_normals      = 0 ;
      s->tx_light_cmds     = 0 ;
      s->nodex_tri_list_idxs = 0 ;
      s->tri_list_idxs     = 0 ;
      s->linelist_idxs     = 0 ;
      s->nodex_tri_list_idxs2 = 0 ;
      s->tri_lists         = 0 ;
      s->line_lists        = 0 ;
      s->pt_lists          = 0 ;
      s->nodex_tri_lists   = 0 ;
      s->n_geom_nodes      = 0 ;
      s->geom_node         = NULL ;
      s->n_node_maps       = 0 ;
      s->node_map          = NULL ;
      s->n_shaders         = 0 ;
      s->shader            = NULL ;
      s->n_light_cfgs      = 0 ;
      s->light_cfg         = NULL ;
      s->n_vertices        = 0 ;
      s->vertex            = NULL ;
      s->n_vertex_sets     = 0 ;
      s->vertex_set        = NULL ;
      s->n_tri_lists       = 0 ;
      s->tri_list          = NULL ;



      return ;
}


void init_tri_list(TriList *t){

      t->prim_state_trilist = 0 ;
      t->n_vertex_idxs      = 0 ;
      t->vertex_idx         = NULL ;
      t->n_normal_idxs      = 0 ;
      t->normal_idx         = NULL ;
      t->n_flags            = 0 ;
      t->flag               = NULL ;
  return ;
}

/*
 *  Either the following shape files contain errors or ZR is not displaying
 *  them correctly.
 *
 *  OESignal01 :  Double signal in EUROPE2 route
 *             :  Distance level 0 (< 50m) is missing the top signal arm
 *             :  Solution - copy the data from distance level 1.
 */

int correct_shape(ShapeNode *snode){

  int ip = 0 ;
  char *name1   = "OESignal01" ;
  char *my_name = "correct_shape" ;

      if(!strcmp_nqic(snode->name,name1)){
SubObject *so =  &(snode->lod_control[0].dist_level[0].sub_object[1]) ;
// First free malloc memory in sub-object
        if(so->geom_node) free(so->geom_node) ;
        if(so->node_map)  free(so->node_map) ;
        if(so->shader)    free(so->shader) ;
        if(so->light_cfg) free(so->light_cfg) ;
        if(so->vertex)    free(so->vertex) ;
        if(so->vertex_set)free(so->vertex_set) ;
        if(so->tri_list)  free(so->tri_list) ;
//  Copy across distance 1 data
        *so = snode->lod_control[0].dist_level[1].sub_object[1] ;

        if(ip)printf("  Routine %s.  Shape = %s\n",my_name,name1) ;
      }


      return 0 ;
}
