int read_files_section(SmsTrgNode  *smstrg_node,  MSfile *msfile){

int  ip = 0 ;
int  i, n ;
SmsFilNode  *smsfil_node ;
char  *token8 = NULL,
      *token9 = NULL,
      *string = NULL ;
char  *my_name = "read_files_section";

      if(ip)printf("  Enter Routine %s\n",my_name) ;
      skip_lbr(msfile) ;
      n = smstrg_node->n_files = itoken(msfile) ;
      smstrg_node->smsfil_node =
          (SmsFilNode *)malloc(n*sizeof(SmsFilNode)) ;
      for(i=0;i<n;i++){
        smsfil_node = &(smstrg_node->smsfil_node[i]) ;
        smsfil_node->wav_file = NULL ;
        smsfil_node->iopt     = 0 ;
        smsfil_node->soundfile = NULL ;
      }
      for(i=0;;){
        token8 = ctoken(msfile);
        if(is_rbr(token8))break ;
        if(ip)printf("                  Level 8 :: token8 = %s \n",token8) ;
        SWITCH(token8)
          CASE("File")
            if(i > n){
              printf("  Routine %s error\n",my_name) ;
              printf("  StartLoop contains too many files\n") ;
              close_system() ;
            }
            skip_lbr(msfile) ;
            string = ctoken(msfile) ;
            smstrg_node->smsfil_node[i].wav_file = convert_dir_markers(string) ;
            smstrg_node->smsfil_node[i].iopt     = itoken(msfile) ;
            smstrg_node->smsfil_node[i].wav_file=
                          strip_quotes(smstrg_node->smsfil_node[i].wav_file) ;

            string = zr_corename(smstrg_node->smsfil_node[i].wav_file) ;
            if(ip)printf("  Routine %s.  Wave file = %s\n",my_name, string) ;
            if(!find_btree(wav_master,string)){
              wav_master = insert_node(wav_master,string,NULL );
              if(ip)printf("    ... added to btree wav_master\n") ;
            }
            skip_rbr(msfile) ;
            i++ ;
            break ;
          CASE("SelectionMethod")
          CASE("RandomMethod")               //  Error (??) ctn-freight-bogie.sms
            skip_lbr(msfile) ;
            token9 = ctoken(msfile) ;
            if(ip)printf("                    Level 9 :: token9 = %s \n",token9) ;
            SWITCH(token9)
              CASE("SequentialSelection")
                smstrg_node->sequential = 1 ;
                break ;
              CASE("RandomSelection")
                smstrg_node->random = 1 ;
                break ;
              DEFAULT
                printf("  Routine '%s' error\n",my_name) ;
                printf("  Unrecognised level 9 token.  Token = '%s'",token9) ;
                close_system()  ;
            END
            if(token9){free(token9) ; token9 = NULL; }
            skip_rbr(msfile) ;
            break ;
          DEFAULT
            printf("  Routine '%s' error\n",my_name) ;
            printf("  Unrecognised level 8 token.  Token = '%s'",token8) ;
            close_system()  ;
        END
        if(token8){free(token8) ; token8 = NULL; }
      }
      if(token8){free(token8) ; token8 = NULL; }
      smstrg_node->n_files = i ;  //  Allow for missing files.

//      printf("  Exit routine %s.  File 0 = %p\n ",my_name, smstrg_node->smsfil_node[0].wav_file) ;
      if(ip)printf("  Exit Routine %s\n",my_name) ;


      return 0 ;
}
