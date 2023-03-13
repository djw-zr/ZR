/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  enum.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to seep searching for enum values from names
 *
 *==============================================================================
 */

/*
 *  Routine to initialise a master btree for the enum strings.
 *
 *  The tree allows the enum strings to be quickly converted to integer values,
 *  the integer values being stored as data pointers instead of in a separate
 *  integer array pointed to be the data field.
 */

int  enum_btree_init(void){

  int  i ;
  char *data ;
  char *data0 = NULL ;
  BTree *btree ;
  BTree *b = enum_master ;

      for(i=0;i<1552;i++){
        data = (void *)(&data0[i] - data0) ;
        enum_master = insert_node(enum_master,(char *)token_idc[i],(void *)data) ;
      }
/*
 *  Special cases where token_idc (enum.h) adds '_ALT'
 */
      enum_master = insert_node(enum_master,"SIGNAL",(void *)(&data0[317] - data0));
      enum_master = insert_node(enum_master,"PLATFORM",(void *)(&data0[360] - data0));
      enum_master = insert_node(enum_master,"SIDING",(void *)(&data0[1541] - data0));
      enum_master = insert_node(enum_master,"PICKUP",(void *)(&data0[1545] - data0));
/*
 *  Tests
 */
#if 0
  long l ;
      printf("  Call dump_btree for enum_master\n") ;
      dump_btree(enum_master,0,"X") ;
#endif
#if 1
  long l1 ;
      printf("  Call find_btree for SIGNAL and TR_WORLDFILE\n") ;
      btree = find_btree(enum_master,"SIGNAL") ;
      l1 = (long)btree->data ;
      printf("    Test btree = %p, SIGNAL       data = %li\n",(void *)btree,l1) ;
      btree = find_btree(enum_master,"TR_WORLDFILE") ;
      l1 = (long)btree->data ;
      printf("    Test btree = %p, TR_WORLDFILE data = %li\n",(void *)btree,l1) ;
#endif


      return 0 ;
}
