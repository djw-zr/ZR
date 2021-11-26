/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  freetype.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file specifying with the freetype compiler option to
 *   definee freetype variables and functions used to display text.
 *
 *==============================================================================
 */
#ifdef zr_freetype

//#include <freetype/ft2build.h>    // [Package freetype2-devel]
#include <ft2build.h>    // [Package freetype2-devel]
#include FT_FREETYPE_H


int freetype_init() ;
void render_text(const char *text, GLfloat x, GLfloat y, float sx, float sy) ;
void render_text_as_greyscale(const char *text, FT_Face type, int isize) ;

// ZR freetype variables

FT_Library   ft_lib ;
FT_Face      ft_tahoma,
             ft_verdana ;



#endif

