 /*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  train.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for initialising trains, consists, wagons and travellers
 *
 *==============================================================================
 */

/*
 *  Routine to initialise a WagonNode from the list of raw wagons
 *  Name is name of wagon in rawwagonenode list
 */

WagonNode *wagon_init(char *name){

  int          n, m ;
  RawWagonNode *raw ;
  WagonNode    *w   ;
  char         *my_name = "wagon_init" ;

      if(strlen(name) == 0){
        printf("ERROR.  Routine %s.  Parameter name has zero length\n",my_name);
        exit(-1) ;
      }

//  Search for name

      n = 1 ;
      for(raw = wagonlist_beg; raw==NULL; raw = raw->next){
        if((n=strcmp(raw->name,name))==0) break ;
      }
      if(n != 0){
         printf(" Routine %s.  Unable to find wagon %s\n",my_name,name);
         exit(-1) ;
      }
/*
 *  Generate new WagonNode
 */
      w = (WagonNode *)malloc(sizeof(WagonNode)) ;
      w->next       = NULL  ;
      w->prev       = NULL  ;
      w->name       = (char *)malloc(strlen(name)*sizeof(char)) ;
      strcpy(w->name,name)  ;
      w->index      = 0     ;
      n = strlen(raw->file) ;
      w->motor = !strcmp(".eng",&(raw->file[n-4]));
      w->direction  = 1     ;     // Initialise as 'true'
      w->n_travel   = 1     ;     // NOTE Change for multi-bogie wagons
      w->dist_front = 10.0  ;     // NOTE Change once sd file read
      w->dist_back  = 10.0  ;
      w->traveller  = NULL  ;     //  Initialise once train is generated
      w->shape      = NULL  ;
      w->raw_wagon = raw    ;

      return w ;
}

TrainNode *consist_init(WagonNode *wagon, char *name){

  TrainNode    *c   ;
  char         *my_name = "consist_init" ;

      if(wagon == NULL){
        printf("ERROR.  Routine %s.  Parameter wagon is NULL\n",my_name);
        exit(-1) ;
      }
      if(strlen(name) == 0){
        printf("ERROR.  Routine %s.  Parameter name has zero length\n",my_name);
        exit(-1) ;
      }

      c = (TrainNode *)malloc(sizeof(TrainNode)) ;
      c->next       = NULL ;
      c->name       = (char *)malloc(strlen(name)*sizeof(char)) ;
      strcpy(c->name,name)  ;
      c->speed      = 0.0   ;
      c->first      = wagon ;
      c->last       = wagon ;
      c->motor      = (wagon->motor) ? wagon : NULL ;

      return c ;
}

/*
 *  Routine to add one wagon onto the end of a consist
 */

int *consist_add_wagon(TrainNode *c, WagonNode *w){

  WagonNode    *new  ;
  WagonNode    *last  ;
  char         *my_name = "consist_add_wagon" ;

      if(c == NULL){
        printf("ERROR. Routine %s.  Parameter TrainNode is NULL\n",my_name);
        exit(-1) ;
      }
      if(w == NULL){
        printf("ERROR. Routine %s.  Parameter WagonNode is NULL\n",my_name);
        exit(-1) ;
      }
/*
 *  Generate new wagon mode and copy across data
 *  Variables next, prev and traveller must be reinitialised later
 */
      new = (WagonNode *)malloc(sizeof(WagonNode)) ;
      *new = *w ;               //  Copy contents

      last           = c->last         ;
      new->index     = last->index + 1 ;
      new->traveller = NULL            ;
      new->prev      = last            ;
      last->next     = new             ;
      c->last        = new             ;
      if(c->motor == NULL){
        if(new->motor) c->motor = new ;
      }
      return 0;
}

TrainNode  *init_train(TrainNode *consist) {

  char         *my_name = "init_train" ;

      return NULL;
}
/*
 *  Routine to intiialise wagons, consists and trains
 */

int  init_trains(){




      return 0;
}


