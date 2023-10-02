/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  setup_sound .c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for initialising sound objects
 *
 *==============================================================================
 */

/*
 *  Routine to collect the required sounds
 *  It scans the world tiles looking for required sounds and placing them in
 *  a standard list
 */

SoundNode *soundnode_init(char *name) ;


int collect_sounds(void){

  int  ip = 1 ;
  WorldNode       *w_node ;
  SoundSourceNode *ss_node ;
  SoundRegionNode *sr_node ;
  SoundNode       *s_node ;
  char            *my_name = "collect_sounds" ;

      if(ip)printf("  Enter routine '%s'\n",my_name) ;
/*
 *  Loop through world tiles, link the sound sources and regions
 *  to the sound nodes
 */
      for(w_node = worldlist_beg; w_node != NULL; w_node = w_node->next){
        for( ss_node = w_node->sound_source; ss_node != NULL;
                                             ss_node = ss_node->next){
          ss_node->sound = soundnode_init(ss_node->sms_filename) ;
        }
        for( sr_node = w_node->sound_region; sr_node != NULL;
                                             sr_node = sr_node->next){
          sr_node->sound = soundnode_init(sr_node->sms_filename) ;
        }
      }
#if 1
      for(s_node = soundlist_beg; s_node != NULL; s_node = s_node->next){
        printf("  Sound file = '%s'\n",s_node->sms_filename) ;
      }
#endif
      return  0 ;
}

/*
 *  Given a filename this routine searches for the corresponding
 *  SoundNode.  If one does not exist it creates a new one.
 */

SoundNode *soundnode_init(char *sms_filename){

  int  ip = 1 ;
  SoundNode  *s_node ;

  char *my_name = "sound_init" ;

      if(ip)printf("  Enter routine '%s'\n",my_name) ;

      for(s_node = soundlist_beg; s_node != NULL; s_node = s_node->next){
        if(strcmp(s_node->sms_filename,sms_filename) == 0)break ;
      }
      if(s_node)return s_node ;
/*
 *  Create a new sound node
 */
      s_node = (SoundNode *)malloc(sizeof(SoundNode)) ;
      s_node->next = NULL ;
      s_node->sms_filename  = sms_filename ;
      s_node->sms_fullfilename = NULL ;
      s_node->needed = 0 ;
      s_node->loaded = 0 ;

      if(soundlist_beg == NULL){
        soundlist_beg = s_node ;
      }else{
        soundlist_end->next = s_node ;
      }
      soundlist_end = s_node ;
      return s_node ;
}

