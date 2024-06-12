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
  char        *texture_dir   = "/TEXTURES" ;
  char        *terrtex_dir   = "/TerrTex" ;
  char        *g_texture_dir = "/Global/Textures" ;
  char        my_name[] = "load_texture_filenames" ;

/*
 *  1.  Search for world directory (world, WORLD)
 */
      if(ip){
        printf(" *******************************************************\n");
        printf(" Enter %s\n",my_name);
        printf(" *******************************************************\n");
      }
#if 1
      for(idir=0;idir<3;idir++){
        if(0 == idir){
          len1 = strlen(ORroutedir) + strlen(texture_dir) + 1 ;
          tdir_name = (char *)malloc(len1) ;
          strcpy(tdir_name,ORroutedir)    ;
          strcat(tdir_name,texture_dir)      ;
        }else if(1 == idir){
          len1 = strlen(ORdir) + strlen(g_texture_dir) + 1 ;
          tdir_name = (char *)malloc(len1) ;
          strcpy(tdir_name,ORdir)    ;
          strcat(tdir_name,g_texture_dir)      ;
        }else{
          len1 = strlen(ORroutedir) + strlen(terrtex_dir) + 1 ;
          tdir_name = (char *)malloc(len1) ;
          strcpy(tdir_name,ORroutedir)    ;
          strcat(tdir_name,terrtex_dir)      ;
        }
#else
        tdir_name = current_dir ;
        len1 = strlen(tdir_name) + 1 ;
#endif
        iret = zr_find_msfile2(&tdir_name) ;
        if(iret){
          printf(" Routine %s : ERROR : Unable to open TEXTURES directory\n",
                                                                my_name) ;
          printf("    Directory name = %s\n", tdir_name) ;
          printf("    Program stopping ...\n") ;
          exit(1) ;
        }
        if(ip)printf(" Trying textures directory = %s\n",tdir_name) ;
        tdir = opendir(tdir_name) ;
        if(tdir == NULL){
          printf(" Program %s,\n",my_name) ;
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
 *  Loop over filenames
 *  Skip files in TERRTEX directory with tile-like names
 */
        for(i=0;i<n;i++){
          if(idir > 1 && (namelist[i]->d_name[0] == '_'
                       ||  namelist[i]->d_name[0] == '-' )) continue ;
/*
 * Initialise new texture node
 *   Skip files in TERRTEX directory with tile-like names
 */
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
          texture->filename = (char *)malloc(len1 + len2 + 2) ;
          strcpy(texture->filename,tdir_name) ;
          strcat(texture->filename,"/") ;
          strcat(texture->filename,namelist[i]->d_name);
          if(namelist[i])free(namelist[i]) ;
        }
        if(namelist)free(namelist);
//      closedir(tdir) ;
        if(tdir_name)free(tdir_name);
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

  int   i, j, k, l, m, n ;
  int   ip = 0    ;  // 0 = no printing
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
  int   is_alpha = 0 ;
  int   is_mask  = 0 ;
  int   nz_alpha     ;
  int   nz_mask      ;
  unsigned char  c, d  ;
  GLubyte *maskp = NULL ;   //  Pointer to mask arrays

  MSfile  msfile ;
  FILE   *fp ;
  char    my_name[] = "load_texture" ;
/*
 *  open_msfile reads and checks the first 16 bytes of the texture file
 *  and sets the compress flag.
 */
//      ip = !strcmp(tnode->name,"ACleanTrackBase") ;
      if(ip){
        printf("\n  Read texture:\n") ;
        printf(  "    == name     : %s\n",tnode->name) ;
        printf(  "    == filename : %s\n",tnode->filename) ;
      }
//      ip = !strcmp(tnode->name,"hhp") ;
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

/*
 *  Check file signature
 */
      for(i=0;i<4;i++) signature[i]  = (char)getc(fp);
      signature[4] = (char)'\0' ;
      if(0 != strcmp("\x01\x00\x00\x00",signature)){
        printf("  Routine %s. Wrong signature.\n", my_name);
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

      options        = read_int32(fp) ;
      tnode->width   = read_int32(fp) ;
      tnode->height  = read_int32(fp) ;
      tnode->surface_format = read_int32(fp) ;
      channel_count  = read_int32(fp) ;
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
          printf("  Routine %s exiting without loading texture.\n",my_name) ;
          close_msfile(&msfile) ;
          free(tnode->name)  ;
          tnode->name = NULL ;
          return 1  ;
        }
        if((width & (width-1)) != 0){
          printf("  Texture file : %s\n",tnode->name);
          printf("    contains mipmaps but the width is not a power of 2.\n") ;
          printf("    width = %i,  height = %i\n", width, height) ;
          printf("  Routine %s exiting without loading texture\n",my_name) ;
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
        printf("  Routine %s, Channel_count (= %i) exceeds limit of 10\n",my_name, (int)channel_count);
        return 1 ;
      }
/*
 *  Read channel data
 */
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
      if(ip)printf(" Flags  raw_data = %i  mipmap = %i  alpha = %i  mask = %i  s_format = %2x  channels = %i,  image_count = %i, compress = %i, filename = %s\n",
           raw_data, mipmaps, is_alpha, is_mask, surface_format,channel_count,
           image_count, msfile.compress, tnode->filename);
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
 *   'Raw Data' is used by MSTS to store 0x12 (Compressed Dxt1) images
 *   Dxt1 compuression uses 64 bits to store colours in a 4x4 block of pixels
 *   First 32 bits contains 2 colours stored as RGB in 5, 6 and 5 bits.
 *
 *   All cells white = 0xFF 0xFF 0xFF 0xFF 0x00 0x00 0x00 0x00
 *   All cells black = 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
 *   All cells grey  = 0x84 0x10 0x84 0x10 0x00 0x00 0x00 0x00
 *   All cells red   = 0xF8 0x00 0xF8 0x00 0x00 0x00 0x00 0x00
 *
 *   Note:  Although Ox12 uses the above scheme, the file header usually
 *          indicates that colour data is stored as 8 bytes and mask data
 *          as 1 byte.
 *
 *==============================================================================
 *
 *     Raw data starts with a table of 32-bit integers, each giving the offset
 *       in bytes from the start of the table to the start of the corresponding
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

int  *tex_offset = (int *)malloc(image_count*sizeof(int)) ;
int  tex_size ;

        for(i=0;i<image_count;i++){
          tex_offset[i] = read_int32(fp) ;
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
            tex_size = read_uint32(fp);
            tnode->texture[i] = (GLubyte *)malloc(tex_size*sizeof(GLubyte));
            if(ip)printf("  Mipmap image %2i, width = %3i, size = %5i,"
                         " texture[i] = %p\n",
                         i, w,  tex_size, (void *)tnode->texture[i] ) ;

            for(j=0;j<tex_size;j++){
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
#if 1
            tnode->texture[i][0] = 0x84 ;  // Grey
            tnode->texture[i][1] = 0x10 ;
            tnode->texture[i][2] = 0x84 ;
            tnode->texture[i][3] = 0x10 ;
            tnode->texture[i][4] = 0x00 ;
            tnode->texture[i][5] = 0x00 ;
            tnode->texture[i][6] = 0x00 ;
            tnode->texture[i][7] = 0x00 ;
#else
            tnode->texture[i][0] = 0xF8 ;  // Red
            tnode->texture[i][1] = 0x00 ;
            tnode->texture[i][2] = 0xF8 ;
            tnode->texture[i][3] = 0x00 ;
            tnode->texture[i][4] = 0x00 ;
            tnode->texture[i][5] = 0x00 ;
            tnode->texture[i][6] = 0x00 ;
            tnode->texture[i][7] = 0x00 ;
#endif
          }
        }
        free(tex_offset) ;
      }else{
/*
 *==============================================================================
 *  Read structured data
 *
 *  Although the files on disk are usually compressed, once the file is
 *  uncompressed  structured data (types 0x0E, 0x10 and 0x11) has as one
 *  byte per colour or alpha channel and (usually) one bit per mask channel.
 *
 *  Type 0x11 files often have both alpha and mask channels, with the mask set
 *  to all zeroes or both channels set to all zeroes (implying a pure
 *  transparency).  In the first case, the alpha channel is used.  In the
 *  second case, alpha is set to 0XFF everywhere (solid colour).
 *
 *  Although the file format uses full bytes for each variable, the definitions
 *  of the three channels imply that for transfer to graphics, structured data
 *  is designed to be packed into two bytes, the number of bits used for
 *  each channel being:
 *
 *     0x0E :  R5  G5  B5
 *     0x10 :  R5  G5  B5  A1
 *     0x11 :  R4  G4  B4  A4
 *
 *==============================================================================
 *
 *  Structured data starts with a table of 32 bit offsets to each scan line
 *  for each image
 *
 *==============================================================================
 */
/*
 *  First check that the structured data supported
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
          if(ip)printf("  Image %i,  height = %i\n",i,n) ;
          for(j=0;j<n;j++){
            ioff = read_int32(fp) ;    // Offsets to each scan line
            if(ip)printf("   Line %4i, offset = %4x  %i\n",j, ioff, ioff);
          }
        }

        tnode->gl_mem_format   = GL_RGBA          ;
        tnode->gl_mem_packing  = GL_UNSIGNED_BYTE ;
/*
 *  Data can arrive as RGB, RGBA or RGB and mask, where teh colours are
 *  8-bit bytes and the mask is a 1-bit field.
 *
 *  Data is thenstored internally as RGBA, using the alpha field if present
 *  or by converting mask to transparent/solid if alpha is not present
 *  or by setting alpha to solid colour (0xFF).
 */
int   w = width, nb, h, nwords ;
        nb = 3 ;
        for(n=0;n<channel_count;n++){
          if(type[n] == 2 || type[n] == 6) nb = 4 ;  // Mask or alpha present
        }
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
          nwords = (w*h <8) ? 8 : w*h ;         // Ensure on 8 byte boundary
          nz_alpha = nz_mask = 0 ;
          if(maskp)free(maskp) ;
//  calloc = malloc with memory set to zero.
          tnode->texture[i] = (GLubyte *)calloc(nb*nwords,sizeof(GLubyte));
          maskp             = (GLubyte *)calloc(nwords,sizeof(GLubyte));
#if 0
          if(ip)printf("  Memory for image %i, nb = %i, w = %i, h = %i,"
                  " nwords = %i, size = %i, texture[i] = %p\n",
                  i,nb,w,h,nwords, nb*nwords,tnode->texture[i]);
#endif
/*
 *  Loop over the scan rows
 */
          for(j=0;j<h;j++){
#if 0
            if(ip)printf("  Read image %i, row %i, channel ",i,j);
#endif
/*
 *  Initialise alpha channel (for 0x0e ony??)
 */
            if(4 == nb){
              for(l=0;l<w;l++){
                tnode->texture[i][(j*w+l)*nb + 3] = 0xFF ;
              }
            }
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
/*
 *   8-bit data
 *   NOTE:  getc should check for EOF.
 *   EOF has the value -1 so cannot be represented in a 'char' variable.
 *   Instead either read into an 'int' or call 'feof(fp)' after every read.
 *   Assumes that if an alpha channel is present then any mask channel can be
 *   skipped.
 */
              if(8 == size[k]){
                  if(type[k] == MASK){
                    for(l=0;l<w;l++){
                      maskp[(j*w+l)*nb + icol] = c = getc(fp);
                      nz_mask = nz_mask | c ;
                    }
                  }else if(type[k] == ALPHA){
                    for(l=0;l<w;l++){
                      tnode->texture[i][(j*w+l)*nb + icol] = c = getc(fp);
                      nz_alpha = nz_alpha | c ;
                    }
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
                if((type[k] == MASK)){
                  for(l=0;l<1+(w-1)/8;l++){
                    c = getc(fp);
                    for(m=0;m<8;m++){
                      if( m >= w - l*8 - 1) break ;
                      maskp[j*w + l*8 + m] = d = ((c>>(7-m)) & 1) ? 0xff : 0 ;
                      nz_mask = nz_mask | d ;
                    }
                  }
                }else{
                  for(l=0;l<1+(w-1)/8;l++) c = getc(fp);
                }
              }    // 8-bit or 1-bit
            }      // Loop over channels
          }        // Loop over rows
/*
 *   alpha/mask logic
 *   if nz_alpha :: do nothing more
 *     if nz_mask  :: set alpha from mask
 *     else        :: set alpha to 255
 */
#if 0
          if(is_alpha && !nz_alpha && is_mask && !nz_mask){
              printf("  Routine %s.  Texture %s, type %x, has alpha and mask "
                     "channels with all values set to zero (fully transparent).\n",
                             my_name, tnode->name, tnode->surface_format) ;
          }else{
            if(is_alpha && !nz_alpha){
                printf("  Routine %s.    Texture %s, type %x, has an alpha "
                       "channel with all values set to zero (fully transparent).\n",
                             my_name, tnode->name, tnode->surface_format) ;
            }
            if(is_mask && !nz_mask){
                printf("  Routine %s.    Texture %s, type %x, has a mask "
                       "channel with all values set to zero (fully transparent).\n",
                             my_name, tnode->name, tnode->surface_format) ;
            }
          }
#endif
          if(!nz_alpha && 4 == nb){
            if(nz_mask){
              for(j=0;j<h;j++){
                for(l=0;l<w;l++){
                  tnode->texture[i][(j*w+l)*nb + 3] = maskp[j*w + l] ;
                }
              }
            }else{
              for(j=0;j<h;j++){
                for(l=0;l<w;l++){
                  tnode->texture[i][(j*w+l)*nb + 3] = 0xFF ;
                }
              }
            }
          }
          if(maskp)free(maskp) ;
          maskp = NULL ;
        }          // Loop over images
      }          // End processing of "structured data" style file
      close_msfile(&msfile) ;
      if(maskp)free(maskp) ;
      return 0 ;
}

/*
 *  DEBUG
 *  Routine used to print a summary of a texture
 */
int  texture_print(char *name){
  TextureNode *tnode   ;
  int         l_found  ;
  int         width, height, i, j, k   ;

      l_found = 0 ;
      for(tnode = texturelist_beg; tnode != NULL; tnode = tnode->next){
        if(!strcmp(tnode->name, name)){
          l_found = 1 ;
          break ;
        }
      }
      if(!l_found) return 0 ;
      width  = tnode->width  ;
      height = tnode->height ;
      for(j=0;j<height; j += 50){
        printf(" row = %i : ",j) ;
        for(k=0;k<4;k++){
          printf("%i : ",k) ;
          for(i=0;i<width && i<30; i++){
            printf("%3i ",tnode->texture[0][(j*width+i)*4+k]) ;
          }
          printf("\n") ;
        }
      }
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
      if(tx==NULL) return 0 ;
/*
 *  Count textures
 */

      n_textures = 0 ;
      for(tx = *texturelist_beg; tx != NULL; tx = tx->next){
//        printf("  i  = %i\n", n_textures) ; fflush(NULL) ;
//        printf("  tx = %p\n", (void *)tx) ; fflush(NULL) ;
        n_textures++ ;
      }

      ta = (TextureNode **)malloc(n_textures*sizeof(TextureNode *)) ;
      for(tx = *texturelist_beg, i=0; tx != NULL; tx = tx->next, i++) ta[i] = tx ;

      if(ip)for(i=0;i<n_textures;i++){
        printf(" CC %p ta = %p,  name = %s\n",(void *)ta,(void *)ta[i], ta[i]->name) ;
      }

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

int  mark_needed_wagon_textures(ShapeNode *snode){

  int  ip = 0 ;
  int  i, n_textures ;
  int  it = 0 ;

  TextureNode *tx_node ;
  TextureNode *tx_prev ;
  char        *tx_name = NULL ;
  char        *my_name="mark_needed_wagon_textures" ;


      if(ip){
        printf("  Routine %s  ENTER\n",my_name) ;
        printf("    shape = %s\n",snode->name) ;
      }
      n_textures = snode->n_textures ;
      if(n_textures){
        for(i=0; i<n_textures; i++){
          if(tx_name)free(tx_name) ;
          tx_name = zr_corename(snode->texture_name[i]) ;
          if(ip)printf("    texture %i = %s\n", i, tx_name) ;
/*
 *  Search for tx_name in texturelist and wtexture list
 */
          for(tx_node = wtexturelist_beg; tx_node!= NULL; tx_node=tx_node->next){
            if(ip)printf("      wtexturelist texture = %s\n", tx_node->name) ;
            if(!strcmp(tx_node->name, tx_name)) break ;
          }
          if(tx_node){
            snode->texture[i] = tx_node ;
            if(ip)printf("      texture found in wtexturelist.  needed = %i\n",
                                tx_node->needed) ;
            tx_node->needed = 1;
            continue ;
          }
          if(ip)printf("      texture not found in wtexturelist\n") ;
/*
 * Search in main texture list
 */
          tx_prev = NULL ;
          for(tx_node = texturelist_beg; tx_node!= NULL; tx_node=tx_node->next){
            if(ip)printf("      texturelist texture = %s\n", tx_node->name) ;
            if(!strcmp(tx_node->name, tx_name)) break ;
            tx_prev = tx_node ;
          }
/*
 *  If unsuccesssful try again with case independent comparison
 */
          if(tx_node == NULL){
            for(tx_node = wtexturelist_beg; tx_node!= NULL; tx_node=tx_node->next){
              if(ip)printf("      wtexturelist texture = %s\n", tx_node->name) ;
              if(!strcmp_ic(tx_node->name, tx_name)) break ;
            }
            if(tx_node){
              snode->texture[i] = tx_node ;
              if(ip)printf("      texture found in wtexturelist.  needed = %i\n",
                                  tx_node->needed) ;
              tx_node->needed = 1;
              continue ;
            }
            if(ip)printf("      (case independent) texture not found in wtexturelist\n") ;
            tx_prev = NULL ;
            for(tx_node = texturelist_beg; tx_node!= NULL; tx_node=tx_node->next){
              if(ip)printf("      texturelist texture = %s\n", tx_node->name) ;
              if(!strcmp_ic(tx_node->name, tx_name)) break ;
              tx_prev = tx_node ;
            }
          }
/*
 *  Failure
 *   If this point is reached
 *   Try again looking for lower case comparison
 *      strcmp_ic
 */
          if(tx_node == NULL){
            if(ip)printf("  Routine %s unable to find texture %s for wagon %s\n",
                         my_name, tx_name, snode->name) ;
            continue ;
          }
/*
 * Success and already loaded
 */
          snode->texture[i] = tx_node ;
          if(ip)printf("      texture found in wtexturelist.  needed = %i\n",
                              tx_node->needed) ;
          if(tx_node->needed)  continue ;
/*
 *  Success but not loaded
 *    Move to wagon texture list and mark as needed
 */
          if(tx_node == texturelist_beg){
            texturelist_beg = tx_node->next ;
          }else{
            tx_prev->next = tx_node->next ;
          }
          if(tx_node == texturelist_end){
            texturelist_end = tx_prev ;
          }
          wtexturelist_end->next = tx_node;
          tx_node->next = NULL ;
          wtexturelist_end = tx_node ;
          tx_node->needed = 1 ;
        }
      }
      if(tx_name)free(tx_name) ;
      if(ip)printf("  Routine %s  EXIT\n",my_name) ;
      return 0 ;
}
