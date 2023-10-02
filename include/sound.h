/*
 * =============================================================================
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  sound.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file containing the structures used by ZR to store
 *   sound data
 *
 * =============================================================================
 */

#ifdef OPENAL

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

  ALCcontext *al_context;
  ALCdevice  *al_device;

#endif

/*
 *  All sounds in use by the program have a SundNode,
 *    with its name, full filename and information on whether it is
 *    needed and loaded
 */

struct soundnode {
  struct soundnode *next ;
  char             *sms_filename  ;
  char             *sms_fullfilename ;
  int              needed ;
  int              loaded ;
} ;
