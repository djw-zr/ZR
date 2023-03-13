/*
 *******************************************************************************
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File: junction.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File contains routines for
 *       reading the track database file and
 *       populating the track section database
 *   Based on OpenRails file TrackDatabaseFile.cs
 *
 * *****************************************************************************
 */

double branch_curve(TrkSector *tn, int pin_1);

/*
 *  Routine to simplify storage of two-way switches (UK points).
 *    The first, denoted by 'pin' zero is always the common track.
 *    The next two (1 and 2) correspond to the left and right branches
 *    The integer straight indicates which of 1 or 2 is the straightest
 *    The integer 'branch' indicates the current position of the switch
 *
 *    Sometimes both branches are the same (Route USA2, Track section 196)
 */

int   set_junction_path(TrkSector *tn){

uint   ip = 0 ,          // DEBUG
       n_in_pins,
       n_ot_pins,
       n, i0, i1          ;
double c1, c2, c0 ;
char  my_name[] = "set_junction_path" ;


//      ip = ((int)tn->uid == 196) ;

      n_in_pins = tn->type_of_pin[0] ;
      n_ot_pins = tn->type_of_pin[1] ;
      if(n_in_pins+n_ot_pins>3){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : Wrong number of new pins : n_in_pins, n_ot_pins = %i %i\n",
                                                               n_in_pins,n_ot_pins) ;
        exit(1) ;
      }
      if(ip){
        printf("  Junction section:  %i\n",tn->uid);
        printf("    Number of in and out pins %i %i\n", n_in_pins, n_ot_pins) ;
        printf("    Pinned sections:          %i %i %i\n",
          tn->pin_to_section[0],tn->pin_to_section[1],tn->pin_to_section[2]) ;
      }
/*
 *  If necessary reorganise so that number of 'in' pins equals 1
 */
      if(n_in_pins>1){
        i1                    = tn->pin_to_section[2] ;
        tn->pin_to_section[2] = tn->pin_to_section[0] ;
        tn->pin_to_section[0] = i1 ;
        tn->type_of_pin[0] = tn->type_of_pin[1] ;
        tn->type_of_pin[1] = n_in_pins ;
      }
/*
 *  Calculate curvature of 'out' tracks
 */
      c1 = branch_curve(tn, 1) ;
      c2 = branch_curve(tn, 2) ;
      if((c1 == c2) && (tn->pin_to_section[1] != tn->pin_to_section[2]) ){
        printf("  Routine %s.  Routine branch_curve returns equal curvatures.\n",my_name) ;
        printf("  Junction %i, Pins %i %i %i, Curvature %f %f\n",
          tn->uid,     tn->pin_to_section[0],
          tn->pin_to_section[1], tn->pin_to_section[2],
          c1, c2) ;
      }else if(ip){
        printf("  Junction %i, Pins %i %i %i, Curvature %f %f\n",
          tn->uid,     tn->pin_to_section[0],
          tn->pin_to_section[1], tn->pin_to_section[2],
          c1, c2) ;
      }
/*
 *  Order 'out' tracks so left comes first
 */

      if(c1>c2){
        i1                    = tn->pin_to_section[2] ;
        tn->pin_to_section[2] = tn->pin_to_section[1] ;
        tn->pin_to_section[1] = i1 ;
        c0 = c1 ; c1 = c2; c2 = c0 ;
      }
/*
 *  Specify which 'out' strait is straightest
 */
      tn->straight = fabs(c1)>fabs(c2) ? 2 : 1 ;
/*
 *  Initially set switch to straight branch
 */
      tn->branch = tn->straight ;
      if(ip){
      printf("  BB Junction %i, Pins %i %i %i, Curvature %f %f :: %i %i\n",
        tn->uid ,
        tn->pin_to_section[0],
        tn->pin_to_section[1],
        tn->pin_to_section[2],
        c1, c2,
        tn->straight,
        tn->branch) ;
      }
/*
 *  Set iz_off in world item corresponding to first vector of
 *  straight to 2
 */
      i0 = tn->uid ;
      i1 = tn->pin_to_section[tn->straight] ;  // index of TrkSecNode
TrkSector   *tsn ;
TrkVectorNode *vec ;
WorldItem     *wit ;
double        length = 0.0 ;
      tsn = &track_db.trk_sections_array[i1-1] ;
      if(i0 == tsn->pin_to_section[0]){
        for(n=1;n<tsn->length_of_vector;n++){
          vec = &(tsn->vector[n-1])  ;
          wit = vec->world_item ;
          if(!wit)break ;
          wit->iz_off = 2       ;
          length += vec->length ;
          if(391==i0)printf(" Junction : %i %f %i %i\n",n,length, wit->uid, wit->iz_off);
          if(length>100.0)break ;
        }
      }else if (i0 == tsn->pin_to_section[1]){
        for(n=tsn->length_of_vector;n>0;n--){
          vec = &(tsn->vector[n-1])  ;
          wit = vec->world_item ;
          if(!wit) break ;
          wit->iz_off = 2       ;
          length += vec->length ;
          if(391==i0)printf(" Junction : %i %f %i %i\n",n,length, wit->uid, wit->iz_off);
          if(length>100.0)break  ;
        }
      }else{
        printf("  Trying to match pins.  Orig = %i, New = %i, pin[0] = %i, pin[1] = %i\n",
             i0, i1, tsn->pin_to_section[0], tsn->pin_to_section[1]) ;
      }

      return 0 ;
}
/*
 *  Routine to calculate the curvature of each branch at a junction
 *  This works when the branch is curved most.
 *
 *  In some (all?) MSTS junctions the branch line starts with a short section
 *  typically ~ 1.5 m long.  For these cases the second section is tested.
 */

double branch_curve(TrkSector *tn, int pin_1){

int           ip = 0 ;
uint          n0, n1 ;
TrkSector   *tn1 ;
TrkVectorNode *vn1 ;
double        c1 ;
char  my_name[] = "branch_curve" ;

//      ip = ((int)tn->uid == 492) ;
      if(ip)printf("    Routine %s :: Node index = %i\n",my_name,(int)tn->uid) ;

      if(tn->type_of_node != JUNCTION){
        return -1 ;
      }
      n0 = tn->uid ;
/*
 *  Find curvature of track connected to Pin 1
 */
      n1 = tn->pin_to_section[pin_1] ;
      tn1 = &track_db.trk_sections_array[n1-1] ;
      if(ip){
        printf("      Calculate curvature of track connected to node %i, pin %i\n",n0,pin_1) ;
        printf("      Index of connected track = %i\n", n1) ;
      }

      if(tn1->pin_to_section[0] == n0){
        vn1 = &tn1->vector[0] ;
        if(ip){
          printf("      Pin matches start of track section %i\n",tn1->uid) ;
          printf("      First vector : angle = %f, radius = %f\n",vn1->angle, vn1->radius) ;
        }
        if(vn1->angle == 0.0){
          c1 = 0.0 ;
        }else{
          c1 = 1.0/vn1->radius ;
          if(vn1->angle < 0.0)c1 = -c1 ;
        }
        if(ip)printf("      Curvature =  %f  : length = %f %i\n",c1,vn1->length,tn1->length_of_vector) ;
        if(c1 == 0.0 && vn1->length < 3.0 && tn1->length_of_vector>1){
          vn1 = &tn1->vector[1] ;
          if(vn1->angle == 0.0){
            c1 = 0.0 ;
          }else{
            c1 = 1.0/vn1->radius ;
            if(vn1->angle < 0.0)c1 = -c1 ;
          }
        }
      }else if(tn1->pin_to_section[1] == n0){
        vn1 = &tn1->vector[tn1->length_of_vector-1] ;
        if(ip){
          printf("     Pin matches end   of track section %i\n",tn1->uid) ;
          printf("     Last  vector : angle = %f, radius = %f\n",vn1->angle, vn1->radius) ;
        }
        if(vn1->angle == 0.0){
          c1 = 0.0 ;
        }else{
          c1 = -1.0/vn1->radius ;
          if(vn1->angle < 0.0)c1 = -c1 ;
        }
        if(ip)printf("      Curvature =  %f  : length = %f %i\n",c1,vn1->length,tn1->length_of_vector) ;
        if(c1 == 0.0 && vn1->length < 3.0 && tn1->length_of_vector>1){
          vn1 = &tn1->vector[tn1->length_of_vector-2] ;
          if(vn1->angle == 0.0){
            c1 = 0.0 ;
          }else{
            c1 = -1.0/vn1->radius ;
            if(vn1->angle < 0.0)c1 = -c1 ;
          }
        }
      }else{
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : Pin 1 : Unable to find matching section %i\n", n1) ;
        exit(1) ;
      }
      return c1 ;
}
