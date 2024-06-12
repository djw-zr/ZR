/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  process.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routine to initialise system and top level variables MSTSDir, ORdir,
 *   ORroutedir etc.  See zr.h.
 *

In windows you can view all environment variables available on your device using
the Powershell command
 Get-ChildItem Env: | Sort Name


Name                           Value
----                           -----
ALLUSERSPROFILE                C:\ProgramData
APPDATA                        C:\Users\David\AppData\Roaming
CommonProgramFiles             C:\Program Files\Common Files
CommonProgramFiles(x86)        C:\Program Files (x86)\Common Files
CommonProgramW6432             C:\Program Files\Common Files
COMPUTERNAME                   IDEALPAD
ComSpec                        C:\Windows\system32\cmd.exe
DriverData                     C:\Windows\System32\Drivers\DriverData
FPS_BROWSER_APP_PROFILE_STRING Internet Explorer
FPS_BROWSER_USER_PROFILE_ST... Default
HOMEDRIVE                      C:
HOMEPATH                       \Users\David
LOCALAPPDATA                   C:\Users\David\AppData\Local
LOGONSERVER                    \\IDEALPAD
NUMBER_OF_PROCESSORS           2
OneDrive                       C:\Users\David\OneDrive
OS                             Windows_NT
Path                           C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPo...
PATHEXT                        .COM;.EXE;.BAT;.CMD;.VBS;.VBE;.JS;.JSE;.WSF;.WSH;.MSC;.CPL
PROCESSOR_ARCHITECTURE         AMD64
PROCESSOR_IDENTIFIER           Intel64 Family 6 Model 76 Stepping 4, GenuineIntel
PROCESSOR_LEVEL                6
PROCESSOR_REVISION             4c04
ProgramData                    C:\ProgramData
ProgramFiles                   C:\Program Files
ProgramFiles(x86)              C:\Program Files (x86)
ProgramW6432                   C:\Program Files
PSModulePath                   C:\Users\David\Documents\WindowsPowerShell\Modules;C:\Program Files\WindowsPowerShell...
PUBLIC                         C:\Users\Public
SESSIONNAME                    Console
SystemDrive                    C:
SystemRoot                     C:\Windows
TEMP                           C:\Users\David\AppData\Local\Temp
TMP                            C:\Users\David\AppData\Local\Temp
USERDOMAIN                     IDEALPAD
USERDOMAIN_ROAMINGPROFILE      IDEALPAD
USERNAME                       David
USERPROFILE                    C:\Users\David
windir                         C:\Windows
 *
 *==============================================================================
 */


/*
 *  Routine to initialise or process the defaults file $HOME/.zr/config
 */
void process_defaults(){

int    i, n, n_dirs  ;
int    ip = 0        ;     // 0 = no printing
size_t slen, nread, ll=128 ;
char   *home         ;     // Full name of user's home directory
char   *dotdir       ;     // Full name of .zr directory
char   *config       ;     // Full name of config file
char   *fonts        ;     // Full name of fonts file
char   *routes_dir   ;
char   *line = NULL  ;
char   *token_a[3]   ;
char   *token        ;
char   *string1, *string2 ;
FILE   *fptr = NULL  ;     // Pointer to config file
DIR    *dotzr        ;     // DIR pointer to .zr directory
DIR    *fontdir      ;     // DIR pointer to fonts directory
BTree  *ORdirs = NULL ;    // BTree with list of directories
BTree  *bt_node      ;
BTree  *bt_dirs      ;
char   *routes = "/Routes"    ;
char *my_name = "process_defaults" ;

      if(ip)printf("  Enter %s\n",my_name) ;
      token_a[0] = token_a[1] = token_a[2] = NULL ;
      reset_bt_count() ;           //  Reset counter of ORdirs

//  Find users home directory
#ifdef WINDOWS
      home = getenv("HOMEPATH") ;
#else
      home = getenv("HOME") ; // Pointer to string "HOME" in program address space
#endif
      if(ip)printf("  AA\n") ;
      if(ip)printf("  BB ::%s:: \n",home) ;
      slen = strlen(home) ;
      if(ip)printf("  BB ::%i:: \n",(int)slen) ;
      if(slen==0){
        printf(" Unable to determine users HOME directory\n");
        printf(" Program stopping ...\n");
        exit(EXIT_FAILURE);
      }
      if(ip)printf(" HOME = %s\n",home);

//  Does $HOME/.zr exist?  If not create it.

      slen = strlen(home) + strlen("/.zr");
      dotdir = (char *)malloc((slen+1)*sizeof(char));
      strcpy(dotdir,home)         ;
      strcat(dotdir,"/.zr")   ;

      dotzr = opendir(dotdir) ;
      if(dotzr == NULL){
        printf(" Creating new .zr directory in users home folder\n");
#ifdef WINDOWS
        mkdir(dotdir);
#else
        mkdir(dotdir, 0777);
#endif
      }else{
        if(ip)printf(" Users .zr directory exists.\n");
      }
      closedir(dotzr) ;
      ZRdotdir = strdup(dotdir) ;
      printf(" ZRdotdir = %s\n",ZRdotdir) ;

//  Does fonts directory exist?

      slen = strlen(dotdir) + strlen("/fonts");
      fonts = (char *)malloc((slen+1)*sizeof(char));
      strcpy(fonts,dotdir)     ;
      strcat(fonts,"/fonts")  ;

      fontdir = opendir(fonts) ;
      if(ip)printf(" Testing for fonts directory: %s %p\n",fonts,(void *)fontdir);
      if(fontdir == NULL){
// Look for webcore fonts in system fonts directory
        free(fonts) ;
        slen = strlen("/usr/share/fonts/webcore");
        fonts = (char *)malloc((slen+1)*sizeof(char));
        strcpy(fonts,"/usr/share/fonts/webcore")     ;
        fontdir = opendir(fonts) ;
      }
      if(fontdir == NULL){
        printf(" ERROR:  Webcore font directory not found\n") ;
        printf(" ERROR:  Display text will fail\n") ;
        ZRfonts = NULL ;
      }else{
        closedir(fontdir) ;
        ZRfonts = fonts ;
        if(ip)printf(" Fonts (WebFonts) directory exists.\n");
      }

//  Does config file exist?  If not create it

      slen = strlen(dotdir) + strlen("/config");
      config = (char *)malloc((slen+1)*sizeof(char));
      strcpy(config,dotdir)     ;
      strcat(config,"/config")  ;

      fptr = gopen(config,"r") ;
      if(ip)printf(" Testing for config file: %s %p\n",config,(void *)fptr);
      if(fptr == NULL){
        printf(" Creating new config file in directory .zr\n");
        fptr = gopen(config,"w");
        if(ip)printf(" File pointer is %p\n",(void *)fptr);
        fprintf(fptr,"#\n");
        fprintf(fptr,"#  File .zr/config\n");
        fprintf(fptr,"#\n");
        fprintf(fptr,"#  The file contains the names of directories used by ZR\n");
        fprintf(fptr,"#\n");
        fprintf(fptr,"#  MSTSdir    // Top directory for MSTS files (if any)\n");
        fprintf(fptr,"#  ORdir      // Top directory of current OR system\n");
        fprintf(fptr,"#  ORroutedir // Top directory of current route\n");
        fprintf(fptr,"#\n");
        fprintf(fptr,"MSTSdir    =\n");
        fprintf(fptr,"ORdir      =\n");
        fprintf(fptr,"ORroutedir =\n");
        fprintf(fptr,"#\n");
        gclose(fptr);
        printf("  Please now edit file '.zr/config' in your HOME directory\n");
        printf("  Giving the locations of the current system and the current route\n");
        return ;
      }
/*
 *  If configure file exists use it to initialise directories.
 */
      if(ip)printf(" File .zr/config exists.\n\n");
      for(i=0;i<3;i++)token_a[i] =(char *)malloc(ll+1);
      slen = 0 ;
      printf("  Content of config file: \n") ;
#ifdef SDL2
      while ((int)(nread = getline1(&line, &slen, fptr)) != -1) {
#else
      while ((int)(nread = getline(&line, &slen, fptr)) != -1) {
#endif
        if(ip)printf("Retrieved line of length %i: \n", (int)nread);
        fwrite(line, nread, 1, stdout);
        if(ip)printf(" Line = ::%s::\n",line) ;

//  Routine strtok usually uses the same memory locations to store text.
//  Thus if needed later the text needs to be copied to another location,
//  here 'token_a'.

        token = strtok(line," \n\r\t");
        if((token==NULL))continue ;         // If blank skip line
        if(token[0] == '#')continue;        // If comment ('#') skip line
        free(token_a[0]) ;
        token_a[0] = strdup(token) ;        // "strdup" defined in system.c

        token = strtok(NULL," \n\r\t");
        if((token==NULL))continue ;         // If no 2nd token skip line
        if(strcmp(token,"="))continue;      // If not '=' skip line
        free(token_a[1]) ;
        token_a[1] = strdup(token) ;

        token = strtok(NULL," \n\r\t");
        if((token==NULL))continue ;         //  If no 3rd token skip line
        while(token[strlen(token)-1] == '/')
              token[strlen(token)-1] = '\0' ;  // Remove any final '/'
        free(token_a[2]) ;
        token_a[2] = strdup(token) ;

        if(!strcmp(token_a[0],"MSTSdir")){
#if 1
          free(MSTSdir);
          MSTSdir = token_a[2];
          token_a[2] = NULL ;               //  Leave string for MSTSdir
#else
          free(token_a[2]) ; token_a[2] = NULL ;
          printf("  Token MSTSdir no longer used\n") ;
#endif
        }else if(!strcmp(token_a[0],"ORdir")){
          free(ORdir);
          ORdir = token_a[2];
          token_a[2] = NULL ;               //  Remove link to string
/*
 *  Find basename of directory
 */
          string1 = strdup(ORdir) ;
          string2 = zr_basename(string1) ;
          if(0 == strlen(string2)){
            printf("  ORdir name not recognised.  Full name = %s\n",ORdir);
            continue ;
          }
/*
 *  Add top level directory to Btree if it is new
 */
          if(!find_btree(ORdirs,string2)){
            ORdirs = insert_node(ORdirs,strdup(string2),strdup(ORdir)) ;
            increment_bt_count((void *)ORdirs) ;
          }
          free(string1) ;
          free(string2) ;
        }else if(!strcmp(token_a[0],"ORroutedir")){
          free(ORroutedir);
          ORroutedir = token_a[2];
          printf("  Token ORroutedir no longer used\n") ;
          token_a[2] = NULL ;               //  Remove link to string
        }else if(!strcmp(token_a[0],"ORroute")){
          if(ORroute){
            printf("  Token ORroute redefined\n") ;
            free(ORroute) ;
          }
          ORroute = token_a[2] ;
          token_a[2] = NULL ;
        }
      }
/*
 *  If number of directories in top level lisy is greater than one
 *     ask user to choose:
 */
      n_dirs = return_bt_count() ;
      printf("  Number of ORdir directories = %i\n",n_dirs) ;
      printf("    List of directories:\n") ;
      print_bt_nodes_with_count_and_index(ORdirs) ;

      if(n_dirs > 1){
#ifndef DEBUG1
        n = 0 ;
        while(n<1 || n>n_dirs){
          printf("  Enter index of top level directory to use (0 to exit):\n") ;
          scanf("%i",&n) ;
          if(n<1) exit(0) ;
        }
#else
        n = 4 ;
#endif
      }else if(1==n_dirs){
        n = 1 ;
      }else{
        printf("  Error:  Configuration file contains no 'ORdir'.\n") ;
        printf("          Program stopping ... \n") ;
        exit(0) ;
      }
      bt_node = find_bt_node_with_index(ORdirs,n) ;
      printf("  Found  t_node = %p\n",(void *) bt_node) ;
      if(bt_node)printf("    index = %s\n    name = %s\n",
            (char *)bt_node->index, (char *)bt_node->data) ;
      ORdir = (char *)bt_node->data ;
/*
 *  If ORroute is not defined ask user to choose:
 */
      if(ORdir && !ORroute){
        reset_bt_count() ;
        n = strlen(ORdir) + strlen(routes) + 2 ;
        routes_dir = malloc(n*sizeof(char)) ;
        strcpy(routes_dir,ORdir)  ;
        strcat(routes_dir,routes) ;
        zr_find_msfile2(&routes_dir) ;
        bt_dirs = bt_subdirectory_list(routes_dir) ;
        n_dirs  = return_bt_count() ;

        printf("  Number of routes = %i\n",n_dirs) ;
        printf("    List of routes:\n") ;
        print_bt_nodes_with_count_and_index(bt_dirs) ;
        if(n_dirs > 1){
#ifndef DEBUG1
          n = 0 ;
          while(n<1 || n>n_dirs){
            printf("  Enter index of route to use (0 to exit):\n") ;
            scanf("%i",&n) ;
            if(n<1) exit(0) ;
          }
#else
          n = 7 ;
#endif
          bt_node = find_bt_node_with_index(bt_dirs,n) ;
        }else if(n_dirs == 1){
          bt_node = find_bt_node_with_index(bt_dirs,1) ;
        }else{
          printf("  ERROR:  No routes found.\n") ;
          printf("          Program stopping ... \n") ;
          exit(0) ;
        }
        ORroute = strdup(bt_node->index) ;
        ORroutedir = (char *)bt_node->data ;
        printf("  Found  t_node = %p\n",(void *) bt_node) ;
        printf("    ORroute = %s\n    ORroutedir = %s\n",
                     ORroute, ORroutedir) ;

/*
 *  If only one ORdir and ORroute specified construct Route directory
 */
      }else if(!ORroutedir && ORdir && ORroute){
        n = strlen(ORdir) + strlen(routes) + strlen(ORroute) + 1 ;
        if(ORroutedir)free(ORroutedir) ;
        ORroutedir = (char *)malloc(n) ;
        strcpy(ORroutedir,ORdir) ;
        strcat(ORroutedir,routes)   ;
        strcat(ORroutedir,ORroute) ;
      }
/*
 *  Check for error
 */
      if(!ORdir || !ORroutedir){
        printf("  ERROR.  Base directory or route not set.\n") ;
        printf("    Base directory  = %s\n",ORdir)      ;
        printf("    Route directory = %s\n",ORroutedir) ;
        printf("    Program stopping ... \n") ;
        exit(0) ;
      }
      printf("  ORdir      = %s\n",ORdir) ;
      printf("  ORroute    = %s\n",ORroute) ;
      printf("  ORroutedir = %s\n",ORroutedir) ;
/*
 *  Free malloc storage
 */
      gclose(fptr) ;
      free(dotdir) ;
      free(config) ;
      free(line)   ;
      for(i=0;i<3;i++)free(token_a[i]);

      printf("  AA ORdir   = %s\n",ORdir) ;
      printf("  ORroute    = %s\n",ORroute) ;
      printf("  ORroutedir = %s\n",ORroutedir) ;
      if(ip){
        printf("   ORdir      = %s\n",ORdir);
        printf("   ORroutedir = %s\n",ORroutedir);
      }
#ifdef ROUTE_MSTS
      printf("   Program ZR will use MSTS route %s\n",msts_route) ;
      free(ORdir) ;
      ORdir = MSTSdir ;
      free(ORroutedir) ;
      ORroutedir = malloc(strlen(ORdir)+strlen(msts_route)+10) ;
      strcpy(ORroutedir,ORdir) ;
      strcat(ORroutedir,"ROUTES/") ;
      strcat(ORroutedir,msts_route) ;
      strcat(ORroutedir,"/") ;
      printf("   Home directory  = %s\n",ORdir);
      printf("   Route directory = %s\n",ORroutedir);
#endif
      return ;
}
