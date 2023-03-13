/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  train_events.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines for Processing train events (split_trains, join_trains, ...)
 *
 *==============================================================================
 */

static int new_train_index = 100 ;  // Used to generate new train names

void  split_train_at_wagon(TrainNode *train, int wagon_number){

  int        ip = 0 ;
  int        n, m ;
  TrainNode  *new_train, new_train_node, *t ;
  WagonNode  *wagon, *last_wagon ;
  char       *my_name = "split_train_at_wagon" ;

      if(ip)printf("  Routine %s.  Train = %s, wagon = %i. n_wagons = %i\n",
                   my_name, train->name, wagon_number, train->n_wagons) ;
/*
 *   Generate new train
 */
      new_train = (TrainNode *)malloc(sizeof(TrainNode)) ;

      trainlist_end->next = new_train ;
      trainlist_end       = new_train ;
      new_train->next     = NULL ;
      new_train->speed    = 0.0  ;
      new_train->motor    = NULL ;
      new_train->last     = train->last ;
      new_train->back     = train->back ;
      for(n=2, m=10; m<new_train_index; n++, m *=10 ) ;
      new_train->name = (char *)malloc(n);
      sprintf(new_train->name,"%i",new_train_index++) ;

      if(ip)printf("  New train name = ::%s::\n",new_train->name) ;

      new_train->n_wagons = train->n_wagons - wagon_number - 1 ;
      train->n_wagons     = wagon_number + 1 ;
      f9_data.last_train = NULL ;

      if(ip)printf(" ZZ AA\n") ;
/*
 *  Determine end of 'train' and start of 'new train'.
 */
      last_wagon = NULL ;
      for(wagon = train->first, n=0; wagon!=NULL && n<=wagon_number;
                                     wagon=wagon->next, n++ ){
        if(ip)printf(" ZZ BB\n") ;
        last_wagon= wagon ;
      }
      if(!last_wagon){
        if(ip)printf("  Routine %s.  ERROR.  Last wagon not found",my_name) ;
        return ;
      }
      train->last = last_wagon ;
      new_train->first = wagon ;

      train->last->next      = NULL ;
      new_train->first->prev = NULL ;
      if(ip)printf(" ZZ CC\n") ;
/*
 *  Initialise front of new train
 */
      new_train->front = *(new_train->first->traveller) ;
      trv_move(&(new_train->front),
               0.5*new_train->first->raw_wagon->length) ;
      if(ip)printf(" ZZ DD\n") ;
/*
 *  Initialise back of original train
 */
      train->back = *(train->last->traveller) ;
      trv_move(&(train->back),
               -0.5*train->last->raw_wagon->length) ;
      if(ip)printf(" ZZ EE\n") ;
/*
 *   Initialise new train engine
 */
      new_train->motor = NULL ;
      for(wagon = new_train->first; wagon!=NULL; wagon = wagon->next){
        if(ip)printf("  New train wagons :  %s : %i : %p\n",
                    wagon->name, wagon->is_engine, (void *)new_train->motor) ;
        if(wagon->is_engine){
           if(ip)printf(" ZZ FF\n") ;
          new_train->motor = wagon ;
          break ;
        }
      }
/*
 *   Initialise old train engine
 */
      train->motor = NULL ;
      for(wagon = train->first; wagon!=NULL; wagon = wagon->next){
        if(wagon->is_engine){
          if(ip)printf(" ZZ FF\n") ;
          train->motor = wagon ;
          break ;
        }
      }

      if(ip){
        if(ip)printf(" ZZ GG\n") ;
        for(t=trainlist_beg; t!=NULL; t=t->next){
          printf("  Train %s, n_wagons = %i, motor = %p\n",
                t->name, t->n_wagons, (void *)t->motor) ;
        }
      }

      return ;
}

void  trigger_wagon_event(TrainNode *train, int wagon_number){

  char  *my_name = "trigger_wagon_event" ;

      printf("  Routine %s.  Train = %s, wagon = %i\n",
                                        my_name, train->name, wagon_number) ;

      return ;
}



void crash_train(TrainNode *t, int i_crash){

      t->speed = 0.0 ;

      if(i_crash == 1){
        printf("  Train %s crashed into buffers at end of line\n",t->name) ;
      }else if(i_crash == 2){
        printf("  Train %s derailed trying ot enter junction from wrong branch\n",
                                                                  t->name) ;
      }else{
        printf("  Train %s derailed from unknown cause.  Crash type = %i\n",
                                                         t->name, i_crash) ;
      }
      return ;
}

void crash_trains(TrainNode *t0, TrainNode *t1, int i_collide){

      t0->speed = t1->speed = 0.0 ;
      printf("  Train %s crashed at speed into %s of train %s\n",
                 t0->name, (i_collide>0)? "front" : "back",t1->name) ;
      return ;
}

void reverse_train(TrainNode *t0){

  WagonNode     *w0, *w1 ;
  TravellerNode *tv0, tv ;
/*
 *   Swap direction of each wagon in the train
 */
      for(w0 = t0->first ; w0 != NULL ; w0 = w1){
        w1       = w0->next ;
        w0->next = w0->prev ;
        w0->prev = w1 ;
        tv0      = w0->traveller ;
        tv0->idirect  = tv0->idirect  ? 0 : 1 ;
        w0->train_dir = w0->train_dir ? 0 : 1 ;
      }
/*
 *   Swap train data
 */
      w1 = t0->first ;
      t0->first = t0->last ;
      t0->last  = w1 ;
      tv = t0->front ;
      t0->front = t0->back ;
      t0->back  = tv ;
      t0->speed = -t0->speed ;   //  Should be zero.

      return ;
}

void join_trains(TrainNode *t0, TrainNode *t1, int i_collide) {

  int ip = 0 ;
  int iend0 = t0->speed > 0.0 ;
  int iend1 = i_collide > 0   ;
  WagonNode *w0, *w1 ;
  TrainNode *tt, *tt_last     ;
  char      *my_name = "join_trains" ;

      t0->speed = t1->speed = 0.0 ;
      printf("  Train %s joined %s of train %s\n",
                 t0->name, (i_collide>0)? "front" : "back",t1->name) ;
      f9_data.last_train = NULL ;
/*
 * Couple train t1 to train t0
 */
      if(ip){
        printf(" AA  Train t0\n");
        printf(" AA  t0->next  = %p\n",(void *)t0->next);
        printf(" AA  t0->name  = %s\n",        t0->name);
        printf(" AA  t0->first = %p\n",(void *)t0->first);
        printf(" AA  t0->last  = %p\n",(void *)t0->last);
        printf(" AA  Train t1\n");
        printf(" AA  t1->next  = %p\n",(void *)t1->next);
        printf(" AA  t1->name  = %s\n",        t1->name);
        printf(" AA  t1->first = %p\n",(void *)t1->first);
        printf(" AA  t1->last  = %p\n",(void *)t1->last);

        for(w0 = t1->last; w0 != NULL; w0 = w0->prev){
          printf(" BB  %p, prev = %p, next = %p, name = %s\n",
                (void *)w0,(void *)w0->prev, (void *)w0->next, w0->name) ;
        }
        for(w0 = t0->last; w0 != NULL; w0 = w0->prev){
          printf(" CC  %p, prev = %p, next = %p, name = %s\n",
                (void *)w0, (void *)w0->prev, (void *)w0->next, w0->name) ;
        }
      }

      if(!iend0 && iend1){
        w0 = t0->last  ;
        w1 = t1->first ;
        w0->next = w1 ;
        w1->prev = w0 ;

        t0->last = t1->last ;
        t0->back = t1->back ;
      }else if(iend0 && !iend1){
        w0 = t0->first  ;
        w1 = t1->last ;
        w0->prev = w1 ;
        w1->next = w0 ;

        t0->first = t1->first ;
        t0->front = t1->front ;
      }else if(!iend0 && !iend1){
        reverse_train(t1) ;
        w0 = t0->last  ;
        w1 = t1->first ;
        w0->next = w1 ;
        w1->prev = w0 ;

        t0->last = t1->last ;
        t0->back = t1->back ;
      }else{     // iend0 && iend1
        reverse_train(t1) ;
        w0 = t0->first  ;
        w1 = t1->last ;
        w0->prev = w1 ;
        w1->next = w0 ;

        t0->first = t1->first ;
        t0->front = t1->front ;
      }

      t0->n_wagons += t1->n_wagons ;

      if(ip){
        printf(" DD  Train t0\n");
        printf(" DD  t0->next  = %p\n",(void *)t0->next);
        printf(" DD  t0->name  = %s\n",        t0->name);
        printf(" DD  t0->first = %p\n",(void *)t0->first);
        printf(" DD  t0->last  = %p\n",(void *)t0->last);
/*
 *  List wagons
 */
        for(w0 = t1->last; w0 != NULL; w0 = w0->prev){
          printf(" EE  %p, prev = %p, next = %p, name = %s\n",
                (void *)w0, (void *)w0->prev, (void *)w0->next, w0->name) ;
        }
      }
/*
 *  Before freeing the t1 data structure, it needs to be removed from
 *  the list of train structures.
 */
      if(t1 == trainlist_beg){
        trainlist_beg = t1->next ;
      }else{
        tt_last = trainlist_beg ;
        for(tt = tt_last->next; tt!= NULL; tt = tt->next){
          if(tt == t1){
            tt_last->next = tt->next ;
            if(trainlist_end == tt)trainlist_end = tt_last ;
            break ;
          }
          tt_last = tt ;
        }
      }
      free(t1) ;
      if(ip){
        printf(" ZZ  Routine %s\n",my_name) ;
        for(tt=trainlist_beg; tt!=NULL; tt=tt->next){
          printf("  List new trains: %i %s\n",(tt==t0),tt->name) ;
        }
      }

      return ;
}

