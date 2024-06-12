/*
 * =================================================================================================
 *
 *   File load_wave_file.c
 *
 *   Program to list the chunks in a RIFF file
 *
 *  gcc -o rw  read_wave_file.c
 * =================================================================================================
 */

typedef struct waveformatnode   WaveFormatNode  ;

/*
 *  MSTS wave files appear to be non-standard:
 *    block align is set to the size of each sample in bytes
 *    bits_per_sample is set to 1
 */


struct waveformatnode{
    short int wave_format;
    short int num_channels;
    int sample_rate;
    int av_byte_rate;
    short int block_align;  // Number of bytes per block
    short int bits_per_sample ;
} ;

typedef struct cuenode CueNode ;

struct cuenode{
    int  name;
    int  position;
    char cc_chunk[4] ;
    int  chunk_start;
    int  block_start;
    int  sample_offset;
} ;

/*
 *   NOTE:  Some sounds may be needed from the MSTS Sound Directory
 */


int load_wave_file(char* filename, char **data, ALenum *format,
                   ALsizei *size, ALsizei *frequency) {

  int     ip = 0 ;
  FILE* infile ;

  WaveFormatNode  wfn ;

  size_t nin ;
  int    iposn = 0 ;
  fpos_t old_fpos    ;
  int    offset = 0 ;
  int    file_size ;
  int    chunk_size ;
  int    n_cues, n_char, index;
  CueNode *cues ;
  char   cue_string[32] ;
  int    i, j, n, m, c, iret ;
  char   string4[5], last_string4[5] ;
  char   *base_name ;
  char   *filename2 ;
  char   *my_name = "load_wave_file" ;

      if(ip){
        printf("\n**********************************\n") ;
        printf("  Enter routine %s\n",my_name) ;
        printf("    filename  = %s\n",filename) ;
      }

      iret = find_msstyle_file(&filename) ;

      if(iret != 0){
        if(ip){
          printf("  Routine %s.  Filename not found by 'find_msstyle_file()'\n",
                 my_name);
          printf("    Current filename  = %s\n",filename);
        }
        if(MSTSdir){
          if(ip)printf("    Trying MSTSDir %s\n",MSTSdir);
          base_name = zr_basename(filename) ;
          n = strlen(base_name) + strlen("/Sound/") + strlen(MSTSdir) + 1;;
          filename2 = (char *)malloc(n*sizeof(char)) ;
          strcpy(filename2,MSTSdir) ;
          strcat(filename2,"/SOUND/") ;
          strcat(filename2,base_name) ;
          if(ip)printf("    Trying filename %s\n",filename2);
          iret = find_msstyle_file(&filename2) ;
        }
        if(iret != 0){

        }
        if(iret != 0){
          printf("  Routine %s error\n",my_name) ;
          printf("  File not found\n") ;
          printf("  File =  %s \n",filename) ;
          *format = 0 ;
          *size = 0 ;
          *frequency = 0 ;
          return 1 ;
        }
      }else{
        filename2 = strdup(filename) ;
      }
      if(ip)printf("    Using filename %s\n",filename2) ;
      infile = fopen(filename2, "rb") ;
      free(filename2) ;

      if(infile == NULL){
        printf("  Routine %s error\n",my_name) ;
        printf("  File not found\n") ;
        printf("  File =  %s \n",filename) ;
        *format = 0 ;
        *size = 0 ;
        *frequency = 0 ;
        return 1 ;
      }
/*
 *   Read RIFF Header
 */
      iposn = 0 ;
      nin = fread(string4, 1, 4, infile) ;
      string4[4] = '\0' ;
      if(ip)printf("  Position = %6i  %6X,  Tag = %s :: %i\n",iposn,iposn, string4, (int)nin) ;
      iposn += 4 ;

//      printf("  string4 = %s\n",string4);
      if(strcmp(string4,"RIFF")){
        printf("  Routine %s error\n",my_name) ;
        printf("    RIFF header string reads: '%s'\n",string4) ;
        return 1 ;
      }
/*
 *  Read offset to end of file
 */
      nin = fread(&offset, 4, 1, infile) ;
      file_size = iposn + offset ;
      iposn += 4 ;
      if(ip){
        printf("    offset    = %i :: %x :: %i\n", offset, offset, (int)nin) ;
        printf("    file_size = %i :: %x :: %i\n", file_size, file_size, (int)nin) ;
      }
/*
 *   Read WAVE Header
 */
      nin = fread(string4, 1, 4, infile) ;
      string4[4] = '\0' ;
      if(ip)printf("  Position = %6i  %6X,  Tag = %s\n",iposn,iposn, string4);
      iposn += 4 ;

      if(strcmp(string4,"WAVE")){
        printf("  Routine %s error\n",my_name) ;
        printf("    WAVE header string reads: '%s'\n",string4) ;
        return 1 ;
      }
/*
 *   The rest of the file should contain a series of chunks
 *
 *   Loop reading chunks
 */
      while((c=getc(infile)) != EOF){
        ungetc(c,infile) ;

//        printf("  At start of loop ;; ftell = %i\n",ftell(infile)) ;
        last_string4[4] = '\0' ;
        strcpy(last_string4, string4) ;
        nin = fread(string4, 1, 4, infile) ;
        string4[4] = '\0' ;
        if(ip)printf("\n  AA Position = %6i  %6X,  Tag = %s :: %i\n",iposn, iposn, string4, (int)nin) ;
        iposn += 4 ;

        nin = fread(&chunk_size, 4, 1, infile) ;
        if(ip)printf("     chunk_size = %i %x:: end at %i %x:: %i\n",
                     chunk_size,chunk_size,
                     chunk_size+iposn, chunk_size+iposn, (int)nin) ;
        iposn += 4 ;
/*
 *  WAVE format chunk
 */
        if(!strcmp(string4,"fmt ")){
          nin = fread(&wfn, 1, sizeof(WaveFormatNode), infile) ;
          iposn += sizeof(WaveFormatNode) ;
          for(i=0;i<(int)(chunk_size-sizeof(WaveFormatNode));i++){
            iposn++ ;
            getc(infile) ;
          }
        if(ip){
          printf("\n  Routine %s\n",my_name) ;
          printf("    filename = %s\n",filename) ;
          printf("    WAVE format       = %i\n",wfn.wave_format) ;
          printf("    WAVE num_channels = %i\n",wfn.num_channels) ;
          printf("    WAVE sample_rate  = %i\n",wfn.sample_rate) ;
          printf("    WAVE av_byte_rate = %i\n",wfn.av_byte_rate) ;
          printf("    WAVE num_bytes    = %i\n",*size) ;
//          printf("    data              = %p\n",(void *)data) ;
        }
/*
 *  Audio data chunk
 *
 *  The four standard OpenAL formats correspond to the GIFF/WAVE formats for
 *  mono and stereo audio using either 8 or 16 bits.  Additional code will
 *  be needed for compressed WAVE data or cases when ther are more data streams.
 *  For 16 bit data, WAVE uses the little-endian format with the lowest order byte
 *  first.  The byte order will need changing for computers which uses the big-endian
 *  format.
 */
        }else if(!strcmp(string4,"data")){
          if(ip){
            printf("  Reading audio data.  chunk_size = %i\n",chunk_size) ;
            printf("   data  = %p\n",(void *)data) ;
          }
          *data = (char *)malloc(chunk_size*sizeof(char)) ;
          if(ip){
            printf("   data  = %p\n",(void *)data) ;
            printf("   *data = %p\n",(void *)*data) ;
          }
          if(data == NULL){
            printf("  Subroutine %s error",my_name) ;
            printf("    malloc failed for data") ;
            return 1 ;
          }
          nin = fread(*data, 1, chunk_size, infile) ;
          *size = chunk_size ;
          iposn += chunk_size ;
/*
 *  Cue point chunk
 *
 *  The chunk contains a list of cuepoints
 */
        }else if(!strcmp(string4,"cue ")){
          nin = fread(&n_cues, 1, 4, infile) ;
          cues = (CueNode *)malloc(n_cues*sizeof(CueNode)) ;
          nin = fread(cues, n_cues, sizeof(CueNode), infile) ;
          iposn += chunk_size ;
        }else if(!strcmp(string4,"LIST") &&
                 !strcmp(last_string4,"cue ") && 0){
          fgetpos(infile, &old_fpos) ;
/*
 *  Read 'adtl' tag
 */
          nin = fread(string4, 1, 4, infile) ;
          string4[4] = '\0' ;
          if(strcmp(string4,"adtl")){
            printf("  Routine %s error\n",my_name) ;
            printf("    Label 'adtl' missing in LIST section\n") ;
            exit(0) ;
          }
/*
 *  Loop over labels
 */
          for(i=0; i<n_cues; i++){
            nin = fread(string4, 1, 4, infile) ;
            string4[4] = '\0' ;
            if(strcmp(string4,"labl")){
              printf("  Routine %s error\n",my_name) ;
              printf("    Label 'labl' missing in LIST section\n") ;
              printf("    string4 = %s  %X %X %X %x\n",string4,
                     string4[0], string4[1], string4[2], string4[3]) ;
              exit(0) ;
            }
            nin = fread(&n_char, 4, 1, infile) ;
            nin = fread(&index, 4, 1, infile) ;
            n_char = n_char - 4 ;

            m = (n_char<=32) ? n_char : 32 ;
            nin = fread(&cue_string, m, 1, infile) ;
            cue_string[m-1] = '\0' ;
            if(ip)printf("  Cue_string[%i] = '%s'\n",index, cue_string) ;
            for(j=0;j<n_char-m;j++)getc(infile) ;
          }
/*
 *  Reset file position
 */
          fsetpos(infile, &old_fpos) ;
          for(i=0; i<chunk_size; i++) getc(infile) ;
          iposn += chunk_size ;
        }else{
          for(i=0; i<chunk_size; i++) getc(infile) ;
          iposn += chunk_size ;
        }
      }

/*
 *  Print results
 */
      if(ip){
        printf("\n  Routine %s\n",my_name) ;
        printf("    filename = %s\n",filename) ;
        printf("    WAVE format       = %i\n",wfn.wave_format) ;
        printf("    WAVE num_channels = %i\n",wfn.num_channels) ;
        printf("    WAVE sample_rate  = %i\n",wfn.sample_rate) ;
        printf("    WAVE av_byte_rate = %i\n",wfn.av_byte_rate) ;
        printf("    WAVE num_bytes    = %i\n",*size) ;
        printf("    data              = %p\n",(void *)data) ;
      }

    short int wave_format;
    short int num_channels;
    int sample_rate;
    int av_byte_rate;
    short int block_align;  // Number of bytes per block
    short int bits_per_sample ;
/*
 *   Calculate
 */
      *format = *frequency = 0 ;
      if(wfn.wave_format != 1){
        printf("  Routine %s error\n",my_name) ;
        printf("    Wave file format is not PCM\n") ;
        return 1 ;
      }
      if(wfn.num_channels != 1  && wfn.num_channels != 2){
        printf("  Routine %s error\n",my_name) ;
        printf("    Number of chanels unsupported\n") ;
        printf("    Number of channels = %i\n",wfn.num_channels) ;
        return 1 ;
      }

int byte_per_value = wfn.av_byte_rate/
                 (wfn.sample_rate*wfn.num_channels);

      if(byte_per_value != 1  && byte_per_value != 2){
        printf("  Routine %s error\n",my_name) ;
        printf("    Unexpected bytes per value\n") ;
        printf("    bytes per value = %i\n",byte_per_value) ;
        return 1 ;
      }

      *frequency = wfn.sample_rate ;

      if(wfn.num_channels == 1){
        if(byte_per_value == 1){
          *format = AL_FORMAT_MONO8 ;
        }else{
          *format = AL_FORMAT_MONO16 ;
        }
      }else{
        if(byte_per_value == 1){
          *format = AL_FORMAT_STEREO8 ;
        }else{
          *format = AL_FORMAT_STEREO16 ;
        }
      }
      return 0;
}


