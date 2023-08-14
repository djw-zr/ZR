/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  load_consist.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for reading the consist files
 *
 *==============================================================================
 */

 /*
  *  Consists files are stored in the top level directory 'Trainset/Consists'
  *  The routine 'load_consists', finds and opens the directory and stores the
  *  data in a linked list of 'ConsistNode' data structures (see train.h).
  *  The first and last nodes in this list are pointed to by 'consistlist_beg'
  *  and 'consistlist_end' (see zr.h).
  */

int         read_consist_file(char *file_name) ;
ConsistNode *new_consist(void) ;
int add_wagon_to_consist(ConsistNode *consist, char *filename, char *directory) ;

/*
 *  Routine to sort the consists
 */

static int  compare_consist_names(const void *p1, const void *p2){

ConsistNode **s1, **s2, *t1, *t2 ;

      s1 = (ConsistNode **)p1 ;
      s2 = (ConsistNode **)p2 ;
      t1 = (ConsistNode *)*s1 ;
      t2 = (ConsistNode *)*s2 ;

//      printf("  compare  %p %p :: %p %p:: %p %p :: %s %s\n",p1,p2,(void *)s1,(void *)s2,
//             (void *)t1, (void *)t2,t1->name,t2->name) ;

      return strcmp(t1->name, t2->name) ;
}


int load_consist_files(void){

  int    ip = 0 ;
  int    iret   ;
  int    len, i, n ;
  char   *dir_name ;
  char   *cpy_name ;
  char   *file_name ;
  char   *root_name ;
  DIR    *cdir      ;
  struct dirent *f_entry ;
  ConsistNode *consist,
              **consist_a ;

  char   *con_dir  = "Trains/Consists" ;
  char   *con_dir1 = "Trains/Consists_In_Use" ;
  char   *my_name = "load_consists"    ;


      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
 *  Find consists directory allowing for possible MS style filenames
 *  In malloc call, allow space for closing '\0'.
 */
      dir_name = (char *)malloc(strlen(ORdir)+strlen(con_dir1) + 1 ) ;
      strcpy(dir_name,ORdir) ;
      strcat(dir_name,con_dir1) ;
      cpy_name = strdup(dir_name) ;
      iret = find_msstyle_file(dir_name) ;
      if(iret){
        printf("  Routine %s.  Unable to find directory %s\n",my_name,dir_name);
        free(dir_name) ;
        dir_name = (char *)malloc(strlen(ORdir)+strlen(con_dir) + 1 ) ;
        strcpy(dir_name,ORdir) ;
        strcat(dir_name,con_dir) ;
        cpy_name = strdup(dir_name) ;
        iret = find_msstyle_file(dir_name) ;
        if(iret){
          printf("  Routine %s error\n", my_name) ;
          printf("   Unable to find consists directory\n") ;
          printf("   Directory name = %s\n",cpy_name) ;
          printf("   Program stopping ...\n") ;
          free(dir_name) ;
          free(cpy_name) ;
          close_system() ;
        }
      }
      free(cpy_name) ;  cpy_name = NULL ;
/*
 *  Open Consists directory
 */
      if(ip)printf("  Consist directory name = %s\n",dir_name) ;

      cdir = opendir(dir_name) ;
      if(cdir == NULL){
        printf("  Routine %s error\n", my_name) ;
        printf("   Unable to find world directory\n") ;
        printf("   Directory name = %s\n",cpy_name) ;
        printf("   Program stopping ... \n") ;
        free(dir_name) ;
        close_system() ;
      }
/*
 *  Loop through files
 */
      while ((f_entry = readdir(cdir)) != NULL) {
        len = strlen(f_entry->d_name) ;
        if(ip)printf("  Found directory file = %s :: %s\n",
            f_entry->d_name,&(f_entry->d_name[len-4]) ) ;
        if(strncmp(".con",&(f_entry->d_name[len-4]),4)!= 0) continue ;
        if(ip)printf("  Found consist file = %s :: %s\n",
            f_entry->d_name,&(f_entry->d_name[len-4]) ) ;
        file_name = (char *)malloc(strlen(dir_name)+strlen(f_entry->d_name)+2) ;
        strcpy(file_name,dir_name) ;
        strcat(file_name,"/") ;
        strcat(file_name,f_entry->d_name) ;
        read_consist_file(file_name) ;
        free(file_name) ;
        file_name = NULL ;
      }
      closedir(cdir) ;
      free(dir_name) ;
/*
 *  Order consist files
 *  1.  Allocate array of pointers
 */
      n = 0 ;
      for(consist=consistlist_beg ; consist != NULL ; consist=consist->next){
        n++ ;
        root_name = zr_basename2(consist->file_name) ;
        if(strcmp(root_name,consist->name)){
          printf("  Consist file possible error.  Filename and consist name do not match.\n");
          printf("  Consist name = %s.  Filename = %s\n", consist->name, root_name) ;
        }
#if 0
        printf("  Consist name = %s.  Filename = %s\n",
                               consist->name, root_name) ;
#endif
        free(root_name) ;
      }
      if(n<1){
        printf("  Routine %s.  No consist files found.\n",my_name) ;
        return 0 ;
      }
      consist_a = (ConsistNode **)malloc(n*sizeof(ConsistNode *)) ;
/*
 *  Fill array of pointers
 */
      i = 0 ;
      for(consist=consistlist_beg; consist != NULL ; consist=consist->next){
        consist_a[i] = consist ;
        i++ ;
      }
/*
 * qsort
 */
      qsort(consist_a, n, sizeof(ConsistNode *), compare_consist_names) ;
/*
 *  Reorder list
 */
      consistlist_beg = consist_a[0] ;
      for(i=1;i<n;i++) consist_a[i-1]->next = consist_a[i] ;
      consist_a[n-1]->next = NULL ;
      free(consist_a) ;
      return 0 ;
}

int read_consist_file(char *file_name){

  int  ip = 0 ;
  int  iret   ;
  MSfile   msfile0 ;
  MSfile  *msfile = &msfile0 ;
  ConsistNode  *consist      ;
  char *token1 = NULL,
       *token2 = NULL,
       *token3 = NULL,
       *token4 = NULL,
       *token5 = NULL,
       *string = NULL,
       *string2= NULL ;

  char  *my_name = "read_consist_file" ;

      if(ip)printf("  Enter routine '%s'\n",my_name) ;
/*
 *  Open MS/KUJU style file
 */
      iret = open_msfile(file_name, msfile, 0, 0) ;
      if(iret){
        printf("  Routine '%s' error\n", my_name) ;
        printf("   Unable to open Microsoft/Kuju style file\n") ;
        printf("   File name = %s\n",file_name) ;
        printf("   Program stopping ... \n") ;
        free(file_name) ;
        file_name = NULL ;
        close_system() ;
      }
/*
 *  Create new consist node ;
 */
      consist = new_consist() ;
      consist->file_name = strdup(file_name) ;
      if(ip){
        printf("  Routine %s ::",my_name) ;
        printf("  New Consist pointer = %p\n",(void *)consist) ;
      }
      if(consist  == NULL) return 1 ;   // Error return
/*
 *   Level 0 : Train
 */
#if 1
      for(;;){
        if(token1)free(token1) ;
        token1 = ctoken(msfile) ;
        if(is_rbr(token1)) break ;
        if(!strcmp(token1,eof_mark)) break ;
        if(ip)printf("  AA  token 1 = %s \n",token1) ;
        SWITCH(token1)
/*
 *  Top level 1.  Train
 *    There should be a single top level Train token
 */
          CASE("Train")
            skip_lbr(msfile) ;
/*
 *  Level 2.  TrainCfg
 *    There should be a single level 2 TrainCfg token
 */
            for(;;){
              if(token2)free(token2) ;
              token2 = ctoken(msfile) ;
              if(is_rbr(token2)) break ;
              SWITCH(token2)
                CASE("TrainCfg")
                  skip_lbr(msfile) ;
                  consist->name = ctoken(msfile) ;
                  consist->name = strip_quotes(consist->name) ;
                  if(ip)printf("  Consist name = %s\n",consist->name) ;
/*
 *  Level 3.  Name. MaxVeloocity, ...
 */
                  for(;;){
                    if(token3)free(token3) ;
                    token3 = ctoken(msfile) ;
                    if(is_rbr(token3)) break ;
                    SWITCH(token3)
                      CASE("Name")
                        skip_lbr(msfile) ;
                        string = ctoken(msfile) ;
                        consist->description = strip_quotes(string) ;
                        skip_rbr(msfile) ;
                        break ;
                      CASE("Serial")
                        skip_lbr(msfile) ;
                        itoken(msfile)   ;  //  Skip
                        skip_rbr(msfile) ;
                        break ;
                      CASE("MaxVelocity")
                        skip_lbr(msfile) ;
                        consist->max_velocity     = dtoken(msfile) ;  // m/s
                        consist->max_acceleration = dtoken(msfile) ;  // m/s/s
                        skip_rbr(msfile) ;
                        break ;
                      CASE("NextWagonUID")
                        skip_lbr(msfile) ;
                        consist->n_wagons = itoken(msfile)   ;
                        skip_rbr(msfile) ;
                        break ;
                      CASE("Durability")
                        skip_lbr(msfile) ;
                        consist->durability = dtoken(msfile) ;
                        skip_rbr(msfile) ;
                        break ;
                      CASE("Engine")
                      CASE("Wagon")
                        skip_lbr(msfile) ;
/*
 *  Level 4.  Wagon name and directory
 */
                        for(;;){
                          if(token4)free(token4) ;
                          token4 = ctoken(msfile) ;
                          if(is_rbr(token4)) break ;
                          SWITCH(token4)
                            CASE("WagonData")
                            CASE("EngineData")
                              skip_lbr(msfile) ;
                              string  = ctoken(msfile) ;
                              string  = strip_quotes(string) ;
                              string2 = ctoken(msfile) ;
                              string2 = strip_quotes(string2) ;
                              add_wagon_to_consist(consist,string, string2) ;
                              if(string ){free(string) ; string  = NULL ; }
                              if(string2){free(string2); string2 = NULL ; }
                              skip_rbr(msfile) ;
                              break ;
                            CASE("Flip")
                              skip_lbr(msfile) ;
                              skip_rbr(msfile) ;
                              break ;
                            CASE("UiD")
                              skip_lbr(msfile) ;
                              itoken(msfile) ;  //  Skip
                              skip_rbr(msfile) ;
                              break ;
                            DEFAULT
                            printf("  Routine '%s' error\n",my_name) ;
                            printf("  Unrecognised level 4 token.  Token = '%s'",token4) ;
                            close_system()  ;
                          END
                        }
                        if(token4){free(token4) ; token4 = NULL; }
                        break ;
                      DEFAULT
                      printf("  Routine '%s' error\n",my_name) ;
                      printf("  Unrecognised level 3 token.  Token = '%s'",token3) ;
                      close_system()  ;
                    END
                  }
                  if(token3){free(token3) ; token3 = NULL; } ;
                  break ;
                DEFAULT
                printf("  Routine '%s' error\n",my_name) ;
                printf("  Unrecognised level 2 token.  Token = '%s'",token2) ;
                close_system()  ;
              END
            }
            if(token2){free(token2) ; token2 = NULL; } ;
            break ;
          DEFAULT
            printf("  Routine '%s' error\n",my_name) ;
            printf("  Unrecognised level 1 token.  Token = '%s'",token1) ;
            close_system()  ;
        END
      }
      if(token1)free(token1) ;
#endif
      close_msfile(msfile) ;
      return 0 ;
}

ConsistNode *new_consist(void){

  int    ip = 0;
  ConsistNode *consist ;

  char  *my_name = "new_consist" ;

      if(ip)printf("  Enter routine '%s'\n",my_name) ;
      consist = (ConsistNode *)malloc(sizeof(ConsistNode)) ;
      if(!consist){
        printf("  Routine '%s' error\n", my_name) ;
        printf("    Unable to create new consist\n") ;
        return NULL ;
      }
      consist->next = NULL ;
      consist->name = NULL ;
      consist->description = NULL ;
      consist->max_velocity = 20.0  ;   //  Default 20 m/s
      consist->durability   =  1.0  ;
      consist->n_wagons     =  0    ;
      consist->first        = NULL  ;
      consist->last         = NULL  ;
/*
 *  Add consist to list of consists
 */
      if(consistlist_beg == NULL){
        consistlist_beg = consist ;
      }else{
        consistlist_end->next = consist ;
      }
      consistlist_end = consist ;
#if 0
ConsistNode *c ;
      printf("  consistlist_beg = %p\n",(void *)consistlist_beg) ;
      printf("  consistlist_end = %p\n",(void *)consistlist_end) ;

      for(c = consistlist_beg ; c != NULL; c = c->next){
        printf("  Consist = %p :: '%s'\n",(void *)c, c->name) ;
      }
#endif
      return consist ;
}

int add_wagon_to_consist(ConsistNode *consist, char *wagon_name, char *directory){

  int           ip = 0 ;
  RawWagonNode  *rw    ;
  RawWagonNode  *raw_wagon = NULL ;
  DLPointerNode *new_node ;
  char  *my_name = "add_wagon_to_consist" ;

      if(ip)printf("  Enter routine '%s'\n",my_name) ;

      for(rw = rawwagonlist_beg; rw != NULL; rw = rw->next) {
        if(strcmp(rw->name, wagon_name))continue;
        raw_wagon = rw ;
        break ;
      }
      if(raw_wagon == NULL){
        printf("  Routine '%s' error.  Unable to find wagon.\n", my_name) ;
        printf("    Consist name = %s\n",consist->name) ;
        printf("    Description  = %s\n",consist->description) ;
        printf("    Wagon name   = %s\n",wagon_name) ;
        printf("    Directory    = %s\n",directory)  ;
        return 1 ;
      }
/*
 *  Add a new node to list.  Node contains RawWagonNode pointer for this wagon.
 */
      new_node = (DLPointerNode *)malloc(sizeof(DLPointerNode)) ;
      if(consist->first == NULL){
        consist->first = new_node ;
        new_node->prev = NULL  ;
      }else{
        new_node->prev      = consist->last ;
        consist->last->next = new_node         ;
      }
      consist->last  = new_node ;
      new_node->next = NULL ;

      new_node->pointer = (void *)raw_wagon ;

      return 0 ;
}
