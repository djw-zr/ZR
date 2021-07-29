/*
 * *****************************************************************************
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File: load_wagons.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to handle the loading of data from the wagon and engine
 *   directories.  This includes loading the shape files
 *
 * *****************************************************************************
 */
#include <errno.h>
#include <sys/stat.h>
#include <ftw.h>

int init_raw_wagon_node(RawWagonNode *w) ;
int process_wagon_files(const char *fpath, const struct stat *sb,
                        int typeflag) ;

 /**
  *
  *   Routine to search for wagon files and generate a list of wagon nodes
  *   one for each wagon and engine found.
  */
int generate_wagon_list(){

  int    ip = 0          ;              // Debug printing
  int    l1, iret        ;
  char   *top_dir ;
  char  my_name[] = "generate_wagon_list" ;

      l1      = strlen(ORdir) + strlen("Trains/Trainset/") + 1 ;
      top_dir = (char *)malloc(l1*sizeof(char)) ;
      strcpy(top_dir,ORdir)                   ;
      strcat(top_dir,"Trains/Trainset/")           ;
      if(ip)printf(" Top trainset directory = %s\n",top_dir) ;

      iret = ftw(top_dir, process_wagon_files, 5) ;

      free(top_dir) ;
      if(ip)printf(" Return from routine %s\n",my_name) ;
      return iret ;
}



int process_wagon_files(const char *fpath, const struct stat *sb,
                        int typeflag){

  int    ip = 0          ;     // Debug printing
  int    n               ;
  FILE   *fp             ;
  char   *file_name,           //  Full filename
        *base_name,           //  Base name inc extension
        *core_name,           //  Base name without extension
        *extension,           //  Extension
        *string = NULL  ;     //  Used with malloc to construct filenames
                              //     ... freed when name not needed later
  char  my_name[] = "process_wagon_files" ;
  RawWagonNode *wagon_node   ;
  ShapeNode    *shape_node   ;
  TextureNode  *texture_node ;
/*
 *  Return if not a regular file
 */
      if(typeflag != FTW_F) return 0 ;
/*
 *  Use realpath to prevet any file name confusion
 *  Malloc variables need to be used permenantly or freed.
 */
      file_name = realpath(fpath, NULL)      ;  // malloc
      base_name = zr_basename(file_name)     ;  // malloc
      core_name = zr_corename(base_name)     ;  // malloc
      extension = zr_extension(base_name)    ;  // malloc
/*
 *  Skip lowest level directories "OpenRails".  These contain extra
 *  wag and eng data only supported by OpenRails
 */


      if(ip)printf("\n") ;
      if(ip)printf(" Program %s :: fpath = %s :: %s\n",my_name,
                                                       file_name, base_name) ;
/*
 * *****************************************************************************
 *    Process engine or wagon files plus any jpeg files
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
        if(wagonlist_beg == NULL){
          wagonlist_beg = wagon_node       ;
        }else{
          wagonlist_end->next = wagon_node ;
        }
        wagonlist_end     = wagon_node     ;
        wagon_node->next  = NULL           ;

        init_raw_wagon_node(wagon_node)                      ;
        wagon_node->file = strdup(file_name)             ;
        wagon_node->name = strdup(core_name)             ;
        wagon_node->is_engine = !strcmp(extension,"eng") ;
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

          shape_node->sfile =  string ;    // malloc kept
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
          shape_node->sdfile  = string ;  // malloc used
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

int init_raw_wagon_node(RawWagonNode *w){

      w->is_engine  = 0  ;
      w->name       = NULL ;
      w->file       = NULL ;
      w->s_file     = NULL ;
      w->sd_file    = NULL ;
      w->image      = NULL ;
      w->wagon      = NULL ;
//      w->shape      = NULL ;
      w->tender     = NULL ;
      return 0;
}

/*
 *  Routine to add Shapes and basic WagonNodes to Raw WagonNodes
 */
int init_raw_wagon_nodes(){

  RawWagonNode  *raw  ;
  WagonNode     *w    ;
  ShapeNode     *s    ;

        for(raw = wagonlist_beg; raw != NULL; raw = raw->next){
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
  int  i, l    ;
  int  ip = 0  ;   // 0 = no printing, -1 = error printing only
  TextureNode  *tnode        ;
  char         *name  = NULL ;
//  char         *tname = NULL ;
//  char    *file_name  = NULL ,            //  Full filename
//          *base_name  = NULL ,            //  Base name inc extension
//          *core_name  = NULL ,            //  Base name without extension
//          *extension  = NULL ,            //  Extension
//          *string = NULL          ;       //  Temporary string
  char         my_name[] = "add_texture_pointers_to_shape_items" ;

      if(ip>1){
        printf(" Enter routine %s\n",my_name) ;
        printf("   Finding textures for shape %s\n",snode->name) ;
      }
/*
 *  Loop through required textures
 */
      for(i=0;i<snode->n_textures;i++){
        snode->texture[i] = NULL ;           //  Initialise link to NULL
        if(snode->texture_name[i] == NULL)continue ;  //  Skip if name is mising
        free(name) ;
        name = zr_basename(snode->texture_name[i]) ;
        l = strlen(name) ;
        if(strcmp(&name[l-4],".ace")){
          printf("  Image filename does not end with '.ace'\n");
          printf("  Filename = %s\n",name) ;
          printf(" Program stopping ... \n") ;
          exit(1) ;
        }

        name[l-4] = '\0' ;
        if(ip>1)printf("  Texture index = %i,  Texture required = %s\n",i,name) ;
/*
 *  Loop through list of textures
 */
        for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
          if(strcmp_ic(name,tnode->name))continue ;
          snode->texture[i] = tnode     ;          // save link
          tnode->needed       = 1         ;
          if(ip>1)printf("  Texture found    = %s\n",tnode->name) ;
          break ;
        }
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
      }
      free(name) ;
      return 0;
}

int check_wagon_textures(int k){

  int         i      ;
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
