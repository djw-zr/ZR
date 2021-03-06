/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  wagon.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *
 *    Wagon structure variables used by ZR
 *
 *==============================================================================
 */


typedef struct wagonnode {

struct wagonnode *next       ;
int              is_engine   ;      //  True if engine
char             *name       ;
char             *file       ;      // *.eng or *.wag file
char             *s_file     ;      // Shape file
char             *sd_file    ;      // Additional information file
char             *image      ;      // Silhouette image file (*.jpg)
struct wagonnode *tender     ;      //  For engines only pointer to tender
} WagonNode  ;

typedef struct wagondir {
struct wagondir  *next      ;
char             *name      ;       //  Directory name
char             *path_name ;       //  Full path name for this directory
WagonNode        *wagon     ;       //  Engines/Wagons in this directory
TextureNode      *texture   ;       //  List of textures in this directory
} WagonDir   ;
