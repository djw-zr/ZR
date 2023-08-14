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

int    i             ;
int    ip = 0        ;     // 0 = no printing
size_t slen, nread, ll=128 ;
char   *home         ;     // Full name of user's home directory
char   *dotdir       ;     // Full name of .zr directory
char   *config       ;     // Full name of config file
char   *fonts        ;     // Full name of fonts file
char   *line = NULL  ;
char   *token_a[3]   ;
char   *token        ;
FILE   *fptr = NULL  ;     // Pointer to config file
DIR    *dotzr        ;     // DIR pointer to .zr directory
DIR    *fontdir      ;     // DIR pointer to fonts directory
char *my_name = "process_defaults" ;

      if(ip)printf("  Enter %s\n",my_name) ;
      token_a[0] = token_a[1] = token_a[2] = NULL ;

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
        free(token_a[2]) ;
        token_a[2] = strdup(token) ;

        if(!strcmp(token_a[0],"MSTSdir")){
          free(MSTSdir);
          MSTSdir = token_a[2];
          token_a[2] = NULL ;               //  Leave string for MSTSdir
        }
        if(!strcmp(token_a[0],"ORdir")){
          free(ORdir);
          ORdir = token_a[2];
          token_a[2] = NULL ;               //  Remove link to string
        }
        if(!strcmp(token_a[0],"ORroutedir")){
          free(ORroutedir);
          ORroutedir = token_a[2];
          token_a[2] = NULL ;               //  Remove link to string
        }
      }
// Free malloc storage
      gclose(fptr) ;
      free(dotdir) ;
      free(config) ;
      free(line)   ;
      for(i=0;i<3;i++)free(token_a[i]);

      if(ip){
        printf("   MSTSdir    = %s\n",MSTSdir);
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
