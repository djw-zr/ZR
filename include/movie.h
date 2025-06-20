/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  movie.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file specifying the variables and functions used by ZR
 *   to generate a sequence of images or a movie
 *
 *==============================================================================
 */

//#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>

#if LIBPNG
#include <png.h>
#endif

#if FFMPEG
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
#endif

#if defined FFMPEG || defined LIBPNG || defined PPM
# define MOVIE
#endif

#if (defined FFMPEG && defined LIBPNG) ||(defined PPM && defined LIBPNG) ||(defined FFMPEG && defined PPM)
# define MOVIE_ERROR
#endif

int  movie_on    = 0 ;  //  Keyboard switch
int  movie_stat  = 0 ;  //  Current status
int  movie_shot  = 0 ;  //  Incremented at start of each shot
int  movie_frame = 0 ;  //  Incremented before fram capture
double shot_start = 0.0 ; // Time seconds at start of shot
char *movie_dir ;       //  Directory holding movie or movie frames




