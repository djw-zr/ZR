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

int print_train_data(void) ;
int check_train_movement(TrainNode *t) ;

/*
 *  Routine to initialise the trains and thei positions.  The composition of
 *  each train can be done either through a set of subroutines (add_train(),
 *  add_wagon_to_train(), etc) or (to be implimented) via input files and/or
 *  a gui (graphical user interface).
 */

int   trains_init(void){

  int  ip = 0 ;
  char *my_name = "trains_init" ;

/*
 *  Initialise one or more trains
 */
#if 1
      add_new_train("T01") ;
      add_wagon_to_train("T01","1905-D347", 1) ;
      add_wagon_to_train("T01","1905-D347-t", 1) ;

      add_wagon_to_train("T01","AU_NSW_Dtruck1_Empty", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck2_Empty", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck3_Empty", 1) ;
#if 0
//      add_wagon_to_train("T01","4W-CV-load", 0) ;
      add_wagon_to_train("T01","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T01","NI4wHopUnb4MT", 1) ;
      add_wagon_to_train("T01","4W-CV-mty", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck3_FlatSheet", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck3_HumpSheet", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck3_Coal", 1) ;
      add_wagon_to_train("T01","AU_NSW_Dtruck3_Ballast", 1) ;
      add_wagon_to_train("T01","JWW_Htype_L", 1) ;
      add_wagon_to_train("T01","JWW_Htype_E", 1) ;

      add_wagon_to_train("T01","AU_NSW_Dtruck3_Coal", 1) ;
      add_wagon_to_train("T01","JWW_Htype_L", 1) ;
      add_wagon_to_train("T01","JWW_Htype_E", 1) ;
      add_wagon_to_train("T01","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T01","NI4wHopUnb4MT", 1) ;
//      position_train("T01",376,1,1) ;
//      position_train("T01",123,1,1) ;
//      position_train("T01",377,6,0) ;
//  The direction here has no ef2fect on the way the wagon is displayed
//  before or after movement - whatever teh train direction
//  The error only occurs when the train direction is reverse - but
//  is corrected as soon as the train moves!!!!!!
      add_wagon_to_train("T01","4W-HG-Brake-Van", 0) ;
      add_wagon_to_train("T01","4W-HG-Brake-Van", 1) ;
      add_wagon_to_train("T01","4W-HG-Brake-Van", 0) ;
      add_wagon_to_train("T01","4W-HG-Brake-Van", 1) ;
#endif
//      add_wagon_to_train("T01","4W-HG-Brake-Van", 1) ;
//      add_wagon_to_train("T01","4W-HG-Brake-Van", 1) ;
// This does afffect position
      position_train("T01",392,1,0) ;
#endif
#if 1
      add_new_train("T02") ;

      add_wagon_to_train("T02","1905-I103", 1) ;

      add_wagon_to_train("T02","4W-LV-load", 1) ;
      add_wagon_to_train("T02","4W-LV-mty", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_Ash", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_Ballast", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_Coal", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_Empty", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_FlatSheet", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck1_HumpSheet", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck2_Coal", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck2_Empty", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck2_FlatSheet", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck2_HumpSheet", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck3_Ballast", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck3_Coal", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck3_Empty", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck3_FlatSheet", 1) ;
      add_wagon_to_train("T02","AU_NSW_Dtruck3_HumpSheet", 1) ;

      add_wagon_to_train("T02","4W-HG-Brake-Van", 1) ;
      position_train("T02",373,1,1) ;
//      position_train("T02",384,1,1) ;
#endif
#if 1
      add_new_train("T03") ;
      add_wagon_to_train("T03","1905-A115", 1) ;
      add_wagon_to_train("T03","1905-A115-t", 1) ;
      add_wagon_to_train("T03","1905-J484", 1) ;
      add_wagon_to_train("T03","1905-J484-t", 1) ;
      add_wagon_to_train("T03","1905-AD-Sleeper", 1) ;
      add_wagon_to_train("T03","1905-AD-Sleeper", 1) ;
      add_wagon_to_train("T03","1905-AC-StateCar", 1) ;
      add_wagon_to_train("T03","1905-AB-Diner", 1) ;
      add_wagon_to_train("T03","1905-CBC", 1) ;
      add_wagon_to_train("T03","1905-CBC", 1) ;
      add_wagon_to_train("T03","1905-CBC", 1) ;
      add_wagon_to_train("T03","1905-HO-JWW", 1) ;
//      position_train("T03",384,1,1) ;
      position_train("T03",356,1,1) ;
#endif
#if 1
      add_new_train("T04") ;
      add_wagon_to_train("T04","1905-O449", 1) ;
      add_wagon_to_train("T04","1905-O449-t", 1) ;
      add_wagon_to_train("T04","4W-CV-load", 1) ;
      add_wagon_to_train("T04","4W-CV-mty", 1) ;
      add_wagon_to_train("T04","4W-CW2-LD", 1) ;
      add_wagon_to_train("T04","4W-CW2", 1) ;
      add_wagon_to_train("T04","4W-GSV2-Pigs", 1) ;
      add_wagon_to_train("T04","4W-GSV2-Sheep", 1) ;
      add_wagon_to_train("T04","4W-GSV2", 1) ;
      add_wagon_to_train("T04","4W-GSV2", 1) ;
      add_wagon_to_train("T04","4W-LV-load", 1) ;
      add_wagon_to_train("T04","4W-LV-mty", 1) ;
      add_wagon_to_train("T04","4W-HG-Brake-Van", 1) ;
      position_train("T04",415,1,0) ;
#endif

#if 1
      add_new_train("T05") ;
      add_wagon_to_train("T05","1905-B319", 1) ;
      add_wagon_to_train("T05","1905-B319-t", 1) ;
      add_wagon_to_train("T05","1905-P708", 1) ;
      add_wagon_to_train("T05","1905-P708-t", 1) ;

      add_wagon_to_train("T05","JWW_Htype_E", 1) ;
      add_wagon_to_train("T05","JWW_Htype_L", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      add_wagon_to_train("T05","4W-CV-load", 1) ;
      add_wagon_to_train("T05","4W-CV-load", 1) ;
      add_wagon_to_train("T05","4W-CV-load", 1) ;
      add_wagon_to_train("T05","4W-CV-load", 1) ;
      position_train("T05",242,2,0) ;
#endif

#if 1
      add_new_train("T06") ;
      add_wagon_to_train("T06","1905-S654", 1) ;
      add_wagon_to_train("T06","1905-C144", 1) ;
      add_wagon_to_train("T06","1905-C144-t", 1) ;
      add_wagon_to_train("T06","4W-CV-load", 1) ;
      add_wagon_to_train("T06","4W-CV-load", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb1LD", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb1MT", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb2LD", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb2MT", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb3LD", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb3MT", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb4LD", 1) ;
      add_wagon_to_train("T06","NI4wHopUnb4MT", 1) ;
/*
 *  These wagons cause a segmentation fault
 */
//      add_wagon_to_train("T05","WVR-4W-AcidTank-9-LD", 1) ;
//      add_wagon_to_train("T05","WVR-4W-AcidTank-9", 1) ;
      position_train("T06",51,12,0) ;
#endif

#if 1
      add_new_train("T07") ;
      add_wagon_to_train("T07","1905-T524", 1) ;
      add_wagon_to_train("T07","1905-T524-t", 1) ;
      add_wagon_to_train("T07","4W-CV-load", 1) ;
      add_wagon_to_train("T07","1905-AD-Sleeper", 1) ;
      add_wagon_to_train("T07","1905-AB-Diner", 1) ;
      add_wagon_to_train("T07","1905-CBC", 1) ;
      add_wagon_to_train("T07","1905-CBC", 1) ;
      add_wagon_to_train("T07","1905-CBC", 1) ;
      add_wagon_to_train("T07","1905-HO-JWW", 1) ;
      position_train("T07",485,5,0) ;
#endif

      mark_textures()              ;
      load_needed_display_lists()  ;

      zr_clock_gettime(zr_clock_1) ;
      update_trains(zr_clock_1[1]) ;  // Update using monotonic time
      zr_clock_gettime(zr_clock_1) ;
      update_trains(zr_clock_1[1]) ;  // Update using monotonic time

      if(ip)print_train_data() ;

      return 0  ;
}

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
      for(raw = rawwagonlist_beg; raw==NULL; raw = raw->next){
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
      w->train      = NULL  ;
      w->name       = (char *)malloc(strlen(name)*sizeof(char)) ;
      strcpy(w->name,name)  ;
      w->index      = 0     ;
      n = strlen(raw->file) ;
      w->is_engine  = !strcmp(".eng",&(raw->file[n-4]));
      w->train_dir  = 1     ;     // Initialise as 'true'
      w->n_travel   = 1     ;     // NOTE Change for multi-bogie wagons
      w->dist_front = 10.0  ;     // NOTE Change once sd file read
      w->dist_back  = 10.0  ;
      w->wheel_angle       = 0.0 ;  //  Angle of wheels
      w->driverwheel_angle = 0.0 ;  //  Changes when train moves
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
      c->motor      = (wagon->is_engine) ? wagon : NULL ;

      return c ;
}

/*
 *==============================================================================
 *  Routine to add one wagon onto the end of a consist
 *==============================================================================
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
        if(new->is_engine) c->motor = new ;
      }
      return 0;
}

/*
 *==============================================================================
 *  Routine to intiialise a train.
 *  This add a train to the train list, gives it a name
 *  and initialises the variables
 *==============================================================================
 */

int  add_new_train(char *name){

int ip   = 0 ;  // Debug
TrainNode *t ;
char      *my_name = "add_new_train" ;

      t = (TrainNode *)malloc(sizeof(TrainNode)) ;
      t->name  = strdup(name) ;
      t->speed = 0.0  ;
      t->next  = NULL ;
      t->motor = NULL ;
      t->first = NULL ;
      t->last  = NULL ;

      if(NULL == trainlist_beg) trainlist_beg = t ;
      if(NULL != trainlist_end){
        trainlist_end->next = t ;
      }
      trainlist_end = t       ;
      if(NULL == player_train) player_train = t ;
      if(ip){
        printf("  Routine %s : train beg = %s\n",my_name,trainlist_beg->name) ;
        printf("  Routine %s : train end = %s\n",my_name,trainlist_end->name) ;
      }
      return 0 ;
}
/*
 *==============================================================================
 *==============================================================================
 */

 int   add_wagon_to_train(char *train, char *wagon, int idirn){

  int           ip = 0     ;  //  Debug
  TrainNode     *t  = NULL ;
  WagonNode     *w  = NULL ;
  RawWagonNode  *rw = NULL ;
  TravellerNode *tv = NULL ;
  char         *my_name = "add_wagon_to_train" ;

/*
 *==============================================================================
 *  Find train in list of TrainNodes
 *==============================================================================
 */
      if(ip)printf(" Routine %s.  Train = %s,  wagon = %s\n",my_name,train,wagon) ;
      for(t = trainlist_beg; t != NULL ; t=t->next){
        if(ip)printf(" Routine %s.  AA Train = %s\n",my_name,t->name) ;
      }
      for(t = trainlist_beg; t != NULL ; t=t->next){
        if(ip)printf(" Routine %s.  BB Train = %s\n",my_name,t->name) ;
        if(!strcmp(t->name,train)) break ;
      }
      if(t==NULL){
        printf(" Routine %s.\n  Unable to fine train with name %s\n",
               my_name,train) ;
        exit(1) ;
      }
/*
 *==============================================================================
 *  Find wagon in list of raw wagons
 *==============================================================================
 */
      for(rw = rawwagonlist_beg; rw != NULL; rw = rw->next){
        if(ip)printf(" Routine %s, wagon name = %s :: %s\n",my_name,
                    rw->name, rw->shape->name) ;
        if(!strcmp(rw->name,wagon)) break ;
      }
      if(rw==NULL){
        printf(" Routine %s.\n  Unable to find wagon with name %s\n",
               my_name,wagon) ;
        exit(1) ;
      }
/*
 *==============================================================================
 *  Create new WagonNode and initialise
 *==============================================================================
 */
      w  = (WagonNode *)malloc(sizeof(WagonNode)) ;
      tv = (TravellerNode *)malloc(sizeof(TravellerNode)) ;

      w->name        = wagon ;
      w->train       = t     ;
      w->index       = 0     ;
      w->is_engine   = rw->is_engine    ;
      w->train_dir   = idirn ;
      w->n_travel    = 1     ;
      w->dist_front  = rw->length*0.5  ;
      w->dist_back   = w->dist_front  ;
      w->wheel_angle = (360.0*rand())/RAND_MAX ;
      w->driverwheel_angle = (360.0*rand())/RAND_MAX ;
      w->traveller   = tv ;
      w->shape       = rw->shape ;
      w->raw_wagon   = rw        ;
      w->next = NULL ;
      w->prev = NULL ;

      tv->shape  = w->shape ;
      tv->wagon  = w        ;
      tv->shape->needed = 1 ;
      tv->shape->loaded = 0 ;


      if(t->first == NULL) t->first = w ;
      if(t->last != NULL){
        t->last->next = w ;
        w->prev       = t->last ;
      }
      t->last = w ;
      if(ip)printf("*** Routine %s.  Added wagon %s to train %s\n",my_name,w->name,t->name);

/*
 *==============================================================================
 *  Set train main engine if appropiate
 *==============================================================================
 */
      if(w->is_engine && t->motor == NULL)t->motor = w ;

      return 0 ;
}

int position_train(char *train, int itrack, int ivector, int idirect){

  int           ip  = 0    ;  // Debug
  TrainNode     *t  = NULL ;
  WagonNode     *w  = NULL ;
  RawWagonNode  *rw = NULL ;
  TravellerNode *tv = NULL ;
  double       dist = 0.0  ;
  char         *my_name = "position_train" ;

      if(ip)printf("  Routine %s :: AA\n",my_name) ;
      fflush(NULL) ;
/*
 *  Find train in TrainNode list
 */
      for(t = trainlist_beg; t != NULL ; t=t->next){
        if(!strcmp(t->name,train)) break ;
      }
      if(ip)printf("  Routine %s :: BB\n",my_name) ;
      fflush(NULL) ;
      if(t==NULL){
        printf(" Routine %s.\n  Unable to fine train with name %s\n",
               my_name,train) ;
        exit(1) ;
      }

      fflush(NULL) ;
/*
 *  Loop over wagons
 */
      for(w = t->last ; w != NULL ; w=w->prev){
        if(ip)printf("  Routine %s :: train = %s, dist = %f, wagon = %s\n",
                                      my_name,t->name,dist,w->name) ;
        fflush(NULL) ;
        tv = w->traveller ;
        trv_initv(tv, itrack, ivector, idirect) ;
        trv_move(tv, dist) ;    //  Initialise tv - even if dist == 0.
        if(w->prev != NULL){
          dist += w->dist_front + w->prev->dist_back ;
        }
      }
      l_disp1 = 1 ;  // Switch on printing DEBUG
      return 0 ;
}

int  copy_traveller_position(TravellerNode *tv, TravellerNode *tv0) {

      tv->tn = tv0->tn ;
      tv->vn = tv0->vn ;
      tv->x  = tv0->x  ;
      tv->y  = tv0->y  ;
      tv->z  = tv0->z  ;
      tv->ang_deg  = tv0->ang_deg  ;
      tv->position = tv0->position ;
      tv->itrack   = tv0->itrack ;
      tv->ivector  = tv0->ivector ;
      tv->idirect  = tv0->idirect ;

      return 0 ;
}


int update_trains(struct timespec time_now){

  int         ip = 0 ;
  int         it = 1 ;
  double      x0, y0, z0,
              x1, y1, z1,
              a, b, c  ;
  TrkVectorNode *vn ;

  int       iret = 0 ;  //  Return code
  TrainNode *t       ;
  WagonNode *w       ;
  TravellerNode *tv      ;
  TravellerNode *tv0     ;
  double    time         ;
  double    dist_moved   ;
  double    dist         ;
  static double time_last = -1.0   ;
  char      *my_name = "update_trains" ;

      if(ip)printf("  Enter %s\n",my_name) ;

      time = (time_now.tv_sec - run_clock0.tv_sec)
           + (time_now.tv_nsec - run_clock0.tv_nsec)*0.000000001 ;
      if(time_last<0.0){
        if(ip)printf("  Routine %s, time = %f, time_last = %f\n",
                my_name,time,time_last) ;
        time_last = time ;
        return 0         ;
      }

      if(ip)print_train_data() ;

/*
 *==============================================================================
 *  Loop over trains
 *==============================================================================
 */
      for(t = trainlist_beg; t != NULL ; t=t->next){
        if(ip)printf("  Routine %s :: train = %s, speed = %f\n",
                                                 my_name,t->name,t->speed) ;
        if(0.0 == t->speed)continue   ;

        iret = check_train_movement(t) ;  //  Check for obstructions
        if(iret)continue               ;  //  Train stopped by obstruction

        if(ip)printf("  Routine %s :: AA :: train = %s\n",my_name,t->name) ;
        dist_moved = t->speed*(time - time_last) ;
        w  = t->last ;
        if(ip)printf("  Routine %s :: wagon = %s\n",my_name,w->name) ;
        tv0 = w->traveller ;
        iret = trv_move(tv0,dist_moved) ;
/*
 *  Check if end of line or attempt to enter junction from the wrong direction
 */
        if(iret){
          t->speed = 0.0 ;
          if(iret == 1) {
            printf("  Train %s derailed at junction.  Switch incorrectly set.\n",
                                                                   t->name) ;
          }else{
            printf("  Train %s crashed into buffers at end of line\n",t->name) ;
          }
        }
        camera_changed = 1 ;
        if(w->train_dir){
          w->wheel_angle = w->wheel_angle +
                           degree*dist_moved*w->raw_wagon->inv_wheelradius ;
        }else{
          w->wheel_angle = w->wheel_angle -
                           degree*dist_moved*w->raw_wagon->inv_wheelradius ;
        }
        w->wheel_angle = fmod((double)w->wheel_angle, (double)360.0) ;
        if(w->wheel_angle<0.0)w->wheel_angle += 360.0 ;
        if(ip){
          printf("  Routine %s AA1 :: first  = %p, last = %p\n",
                                  my_name,(void *)t->first,(void *)t->last) ;
          printf("  Routine %s AA2 :: wagon  = %p, prev = %p\n",
                                         my_name,(void *)w,(void *)w->prev) ;
        }
        if(ip)printf(" Wagon = %s,  Angle = %f\n",w->name,w->wheel_angle ) ;

        if(ip)trv_print(w->traveller) ;
#if 0
//     Model position
        tv = w->traveller ;
        vn = tv->vn ;
        a = degree*vn->a_east_x   ;
        b = degree*vn->a_height_y ;
        c = degree*vn->a_north_z  ;
/*
 *   Convert from MSTS location to coordinates used for graphics
 */
          global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                         vn->tile_east_x, vn->tile_north_z,
                         vn->east_x , vn->north_z, vn->height_y+rail_height,
                         &x0, &y0, &z0) ;
          printf("\n") ;
          printf("  AA %f  %f  %f :: %f %f %f\n",x0,y0,z0,tv->x,tv->y,tv->z)
#endif
/*
 *==============================================================================
 *  Loop over wagon - starting with last and moving forward
 *==============================================================================
 */
        dist = 0.0 ;
        if(w->prev!= NULL){
          for(w=w->prev; w!=NULL; w=w->prev){
            if(ip)printf("  Routine %s :: wagon = %s\n",my_name,w->name) ;
#if 1
            dist = dist_moved + (w->dist_back + w->next->dist_front) ;
            if(ip){
              printf("  Routine %s BB1 :: train = %s, wagon = %s\n",
                                                   my_name,t->name,w->name) ;
              printf("  Routine %s BB2 :: wagon = %p, next = %p\n",
                                         my_name,(void *)w,(void *)w->next) ;
              printf("  Routine %s BB3 :: train = %s, wagon = %s, dist = %f %f %f\n",
                  my_name,t->name,w->name, w->dist_back, w->next->dist_front, dist) ;
            }
            tv = w->traveller ;
            dist = (w->dist_back + w->next->dist_front) ;
            copy_traveller_position(tv,tv0) ;
            iret = trv_move(tv, dist) ;
#else
            dist = dist_moved + (w->dist_back + w->next->dist_front) ;
            iret = trv_move(tv,dist_moved) ;
#endif
            if(ip)trv_print(w->traveller) ;
#if 0
//     Model position
            tv = w->traveller ;
            vn = tv->vn ;
            a = degree*vn->a_east_x   ;
            b = degree*vn->a_height_y ;
            c = degree*vn->a_north_z  ;
/*
 *   Convert from MSTS location to coordinates used for graphics
 */
          global2local(tile_x0, tile_y0, tile_h0, tile_size, plot_scale,
                         vn->tile_east_x, vn->tile_north_z,
                         vn->east_x , vn->north_z, vn->height_y+rail_height,
                         &x1, &y1, &z1) ;
          printf("  BB %f  %f  %f :: %f %f %f\n",x1,y1,z1,tv->x,tv->y,tv->z)
#endif
/*
 *  Check if end of line or attempt to enter junction from the wrong direction
 */
            if(iret || junction_error){
              t->speed = 0.0 ;
              if(iret)printf("  Train %s reached end of line\n",t->name) ;
              if(junction_error)printf("  Train %s derailed at junction.  Switch incorrectly set.\n",
                                                                            t->name) ;
            }
            tv0 = tv ;
            if(ip){
              printf(" BB Wagon = %s,  DistMoved = %f\n",w->name,dist_moved ) ;
              printf(" BB Wagon = %s,  Radius    = %f\n",w->name,w->raw_wagon->wheelradius ) ;
              printf(" BB Wagon = %s,  InvRadius = %f\n",w->name,w->raw_wagon->inv_wheelradius ) ;
              printf(" BB Wagon = %s,  Angle     = %f\n",w->name,w->wheel_angle ) ;
            }
            if(w->train_dir){
              w->wheel_angle += degree*dist_moved*w->raw_wagon->inv_wheelradius;
            }else{
              w->wheel_angle -= degree*dist_moved*w->raw_wagon->inv_wheelradius;
            }
            w->wheel_angle = fmod((double)w->wheel_angle, (double)360.0) ;
            if(w->wheel_angle<0.0)w->wheel_angle += 360.0 ;
            if(w->wheel_angle<0.0 || w->wheel_angle>360.0)printf(" Train.c :: wheel angle = %f\n",
              w->wheel_angle) ;
          }
        }
      }
      time_last = time ;
      if(ip)printf("  EXIT %s\n",my_name) ;
      return 0 ;
}

/*
 *   Routine to check whether a train is due to hit the end of the
 *   line or to enter a junction from the closed branch.
 *   In both cases the train should derail - however for the moment
 *   the train is only stopped and can be reversed.
 */

int check_train_movement(TrainNode *t){

  int           iret  ;
  double        dist  ;
  WagonNode     *w    ;
  TravellerNode *tn   ;
  TravellerNode tc    ;  //  Tempory traveller
  char          *my_name = "check_wagon_movement" ;

      if(t->speed == 0.0) return 0 ;
      junction_error = 0 ;
      if(t->speed > 0.0){
        w = t->first           ;    //  Front of train
        if(w == NULL) return 0 ;
        tn = w->traveller      ;
        tc = *tn               ;    //  Make temporary traveller
        dist = 1.0 + 0.1*t->speed + 0.5*w->raw_wagon->length ;
        iret = trv_move(&tc,dist) ; //  Move temporary traveller
      }else{
        w = t->last            ;    //  Back of train
        if(w == NULL) return 0 ;
        tn = w->traveller      ;
        tc = *tn               ;     //  Make temporary traveller
        dist = 1.0 + 0.1*t->speed + 0.5*w->raw_wagon->length ;
        iret = trv_move(&tc,-dist) ; //  Move temporary traveller
      }
      if(iret || junction_error){    //  Check for errors
        t->speed = 0.0 ;             //  Stop train
        if(iret)printf("  Train %s reached end of line\n",t->name) ;
        if(junction_error){
          printf("  Train %s derailed at junction.  Switch incorrectly set.\n",
                                                                      t->name) ;
          iret           = 1 ;
          junction_error = 0 ;
        }
      }

      return iret ;
}

int print_train_data(void){
  int       ip = 0 ;
  TrainNode *t     ;
  WagonNode *w     ;
  char      *my_name = "print_train_data" ;

      printf(" Enter %s\n",my_name) ;
      for(t = trainlist_beg; t != NULL ; t=t->next){
        printf("  Train = %s, speed = %f\n",t->name,t->speed) ;
        if(t->motor!= NULL)
          printf("              Motor = %s\n",t->motor->name) ;
        for(w = t->first; w != NULL; w=w->next){
          printf("    Wagon = %s,  Distances = %f,  %f\n",
                     w->name, w->dist_front, w->dist_back) ;
          printf("          prev = %p,  w = %p, next = %p\n",
                 (void *)w->prev, (void *)w, (void *)w->next) ;
        }
      }
      printf(" Exit  %s\n",my_name) ;

      return 0 ;
}
/*
 *  List wagons to file wagonlist.txt
 */

int print_wagon_data(void){

  int          ip = 0 ;
  RawWagonNode *rw    ;
  char         *my_name = "print_wagon_data" ;

      for(rw=rawwagonlist_beg; rw!=NULL; rw=rw->next){
        printf("  Wagon: %-25s :: Type: %-8s :: %s\n",rw->name,rw->type,rw->full_name) ;
      }
      return 0;
}

int print_wagon_data_to_file(char *filename){

  int          ip = 0,
               i      ;
  FILE         *fp    ;
  RawWagonNode *rw    ;
  char         *my_name = "print_wagon_data" ;

      fp = fopen(filename,"w") ;
      fprintf(fp,"\n  List of engine and wagon files generated by ZR routine \"%s()\".\n\n",my_name);
      for(i=0; i<4; i++){
        for(rw=rawwagonlist_beg; rw!=NULL; rw=rw->next){
          if((i == 0 && !strcmp(rw->type,"Engine")   ) ||
             (i == 1 && !strcmp(rw->type,"Tender")   ) ||
             (i == 2 && !strcmp(rw->type,"Carriage") ) ||
             (i == 3 && !strcmp(rw->type,"Freight")   ) )
             fprintf(fp,"  Wagon: %-28s :: File: %-10s :: Type: %-8s :: %s\n",rw->name,rw->s_file, rw->type,rw->full_name) ;
        }
      }
      fclose(fp) ;
      return 0;
}
