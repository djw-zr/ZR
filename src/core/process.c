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

//  Find users home directory

      home = getenv("HOME") ; // Pointer to string "HOME" in program address space
      slen = strlen(home) ;
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
        mkdir(dotdir, 0777);
      }else{
        if(ip)printf(" Users .zr directory exists.\n");
      }
      closedir(dotzr) ;

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
      while ((int)(nread = getline(&line, &slen, fptr)) != -1) {
        if(ip)printf("Retrieved line of length %zu:\n", nread);
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
      return ;
}
