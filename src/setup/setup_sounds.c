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

SMS_Node *smsnode_init(char *name) ;

/*
 *  Routine to link the world sound sources and sound regions
 *  to the required SoundManagementSystem node
 */

int link_world_sounds_to_sms_nodes(void){

  int  ip = 0 ;
  WorldNode       *w_node ;
  SoundObjectNode *so_node ;
  SoundRegionNode *sr_node ;
  SMS_Node        *sms_node ;
  char            *my_name = "link_world_sounds_to_sms_nodes" ;

      if(ip)printf("  Enter routine '%s'\n",my_name) ;
/*
 *  Loop through world tiles, link the sound sources and regions
 *  to the sound nodes
 */
      for(w_node = worldlist_beg; w_node != NULL; w_node = w_node->next){
        for( so_node = w_node->sound_source; so_node != NULL;
                                             so_node = so_node->next){
          so_node->sms_node = smsnode_init(so_node->sms_file) ;
        }
#if 0
        for( sr_node = w_node->sound_region; sr_node != NULL;
                                             sr_node = sr_node->next){
          sr_node->sound = soundnode_init(sr_node->sms_filename) ;
        }
#endif
      }
      if(ip){
        for(sms_node = sms_list_beg; sms_node != NULL; sms_node = sms_node->next){
          printf("  Sound = %s :: file = %s\n",sms_node->name, sms_node->sms_file) ;
        }
      }
      return  0 ;
}

/*
 *  Given a filename this routine searches for the corresponding
 *  SMS_Node representing a sound management system sound.
 *  If one does not exist it creates a new one.
 */

SMS_Node *smsnode_init(char *sms_file){

  int  ip = 0 ;
  int  i;
  SMS_Node   *sms_node ;
  SmsGrpNode *sndpri_node ;
  char      *name, *extn ;
  BTree     *btree ;

  char *my_name = "smsnode_init" ;

      if(ip)printf("  Enter routine '%s'\n",my_name) ;

      name = zr_corename(sms_file) ;
      str2lc(name) ;
      btree = find_btree(sms_master, name) ;
      if(btree) return (SMS_Node *) btree->data ;

      extn = zr_extension_lc(sms_file) ;
      if(strcmp(extn,"sms")){
        free(extn)  ;
        return NULL ;
      }
      free(extn) ;
/*
 *  Create a new sound node
 */
      sms_node = (SMS_Node *)malloc(sizeof(SMS_Node)) ;
      sms_node->next = NULL ;
      sms_node->name = name ;
      sms_node->sms_file = sms_file ;
      sms_node->sms_fullfilename = NULL ;
      sms_node->needed = 0 ;
      sms_node->loaded = 0 ;
      for(i=0;i<4;i++){
        sndpri_node = &(sms_node->smsgrp_node[i]) ;
        sndpri_node->active.cab_cam = 0 ;
        sndpri_node->active.ext_cam = 0 ;
        sndpri_node->active.pas_cam = 0 ;
        sndpri_node->active.dist    = 0.0 ;
        sndpri_node->deacti.cab_cam = 0 ;
        sndpri_node->deacti.ext_cam = 0 ;
        sndpri_node->deacti.pas_cam = 0 ;
        sndpri_node->deacti.dist    = 0.0 ;

        sndpri_node->volume         = 1.0 ;
        sndpri_node->stereo         = 0 ;
        sndpri_node->loadall_waves  = 0 ;
        sndpri_node->n_streams      = 0 ;
        sndpri_node->snd_stream     = NULL ;
      }

      if(sms_list_beg == NULL){
        sms_list_beg = sms_node ;
      }else{
        sms_list_end->next = sms_node ;
      }
      sms_list_end = sms_node ;
      sms_master = insert_node(sms_master, name, (void *)sms_node) ;
      if(ip)printf("  SMS_MASTER - ADD NAME %s\n",name) ;
      return sms_node ;
}



