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
int  init_rawenginenode(RawWagonNode *w)   ;
//int  check_wheel_radii(RawWagonNode *w) ;
char string[4096] ;

int read_raw_wagon_file(RawWagonNode *w){

int      i, l, cdir_len ;
int      ip = 0 ;  //Debug
float    temp   ;
MSfile   msfile0           ;
MSfile   *msfile = &msfile0 ;
char     my_name[] = "read_raw_wagon_file" ;
char     *string = NULL,
         *token = NULL ,
         *cdir  = NULL ,             //  Current wagon directory
         *cext  = NULL ,             //  File extension
         *no_type        = "No Type",
         *no_description = "No Description" ;
RawEngineNode *e ;
WagonIntake   *intake ;
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

      cdir = zr_full_parentdir(w->file) ;
      cdir_len = strlen(cdir) ;
      if(ip)printf(" DD cdir_len = %i, cdir = %s\n",cdir_len,cdir) ;
      cext = zr_extension(w->file) ;
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
//      ip = !strcmp(w->name,"US2Freightcar3") ;
//      ip = !strcmp(w->name,"Superliner") ;
//      ip = (!strcmp(w->name,"KIHA140") || !strcmp(w->name,"kiha140") ) ;
//      ip = !strcmp(w->name,"AcelaHHLCO") ;
//      ip = !strcmp(w->name,"NI4wHopUnb2LD") ;
//      ip = !strcmp(w->name,"1905-P708") ;     <- Has the extra engine data
      if(ip){
        printf("  File = %s\n",w->file);
        printf("  Name = %s\n",w->name) ;
      }

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
      w->shape     = NULL ;
      w->s_file    = NULL     ;
      w->fs_file   = NULL     ;   //  Freight shape file
      w->f_shape   = NULL     ;
      w->intake    = NULL     ;
      w->raw_engine = NULL    ;
      w->f_max_level = 0.0    ;
      w->f_min_level = 0.0    ;
      w->f_anim_flag = 0      ;
      w->brakeequipmenttype = NULL ;

/*
 * ***********************************************************************
 *  Loop over tokens
 * ***********************************************************************
 */
      for(;;){
        if(ip)printf(" AAAA\n") ;
        token = new_tmp_token(msfile);
        if(ip)printf(" AA Token = :%s:\n",token) ;
        if(is_rbr(token)) break ;  // End of Wagon data
        SWITCH(token)
          CASE("Comment")
          CASE("comment")
            skip_lbr(msfile) ;
            skippast_rbr(msfile) ;
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
            skippast_rbr(msfile) ;
            break ;
          CASE("Mass")
            skip_lbr(msfile) ;
            token = read_scaled_token(msfile) ;
            if(ip)printf("  Mass = %s\n",token) ;
            w->mass  = convert_unit(token,"kg") ;
            if(ip)printf("  Mass = %f\n",w->mass) ;
            skippast_rbr(msfile) ;
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
                      CASE("Damping")
                        skip_lbr(msfile) ;
                        token = new_tmp_token(msfile) ;
                        w->damping21  = convert_unit(token,"N/m/s") ;
                        token = new_tmp_token(msfile) ;
                        w->damping22  = convert_unit(token,"N/m/s") ;
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
//                      CASE("comment")
                        skip_lbr(msfile) ;
                        skippast_rbr(msfile) ;
                        break ;
                      DEFAULT
                         printf("     Routine %s. Wagon %s. Spring token not recognised.  Token = %s\n",
                                      my_name,w->name,token);

                        exit(1) ;
                    END
                  }
                  break ;
                CASE("CouplingHasRigidConnection")
                  skip_lbr(msfile) ;
                  w->couplinghasrigidconnection2 = itoken(msfile) ;
                  skip_rbr(msfile) ;
                  break ;
                CASE("comment")
                CASE("Velocity")
                CASE("CouplingUniqueType")
                  skip_lbr(msfile) ;
                  skippast_rbr(msfile) ;
                  break ;
                DEFAULT
                  printf("     Routine %s. Wagon %s. Coupling token not recognised.  Token = %s\n",
                  my_name,w->name,token);
                  skip_lbr(msfile)   ;
                  skippast_rbr(msfile) ;

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
            skip_lbr(msfile)        ;
            string = ctoken(msfile) ;         // malloc
            if(w->brakeequipmenttype){
              printf("     Routine: %s, wagon: %s\n",my_name, w->name) ;
              printf("       BrakeEquipmentType already set.\n") ;
              printf("         old version = %s\n",w->brakeequipmenttype) ;
              printf("         new version = %s\n",string) ;
              free(string) ; string = NULL ;  // malloc no longer needed
            }else{
              w->brakeequipmenttype = string ;
            }
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
                CASE("Sound")
                  skip_lbr(msfile)   ;
                  skippast_rbr(msfile) ;
                  break ;
                DEFAULT
                  printf("     Routine %s. Wagon %s. Inside token not recognised.  Token = %s\n",
                   my_name,w->name,token);
                  skip_lbr(msfile)   ;
                  skippast_rbr(msfile) ;
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

          CASE("Pantograph")
//            init_rawenginenode(w) ;
            w->has_pantographs = 1;
            skip_lbr(msfile) ;
            skippast_rbr(msfile) ;
            break ;
          CASE("Wiper")
//            init_rawenginenode(w) ;
            w->has_wipers = 1;
            skip_lbr(msfile) ;
            skippast_rbr(msfile) ;
            break ;
          CASE("Mirror")
//            init_rawenginenode(w) ;
            w->has_mirrors = 1;
            skip_lbr(msfile) ;
            skippast_rbr(msfile) ;
            break ;
          CASE("Door")
            w->has_doors = 1;
            skip_lbr(msfile) ;
            skippast_rbr(msfile) ;
            break ;
          CASE("IntakePoint")
            intake = (WagonIntake *)malloc(sizeof(WagonIntake)) ;
            intake->next = w->intake ;
            w->intake = intake ;
            skip_lbr(msfile) ;
            intake->position = dtoken(msfile) ;
            intake->radius   = dtoken(msfile) ;
            intake->name     = ctoken(msfile) ;
            intake->type     = 0 ;;
            if(!strcmp(intake->name,"FreightGrain"))    intake->type = 1 ;
            if(!strcmp(intake->name,"FreightCoal"))     intake->type = 2 ;
            if(!strcmp(intake->name,"FreightGravel"))   intake->type = 3 ;
            if(!strcmp(intake->name,"FreightSand"))     intake->type = 4 ;
            if(!strcmp(intake->name,"FuelWater"))       intake->type = 5 ;
            if(!strcmp(intake->name,"FuelCoal"))        intake->type = 6 ;
            if(!strcmp(intake->name,"FuelDiesel"))      intake->type = 7 ;
            if(!strcmp(intake->name,"FuelWood"))        intake->type = 8 ;
            if(!strcmp(intake->name,"FuelSand"))        intake->type = 9 ;
            if(!strcmp(intake->name,"FreightGeneral"))  intake->type = 10 ;
            if(!strcmp(intake->name,"FreightLivestock"))intake->type = 11 ;
            if(!strcmp(intake->name,"FreightFuel"))     intake->type = 12 ;
            if(!strcmp(intake->name,"FreightMilk"))     intake->type = 13 ;
            if(!strcmp(intake->name,"SpecialMail"))     intake->type = 14 ;
            skip_rbr(msfile) ;
            break ;
          CASE("Wheelset")
          CASE("Relaxation")
          CASE("Friction")
//          CASE("comment")
          CASE("Adheasion")
          CASE("ExtraParameters")
//  Unused MSTS Parameters
          CASE("Velocity")
          CASE("Buffers")
          CASE("DerailRailHeight")
          CASE("DerailRailForce")
          CASE("DerailBufferForce")
          CASE("AntiSlip")
          CASE("EmergencyBrakeResMaxPressure")
          CASE("Lights")
          CASE("NumberOfHandbrakeLeverSteps")
          CASE("InertiaTensor")
          CASE("QuickReleaseTriggerRate")
          CASE("QuickReleaseMagnification")
          CASE("QuickApplicationTriggerRate")
          CASE("QuickApplicationMagnification")
          CASE("EmergencyBrakeTriggerRate")
          CASE("TrainPipeLeakRate")
          CASE("AuxilaryLeakRate")
          CASE("NumberOfHandbrakeLeverSteps")
          CASE("MaxTilt")
          CASE("BrakeDistributorFullServicePressure")
          CASE("BrakeDistributorMaxAuxilaryResPressure")
          CASE("BrakeDistributorEmergencyResPressure")
          CASE("BrakeDistributorReleaseRate")
          CASE("BrakeDistributorApplicationRate")
          CASE("BrakeDistributorTrainPipeControlRatio")
          CASE("BrakeDistributorNormalFullReleasePressure")
          CASE("Sound")
          CASE("Thumbnail")
          CASE("RetainingValveSlowReleaseRate")
          CASE("BrakeCylinderPressureForMaxBrakeBrakeForce")
          CASE("BrakeAirPressureGainInResForOnePoundFromPipe")
          CASE("Centre")
          CASE("Id")
          CASE("#Size")
          CASE("PassengerCapacity")
            skip_lbr(msfile) ;
            skippast_rbr(msfile) ;
            break ;

          DEFAULT
            printf("     Routine %s. Wagon %s. Token not recognised.  Token = %s\n",
                   my_name,w->name,token);
            skip_lbr(msfile)   ;
            skippast_rbr(msfile) ;
        END
        if(ip)printf(" ZZZZ\n") ;

      }
/*
 *  If eng file read engine section
 */
      if(!strcmp(cext,"eng")){
/*
 * Check for 'Engine' token
 */
        for(;;){
          token = new_tmp_token(msfile) ;  // Temporaty token !!
          if(!strcmp(token,"Comment")){
            skip_lbr(msfile)   ;
            skippast_rbr(msfile) ;
          }else if(!strcmp(token,"Engine")) {
            skip_lbr(msfile) ;           // skip left bracket
            break ;
          }else{
            printf("     Routine %s. Wagon %s. Token '%s' not recognised while "
                   " searching for Engine section.  \n",my_name,w->name,token) ;
            skip_lbr(msfile)   ;
            skippast_rbr(msfile) ;
          }
        }
/*
 *  Generate new RawWagonNode structure
 */
        w->raw_engine = e = (RawEngineNode *)malloc(sizeof(RawEngineNode)) ;
        e->type        = NULL ;
        e->name        = NULL ;
        e->description = NULL ;
        e->sound       = NULL ;
        e->num_wheels = 0     ;
        for(i=0;i<3;i++)e->head_out[i] = 0.0 ;
/*
 *  Read engine name
 */
        e->name = ctoken(msfile) ;
        if(ip)printf("  Engine Name = %s\n",e->name) ;
/*
 * Sometimes the engine name is missing.  The wagon name should be the same.
 */
        if(!strcmp(e->name,"Comment")){
          return_token(e->name,msfile) ;
          free(e->name) ;
          e->name = strdup(w->name) ;
        }
/*
 * ***********************************************************************
 *  Loop over tokens
 * ***********************************************************************
 */
        for(;;){
          if(ip)printf(" EEEE\n") ;
          token = new_tmp_token(msfile);
          if(ip)printf(" EE Token = :%s:\n",token) ;
          if(is_rbr(token)) break ;  // End of Engine data
          SWITCH(token)
            CASE("Comment")
            CASE("comment")
              skip_lbr(msfile) ;
              skippast_rbr(msfile) ;
              break ;
            CASE("Type")
              skip_lbr(msfile) ;
              e->type = ctoken(msfile) ;
              if(ip)printf("    Type = :%s:\n",e->type) ;
              skip_rbr(msfile) ;
              break ;
            CASE("Name")
              skip_lbr(msfile) ;
              e->long_name = ctoken(msfile) ;
              if(ip)printf("    Long name = :%s:\n",e->name) ;
              skip_rbr(msfile) ;
              break ;
            CASE("Description")
              skip_lbr(msfile) ;
              e->description = ctoken(msfile) ;
              if(ip)printf("    Description = :%s:\n",e->description) ;
              skippast_rbr(msfile) ;
              if(ip)printf("    End of Description\n") ;
              break ;
            CASE("WheelRadius")
              skip_lbr(msfile) ;
              token = new_tmp_token(msfile) ;
              e->wheel_radius  = convert_unit(token,"m") ;
              if(e->wheel_radius < 0.25)e->wheel_radius = 0.5 ; // Error?
              w->driverwheelradius = e->wheel_radius ;
              w->inv_driverwheelradius = 1.0/e->wheel_radius ;
              if(ip)printf("  Engine %s has wheel of radius %f\n",
                                                e->name,e->wheel_radius) ;
              skip_rbr(msfile) ;
              break ;
            CASE("Sound")
              skip_lbr(msfile) ;
              e->sound = ctoken(msfile) ;
              if(ip)printf("    Sound = :%s:\n",e->sound) ;
              skip_rbr(msfile) ;
              break ;
            CASE("HeadOut")
              skip_lbr(msfile) ;
              for(i=0;i<3;i++)e->head_out[i] = dtoken(msfile) ;
              if(ip)printf("    Head Out = :%f %f %f:\n",
                              e->head_out[0],e->head_out[1],e->head_out[2]) ;
              skip_rbr(msfile) ;
              break ;
            CASE("Effects")
            CASE("Wagon")
            CASE("MaxPower")
            CASE("MaxForce")
            CASE("RunUpTimeToMaxForce")
            CASE("MaxContinuousForce")
            CASE("MaxVelocity")
            CASE("MaxCurrent")
            CASE("MaxTemperature")
            CASE("MaxOilPressure")
            CASE("MaxSandingTime")

            CASE("ORTSSteamLocomotiveType")
            CASE("ORTSSteamBoilerType")
            CASE("ORTSDriveWheelWeight")

            CASE("Sanding")
            CASE("NumWheels")
            CASE("IsTenderRequired")
            CASE("CabView")

            CASE("BoilerLength")
            CASE("BoilerEffectivity")
            CASE("DraftingEffect")
            CASE("BoilerResponsiveness")
            CASE("CoalBurnage")
            CASE("MaxBoilerOutput")
            CASE("CylinderVolume")
            CASE("CylinderEffectivity")
            CASE("InjectorSizes")
            CASE("ExhaustLimit")
            CASE("PrimingFactor")
            CASE("BlastExponent")
            CASE("MaxFireMass")
            CASE("IdealFireMass")
            CASE("MaxWaterMass")
            CASE("SafetyValvesSteamUsage")
            CASE("SmokeCombustion")
            CASE("InjectorTypes")
            CASE("SuperHeater")
            CASE("BasicSteamUsage")
            CASE("BasicCoalUsage")
            CASE("InjectorLimits1")
            CASE("InjectorLimits2")
            CASE("CylinderCocksPowerEfficiency")
            CASE("SteamSmallestCutoff")
            CASE("NumberOfDamperSteps")
            CASE("RegulatorValveType")
            CASE("RegulatorSecondValveStartPosition")
            CASE("RegulatorPilotValveExponent")
            CASE("RegulatorSecondValveExponent")
            CASE("RegulatorPilotValveFullOpenning")
            CASE("RegulatorMainValveInitialOpenning")
            CASE("SteamBlowerEffectExponent")
            CASE("SteamBlowerEfficiencyExponent")
            CASE("SteamBlowerMaxSteamUsageRate")
            CASE("SteamBlowerMaxProportionOfBlastEffect")

            CASE("AirBrakesAirCompressorPowerRating")
            CASE("AirBrakesMainMinResAirPressure")
            CASE("AirBrakesAirCompressorWattage")
            CASE("AirBrakesAirUsedPerPoundsOfBrakePipePressure")
            CASE("AirBrakesHasLowPressureTest")
            CASE("AirBrakesIsCompressorElectricOrMechanical")
            CASE("AirBrakesMainMaxAirPressure")
            CASE("AirBrakesCompressorRestartPressure")
            CASE("AirBrakesMainResVolume")
            CASE("AirBrakesSteamUsageRate")

            CASE("EngineBrakesControllerDirectControlExponent")
            CASE("EngineBrakesControllerHasProportionalBrake")
            CASE("EngineBrakesProportionalBrakeLag")
            CASE("EngineBrakesControllerMinSystemPressure")
            CASE("TrainBrakesControllerMinSystemPressure")
            CASE("EngineBrakesControllerEmergencyBrakeTimePenalty")
            CASE("TrainBrakesControllerEmergencyBrakeTimePenalty")
            CASE("TrainBrakesControllerMaxPressureDropInNormalApplication")

            CASE("DynamicBrakesMinUsableSpeed")
            CASE("DynamicBrakesMaximumEffectiveSpeed")
            CASE("DynamicBrakesMaximumForce")
            CASE("DynamicBrakesResistorCurrentLimit")
            CASE("DynamicBrakesCutInSpeed")
            CASE("DynamicBrakesMaxAirBrakePressure")
            CASE("DynamicBrakesFadingSpeed")
            CASE("DynamicBrakesDelayTimeBeforeEngaging")
            CASE("DynamicBrakesMaximumSpeedForFadeOut")
            CASE("DynamicBrakesEffectAtMaximumFadeOut")
            CASE("DynamicBrakesHigherSpeedCurveExponent")
            CASE("DynamicBrakesLowerSpeedCurveExponent")
            CASE("DynamicBrakesNumberOfControllerNotches")
            CASE("DynamicBrakeHasAutoBailOff")

            CASE("VacuumBrakesSmallEjectorPowerRating")
            CASE("VacuumBrakesMinBoilerPressureMaxVacuum")
            CASE("VacuumBrakesSmallEjectorUsageRate")
            CASE("VacuumBrakesLargeEjectorUsageRate")
            CASE("VacuumBrakesHasVacuumPump")


            CASE("BrakesEngineBrakeType")
            CASE("BrakesTrainBrakeType")
            CASE("BrakesEngineControllers")

            CASE("CutoffMaxReverse")
            CASE("CutoffMaxForward")

            CASE("DoesBrakeCutPower")
            CASE("DoesHornTriggerBell")
            CASE("BrakeCutsPowerAtBrakeCylinderPressure")

            CASE("EmergencyStopMonitor")

            CASE("DieselEngineType")

            CASE("MaxDieselLevel")
            CASE("DieselUsedPerHourAtMaxPower")
            CASE("DieselUsedPerHourAtIdle")
            CASE("DieselSmokeEffectInitialMagnitude")
            CASE("DieselSmokeEffectMaxMagnitude")
            CASE("DieselSmokeEffectInitialSmokeRate")
            CASE("DieselSmokeEffectMaxSmokeRate")
            CASE("DieselEngineIdleRPM")
            CASE("DieselEngineStallRPM")
            CASE("DieselEngineMaxRPM")
            CASE("DieselEngineSpeedOfMaxTractiveEffort")
            CASE("DieselEngineMaxRPMChangeRate")
            CASE("AWSMonitor")
            CASE("VigilanceMonitor")
            CASE("OverspeedMonitor")
            CASE("EngineOperatingProcedures")

            CASE("GearBoxNumberOfGears")
            CASE("GearBoxDirectDriveGear")
            CASE("GearBoxMaxSpeedForGears")
            CASE("GearBoxMaxTractiveForceForGears")
            CASE("GearBoxOverspeedPercentageForFailure")
            CASE("GearBoxBackLoadForce")
            CASE("GearBoxCoastingForce")
            CASE("GearBoxUpGearProportion")
            CASE("GearBoxDownGearProportion")
            CASE("GearBoxOperation")
            CASE("GearBoxEngineBraking")

            CASE("MaxBoilerPressure")
            CASE("BoilerVolume")
            CASE("ORTSEvaporationArea")
            CASE("ORTSBoilerEfficiency")
            CASE("ORTSBoilerEvaporationRate")
            CASE("ORTSMaxIndicatedHorsepower")
            CASE("NumCylinders")
            CASE("CylinderStroke")
            CASE("CylinderDiameter")
            CASE("ORTSCylinderPortOpening")
            CASE("ORTSCylinderInitialPressureDrop")
            CASE("ORTSCylinderEfficiencyRate")
            CASE("ORTSGrateArea")
            CASE("ORTSFuelCalorific")
            CASE("SteamFiremanMaxPossibleFiringRate")
            CASE("ShovelCoalMass")
            CASE("ORTSBurnRate")
            CASE("MaxTenderCoalMass")
            CASE("MaxTenderWaterMass")
            CASE("ORTSMainResChargingRate")
            CASE("ORTSBrakePipeChargingRate")
            CASE("TrainPipeLeakRate")
            CASE("TrainBrakesControllerMaxSystemPressure")
            CASE("TrainBrakesControllerMaxReleaseRate")
            CASE("TrainBrakesControllerMaxQuickReleaseRate")
            CASE("TrainBrakesControllerMaxApplicationRate")
            CASE("TrainBrakesControllerEmergencyApplicationRate")
            CASE("TrainBrakesControllerFullServicePressureDrop")
            CASE("TrainBrakesControllerMinPressureReduction")
            CASE("EngineBrakesControllerMaxSystemPressure")
            CASE("EngineBrakesControllerMaxReleaseRate")
            CASE("EngineBrakesControllerMaxQuickReleaseRate")
            CASE("EngineBrakesControllerMaxApplicationRate")
            CASE("EngineBrakesControllerEmergencyApplicationRate")
            CASE("EngineBrakesControllerFullServicePressureDrop")
            CASE("EngineBrakesControllerMinPressureReduction")
            CASE("EngineControllers")
            CASE("EngineVariables")

            CASE("SteamWaterScoopMinPickupSpeed")
            CASE("SteamWaterScoopMaxPickupSpeed")
            CASE("SteamWaterScoopResistance")
            CASE("SteamWaterScoopMaxPickupRate")

            CASE("SteamSmokeUnitsPerPoundOfFuel")
            CASE("SteamMaxSmokeUnitsReleaseRate")
            CASE("MaxSteamHeatingPressure")
            CASE("SafetyValvePressureDifference")
            CASE("SteamGaugeGlassHeight")
            CASE("SteamCylinderCocksOperation")
              skip_lbr(msfile) ;
              skippast_rbr(msfile) ;
              break ;
            CASE("(")
//              printf("  Skip past (\n");
              skippast_rbr(msfile) ;
//              printf("  Skipped past (\n");
              break ;
            DEFAULT
              printf("     Routine %s. Engine %s. Token not recognised.  Token = %s\n",
                    my_name,w->name,token);
              skip_lbr(msfile)   ;
              skippast_rbr(msfile) ;
          END
          if(ip)printf(" EEEE ZZZZ\n") ;
        }
      }
/*
 *  Close directory and file
 */
      free(cdir) ;
      free(cext) ;
      if(ip)printf(" WWWWWWWWWWWWWWWWWWWWW\n") ;
      fflush(NULL) ;
//      check_wheel_radii(w) ;
      close_msfile(msfile) ;
      if(ip)printf(" Exit routine %s\n",my_name) ;

      return 0 ;
}

/*
 *  In a number of the wagon files, tokens containing measurements are often
 *  in a form of free format.
 *
 *  Examples are file Trains/Trainset/AU_NSW_1905-Locos/1905-C144.eng, part
 *  of au_great_zig_zag contains Size on two lines:
 *    Size ( 2.71577m 7.4
 *      7m )
 *
 *  MSTS file TRAINS/TRAINSET/US2FREIGHTCAR3/US2Freightcar3.s contains
 *    Mass ( "98.163t #24.186t empty, 98.163t full" )
 *
 *  I do not know how often this occurs but this routine is designed to
 *  deal with some such cases, returning a temporary string
 *  Possibilities are "( 52 ft^2 )" or ( 52 )
 *
 *  MSTS file TRAINS/TRAINSET/US2FREIGHTCAR3/US2Freightcar3.s contains
 *    Mass ( "98.163t #24.186t empty, 98.163t full" )
 */

char *read_scaled_token(MSfile *msfile){
  uint l, n ;
  char *token ;

      token = new_tmp_token(msfile) ;
      strncpy(string,token,256);

      l = strlen(string) ;                 // length of string
      n = strspn(string,"1234567890-.e");
//      printf(" read_scaled_token 1 : l = %i, n = %i, string = %s\n",l,n,string);
      if(l==n){
        token = new_tmp_token(msfile) ;
        l = strlen(token) ;
        n = strspn(token,"1234567890-.e") ;
        if(l==n ||is_rbr(token)){
          return_token(token,msfile) ;
        }else if(token[0]!='#'){
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
DistLevel  *dist_level  ;
int        *hierarchy   ;   ///  Can be -1

uint        nmatrices    ;
uint        n_hierarchy ;
char       *name ;
char       *myname = "check_wheel_radii" ;

uint    ip = 0 ;   // DEBUG
uint    i, j ;
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
      n_hierarchy  = dist_level->n_hierarchy   ;
      hierarchy    = dist_level->hierarchy   ;
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

#if 0
int  init_rawenginenode(RawWagonNode *w){

      if(!w->raw_engine)w->raw_engine = (RawEngineNode *)malloc(sizeof(RawEngineNode)) ;
      w->raw_engine->has_wipers      = 0 ;
      w->raw_engine->has_mirrors     = 0 ;
      w->raw_engine->has_pantographs = 0 ;
      return 0;
}
#endif

