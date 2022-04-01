/*
 *******************************************************************************
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File : dynamic.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to handle dynamic tracks and roads.
 *
 *   For systems where track shapes are not provided this file also has the
 *   compiler option "-D_Generate_Track_Shapes" which generates the required
 *   track and road shapes using data in the track and road database files
 *   "[route].tdb" and "[route]".rdb.
 *
 *******************************************************************************
 */


/*
 *  Routine to generate a new shape for each section of dynamic track -
 *  and to link it to the world item.
 */
int create_dynamic_track_node(WorldItem *witem){

int          i      ;
TrkSectNode  *pnode ;

/*
 *  Generate initialise track section node
 */
      pnode = (TrkSectNode *)malloc(sizeof(TrkSectNode));
      witem->u.dyn_track_obj.tsnode = pnode ;
      for(i=0;i<3;i++){
       witem->u.dyn_track_obj.gl_display_list[i] =   0 ;
       witem->u.dyn_track_obj.dist_level[i]      = 0.0 ;
      }
      init_track_path(pnode) ;

      pnode->type_of_node     = VECTOR_SECTION ;
      pnode->length_of_vector = 6              ;
      pnode->vector = (TrkVectorNode *)malloc(6*sizeof(TrkVectorNode)) ;

      return 0;
}

