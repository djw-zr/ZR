/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  movie.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File containing the subroutines used to generate a imaeg sequence
 *   or a movie
 *
 *==============================================================================
 */


enum Constants { SCREENSHOT_MAX_FILENAME = 256 };
static GLubyte *pixels = NULL;
static GLuint fbo;
static GLuint rbo_color;
static GLuint rbo_depth;
static int offscreen = 1;
static unsigned int max_nframes = 128;
static unsigned int nframes = 0;
static unsigned int time0;
static unsigned int mov_height = 128;
static unsigned int mov_width = 128;
#define PPM_BIT (1 << 0)
#define LIBPNG_BIT (1 << 1)
#define FFMPEG_BIT (1 << 2)
static unsigned int output_formats = PPM_BIT | LIBPNG_BIT | FFMPEG_BIT;

/* Model. */
static double angle;
static double delta_angle;

double  zr_run_time(void){

struct timespec clock ;
double time ;

    clock_gettime(CLOCK_MONOTONIC_COARSE, &clock );
    time = (clock.tv_sec - run_clock0.tv_sec)
           + (clock.tv_nsec - run_clock0.tv_nsec)*0.000000001 ;
    return time ;
}



#if PPM
/* Take screenshot with glReadPixels and save to a file in PPM format.
 *
 * -   filename: file path to save to, without extension
 * -   width: screen width in pixels
 * -   height: screen height in pixels
 * -   pixels: intermediate buffer to avoid repeated mallocs across multiple calls.
 *     Contents of this buffer do not matter. May be NULL, in which case it is initialized.
 *     You must `free` it when you won't be calling this function anymore.
 */
static void screenshot_ppm(const char *filename, unsigned int width,
        unsigned int height, GLubyte **pixels) {
    size_t i, j, cur;
    int  ip = 0 ;
    const size_t format_nchannels = 3;
    FILE *f = fopen(filename, "w");

    if(ip)printf("  PPM AA time = %f\n", zr_run_time()) ;
    fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
    *pixels = realloc(*pixels, format_nchannels * sizeof(GLubyte) * width * height);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, *pixels);
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            cur = format_nchannels * ((height - i - 1) * width + j);
            fprintf(f, "%3d %3d %3d ", (*pixels)[cur], (*pixels)[cur + 1], (*pixels)[cur + 2]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    if(ip)printf("  PPM ZZ time = %f\n", zr_run_time()) ;
}
#endif

#if LIBPNG
/* Adapted from https://github.com/cirosantilli/cpp-cheat/blob/19044698f91fefa9cb75328c44f7a487d336b541/png/open_manipulate_write.c */
static png_byte *png_bytes = NULL;
static png_byte **png_rows = NULL;
static void screenshot_png(const char *filename, unsigned int width, unsigned int height,
        GLubyte **pixels, png_byte **png_bytes, png_byte ***png_rows) {
    int ip = 0 ;
    size_t i, nvals;
    const size_t format_nchannels = 4;
    FILE *f = fopen(filename, "wb");

    nvals = format_nchannels * width * height;
    *pixels = realloc(*pixels, nvals * sizeof(GLubyte));
    *png_bytes = realloc(*png_bytes, nvals * sizeof(png_byte));
    *png_rows = realloc(*png_rows, height * sizeof(png_byte*));
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, *pixels);
    for (i = 0; i < nvals; i++)
        (*png_bytes)[i] = (*pixels)[i];
    for (i = 0; i < height; i++)
        (*png_rows)[height - i - 1] = &(*png_bytes)[i * width * format_nchannels];
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();
    png_infop info = png_create_info_struct(png);
    if (!info) abort();
    if (setjmp(png_jmpbuf(png))) abort();
    png_init_io(png, f);
    png_set_IHDR(
        png,
        info,
        width,
        height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_set_compression_level(png,0) ;
    png_write_info(png, info);
    png_write_image(png, *png_rows);
    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(f);
}
#endif

#if FFMPEG
/* Adapted from: https://github.com/cirosantilli/cpp-cheat/blob/19044698f91fefa9cb75328c44f7a487d336b541/ffmpeg/encode.c */

static AVCodecContext *c = NULL;
static AVFrame *frame;
static AVPacket pkt;
static FILE *file;
static struct SwsContext *sws_context = NULL;
static uint8_t *rgb = NULL;

static void ffmpeg_encoder_set_frame_yuv_from_rgb(uint8_t *rgb) {
    const int in_linesize[1] = { 4 * c->width };
    sws_context = sws_getCachedContext(sws_context,
            c->width, c->height, AV_PIX_FMT_RGB32,
            c->width, c->height, AV_PIX_FMT_YUV420P,
            0, NULL, NULL, NULL);
    sws_scale(sws_context, (const uint8_t * const *)&rgb, in_linesize, 0,
            c->height, frame->data, frame->linesize);
}

void ffmpeg_encoder_start(const char *filename, int codec_id, int fps, int width, int height) {
    AVCodec *codec;
    int ret;
// DJW ::  In ffmpeg-4 'avcodec_register_all' does nothing
//    avcodec_register_all();
    codec = avcodec_find_encoder(codec_id);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    c->bit_rate = 400000;
    c->width = width;
    c->height = height;
    c->time_base.num = 1;
    c->time_base.den = fps;
    c->gop_size = 10;
    c->max_b_frames = 1;
/*
 *  Pixel Format
 *
 *  The pixel format is the format used in the final video.
 *    See '/usr/include/ffmpeg/libavutil/pixfmt.h'.
 *  Each codec only supports a limited number of pixel formats.
 *  Those for H264 can be listed with the command
 *     ffmpeg -h encoder=h264
 *     ffmpeg -h encoder=mpeg1video
 *  Both of these support YUV420P, neither supports RGB24 etc.
 *
 *  The standard AV_PIX_FMT_YUV420P is designed for TV transmission>
 *    This uses uses 12 bits per pixel   (1 Cr & Cb sample per 2x2 Y samples)
 *  There are anumber of RGB standards AV_PIX_FMT_NAME, where 'NAME' is
 *    RGB24, BGR21 :: 24 bits per pixel (8,8,8)
 *    RGB8, BGR8   :: 8 bits per pixel (3,3,2)
 *    RGB565BE,RGB565LE :: 16 bits per pixel (big endian or little endian(intel))
 */
    c->pix_fmt = AV_PIX_FMT_YUV420P;
/*
 *  Codec
 *
 *  There is a wide range of possible codecs.  These are listed in:
 *    '/usr/include/ffmpeg/libavcodec/codec_id.h'.
 *  If available a recommended one is H264 (or the later H265/H266?).
 *  Altenatively use MPEG1VIDEO
 */

/*
 *  The H264 presets can be seen with the comand
 *    x264 -fullhelp
 *  This usually means installing the x264 package.
 */

    if (codec_id == AV_CODEC_ID_H264){
//        av_opt_set(c->priv_data, "preset", "slow", 0);  //  Smaller file preset
        av_opt_set(c->priv_data, "preset", "fast", 0) ;   //  File size preset
        av_opt_set(c->priv_data, "crf","22", 0) ;         //  Image quality preset
        av_opt_set(c->priv_data, "movflags","+faststart", 0) ;
    }
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;
    ret = av_image_alloc(frame->data, frame->linesize, c->width, c->height, c->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }
}

void ffmpeg_encoder_finish(void) {
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    int got_output, ret;

/*
 *  Send flush signal
 */
      ret = avcodec_send_frame(c, NULL) ;
/*
 * Loop any output
 */
      do {
        ret = avcodec_receive_packet(c, &pkt);
//        printf(" CC iret = %i :: %i\n",ret, AVERROR(EAGAIN)) ;
        if(ret == AVERROR(EAGAIN)){
          fwrite(pkt.data, 1, pkt.size, file);
        }else{
          if(ret && (ret != AVERROR_EOF)){
            printf(" Routine %s returned error %i\n","avcodec_receive_packet",ret );
            exit(1);
          }
        }
      }while(ret != AVERROR_EOF) ;
      fwrite(endcode, 1, sizeof(endcode), file);
      fclose(file);
      avcodec_close(c);
      av_free(c);
      av_freep(&frame->data[0]);
      av_frame_free(&frame);
}

void ffmpeg_encoder_encode_frame(uint8_t *rgb) {
    int ret, got_output;

    ffmpeg_encoder_set_frame_yuv_from_rgb(rgb);

/*
 *  Read and process packets
 *  Send new frame
 *  Repear until send frame is successful - return code = 0
 */
      do{
        ret = avcodec_receive_packet(c, &pkt);
        if(!ret){
          fwrite(pkt.data, 1, pkt.size, file) ;
        }else if(ret != AVERROR(EAGAIN)){
            printf(" Routine %s returned error %i\n","avcodec_receive_packet",ret );
            exit(1);
        }
        ret = avcodec_send_frame(c,frame) ;
        if(ret && ret != AVERROR(EAGAIN)){
            printf(" Routine %s returned error %i\n","avcodec_send_frame",ret );
            exit(1);
        }
      }while (ret) ;
}

/*
 *  This routine requires glReadPixels to use GL_BGRA to generate pixels with the
 *  correct format for ffmpeg.
 */
void ffmpeg_encoder_glread_rgb(uint8_t **rgb, GLubyte **pixels, unsigned int width, unsigned int height) {
    size_t i, j, k, cur_gl, cur_rgb, nvals;
    const size_t format_nchannels = 4;

     nvals = format_nchannels * width * height;
    *pixels = realloc(*pixels, nvals * sizeof(GLubyte));
    *rgb = realloc(*rgb, nvals * sizeof(uint8_t));

/* Get RGBA to align to 32 bits instead of just 24 for RGB. May be faster for FFmpeg. */

    glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, *pixels);
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            cur_gl  = format_nchannels * (width * (height - i - 1) + j);
            cur_rgb = format_nchannels * (width * i + j);
            for (k = 0; k < format_nchannels; k++)
                (*rgb)[cur_rgb + k] = (*pixels)[cur_gl + k];
        }
    }
}

#endif

/*
 *  Routine to initialise movie making.
 *  1.  Search for MOVIE directory
 *      If not present create one
 *  2.  In the
 */
#ifdef MOVIE_ERROR
int movie_init(void){
      printf("  Movie error\n") ;
      printf("  More than one movie option defined\n") ;
      printf("  Program stopping ...\n");
      exit(0) ;
}
#elif ! defined MOVIE
int movie_init(void){

      return 0 ;
}
#else
int movie_init(void){

  int ierr = 0;
  DIR  *movie_dir ;
  char *dir_name = NULL ;

      movie_dir = opendir("./MOVIE") ;
      printf("   Search for ./MOVIE.  Result = %p\n",(void *)movie_dir) ;
      if(movie_dir){
        closedir(movie_dir);
      }else{
        mkdir("./MOVIE",0755) ;
      }

      return 0 ;
}
/*
 *  Routine to generate one movie frame
 */
int movie_new_frame(void){

double shot_length ;
char filename[SCREENSHOT_MAX_FILENAME];

/*
 *  Process start of shot
 */
      if(movie_on && !movie_stat){
        movie_shot ++ ;
        movie_frame = 0 ;
        movie_stat  = 1 ;
        shot_start  = run_seconds ;
        printf("  Movie : Start of Shot %i.  Start time = %f s.\n",
                 movie_shot, shot_start) ;
#if FFMPEG
        snprintf(filename, SCREENSHOT_MAX_FILENAME, "MOVIE/movie_%03i.mpeg",
               movie_shot);
        printf("   movie filename = %s\n",filename) ;
//      ffmpeg_encoder_start(filename, AV_CODEC_ID_MPEG1VIDEO, 25, viewport_width, viewport_height);
        ffmpeg_encoder_start(filename, AV_CODEC_ID_H264, 25, viewport_width, viewport_height);
#endif
/*
 *  Process end of shot
 */
      }else if(movie_stat && !movie_on){
        movie_stat = 0 ;
        shot_length = run_seconds - shot_start ;
        printf("  Movie : End of Shot %i.    Stop time = %f s.\n",
                 movie_shot, run_seconds) ;
        printf("        : Number of frames = %i.  Length of shot = %f s.\n",
                 movie_frame, shot_length) ;
        printf("        : Frames per second = %f\n", movie_frame/shot_length) ;
#ifdef FFMPEG
        ffmpeg_encoder_finish();
        free(rgb);
        rgb = NULL ;
#endif
        return 0 ;
      }
      if(!movie_on)return 0 ;
/*
 *  Check timings
 */
      shot_length = run_seconds - shot_start ;
      shot_length = shot_length - movie_frame/30 ;
//      shot_length = shot_length - movie_frame ;
      if(shot_length < 0.0) return 0 ;
/*
 *  Process New Frame
 */
      movie_frame++ ;
#ifdef PPM
      snprintf(filename, SCREENSHOT_MAX_FILENAME, "MOVIE/movie_%03i_%04i.ppm",
               movie_shot,movie_frame);
//      printf("   movie filename = %s\n",filename) ;
      screenshot_ppm(filename, viewport_width, viewport_height, &pixels);
#endif

#ifdef LIBPNG
      snprintf(filename, SCREENSHOT_MAX_FILENAME, "MOVIE/movie_%03i_%04i.png",
               movie_shot,movie_frame);
      printf("   movie filename = %s\n",filename) ;
      screenshot_png(filename, viewport_width, viewport_height,
                                             &pixels, &png_bytes, &png_rows);
#endif

#ifdef FFMPEG
      frame->pts = movie_frame ;
      ffmpeg_encoder_glread_rgb(&rgb, &pixels, viewport_width, viewport_height);
      ffmpeg_encoder_encode_frame(rgb);
#endif
      return 0 ;
}
#endif
