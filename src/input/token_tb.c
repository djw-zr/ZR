/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File token_tb.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File containing routines for use in reading both text and binary
 *   shape files.
 *
 *==============================================================================
 */

void  init_levels(MSfile *msfile)
{
  int      i      ;
  MSblock  *block_level ;

      for(i=0;i<20;i++){
        block_level = &(msfile->level[i]) ;
        block_level->token_name = NULL ;
        block_level->itoken   = -1 ;
        block_level->flags    =  0 ;
        block_level->l_label  =  0 ;
        block_level->length   =  0 ;
        block_level->label    = NULL ;
        block_level->byte_end = -1 ;
      }
      current_block_level = -1 ;
      return ;
}

/*
 *  Routine to open a new block in either a text or binary file
 */

int  open_block(MSfile *msfile, int i_level){
  long     tell ;
  int      i    ;
  unsigned int k, n ;
  int      ip = 0 ;   //  1 for printing
  FILE     *fp = msfile->fp ;
  MSblock  *block ;
  char     *token  = NULL;
  char     *string = NULL ;
  char     my_name[] = "open_block" ;

      if(current_block_level != i_level-1){
        printf("  open_block() error.\n") ;
        printf("    Current block level   = %i\n",current_block_level) ;
        printf("    Requested block level = %i\n",i_level) ;
        printf("  Program stopping ... \n\n") ;
        exit(1) ;
      }
/*
 *  Text file
 *    read token
 */
      if(msfile->text){
        if(ip)printf("  Routine open_block : text file \n");
        current_block_level++ ;
        block = &(msfile->level[i_level]) ;
        if(NULL != block->token_name)free(block->token_name) ;
        block->token_name = strdup(new_token(msfile)) ;
        block->itoken     = 0 ;
        block->flags      = 0 ;
        block->length     = 0 ;
        block->byte_end   = 0 ;
        block->l_label    = 0 ;
/*
 *  End of block
 */
       if(ip) printf("    block->token_name = %s\n",block->token_name) ;
        if(block->token_name[0] == ')'){
          return_token(")",msfile) ;
          if(ip)printf("  open_block return token ')'\n") ;
          return -2 ;
        }
        if(!strcmp(block->token_name,"******Z")) return -1 ;
/*
 *  Search for index corresponding to token
 *    First convert name to upper case
 *    Then compare with list of token names
 */
        token = strdup(block->token_name) ;
        n  = strlen(token) ;
        for(k=0;k<n;k++)token[k] = toupper(token[k]) ;
/*
 *  Search enum tokens
 */
#if 1
{
  BTree *btree ;
  long  l ;
         btree = find_btree(enum_master, token) ;
         if(NULL == btree){
          printf(" Routine %s : BTree : Unable to determine token name\n",my_name);
          printf("    Data file = %s\n",msfile->filename);
          printf("    Token     = %s\n",token);
          printf("    Program stopping ... \n\n") ;
          dump_btree(enum_master,0,"A") ;
          exit(1) ;
        }
        l = (long) btree->data ;
        k = (int) l  ;
}

#else
  int   token_found = 0 ;

        for(k=0;k<1552;k++){
          if(!strcmp(token,token_idc[k])){
            token_found = 1 ;
            break ;
          }
        }
/*
 *  If not found try adding "_ALT"
 *  In some cases this was added to enum names to prevent conflict
 */
        if(!token_found){
          string = (char *)malloc(n+5) ;
          strcpy(string,token) ;
          strcat(string,"_ALT") ;
          for(k=0;k<1552;k++){
            if(strcmp(string,token_idc[k])==0){
              token_found = 1 ;
              break ;
            }
          }
          free(string) ;
        }

        if(!token_found){
          printf(" Routine %s.  Unable to determine token name\n",my_name);
          printf("    Data file = %s\n",msfile->filename);
          printf("    Token     = %s\n",token);
          printf("    Program stopping ... \n\n") ;
          exit(1) ;
        }
#endif
        block->itoken = k ;
        free(token) ;
/*
 *  Check for label and deal with lh bracket
 */
        token = new_token(msfile) ;
        free(block->label);
        block->label = NULL ;
        if(strcmp(token,"(") != 0){
          block->label = strdup(token) ;
          block->l_label = strlen(token) ;
          skip_lbr(msfile) ;
        }
 /*
 *  Check for end of file ;
 */
      }else if(msfile->binary){
        i = fgetc(fp) ;              // Needed to trigger EOF
        if(feof(fp))return -1 ;
        ungetc(i, fp)   ;            //  Restore character just read

        current_block_level++ ;

        block               = &(msfile->level[i_level]) ;
        tell                = ftell(fp) ;
        block->itoken       = read_uint16(fp) ;
        block->flags        = read_uint16(fp) ;
        block->length       = read_uint32(fp) + 8  ;
        block->byte_end     = tell + block->length ;
        block->l_label      = read_uint8(fp) ;
        if(block->l_label != 0){
          free( block->label) ;
          block->label = (char *)malloc(block->l_label+1);
          for(i=0;i<block->l_label;i++){
            block->label[i] = getc(fp);
            getc(fp) ;
          }
          block->label[block->l_label]='\0' ;      // Add end of string
        }
//  Add offset for world files
        if(msfile->world)  block->itoken += world_token_offset ;
/*
 *  Error
 */
      }else{
        printf("  Routine %s.  Error file is neither text or binary.\n",my_name) ;
        printf("    Data file = %s\n",msfile->filename);
        printf("    Program stopping ... \n\n") ;
        exit(1) ;
      }

#if 0
      printf("\n open_level()\n") ;
      printf("   i_level       = %5i\n",i_level) ;
      printf("   at byte       = %5i  %6x\n",(int)tell,      (int)tell)    ;
      printf("   itoken        = %5i  %6x  %s\n",block->itoken, block->itoken,
                                                     token_idc[block->itoken]) ;
      printf("   flags         = %5i  %6x\n",block->flags,   block->flags) ;
      printf("   block_length  = %5i  %6x\n",block->length,  block->length) ;
      printf("   byte_end      = %5i  %6x\n",block->byte_end,block->byte_end) ;
#endif
      return block->itoken ;
}

int  open_named_block(MSfile *msfile, int i_level, int itoken){

int  ifound ;
char my_name[] = "open_named_block" ;

      ifound = open_block(msfile, i_level);
/*
 *  Text files : Read token and skip following '('
 */
      if(msfile->text){
        if(ifound == itoken)return 0 ;
        else           return 1 ;

      }else if(msfile->binary){
        if(ifound == itoken) return 0;
        if(ifound == -1) return -1 ;
        printf(" Routine open_named_block.\n");
        printf("   Data file name = %s\n",msfile->filename);
      printf("   Wrong token found.\n");
        printf("   Token expected = %4i   %s",itoken, token_idc[itoken]);
        printf("   Token found    = %4i   %s\n",ifound,token_idc[ifound]);
        if(0==i_level) return 1;
        printf("  Program stopping ... \n");
        exit(1) ;
      }else{
        printf("  Routine %s.  Error file is neither text or binary.\n",my_name) ;
        printf("    Data file = %s\n",msfile->filename);
        printf("    Program stopping ... \n\n") ;
        exit(1) ;
      }
}

int   is_block_end(MSfile *msfile, int i_level, int i_print){
  FILE     *fp = msfile->fp ;
  MSblock  *block ;
  char     *token ;

      block = &(msfile->level[i_level]) ;
/*
 *  Check binary file
 */
      if(msfile->binary){
        if(block->byte_end == ftell(fp))return 1 ;
        if(i_print){
          printf("  Routine 'is_block_end'\n");
          printf("  When checked expected block end does not match"
                                                " file position\n") ;
          printf("    Block level = %i\n",i_level);
          printf("    Block token = %i  %s\n",block->itoken,
                                                token_idc[block->itoken]) ;
          printf("    Expected block end = %i\n",block->byte_end);
          printf("    File position      = %i\n",(int)ftell(fp));
        }
        return 0 ;
      }
/*
 *  Check text file for ')' but keep a copy in token_unused
 */
      token = new_token(msfile) ;
      free(msfile->token_unused) ;
      msfile->token_unused = strdup(token) ;             // Save a new copy
      if(strcmp(token,")") == 0) return 1   ;
      return 0 ;
}

void end_block(MSfile *msfile, int i_level){

int ip = 0 ;

      if(msfile->text){
        skip_rbr(msfile) ;
      }

      if(current_block_level != i_level){
        printf("  end_block() error.\n") ;
        printf("    Current block level       = %i\n",current_block_level) ;
        printf("    Requested end block level = %i\n",i_level) ;
        printf("  Program stopping ... \n\n") ;
        exit(1) ;
      }
      current_block_level-- ;

      if(ip)printf(" End block level = %i\n",i_level) ;

      return ;
}

/*
 * =========================================================================
 */

/*
 *   Section Z.  Text or binary read subroutines
 */

uint  read_uint8_a(MSfile *msfile){
FILE  *fp = msfile->fp ;
int   fbin = msfile->binary ;
union u_tag {
  uint  m ;
  char  ca[4] ;
}  u ;
//char  myname[] = "read_uint8";

      if(fbin){
        u.m = 0 ;
        u.ca[0] = getc(fp) ;
        return u.m ;
      }
      return uitoken(msfile) ;
}

int  read_int16_a(MSfile *msfile){
int  i ;
FILE  *fp = msfile->fp ;
int   fbin = msfile->binary ;
union u_tag {
  int  m ;
  char ca[4] ;
}  u ;
//char  myname[] = "read_int16";

      if(fbin){
        u.m = 0 ;
        for(i=0;i<2;i++){
        u.ca[i] = getc(fp) ;
        }
        return u.m ;
      }
      return itoken(msfile) ;
}

uint  read_uint16_a(MSfile *msfile){
int  i ;
FILE  *fp = msfile->fp ;
int   fbin = msfile->binary ;
union u_tag {
  uint  m ;
  char  ca[4] ;
}  u ;
//char  myname[] = "read_uint16";

      if(fbin){
        u.m = 0 ;
        for(i=0;i<2;i++){
        u.ca[i] = getc(fp) ;
        }
        return u.m ;
      }
      return uitoken(msfile) ;
}


int  read_int32_a(MSfile *msfile){

int  i ;
FILE  *fp = msfile->fp ;
int   fbin = msfile->binary ;
union u_tag {
  int  m ;
  char ca[4] ;
}  u ;
//char  myname[] = "read_int32";

      if(fbin){
        u.m = 0 ;
        for(i=0;i<4;i++){
          u.ca[i] = getc(fp) ;
        }
        return u.m ;
      }else{
        return itoken(msfile) ;
      }
}

uint  read_uint32_a(MSfile *msfile){

int  i ;
FILE  *fp = msfile->fp ;
int   fbin = msfile->binary ;
union u_tag {
  uint m ;
  char ca[4] ;
}  u ;
//char  myname[] = "read_uint32";

      if(fbin){
        u.m = 0 ;
        for(i=0;i<4;i++){
        u.ca[i] = getc(fp) ;
        }
        return u.m ;
      }else{
        return uitoken(msfile) ;
      }
}

 long  read_lint32_a(MSfile *msfile){

int   i, ip = 0 ;
long  l ;
FILE  *fp = msfile->fp ;
int   fbin = msfile->binary ;
union u_tag {
  uint m ;
  char ca[4] ;
}  u ;
char  my_name[] = "read_lint32";

      if(fbin){
        u.m = 0 ;
        for(i=0;i<4;i++){
        u.ca[i] = getc(fp) ;
        }
        if(ip)printf("  Routine %s.  value = %i\n", my_name, u.m) ;
        return u.m ;
      }else{
        l = ltoken(msfile) ;
        if(ip)printf("  Routine %s.  value = %li\n", my_name, l) ;
        return l ;
      }
}

int  read_int64_a(MSfile *msfile){

int  i ;
FILE  *fp = msfile->fp ;
union u_tag {
  uint m ;
  char ca[4] ;
}  u ;
//char  myname[] = "read_int64";

      u.m = 0 ;
      for(i=0;i<4;i++){
       u.ca[i] = getc(fp) ;
      }
      for(i=0;i<4;i++)getc(fp) ;
      return u.m ;
}

float read_real32_a(MSfile *msfile){

int  i ;
FILE  *fp = msfile->fp ;
int   fbin = msfile->binary ;
union u_tag {
  float m ;
  char ca[4] ;
}  u ;
//char  myname[] = "read_real32";

      if(fbin){
        u.m = 0.0 ;
        for(i=0;i<4;i++){
        u.ca[i] = getc(fp) ;
        }
        return u.m ;
      }
/*
 * Text
 */
      return dtoken(msfile) ;
}

/*
 *   read_ucarz
 *   This is a byte reading routine designed to read the special unicode text
 *   in the *.t files and possibly others.  The string consists of int16 giving
 *   n, the number of characrters, followed by n unicode UTF16LE characters.
 *   These are read and converted to UTF8 for linux
 *
 *   NOTE the corrected form of this routine has been moved to token.c
 *        where it can use array 'token' and the routines fgetu and ut_utf8
 */
#if 0
char * read_ucharz_a(MSfile *msfile){

int  i, n  ;
char  *string ;
FILE  *fp = msfile->fp ;
int   fbin = msfile->binary ;

      if(fbin){
        n = (int)read_uint16(fp) ;
        string = (char *)malloc(n+1);  //  generate a new string
        for(i=0;i<n;i++){
          string[i] = getc(fp) ;
          getc(fp) ;
        }
        string[n] = '\0' ;
        return string ;
      }
      return ctoken(msfile) ;
}
#endif
/*
 *  Routine to skip to the end of a block
 *  for both ascii and binary  MSTS files
 */


void skip_to_bblock_end_a(MSfile *msfile, MSblock *block){

int i ;
int is_binary = msfile->binary ;

      if(is_binary){
        for(i=ftell(msfile->fp);i<block->byte_end;i++){
          fgetc(msfile->fp) ;
        }
      }else{
        skippast_rbr(msfile) ;
        return_token(")",msfile) ;  // Leave ')' for endblock routine
      }
      return ;
}

