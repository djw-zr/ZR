/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  setup_openal.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for initialising OpenAL sound
 *
 *==============================================================================
 */

static void list_audio_devices(const ALCchar *devices) ;
/*
 *   Test for error script
 */
#define TEST_ERROR(_msg)             \
      error = alGetError();          \
      if (error != AL_NO_ERROR) {    \
        fprintf(stderr, _msg " :: error = %i :: %i %i\n", \
                error,AL_INVALID_ENUM,AL_INVALID_NAME);  \
        return -1;                   \
      }

/*
 *  Initial setup
 */
int  setup_openal(int argc, char **argv){

  int  ip = 0 ;
  ALboolean enumeration;
  const ALCchar *devices;
  const ALCchar *defaultDeviceName = NULL ;

  int   iret;
  char  *my_name = "setup_openal" ;

      if(ip){
        printf("  Enter routine %s\n",my_name) ;
        printf("***********************************************************\n") ;
        printf("  Initialise OpenAL sound system\n") ;
        printf("***********************************************************\n") ;
      }
      defaultDeviceName = (argc>1) ? argv[1] : "ZR" ;
      enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
      if (enumeration == AL_FALSE)
        fprintf(stderr, "  Enumeration extension not available\n");

      list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
      if (!defaultDeviceName)
        defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

      printf("  defaultDeviceName = %s\n",defaultDeviceName) ;

      al_device = alcOpenDevice(defaultDeviceName);
      if (!al_device) {
        fprintf(stderr, "  unable to open default device\n");
        return -1;
      }

      fprintf(stdout, "  Device: %s\n", alcGetString(al_device, ALC_DEVICE_SPECIFIER));
      alGetError();
/*
 *  Create context
 */
      al_context = alcCreateContext(al_device, NULL);
      if (!alcMakeContextCurrent(al_context)) {
        fprintf(stderr, "  failed to make default context\n");
        return -1;
      }
      if(ip){
        printf("***********************************************************\n") ;
        printf("  OpenAL sound system initialised\n") ;
      }
      return 0 ;
}

static void list_audio_devices(const ALCchar *devices)
{
  const ALCchar *device = devices, *next = devices + 1;
  size_t len = 0;

      fprintf(stdout, "Devices list:\n");
      fprintf(stdout, "  ----------\n");
      while (device && *device != '\0' && next && *next != '\0') {
        fprintf(stdout, "  %s\n", device);
        len = strlen(device);
        device += (len + 1);
        next += (len + 2);
      }
      fprintf(stdout, "  ----------\n");
}

