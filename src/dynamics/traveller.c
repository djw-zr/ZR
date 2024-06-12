/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  traveller.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines structures used for travellers
 *   Based on Traveller.cs and other OpenRails files
 *
 *   Initially the routines need tobe robust and accurate but as they are
 *   the core part of the dynamics code they eventually also need to be very
 *   efficient
 *
 *==============================================================================
 */

//int  trv_position(TravellerNode *t) ;
int  check_matrices(TravellerNode *t) ;

/*
 *  Routine to move traveller a fixed distance
 */

int   trv_move(TravellerNode *t, double distance){

  int           ip = 0 ;
  int           iret ;
  TrkVectorNode *vn ;
  double        d = distance ;
  double        dn ;
  char          *my_name = "trv_move" ;

//      ip =  t->wagon && i_zrt ;     //  Wagon exists and toggle (Alt-9) set

      if(ip)printf("\n  Enter routine %s.  Name = %s\n",my_name,
                               (t->wagon) ? t->wagon->train->name : "(null)") ;

      if(ip){
        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n") ;
        printf("  ENTER %s : idirect = %i,  distance = %f\n",
                                                 my_name,t->idirect,distance) ;
      }

      for(;;){
        vn = t->vn ;
/*
 *  Case where traveller 'forward' corresponds to positive track direction
 */
        if(t->idirect){
          if(ip)printf("  Enter %s : move 'down' track distance %f from position %f\n",
                 my_name,d,t->vect_position ) ;
          dn = t->vect_position + d ;
          if(dn >= 0.0 && dn < vn->length){
            t->vect_position = dn ;
            if(ip)printf("  Routine %s :: simple return  :: position = %f / %f\n",
                                my_name, t->vect_position, vn->length) ;
            iret = trv_position(t) ;
            if(ip){
              printf("  EXIT DOWN TRACK  %s,  iret = %i\n",my_name, iret) ;
              printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n") ;
            }
            return iret ;
          }
          if(ip){
          printf("  Routine %s :: about to call trv_next or trv_prev\n",my_name) ;
          printf("    Length of section  =  %f\n",vn->length)  ;
          printf("    Position           =  %f\n",t->vect_position) ;
          printf("    Distance to move   =  %f\n",d)           ;
          }
          if(dn >= vn->length){
            d = dn - vn->length  ;
            if(ip)printf("    Distance in next section = %f\n",d) ;
            iret = trv_next(t) ;            //  move to next section
            if(ip)printf("    Position = %f\n",t->vect_position) ;
          }else{
            d = dn ;
            if(ip)printf("    Distance in prev section = %f\n",d) ;
            if(ip)printf("    call trv_prev()\n") ;
            iret = trv_prev(t) ;
            if(ip)printf("  %s.  Position = %f\n",my_name, t->vect_position) ;
          }
/*
 *  Case where traveller 'forward' corresponds to negative track direction
 */
        }else{
          if(ip)printf("  Enter %s : move 'up' track distance %f\n",my_name,d) ;
          dn = t->vect_position - d ;
          if(dn >= 0.0 && dn < vn->length){
            t->vect_position = dn ;
            if(ip)printf("  Routine %s :: simple return  :: position = %f / %f\n",
                                my_name,t->vect_position, vn->length) ;
            iret = trv_position(t) ;
            if(ip){
              printf("  EXIT UP TRACK  %s,  iret = %i\n",my_name, iret) ;
              printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n") ;
            }
            return iret ;
          }
          if(ip){
          printf("  Routine %s :: about to call trv_next or trv_prev\n",my_name) ;
          printf("    Distance to move   =  %f\n",d)           ;
          printf("    Length of section  =  %f\n",vn->length)  ;
          printf("    Position           =  %f\n",t->vect_position) ;
          }
          if(dn < 0.0){
            d = -dn ;
            if(ip)printf("    Distance in prev section = %f\n",d) ;
            iret = trv_prev(t) ;             // move to previous section
          }else{
            d = vn->length - dn ;
            if(ip)printf("    Distance in next section = %f\n",d) ;
            iret = trv_next(t) ;            //  move to next section
          }
        }
        if(iret) {
          if(ip){
            printf("  EXIT AFTER CRASH  %s,  iret = %i\n",my_name, iret) ;
            printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n") ;
          }
          return iret ;
        }
      }
      exit(1) ;  //  Should never be reached!
}

/*
 *  Routine to move traveler to next vector
 */

int   trv_next(TravellerNode *t){

int           ip   = 0 ;  //  Debug
int           iret = 0 ;  //  Return code
TravellerNode tc       ;  //  Copy
TrkSector     *tn = t->tn      ;
TrkVectorNode *vn = tn->vector ;
char          my_name[] = "trv_next" ;

      if(!t->wagon) ip = 0 ;
//      ip =  t->wagon && i_zrt ;     //  Wagon exists and toggle (Alt-9) set

      if(ip){
      printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n") ;
      printf("  ENTER %s\n",my_name);
      }
/*
 *   Move to next VectorNode
 */
      t->ivector = t->ivector + 1 ;
      if(ip)printf("        %s : next ivector = %i/%i\n",
                      my_name, t->ivector, tn->length_of_vector) ;
/*
 *  If new node is within group of vectors
 */
      if(t->ivector < tn->length_of_vector){
        vn = &vn[t->ivector] ;

        t->vn       = vn  ;
        t->vect_position = 0.0 ;
        t->x        = 0.0 ;
        t->y        = 0.0 ;
        t->z        = 0.0 ;
        t->ang_deg  = 0.0 ;
        if(ip){
        printf("        %s : new ivector\n",my_name) ;
        printf("        %s : ivector = %i, position = %f,  x, y, z = "
               " %f %f %f : a = %f\n",  my_name, t->ivector, t->vect_position,
                                        t->x, t->y, t->z, t->ang_deg);
        }
/*
 *  End of vector nodes ...
 *    ... search for next node
 *    ... temporary fix - change direction
 */
      }else{
        if(ip){
        printf("   MOVE to next section node  +++++++++++++++++++\n");
        printf("     Call trk_next            +++++++++++++++++++\n");
        }
#if 1
        tc   = *t ;
        iret = trk_next(t,1) ;
        if(iret){
           *t = tc ;
           t->ivector = t->ivector - 1 ;  // Reset vector
        }
#else
        if(-1 == trk_next(t,1)){
          t->wagon->train->speed = -t->wagon->train->speed ;
//          t->idirect = !t->idirect ;
/*
          if(t->ivector<0){
            t->ivector = 0 ;
          }else{
            t->ivector = t->ivector - 1 ;
          }
 */
        }
        if(ip)printf("   SWAP Direction      ++++++++++++++++++++++++++\n");
#endif
      }
      if(ip)trv_print(t) ;
      if(ip)printf("  EXIT %s\n",my_name);
      return iret ;
}

int   trv_prev(TravellerNode *t){

int           ip = 0   ;  // DEBUG
int           iret = 0 ;  // Return code
int           ivector = t->ivector ;
TravellerNode tc               ;  //  Copy of travelelr node
TrkSector   *tn = t->tn      ;
TrkVectorNode *vn = tn->vector ;
char          my_name[] = "trv_prev" ;

      if(!t->wagon) ip = 0 ;  // Skip dummy wagons
//      ip =  t->wagon && i_zrt ;     //  Wagon exists and toggle (Alt-9) set

      if(ip){
        printf("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n") ;
        printf("  ENTER %s\n",my_name);
      }
/*
 *  If new node is within group of vectors
 */
      ivector = ivector - 1 ;
      if(ip)printf("        %s : next ivector = %i/%i\n",
                      my_name, ivector, tn->length_of_vector) ;
/*
 *  If new node is within vector limits - reset traveller
 */
      if(ivector >= 0){
        t->ivector = ivector         ;
        vn         = &vn[t->ivector] ;

        t->vn       = vn          ;
        t->vect_position = vn->length  ;

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
        if(ip){
        printf("        %s : new ivector\n",my_name) ;
        printf("        %s : ivector = %i, position = %f, x, y, z = %f %f %f : a = %f\n",
                        my_name, t->ivector, t->vect_position, t->x, t->y, t->z, t->ang_deg);
        }
/*
 *  End of vector nodes ...
 *    ... search for next node
 *    ... temporary fix - change direction
 */
      }else{
        if(ip)printf("   MOVE to next section node  +++++++++++++++++++\n");
        if(ip)printf("     Call trk_next(t,-1)      +++++++++++++++++++\n");
#if 1
        tc = *t ;
        iret = trk_next(t,-1) ;
        if(iret){
          *t = tc ;
          t->ivector = ivector + 1 ;  // Reset vector
        }
#else
        if(-1 == trk_next(t,-1)){
          if(ip)printf("   END of vector nodes ++++++++++++++++++++++++++\n");
          t->wagon->train->speed = -t->wagon->train->speed ;

//          t->idirect = !t->idirect ;
/*          if(t->ivector<0){
            t->ivector = 0 ;
          }else{
            t->ivector = t->ivector - 1 ;
          }
          printf("   SWAP Direction      ++++++++++++++++++++++++++\n");
 */
        }
#endif
      }
      if(ip)trv_print(t) ;
      if(ip){
        printf("  EXIT  %s\n",my_name);
        printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n") ;
      }
      return iret ;
}


int  init_trav(TravellerNode *tn){

      tn->wagon       = NULL ;
      tn->shape       = NULL ;
      tn->tn          = NULL ;
      tn->vn          = NULL ;

      tn->x           = 0.0 ;
      tn->y           = 0.0 ;
      tn->z           = 0.0 ;
      tn->ang_deg     = 0.0 ;
      tn->vect_position = 0.0 ;
      tn->sect_distance = 0.0 ;

      tn->itrack      = 0   ;
      tn->ivector     = 0   ;
      tn->idirect     = 1   ;

      return 0;
}

int   trv_print(TravellerNode *t){

int           ip = 0  ;
TrkSector   *tn = t->tn ;
TrkVectorNode *vn = t->vn ;
char  my_name[]="trv_print" ;

      printf("    ENTER %s\n",my_name) ;

      printf("      x,   y,   z,  ang_deg    = %f %f %f : %f\n",t->x,t->y,t->z,t->ang_deg) ;

      printf("      section         = %i\n",tn->uid) ;
      printf("      sect length     = %f\n",tn->length)  ;
      printf("      sect_distance   = %f\n",t->sect_distance) ;
      printf("      ivector         = %i\n",t->ivector)       ;
      printf("      vect_position   = %f\n",t->vect_position) ;
      printf("      vect length     = %f\n",vn->length) ;
      printf("      vect distance0  = %f\n",vn->distance0) ;
      printf("      idirect         = %i\n",t->idirect)  ;
      if(t->shape != NULL){
         printf("      shape        = %s\n",t->shape->name) ;
         printf("      needed       = %i\n",t->shape->needed) ;
         printf("      loaded       = %i\n",t->shape->loaded) ;
         printf("      display_list = %i\n",
             t->shape->lod_control[0].dist_level[0].gl_display_list) ;
      }else{
         printf("      No wagon specified\n") ;
      }
      vn = t->vn ;
      printf("      vn           = %p\n",(void *)vn) ;
      if(t->vn == NULL) return 0;
      printf("      length, radius, angle = %f %f %f\n",vn->length,vn->radius,vn->angle) ;
      printf("    EXIT  %s\n",my_name) ;

      return 0 ;
}

/*
 *   Routine to update position of traveller
 */

int  trv_position(TravellerNode *t){

TrkVectorNode  *vn  = t->vn       ;
WagonNode      *wn  = t->wagon    ;
ShapeNode      *sn  = t->shape    ;
double         dist = t->vect_position ;
double         a ;

int            ip = 0 ;
char           my_name[] = "trv_position" ;

      if(!t->wagon) ip = 0 ;  // Skip dummy wagons
/*
 *  Update distance relative to section origin
 */
      t->sect_distance = vn->distance0 + dist ;

//      ip = l_time_1s ;
      if(!t->wagon || !t->shape) return 0 ;

      if(ip){
        printf("\n----------------------------------------------------------\n") ;
        printf("  ENTER %s\n",my_name);
        printf("  Node Index = %i, distance = %f :: Note type = %i :: v %i, j %i, e %i\n",
                  t->tn->uid, t->sect_distance, t->tn->type_of_node,
                  VECTOR_SECTION,JUNCTION,END_SECTION) ;
        printf("  vector length = %i, itrack = %i, ivector = %i, idirect = %i\n",
                  t->tn->length_of_vector,t->itrack,t->ivector,t->idirect) ;
        printf("  distance0 = %f, dist = %f, sect_dist = %f\n",
               vn->distance0, dist, t->sect_distance) ;
      }

      l_disp1 = l_disp0 ;  //  Traveller printing
/*
 *  Place traveller in position relative to origin of vector
 */
      if(0.0 == vn->angle){       // Straight track
          t->x       = 0.0  ;
          t->y       = 0.0  ;
          t->z       = dist ;
          t->ang_deg = 0.0  ;
        }else{                     // Curved track
          a = dist/vn->radius ;
          t->x       = vn->radius*(1.0 - cos(a)) ;
          t->y       = 0.0                       ;
          t->z       = vn->radius*sin(a)         ;
          t->ang_deg = -a*degree                 ;
          if(vn->angle<0.0){
            t->x       = -t->x                   ;
            t->ang_deg = -t->ang_deg             ;
          }
        }
      if(t->idirect == 0)   t->ang_deg += 180.0 ;
      if(wn->train_dir == 0)t->ang_deg += 180.0 ;

      if(ip)printf("    Dist = %f,  final position x, y, z = %f %f %f,"
                   " t->ang_deg = %f, vn->angle = %f, vn->radius = %f m\n",
              dist, t->x, t->y, t->z, t->ang_deg, vn->angle, vn->radius) ;
      return 0 ;
}

/*
 *==============================================================================
 *  Routine to convert traveller position along track into:
 *  1.  Position relative to origin of track vector ( x, y, z)
 *  2.  Position relative to origin of world tiles  (rx,ry,rz)
 *==============================================================================
 */

int set_traveller_posn(WagonNode *w){

  double         a, b, c ;
  double   rail_height = 0.27 ;  // Best fit placing wheels on track
  GLfloat  scalei = 1.0/plot_scale ;
  TravellerNode  *tv ;
  TrkVectorNode  *vn ;

      tv = w->traveller ;
      vn = tv->vn ;
      a = degree*vn->a_east_x   ;
      b = degree*vn->a_height_y ;
      c = degree*vn->a_north_z  ;

      global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                         vn->tile_east_x, vn->tile_north_z,
                         vn->east_x, vn->north_z, vn->height_y+rail_height,
                         &(tv->x), &(tv->y), &(tv->z) ) ;
      mstswagon2local(0.0, 0.0, 0.0, tv->x, tv->y, tv->z, a, b, c, scalei,
                         tv->x, tv->y, tv->z,
                         &(tv->rx), &(tv->ry), &(tv->rz) ) ;
      return 0 ;
}

/*
 *  Routine to check the matrices sued by the traveller.
 *  This is left here in case it is ever useful again.
 */

#if 0
int   check_matrices(TravellerNode *t){

TrkSector    *nn ;
TrkVectorNode *vn, *vn1 ;
WorldItem     *wi, *wi1 ;
int           lov ;
int           iv  ;
double        X, Y, Z, X1, Y1, Z1 ;
double        XX, YY, ZZ, DD   ;
double        V0X, V0Y, V0Z, V1X, V1Y, V1Z, V2X, V2Y, V2Z ;
double        A0X, A0Y, A0Z, A0A ,A1X, A1Y, A1Z, A1A ;

      nn  = t->trk_net_node   ;
      iv  = t->ivector        ;
      vn  = &(nn->vector[iv]) ;
      vn1 = vn++              ;
      wi  = vn->world_item    ;
      wi1 = vn1->world_item   ;

      X  = wi->X  ; Y  = wi->Y  ; Z  = wi->Z  ;
      X1 = wi1->X ; Y1 = wi1->Y ; Z1 = wi1->Z ;
      XX = X1 - X ; YY = Y1 - Y ; ZZ = Z1 - Z ;
      DD = 1.0/sqrt(XX*XX + YY*YY + ZZ*ZZ) ;

      V0X = 0.0   ; V0Y = -1.0   ; V0Z = 0.0   ;
      V1X = XX*DD ; V1Y = YY*DD ; V1Z = ZZ*DD ;

      V2X = V0Z*V1Y - V1Z*V0Y ;
      V2Y = V0X*V1Z - V1X*V0Z ;
      V2Z = V0Y*V1X - V1Y*V0X ;
      DD = 1.0/sqrt(V2X*V2X + V2Y*V2Y + V2Z*V2Z) ;

      printf("  matrix Check\n");
      printf("  Vector 0 : position = %f %f %f\n",X,Y,Z);
      printf("  Vector 1 : position = %f %f %f\n",X1,Y1,Z1);
      printf("          difference  = %f %f %f\n",XX,YY,ZZ);
      printf("       Unit vector 0  = %f %f %f\n",V0X,V0Y,V0Z);
      printf("       Unit vector 1  = %f %f %f\n",V1X,V1Y,V1Z);
      printf("        cross product = %f %f %f\n",V2X,V2Y,V2Z);
      printf("        normalised    = %f %f %f : %f %f\n",
                                            V2X*DD,V2Y*DD,V2Z*DD,degree/DD,1.0/DD);

      A0X =   wi->AX  ; A0Y =   wi->AY  ; A0Z =   wi->AZ  ; A0A = wi->ANG ;
      A1X =   wi1->AX ; A1Y =   wi1->AY ; A1Z =   wi1->AZ ; A1A = wi1->ANG ;
      printf("       Rotation Vec 0 = %f %f %f : %f %f\n",A0X,A0Y,A0Z,A0A,A0A*radian);
      printf("       Rotation Vec 1 = %f %f %f : %f %f\n",A1X,A1Y,A1Z,A1A,A1A*radian);
      printf("       Quaternion 0   = %f %f %f %f\n",wi->A, wi->B, wi->C, wi->D) ;
      printf("       Quaternion 1   = %f %f %f %f\n",wi1->A,wi1->B,wi1->C,wi1->D) ;
// If direction of shape is swapped then  TrackVectorNode flag2 should be zero
      printf("       Vector 0: iflags = %i %i\n",vn->flag1, vn->flag2);
      printf("       Vector 1: iflags = %i %i\n",vn1->flag1, vn1->flag2);

      return 0;
}
#endif



