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

double branch_curve(TrkNetNode *tn, int pin_1);

/*
 *  Routine to simplify storage of two-way switches (UK points).
 *    The first, denoted by 'pin' zero is always the common track.
 *    The next two (1 and 2) correspond to the left and right branches
 *    The integer straight indicates which of 1 or 2 is the straightest
 *    The integer 'branch' indicates the current position of the switch
 */

int   set_junction_path(TrkNetNode *tn){

uint  ip = 0 ,          // DEBUG
      n_in_pins,
      n_ot_pins,
      i1, i2 ;
double c1, c2, c0 ;
char  my_name[] = "set_junction_path" ;

      n_in_pins = tn->type_of_pin[0] ;
      n_ot_pins = tn->type_of_pin[1] ;
      if(n_in_pins+n_ot_pins>3){
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : Wrong number of new pins : n_in_pins, n_ot_pins = %i %i\n",
                                                               n_in_pins,n_ot_pins) ;
        exit(1) ;
      }
      if(ip){
        printf("  Junction section:  %i\n",tn->index_of_node);
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
      if(ip)printf(" Junction %i, Pins %i %i %i, Curvature %f %f\n",
        tn->index_of_node ,
        tn->pin_to_section[0],
        tn->pin_to_section[1],
        tn->pin_to_section[2],
        c1, c2) ;
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
 *  Initially set swith to straight branch
 */
      tn->branch = tn->straight ;
      if(ip)printf(" Junction %i, Pins %i %i %i, Curvature %f %f :: %i %i\n",
        tn->index_of_node ,
        tn->pin_to_section[0],
        tn->pin_to_section[1],
        tn->pin_to_section[2],
        c1, c2,
        tn->straight,
        tn->branch) ;
      return 0 ;
}
/*
 *  Routine to calculate the curvature of each branch at a junction
 */

double branch_curve(TrkNetNode *tn, int pin_1){

int            n0, n1 ;
TrkNetNode    *tn1 ;
TrkVectorNode *vn1 ;
double        c1 ;
char  my_name[] = "branch_curve" ;

      if(tn->type_of_node != JUNCTION){
        return -1 ;
      }
      n0 = tn->index_of_node ;
/*
 *  Find curvature of track connected to Pin 1
 */
      n1 = tn->pin_to_section[pin_1] ;
      tn1 = &track_db.trk_sections_array[n1-1] ;
      if(tn1->pin_to_section[0] == n0){
        vn1 = &tn1->vector[0] ;
        if(vn1->angle == 0.0){
          c1 = 0.0 ;
        }else{
          c1 = 1.0/vn1->radius ;
          if(vn1->angle < 0.0)c1 = -c1 ;
        }
      }else if(tn1->pin_to_section[1] == n0){
        vn1 = &tn1->vector[tn1->length_of_vector-1] ;
        if(vn1->angle == 0.0){
          c1 = 0.0 ;
        }else{
          c1 = -1.0/vn1->radius ;
          if(vn1->angle < 0.0)c1 = -c1 ;
        }
      }else{
        printf("  ERROR in routine %s\n",my_name) ;
        printf("  ERROR : Pin 1 : Unable to find matching section %i\n", n1) ;
        exit(1) ;
      }
      return c1 ;
}
