/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  core.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file specifying the structures used by the core files
 *
 *==============================================================================
 */

typedef struct btree {
struct btree *left  ;        //    Left node
struct btree *right ;        //    Right node
char         *index ;        //    Name used to find node
void         *data  ;        //    Pointer to data structure
int          height ;        //    Height of node
} BTree ;

typedef struct tree {
BTree    *root ;
} Tree;

BTree *insert_node(BTree *root_node, char *index, void *data) ;
BTree *find_btree(BTree *root_node, char *string) ;
