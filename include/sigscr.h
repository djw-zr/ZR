/*
 * File sigsdc.h
 *
 *   This file contains data structures used to store signal script
 *   programs.
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   The file is based on the "Include File" included in:
 *     Lex & Yacc Tutorial by Tom Niemann
 *   and the file "calc3.c" available from the site given below.

 *   The tutorial document states that "Source code, when part of a
 *   software project, may be used freely without reference to the
 *   author and is available at the following web site.
 *     http://epaperpress.com/lexandyacc"
 */

typedef enum { typeCon, typeId, typeStr, typeOpr } nodeEnum;

/* constants */

typedef struct {
nodeEnum type;      /* type of node */
int value;          /* value of constant */
} conNodeType;

/* identifiers */

typedef struct {
nodeEnum type;      /* type of node */
int i;              /* subscript to ident array */
} idNodeType;

typedef struct {
nodeEnum type;      /* type of node    */
char    *name;      /* Pointer to name */
} strNodeType;

/* operators */

typedef struct {
nodeEnum type;      /* type of node */
int oper;           /* operator */
int nops;           /* number of operands */
union nodeTypeTag *op[1];  /* operands (expandable) */
} oprNodeType;

typedef union nodeTypeTag {
nodeEnum    type;    /* Type of node */
conNodeType con ;    /* Constants    */
idNodeType  id  ;    /* Identifiers  */
strNodeType str ;    /* Name/String  */
oprNodeType opr ;    /* Operators    */
} nodeType;

extern int sym[26];



