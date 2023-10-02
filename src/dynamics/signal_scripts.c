/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  signal_scripts.c
 *
 *   This file contains routines used to process the signal scripts
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *==============================================================================
 */

/*
 *  external variables
 *  state
 *  draw_state
 *  enabled     ::  Signal is enabled
 */
static SignalDB *signal0 ;
static int       i_stack = 0   ;
static const int n_stack = 100 ;
static double    sc_stack[100] ;
static double    sc_next_state ;       //  Used by scripts as tempory storage
static double    has_number_plate ;    //  Used by scripts as tempory storage
static double    has_gradient_plate ;  //  Used by scripts as tempory storage
static char      *sc_name      ;

int find_next_signal(SignalDB *signal, int itype) ;

#include"signal_script_functions.c"
#include"signal_script_functions2.c"

int process_signal_statement(nodeType *node);
int print_sigscr_tree(nodeType *p);


int  process_signal_script(SignalDB *signal){

int        ip = 0 ;
static int ierr = 0 ;
nodeType   *sig_script, *n1, *n2, *n3 ;
char       *my_name="process_signal_script" ;

      ip = (n_sig1 == signal->uid)  ;

      if(ip){
        printf("\n**********************************************************\n") ;
        printf(" Routine %s.  Process signal %i, %s\n",my_name,
                                     signal->uid, signal->sig_type->name) ;
        printf("**********************************************************\n\n") ;
        fflush(NULL) ;
      }
      signal0 = signal ;

      sig_script = signal->sig_script ;
/*
 *  Limit error printing
 */
      if(!sig_script){
        if(ierr++ < 4){
          printf("  Routine %s.  Signal script missing for signal %i :: %s :: %s\n",
                my_name, signal->uid, signal->shape_name, signal->type_name) ;
        }
        return 0 ;
      }

      n1 = sig_script ;

//      printf("   ====  sTree = %p\n",(void *)sTree) ;
//      if(ip)print_sigscr_tree(sTree) ;  // Everything
      if(ip)print_sigscr_tree(n1) ;

      for(;;){
        if(ip){
          printf("  n1 = %p",(void *) n1) ;
          if(n1)printf(" : n1->type = %i :: %i", n1->type, typeOpr) ;
          if(n1->type == typeOpr)printf(" : n1->opr.oper %i : SC_SCRIPT %i : SC_DEF_LIST %i : SC_STMT_LIST %i ",
                         n1->opr.oper, SC_SCRIPT, SC_DEF_LIST, SC_STMT_LIST) ;
          printf("\n") ;
        }
        if(n1->type == typeOpr){
          switch(n1->opr.oper){
            case SC_SCRIPT:
              n1 = n1->opr.op[2] ;
              break ;
            case SC_DEF_LIST:
              n1 = n1->opr.op[1] ;
              break ;
            case SC_STMT_LIST:
              process_signal_statement(n1->opr.op[0]);
              n1 = n1->opr.op[1] ;
              if(!n1) break ;
              if(n1->type == typeOpr && n1->opr.oper == SC_STMT_LIST) break;
              process_signal_statement(n1);
              n1 = NULL ;
              break ;
            default:
              process_signal_statement(n1) ;
              n1 = NULL ;
          }
        }
        if(!n1) break ;
      }
      return 0 ;
}

/*
 *  Routine to process arithmetic and logical statements
 *  This is the core of the 'script run time ststem'.  It uses
 *  re-entrant code and a switch statement to process each set
 *  of statements.
 *
 *  The use of a switch statement would be inefficient if compiled
 *  as a series of "if(){...}else if{...} ..." but the gcc compiler
 *  and most other compilers will change it into a jump/branch table
 *  indexed by the labels, if the labels are in some sense packed.
 *  This works when the labels are a series of close enums - in this
 *  case SC_OR, SC_AND, etc - and is most efficient when there are
 *  no gaps in the series.
 *
 *  See http://www.jauu.net/2010/06/15/gcc-generated-switch-jump-tables/
 */

int  process_signal_statement(nodeType *node){

  int  ip = 0 ;
  int n1, n2 ;
  double x, y1, y2 ;
  float  f ;
  char *name1, *name2 ;
  char *my_name="process_signal_statement" ;


//      ip = strcmp("NSW_SemLQHome", signal0->sig_type->name) ;
//      ip = (67 == signal0->uid) ;
//      ip = (87 == signal0->uid) ;
      ip = (n_sig1 == signal0->uid)  ;

      if(ip)printf(" Routine %s.  Uid = %i, [%i].  Node = %s\n",my_name,
                                     signal0->uid, n_sig1, signal0->sig_type->name) ;
//      if(ip)print_sigscr_tree(node) ;
//      if(!ip)return 0 ;

//      if(ip)printf(" AA Type = %i %i\n",node->type, typeOpr) ;
      if(node->type == typeOpr){
        if(ip)printf("  type = typeOpr.  Operation = %i '%s'\n",
                                  node->opr.oper, sigscr_token_string(node->opr.oper)) ;
        switch(node->opr.oper) {
          case SC_STMT_LIST :
            process_signal_statement(node->opr.op[0]) ;
            process_signal_statement(node->opr.op[1]) ;
            break ;

          case SC_ASSIGN :
            process_signal_statement(node->opr.op[0]) ;  //  Access name
            name1 = sc_name ;
            process_signal_statement(node->opr.op[1]) ;  //  Place value on stack
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ Assign  %i : %s  =  %f\n",i_stack,name1,x) ;
            }
            sc_save(name1) ;           //  Save stack variable
            break ;

          case SC_FUNCTION :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            name1 = sc_name ;
            if(node->opr.nops>1){
              process_signal_statement(node->opr.op[1]) ;  // Place prameter on stack
            }else{
              sc_push_stack(0.0) ;                  // Place dummy parameter on stack
            }
            sc_func(name1) ;                  // Call function, save to stack

            x = sc_pop_stack() ;
            sc_push_stack(x)   ;
            if(ip)printf("+++ Function  %s  =  %f\n",name1,x) ;
            break ;

          case SC_EXPR_LIST :
            process_signal_statement(node->opr.op[0])   ;  //  Place result on stack
            if(node->opr.nops>1){
              process_signal_statement(node->opr.op[1]) ;  //  Place result on stack
            }
            break ;

          case SC_VARIABLE :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            sc_var(sc_name) ;                   // Load variable to stack

            x = sc_pop_stack() ;
            sc_push_stack(x)   ;
            if(ip)printf("+++ Load  %i : %s  =  %f\n",i_stack,sc_name,x) ;
            break ;

          case SC_IVALUE :
            sscanf(sc_name,"%f",&f) ;
            sc_push_stack((double)f) ;
            if(ip)printf("+++  Ivalue :: sc name = %s\n",sc_name) ;
            break ;

          case SC_IF :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            x = sc_pop_stack() ;
            if(ip)printf("+++ IF statement testing :: %f\n",x) ;
            if(x>0.0){
              process_signal_statement(node->opr.op[1]) ;  // Function name
            }else if(node->opr.nops>2){
              process_signal_statement(node->opr.op[2]) ;  // Function name
            }
            break ;

          case SC_OR :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            x = sc_pop_stack() ;
            if(x==0.0){
              process_signal_statement(node->opr.op[1]) ;  // Function name
              x = sc_pop_stack() ;
            }
            sc_push_stack(x) ;

            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ OR result  =  %f\n",x) ;
            }
            break ;

          case SC_AND :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            x = sc_pop_stack() ;
            if(x > 0.0){
              process_signal_statement(node->opr.op[1]) ;  // Function name
              x = sc_pop_stack() ;
            }
            sc_push_stack(x) ;

            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ AND result  =  %f\n",x) ;
            }
            break ;

          case SC_NOT :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            x = sc_pop_stack() ;
            if(x==0.0){
              x = 1.0 ;
            }else{
              x = 0.0 ;
            }
            sc_push_stack(x) ;

            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ NOT result  =  %f\n",x) ;
            }
            break ;

          case SC_EQ :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            y1 = nint(sc_pop_stack()) ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            y2 = nint(sc_pop_stack()) ;
            if(y1 == y2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ IEQ result  =  %f\n",x) ;
            }
            break ;

          case SC_NE :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            y1 = nint(sc_pop_stack()) ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            y2 = nint(sc_pop_stack()) ;
            if(y1 != y2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ INE result  =  %f\n",x) ;
            }
            break ;

          case SC_IEQ :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            n1 = nint(sc_pop_stack()) ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            n2 = nint(sc_pop_stack()) ;
            if(n1 == n2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ IEQ result  =  %f\n",x) ;
            }
            break ;

          case SC_INE :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            n1 = nint(sc_pop_stack()) ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            n2 = nint(sc_pop_stack()) ;
            if(n1 != n2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ INE result  =  %f\n",x) ;
            }
            break ;

          case SC_IGT :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            n1 = nint(sc_pop_stack()) ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            n2 = nint(sc_pop_stack()) ;
            if(n1 > n2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ IEQ result  =  %f\n",x) ;
            }
            break ;

          case SC_IGE :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            n1 = nint(sc_pop_stack()) ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            n2 = nint(sc_pop_stack()) ;
            if(n1 >= n2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ INE result  =  %f\n",x) ;
            }
            break ;

          case SC_ILT :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            n1 = nint(sc_pop_stack()) ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            n2 = nint(sc_pop_stack()) ;
            if(n1 < n2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ IEQ result  =  %f\n",x) ;
            }
            break ;

          case SC_ILE :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            n1 = nint(sc_pop_stack()) ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            n2 = nint(sc_pop_stack()) ;
            if(n1 <= n2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ INE result  =  %f\n",x) ;
            }
            break ;

          case SC_GT :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            y1 = sc_pop_stack() ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            y2 = sc_pop_stack() ;
            if(y1 > y2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ IEQ result  =  %f\n",x) ;
            }
            break ;

          case SC_GE :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            y1 = sc_pop_stack() ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            y2 = sc_pop_stack() ;
            if(y1 >= y2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ INE result  =  %f\n",x) ;
            }
            break ;

          case SC_LT :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            y1 = sc_pop_stack() ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            y2 = sc_pop_stack() ;
            if(y1 < y2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ IEQ result  =  %f\n",x) ;
            }
            break ;

          case SC_LE :
            process_signal_statement(node->opr.op[0]) ;  // Function name
            y1 = sc_pop_stack() ;
            process_signal_statement(node->opr.op[1]) ;  // Function name
            y2 = sc_pop_stack() ;
            if(y1 <= y2){
              sc_push_stack(1.0) ;
            }else{
              sc_push_stack(0.0) ;
            }
            if(ip){
              x = sc_pop_stack() ;
              sc_push_stack(x)   ;
              printf("+++ INE result  =  %f\n",x) ;
            }
            break ;

          default :
            printf("  =========== Operation %i '%s' not recognised\n",
                   node->opr.oper, sigscr_token_string(node->opr.oper)) ;
            break ;
        }
      }else if(node->type == typeStr){
        if(ip)printf("  node = %p, type = typeStr.  name = %p, %s\n",
          (void *)node, (void *)node->str.name, node->str.name) ;
        sc_name = node->str.name ;
      }
      return 0;
}
