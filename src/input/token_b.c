/*
 * *****************************************************************************
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File : token_b.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  Subroutines equivalent to those of token.c but for binary files.
 *
 *  The core section of MSTS data files have two standard formats.  The easiest
 *  to read are the ascii files which are structured along the lines of:
 *    token ( data data token ( more data ... ) more data and tokens  )
 *  The tokens are a series of ascii names and the data is in the form of
 *  ascii numbers or text.  Text consists of single words or is enclosed by
 *  quotes (").  All elements of the data are separated by white space except
 *  when the final quote followed immediately by plus ("+).  The text is then
 *  joined to the following text.
 *
 *  In the alternative binary form, the tokens are relaced by 16-bit integers,
 *  with the data in machine format.  In this form there is no equivalent
 *  of the white space.  The opening bracket is replaced by a 4-byte integer.
 *  This gives the length of the token data, starting from the initial 2-byte
 *  'name' and ending with a single byte containing 0xFF, which replaces the
 *  last bracket.  Tokens with data contained within this data stream may use
 *  a similar format.
 *
 *  In Openrails the programmer(s) responsible for SBR.cs have structured it
 *  so that the subroutine calls used in processing a data file are similar for
 *  both data types.
 *
 *  Here an attempt is made to start along these lines by using routine calls
 *  and names similar to those in file token.c but where the names have an
 *  extra "_b" to denote binary.  It is not obvious that particular file types
 *  are found in both ascii forms but, if so, it would be posible to read both
 *  types either by having both token.c and token_b.c calls within the
 *  data file processing subroutine or by having subroutine pointers within the
 *  'msfile' structure.  Once it was known whether the file contained ascii or
 *  binary these could be set to the correct set.  The processing subroutine
 *  would then use these pointers.
 *
 *  Routines in token_tb.c should be used where a file can be either binary
 *  or ascii.
 *
 * *****************************************************************************
 */

/*
 *   Section Z.  Binary read subroutines
 *   Specifications need to be included in zr.h (or functions.h).
 */

uint  read_uint8(FILE *fp){

union u_tag {
  uint  m ;
  char  ca[4] ;
}  u ;
//char  myname[] = "read_uint8";

      u.m = 0 ;
      u.ca[0] = getc(fp) ;
      return u.m ;
}

int  read_int16(FILE *fp){

int  i ;
union u_tag {
  int  m ;
  char ca[4] ;
}  u ;
//char  myname[] = "read_int16";

      u.m = 0 ;
      for(i=0;i<2;i++){
       u.ca[i] = getc(fp) ;
      }
      return u.m ;
}

uint  read_uint16(FILE *fp){

int  i ;
union u_tag {
  uint  m ;
  char  ca[4] ;
}  u ;
//char  myname[] = "read_uint16";

      u.m = 0 ;
      for(i=0;i<2;i++){
       u.ca[i] = getc(fp) ;
//       printf(" read_uint16 i= %i, ca = %c,  m = %X\n",i,u.ca[i],u.m) ;
      }
      return u.m ;
}


int  read_int32(FILE *fp){

int  i ;
union u_tag {
  int  m ;
  char ca[4] ;
}  u ;
//char  myname[] = "read_int32";

      u.m = 0 ;
      for(i=0;i<4;i++){
       u.ca[i] = getc(fp) ;
      }
      return u.m ;
}

uint  read_uint32(FILE *fp){

int  i ;
union u_tag {
  uint m ;
  char ca[4] ;
}  u ;
//char  myname[] = "read_uint32";

      u.m = 0 ;
      for(i=0;i<4;i++){
       u.ca[i] = getc(fp) ;
//      printf(" read_uint16 i= %i, ca = %c,  m = %X\n",i,u.ca[i],u.m) ;
      }
      return u.m ;
}

int  read_int64(FILE *fp){

int  i ;
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

float read_real32(FILE *fp){

int  i ;
union u_tag {
  float m ;
  char ca[4] ;
}  u ;
//char  myname[] = "read_real32";

      u.m = 0.0 ;
      for(i=0;i<4;i++){
       u.ca[i] = getc(fp) ;
      }
      return u.m ;
}

/*
 *   read_ucarz
 *   This is a byte reading routine designed to read the special unicode text
 *   in the *.t files and possibly others.  The string consists of int16 giving
 *   n, the number of characrters, followed by n unicode characters.
 */

char * read_ucharz(FILE *fp){

int  i, n  ;
char  *string ;

      n = (int)read_uint16(fp) ;
      string = (char *)malloc(n+1);  //  generate a new string
      for(i=0;i<n;i++){
        string[i] = getc(fp) ;
        getc(fp) ;
      }
      string[n] = '\0' ;
      return string ;
}

/*
 *  Routine to skip to the end of a block in an MSTS binary file
 */


void skip_to_bblock_end(FILE *fp, MSblock *block){

int i ;
      for(i=ftell(fp);i<block->byte_end;i++){
        fgetc(fp) ;
      }
      return ;
}
