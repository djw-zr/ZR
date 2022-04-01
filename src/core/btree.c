/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *  File btree.c
 *
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File containing the code to display the one of the shapes
 *
 *   Binary tree routines based on: https://appliedgo.net/balancedtree/
 *
 *   These routines are designed to initialise a balanced binary tree
 *   in which the branch lengths differ at most by one node.  These C
 *   routine are based on https://appliedgo.net/balancedtree/ where
 *   the code is written for the 'go' language.
 *
 *   Each node of the tree contains its name and a (void) pointer to
 *   a data structure.  The user routines are:
 *
 *
 *   Btree insert_node(BTree *root_node, char *index, void *data)
 *           Returns : new root node
 *   Btree find_btree(BTree *root_node, char *string)
 *           Returns : node corrresponding to string or NULL
 *         find_btree(BTree *root_node, char *string)
 *           Prints out btree.  At top level 'string' is any name of top level
 *         bt_walk_a2z(BTree *root_node, void f(void *n))
 *         bt_walk_a2z(BTree *root_node, void f(void *n))
 *         bt_walk_a2z(BTree *root_node, void f(void *n))
 *         bt_walk_a2z(BTree *root_node, void f(void *n))
 *           Walk btree in order a to z (alhabetical order), z to a,
 *           top to bottom or bottom to top, applying
 *           function f(*node) to each node.
 *
 *  To test:
 *    gcc -o btree btree.c
 *==============================================================================
 */

#ifndef __ZR_H__

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


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

#endif

BTree  *rebalance(BTree *n) ;
void   print_btree(char *s, BTree *n) ;
void   dump_btree(BTree *n, int i, char *lr) ;

int  bt_max(int i, int j){
    return (i>=j) ? i : j ;
}

int height(BTree *node){

      if(!node) return 0 ;
      return node->height ;
}

int bal(BTree *node){

int l ;
      l = 0 ;
      if(node->left)  l += node->left->height ;
      if(node->right) l -= node->right->height ;
      return l ;
}

BTree *insert_node(BTree *node, char *index, void *data){

BTree *b ;
int   n  ;
//      printf("  Enter insert_node.            INDEX = %s,             data = %s\n",index,data) ;
//      printf("  Node = %p\n",(void *)node) ;
      fflush(NULL) ;

      if(node == NULL){
        b = malloc(sizeof(BTree)) ;
        b->left  = NULL ;
        b->right = NULL ;
        b->index = index ;
        b->data  = data  ;
        b->height = 1 ;
//        printf("  New node = %p\n",(void *)b) ;
        return b ;
      }

      n = strcmp(index,node->index) ;
      if(n>0){
//        printf("  Call insert_node_left  :: %s :: %s\n",index,node->index) ;
        node->left = insert_node(node->left,index,data) ;
        node->height = 1 + node->left->height ;
      }else{
//        printf("  Call insert_node_right :: %s :: %s\n",index,node->index) ;
        node->right = insert_node(node->right,index,data) ;
        node->height = 1 + node->right->height ;
      }
      if(node->left && node->right)
          node->height = bt_max(node->left->height, node->right->height) + 1 ;

      return rebalance(node) ;
}

BTree *RotateLeft(BTree *n){

int   ll, rr ;
BTree *r ;

      r        = n->right ;
      n->right = r->left ;
      r->left  = n ;

      ll = rr = 0 ;
      if(n->left)  ll = n->left->height ;
      if(n->right) rr = n->right->height ;
      n->height = 1 + ((ll >= rr) ? ll : rr) ;

      ll = rr = 0 ;
      if(r->left)  ll = r->left->height ;
      if(r->right) rr = r->right->height ;
      r->height = 1 + ((ll >= rr) ? ll : rr) ;

      return r ;
}

BTree *RotateRight(BTree *n){

int  ll, rr ;
BTree *l ;

      l        = n->left ;
      n->left  = l->right;
      l->right = n ;

      ll = rr = 0 ;
      if(n->left)  ll = n->left->height ;
      if(n->right) rr = n->right->height ;
      n->height = 1 + ((ll >= rr) ? ll : rr) ;

      ll = rr = 0 ;
      if(l->left)  ll = l->left->height ;
      if(l->right) rr = l->right->height ;
      l->height = 1 + ((ll >= rr) ? ll : rr) ;

      return l ;
}

BTree *RotateRightLeft(BTree *n){

      n->right  = RotateRight(n->right) ;
      n         = RotateLeft(n)         ;
      n->height = 1 + bt_max(height(n->left),height(n->right) ) ;
      return n ;
}

BTree *RotateLeftRight(BTree *n){

      n->left   = RotateLeft(n->left) ;
      n         = RotateRight(n)      ;
      n->height = 1 + bt_max(height(n->left),height(n->right) ) ;
      return n ;
}

BTree *rebalance(BTree *n){

      if(bal(n) > 1){
        if(bal(n->left) >= 0){
          return RotateRight(n) ;
        }else{
          return RotateLeftRight(n) ;
        }
      }else if(bal(n) < -1){
        if(bal(n->right) <= 0){
          return RotateLeft(n) ;
        }else{
          return RotateRightLeft(n) ;
        }
      }
      return n ;
}

BTree *find_btree(BTree *n, char *string){

int  l ;

      if(!n) return NULL ;

      l = strcmp(n->index,string) ;

      if(l == 0)return n ;
      if(l <  0)return find_btree(n->left,string) ;
      return find_btree(n->right,string) ;
}

void  dump_btree(BTree *n, int i, char *lr){

int  l;

      fflush(NULL) ;
      if(!n) return ;
      for(l=0;l<i;l++){ printf("  ");}
      printf(" %s + -- bal = %2i, height =  %2i, INDEX = %s,         data = %p\n",
             lr,bal(n),n->height,n->index,n->data) ;
      dump_btree(n->left,i+1,"L") ;
      dump_btree(n->right,i+1,"R") ;
      return ;
}


/*
 *  Walk structure and apply function f() to each node
 */
void  bt_walk_a2z(BTree *n, void f(void *n)){

      if(!n) return ;
      bt_walk_a2z(n->left,f) ;
      f((void *)n) ;
      bt_walk_a2z(n->right,f) ;

      return ;
}

void  bt_walk_z2a(BTree *n, void f(void *n)){

      if(!n) return ;
      bt_walk_z2a(n->right,f) ;
      f((void *)n) ;
      bt_walk_z2a(n->left,f) ;

      return ;
}

void  bt_walk_t2b(BTree *n, void f(void *n)){

      if(!n) return ;
      f((void *)n) ;
      bt_walk_t2b(n->left,f) ;
      bt_walk_t2b(n->right,f) ;

      return ;
}

void  bt_walk_b2t(BTree *n, void f(void *n)){

      if(!n) return ;
      bt_walk_b2t(n->left,f) ;
      bt_walk_b2t(n->right,f) ;
      f((void *)n) ;

      return ;
}

void  print_node(void *n){

BTree *p ;
      p = (BTree *) n ;
      printf("  Print : Node = %p,  Index = %s,     Value = %s\n",(void *)p, p->index, (char *)p->data) ;
      return ;
}

#ifndef __ZR_H__

int main(int argc, char **argv){

BTree  *bt = NULL ;
BTree  *bp ;

      bt = insert_node(bt,"z",(void *)"Data z") ;
      printf(" ==================================\n") ;
      dump_btree(bt,0,"X") ;
      printf(" ==================================\n") ;
      bt = insert_node(bt,"t",(void *)"Data t") ;
      dump_btree(bt,0,"X") ;
      printf(" ==================================\n") ;
      bt = insert_node(bt,"m",(void *)"Data m") ;
      dump_btree(bt,0,"X") ;
      printf(" ==================================\n") ;
      bt = insert_node(bt,"a",(void *)"Data a") ;
      dump_btree(bt,0,"X") ;
      printf(" ==================================\n") ;
      bt = insert_node(bt,"p",(void *)"Data p") ;
      dump_btree(bt,0,"X") ;
      printf(" ==================================\n") ;
      bt = insert_node(bt,"f",(void *)"Data f") ;
      dump_btree(bt,0,"X") ;
      printf(" ==================================\n") ;
      bt = insert_node(bt,"n",(void *)"Data n") ;
      dump_btree(bt,0,"X") ;
      printf(" ==================================\n") ;
      bt = insert_node(bt,"q",(void *)"Data q") ;

      dump_btree(bt,0,"X") ;

      bp = find_btree(bt,"p");
      printf("  Found node = %p,  Index = %s,     Value = %s\n",(void *)bp, bp->index, (char *)bp->data) ;

      printf("\n") ;
      bt_walk_a2z(bt,(void *)print_node) ;
      printf("\n") ;
      bt_walk_z2a(bt,(void *)print_node) ;
      printf("\n") ;
      bt_walk_t2b(bt,(void *)print_node) ;
      printf("\n") ;
      bt_walk_b2t(bt,(void *)print_node) ;

      return 0 ;
}
#endif
