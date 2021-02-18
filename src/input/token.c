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
 * *****************************************************************************
 */

#include <assert.h>                      // For zlib
#include "zlib.h"                        //  For zlib
#define CHUNK 16384                      //  For zlib

static char token[4096] ;
static int  rbr_buffered  ;
static int  skip_underscore_token(MSfile *msfile) ;

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
 *  new_token parses a new token from the input file
 *  It returns a pointer to its own storage array 'token'.
 *  The token must be copied or used before the next call to
 *  new_token.
 * *****************************************************************************
 */

char *new_token(MSfile *msfile){

int   l, c, lastc;
int   unicode, compress, text, binary ;
FILE  *fp ;
char  my_name[] = "new_token" ;

      fp       = msfile->fp        ;
      unicode  = msfile->unicode   ;
      compress = msfile->compress  ;
      text     = msfile->text      ;
      binary   = msfile->binary    ;

      if(text != 1){
        printf("\n  ERROR : Routine new_token : File type not supported\n");
        printf("  unicode  = %i\n",unicode);
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
        return token ;
      }

      if(0 != rbr_buffered){
        rbr_buffered = 0;
        token[0] = ')'  ;
        token[1] = '\0' ;
        return token ;
      }
/*
 *  Skip white space, check for EOF and non-standard tokens
 */
      for(;;){
        while((c=fgetc(fp)) != EOF && (c == ' '  || c == '\t' ||
                                      c == '\n' || c == '\r') ){
          if(unicode) getc(fp);
        }
        if(c == EOF){
          return strcpy(token,eof_mark) ;
        }
        if(unicode) fgetc(fp);
        if(c != '_')break  ;            //  Normal token
        l = skip_underscore_token(msfile) ; //  Special treatment routine
        if(1 == l ){
          return strcpy(token,eof_mark) ;
        }
      }
      l = 0 ;
      token[l++] = c ;
/*
 *  Open and closing brackets
 */
      if(c == '(' || c == ')'){
        token[1] = '\0' ;
        return token ;
      }
/*
 *  Normal tokens
 */
        if(c !='"'){
        while((c=fgetc(fp)) != EOF && c != ' ' && c != '\t' && c != '\n' && c!= '\r'
                          && c != '(' && c != ')' && l <4095 ){
          token[l++] = c ;
          if(unicode)c = fgetc(fp);
        }
        ungetc(c,fp) ;                         // Put back last character
//
//  Tokens that are a stream of text
//
      }else{
        lastc = ' ';
        for(;;){
// 1.  Search for matching '"'
          while(((c=getc(fp)) != '"' || lastc == '\\' ) && l <4094 ){
            if(c != '\r')token[l++] = c ;       // Skip line feed
            lastc = c ;
            if(unicode)getc(fp);
          }
          if(l>= 4094) break ;                  // Leave 'for' loop
          token[l++] = c ;
          if(unicode)getc(fp);
          c = getc(fp) ;
//  2.  Test for following '+'
          if(c != '+'){
            ungetc(c,fp);
            break ;                             // Leave 'for' loop
          }
          token[l++] = c ;
//  3.  Search for second starting '"'
          if(unicode)getc(fp);
          while((c=getc(fp)) != '"' && l <4094 ){
            if(c != '\r')token[l++] = c ;
            if(unicode)getc(fp);
          }
          if(l>= 4094) break ;                 // Leave 'for' loop
          token[l++] = c ;
          if(unicode)getc(fp);
        }
      }
      if(l>=4094){
        printf("\n  ERROR : Routine new_token : Token is > 4094 characters\n");
        printf(" Program exiting ... \n\n");
        exit(1) ;
      }
      token[l] = '\0';

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
 * *****************************************************************************
 *  Return double
 * *****************************************************************************
 */
double dtoken(MSfile *msfile){
       return atof(new_token(msfile)) ;
}

/*
 * *****************************************************************************
 * Search for left bracket and skip
 * *****************************************************************************
 */
int  skip_lbr(MSfile *msfile){
  char *token ;

    token = new_token(msfile);
    if( 0 == strcmp(token,eof_mark) ) return(-1) ;
    if( 0 != strcmp(token,"(") ){
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
 * *****************************************************************************
 * Check for right bracket
 * *****************************************************************************
 */
int  is_rbr(char *token){
    if (0 == strcmp(token,")") ) return 1 ;   // True
    else return 0 ;                           // False
}

/*
 *  Tokens starting with '_' may be used for comments.
 *  Here they are scanned and ignored.
 *  As far as is known that always contain matching
 *  brackets '(' and ')' but these may not be separated
 *  from other text by white space
 */

int  skip_underscore_token(MSfile *msfile){

int   n  ;
int   c  ;
int   unicode = msfile->unicode ;
FILE  *fp     = msfile->fp      ;
char  my_name[] = "skip_underscore_token" ;

// Skip rest of name
      while((c=fgetc(fp)) != EOF &&  c != ' '  && c != '\t'
                                 &&  c != '\n' && c != '\r' && c != '('){
        if(unicode) getc(fp);
      }
//  Check for EOF
      if(c == EOF){
        printf("\n  ERROR?? : Routine %s : EOF found\n",my_name);
        exit(0) ;
        return 1 ;
      }
//  Skip white space
      if('(' != c) {
        if(unicode) getc(fp);
        while((c=fgetc(fp)) != EOF &&  c == ' '  && c == '\t'
                                  &&  c == '\n' && c == '\r' ){
          if(unicode) getc(fp);
        }
      }
//  Return if opening bracket is not present
      if(c!='('){
        printf("  Routine %s : Token has no opening bracket\n",my_name) ;
        ungetc(c,fp) ;
        return 0 ;
      }
      if(unicode) getc(fp);
      n = 1 ;
      while(n != 0){
        if((c = fgetc(fp)) == EOF){
          printf("\n  ERROR?? : Routine %s : EOF found\n",my_name);
          printf("    c = %x,  EOF = %x\n",c,EOF);
          return 1 ;
        }  ;
        if(unicode) getc(fp) ;
        if(c == '(')n++ ;
        if(c == ')')n-- ;
      }
      return 0 ;
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
              ret = Z_DATA_ERROR;     /* and fall through */
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
 *  Open 'temporary' file
 */
      fp_out = tmpfile() ;
//      fp_out = gopen(tmp_file,"w+") ;  //  keep file at program end
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
      msfile->unicode   = 0    ;
      msfile->ascii     = 0    ;
      msfile->compress  = 0    ;
      msfile->text      = 0    ;
      msfile->binary    = 0    ;
      msfile->world     = 0    ;
      msfile->texture   = 0    ;
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
 *   * If it is a text file using unicode the file starts with the characters
 *       0xff and 0xfe, the order showing whether the file is big-endian or
 *       littel-endian.  If it is not unicode these characters are missing.
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

  int i, iret ;
  uint m, n ;
  int  unicode ;
  char buffer[32] ;
  char string[33] ;
  FILE *fp ;
/*
 *  Initialise tokens
 */
      init_token() ;
      if(iprint && n_open_files>0)printf(" n_open_files = %i\n",n_open_files) ;
/*
 * Open File
 */
      fp = gopen(filename,"r");
      if(fp==NULL){
        printf(" File not found\n");
        return 1 ;
      }
      init_msfile(msfile) ;           // Initialise the data structure

      msfile->filename = (char *)malloc((strlen(filename)+1)*sizeof(char)) ;
      strcpy(msfile->filename,filename) ;
/*
 *   Initialise flags
 */
      msfile->fp = fp ;
      msfile->unicode = msfile->ascii = msfile->compress = msfile->text
                                      = msfile->binary   = msfile->world = 0 ;
      if(1 == texture)msfile->texture = texture   ;
      msfile->token_unused = NULL ;    // Used when reading text files
/*
 *  Test for two-character unicode.  MSTS files, including the binary files,
 *  represent text and names either using ascii or two character unicode
 *  characters.  These routines assume that the second character of each
 *  two character uncode is a binary zero and so can be ignored.
 */
      buffer[0] = getc(fp);
      buffer[1] = getc(fp);
      unicode = (char)buffer[0]==(char)0xff
                     && (char)buffer[1]==(char)0xfe ;
      msfile->unicode = unicode ;
      msfile->ascii = unicode ? 0 : 1 ;
/*
 *  Read first MSTS header
 */
      if(unicode){
        for(i=0;i<16;i++){
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
 */
        n = 0 ;
        m = 1 ;
        for(i=8;i<12;i++){
          n = n + m*(buffer[i] & 0xff) ;
          m = m*256 ;
          if(iprint)printf(" BB i, m, n = %i %i %i \n",i,m,n);
        }

        if(iprint)printf(" CC msfile->pr = %p  n = %i\n",(void *)msfile->fp,n);
        iret = zlib_uncompress(msfile,n,iprint);
        fp = msfile->fp;
        if(iprint)printf(" DD msfile->pr = %p\n",(void *)msfile->fp);

        if(iret != 0){
          printf("/n  Error uncompressing file %s\n",msfile->filename);
          printf(  "  Program Stopping ...\n");
          exit(1) ;
        }
      }else if(0 != strncmp(buffer,"SIMISA@@",8)){
//  Possible Ascii/text file
        if(-1 == texture){
          rewind(fp);
          if(unicode){ getc(fp) ; getc(fp) ; }
          msfile->text = 1 ;                      // Give it a chance
          return 0 ;
// Error
        }else{
          printf("ERROR 1 : file header not recognised\n");
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
      if(iprint){
        printf("\n  file       = %s\n",filename);
        printf("    unicode  = %i,",msfile->unicode);
        printf(  "  ascii    = %i,",msfile->ascii);
        printf(  "  compress = %i\n",msfile->compress);
        if(msfile->unicode)
          printf(" File is little-endian UTF-16 Unicode text"
                                  " with CRLF terminators.\n");
        if(msfile->ascii) printf(" File is text file\n");
      }
        return 0 ;
      }
/*
 *  Read second MSTS header
 */
      if(unicode){
        for(i=16;i<32;i++){
          buffer[i] = getc(fp); getc(fp) ;
        }
      }else{
        for(i=16;i<32;i++){
          buffer[i] = getc(fp);
        }
      }
/*
 *  Check for normal text or binary file
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
      if(iprint){
        printf("\n  file       = %s\n",filename);
        printf("    unicode  = %i,",msfile->unicode);
        printf(  "  ascii    = %i,",msfile->ascii);
        printf(  "  compress = %i,",msfile->compress);
        printf(  "  text     = %i,",msfile->text);
        printf(  "  binary   = %i,",msfile->binary);
        printf(  "  world    = %i,",msfile->world);
        printf(" ms file ptr = %p\n",(void *)fp) ;

        if(msfile->unicode)
          printf(" File is little-endian UTF-16 Unicode text"
                                  " with CRLF terminators.\n");
        if(msfile->ascii) printf(" File is text file\n");
      }

      return 0;
}
/*
 * *****************************************************************************
 * Close msfile
 *
 * "if NULL" statements used here and elsehwere to keep valgrind happy.
 * It should be possible to remove them.
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
      free(msfile->token_unused) ;
      free(msfile->filename) ;
      gclose(msfile->fp) ;
      return 0;
}
