/*
 * *****************************************************************************
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File: load_wagon_files.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to:
 *   1. Search for and save the loactions of files describing wagons and
 *      engines in the wagon and engine directories.
 *   2. This involves calling the routine load_wagon_file() in
 *      "load_wagon_file.c" which reads each main *.wag  or *.eng files
 *      and saves the data in a wagon structure.
 *
 * *****************************************************************************
 */
#include <errno.h>
#include <sys/stat.h>
#include <ftw.h>

int init_raw_wagon_node(RawWagonNode *w) ;
int read_raw_wagon_file(RawWagonNode *w) ;
int process_wagon_file(const char *fpath, const struct stat *sb,
                        int typeflag) ;
void create_new_texture(char *fname) ;
int create_new_wagon_node(char *fname) ;

 /**
  *
  *   Routine to search for wagon files and generate a list of wagon nodes
  *   one for each wagon and engine found.
  */
int scan_for_wagon_files(){

  int    ip = 0    ;       // Debug printing
  int    l1, l2, l3, l4, iret  ;
  char          *top_dir1,
                *top_dir2 = NULL,
                *fname    = NULL ;
  DIR           *wdir1,
                *wdir2 ;
  struct dirent *f_entry1,
                *f_entry2 ;
  struct stat   *stbuf   ;
  char  *trainset       ="/Trains/Trainset/" ;
  char  *trainset_inuse ="/Trains/Trainset_In_Use/" ;
  char  my_name[] = "scan_for_wagon_files" ;

      l1      = strlen(ORdir) + strlen(trainset_inuse) + 1 ;
      top_dir1 = (char *)malloc(l1*sizeof(char)) ;
      strcpy(top_dir1,ORdir)           ;
      strcat(top_dir1,trainset_inuse)  ;
      iret = zr_find_msfile2(top_dir1) ;
      if(iret){
        printf("  Routine %s.  Unable to find directory %s\n",my_name,top_dir1);
        free(top_dir1) ;
        l1      = strlen(ORdir) + strlen(trainset) + 1 ;
        top_dir1 = (char *)malloc(l1*sizeof(char)) ;
        strcpy(top_dir1,ORdir)                   ;
        strcat(top_dir1,trainset)           ;
        iret = zr_find_msfile2(top_dir1);
        if(iret){
          printf("  Routine %s.  Unable to find directory %s\n",my_name,top_dir1);
        }
      }
      if(ip)printf(" Top trainset directory = %s\n",top_dir1) ;

#if 1
/*
 *  Open top directory and loop through
 */
      wdir1 = opendir(top_dir1) ;
      while ((f_entry1 = readdir(wdir1)) != NULL) {
        if(f_entry1->d_type != DT_DIR ) continue ;
        if((f_entry1->d_name)[0] == '.') continue ;
        if(ip)printf("  Found directory = %s\n", f_entry1->d_name);
//#ifdef ROUTE_NEW_FOREST
/*
 *  Ignore problem engine included in New Forest Route
 */
        if(!strcmp(f_entry1->d_name,"CCW_George_Jennings") ||
           !strcmp(f_entry1->d_name,"GeorgeJennings")         )continue ;
//#endif
/*
 *  Generate full directory name
 */
        l2 = l1 + 1 + strlen(f_entry1->d_name) ;
        if(top_dir2) free(top_dir2) ;
        top_dir2 = (char *)malloc(l2*sizeof(char)) ;
        strcpy(top_dir2,top_dir1) ;
        strcat(top_dir2,f_entry1->d_name)                   ;
/*
 *  Open next directory and loop through
 */
        wdir2 = opendir(top_dir2) ;
        while ((f_entry2 = readdir(wdir2)) != NULL) {
          l3 = strlen(f_entry2->d_name) ;
          if(strcmp_ic(".wag",&(f_entry2->d_name[l3-4])) != 0 &&
             strcmp_ic(".eng",&(f_entry2->d_name[l3-4])) != 0 &&
             strcmp_ic(".ace",&(f_entry2->d_name[l3-4])) != 0) continue ;
/*
 *  Generate full file name
 */
          l4 = l2 + l3 + 2 ;
          if(fname) free(fname) ;
          fname = (char *)malloc(l4*sizeof(char)) ;
          strcpy(fname,top_dir2) ;
          strcat(fname,"/") ;
          strcat(fname,f_entry2->d_name) ;
          if(ip)printf("    File full name = %s\n", fname);

          if(strcmp_ic(".ace",&(f_entry2->d_name[l3-4])) == 0){
            create_new_texture(fname);
          }else{
            create_new_wagon_node(fname) ;
          }
        }
      }
#else
//  Ust ftw to walk directory

      if(!iret){
        iret = nftw(top_dir, process_wagon_file, 5) ;
      }
#endif
      if(top_dir1)free(top_dir1) ;
      if(top_dir2)free(top_dir2) ;
      if(fname)   free(fname) ;

      if(ip)printf(" Return from routine %s\n",my_name) ;
      return iret ;
}


void create_new_texture(char *fname){

int  ip = 0 ;
char *file_name,
     *base_name,
     *core_name ;
char *my_name = "create_new_texture" ;
TextureNode  *texture_node ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
*  Create new texture structure
*/
      texture_node = (TextureNode *)malloc(sizeof(TextureNode)) ;
      if(wtexturelist_beg == NULL){
        wtexturelist_beg = texture_node  ;
      }else{
        wtexturelist_end->next = texture_node ;
      }
      wtexturelist_end    = texture_node ;
      texture_node->next  = NULL         ;
      init_texture_node(texture_node)    ;
/*
 *  Save names
 */
#ifdef MinGW
      file_name = realpath1(fname, NULL)     ;  // malloc
#else
      file_name = realpath(fname, NULL)     ;  // malloc
#endif
      base_name = zr_basename(file_name)     ;  // malloc
      core_name = zr_corename(base_name)     ;  // malloc
      if(ip)printf("    Found *.ace file : %s\n",file_name) ;
      if(ip)printf("    Name of texture  : %s\n",core_name) ;

      texture_node->filename = strdup(file_name) ;
      texture_node->name     = strdup(core_name) ;

      free(core_name) ;
      free(base_name) ;
      free(file_name) ;
      if(ip)printf("  Return from routine %s\n",my_name) ;
      return ;
}

int create_new_wagon_node(char *fname){
int  ip = 0 ;
int   n               ;
FILE  *fp             ;
char  *file_name,           //  Full filename
        *base_name,           //  Base name inc extension
        *core_name,           //  Base name without extension
        *extension,           //  Extension
        *s_file,              //  File name of shape
        *string = NULL  ;     //  Used with malloc to construct filenames
                              //     ... freed when name not needed later
char *my_name = "create_new_wagon_node" ;
RawWagonNode *wagon_node, *rw ;
ShapeNode    *shape_node   ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
 *  Use realpath to prevet any file name confusion
 *  Malloc variables need to be used permenantly or freed.
 */
#ifdef MinGW
      file_name = realpath1(fname, NULL)     ;  // malloc
#else
      file_name = realpath(fname, NULL)     ;  // malloc
#endif
      base_name = zr_basename(file_name)     ;  // malloc
      core_name = zr_corename(base_name)     ;  // malloc
      extension = zr_extension(base_name)    ;  // malloc
      if(ip)printf("  Name of engine or wagon : %s\n",core_name) ;
      if(ip)printf("  Found wagon file        : %s\n",file_name) ;
/*
 *  Ignore files in directory non_BIN_eng
 */
      if(strstr(base_name,"non_BIN_eng")!= NULL) return 0 ;
/*
 *  Skip lowest level directories "OpenRails".  These contain extra
 *  wag and eng data only supported by OpenRails
 */
      if(ip)printf("\n") ;
      if(ip)printf(" Program %s :: fpath = %s :: %s\n",my_name,
                                                       file_name, base_name) ;
/*
 *  Check this is not below directory named OpenRails (used for extra OR data)
 */
      string = zr_parentdir(file_name) ;       // malloc
      if(strcmp(string,"OpenRails")==0){
        if(ip)printf("  Exit because of parent\n") ;
        free(file_name) ;
        free(string)    ;
        free(base_name) ;
        free(core_name) ;
        free(extension) ;
        return 0;
      }
      free(string) ;                           // parent no longer needed
/*
 *  Create new engine/wagon structure
 */
      wagon_node = (RawWagonNode *)malloc(sizeof(RawWagonNode)) ;
      if(rawwagonlist_beg == NULL){
        rawwagonlist_beg = wagon_node       ;
      }else{
        rawwagonlist_end->next = wagon_node ;
      }
      rawwagonlist_end  = wagon_node     ;
      wagon_node->next  = NULL           ;

      init_raw_wagon_node(wagon_node)                  ;
      wagon_node->file = strdup(file_name)             ;
      wagon_node->name = strdup(core_name)             ;
      wagon_node->is_engine = !strcmp(extension,"eng") ;

      if(ip)printf(" Call read_raw_wagon for %s\n",wagon_node->name) ;
/*
 *  Read wagon/engine (*.wag or *.eng) file
 *                          and store data in wagon_node
 */
      read_raw_wagon_file(wagon_node) ;
/*
 *    List wagons
 */
#if 0
      for(rw = rawwagonlist_beg; rw != NULL; rw = rw->next){
        printf("  Wagon name = %s\n",rw->name) ;
      }
#endif

/*
 *  Check for shape
 */
      s_file = wagon_node->s_file ;
      if(s_file == NULL) return 0 ;  //  Probably default.wag with extra parameters only

      if(ip)printf("    s_file = %s\n",s_file) ;
      if(ip)printf("    wshapelist_beg = %p\n",(void *)wshapelist_beg) ;
      wagon_node->shape = NULL ;
      for(shape_node = wshapelist_beg; shape_node != NULL;
                                      shape_node = shape_node->next){
        if(ip)printf("    shape_node->s_file = %s\n",shape_node->s_file) ;
        if(!strcmp(s_file,shape_node->s_file)){
          wagon_node->shape = shape_node ;
          break ;
        }
      }
      if(wagon_node->shape == NULL){
        if(ip)printf("  Shape not loaded\n") ;
      }else{
        if(ip)printf("  Shape already loaded\n") ;
      }
/*
*  Create new shape node
*/
      if(wagon_node->shape == NULL){
/*
*  Create new shape structure
*/
        shape_node = (ShapeNode *)malloc(sizeof(ShapeNode)) ;
        init_shape_node(shape_node)            ;
        if(wshapelist_beg == NULL){
          wshapelist_beg = shape_node          ;
        }else{
          wshapelist_end->next = shape_node    ;
        }
        wshapelist_end      = shape_node ;
        shape_node->next    = NULL       ;
        shape_node->s_file  =  s_file    ;
        wagon_node->shape   = shape_node ;
        shape_node->name    = zr_basename2(s_file) ;
        wagon_node->f_shape = NULL       ;
//          printf("  zr_basename(s_file) = %s\n",zr_basename(s_file)) ;
//          printf("  zr_corename(s_file) = %s\n",zr_corename(s_file)) ;
//          printf("  shape_node->name    = %s\n",shape_node->name) ;
/*
*  Search for sd file corresponding to shape node
*  For the wagons directories, the *.sd files have the same core
*  names as the shape *.s files, not the *.wag or *.eng files.
*/
        n = strlen(s_file) ;
        string = (char *)malloc(sizeof(char)*(n+2)) ;
        strcpy(string,s_file) ;
        strcat(string,"d")    ;

        if((fp = gopen(string,"r"))!= NULL){
          if(ip)printf("  Found *.sd file         : %s\n",string) ;
          shape_node->sd_file = string ;  // malloc used
          wagon_node->sd_file = string ;  // Which node type needs the sdfile
          gclose(fp) ;
        }else{
          if(ip)printf("  Not found *.sd file     : %s\n",string) ;
          free(string)               ;     // not needed
        }
      }else{
        wagon_node->sd_file = wagon_node->shape->sd_file ;
      }
/*
*  Check for freight shape
*/
      s_file = wagon_node->fs_file ;
      if(ip)printf("   Freight shape file = %s\n",s_file) ;
      if(s_file != NULL){
        if(ip)printf("    wshapelist_beg = %p\n",(void *)wshapelist_beg) ;
        wagon_node->f_shape = NULL ;
        for(shape_node = wshapelist_beg; shape_node != NULL;
                                        shape_node = shape_node->next){
          if(ip)printf("    shape_node->s_file = %s\n",shape_node->s_file) ;
          if(!strcmp(s_file,shape_node->s_file)){
            wagon_node->f_shape = shape_node ;
            break ;
          }
        }
        if(wagon_node->f_shape == NULL){
          if(ip)printf("  Freight shape not loaded\n") ;
        }else{
          if(ip)printf("  Freight shape already loaded\n") ;
        }
      }


      return 0 ;
}

/*
 * System routine ftw requires a routine parameter for which the
 * calling sequence is as follows.  However for processing here
 * the  second parameter is not required.
 *
 *   string.h
 *   strstr(a, b)  look for b in a   strcasestr - without case
 *   see 'man string'
 */

int process_wagon_file(const char *fpath,
                  __attribute__((unused))const struct stat *sb,
                  int typeflag){

  int   ip = 0          ;     // Debug printing
  int   n               ;
  FILE  *fp             ;
  char  *file_name,           //  Full filename
        *base_name,           //  Base name inc extension
        *core_name,           //  Base name without extension
        *extension,           //  Extension
        *s_file,              //  File name of shape
        *string = NULL  ;     //  Used with malloc to construct filenames
                              //     ... freed when name not needed later
  char  my_name[] = "process_wagon_files" ;
  RawWagonNode *wagon_node   ;
  ShapeNode    *shape_node   ;
  TextureNode  *texture_node ;
/*
 *  Return if not a regular file
 */
      if(typeflag != FTW_F) {
        return 0 ;
      }
/*
 *  Use realpath to prevet any file name confusion
 *  Malloc variables need to be used permenantly or freed.
 */
#ifdef MinGW
      file_name = realpath1(fpath, NULL)     ;  // malloc
#else
      file_name = realpath(fpath, NULL)     ;  // malloc
#endif
      base_name = zr_basename(file_name)     ;  // malloc
      core_name = zr_corename(base_name)     ;  // malloc
      extension = zr_extension(base_name)    ;  // malloc
/*
 *  Ignore files in directory non_BIN_eng
 */
      if(strstr(base_name,"non_BIN_eng")!= NULL) return 0 ;
/*
 *  Skip lowest level directories "OpenRails".  These contain extra
 *  wag and eng data only supported by OpenRails
 */
      if(ip)printf("\n") ;
      if(ip)printf(" Program %s :: fpath = %s :: %s\n",my_name,
                                                       file_name, base_name) ;
/*
 * *****************************************************************************
 *    Process engine or wagon files only
 * *****************************************************************************
 */
      if(!strcmp(extension,"eng")|| !strcmp(extension,"wag") ){
        if(ip)printf("  Name of engine or wagon : %s\n",core_name) ;
        if(ip)printf("  Found wagon file        : %s\n",file_name) ;
/*
 *  Check this is not below directory named OpenRails (used for extra OR data)
 */
        string = zr_parentdir(file_name) ;       // malloc
        if(strcmp(string,"OpenRails")==0){
          if(ip)printf("  Exit because of parent\n") ;
          free(file_name) ;
          free(string)    ;
          free(base_name) ;
          free(core_name) ;
          free(extension) ;
          return 0;
        }
        free(string) ;                           // parent no longer needed
/*
 *  Create new engine/wagon structure
 */
        wagon_node = (RawWagonNode *)malloc(sizeof(RawWagonNode)) ;
        if(rawwagonlist_beg == NULL){
          rawwagonlist_beg = wagon_node       ;
        }else{
          rawwagonlist_end->next = wagon_node ;
        }
        rawwagonlist_end  = wagon_node     ;
        wagon_node->next  = NULL           ;

        init_raw_wagon_node(wagon_node)                  ;
        wagon_node->file = strdup(file_name)             ;
        wagon_node->name = strdup(core_name)             ;
        wagon_node->is_engine = !strcmp(extension,"eng") ;

        if(ip)printf(" Call read_raw_wagon for %s\n",wagon_node->name) ;
/*
 *  Read wagon/engine (*.wag or *.eng) file
 *                          and store data in wagon_node
 */
        read_raw_wagon_file(wagon_node) ;
/*
 *  Check for shape
 */
        s_file = wagon_node->s_file ;
        if(s_file == NULL) return 0 ;  //  Probably default.wag with extra parameters only

        if(ip)printf("    s_file = %s\n",s_file) ;
        if(ip)printf("    wshapelist_beg = %p\n",(void *)wshapelist_beg) ;
        wagon_node->shape = NULL ;
        for(shape_node = wshapelist_beg; shape_node != NULL;
                                        shape_node = shape_node->next){
          if(ip)printf("    shape_node->s_file = %s\n",shape_node->s_file) ;
          if(!strcmp(s_file,shape_node->s_file)){
            wagon_node->shape = shape_node ;
            break ;
          }
        }
        if(wagon_node->shape == NULL){
          if(ip)printf("  Shape not loaded\n") ;
        }else{
          if(ip)printf("  Shape already loaded\n") ;
        }
/*
 *  Create new shape node
 */
        if(wagon_node->shape == NULL){
/*
 *  Create new shape structure
 */
          shape_node = (ShapeNode *)malloc(sizeof(ShapeNode)) ;
          init_shape_node(shape_node)            ;
          if(wshapelist_beg == NULL){
            wshapelist_beg = shape_node          ;
          }else{
            wshapelist_end->next = shape_node    ;
          }
          wshapelist_end      = shape_node ;
          shape_node->next    = NULL       ;
          shape_node->s_file  =  s_file    ;
          wagon_node->shape   = shape_node ;
          shape_node->name    = zr_basename2(s_file) ;
          wagon_node->f_shape = NULL       ;
//          printf("  zr_basename(s_file) = %s\n",zr_basename(s_file)) ;
//          printf("  zr_corename(s_file) = %s\n",zr_corename(s_file)) ;
//          printf("  shape_node->name    = %s\n",shape_node->name) ;
/*
 *  Search for sd file corresponding to shape node
 *  For the wagons directories, the *.sd files have the same core
 *  names as the shape *.s files, not the *.wag or *.eng files.
 */
          n = strlen(s_file) ;
          string = (char *)malloc(sizeof(char)*(n+2)) ;
          strcpy(string,s_file) ;
          strcat(string,"d")    ;

          if((fp = gopen(string,"r"))!= NULL){
            if(ip)printf("  Found *.sd file         : %s\n",string) ;
            shape_node->sd_file = string ;  // malloc used
            wagon_node->sd_file = string ;  // Which node type needs the sdfile
            gclose(fp) ;
          }else{
            if(ip)printf("  Not found *.sd file     : %s\n",string) ;
            free(string)               ;     // not needed
          }
        }else{
          wagon_node->sd_file = wagon_node->shape->sd_file ;
        }
/*
 *  Check for freight shape
 */
        s_file = wagon_node->fs_file ;
        if(ip)printf("   Freight shape file = %s\n",s_file) ;
        if(s_file != NULL){
          if(ip)printf("    wshapelist_beg = %p\n",(void *)wshapelist_beg) ;
          wagon_node->f_shape = NULL ;
          for(shape_node = wshapelist_beg; shape_node != NULL;
                                          shape_node = shape_node->next){
            if(ip)printf("    shape_node->s_file = %s\n",shape_node->s_file) ;
            if(!strcmp(s_file,shape_node->s_file)){
              wagon_node->f_shape = shape_node ;
              break ;
            }
          }
          if(wagon_node->f_shape == NULL){
            if(ip)printf("  Freight shape not loaded\n") ;
          }else{
            if(ip)printf("  Freight shape already loaded\n") ;
          }
/*
 *  Create new shape node
 */
          if(wagon_node->f_shape == NULL){
/*
 *  Create new shape structure
 */
            shape_node = (ShapeNode *)malloc(sizeof(ShapeNode)) ;
            init_shape_node(shape_node)            ;
            if(wshapelist_beg == NULL){
              wshapelist_beg = shape_node          ;
            }else{
              wshapelist_end->next = shape_node    ;
            }
            wshapelist_end      = shape_node ;
            shape_node->next    = NULL       ;
            shape_node->s_file  =  s_file    ;
            wagon_node->f_shape = shape_node ;
            shape_node->name    = zr_basename2(s_file) ;
          }
        }
#if 0
/*
 *  Search for corresponding *.jpg file
 */
        string = strdup(file_name)   ;
        n = strlen(string)           ;
        string[n-4] = '\0'           ;
        strcat(string,".jpg")        ;
        if((fp = gopen(string,"r"))!= NULL){
          wagon_node->image = string ;
          gclose(fp)                 ;
          if(ip)printf("  Found     *.jpg file    : %s\n",string) ;
        }else{
          if(ip)printf("  Not found *.jpg file    : %s\n",string) ;
          free(string)               ;
        }
/*
 *  Search for corresponding *.s file
 *
 *  THis should be obtained from the *.wag file !!!!
 *  Some trucks share shape files - this needs to be allowed for ...
 */
        string = strdup(file_name)   ;     // malloc but may be kept
        n = strlen(string)           ;
        string[n-4] = '\0'           ;
        strcat(string,".s")          ;
        if((fp = gopen(string,"r"))== NULL){
          if(ip)printf("  Not found *.s file      : %s\n",string) ;
          free(string)               ;     // not needed
        }else{
          if(ip)printf("  Found *.s file          : %s\n",string) ;
/*
 *  Create new shape structure
 */
          shape_node = (ShapeNode *)malloc(sizeof(ShapeNode)) ;
          init_shape_node(shape_node)            ;
          if(wshapelist_beg == NULL){
            wshapelist_beg = shape_node          ;
          }else{
            wshapelist_end->next = shape_node    ;
          }
          wshapelist_end    = shape_node ;
          shape_node->next  = NULL       ;

          shape_node->s_file = string ;    // malloc kept
          wagon_node->s_file = string ;
          wagon_node->shape = shape_node         ;
          shape_node->name  = strdup(core_name)  ;
        }
/*
 *  Sarch for corresponding *.sd file
 */
        string = strdup(file_name)   ;
        n = strlen(string)           ;
        string[n-4] = '\0'           ;
        strcat(string,".sd")         ;
        if((fp = gopen(string,"r"))!= NULL){
          if(ip)printf("  Found *.sd file         : %s\n",string) ;
          shape_node->sd_file = string ;  // malloc used
          wagon_node->sd_file = string ;
          gclose(fp) ;
        }else{
          if(ip)printf("  Not found *.sd file     : %s\n",string) ;
          free(string)               ;     // not needed
        }
/*
* *****************************************************************************
*    Texture files in this directory
* *****************************************************************************
*/
      }else if(!strcmp(extension,"ace")){
        if(ip)printf("  Found *.ace file        : %s\n",file_name) ;
//        if(ip)printf("  base_name               : %s\n",base_name) ;
//        if(ip)printf("  extension         : %s\n",extension) ;
        if(ip)printf("  Name of texture         : %s\n",core_name) ;
/*
*  Create new texture structure
*/
        texture_node = (TextureNode *)malloc(sizeof(TextureNode)) ;
        if(wtexturelist_beg == NULL){
          wtexturelist_beg = texture_node  ;
        }else{
          wtexturelist_end->next = texture_node ;
        }
        wtexturelist_end    = texture_node ;
        texture_node->next  = NULL         ;
        init_texture_node(texture_node)    ;

        texture_node->filename = strdup(file_name) ;
        texture_node->name     = strdup(core_name) ;
#endif
/*
* *****************************************************************************
*    Texture files in this directory
* *****************************************************************************
*/
      }else if(!strcmp(extension,"ace")){
        if(ip)printf("  Found *.ace file        : %s\n",file_name) ;
//        if(ip)printf("  base_name               : %s\n",base_name) ;
//        if(ip)printf("  extension         : %s\n",extension) ;
        if(ip)printf("  Name of texture         : %s\n",core_name) ;
/*
*  Create new texture structure
*/
        texture_node = (TextureNode *)malloc(sizeof(TextureNode)) ;
        if(wtexturelist_beg == NULL){
          wtexturelist_beg = texture_node  ;
        }else{
          wtexturelist_end->next = texture_node ;
        }
        wtexturelist_end    = texture_node ;
        texture_node->next  = NULL         ;
        init_texture_node(texture_node)    ;

        texture_node->filename = strdup(file_name) ;
        texture_node->name     = strdup(core_name) ;
      }
      free(extension) ;
      free(core_name) ;
      free(base_name) ;
      free(file_name) ;
      if(ip)printf(" Return from routine %s\n",my_name) ;
      return 0 ;
}



#if 0
          n = strlen(f_entry2->d_name) ;
          free(name) ;
          name = strdup(f_entry2->d_name) ;
//          if(ip)printf(" File = %s :: %s\n",name,&name[n-4]) ;
        }
        closedir(wdir2)  ;
        free(wdir2_name) ;
      }
      closedir(wdir1)  ;
      free(wdir1_name) ;
      free(name)       ;
      free(file_name)  ;
      return 0;
 }
#endif

/*
 *  Routine to initialise raw wagon structure to NULLs etc
 */

int init_raw_wagon_node(RawWagonNode *rw){

      rw->is_engine  = 0  ;
      rw->name       = NULL ;
      rw->file       = NULL ;
      rw->s_file     = NULL ;
      rw->sd_file    = NULL ;
      rw->image      = NULL ;
      rw->tender     = NULL ;
      rw->has_wipers      = 0 ;
      rw->has_mirrors     = 0 ;
      rw->has_pantographs = 0 ;
      return 0;
}

/*
 *  Routine to add Shapes and basic WagonNodes to Raw WagonNodes
 */
int init_raw_wagon_nodes(){

  RawWagonNode  *raw  ;
  WagonNode     *w    ;
  ShapeNode     *s    ;

        for(raw = rawwagonlist_beg; raw != NULL; raw = raw->next){
            for(s = wshapelist_beg; s != NULL; s = s->next){


            }


        }


      return 0 ;
}

/*
 *  On the basis of the OZZ datasets:
 *  1. Wagon items use textures from the same directory or from a nearby
 *     directory - in which case the local filename is replaced by
 *     the full relative path (i.e. ../somedir/somefile.ace)
 *  2. The same names (i.e. blank.ace) may occur in each of the local
 *     directories.
 *  3. It is possible some wagons will use testures from MSTS directories
 *
 *  In the following (1) and (2) are handled by checking that the fully
 *  expanded filenames match, (3) is not attempted until more is known.
 *
 */

int  add_texture_pointers_to_wagon_shapes(ShapeNode *snode){
//  int  i, j, k, l ;
  uint  i, l    ;
  uint  ip = 0  ;   // 0 = no printing, -1 = error printing only
  uint  ace_error = 0 ;
  TextureNode  *tnode        ;
  char         *name  = NULL ;
//  char         *tname = NULL ;
//  char    *file_name  = NULL ,            //  Full filename
//          *base_name  = NULL ,            //  Base name inc extension
//          *core_name  = NULL ,            //  Base name without extension
//          *extension  = NULL ,            //  Extension
//          *string = NULL          ;       //  Temporary string
  char         my_name[] = "add_texture_pointers_to_shape_items" ;

//      printf("  Routine %s.  Shape = ::%s::\n",my_name,snode->name) ;
//      ip = strcmp(snode->name,"33020") ? 0 : 2 ;
      if(ip>0) ip++ ;

      if(ip>1){
        printf(" Enter routine %s\n",my_name) ;
        printf("   Finding textures for shape %s\n",snode->name) ;
        printf("    Shape file             =  %s\n",snode->s_file) ;
        printf("    Shape discription file =  %s\n",snode->sd_file) ;
        printf("    n_textures =  %i\n",snode->n_textures) ;
      }
/*
 *  Loop through required textures
 */
      for(i=0;i<snode->n_textures;i++){
        if(ip)printf("  AA i = %i :: %i \n",i,snode->n_textures) ;
        snode->texture[i] = NULL ;           //  Initialise link to NULL
        if(ip)printf("  BB i = %i :: %i \n",i,snode->n_textures) ;
        if(snode->texture_name[i] == NULL)continue ;  //  Skip if name is missing
        if(name != NULL)free(name) ;
        if(ip)printf("  CC i = %i :: %i \n",i,snode->n_textures) ;
        name = zr_basename(snode->texture_name[i]) ;
        l = strlen(name) ;
        if(!strcmp_ic(&name[l-4],".ace")){
          name[l-4] = '\0' ;
        }else if(!strcmp_ic(&name[l-2],".a")){
          name[l-2] = '\0' ;
          if(ace_error == 0){
            printf("  Routine %s ERROR\n",my_name) ;
            printf("  Shape %s refers to texture file names ending in '.a'\n",
                    snode->name) ;
            ace_error = 1 ;
          }
        }else{
            printf("  Image filename does not end with '.ace' (or '.a')\n");
            printf("  i        = %i :: %i\n",i,snode->n_textures) ;
            printf("  Filename = %s\n",name) ;
            printf(" Program stopping ... \n") ;
            exit(1) ;
        }

        if(ip>1)printf("  Texture index = %i,  Texture required = %s\n",i,name) ;
/*
 *  Loop through list of textures
 */
        for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
          if(strcmp_ic(name,tnode->name))continue ;
          if(ip)printf("  DD i = %i :: %i \n",i,snode->n_textures) ;
          snode->texture[i] = tnode     ;          // save link
          tnode->needed     = 1         ;
          if(ip>1)printf("  Texture found    = %s\n",tnode->name) ;
          break ;
        }
        if(ip)printf("  EE i = %i :: %i \n",i,snode->n_textures) ;
        if(ip && snode->texture[i] == NULL){
          printf("  Shape %s.   Texture %s not found\n",snode->name,name) ;
#if 0
          for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
            printf(" Texture required = %s,  Present = %s\n",name,tnode->name) ;
          }
          printf(" Stopping ... \n") ;
          exit(0) ;
#endif
/*
 *  If texture is not found substitute 'blank'
 */
          for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
            if(strcmp_ic("blank",tnode->name))continue ;
            snode->texture[i] = tnode     ;          // save link
            tnode->needed       = 1         ;
            printf("            Substitute used = %s\n",tnode->name) ;
            break ;
          }
          if(ip && snode->texture[i] == NULL){
            printf("       Substitute 'BLANK not found \n") ;
          }
        }
        if(ip)printf("  ZZ i = %i :: %i \n",i,snode->n_textures) ;
      }
      if(ip)printf("  ZZ ZZ i = %i :: %i \n",i,snode->n_textures) ;
      if(name != NULL)free(name) ;
      return 0;
}

int check_wagon_textures(int k){

  uint      i      ;
  ShapeNode *snode ;

        for(snode=wshapelist_beg; snode!=NULL; snode=snode->next){
          if(strcmp(snode->name,"1905-KL")) continue ;
          printf(" ==============================================================\n");
          for(i=0;i<snode->n_textures;i++){
            printf("  Check_wagon_textures %i.  wagon = %s,  i = %i, texture = %p\n",
                           k,snode->name,i,(void *)snode->texture[i]);
          }
          printf(" ==============================================================\n");
        }
      return 0 ;
}

ShapeNode *find_wagon(char *name){

char       my_name[] = "find_wagon" ;
ShapeNode  *shape ;

      for(shape = wshapelist_beg; shape != NULL; shape = shape->next){
        if(strcmp(name,shape->name)==0)break ;
      }
      if(shape == NULL){
        printf("  Routine %s unable to find wagon %s\n",my_name,name) ;
        exit(0) ;
      }
      return shape ;
}

