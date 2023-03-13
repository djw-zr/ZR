/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File texture.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File contains routines for loading textures into main memory
 *   Code based on comments and code in Acefile.cs
 *
 *   Some information on the different formats is given in README.ACE
 *
 *==============================================================================
 */


TextureNode *new_texture_struct(int width, int height,
                               GLenum mem_format, GLenum mem_packing,
                               char *filename, char *full_filename) ;
int  init_texture_node(TextureNode *texture)            ;
int  dxt1_2_rgba(unsigned int ia[2], unsigned char *ja) ;


enum colour_type{
    MASK  = 2,
    RED   = 3,
    GREEN = 4,
    BLUE  = 5,
    ALPHA = 6 } ;

int file_is_ace( const struct dirent *p){

  int len = strlen(p->d_name);
      if(len<5) return 0 ;
      if(strcmp_ic((char *)&p->d_name[len-4],".ace")== 0) return 1;
      return 0;
}

int load_texture_filenames() {

  int         idir, len1, len2, i, n, iret ;
  int         ip = 0 ;                // Control printing : 0 = no printing
  char        *tdir_name   = NULL ;
  DIR         *tdir        = NULL ;
  struct dirent *f_entry   = NULL ;
  struct dirent **namelist = NULL ;
  TextureNode *texture     = NULL ;
  char        myname[] = "load_texture_filenames" ;

/*
 *  1.  Search for world directory (world, WORLD)
 */
      if(ip){
        printf(" *******************************************************\n");
        printf(" Enter %s\n",myname);
        printf(" *******************************************************\n");
      }
#if 1
      for(idir=0;idir<2;idir++){
        if(idir==0){
          len1 = strlen(ORroutedir) + strlen("TEXTURES/") + 1 ;
          tdir_name = (char *)malloc(len1) ;
          strcpy(tdir_name,ORroutedir)    ;
          strcat(tdir_name,"TEXTURES/")      ;
        }else{
          len1 = strlen(ORdir) + strlen("Global/Textures/") + 1 ;
          tdir_name = (char *)malloc(len1) ;
          strcpy(tdir_name,ORdir)    ;
          strcat(tdir_name,"Global/Textures/")      ;
        }
#else
        tdir_name = current_dir ;
        len1 = strlen(tdir_name) + 1 ;
#endif
        iret = zr_find_msfile2(tdir_name) ;
        if(iret){
          printf(" Routine %s : ERROR : Unable to open TEXTURES directory\n",
                                                                myname) ;
          printf("    Directory name = %s\n", tdir_name) ;
          printf("    Program stopping ...\n") ;
          exit(1) ;
        }
        if(ip)printf(" Trying textures directory = %s\n",tdir_name) ;
        tdir = opendir(tdir_name) ;
        if(tdir == NULL){
          printf(" Program %s,\n",myname) ;
          printf("   Unable to find TEXTURES directory\n") ;
          printf("   Program stopping ... \n") ;
          exit(1) ;
        }
        closedir(tdir) ;
/*
 * 1.5  Generate an ordered list
 */
#ifdef MinGW
         n = scandir(tdir_name, &namelist, file_is_ace, NULL) ;
#else
         n = scandir(tdir_name, &namelist, file_is_ace, versionsort);
#endif
         if (n == -1) {
           perror("scandir");
           exit(EXIT_FAILURE);
         }
         printf("     Textures from directory %s\n",tdir_name );
//        for(i=0;i<n;i++){
//           printf("%s\n", namelist[i]->d_name);
//           free(namelist[i]);
//         }
//         free(namelist);

/*
 *  2.  Loop through files
 */
//        while ((f_entry = readdir(tdir)) != NULL) {
//          len2 = strlen(f_entry->d_name) ;
//          if(strcmp_ic(".ace",&(f_entry->d_name[len2-4]))!= 0) continue ;
//          if(ip)printf("  Found texture file = %s\n", f_entry->d_name);
/*
 * Initialise new texture node
 */
        for(i=0;i<n;i++){
          texture = (TextureNode *)malloc(sizeof(TextureNode)) ;
          if(texturelist_beg == NULL){
            texturelist_beg = texture       ;
          }else{
            texturelist_end->next = texture ;
          }
          texturelist_end = texture ;
          texture->next   = NULL ;
          init_texture_node(texture) ;
/*
 *  Save name and filename  (strip off 4 characters '.ace')
 */
          len2 = strlen(namelist[i]->d_name) ;
          texture->name = (char *)malloc(len2-3) ;
//          strncpy(texture->name,f_entry->d_name,len2-4);
          strncpy(texture->name,namelist[i]->d_name, len2-4);
          texture->name[len2-4] = '\0' ;
          if(ip)printf("                name = %s\n", texture->name);
/*
 *  Save full name of texture file
 */
          texture->filename = (char *)malloc(len1 + len2 ) ;
          strcpy(texture->filename,tdir_name) ;
//          strcat(texture->filename,f_entry->d_name);
          strcat(texture->filename,namelist[i]->d_name);
          free(namelist[i]) ;
        }
      free(namelist);
//      closedir(tdir) ;
      free(tdir_name);
      }
      return 0;
}

/*
 *  Routine to initialise texture structure
 */

int  init_texture_node(TextureNode *texture){

        texture->filename       = NULL ;
        texture->name           = NULL ;
        texture->basic          = 0 ;
        texture->needed         = 0 ;
        texture->loaded         = 0 ;
        texture->width          = 0 ;
        texture->height         = 0 ;
        texture->is_alpha       = 0 ;
        texture->is_mask        = 0 ;
        texture->alpha_is_mask  = 0 ;
        texture->needed         = 0 ;
        texture->n_textures     = 0 ;
        texture->texture        = NULL ;
        texture->surface_format = 0 ;
        texture->gl_mem_format  = ZR_ERROR ;
        texture->gl_mem_packing = ZR_ERROR ;
        texture->gl_tex_ref_no  = 0 ;
      return 0 ;
}



/*
 *==============================================================================
 * Routine to read MTST style texture files
 *
 * There appear to be a number of possible formats, some of which may
 * not be open-source.  See file README.TEXTURES for details.
 *==============================================================================
 */
int load_texture(TextureNode *tnode){

  int     i, j, k, l, m, n ;
  int     ip = 0   ;  // 0 = no printing
  int   width     ;
  int   height    ;
  int   mipmaps   ;
  int   raw_data  ;
  int   options   ;
  int   surface_format ;
  int   channel_count  ;
  int   image_count    ;
  char  signature[5]   ;
  int   icol           ;
  char  c ;

  MSfile  msfile ;
  FILE   *fp ;
  char    myname[] = "load_texture" ;

//      ip = !strncmp(tnode->name,"brake3",6) ;

/*
 *  open_msfile reads and checks the first 16 bytes of the texture file
 *  and sets the compress flag.
 */
      if(ip){
        printf("\n  Read texture  : %s\n",tnode->name) ;
        printf(  "    == filename : %s\n",tnode->filename) ;
      }
/*
 *  Call open_msfile with third parameter = 1 to indicate a texture file.
 */
      l = open_msfile(tnode->filename, &msfile, 1, 0);

      if(l!=0){
        printf("\n\n  ERROR : Routine open_msfile failed to open file\n\n");
        exit(1) ;
      }
      if(ip)printf(" msfileptr  = %p\n",(void *)msfile.fp);
      fp = msfile.fp ;

//      if(1 != msfile.compress){
//        printf("  Routine %s expects files to be compress.\n",myname);
//        printf("    Program stopping ... \n");
//        exit(1) ;
//      }
/*
 *  Check file signature
 */
      for(i=0;i<4;i++) signature[i]  = (char)getc(fp);
      signature[4] = (char)'\0' ;
      if(0 != strcmp("\x01\x00\x00\x00",signature)){
        printf("  Routine %s. Wrong signature.\n", myname);
        printf("    Signature is %x %x %x %x \n",
                        signature[0],signature[1],signature[2],signature[3]);
        printf("    Program stopping ... \n");
        exit(1) ;
      }
      if(ip)printf("    Signature is %x %x %x %x \n",
                            signature[0],signature[1],signature[2],signature[3]);
/*
 *  Read basic data
 */

      options = read_int32(fp) ;
      tnode->width  = read_int32(fp) ;
      tnode->height = read_int32(fp) ;
      tnode->surface_format = read_int32(fp) ;
      channel_count = read_int32(fp) ;
      width          = tnode->width  ;
      height         = tnode->height ;
      surface_format = tnode->surface_format ;
      if(ip){
        printf("    options = %8.8x \n",options);
        printf("    width   = %i \n",tnode->width);
        printf("    height  = %i \n",tnode->height);
        printf("    surface_format = %i %x\n",tnode->surface_format,
                                              tnode->surface_format);
        printf("    channel_count  = %i \n",channel_count);
      }
/*
 *  Skip unwanted data
 */
      for(i=0;i<128;i++) getc(fp) ;
/*
 *  Check for mipmaps.
 *  If present check that the image is square and each dimension
 *  is an integral power-of-two.
 */
      image_count = 1 ;
      mipmaps = options & 0x01 ;
      if(mipmaps){
        if(ip)printf("  File contains mipmaps\n");
        if(width != height){
          printf("  Texture file : tnode->name\n");
          printf("    contains mipmaps but the height and with do not match.\n") ;
          printf("    width = %i,  height = %i.\n", width, height) ;
          printf("  Routine %s exiting without loading texture.\n",myname) ;
          close_msfile(&msfile) ;
          free(tnode->name)  ;
          tnode->name = NULL ;
          return 1  ;
        }
        if((width & (width-1)) != 0){
          printf("  Texture file : %s\n",tnode->name);
          printf("    contains mipmaps but the width is not a power of 2.\n") ;
          printf("    width = %i,  height = %i\n", width, height) ;
          printf("  Routine %s exiting without loading texture\n",myname) ;
          close_msfile(&msfile) ;
          free(tnode->name)  ;
          tnode->name = NULL ;
          return 1  ;
        }
        for(i=width,image_count = 0;i>0;i=i>>1,image_count++) ;
        if(ip)printf(" image_count = %i\n",image_count) ;
      }
/*
 *  Check for raw data file
 */

      raw_data = (0x10 & options) != 0  ;
      if(raw_data && ip){
        printf("  File contains raw data\n");
      }
/*
 *  Check that surface format is supported
 */
      if(surface_format != 0x0e && surface_format != 0x10 &&
         surface_format != 0x11 && surface_format != 0x12){
        printf("  Texture file : '%s.ace',      surface format option = %x."
               "  Option not supported.\n", tnode->name, surface_format);
        printf("    options = %8.8x \n",options);
        printf("    width   = %i \n",tnode->width);
        printf("    height  = %i \n",tnode->height);
        printf("    surface_format = %i %x\n",tnode->surface_format,
                                              tnode->surface_format);
        printf("    channel_count  = %i \n",channel_count);
        printf("    raw_data       = %i \n",raw_data);
        printf("    image_count    = %i \n",image_count);
        close_msfile(&msfile) ;
        free(tnode->name)  ;
        tnode->name = NULL ;
        return 1;
      }
/*
 *  Check channel count
 */
int  size[10] ;
enum colour_type type[10] ;      //  Allow for maximum of 10
      if(channel_count>10){
        printf("  Routine %s, Channel_count (= %i) exceeds limit of 10\n",myname, (int)channel_count);
        return 1 ;
      }
/*
 *  Read channel data
 */
int   is_alpha = 0;
int   is_mask  = 0;
      for(i=0;i<channel_count;i++){
        size[i] = read_int64(fp) ;
        type[i] = read_int64(fp) ;
//        if(type[i] == colour_type(ALPHA)) is_alpha = 1 ;
//        if(type[i] == colour_type(MASK))  is_mask  = 1 ;
        if(type[i] == ALPHA) is_alpha = 1 ;
        if(type[i] == MASK)  is_mask  = 1 ;
        if(ip)printf(" Channel %i,  size = %i, type = %i\n",i,size[i],type[i]);
      }
      tnode->is_alpha = is_alpha ;
      tnode->is_mask  = is_mask  ;
      if(ip)printf(" Flags  raw_data = %i  mipmap = %i  alpha = %i  mask = %i  s_format = %2x  channels = %i,  image_count = %i, compress = %i, name = %s\n",
           raw_data, mipmaps, is_alpha, is_mask, surface_format,channel_count,
           image_count, msfile.compress, tnode->name);
/*
 *  Create texture pointer array
 *  This is of length one - unless mipmaps is set.
 */
//  Free any allocated memory
      if(tnode->n_textures != 0){
        for(i=0;i<tnode->n_textures;i++) free(tnode->texture[i]) ;
        free(tnode->texture)  ;
        tnode->n_textures = 0 ;
      }
//  Allocate new texture pointer array
      tnode->texture = (GLubyte **)malloc(image_count*sizeof(GLubyte **)) ;
      tnode->n_textures = image_count ;
      for(i=0;i<image_count;i++)tnode->texture[i] = NULL ;
/*
 *==============================================================================
 *   Read Raw data
 *
 *   Used in MSTS to  store 0x12 (Compressed Dxt1) images
 *   Dxt1 compuression uses 64 bits to store colours in a 4x4 block of pixels
 *   First 32 bits contains 2 colours stored as RGB in 5, 6 and 5 bits.
 *   All cells white = 0xFF 0xFF 0xFF 0xFF 0x00 0x00 0x00 0x00
 *   All cells black = 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
 *   All cells grey  = 0x84 0x10 0x84 0x10 0x00 0x00 0x00 0x00
 *   All cells red   = 0xF8 0x00 0xF8 0x00 0x00 0x00 0x00 0x00
 *==============================================================================
 *     Raw data starts with a table of 32-bit integers, each giving the offset
 *       inbytes from the start of the table to the start of the corresponding
 *       mipmap.
 *     If the mipmap width is 4 or larger, the data is stored as raw
 *       data following an initial 32-bit integer giving the number of raw
 *       data elements.  In most cases (surface formats 0x0e, 0x10, 0x11, 0x12,
 *       0x14, 0x16) each element is a two byte integer.  However other
 *       formats may use four byte elements (i.e C# formats: SurfaceFormat.Colour
 *       and NormalizedByte4).
 *     Coding for the 2x2 and 1x1 mipmaps is not clear.  The OR C# code says both
 *       that it is stored as a 32-bit ARGB block and as RGB data.  Coding may
 *       depend on the format (mem_format).  Here, as it is not clear, it is
 *       set to four zero bytes.  This should either correspond to black or
 *       transparent black.
 *==============================================================================
 */
/*
 *  Read table of offsets and convert to bytes in each mipmap
 */
      if(raw_data) {
/*
 *  Check raw_data supported
 */
      if(surface_format != 0x12){
        printf("    options = %8.8x \n",options);
        printf("  Texture file : '%s.ace',      surface format option = %x,  Option not supported for raw data files.\n",
                         tnode->name, surface_format);
        printf("    width   = %i \n",tnode->width);
        printf("    height  = %i \n",tnode->height);
        printf("    surface_format = %i %x\n",tnode->surface_format,
                                              tnode->surface_format);
        printf("    channel_count  = %i \n",channel_count);
        printf("    raw_data       = %i \n",raw_data);
        printf("    image_count    = %i \n",image_count);
//        return 1;
      }
      if(ip)printf("  Reading raw data\n");

int  *tex_size = (int *)malloc(image_count*sizeof(int)) ;

        for(i=0;i<image_count;i++){
          tex_size[i] = read_int32(fp) ;
          if(i>0)tex_size[i-1] = tex_size[i]-tex_size[i-1] - 4 ;
        }
        if(ip){
          for(i=0;i<image_count;i++)
            printf(" AA mipmap %i, size = %i bytes\n",i, tex_size[i]) ;
        }
        if(mipmaps && (tex_size[image_count-2] == 4))tex_size[image_count-1] = 4 ;
        if(ip){
          for(i=0;i<image_count;i++)
            printf(" BB mipmap %i, size = %i bytes\n",i, tex_size[i]) ;
        }
/*
 * read mipmaps
 */
int     w = width ;
        for(i=0;i<image_count;i++,w=w>>1){
/*
 *  Allocate memory
 */
          if(w>2){
            tnode->texture[i] = (GLubyte *)malloc(tex_size[i]*sizeof(GLubyte));
            l = read_uint32(fp);
            if(ip)printf("  Mipmap image %2i, width = %3i, size = %5i,"
                         " bytes %5i, texture[i] = %p\n",
                         i, w, l, tex_size[i], (void *)tnode->texture[i] ) ;
            if(l != tex_size[i]){
              printf("  Error while processing texture file %s.ace \n",tnode->name) ;
              printf("    l != tex_size[i] \n");
              printf("    i = %i,  l = %i, tex_size[i] = %i\n",i,l,tex_size[i]) ;
              printf("    Surface format option = %x\n", surface_format);
#if 0
              printf(" Next 100 bytes:\n");
              for(i=0;i<100;i++){
                 c = fgetc(fp);
                 if(feof(fp))break ;
                 printf(" I = %i,  byte = %2x\n",i,(uint)(c&0xff));
              }
#endif
              close_msfile(&msfile) ;
              free(tnode->name)  ;
              tnode->name = NULL ;
              return 1 ;
            }
            for(j=0;j<tex_size[i];j++){
              c = fgetc(fp) ;
              if(feof(fp)){
                printf(" EOF while reading texture data\n");
                printf("    Texture loading aborted\n") ;
                close_msfile(&msfile) ;
                free(tnode->name)  ;
                tnode->name = NULL ;
                return 1;
              }
              tnode->texture[i][j] = c ;
            }
// 2x2 and 1x1 images often missing or badly formed - so set to eight zero bytes
// LATER:  Logic to detect errors and/or make averages of 4x4 image
          }else{
            if(ip)printf("  Mipmap image %2i, width = %i. Set to eight zero bytes\n",i,w);
            tnode->texture[i] = (GLubyte *)malloc(8*sizeof(GLubyte));
#if 0
            tnode->texture[i][0] = 0x84 ;  // Grey
            tnode->texture[i][1] = 0x10 ;
            tnode->texture[i][2] = 0x84 ;
            tnode->texture[i][3] = 0x10 ;
            tnode->texture[i][4] = 0x00 ;
            tnode->texture[i][5] = 0x00 ;
            tnode->texture[i][6] = 0x00 ;
            tnode->texture[i][7] = 0x00 ;
#endif
#if 1
            tnode->texture[i][0] = 0xF8 ;  // Red
            tnode->texture[i][1] = 0x00 ;
            tnode->texture[i][2] = 0xF8 ;
            tnode->texture[i][3] = 0x00 ;
            tnode->texture[i][4] = 0x00 ;
            tnode->texture[i][5] = 0x00 ;
            tnode->texture[i][6] = 0x00 ;
            tnode->texture[i][7] = 0x00 ;
#endif
#if 0
//            for(j=0;j<8;j++,k++) tnode->texture[i][j] = 0 ;
            for(j=0;j<8;j++) tnode->texture[i][j] = 0 ;
//            for(j=0;j<8;j++,k++) tnode->texture[i][j] = 0xFF ;
#endif
          }
        }
        free(tex_size) ;
/*
 *==============================================================================
 *  Read structured data
 *  These have RGB or RGBA pixel data backed into two bytes:
 *     0x0E :  R5  G5  B5
 *     0x10 :  R5  G5  B5  A1
 *     0x11 :  R4  G4  B4  A4
 *==============================================================================
 *
 *  This starts with a table of 32 bit offsets to each scan line for each image
 *  So skip over the table
 *==============================================================================
 */
      }else{
/*
 *  Check structured data supported
 */
      if(ip)printf("  Reading structured data\n");
      if(surface_format != 0x0e && surface_format != 0x10 && surface_format != 0x11){
        printf("  Texture file : '%s.ace',      surface format = %x,  Raw data texture not supported for structured files.\n",
                         tnode->name, surface_format);
        printf("    options = %8.8x \n",options);
        printf("    width   = %i \n",tnode->width);
        printf("    height  = %i \n",tnode->height);
        printf("    surface_format = %i %x\n",tnode->surface_format,
                                              tnode->surface_format);
        printf("    channel_count  = %i \n",channel_count);
        printf("    raw_data       = %i \n",raw_data);
        printf("    image_count    = %i \n",image_count);
        for(n=0;n<channel_count;n++)printf("  channel  %i  size = %i, type = %i\n",
                                       n,size[n],type[n]);
      }
/*
 *==============================================================================
 *  Loop over 'image_count' images
 *==============================================================================
 */
int ioff ;
        m = 1 ;
        for(i=0;i<image_count;i++){                 // Number if mipmap images
          n = tnode->height/m ;
          m = m * 2 ;
          if(ip)printf("  Image %i,  width = %i\n",i,n) ;
          for(j=0;j<n;j++){
            ioff = read_int32(fp) ;    // Offsets to each scan line
            if(ip)printf("   Line %4i, offset = %4x  %i\n",j, ioff, ioff);
          }
        }

        tnode->gl_mem_format   = GL_RGBA          ;
        tnode->gl_mem_packing  = GL_UNSIGNED_BYTE ;
/*
 *  Data is stored as RGB if no mask or alpha field is present
 *  otherwise it is stored as RGBA, using the alpha field if present
 *  or by converting mask to transparent black if alpha is not present.
 */
int   w = width, nb, h, nwords ;
        nb = 3 ;
        for(n=0;n<channel_count;n++){
          if(type[n] == 2 || type[n] == 6) nb = 4 ;  // Mask or alpha present
        }
//        if(surface_format == 0x0e) return 0 ;
/*
 *==============================================================================
 *  Loop over mipmaps
 *==============================================================================
 */
        for(i=0;i<image_count;i++,w=w>>1){
/*
 *  Calculate size of each image.
 *  If no mipmaps allow width and height to differ.
 */
          h  = (mipmaps ? w : height) ;
//          nwords = (w*h <4) ? 4 : w*h ;         // 1x1 mipmaps need 2x2 memory
          nwords = (w*h <8) ? 8 : w*h ;         // Ensure on 8 byte boundary
//  Possible problem calloc
          tnode->texture[i] = (GLubyte *)calloc(nb*nwords,sizeof(GLubyte));
          if(ip)printf("  Memory for image %i, nb = %i, w = %i, h = %i,"
                  " nwords = %i, size = %i, texture[i] = %p\n",
                  i,nb,w,h,nwords, nb*nwords,tnode->texture[i]);
/*
 *  Loop over the scan rows
 */
          for(j=0;j<h;j++){
            if(ip)printf("  Read image %i, row %i, channel ",i,j);
/*
 * Loop over channels
 */
            for(k=0;k<channel_count;k++){
              if(type[k] == RED){
                icol = 0 ;
              }else if(type[k] == GREEN){
                icol = 1 ;
              }else if(type[k]== BLUE ){
                icol = 2 ;
              }else{
                icol = 3 ;
              }
              if(ip)printf(" %i, %i :: ",type[k], size[k]) ;
/*
 *   8-bit data
 *   NOTE:  getc should check for EOF.
 *   EOF has the value -1 so cannot be represented in a 'char' variable.
 *   Instead either read into an 'int' or call 'feof(fp)' after every read.
 */
              if(8 == size[k]){
                  if((type[k] == MASK) && is_alpha){
                    for(l=0;l<w;l++) getc(fp); // Skip if MASK and is_alpha
                  }else{
                    for(l=0;l<w;l++){
                      tnode->texture[i][(j*w+l)*nb + icol] = getc(fp);
                    }
                  }
/*
 *   1-bit data : assume 1 => opaque, 0 = fully transparent.
 *   The data is packed 8-bits to a character, starting at the most significant bit
 */
              }else{
                if((type[k] == MASK) && is_alpha){
                  for(l=0;l<1+(w-1)/8;l++) getc(fp);   // Skip ...
                }else{
                  for(l=0;l<1+(w-1)/8;l++){
                    c = getc(fp);
                    for(m=0;m<8;m++){
                      if( m >= w - l*8 - 1) break ;
#if 0
                      if(((j*w + l*8 + m)*4 + 3)>nb*w*h-1){
                        printf(" ERROR nb = %i, w = %i, h = %i nb*w*h = %i\n",
                                         nb, w, h, nb*w*h);
                        printf(" ERROR j= %i, k = %i, l = %i, m = %i, "
                               "(j*w + l*8 + m)*4 + 3 = %i\n",
                                         j,k,l,m,(j*w + l*8 + m)*4 + 3);
                      }
#endif
                      if(surface_format == 0x10 && 0){
                        tnode->texture[i][(j*w + l*8 + m)*4 + 3]
//                                              = 0xff ;
                                              = ((c>>(7-m)) & 1) ? 0 : 0xff ;
                      }else{
                        tnode->texture[i][(j*w + l*8 + m)*4 + 3]
                                              = ((c>>(7-m)) & 1) ? 0xff : 0 ;
                      }
                    }
                  }
                }
              }    // 8-bit or 1-bit
            }      // Loop over channels
            if(ip)printf("\n");
          }        // Loop over rows
        }          // Loop over images
/*
 *  For structure file test level 1 for use of alpha as a mask
 *    False if alpha is missing
 *    True if more than one alpha equal to 0 and no alphas between 0 and 1
 */
      }
      close_msfile(&msfile) ;

      return 0 ;
}

/*
 *  Routine to sort the wagon textures
 */

static int  compare_tx_names(const void *p1, const void *p2){

TextureNode **s1, **s2, *t1, *t2 ;

      s1 = (TextureNode **)p1 ;
      s2 = (TextureNode **)p2 ;
      t1 = (TextureNode *)*s1 ;
      t2 = (TextureNode *)*s2 ;

//      printf("  compare  %p %p :: %p %p:: %p %p :: %s %s\n",p1,p2,(void *)s1,(void *)s2,
//             (void *)t1, (void *)t2,t1->name,t2->name) ;

      return strcmp(t1->name, t2->name) ;
}

int sort_textures(TextureNode **texturelist_beg){

int n_textures, i,
    ip = 0       ;
TextureNode *tx,
            **ta ;
char *my_name="sort_textures" ;

      if(ip){
        printf("  Enter routine %s\n",my_name) ;
        printf("    texturelist_beg = %p\n",(void *)texturelist_beg) ;
        fflush(NULL) ;
      }

//      printf(" AA \n") ; fflush(NULL) ;
      tx = *texturelist_beg;
//      printf(" BB \n") ; fflush(NULL) ;
//      printf("  tx = %p\n", (void *)tx) ; fflush(NULL) ;

      n_textures = 0 ;
      for(tx = *texturelist_beg; tx != NULL; tx = tx->next){
//        printf("  i  = %i\n", n_textures) ; fflush(NULL) ;
//        printf("  tx = %p\n", (void *)tx) ; fflush(NULL) ;
        n_textures++ ;
      }

      ta = (TextureNode **)malloc(n_textures*sizeof(TextureNode *)) ;
      for(tx = *texturelist_beg, i=0; tx != NULL; tx = tx->next, i++) ta[i] = tx ;

//      if(ip)for(i=0;i<n_textures;i++){
//        printf(" CC %p ta = %p,  name = %s\n",(void *)ta,(void *)ta[i], ta[i]->name) ;
//      }

      qsort(ta, n_textures, sizeof(TextureNode *), compare_tx_names) ;

      *texturelist_beg = ta[0] ;
      for(i=1;i<n_textures;i++) ta[i-1]->next = ta[i] ;
      ta[n_textures-1]->next = NULL ;

      if(ip){
        printf(" n_textures = %i\n",n_textures) ;
        for(tx = *texturelist_beg,i=0; tx != NULL; tx = tx->next,i++)
          printf(" %i  Tx = %p  ::  Texture = %s\n",i,(void *)tx,tx->name) ;
      }
      free(ta) ;

      return 0 ;
}
