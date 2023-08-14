/*
 *******************************************************************************
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File : shape_d.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to handle the loading of shape *.sd files
 *
 *   Most if not all shape (*.s) files have an companion (*.sd) file.  These
 *   are (usually/always?) MSTS text files containing tokens starting "esd_",
 *   the accronym presumably standing for Extended or Extra Shape
 *   Data/Descriptor.
 *
 *   The structure of the files is straight forward but ShapeDescriptorFile.cs
 *   warns that some fields may have extra values.
 *
 * *****************************************************************************
 */

int load_shape_d(ShapeNode *snode){

  int     ip = 0 ;                     //  Controls printing
  int     i, j, l, n ;
  char    *token, *token1    ;
  char    *string   ;
  char    *cp       ;
  MSfile  msfile0 ;
  MSfile  *msfile = &msfile0 ;
  char    *shape_file_name   ;
  char    *snode_shape_file  ;
  char    myname[] = "load_shape_d" ;

      if(ip)printf("\n  Enter routine : %s\n",myname);
      if(ip)printf("  File = %s\n",snode->sd_file);
      ip = ip && !strcmp(snode->name,test_shape) ;

/*
 *  Initialise  ESD variables
 */
      snode->esd_detail_level          = 0    ;
      snode->esd_alternative_texture   = 0    ;
      snode->esd_no_visual_obstruction = 0    ;
      snode->esd_snapable              = 0    ;
      snode->esd_sound_file_name       = NULL ;
      snode->esd_bell_animation_fps    = 0.0  ;
      snode->esd_bounding_box_xmin     = 0.0  ;
      snode->esd_bounding_box_xmax     = 0.0  ;
      snode->esd_bounding_box_ymin     = 0.0  ;
      snode->esd_bounding_box_ymax     = 0.0  ;
      snode->esd_bounding_box_zmin     = 0.0  ;
      snode->esd_bounding_box_zmax     = 0.0  ;
//  Return if there is no *.sd file.
      if(NULL == snode->sd_file) return 0 ;
/*
 * =============================================================================
 *  open_msfile reads and checks the first 16 bytes of the file, inflates
 *  compressed files, reads the next 16 bytes and sets the flags.
 * =============================================================================
 */
      l = open_msfile(snode->sd_file, msfile, 0, 0) ;
      if(l!=0){
        printf("\n\n  ERROR : Routine open_msfile failed to open file\n\n");
        exit(1) ;
      }
/*
 *  Check for top level 'Shape' token
 */
      token = new_tmp_token(msfile) ;  // Temporaty token !!
      for(i=0;i<(int)strlen(token);i++)token[i]=toupper(token[i]) ;
      string = (char *)"SHAPE" ;
      if(0 != strcmp(token,string)){
        printf(" Routine %s.\n", myname);
        printf("   File name = %s\n",snode->sd_file);
        printf("   Wrong token found at top level.\n");
        printf("   Token expected = %s\n","Shape");
        printf("   Token found    = %s\n",token);
        free(token) ;
        close_msfile(msfile) ;
        return 1 ;
      }
      skip_lbr(msfile) ;           // skip left bracket
/*
 *  Check name of associated shape file
 */
      shape_file_name = ctoken(msfile) ;

      for(i=0;i<(int)strlen(shape_file_name);i++){
          shape_file_name[i]=tolower(shape_file_name[i]) ;
      }
      cp = strrchr(snode->s_file,'/') ;
      if(NULL == cp){
        cp = snode->s_file ;
      }else{
        cp++ ;
      }
      l = strlen(cp) ;
      snode_shape_file = (char *)malloc(l+1) ;
      for(i=0;i<l;i++){
        snode_shape_file[i] = tolower(cp[i]) ;
      }
      snode_shape_file[l] = '\0' ;

      if(strcmp(shape_file_name,snode_shape_file)){
        printf("     Routine load_shape_d() error :: Expected shape file name = %s\n",snode_shape_file) ;
        printf("                                  :: Actual shape file name   = %s\n",shape_file_name) ;
      }
      free(shape_file_name) ;
      free(snode_shape_file) ;
/*
 *  Cycle over first level tokens
 */
      for(;;){
        token = ctoken(msfile);
        if(ip)printf("  token = %s\n",token) ;
        if(')' == token[0])break ;                   // End of data

        SWITCH (token)
          CASE ("ESD_Detail_Level")
          CASE ("ED_Detail_Level")                  // a2tBrdg250r5d.sd and
            skip_lbr(msfile) ;                      // ukn2t125r5d.sd
            snode->esd_detail_level = itoken(msfile) ;
            skip_rbr(msfile) ;
            break ;
          CASE ("ESD_Alternative_Texture")
          CASE ("ESD_Alernative_Texture")
          CASE ("ESD_Alternative_Teture")   //  JP1Multicarpark.sd
            skip_lbr(msfile) ;
            snode->esd_alternative_texture = itoken(msfile) ;
            skip_rbr(msfile) ;
            break ;
          CASE ("ESD_Tunnel")
            skip_lbr(msfile) ;
            snode->esd_tunnel_length = dtoken(msfile) ;
            snode->esd_tunnel_f1     = dtoken(msfile) ;
            snode->esd_tunnel_f2     = dtoken(msfile) ;
            snode->esd_tunnel_i1     = itoken(msfile) ;
            snode->esd_tunnel_i2     = itoken(msfile) ;
            skip_rbr(msfile) ;
            break ;
          CASE ("ESD_No_Visual_Obstruction")
          CASE ("ESD_No_Visul_Obstruction")    //  ukn2t500r10d.s
            skip_lbr(msfile) ;
            snode->esd_no_visual_obstruction = itoken(msfile) ;
            skip_rbr(msfile) ;
            break ;
          CASE ("ESD_No_Snapable")
            skip_lbr(msfile) ;
            snode->esd_snapable = itoken(msfile) ;
            skip_rbr(msfile) ;
            break ;
          CASE ("ESD_Snapable")
            skip_lbr(msfile) ;
            snode->esd_snapable = 0 ;
            skip_rbr(msfile) ;
            break ;
          CASE ("ESD_Software_DLev")
            skip_lbr(msfile) ;
            itoken(msfile) ;
            skip_rbr(msfile) ;
            break ;
          CASE ("ESD_Subobj")
          CASE ("ESD_SubObj")
            skip_lbr(msfile) ;
            itoken(msfile) ;
            skip_rbr(msfile) ;
//            skip_token_block(msfile) ;
            break ;
          CASE ("ESD_Bounding_Box")
            skip_lbr(msfile) ;
            token = ctoken(msfile) ;
            if(is_rbr(token)) break ;

            snode->esd_bounding_box_xmin = atof(token) ; ;
            snode->esd_bounding_box_ymin = dtoken(msfile) ;
            snode->esd_bounding_box_zmin = dtoken(msfile) ;
            snode->esd_bounding_box_xmax = dtoken(msfile) ;
            snode->esd_bounding_box_ymax = dtoken(msfile) ;
            snode->esd_bounding_box_zmax = dtoken(msfile) ;
/*
 *  Deal with faulty ESD_Bounding_Box (JP2grntext.sd)
 */
            if(0<skip_rbr_1(msfile)){
              snode->esd_bounding_box_xmin = 0.0 ;
              snode->esd_bounding_box_ymin = 0.0 ;
              snode->esd_bounding_box_zmin = 0.0 ;
              snode->esd_bounding_box_xmax = 0.0 ;
              snode->esd_bounding_box_ymax = 0.0 ;
              snode->esd_bounding_box_zmax = 0.0 ;
              skippast_rbr(msfile) ;
            }
            break ;
          CASE ("ESD_No_Ortssoundfilename")
            skip_lbr(msfile) ;
            snode->esd_sound_file_name = ctoken(msfile) ;
            skip_rbr(msfile) ;
            break ;
          CASE ("ESD_Ortsbellanimationfps")
            skip_lbr(msfile) ;
            snode->esd_bell_animation_fps = dtoken(msfile) ;
            skip_rbr(msfile) ;
            break ;
          CASE ("ESD_Complex")
          CASE ("ESD_Complex_Box")  //  JP1A1t50mStrtElev.sd !!
            if(ip)printf("  CASE ESD_Complex\n") ;
            skip_lbr(msfile) ;
            n = itoken(msfile) ;
            snode->n_esd_complex_box = n ;
            if(ip)printf("  ESD_Complex : n = %i\n", n) ;
            snode->esd_complex_box = (float *)malloc(n*12*sizeof(float));

            for(i=0;i<n;i++){
              if(i==0 && !strcmp(token,"ESD_Complex_Box")){
                token1 = strdup(token) ;
              }else{
                token1 = ctoken(msfile);
              }
              if(ip)printf("  ESD_Complex : i = %i,token1 = %s\n",i,token1) ;
              if(is_rbr(token1)){
                return_token(token1,msfile) ;
                snode->n_esd_complex_box = i ;
                break ;
              }
              free(token1) ;
              skip_lbr(msfile) ;
              for(j=0;j<12;j++){
                snode->esd_complex_box[i*12+j] = dtoken(msfile) ;
                if(ip)printf("  ESD_Complex i = %i, j = %i, data = %f\n",
                                 i,j,snode->esd_complex_box[i*12+j]) ;
              }
              skip_rbr(msfile) ;
            }
            skip_rbr(msfile) ;
            break ;
/*
 *  Should the use of the 'Size' token be flagged as an error??
 */
          CASE ("ESD_Sphere")
          CASE ("ESD_No_Visual_Obstruction")
          CASE ("Size")
            skip_lbr(msfile) ;
            skippast_rbr(msfile) ;
            break ;
          DEFAULT
            printf("  Routine load_shape_d() error\n");
            printf("    sd file  = %s\n",snode->sd_file) ;
            printf("    s file   = %s\n",snode->s_file) ;
            printf("    Unrecognised token = %s\n",token) ;
            break ;
        END
        free(token) ;
      }
      free(token) ;
      close_msfile(msfile);
      return 0;
}

