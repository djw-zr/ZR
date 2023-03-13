/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  traveller2.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines structures used for travellers
 *   Based on Traveller.cs and other OpenRails files
 *
 *   Initially the routines need to be robust and accurate but as they are
 *   the core part of the dynamics code they eventually also need to be very
 *   efficient
 *
 *==============================================================================
 */
uint  find_mainline_section(TrkSector *tn, int pin_1, int pin_2) ;

/*
 *  Routine to initialise a traveller - given:
 *    a)  The index of a track node/track section
 *    b)  The vector index
 *    c)  The direction of the wagon relative to the train
 */

int  trv_initv(TravellerNode *t, int itrack, int ivector, int idirect){

int           ip = 0 ;  // Debug
TrkSector   *tn  ;
TrkVectorNode *vn  ;

char          my_name[] = "trv_initv" ;

      if(!t->wagon) ip = 0 ;  // Skip dummy wagons

      if(ip)printf("  Enter %s\n",my_name) ;
/*
 *   1.  Track Setion
 */
      if(itrack <1) itrack = 0 ;
      tn = &track_db.trk_sections_array[itrack-1] ;  //  Remember '-1'!
      t->tn = tn ;
      if(tn->type_of_node != VECTOR_SECTION){
        printf("  Error.  Node %i is not a vector section\n",itrack );
        printf("  Error.  ivector       = %i\n",ivector );
        printf("  Error.  itrack        = %i\n",itrack );
        printf("          wagon         = %s\n",t->wagon->name) ;
        printf("          train         = %s\n",t->wagon->train->name ) ;
        exit(0) ;
      }
/*
 *   Vector
 */
      if((uint)ivector >= tn->length_of_vector){
        printf("  Error.  Vector length too short\n");
        printf("  Error.  Vector length = %i\n", tn->length_of_vector);
        printf("  Error.  ivector       = %i\n",ivector );
        printf("  Error.  itrack        = %i\n",itrack );
        printf("          wagon         = %s\n",t->wagon->name) ;
        printf("          train         = %s\n",t->wagon->train->name) ;
        exit(0) ;
      }

      vn = &tn->vector[ivector] ;

      t->tn = tn ;
      t->vn = vn ;
      t->x        = 0.0 ;
      t->y        = 0.0 ;
      t->z        = 0.0 ;
      t->ang_deg  = 0.0 ;
      t->vect_position = 0.0 ;

      t->itrack  = itrack  ;
      t->ivector = ivector ;
      t->idirect = idirect ;

      if(ip)printf(" Routine %s, tn = %p, vn = %p\n",
                                      my_name,(void *)t->tn,(void *)t->vn) ;
      if(ip)printf("             length, radius, angle = %f %f %f\n",
                                           vn->length,vn->radius,vn->angle) ;
      l_disp1 = 1 ;
      return 0 ;
}

/*
 *  Routine to print the 3D location of the traveller origin
 *    May be used to check the smooth motion when progressing
 *    from one piece of track to the next.
 */

#if 0
int  trv_ploc(TravellerNode *tn){

float   x, y, z ;
float   v1[4], v2[4] ;
GLfloat modelview[16]  ;
char    my_name[] = "trv_ploc" ;


      glMatrixMode(GL_MODELVIEW) ;
      glPushMatrix() ;

      global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                       tn->itile_x, tn->itile_y,
                       tn->tile_x , tn->tile_y, tn->tile_z,
                       &x, &y, &z) ;
      a = degree*tn->trk_net_node->vector[tn->ivector].a_east_x   ;
      b = degree*tn->trk_net_node->vector[tn->ivector].a_height_y ;
      c = degree*tn->trk_net_node->vector[tn->ivector].a_north_z  ;

      glRotatef((GLfloat) 90.0, (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0 ) ;
      glScalef(scale,scale,-scale) ;

      glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z) ;

      glRotatef((GLfloat)b,(GLfloat)0.0,(GLfloat)1.0,(GLfloat)0.0) ; // Direction - Yaw
      glRotatef((GLfloat)a,(GLfloat)1.0,(GLfloat)0.0,(GLfloat)0.0) ; // Slope - Pitch
      glRotatef((GLfloat)c,(GLfloat)0.0,(GLfloat)0.0,(GLfloat)1.0) ; // Camber - Roll

      glTranslatef((GLfloat)(t->x),(GLfloat)(t->y),(GLfloat)(t->z)) ;
      glGetFloatv(GL_MODELVIEW_MATRIX, modelview)  ;
/*
 *  Matrix (i,j) = matrix( (i-1) + 4*(j-1))
 */
      v1[0] = 0.0; v1[1] = 0.0;  v1[2] = 0.0;  v1[3] = 1.0;
      for(i=0;i<4;i++){
        v2(i) = 0.0 ;
        for(j=0;j<4;j++)


      glPopMatrix() ;

      return 0 ;
}
#endif


/*
 *  Routine to move from the end of a track node section, via a junction,
 *  to the next track node section
 *
 *  Returns 0 : no error
 *          1 : end if track reached
 *          2 : Switch set against traveller (ghost travellers only)
 *  If a normal traveller attempts to join a junction from the 'wrong' branch
 *  the movement is allowed but the global flag "junction_error" is set.
 *  However in the case of pseudo travellers, ones without waggons (used to
 *  search for level crossings and other objects then the flag is not set.
 *
 */
int   trk_next(TravellerNode *t, int inext){

int             ip = 0           ;  //  Debug

uint            i                ;
int             j                ;
int             idirect = t->idirect ;
int             fromj_old,          //  True if new/old track section
                fromj_new        ;  //  increased away from the junction
TrkSector       *tn = t->tn, *tn2 ;
TrkVectorNode   *vn = tn->vector ;
char             my_name[] = "trk_next" ;

uint    my_sect   = tn->uid  ;  // Old section number
uint    n_in_pins = tn->type_of_pin[0] ;  // Number of in pins
uint    n_ot_pins = tn->type_of_pin[1] ;  // Number of out pins
uint    j_sect                         ;  // Junction section
uint    n_sect                         ;  // new section

      if(!t->wagon) ip = 0 ;  // Skip dummy wagons

      if(ip){
        printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n") ;
        printf("  ENTER %s.  idirect = %i, inext = %i\n",
                      my_name,idirect,inext);
      }

      if(n_in_pins != 1 || n_ot_pins != 1){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : Wrong number of old pins : n_in_pins, n_ot_pins ="
                                           " %i %i\n",  n_in_pins,n_ot_pins) ;
        printf("  uid              = %i",tn->uid) ;
        printf("  type_of_node     = %i",tn->type_of_node) ;
        printf("  length_of_vector = %i",tn->length_of_vector) ;
        exit(1) ;
      }
      if( (idirect != 1 && idirect !=0) || (inext !=1 && inext != -1)){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : idirect or inext out of range %i %i\n",
                                                              idirect,inext) ;
        exit(1) ;
      }

      if( inext == -1){
        j_sect = tn->pin_to_section[0] ;
        fromj_old = 1 ;                 //  Vector index and track distance
                                        //         increase away from junction
        tn = tn->trk_sector[0] ;
      }else{
        j_sect = tn->pin_to_section[1] ;
        fromj_old = 0 ;                 //   Values increase towards junction
        tn = tn->trk_sector[1] ;
      }

      if(ip){
        printf("  Moving to new section from node (uid): %i\n",tn->uid);
        printf("    Number of in and out pins %i %i\n",
                                  tn->type_of_pin[0],tn->type_of_pin[1]) ;
        printf("    Pinned sections:          %i %i %i\n",
          tn->pin_to_section[0],tn->pin_to_section[1],tn->pin_to_section[2]) ;
        printf("    fromj_old is:             %i\n",fromj_old) ;
      }

//  Moving

//      tn = &track_db.trk_sections_array[j_sect-1] ;      //  Remember '-1'!

//      printf("  Test tn = %p, tn2 = %p\n",(void *)tn, (void *)tn2) ;

      n_in_pins = tn->type_of_pin[0] ;
      n_ot_pins = tn->type_of_pin[1] ;
      if(n_in_pins+n_ot_pins>3){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : Wrong number of new pins : n_in_pins, n_ot_pins = %i %i\n",
                                                               n_in_pins,n_ot_pins) ;
        printf("  uid              = %i\n",tn->uid) ;
        printf("  type_of_node     = %i\n",tn->type_of_node) ;
        printf("  length_of_vector = %i\n",tn->length_of_vector) ;
        exit(1) ;
      }
      if(ip){
        printf("  Junction section:  %i %i\n",tn->uid,j_sect);
        printf("    Number of in and out pins %i %i\n", n_in_pins, n_ot_pins) ;
        printf("    Pinned sections:          %i %i %i\n",
          tn->pin_to_section[0],tn->pin_to_section[1],tn->pin_to_section[2]) ;
      }
/*
 * End of track error
 */
      if(n_in_pins+n_ot_pins < 2 || tn->type_of_node == END_SECTION){
          if(ip){
            printf("    Routine %s : Error :  Wagon entered end section = %i\n",
                                                                my_name, j_sect) ;
            printf("                          Type of node = %i %s\n",
                               tn->type_of_node, token_trackdb[tn->type_of_node]) ;
          }
          return 1 ;
      }
/*
 *  Search for old track section
 */
      j = -1 ;
      for(i=0 ; i<n_in_pins+n_ot_pins ; i++){
        if(my_sect == tn->pin_to_section[i]){j = i ; break ; } ;
      }
      if(j == -1){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : Unable to find old pin in new track section\n") ;
        printf("  ERROR : Old pin = %i, new pins = ",my_sect) ;
        for(i=0 ; i<n_in_pins+n_ot_pins ; i++){ printf(" %i",tn->pin_to_section[i]) ; }
        printf("\n");
        exit(1) ;
      }
      if(ip)printf("  Old section was pin %i, section was %i\n",
                                                 j,tn->pin_to_section[j]) ;
/*
 *  Find next section
 *  After calling routine "set_junction_path()", junctions should have
 *  one input pin and two output pins
 */

      if(j>0){
        if((uint)j!=tn->branch){
          if(!t->wagon) return 2 ;  //  Pseudo traveller
          printf("  Routine %s.  Trying to enter switch from wrong branch\n",my_name);
          printf("    Number of in and out pins %i %i\n", n_in_pins, n_ot_pins) ;
          printf("    Pinned sections:          %i %i %i\n",
             tn->pin_to_section[0],tn->pin_to_section[1],tn->pin_to_section[2]) ;
          printf("    Entering junction from pin %i, section %i\n",j,tn->pin_to_section[j]) ;
          junction_error = 1 ;  //  Wagon entered junction from wrong branch
        }
        n_sect = tn->pin_to_section[0] ;
        tn = tn->trk_sector[0] ;
      }else{
        n_sect = tn->pin_to_section[tn->branch] ;
        tn = tn->trk_sector[tn->branch] ;
      }
//      if(n_sect == -1){
      if(n_sect <= 0){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : New track section %i is not a VECTOR_SECTION"
                " or an END_SECTION\n", n_sect) ;
        exit(1) ;
      }
      if(ip)printf("  Routine %s, new section = %i\n", my_name, n_sect) ;

//      tn = &track_db.trk_sections_array[n_sect-1] ;
      if(tn->type_of_node != VECTOR_SECTION){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : New track section %i is not VECTOR_SECTION\n",
                                                                    n_sect) ;
        exit(1) ;
      }
      t->tn = tn ;
      t->itrack = n_sect ;

      if(ip)printf("  New section = %i %i\n",n_sect,tn->uid) ;

      if(tn->pin_to_section[0] == j_sect){
        fromj_new = 1 ;
        t->ivector  = 0 ;
        t->vn = &tn->vector[0] ;
        t->vect_position = 0.0 ;
        t->sect_distance = 0.0 ;
        t->x        = 0.0 ;
        t->y        = 0.0 ;
        t->z        = 0.0 ;
        t->ang_deg  = 0.0 ;
      }else if(tn->pin_to_section[1] == j_sect){
        fromj_new = 0 ;
        t->ivector  = tn->length_of_vector-1 ;
        t->vn = &tn->vector[t->ivector] ;
        t->vect_position = t->vn->length ;
        t->sect_distance = t->tn->length ;
        if(vn->angle == 0.0){
          t->x       = 0.0 ;
          t->y       = 0.0 ;
          t->z       = vn->length ;
          t->ang_deg = 0.0 ;
        }else{
          t->x       = vn->radius*(1.0-cos(vn->angle)) ;
          if(vn->angle < 0.0) t->x = -t->x               ;
          t->y       = 0.0                             ;
          t->z       = vn->radius*fabs(sin(vn->angle)) ;
          t->ang_deg = degree*vn->angle ;
        }
      }else{
         printf(" Section error\n") ;
         exit(1) ;
      }
      if(fromj_old == fromj_new) t->idirect = !t->idirect ;

      if(ip){
        printf("    ivector =   %i %i\n",t->ivector,tn->length_of_vector);
        printf("    idirect   = %i\n",t->idirect) ;
        printf("    iposition = %f %f\n",t->vect_position,t->vn->length);
        trv_position(t) ;
      }
      return 0 ;
}


int   trk_next_p(TravellerNode *t, int inext){

uint            i                ;
int             j                ;
int             ip = 0           ;  //  Debug
int             idirect = t->idirect ;
int             fromj_old,          //  True if new/old track section
                fromj_new        ;  //  increased away from the junction
TrkSector     *tn = t->tn      ;
TrkVectorNode   *vn = tn->vector ;
char             my_name[] = "trk_next_p" ;

uint    my_sect   = tn->uid  ;  // Old section number
uint    n_in_pins = tn->type_of_pin[0] ;  // Number of in pins
uint    n_ot_pins = tn->type_of_pin[1] ;  // Number of out pins
uint    j_sect                         ;  // Junction section
uint    n_sect                         ;  // new section

      if(!t->wagon) ip = 0 ;  // Skip dummy wagons

      if(ip){
        printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n") ;
        printf("  ENTER %s.  idirect = %i, inext = %i\n",
                      my_name,idirect,inext);
      }

      if(n_in_pins != 1 || n_ot_pins != 1){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : Wrong number of old pins : n_in_pins, n_ot_pins ="
                                           " %i %i\n",  n_in_pins,n_ot_pins) ;
        exit(1) ;
      }
      if( (idirect != 1 && idirect !=0) || (inext !=1 && inext != -1)){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : idirect or inext out of range %i %i\n",
                                                              idirect,inext) ;
        exit(1) ;
      }

      if( inext == -1){
        j_sect = tn->pin_to_section[0] ;
        fromj_old = 1 ;                 //  Vector index and track distance
                                        //         increase away from junction
        tn = tn->trk_sector[0] ;
      }else{
        j_sect = tn->pin_to_section[1] ;
        fromj_old = 0 ;                 //   Values increase towards junction
        tn = tn->trk_sector[1] ;
      }

      if(ip){
        printf("  Previous section node: %i\n",tn->uid);
        printf("    Number of in and out pins %i %i\n",
                                  tn->type_of_pin[0],tn->type_of_pin[1]) ;
        printf("    Pinned sections:          %i %i %i\n",
          tn->pin_to_section[0],tn->pin_to_section[1],tn->pin_to_section[2]) ;
        printf("    fromj_old is:             %i\n",fromj_old) ;
      }

//  Moving

//      tn = &track_db.trk_sections_array[j_sect-1] ;      //  Remember '-1'!
      n_in_pins = tn->type_of_pin[0] ;
      n_ot_pins = tn->type_of_pin[1] ;
      if(n_in_pins+n_ot_pins>3){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : Wrong number of new pins : n_in_pins, n_ot_pins = %i %i\n",
                                                               n_in_pins,n_ot_pins) ;
        printf("  uid              = %i",tn->uid) ;
        printf("  type_of_node     = %i",tn->type_of_node) ;
        printf("  length_of_vector = %i",tn->length_of_vector) ;
        exit(1) ;
      }
      if(ip){
        printf("  Junction section:  %i %i\n",tn->uid,j_sect);
        printf("    Number of in and out pins %i %i\n", n_in_pins, n_ot_pins) ;
        printf("    Pinned sections:          %i %i %i\n",
          tn->pin_to_section[0],tn->pin_to_section[1],tn->pin_to_section[2]) ;
      }
/*
 * End of track error
 */
      if(n_in_pins+n_ot_pins < 2 || tn->type_of_node == END_SECTION){
          if(ip){
            printf("    Routine %s : Error :  Wagon entered end section = %i\n",
                                                                my_name, j_sect) ;
            printf("                          Type of node = %i %s\n",
                               tn->type_of_node, token_trackdb[tn->type_of_node]) ;
          }
          return 1 ;
      }
/*
 *  Search for old track section
 */
      j = -1 ;
      for(i=0 ; i<n_in_pins+n_ot_pins ; i++){
        if(my_sect == tn->pin_to_section[i]){j = i ; break ; } ;
      }
      if(j == -1){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : Unable to find old pin in new track section\n") ;
        printf("  ERROR : Old pin = %i, new pins = ",my_sect) ;
        for(i=0 ; i<n_in_pins+n_ot_pins ; i++){ printf(" %i",tn->pin_to_section[i]) ; }
        printf("\n");
        exit(1) ;
      }
      if(ip)printf("    Old section was pin %i, section was %i\n",
                                                 j,tn->pin_to_section[j]) ;
/*
 *  Find next section
 *  After calling routine "set_junction_path()", junctions should have
 *  one input pin and two output pins
 */

      if(j>0){
        if((uint)j!=tn->branch){
          if(!t->wagon) return 2 ;  //  Pseudo traveller
          printf("  Routine %s.  Trying to enter switch from wrong branch\n",my_name);
          printf("    Number of in and out pins %i %i\n", n_in_pins, n_ot_pins) ;
          printf("    Pinned sections:          %i %i %i\n",
             tn->pin_to_section[0],tn->pin_to_section[1],tn->pin_to_section[2]) ;
          printf("    Entering junction from pin %i, section %i\n",j,tn->pin_to_section[j]) ;
          junction_error = 1 ;  //  Wagon entered junction from wrong branch
        }
        n_sect = tn->pin_to_section[0] ;
        tn = tn->trk_sector[0] ;
      }else{
        n_sect = tn->pin_to_section[tn->branch] ;
        tn = tn->trk_sector[tn->branch] ;
      }
//      if(n_sect == -1){
      if(n_sect <= 0){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : New track section %i is not a VECTOR_SECTION"
                " or an END_SECTION\n", n_sect) ;
        exit(1) ;
      }
      if(ip)printf("    Routine %s, new section = %i\n", my_name, n_sect) ;

//      tn = &track_db.trk_sections_array[n_sect-1] ;
      if(tn->type_of_node != VECTOR_SECTION){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : New track section %i is not VECTOR_SECTION\n",
                                                                    n_sect) ;
        exit(1) ;
      }
      t->tn = tn ;
      t->itrack = n_sect ;

      if(ip)printf("  New section = %i %i\n",n_sect,tn->uid) ;

      if(tn->pin_to_section[0] == j_sect){
        fromj_new = 1 ;
        t->ivector  = 0 ;
        t->vn = &tn->vector[0] ;
        t->vect_position = 0.0 ;
        t->sect_distance = 0.0 ;
        t->x        = 0.0 ;
        t->y        = 0.0 ;
        t->z        = 0.0 ;
        t->ang_deg  = 0.0 ;
      }else if(tn->pin_to_section[1] == j_sect){
        fromj_new = 0 ;
        t->ivector  = tn->length_of_vector-1 ;
        t->vn = &tn->vector[t->ivector] ;
        t->vect_position = t->vn->length ;
        t->sect_distance = t->tn->length ;
        if(vn->angle == 0.0){
          t->x       = 0.0 ;
          t->y       = 0.0 ;
          t->z       = vn->length ;
          t->ang_deg = 0.0 ;
        }else{
          t->x       = vn->radius*(1.0-cos(vn->angle)) ;
          if(vn->angle < 0.0) t->x = -t->x               ;
          t->y       = 0.0                             ;
          t->z       = vn->radius*fabs(sin(vn->angle)) ;
          t->ang_deg = degree*vn->angle ;
        }
      }else{
         printf(" Section error\n") ;
         exit(1) ;
      }
      if(fromj_old == fromj_new) t->idirect = !t->idirect ;

      if(ip){
        printf("    ivector =   %i %i\n",t->ivector,tn->length_of_vector);
        printf("    idirect   = %i\n",t->idirect) ;
        printf("    iposition = %f %f\n",t->vect_position,t->vn->length);
        trv_position(t) ;
      }
      return 0 ;
}
