/*
 * *****************************************************************************
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File : token.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Subroutines to split a file into tokens and to convert them to
 *   int, double and char types.
 *
 *   See:  STFReader.cs
 *         SBR.cs
 *
 * *****************************************************************************
 */

#include <errno.h>
#include <assert.h>                      // For zlib
#include "zlib.h"                        //  For zlib
#define CHUNK 16384                      //  For zlib
#define ZR_MAX_TOKEN 65535

static char token[ZR_MAX_TOKEN] ;
static int  rbr_buffered  ;
static int  skip_underscore_token(MSfile *msfile) ;
static int32_t utf_char_temp,       //  Temporary location for utf character
               utf_flag_temp = 0 ;  //  Set to 1 if utf_char_temp is set.
static unsigned char utf[3] ;
static int32_t utf_flag    ;


/*
 * *****************************************************************************
 *  Initialise tokens
 * *****************************************************************************
 */
void  init_token(){
      rbr_buffered = 0;
      return ;
}

/*
 * *****************************************************************************
 * Test if string only contains an [+|-]integer
 * Returns 0 if OK
 * *****************************************************************************
 */
int is_string_int(char *s){

int i, l;
char c;

      l = strlen(s) ;
      c = s[0] ;

// Check one character string

      if(l == 1 && !isdigit(c)) return 1 ;

//  Check first character

      if(!(isdigit(c) || c=='+' || c == '-')) return 1 ;

//  Check rest of string

      for(i=0;i<l;i++){
        c = s[i] ;
        if(!isdigit(c)) return 1 ;
      }
      return 0 ;
}

/*
 * *****************************************************************************
 * Test if string only contains decimal characters
 * [+|-]integer[.integer]*
 * Returns 0 if OK
 * *****************************************************************************
 */
int is_string_decimal(char *s){

int i, k = 0, l;
char c;

      l = strlen(s) ;
      c = s[0] ;

// Check for one character string
      if(l == 1 && !isdigit(c)) return 1 ;

      if(c=='.'){
        k = k + 1 ;
      }else{
        if(!(isdigit(c) || c=='+' || c == '-'))return 1;
      }
      ;
      for(i=1;i<l;i++){
        c = s[i] ;
        if(c=='.'){
          k = k + 1;
          if(k > 1) return 1 ;
          continue;
        }
        if(!isdigit(c)) return 1 ;
      }
      return 0 ;
}

/*
 * *****************************************************************************
 * Test if string only contains alphanumeric characters
 * Returns 0 if OK
 * *****************************************************************************
 */
int is_string_alnum(char *s){

int i ;

      for(i=1;i<(int)strlen(s);i++){
        if(!isalnum(s[i]))return 1 ;
      }
      return 0;
}

/*
 * *****************************************************************************
 * Test if string endicates end of file
 * Returns 1 if end of file
 * *****************************************************************************
 */
int is_eof(char *token){
      return !strcmp(token,eof_mark);
}

/*
 * *****************************************************************************
 *  Routine to read a character from a file coded using UTF-16LE format
 *  and return the UTF integer representing the character
 *  Note 'getc' is a macro, faster than the function 'fgetc'
 * *****************************************************************************
 */


int32_t fgetu(MSfile *msfile){

  int32_t c1, c2 ;
  FILE  *fp = msfile->fp ;

      if(utf_flag_temp){
        utf_flag_temp = 0 ;
        return utf_char_temp ;
      }
/*
 *  Some MSTS 'binary' files have no unicode header but still
 *  need filenames processed as UTF16LE
 */
      if(msfile->utf8  && !msfile->binary){
        return fgetc(fp) ;
      }
/*
 *  Assume udf-16le
 */
      c1 = getc(fp) ;
      if(c1 == EOF) return EOF ;
      c1 = c1 & 0xFF ;
      c2 = getc(fp) ;
      if(c2 == EOF) return EOF ;
      if(c2 != 0){ c2 = c2 & 0xFF ; c1 = (c2 << 8) | c1 ; }
      return c1 ;
}

void put_utf8(int32_t c, int *l){

  int32_t  c1, c2, c3, c4 ;

//      printf("  put_utf6 : l = %i, c = %x %c\n",*l,c,c) ;
      if(c < 0x7F){                          //  One character code
        token[(*l)++] = c ;
      }else if(c<0x7FF){                     //  Two character code
        c1 = 0x80 | (c & 0x3F) ;
        c2 = 0xC0 | ((c >> 6) & 0x1F) ;
        token[(*l)++] = c2 ;
        token[(*l)++] = c1 ;
      }else if(c<0xFFFF){                     //  Three character code
        c1 = 0x80 | (c & 0x3F) ;
        c2 = 0x80 | ((c >> 6) & 0x3F) ;
        c3 = 0xE0 | ((c >> 12) & 0xF) ;
        token[(*l)++] = c3 ;
        token[(*l)++] = c2 ;
        token[(*l)++] = c1 ;
      }else if(c<0x10FFFF){                   //  Four character code
        c1 = 0x80 | (c & 0x3F) ;
        c2 = 0x80 | ((c >> 6) & 0x3F) ;
        c3 = 0x80 | ((c >> 12) & 0x3F) ;
        c4 = 0xF0 | ((c >> 18) & 0x7) ;
        token[(*l)++] = c4 ;
        token[(*l)++] = c3 ;
        token[(*l)++] = c2 ;
        token[(*l)++] = c1 ;
      }
      return ;
}

/*
 * *****************************************************************************
 *  new_token parses a new token from the input file
 *  It returns a pointer to its own storage array 'token'.
 *  The token must be copied or used before the next call to
 *  new_token.
 *
 *  NOTE:  This routine now reads characters using routine fgetu which
 *         converts any UTF16LE characters to UTF8, the system used by
 *         most software including linux.
 *
 * *****************************************************************************
 */

char *new_token(MSfile *msfile){

  int   ip = 0 ;
  int32_t   l, c, lastc;
  int   utf16le, compress, text, binary ;
  FILE  *fp ;
  char  my_name[] = "new_token" ;

//      ip = l_ip ;                  //  Debug printing only when l_ip is set

      fp       = msfile->fp        ;
      utf16le  = msfile->utf16le   ;
      compress = msfile->compress  ;
      text     = msfile->text      ;
      binary   = msfile->binary    ;

      if(text != 1){
        printf("\n  ERROR : Routine new_token : File type not supported\n");
        printf("  utf16le  = %i\n",utf16le);
        printf("  compress = %i\n",compress);
        printf("  text     = %i\n",text);
        printf("  binary   = %i\n",binary);
        printf(" fp        = %p\n",(void *)fp);
        printf(" Program exiting ... \n\n");
        exit(1) ;
      }
      if(msfile->token_unused != NULL){
        strcpy(token,msfile->token_unused) ;
        free(msfile->token_unused) ;
        msfile->token_unused = NULL ;
        rbr_buffered = 0;
        if(ip)printf("    new_token return OLD TOKEN   :  %s\n",token) ;
        return token ;
      }

      if(0 != rbr_buffered){
        rbr_buffered = 0;
        token[0] = ')'  ;
        token[1] = '\0' ;
        if(ip)printf("    new_token return RBR_BUFFERED:  %s\n",token) ;
        return token ;
      }
/*
 *  Skip white space, check for EOF and non-standard tokens
 *  At this stage c corresponds to one of the UTF codepoints
 */
      for(;;){
        while( (c=fgetu(msfile)) != EOF && (c == ' '  || c == '\t'
                                   || ( (c & 0XFF) == 0XA0)
                                   || c == '\n' || c == '\r' ) ) {
        }
        if(c == EOF){
          return strcpy(token,eof_mark) ;
        }

        if(c != '_')break  ;            //  Normal token
        l = skip_underscore_token(msfile) ; //  Special treatment routine
        if(1 == l ){
          if(ip)printf("    new_token return EOF_MARK    :  %s\n",eof_mark) ;
          return strcpy(token,eof_mark) ;
        }
      }
      l = 0 ;
      put_utf8(c, &l) ;    //  Convert from UTF code point to UTF-8
/*
 *  Open and closing brackets
 */
      if(c == '(' || c == ')'){
        token[1] = '\0' ;
        if(ip)printf("    new_token return BRACKET      :  %s\n",token) ;
        return token ;
      }
/*
 *  Normal tokens
 */
      if(c !='"'){
        while((c=fgetu(msfile)) != EOF && c != ' ' && c != '\t' && c != '\n' && c!= '\r'
                          && c != '(' && c != ')' && l <4095 ){
//          token[l++] = c ;
          put_utf8(c, &l) ;
        }
        utf_flag_temp = 1 ;                         // Put back last character
        utf_char_temp = c ;
/*
 *  Process streams of text.  These start and end with quotes(").
 *  If the final quote is followed by a plus sign, the text is continued
 *  following the next quote.
 *
 *  Sometimes in error:
 *      *  The plus sign appears immediately before the final quote
 *      *  It is a mess
 *      Name ( "60007 A4 Gresley, SIR NIGEL GRESLEY, KX Shed 34A, TE 35.5Klbf\n"+
 *             "Coal 9t, Water 5000gal, Corridor Tender"\n"+)
 *      *  The plus sign is followed by a close bracket ")"
 *  In the first case the error is corrected.
 *  In the second case the plus sign is ignored.
 */
      }else{
        lastc = ' ';
        for(;;){
/*
 *  1.  Search until matching '"' is found
 */
          while(((c=fgetu(msfile)) != '"' || lastc == '\\' ) && l <ZR_MAX_TOKEN ){
            if(c == '"') l-- ;              //  Convert \" to "
            if(c != '\r') put_utf8(c, &l) ; //  Skip carriage return
            lastc = c ;
          }
          if(l>= ZR_MAX_TOKEN) break ;                  // Leave 'for' loop
          put_utf8(c, &l) ;                 //  Save "
/*
 *   2.  The quote '"' should be followed by '+' or white space
 *       It may be followed immediatly by ')'
 *       Skip any other characters
 */
          while((c=fgetu(msfile)) != EOF && c != ' ' && c != '\t' && c != '\n' && c!= '\r'
                          && c != '(' && c != ')' && c != '+' ) ;  //  Throw it away
/*
 *   3.  Exit unless  '+' found
 */
          if(c != '+'){
            utf_flag_temp = 1 ;                 // Put back last character
            utf_char_temp = c ;
            break ;                             // Leave 'for' loop
          }
          lastc = ' ';
/*
 *  4.  Search for second starting '"'
 *      Ignore everything except '"' and  newline.
 */
          while((c=fgetu(msfile)) != '"' && l <ZR_MAX_TOKEN ){
            if(c == ')'){
              rbr_buffered = 1 ;
              break ;
            }
            if(c == '\n'){
              put_utf8(c, &l) ;
            }
          }
          if( rbr_buffered || (l >= ZR_MAX_TOKEN) )  break ;  // Leave 'for' loop
          put_utf8(c, &l) ;                                   //  Save new starting '"'
        }
      }
      if(l>=ZR_MAX_TOKEN){
        printf("\n  ERROR : Routine new_token : Token is > 65535 characters\n");
        for(l=1;l<128;l++)printf("%c",token[l]) ;
        printf("\n") ;
        printf(" Program exiting ... \n\n");
        exit(1) ;
      }
      token[l] = '\0';
      if(ip)printf("    new_token return NORMAL_TOKEN:  %s\n",token) ;

      return token ;
}

/*
 * *****************************************************************************
 *  Return temporay token
 * *****************************************************************************
 */
char *new_tmp_token(MSfile *msfile){
      return new_token(msfile) ;
}

/*
 * *****************************************************************************
 *  Return pointer to new character string
 * *****************************************************************************
 */
char *ctoken(MSfile *msfile){
       return strdup(new_token(msfile));
}
/*
 * *****************************************************************************
 *  Return int
 * *****************************************************************************
 */
int  itoken(MSfile *msfile){
  char *token  = new_token(msfile) ;

      if(0 == strcmp(token,")")){
        rbr_buffered = 1 ;
        return 0;
      }
      return atoi(token);
}
/*
 * *****************************************************************************
 *  Return unsigned int
 * *****************************************************************************
 */
unsigned int  uitoken(MSfile *msfile){
  char *token  = new_token(msfile) ;

      if(0 == strcmp(token,")")){
        rbr_buffered = 1 ;
        return 0;
      }
      return atoi(token);
}
/*
 * *****************************************************************************
 *  Return long
 * *****************************************************************************
 */
long ltoken(MSfile *msfile){
      return atol(new_token(msfile));
}
/*
 * *****************************************************************************
 *  Returns long long - where token is a 16 byte octal string
 * *****************************************************************************
 */
long long lltoken_16(MSfile *msfile){
      return strtol(new_token(msfile), NULL, 16);
}

/*
 *  Routine to read a token and return an integer containing 1-bit flags
 */
uint flagtoken(MSfile *msfile){
  uint  ui = 0 ;
  uint  i, n   ;
  char *token ;
  char *my_name = "flagtoken" ;

      token = new_tmp_token(msfile) ;
      n = strlen(token) ;
      for(i=0; i<n; i++){
        ui = ui << 1 ;
        if(token[i] == '1'){
          ui = ui + 1 ;
        }else if(token[i] != '0'){
          printf("  Routine '%s' error\n",my_name) ;
          printf("  Badly formed token\n") ;
          printf("  Token = %s\n",token);
          return ui ;
        }
      }
      return ui ;
}

/*
 * *****************************************************************************
 *  Return double
 * *****************************************************************************
 */
double dtoken(MSfile *msfile){
       return atof(new_token(msfile)) ;
}
#if 0
/*
 * *****************************************************************************
 *  Return multi-line text with errors
 *
 *  Normal text should be enclodes with quotes '"' and if there are
 *  continuaiton lines the final quite on a line should be followed by a '+'.
 *  Unfortunately this is sometimes written as +" instead of "+.  To allow
 *  for this txt_token checks the token following text.  If it starts with a
 *  '"' the code assumes that it is a continuation line an adds it to the
 *  original text.  If not it returns the last token and exits
 * *****************************************************************************
 */
char *txt_token(MSfile *msfile){
  char *token1 ;
  char *token2 ;
  char *token3 ;
       token1 = strdup(new_token(msfile)) ;
       for(;;){
         token2 = new_token(msfile) ;
         if(token2[0] != '"'){
           return_token(token2,msfile) ;
           return token1 ;
         }
         token3 = (char *)malloc(strlen(token1)+strlen(token2)+1) ;
         strcpy(token3,token1) ;
         strcat(token3,token2) ;
         free(token1) ;
         token1 = token3 ;
       }
}
#endif
/*
 * *****************************************************************************
 * Search for left bracket and skip
 * *****************************************************************************
 */
int  skip_lbr(MSfile *msfile){
  int  i, n ;
  char *token ;

    token = new_token(msfile);
    if( 0 == strcmp(token,eof_mark) ) return(-1) ;
    if(token[0] == '('){
      if((n=strlen(token)==1))return 0 ;
      printf("  routine new token error - token starts with '(')\n");
      printf("  Token = %s\n",token);
      for(i=0;i<n;i++)printf(" %c :: ",token[i]) ;
      printf("\n") ;
      for(i=1;i<n;i++){
        token[i-1] = token[i] ;
      }
      return_token(token,msfile) ;
    }else{
      printf("  Routine skip_lbr.  Left bracket not found\n");
      printf("    Token returned is %s\n", token);
      printf("  Program stopping ... \n\n");
      exit(1);
    }
    return 0;
}
/*
 * *****************************************************************************
 * Search for right bracket and skip
 *   Version aborts if right bracket not found
 * *****************************************************************************
 */
int  skip_rbr(MSfile *msfile){
  char *token ;

    token = new_token(msfile);
    if(0 == strcmp(token,eof_mark)) return(-1) ;
    if(0 != strcmp(token,")")){
      printf("  Routine skip_rbr.  Right bracket not found\n");
      printf("    Token returned is %s\n", token);
      printf("  Program stopping ... \n\n");
      exit(1);
    }
    return 0;
}
/*
 *   Version returns '1', if right bracket not immediately found
 */
int  skip_rbr_1(MSfile *msfile){
  char *token ;

    token = new_token(msfile);
    if(0 == strcmp(token,eof_mark)) return(-1) ;
    if(0 != strcmp(token,")")){
      return_token(token,msfile) ;
      return 1;
    }
    return 0;
}
/*
 * Skip any intermediate tokens as well
 */

int  skippast_rbr(MSfile *msfile){

  int  n, ip = 0 ;
  char *token    ;
  char *myname = "skippast_rbr" ;

      if(ip)printf("  Enter routine %s\n",myname) ;
      n = 1 ;
      for(;;){
        token = new_tmp_token(msfile) ;
        if(ip)printf("   New token n= %i, token = %s\n",n,token) ;
        if(!strcmp(token,")"))n-- ;     // Right bracket
        if(!n)break ;                   // n == 0
        if(!strcmp(token,"("))n++ ;     // Left bracket
      }
      if(ip)printf("  Exit routine %s\n",myname) ;
      return 0;
}
/*
 * *****************************************************************************
 * Check for left and right brackets
 * *****************************************************************************
 */
int  is_lbr(char *token){
    if (0 == strcmp(token,"(") ) return 1 ;   // True
    else return 0 ;                           // False
}
int  is_rbr(char *token){
    if (0 == strcmp(token,")") ) return 1 ;   // True
    else return 0 ;                           // False
}

/*
 *  Tokens starting with '_' may be used for comments.
 *  Here they are scanned and ignored.
 *  As far as is known these always contain matching
 *  brackets '(' and ')' but these may not be separated
 *  from other text by white space
 */

int  skip_underscore_token(MSfile *msfile){

int   n  ;
int32_t   c  ;
char  my_name[] = "skip_underscore_token" ;

// Skip rest of name
      while((c=fgetu(msfile)) != EOF &&  c != ' '  && c != '\t'
                                 &&  c != '\n' && c != '\r' && c != '('){
      }
//  Check for EOF
      if(c == EOF){
        printf("\n  ERROR?? : Routine %s : EOF found\n",my_name);
        exit(0) ;
        return 1 ;
      }
//  Skip white space
      if('(' != c) {
        while((c=fgetu(msfile)) != EOF &&  c == ' '  && c == '\t'
                                  &&  c == '\n' && c == '\r' ){
        }
      }
//  Return if opening bracket is not present
      if(c!='('){
//        printf("  Routine %s : Token has no opening bracket.  Character = %c\n",
//               my_name,c) ;
        utf_char_temp = c ;
        utf_flag_temp = 1 ;
        return 0 ;
      }
      n = 1 ;
      while(n != 0){
        if((c = fgetu(msfile)) == EOF){
          printf("\n  ERROR?? : Routine %s : EOF found\n",my_name);
          printf("    c = %x,  EOF = %x\n",c,EOF);
          return 1 ;
        }  ;
        if(c == '(')n++ ;
        if(c == ')')n-- ;
      }
      return 0 ;
}

/*
 *   Routine to place an unused token in token_unused
 */

int return_token(char *token, MSfile *msfile){
      free(msfile->token_unused) ;
      msfile->token_unused = strdup(token) ;
      return 0 ;
}

/*
 *  Routine to carry out conversions
 *  This also corrects for
 *       values within quotes      ( "4.52kg" )
 *       values with units missing (  4.52    )
 */

double convert_unit(char *token0, char *unit){

int    ip = 0 ;         // Debug
uint   i, j ;
char   string[32]= "" ; // storage for units
char   *token ;
size_t lt, ls ;
float  value = 1.0e9 ;        //  Something really large

      token = strdup(token0) ;
      token = strip_quotes(token) ;

      if(ip)printf(" convert : token = %s\n",token) ;
      ls = strspn(token,"-1234567890e.") ;
      if(ip)printf(" convert : ls = %i\n",(int)ls) ;
      if(token[ls-1]=='e')ls = ls - 1;

      strncpy(string,&token[ls],31) ;
      token[ls] = '\0' ;
      lt = strcspn(string," #") ;
      string[lt] = '\0' ;
      if(ip)printf("  Number = %s, Unit = %s\n",token,string);

      for(i=0, j=0; i<strlen(string); i++){
        if(string[i] != '*' && string[i] != '(' &&string[i] != ')')string[j++] = string[i] ;
      }
      string[j] = '\0' ;
      if(ip)printf("  Number = %s, Unit = %s\n",token,string);

      sscanf(token,"%f",&value) ;
      if(ip)printf("  Value = %f\n",(float)value);

      if(!strlen(unit) || !strlen(string) || !strcmp(string,unit)){
        return (double)value ;
      }else if(0==strcmp(unit,"kg") && 0==strcmp_ic(string,"t-uk")){
        value = value * 1016.047  ;  // Convert long ton to kg
      }else if(0==strcmp(unit,"kg") && 0==strcmp_ic(string,"t")){
        value = value * 907.18474  ;  // Convert short ton to kg
      }else if(0==strcmp(unit,"kg") && 0==strcmp_ic(string,"t-us")){
        value = value * 907.18474  ;  // Convert short ton to kg
      }else if(0==strcmp(unit,"kg") && 0==strcmp_ic(string,"lb")){
        value = value * 0.45359237  ;  // Convert lb to kg
      }else if(0==strcmp(unit,"m") && 0==strcmp_ic(string,"ft")){
        value = value * 12.0 * 0.0254  ;  // Convert feet to metres
      }else if(0==strcmp(unit,"m") && 0==strcmp_ic(string,"ft+1m")){
        value = value*12.0*0.0254 + 1.0 ;
      }else if(0==strcmp(unit,"m") && 0==strcmp_ic(string,"ft+10in")){
        value = (value*12.0 + 10.0)*0.0254 ;
      }else if(0==strcmp(unit,"m") && 0==strcmp_ic(string,"in")){
        value = value * 0.0254  ;  // Convert inches to metres
      }else if(0==strcmp(unit,"m") && 0==strcmp_ic(string,"\"")){
        value = value * 0.0254  ;  // Convert inches to metres
      }else if(0==strcmp(unit,"m") && 0==strcmp_ic(string,"in/2")){
        value = value * 0.0254 * 0.5  ;  // Convert inches/2 to metres
      }else if(0==strcmp(unit,"m") && 0==strcmp_ic(string,"cm")){
        value = value * 0.01  ;  // Convert cm to metres
      }else if(0==strcmp(unit,"m") && 0==strcmp_ic(string,"mm")){
        value = value*0.01 ;       // mm to m
      }else if(0==strcmp(unit,"N") && 0==strcmp_ic(string,"kN")){
        value = value * 1000.0  ;  // Convert kN to N
      }else if(0==strcmp(unit,"N") && 0==strcmp_ic(string,"lbf")){
        value = value * 0.138254954376  ;  // Convert lb ft/s^2 to N (Wikipedia)
      }else if(0==strcmp(unit,"N/m/s") && (   (0==strcmp_ic(string,"N/m/S")) ||
              (0==strcmp_ic(string,"N/m")) || (0==strcmp_ic(string,"N/m.S")) ) ){
        value = value ;            // Coding error ?
      }else if(0==strcmp(unit,"m^2") && 0==strcmp_ic(string,"ft^2")){
        value = value/(144.0 * 0.0254 * 0.0254)  ;   // ft^2 to m^2
      }else if(0==strcmp(unit,"ft^2") && 0==strcmp_ic(string,"m^2")){
        value = value* 144.0 * 0.0254 * 0.0254  ;   // m^2 to ft^2
      }else if(0==strcmp(unit,"ft^3") && 0==strcmp_ic(string,"m^3")){
        value = value* 1728.0 * 0.0254 * 0.0254 * 0.0254  ;   // m^3 to ft^3
      }else if(0==strcmp(unit,"psi") && 0==strcmp_ic(string,"inHg")){
        value = value*0.489771 ;   // inHg to psi (Wikipedia)
      }else if(0==strcmp(unit,"psi") && 0==strcmp_ic(string,"cmHg")){
        value = value*0.489771/2.54 ;
      }else if(0==strcmp(unit,"psi/s") && 0==strcmp_ic(string,"inHg/s")){
        value = value*0.489771 ;   // inHg to psi (Wikipedia)
      }else if(0==strcmp(unit,"ft^3") && 0==strcmp_ic(string,"in^3")){
        value = value/1728.0 ;   // cu in to cu ft

      }else{
        printf(" Conversion failed\n")    ;
        printf("   token  = %s\n",token)  ;
        printf("   string = %s\n",string) ;
        printf("   unit   = %s\n",unit)   ;
//        value = 0./0. ;
        printf(" value = %f\n",value)     ;
      }
      free(token) ;
      return (double)value ;
}



/*
 * *****************************************************************************
 *
 *  zlib inflation/decompression
 *
 *  Decompress from file 'source' to file 'dest' until stream ends or EOF.
 *  inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
 *  allocated for processing, Z_DATA_ERROR if the deflate data is
 *  invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
 *  the version of the library linked do not match, or Z_ERRNO if there
 *  is an error reading or writing the files.
 *
 *  The zr program uses routine zlib_uncompress (below) which calls this
 *  routine to uncompress the MSTS binary files.
 *
 *  The routine requires file zlib.h to compile and the program then
 *  needs to be linked to a matching version of the z library (libz).
 *
 *  This is based on a program on the zlib web pages
 * *****************************************************************************
 */
int inf(FILE *source, FILE *dest)
{
  int ret;
  unsigned have;
  z_stream strm;
  unsigned char in[CHUNK];
  unsigned char out[CHUNK];

    /* allocate inflate state */
      strm.zalloc = Z_NULL;
      strm.zfree = Z_NULL;
      strm.opaque = Z_NULL;
      strm.avail_in = 0;
      strm.next_in = Z_NULL;
      ret = inflateInit(&strm);
      if (ret != Z_OK)
        return ret;

/* decompress until deflate stream ends or end of file */
      do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
          (void)inflateEnd(&strm);
          return Z_ERRNO;
        }
        if (strm.avail_in == 0)
          break;
        strm.next_in = in;

/* run inflate() on input until output buffer not full */
        do {
          strm.avail_out = CHUNK;
          strm.next_out = out;
          ret = inflate(&strm, Z_NO_FLUSH);
          assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
          switch (ret) {
          case Z_NEED_DICT:
              (void)inflateEnd(&strm);
              ret = Z_DATA_ERROR;
              return ret;
          case Z_DATA_ERROR:
          case Z_MEM_ERROR:
              (void)inflateEnd(&strm);
              return ret;
          }
          have = CHUNK - strm.avail_out;
          if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
              (void)inflateEnd(&strm);
              return Z_ERRNO;
          }
        } while (strm.avail_out == 0);

/* done when inflate() says it's done */
      } while (ret != Z_STREAM_END);

/* clean up and return */
      (void)inflateEnd(&strm);
      return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

/* report a zlib or i/o error */
void zerr(int ret)
{
      fputs("zpipe: ", stderr);
      switch (ret) {
      case Z_ERRNO:
          if (ferror(stdin))
              fputs("error reading stdin\n", stderr);
          if (ferror(stdout))
              fputs("error writing stdout\n", stderr);
          break;
      case Z_STREAM_ERROR:
          fputs("invalid compression level\n", stderr);
          break;
      case Z_DATA_ERROR:
          fputs("invalid or incomplete deflate data\n", stderr);
          break;
      case Z_MEM_ERROR:
          fputs("out of memory\n", stderr);
          break;
      case Z_VERSION_ERROR:
          fputs("zlib version mismatch!\n", stderr);
      }
}

/*
 * *****************************************************************************
 *  Uncompress zlib format file
 *
 *  This routine uncompresses the file to a temporary file and then changes
 *  the stream in the structure 'msfile' to point to the start of the
 *  uncompressed file.
 *
 *   A better method would be to use internal buffers but this means either
 *   modifying all the getc(fp) calls in this file, so that they read from
 *   the the input stream of the buffer, or having separate token subroutine
 *   for compressed files.
 *
 *   Check : It might only be necessary to change the calls in ms_token.
 * *****************************************************************************
 */

int zlib_uncompress(MSfile *msfile, int uncompressed_size, int ip)
{
  int   iret ;
  char  tmp_file[] = "unzlib.tmp"    ;
  char  myname[] = "zlib_uncompress" ;
  FILE  *fp_out,  *fp_in ;

      if(ip){
        printf("  Enter %s\n",myname) ;
        printf("  File = %s\n",msfile->filename) ;
        printf("  uncompressed_size (scaled?) = %i\n",uncompressed_size) ;
      }

      fp_in = msfile->fp ;
      if(ip)printf(" Z1\n");
/*
 *  Open 'temporary' file.
 *  With GNU/linux - the file is a simple byte stream
 *                 = usually it will be kept in memory
 *  With Microsoft - the "b" flag has to be defined to prevent text conversion
 *                 - setting up an in-memory file is messy
 *  Here it is assumed that SDL2 is a windows system so the 'temporary'
 *       file created in the local directory
 */
#ifdef SDL2
      fp_out = gopen(tmp_file,"wb+") ;  //  keep file at program end
//      _rmtmp() ;
//      set_fmode(_O_BINARY) ;
//extern int _fmode ;
//      _fmode = _O_BINARY ;
//       fp_out = tmpfile() ;
#else  // GNU/Linux
      fp_out = tmpfile() ;
#endif
      if(ip)printf(" Z2  %p\n",(void *)fp_out);
/*
 *  Decompress
 */
      if(ip)printf(" Z3\n");
      iret = inf(fp_in, fp_out);
      if(ip)printf(" Z4\n");
      if (iret != Z_OK) zerr(iret);
/*
 * Close original file and replace file pointer by new file
 */
      if(ip)printf(" Z5\n");
      gclose(fp_in) ;
      if(ip)printf(" Z6\n");
      msfile->fp = fp_out ;
/*
 *  Rewind ready for further processing
 */
      rewind(msfile->fp) ;

      return 0;
}

void init_msfile(MSfile *msfile){
  int      k ;
  MSblock  *block ;
      msfile->fp        = NULL ;
      msfile->filename  = NULL ;
      msfile->utf16le   = 0    ;
      msfile->utf8      = 0    ;
//      msfile->unicode   = 0    ;
//      msfile->ascii     = 0    ;
      msfile->compress  = 0    ;
      msfile->text      = 0    ;
      msfile->binary    = 0    ;
      msfile->world     = 0    ;
      msfile->texture   = 0    ;
      msfile->token_unused = NULL ;
      for(k=0;k<20;k++){
        block = &(msfile->level[k]) ;
        block->token_name = NULL ;
        block->label      = NULL ;
        block->itoken     = 0    ;
        block->flags      = 0    ;
        block->l_label    = 0    ;
        block->length     = 0    ;
        block->byte_end   = 0    ;
      }
      return ;
}

/*
 * *****************************************************************************
 *   Open MSTS format file
 *
 *   MSTS files start with a MSTS header.
 *   * If it is a text file using UTF-16 unicode (the Microsoft standard) the
 *      file starts with the characters 0xff and 0xfe, the order showing
 *      whether the file is big-endian or littel-endian.  If it is not unicode
 *      these characters are missing.
 *   * The file then has an 16 byte header (16 * 2 bytes if unicode)
 *   * The header should start  "SIMISA@".  If the file is compressed the
 *     next (last) character is 'F', otherwise it is '@'.
 *   * If compressed the next four characters give the size of the
 *     uncompressed file as a four byte 'int'.  This is needed so the
 *     program can provide a large enough buffer to contain the
 *     uncompressed file before further processing.  This is followed by
 *     four bytes '@@@@'.
 *   * Compression uses the zlib library.  In the standard form the core
 *     compressed region is surrounded by a small zlib container which
 *     contains parity information.  In the gzip form it is surrounded by a
 *     gzip container which contains the original filename and other data.
 *   * If compressed the remainder of the file needs to be uncompressed
 *     (decompressed?)
 *
 *   * The file (the uncompressed version if compressed) then contains a
 *     16-byte sub-header (16 * 2 bytes if unicode)
 *       If character 6 is a 'w', it is a world file.
 *        "     "     7 is a 't', it is a text file
 *        "     "     7 is a 'b', it is a binary file
 *
 *   Some files containing tokens are pure ascii files without a header
 *   (see comments in 'tsection_db.c').  To allow these to be read with
 *   the same token routines is also used to flag possible text files.
 *   So    texture =  1  :: texture file
 *                 = -1  :: if not an MSTS file assume ascii
 *                 =  0  :: normal MSTS file.
 * *****************************************************************************
 */

int open_msfile(char *filename, MSfile *msfile, int texture, int iprint){

  int  ip = 0 ;
  int  i, m, iret ;
  uint n ;
  int  utf16le ;
  char buffer[32] ;
  char string[33] ;
  FILE *fp ;
  char *my_name = "open_msfile" ;
/*
 *  Initialise tokens
 */
      iprint = ip = iprint || ip ;
      init_token() ;
      if(ip && n_open_files>0)
                         printf(" n_open_files = %i\n",n_open_files) ;
/*
 * Open File
 */
#ifdef MinGW
      fp = gopen(filename,"rb");
#else
      fp = gopen(filename,"r");
#endif
      if(fp==NULL){
#ifdef MinGW
        fp = gcaseopen(filename,"rb");
#else
        fp = gcaseopen(filename,"r");
#endif
        if(fp==NULL){
          printf(" File not found.  File = %s\n",filename);
          n = strlen(filename) ;
          for(i=0;i<(int)n;i++) printf("  Char %i  :%c:  :%x:\n",
                                            i,filename[i],filename[i]) ;
        }
      }
      if(fp==NULL) return 1 ;
/*
 *  Initialise msfile structure
 */
      init_msfile(msfile) ;           // Set defaults
      msfile->filename = (char *)malloc((strlen(filename)+1)*sizeof(char)) ;
      strcpy(msfile->filename,filename) ;
      msfile->fp = fp ;
      if(1 == texture)msfile->texture = 1 ;
/*
 *  Test for two-character unicode.  MSTS files, including the binary files,
 *  represent text and names either using ascii or two character unicode
 *  characters.  These routines assume that the second character of each
 *  two character uncode is a binary zero and so can be ignored.
 */
      buffer[0] = getc(fp);
      buffer[1] = getc(fp);
      utf16le = (char)buffer[0]==(char)0xff
                     && (char)buffer[1]==(char)0xfe ;
      msfile->utf16le = utf16le ;
      msfile->utf8    = utf16le ? 0 : 1 ;
/*
 *  Read first MSTS header
 *  Some unicode files start with blanks (!)
 */
      if(utf16le){
        for(i=0;i<256;i++){
          buffer[0] = getc(fp) ; getc(fp) ;
          if('S' == buffer[0])break;
        }
        for(i=1;i<16;i++){
          buffer[i] = getc(fp); getc(fp) ;
        }
      }else{
        for(i=2;i<16;i++){
          buffer[i] = getc(fp);
        }
      }
/*
 *  Check for compress
 */
      if(0 == strncmp(buffer,"SIMISA@F",8)){       // Compressed
        msfile->compress = 1 ;
        for(i=0;i<32;i++) string[i] = buffer[i] ;
        string[32] = '\0' ;
/*
 *  Read size of file
 *  File is little-endian so each following byte has a value 256 times larger
 */
        n = 0 ;
        m = 1 ;
        for(i=8;i<12;i++){
          n = n + m*(buffer[i] & 0xff) ;
          m = m*256 ;
          if(ip)printf(" BB i, m, n = %i %i %i \n",i,m,n);
        }

        if(ip)printf(" CC msfile->pr = %p  n = %i\n",(void *)msfile->fp,n);
        iret = zlib_uncompress(msfile,n,ip);
        fp = msfile->fp;
        if(ip)printf(" DD msfile->pr = %p\n",(void *)msfile->fp);

        if(iret != 0){
          printf("/n  Error uncompressing file %s\n",msfile->filename);
          printf(  "  Program Stopping ...\n");
          exit(1) ;
        }
      }else if(0 != strncmp(buffer,"SIMISA@@",8)){
//  Possible Ascii/text file
        if(-1 == texture){
          rewind(fp);
          if(utf16le){ getc(fp) ; getc(fp) ; }
          msfile->text = 1 ;                      // Give it a chance
          return 0 ;
// Error
        }else{
          printf("ERROR 1 : file header not recognised\n");
          printf("        :  File = %s\n",filename) ;
          strncpy(string,buffer,8); string[8] = 0;
          printf("  Header reads : %s\n",string);
          return 1;
        }
      }
/*
 *  Texture file
 */
      if(1 == texture){
/*
 * Print summary line
 */
        if(ip){
          printf("\n  File is texture file\n");
          printf(  "    filename = %s\n",filename);
          printf(  "    compress = %i\n",msfile->compress);
        }
        return 0 ;
      }
/*
 *  Read second MSTS header
 */
      if(utf16le){
        for(i=16;i<32;i++){
          buffer[i] = getc(fp); getc(fp) ;
        }
      }else{
        for(i=16;i<32;i++){
          buffer[i] = getc(fp);
        }
      }
/*
 *  Check for MSTS type text or binary file
 */
      if('t' == buffer[23]){
        msfile->text = 1 ;
      }else if('b' == buffer[23]){
        msfile->binary = 1;
      }else{
        printf("ERROR 2 : file header not recognised\n");
        strncpy(string,buffer,32); string[32] = '\0';
        printf("  Header reads : %s\n",string);
        return 2;
      }
/*
 *  Check for world file
 */
      if('w' == buffer[21]){
        msfile->world = 1 ;
      }
/*
 * Print summary line
 */
      if(ip){
        printf("\n  file       = %s\n",filename);
        printf("    utf16le  = %i,",msfile->utf16le);
        printf(  "  utf8     = %i,",msfile->utf8);
        printf(  "  compress = %i,",msfile->compress);
        printf(  "  text     = %i,",msfile->text);
        printf(  "  binary   = %i,",msfile->binary);
        printf(  "  world    = %i,",msfile->world);
        printf(" ms file ptr = %p\n",(void *)fp) ;

        if(msfile->utf16le)
          printf(" File is little-endian UTF-16 Unicode text"
                                  " with CRLF terminators.\n");
        if(msfile->utf8) printf(" File uses UTF-8 characters.\n");
      }

      return 0;
}
/*
 * *****************************************************************************
 * Close msfile
 *
 * "if NULL" statements used here and elsehwere to keep valgrind happy.
 * It should be possible to remove them.
 *
 * *****************************************************************************
 */
int close_msfile(MSfile *msfile)
{
  int i ;
      for(i=0;i<20;i++){
        if(NULL != msfile->level[i].token_name)
                                 free(msfile->level[i].token_name) ;
        if(NULL != msfile->level[i].label)
                                 free(msfile->level[i].label) ;
      }
      if(msfile->token_unused)free(msfile->token_unused) ;
      if(msfile->filename)free(msfile->filename) ;
      gclose(msfile->fp) ;
      return 0;
}

char * read_ucharz_a(MSfile *msfile){

int     i, n, l ;
int32_t c ;
FILE    *fp  = msfile->fp     ;
int     fbin = msfile->binary ;

      if(fbin){
        utf_char_temp = 0 ;
        utf_flag_temp = 0 ;
        l = 0 ;
        n = (int)read_uint16(fp) ;
        for(i=0;i<n;i++){
          c = fgetu(msfile) ;
          put_utf8( c, &l) ;
        }
        token[l] = '\0' ;
        return strdup(token) ;
      }
      return ctoken(msfile) ;
}
