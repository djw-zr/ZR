/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  find_routes.h
 *   Copyright 2025  David J. Webb
 *
 *   These routines are used to find routes and tabulate them ready for display
 *   by the ZR menu program 'ZM'.
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

/*
 * Routine find_top_dirs
 *
 *   This routine based on the original ZR program process_defaults, reads the
 *   ZR configuration file ".zr/config" and returns the list of directories in a
 *   BTree
 */

BTree  *find_top_dirs(void){

  int  ip = 0 ;
  int  i      ;
  size_t slen, nread, ll=128 ;

  char   *home       = NULL  ;     // Full name of user's home directory
  char   *dotdir     = NULL  ;     // Full name of .zr directory
  char   *config     = NULL  ;     // Full name of config file
  char   *routes_dir = NULL  ;
  char   *line       = NULL  ;
  char   *token_a[3] ;
  char   *token      = NULL  ;
  char   *string1    = NULL  ;
  char   *string2    = NULL  ;

  DIR    *dotzr      = NULL  ;     // DIR pointer to .zr directory
  FILE   *fptr       = NULL  ;     // Pointer to config file
  BTree  *ORdirs     = NULL  ;    // BTree with list of directories

  char *my_name = "find_top_dirs";

      if(ip)printf("  Enter routine %s\n",my_name) ;
      reset_bt_count() ;
/*
 *  Find users home directory
 */

#ifdef WINDOWS
      home = getenv("HOMEPATH") ;
#else
      home = getenv("HOME") ; // Pointer to string "HOME" in program address space
#endif
      slen = strlen(home) ;
      if(slen==0){
        printf(" Unable to determine users HOME directory\n");
        printf(" Program stopping ...\n");
        exit(EXIT_FAILURE);
      }
      if(ip)printf(" HOME = %s\n",home);
/*
 *  Search for $HOME/.zr
 *  If it does not exist create it.
 */
      slen = strlen(home) + strlen("/.zr")+ 1;
      dotdir = (char *)malloc((slen)*sizeof(char));
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
/*
 *  Does config file exist.  If not create it
 */
      slen = strlen(dotdir) + strlen("/config");
      config = (char *)malloc((slen+1)*sizeof(char));
      strcpy(config,dotdir)     ;
      strcat(config,"/config")  ;

      fptr = fopen(config,"r") ;
      if(ip)printf(" Testing for config file: %s %p\n",config,(void *)fptr);
      if(fptr == NULL){
        printf(" Creating new config file in directory .zr\n");
        fptr = fopen(config,"w");
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
        fprintf(fptr,"#  There can be more than one ORdir lines.  If so the\n");
        fprintf(fptr,"#    program lists the directories and asks which to use\n");
        fprintf(fptr,"#  The ORroutedie line can be left blank.  If the chosen \n");
        fprintf(fptr,"#    ORdir directory contains more than one route, the\n");
        fprintf(fptr,"#    program will ask which one to use.\n");
        fprintf(fptr,"#\n");
        fprintf(fptr,"MSTSdir    =\n");
        fprintf(fptr,"ORdir      =\n");
        fprintf(fptr,"ORroutedir =\n");
        fprintf(fptr,"#\n");
        fclose(fptr);
        printf("  Please now edit file '.zr/config' in your HOME directory\n");
        printf("  Giving the locations of the current system and the current route\n");
        return NULL ;
      }
/*
 *  If configure file exists use it to initialise directories.
 */
      if(ip)printf(" File .zr/config exists.\n\n");
      for(i=0;i<3;i++)token_a[i] =(char *)malloc(ll+1);   //  ll=128
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
 *  Find Basename of Directory
 */
          string1 = strdup(ORdir) ;
          string2 = zr_basename(string1) ;
          if(0 == strlen(string2)){
            printf("  ORdir name not recognised.  Full name = %s\n",ORdir);
            continue ;
          }
/*
 *  Add top level directory to Btree if it is new
 *  The Basenames are stored in alphabetical order
 */
          if(!find_btree(ORdirs,string2)){
            ORdirs = insert_node(ORdirs,strdup(string2),strdup(ORdir)) ;
            increment_bt_count((void *)ORdirs) ;
          }
          free(string1) ;  string1 = NULL ;
          free(string2) ;  string2 = NULL ;
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
      if(ip){
        printf("  ORdir      = %s\n",ORdir) ;
        printf("  ORroute    = %s\n",ORroute) ;
        printf("  ORroutedir = %s\n",ORroutedir) ;
        printf("  MSTSdir    = %s\n",MSTSdir) ;
      }
      if(ip)printf("  Exit routine %s\n",my_name) ;

      return ORdirs;
}

BTree  *find_route_dirs(void){

  int  ip = 0 ;
  int  i, n      ;
  size_t slen, nread, ll=128 ;

  char   *home       = NULL  ;     // Full name of user's home directory
  char   *dotdir     = NULL  ;     // Full name of .zr directory
  char   *config     = NULL  ;     // Full name of config file
  char   *routes_dir = NULL  ;
  char   *line       = NULL  ;
  char   *token_a[3] ;
  char   *token      = NULL  ;
  char   *string1    = NULL  ;
  char   *string2    = NULL  ;

  DIR    *dotzr      = NULL  ;     // DIR pointer to .zr directory
  FILE   *fptr       = NULL  ;     // Pointer to config file
  BTree  *bt_dirs    = NULL  ;
  char   *routes = "/Routes"    ;

  char *my_name = "find_route_dirs";

      if(ip)printf("  Enter routine %s\n",my_name) ;
      reset_bt_count() ;
      printf("  ORdname = %s\n", ORdname) ;
      printf("  ORdir   = %s\n", ORdir)   ;

      n = strlen(ORdir) + strlen(routes) + 2 ;
      routes_dir = malloc(n*sizeof(char)) ;
      strcpy(routes_dir,ORdir)  ;
      strcat(routes_dir,routes) ;
      zr_find_msfile2(&routes_dir) ;
      bt_dirs = bt_subdirectory_list(routes_dir) ;
      n       = return_bt_count() ;

//      printf("  ZZZ Number of routes = %i\n",n) ;
//      printf("    List of routes:\n") ;
//      print_bt_nodes_with_count_and_index(bt_dirs) ;
//      printf("  ORroutes = %p\n",(void *)bt_dirs) ;

      return bt_dirs ;
}

/*
 *==============================================================================
 *  Routine to determine route name by looking fir file name.trk
 *  in the route directory
 *==============================================================================
 */
char *find_route_name(char *route_dir){

  int  ip = 0 ;
  int  iret   ;
  DIR    *cdir      ;
  struct dirent *f_entry ;

  char *name = NULL ;
  char *ext ;

  char * my_name = "find_route_name" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
 *  Open directory
 */
      cdir = opendir(route_dir) ;
      iret = errno ;
      if(cdir == NULL){
        printf("  Routine %s error.\n", my_name) ;
        printf("    Unable to open route directory.\n") ;
        printf("    Rute directory = %s\n",route_dir) ;
        printf("    Error number = %i.  Error = %s\n",iret, strerror(iret)) ;
        printf("    Program stopping ... \n") ;
        return name ;
      }
/*
 *  Loop through files
 */
      while ((f_entry = readdir(cdir)) != NULL) {
        ext = zr_extension_lc(f_entry->d_name) ;
        if(ip)printf("  Filename = %s, extension = %s\n",f_entry->d_name, ext) ;
        if(!strcmp(ext,"trk"))break;
      }
      if(f_entry){
        name = zr_basename2(f_entry->d_name) ;
      }
      if(ext) free(ext) ;

      return name ;
}

BTree  *zm_find_consists(void){

  int  ip = 0 ;
  int  i, n      ;
  size_t slen, nread, ll=128 ;

  char   *home       = NULL  ;     // Full name of user's home directory
  char   *dotdir     = NULL  ;     // Full name of .zr directory
  char   *config     = NULL  ;     // Full name of config file
  char   *consist_dir = NULL  ;
  char   *line       = NULL  ;
  char   *token_a[3] ;
  char   *token      = NULL  ;
  char   *string1    = NULL  ;
  char   *string2    = NULL  ;

  DIR    *dotzr      = NULL  ;     // DIR pointer to .zr directory
  FILE   *fptr       = NULL  ;     // Pointer to config file
  BTree  *bt_dirs    = NULL  ;
  char   *consists = "/Trains/Consists"    ;

  char *my_name = "zm_find_consists";

      if(ip)printf("  Enter routine %s\n",my_name) ;
      reset_bt_count() ;
//      printf("  ORdname = %s\n", ORdname) ;
//      printf("  ORdir   = %s\n", ORdir)   ;

      n = strlen(ORdir) + strlen(consists) + 2 ;
      consist_dir = malloc(n*sizeof(char)) ;
      strcpy(consist_dir,ORdir)  ;
      strcat(consist_dir,consists) ;
      zr_find_msfile2(&consist_dir) ;

//      bt_dirs = bt_subdirectory_list(routes_dir) ;
//      n       = return_bt_count() ;

//      printf("  ZZZ Number of routes = %i\n",n) ;
//      printf("    List of routes:\n") ;
//      print_bt_nodes_with_count_and_index(bt_dirs) ;
//      printf("  ORroutes = %p\n",(void *)bt_dirs) ;

      return bt_dirs ;
}
