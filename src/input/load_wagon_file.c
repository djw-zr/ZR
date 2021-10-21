 /*
 * *****************************************************************************
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File: load_wagon_file.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   The code is based on Openrails file MSTSWagon.cs
 *
 *   Routine to:
 *   1. Read one fo the wagon or engine files and save the data in the
 *      related wagon structure
 *
 *   TODO
 *   1.  The MSTS 'wagon' section of a file may be followed by
 *       an OR 'engine' section with more information.
 *
 * *****************************************************************************
 */
char *read_scaled_token(MSfile *msfile) ;
//int  check_wheel_radii(RawWagonNode *w) ;
char string[4096] ;

int read_raw_wagon_file(RawWagonNode *w){

int      i, l, n, cdir_len ;
int      ip = 0 ;  //Debug
float    temp   ;
MSfile   msfile0           ;
MSfile   *msfile = &msfile0 ;
char     my_name[] = "read_raw_wagon_file" ;
char     *string,
         *token = NULL ,
         *cdir  = NULL ,             //  Current wagon directory
         *no_type        = "No Type",
         *no_description = "\"No Description\"" ;
char     ffname[] = "__FILE__"  ;

      if(ip)printf("  Enter %s\n",my_name) ;
      if(ip)printf("    wagon = %s\n",w->name) ;

/*
 * ***********************************************************************
 *  Open File
 * ***********************************************************************
 */
      l = open_msfile(w->file, msfile, 0, 0);
      if(l!=0){
        printf("\n\n  ERROR : Routine open_msfile failed to open file\n\n");
        exit(1) ;
      }
      if(ip)printf(" msfile file     = %s\n",w->file);
      if(ip)printf(" msfile pointer  = %p\n\n",(void *)msfile->fp);

      cdir = dirname(w->file) ;
      cdir_len = strlen(cdir) ;
      if(ip)printf(" DD cdir_len = %i, cdir = %s\n",cdir_len,cdir) ;
/*
 * Check for 'Wagon' token
 */
      token = new_tmp_token(msfile) ;  // Temporaty token !!
      string = (char *)"Wagon" ;
      if(ip)printf("  Token = %s :: %s\n",token,string) ;
      if(0 != strcmp(token,string))error2(my_name,ffname,w->file,string);
      skip_lbr(msfile) ;           // skip left bracket
/*
 *  Read wagon name
 */
      if(NULL != w->name)free(w->name);
      w->name = ctoken(msfile) ;
//      ip = !strcmp(w->name,"NI4wHopUnb2LD") ;
//      ip = !strcmp(w->name,"1905-P708") ;     <- Has the extra engine data
      if(ip)printf("  Name  = %s\n",w->name) ;

      w->ncoupling = 0 ;
//
//  TODO  Initialise wagon structure with default values
//
      w->wheelradius           = 0.5 ;
      w->driverwheelradius     = 1.0 ;
      w->inv_wheelradius       = 2.0 ;
      w->inv_driverwheelradius = 1.0 ;

      w->type      = no_type  ;
      w->full_name = no_description  ;
      w->fs_file   = NULL     ;   //  Freight shape file
      w->f_shape   = NULL     ;
      w->f_max_level = 0.0    ;
      w->f_min_level = 0.0    ;
      w->f_anim_flag = 0      ;

/*
 * ***********************************************************************
 *  Loop over tokens
 * ***********************************************************************
 */
      for(;;){
        token = new_tmp_token(msfile);
        if(ip)printf(" AA Token = :%s:\n",token) ;
        if(is_rbr(token)) break ;  // End of Wagon data
        SWITCH(token)
          CASE("Comment")
            skip_lbr(msfile) ;
            skipto_rbr(msfile) ;
            break ;
          CASE("Type")
            skip_lbr(msfile) ;
            w->type = ctoken(msfile) ;
            if(ip)printf("    Type = :%s:\n",w->type) ;
            skip_rbr(msfile) ;
            break ;
          CASE("WagonShape")
            skip_lbr(msfile) ;
//            if(NULL != w->s_file) free(w->s_file) ;
            free(w->s_file) ;
            token = new_tmp_token(msfile) ;
            w->s_file = (char *)malloc((strlen(token)+cdir_len+2)
                                                     *sizeof(char)) ;
            strcpy(w->s_file,cdir) ;
            strcat(w->s_file,"/") ;
            strcat(w->s_file,token) ;
            if(ip)printf("  WagonShape :: Name: %s :: Token %s :: Shape: %s\n",
                          w->name,token,w->s_file) ;
//            w->s_file = ctoken(msfile) ;
            if(ip)printf("    Shape file = :%s:\n",w->s_file) ;
            skip_rbr(msfile) ;
            break ;
          CASE("FreightAnim")
            if(ip)printf("  Enter FreightAnim\n") ;
            skip_lbr(msfile) ;
            free(w->fs_file) ;
            token = new_tmp_token(msfile) ;
            if(ip)printf(" Token = %s\n",token) ;
            w->fs_file = (char *)malloc((strlen(token)+cdir_len+2)
                                                     *sizeof(char)) ;
            strcpy(w->fs_file,cdir) ;
            strcat(w->fs_file,"/") ;
            strcat(w->fs_file,token) ;
            token = new_tmp_token(msfile) ;
            if(ip)printf(" Token = %s\n",token) ;
            w->f_max_level  = convert_unit(token,"m") ;
            token = new_tmp_token(msfile) ;
            if(ip)printf(" Token = %s\n",token) ;
            w->f_min_level  = convert_unit(token,"m") ;
            token = new_tmp_token(msfile) ;
            if(!is_rbr(token)){
              if(ip)printf(" Token = %s\n",token) ;
              temp  = convert_unit(token,"m") ;
              w->f_anim_flag = (temp != 0.0) ;
              skip_rbr(msfile) ;
            }
            if(ip)printf("    FreightShape file = :%s:\n",w->fs_file) ;

            break ;
          CASE("Size")
            skip_lbr(msfile) ;
            token = read_scaled_token(msfile) ;
            w->width  = convert_unit(token,"m") ;
            token = read_scaled_token(msfile) ;
            w->height = convert_unit(token,"m") ;
            token = read_scaled_token(msfile) ;
            w->length = convert_unit(token,"m") ;
            skip_rbr(msfile) ;
            break ;
          CASE("Mass")
            skip_lbr(msfile) ;
            token = read_scaled_token(msfile) ;
            if(ip)printf("  Mass = %s\n",token) ;
            w->mass  = convert_unit(token,"kg") ;
            skip_rbr(msfile) ;
            break ;
/*
 *  NOTE OpenRails also allows for a 'engine' wheel radius
 *       which it saves as driverwheelradius.
 *       Here it is set equal to wheel radius
 */
          CASE("WheelRadius")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->wheelradius  = convert_unit(token,"m") ;
            if(w->wheelradius < 0.25)w->wheelradius = 0.5 ; // Error?
            w->driverwheelradius = w->wheelradius ;
            w->inv_wheelradius = 1.0/w->wheelradius ;
            w->inv_driverwheelradius = w->inv_wheelradius ;
            skip_rbr(msfile) ;
            if(ip)printf("  Wagon %s has wheel of radius %f\n",w->name,w->wheelradius) ;
            break ;
          CASE("NumWheels")
            skip_lbr(msfile) ;
            w->numwheels  = itoken(msfile) ;
            skip_rbr(msfile) ;
            break ;
          CASE("Coupling")
            skip_lbr(msfile) ;
            w->ncoupling++   ;
            if(w->ncoupling > 2){
              printf(" ERROR  wagon %s has more than 2 couplings\n",w->name);
              exit(1) ;
            }
            for(;;){
              token = new_tmp_token(msfile);
              if(ip)printf("    Token 1 = :%s:\n",token) ;
              if(is_rbr(token)) break ;  // End of Coupling data
              SWITCH(token)
                CASE("Type")
                  skip_lbr(msfile) ;
                    w->couplingtype2 = ctoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("Spring")
                  skip_lbr(msfile) ;
                  for(;;){
                    token = new_tmp_token(msfile);
                    if(ip)printf("    Sprint Token 2 = :%s:\n",token) ;
                    if(is_rbr(token)) break ;  // End of Spring data
                    SWITCH(token)
                      CASE("Stiffness")
                        skip_lbr(msfile) ;
                        token = new_tmp_token(msfile) ;
                        w->stiffness21  = convert_unit(token,"N/m") ;
                        token = new_tmp_token(msfile) ;
                        w->stiffness22  = convert_unit(token,"N/m") ;
                        skip_rbr(msfile) ;
                        break ;
                      CASE("Break")
                        skip_lbr(msfile) ;
                        token = new_tmp_token(msfile) ;
                        w->break21  = convert_unit(token,"N") ;
                        token = new_tmp_token(msfile) ;
                        w->break22  = convert_unit(token,"N") ;
                        skip_rbr(msfile) ;
                        break ;
                      CASE("r0")
                        skip_lbr(msfile) ;
                        token = new_tmp_token(msfile) ;
                        w->r021  = convert_unit(token,"m") ;
                        token = new_tmp_token(msfile) ;
                        w->r022  = convert_unit(token,"m") ;
                        skip_rbr(msfile) ;
                        break ;
                      CASE("Comment")
                        skip_lbr(msfile) ;
                        skipto_rbr(msfile) ;
                        break ;
                      DEFAULT
                        printf(" Error token not recognised.  Token = %s\n",token);
                        exit(1) ;
                    END
                  }
                  break ;
                CASE("CouplingHasRigidConnection")
                  skip_lbr(msfile) ;
                  w->couplinghasrigidconnection2 = itoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                DEFAULT
                  printf(" Routine %s. Wagon %s. Token not recognised.  Token = %s\n",
                  my_name,w->name,token);
                  skip_lbr(msfile)   ;
                  skipto_rbr(msfile) ;

//                  exit(1) ;
              END
              if(1 == w->ncoupling){
                w->couplingtype = w->couplingtype2 ;
                w->stiffness1   = w->stiffness21 ;
                w->stiffness2   = w->stiffness22 ;
                w->break1       = w->break21 ;
                w->break2       = w->break22 ;
                w->r01          = w->r021 ;
                w->r02          = w->r022 ;
                w->couplinghasrigidconnection1 =  w->couplinghasrigidconnection2 ;
              }
            }
            break ;
          CASE("BrakeEquipmentType")
            skip_lbr(msfile) ;
            w->brakeequipmenttype = ctoken(msfile) ;
            if(ip)printf("    BrakeEquipmentType = :%s:\n",
                                               w->brakeequipmenttype) ;
            skip_rbr(msfile) ;
            break ;
          CASE("BrakeSystemType")
            skip_lbr(msfile) ;
            w->brakesystemtype = ctoken(msfile) ;
            if(ip)printf("    BrakeSystemType = :%s:\n",
                                               w->brakesystemtype) ;
            skip_rbr(msfile) ;
            break ;
          CASE("MaxBrakeForce")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->maxbrakeforce  = convert_unit(token,"N") ;
            if(ip)printf("    MaxBrakeForce = :%f:\n",
                                               w->maxbrakeforce) ;
            skip_rbr(msfile) ;
            break ;
          CASE("MaxHandbrakeForce")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->maxhandbrakeforce  = convert_unit(token,"N") ;
            if(ip)printf("    MaxHandbrakeForce = :%f:\n",
                                               w->maxhandbrakeforce) ;
            skip_rbr(msfile) ;
            break ;
          CASE("EmergencyResVolumeMultiplier")
            skip_lbr(msfile) ;
            w->emergencyresvolumemultiplier  = dtoken(msfile) ;
            if(ip)printf("    EmergencyResVolumeMultiplier = :%f:\n",
                                          w->emergencyresvolumemultiplier) ;
            skip_rbr(msfile) ;
            break ;
          CASE("TripleValveRatio")
            skip_lbr(msfile) ;
            w->triplevalveratio  = dtoken(msfile) ;
            if(ip)printf("    TripleValveRatio = :%f:\n",
                                          w->triplevalveratio) ;
            skip_rbr(msfile) ;
            break ;
          CASE("MaxReleaseRate")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->maxreleaserate  = convert_unit(token,"psi/s") ;
            if(ip)printf("    MaxReleaseRate = :%f:\n",
                                               w->maxreleaserate) ;
            skip_rbr(msfile) ;
            break ;
          CASE("MaxApplicationRate")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->maxapplicationrate  = convert_unit(token,"psi/s") ;
            if(ip)printf("    MaxApplicationRate = :%f:\n",
                                               w->maxapplicationrate) ;
            skip_rbr(msfile) ;
            break ;
          CASE("MaxAuxilaryChargingRate")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->maxauxilarychargingrate  = convert_unit(token,"psi/s") ;
            if(ip)printf("    MaxAuxilaryChargingRate = :%f:\n",
                                               w->maxauxilarychargingrate) ;
            skip_rbr(msfile) ;
            break ;
          CASE("EmergencyResChargingRate")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->emergencyreschargingrate  = convert_unit(token,"psi/s") ;
            if(ip)printf("    EmergencyResChargingRate = :%f:\n",
                                               w->emergencyreschargingrate) ;
            skip_rbr(msfile) ;
            break ;
          CASE("EmergencyResCapacity")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->emergencyrescapacity  = convert_unit(token,"ft^3") ;
            if(ip)printf("    EmergencyResCapacity = :%f:\n",
                                               w->emergencyrescapacity) ;
            skip_rbr(msfile) ;
            break ;
          CASE("BrakeCylinderPressureForMaxBrakeBrakeForce")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->brakecylinderpressureformaxbrakebrakeforce  = convert_unit(token,"psi") ;
            if(ip)printf("    BrakeCylinderPressureForMaxBrakeBrakeForce = :%f:\n",
                                w->brakecylinderpressureformaxbrakebrakeforce) ;
            skip_rbr(msfile) ;
            break ;
          CASE("BrakePipeVolume")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->brakepipevolume  = convert_unit(token,"ft^3") ;
            if(ip)printf("    BrakePipeVolume = :%f:\n",
                                               w->brakepipevolume) ;
            skip_rbr(msfile) ;
            break ;
          CASE("ORTSBrakeShoeFriction")
            skip_lbr(msfile) ;
            for(i=0;i<15;i++){
              w->ortsbrakefriction1[i] = dtoken(msfile);
              w->ortsbrakefriction2[i] = dtoken(msfile);
            }
            if(ip)printf("    ORTSBrakeShoeFriction read\n") ;
            skip_rbr(msfile) ;
            break ;
/*
 *  Friction
 */
          CASE("ORTSBearingType")
            skip_lbr(msfile) ;
            w->ortsbearingtype = ctoken(msfile) ;
            if(ip)printf("    ORTSBearingType = :%s:\n",
                                               w->ortsbearingtype) ;
            skip_rbr(msfile) ;
            break ;
          CASE("ORTSDavis_A")
            skip_lbr(msfile) ;
            w->ortsdavis_a  = dtoken(msfile) ;
            if(ip)printf("    ORTSDavis_A = :%f:\n",
                                          w->ortsdavis_a) ;
            skip_rbr(msfile) ;
            break ;
          CASE("ORTSDavis_B")
            skip_lbr(msfile) ;
            w->ortsdavis_b  = dtoken(msfile) ;
            if(ip)printf("    ORTSDavis_B = :%f:\n",
                                          w->ortsdavis_b) ;
            skip_rbr(msfile) ;
            break ;
          CASE("ORTSDavis_C")
            skip_lbr(msfile) ;
            w->ortsdavis_c  = dtoken(msfile) ;
            if(ip)printf("    ORTSDavis_C = :%f:\n",
                                          w->ortsdavis_c) ;
            skip_rbr(msfile) ;
            break ;
          CASE("ORTSWagonFrontalArea")
            skip_lbr(msfile) ;
            token = read_scaled_token(msfile) ;
            w->ortswagonfrontalarea = convert_unit(token,"ft^2") ;
            if(ip)printf("    ORTSWagonFrontalArea = :%f:\n",
                                               w->ortswagonfrontalarea) ;
            skip_rbr(msfile) ;
            break ;
          CASE("ORTSDavisDragConstant")
            skip_lbr(msfile) ;
            w->ortsdavisdragconstant = dtoken(msfile) ;
            if(ip)printf("    ORTSDavisDragConstant = :%f:\n",
                                          w->ortsdavisdragconstant) ;
            skip_rbr(msfile) ;
            break ;
          CASE("CentreOfGravity")
            skip_lbr(msfile) ;
            for(i=0;i<3;i++){
              token = new_tmp_token(msfile) ;
              w->centreofgravity[i] = convert_unit(token,"m") ;
              if(ip)printf("    CentreOfGravity = :%f:\n",
                                               w->centreofgravity[i]) ;
            }
            skip_rbr(msfile) ;
            break ;
          CASE("ORTSTrackGauge")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->ortstrackgauge= convert_unit(token,"m") ;
            if(ip)printf("    ORTSTrackGauge = :%f:\n",
                                              w->ortstrackgauge) ;
            token = new_tmp_token(msfile) ;
            if(is_rbr(token))break ;
            w->ortstrackgauge += convert_unit(token,"m") ;
            if(ip)printf("    ORTSTrackGauge = :%f:\n",
                                              w->ortstrackgauge) ;
            skip_rbr(msfile) ;
            break ;
          CASE("ORTSRigidWheelbase")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->ortsrigidwheelbase= convert_unit(token,"m") ;
            if(ip)printf("    ORTSRigidWheelbase = :%f:\n",
                                              w->ortsrigidwheelbase) ;
            token = new_tmp_token(msfile) ;
            if(is_rbr(token))break ;
            w->ortsrigidwheelbase += convert_unit(token,"m") ;
            if(ip)printf("    ORTSRigidWheelbase = :%f:\n",
                                              w->ortsrigidwheelbase) ;
            skip_rbr(msfile) ;
            break ;
          CASE("ORTSUnbalancedSuperelevation")
            skip_lbr(msfile) ;
            token = new_tmp_token(msfile) ;
            w->ortsunbalancedsuperelevation = convert_unit(token,"m") ;
            if(ip)printf("    ORTSUnbalancedSuperelevation = :%f:\n",
                                     w->ortsunbalancedsuperelevation) ;
            skip_rbr(msfile) ;
            break ;
          CASE("Inside")
            skip_lbr(msfile) ;
            for(;;){
              token = new_tmp_token(msfile);
              if(ip)printf("    Token 1 = :%s:\n",token) ;
              if(is_rbr(token)) break ;  // End of Coupling data
              SWITCH(token)
                CASE("PassengerCabinFile")
                  skip_lbr(msfile) ;
                  w->passengercabinfile = ctoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("PassengerCabinHeadPos")
                  skip_lbr(msfile) ;
                  for(i=0;i<3;i++){
                    w->passengercabinheadpos[i] = dtoken(msfile) ;
                    if(ip)printf("    PassengerCabinHeadPos = :%f:\n",
                                             w->passengercabinheadpos[i]) ;
                  }
                  skip_rbr(msfile) ;
                  break ;
                CASE("StartDirection")
                  skip_lbr(msfile) ;
                  for(i=0;i<3;i++){
                    w->startdirection[i] = dtoken(msfile) ;
                    if(ip)printf("    StartDirection = :%f:\n",
                                             w->startdirection[i]) ;
                  }
                  skip_rbr(msfile) ;
                  break ;
                CASE("RotationLimit")
                  skip_lbr(msfile) ;
                  for(i=0;i<3;i++){
                    w->rotationlimit[i] = dtoken(msfile) ;
                    if(ip)printf("    RotationLimit = :%f:\n",
                                             w->rotationlimit[i]) ;
                  }
                  skip_rbr(msfile) ;
                  break ;
                DEFAULT
                  printf(" Routine %s. Wagon %s. Token not recognised.  Token = %s\n",
                   my_name,w->name,token);
                  skip_lbr(msfile)   ;
                  skipto_rbr(msfile) ;
//                  exit(1) ;
              END
            }
            break ;
          CASE("Sound")
            skip_lbr(msfile) ;
            w->sound_file = ctoken(msfile) ;
            if(ip)printf("    Sound = :%s:\n",
                                              w->sound_file) ;
            skip_rbr(msfile) ;
            break ;
          CASE("Name")
            skip_lbr(msfile) ;
            w->full_name = ctoken(msfile) ;
            if(ip)printf("    Full Name = :%s:\n",
                                              w->full_name) ;
            skip_rbr(msfile) ;
            break ;




          DEFAULT
            printf(" Routine %s. Wagon %s. Token not recognised.  Token = %s\n",
                   my_name,w->name,token);
            skip_lbr(msfile)   ;
            skipto_rbr(msfile) ;
        END
      }
      free(cdir) ;
      if(ip)printf(" WWWWWWWWWWWWWWWWWWWWW\n") ;
      fflush(NULL) ;
//      check_wheel_radii(w) ;
      close_msfile(msfile) ;

      return 0 ;
}

/*
 *  File 1905-C144.eng contains a length which is split between lines
 *  I do not know how often this occurs but this routine is designed to
 *  deal with some such cases, returning a temporary string
 *  Possibilities are "( 52 ft^2 )" or ( 52 )
 */

char *read_scaled_token(MSfile *msfile){
  int  l, n ;
  char *token ;

      token = new_tmp_token(msfile) ;
      strncpy(string,token,256);

      l = strlen(string) ;
      n = strspn(string,"1234567890-.e");
//      printf(" read_scaled_token 1 : l = %i, n = %i, string = %s\n",l,n,string);
      if(l==n){
        token = new_tmp_token(msfile) ;
        if(is_rbr(token)){
          return_token(token,msfile) ;
        }else{
          strncat(string,token,256-l) ;
        }
      }
//  Remove any quotes
      if('"'==string[0]){
        token[0] = ' ' ;
        l = strlen(string)-1 ;
        if('"'==string[l])string[l] = '\0' ;
        for(l=0;l<strlen(string);l++){
          string[l]=string[l+1] ;
        }
      }
      return string ;
}

/*
 *  Routine to check wheel radii against the the corresponding wheel matrices
 */

int  check_wheel_radii(RawWagonNode *w){

ShapeNode  *shape  ;
Matrix4x3  *matrix ;
LodControl *lod_control ;
DistLevel *dist_level   ;
int        *hierarchy   ;

int        nmatrices    ;
int        n_hierarchy ;
//ShapeNode  *shape ;
//Matrix4x3  *matrix ;
//int        nmatrices ;
char       *name ;
char       *myname = "check_wheel_radii" ;

int    ip = 1 ;   // DEBUG
int    i, j ;
double radius ;

      printf("  ENTER %s\n",myname) ;
      fflush(NULL) ;
      printf("  name          = %s\n",w->name) ;
      fflush(NULL) ;

      shape = w->shape ;
      printf("  shape          = %p\n",(void *)shape) ;

      print_shape_file_data(shape) ;
      printf("  ENTER %s\n",myname) ;
      fflush(NULL) ;

      shape = w->shape ;
      printf("  shape          = %p\n",(void *)shape) ;

      nmatrices = shape->nmatrices ;
      printf("  nmatrices       = %i\n",nmatrices) ;

      matrix = shape->matrix ;
      printf("  matrix         = %p\n",(void *)matrix) ;

      printf("  n_lod_controls = %i\n",shape->n_lod_controls) ;
      lod_control = shape->lod_control      ;
      printf("  lod_control    = %p\n",(void *)lod_control) ;

      printf("  n_dist_levels  = %i\n",lod_control->n_dist_levels) ;
      dist_level  = lod_control->dist_level ;
      printf("  dist_level     = %p\n",(void *)dist_level) ;

      printf("  n_hierarchy    = %i\n",dist_level->n_hierarchy) ;
      n_hierarchy   = dist_level->n_hierarchy   ;
      hierarchy   = dist_level->hierarchy   ;
      printf("  hierarchy      = %p\n",(void *)hierarchy) ;

      printf("  ENTER %s %p\n",myname,(void *)w) ;
      fflush(NULL) ;
      printf("  ENTER %s %p\n",myname,(void *)shape) ;
      fflush(NULL) ;
      printf("  ENTER %s %p\n",myname,(void *)matrix) ;
      fflush(NULL) ;




      fflush(NULL) ;
      printf("  ENTER %s nmatrices = %i\n",myname,nmatrices) ;
      fflush(NULL) ;
      printf("  ENTER %s name = %s\n",myname,w->name) ;
      fflush(NULL) ;
//      shape    = w->shape ;matrix   = w->shape->matrix ;nmatrices = w->shape->nmatrices ;

      if(0 == nmatrices) return 0 ;
      for(i=0;i<nmatrices;i++){
         printf(" kk\n") ;
        name = strdup(matrix[i].name) ;
        printf("  Matrix %i, name = %s\n",i,name) ;
        if(strlen(name)<6) continue   ;  // Name to short for Wheel#
        for(j=0;j<strlen(name);j++){
          name[j] = toupper(name[j]) ;
        }
        printf("  Matrix %i, name = %s %i \n",i,name,strncmp(name,"WHEELS",6)) ;
        if(strncmp(name,"WHEELS",6))continue ;
        if(!isdigit(name[6]))continue  ;  // Not wheel followed by number
        if(6 == strlen(name)){           // Driving wheel
          radius = matrix[i].DY ;
          printf(" Routine %s, MM Matrix %i %s, radius %f, default %f\n",
                 myname, i, matrix[i].name, matrix[i].DY, w->driverwheelradius) ;

          w->driverwheelradius = radius ;
        }else{                           //  Normal wheel
          radius = matrix[i].DY ;
          printf(" Routine %s, NN Matrix %i %s, radius %f, default %f\n",
                 myname, i, matrix[i].name, matrix[i].DY, w->wheelradius) ;
          w->wheelradius = radius ;
          if(n_hierarchy>0){
          printf(" Routine %s, NN Matrix %i, hierarchy %p\n",
                 myname,i,(void *)hierarchy) ;
          printf(" Routine %s, NN Matrix %i, hierarchy[i] = %i\n",
                 myname,i,hierarchy[i]) ;
          }
          printf(" KK\n") ;
        }
        printf(" ll\n") ;
        free(name) ;
        printf(" LL\n") ;
      }

      return 0;
}
