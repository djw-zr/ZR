/*
 * =============================================================================
 * 45678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File terrain.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File contains routines for handling terrain files
 *   Code based on comments and code in TerrainFile.cs and Terrain.cs
 *
 *   Uses the following routines:
 *   texture.c     read_int32(FILE *fp)   : Read binary int32
 *                 read_int16(FILE *fp)   : Read binary int16
 *   token.c       open_msfile            : Open ascii and binary MSTS files
 *
 * =============================================================================
 */

/*
 * =============================================================================
 *  Routine to read MTST style terrain *.t files
 *
 *  These contain information on the terrain within each world tile.  They
 *  include information on the textures to use.  Topography (elevation) is in
 *  a separate file.
 *
 *  The files are assumed to be in uncompressed MSTS binary format.  After
 *  a standard MSTS file header this consist of a series of standard blocks.
 *  Each block starts with
 *  1)  A 2-byte ID   - corresponding to a name in enum.h
 *  2)  A 2-byte flag - usually zero
 *  3)  A 4-byte block length - including sub-blocks
 *  4)  A 2-byte label_length - so far always zero but see Terrain.c
 *  Processing is straightforward when a block contains known data or a known
 *  number of sub-blocks of a known type.  There are problems when the types
 *  and number of sub-blocks is unknow.  The C# code handles this by keeping
 *  track of every byte read and cycling at each level until the required
 *  number have been read or the end of file reached.
 *
 *  Here this is done only for levels where the problem arises.  As a backup
 *  when an ID is unrecognised, it is passed to the next higher level.
 *
 *  Text in these files is in 2-byte unicode - following an initial one byte
 *  integer giving the number of unicode characters.
 * =============================================================================
 */

int read_ms_terrain(TileListNode *tile)
{
  int j, k, l ;
  int ip = 0 ;
  int itoken1,   flags1,  remaining_bytes1, label_length1, block_bytes1,
      itoken2,   flags2,  remaining_bytes2, label_length2, block_bytes2,
      itoken3,   flags3,  remaining_bytes3, label_length3, block_bytes3,
      itoken4,   flags4,  remaining_bytes4, label_length4, block_bytes4,
      itoken5,   flags5,  remaining_bytes5, label_length5, block_bytes5,
      itoken6,   flags6,  remaining_bytes6, label_length6, block_bytes6,
      number_of_shaders,
      number_of_patchsets,
      number_of_patches      ;

  TerrainData   *terrain_data   = &(tile->terrain_data)        ;
  TileShader    *tile_shader    = terrain_data->tile_shader    ;  // Array of shaders
  TilePatchSet  *tile_patchset  = terrain_data->tile_patchset  ;  // Array of patchsets

  TexSlotNode   *tex_slot       = NULL  ;
  TexUVCalcNode *uvcalc         = NULL  ;
  TilePatch     *tile_patch     = NULL  ;

  MSfile  msfile ;
  FILE   *fp ;
  char    myname[] = "read_ms_terrain" ;

//      ip = tile->tilex == -6047 && tile->tiley == 14921 ;
//      ip = 0 ;

      if(ip){
        printf("\n=============================================\n") ;
        printf("\n  Enter routine : %s\n",myname);
        printf("  File = %s\n",tile->t_file);
      }
/*
 * =============================================================================
 *  open_msfile reads and checks the first 16 bytes of the terrain file,
 *  inflates compressed files and set the flags
 * =============================================================================
 */
      l = open_msfile(tile->t_file, &msfile, 0, 0);
      if(l!=0){
        printf("\n\n  ERROR : Routine open_msfile failed to open file\n\n");
        exit(1) ;
      }
      fp = msfile.fp ;
      if(ip)printf("  TILEX = %i,  TILEZ = %i\n\n",tile->tilex, tile->tiley);
/*
 *   Code based on TerrainFile.cs
 *                 public class terrain,
 *    function:    public terrain(SBR block)
 *
 *   Read the first sub-block  [ ReadSubBlock in SBR.cs]
 */
      itoken1       = read_uint16(fp) ;
      flags1        = read_uint16(fp) ;
      block_bytes1  = read_uint32(fp) + 8;
      label_length1 = read_uint8(fp)  ;
      remaining_bytes1 = block_bytes1 - 9 ;

      if(ip && 0) {
        printf(" AA itoken1          = %X  :: %i\n", itoken1, itoken1) ;
        printf(" AA flags1           = %X  :: %i\n", flags1,  flags1 ) ;
        printf(" AA block_bytes1     = %X  :: %i\n", block_bytes1, block_bytes1) ;
        printf(" AA label_length1    = %X  :: %i\n", label_length1, label_length1 ) ;
        printf(" AA remaining_bytes1 = %X  :: %i\n", remaining_bytes1, remaining_bytes1) ;

        printf(" AA token_idc[%i] %x = %s\n",itoken1,itoken1,token_idc[itoken1]) ;
        printf(" AA token_id[%s] = %x :: %i\n","TERRAIN",TERRAIN,TERRAIN) ;
      }

      switch(itoken1) {                                       // SWITCH 1
        case(TERRAIN) :
          for(;remaining_bytes1>0;){                          // LOOP1
            itoken2          = read_uint16(fp) ;
            flags2           = read_uint16(fp) ;
            block_bytes2     = read_uint32(fp) + 8  ;
            label_length2    = read_uint8(fp) ;
            remaining_bytes2 = block_bytes2 - 9 ;
            remaining_bytes1 = remaining_bytes1 - block_bytes2 ;

            if(ip && 0){
              printf(" BB itoken2 = %i, flags2 = %i, remaining_bytes2 = %i, label_length2 = %i\n",
                 itoken2, flags2, remaining_bytes2, label_length2) ;
              printf(" BB token_idc[%i] 0x%x = %s\n",itoken2,itoken2,token_idc[itoken2]) ;
            }

            switch(itoken2){                                   // SWITCH 2
              case(TERRAIN_ERRTHRESHOLD_SCALE):
                terrain_data->terrain_errthreshold_scale = read_real32(fp) ;
                remaining_bytes2 = remaining_bytes2 - 4 ;
                if(ip)printf("\n terrain_errthreshold_scale = %f\n",terrain_data->terrain_errthreshold_scale);
                break ;
              case(TERRAIN_WATER_HEIGHT_OFFSET):
                terrain_data->terrain_water_height_offset_SW = read_real32(fp) ;
                remaining_bytes2 = remaining_bytes2 - 4 ;
                if(remaining_bytes2 == 0)break ;
                terrain_data->terrain_water_height_offset_SE = read_real32(fp) ;
                remaining_bytes2 = remaining_bytes2 - 4 ;
                if(remaining_bytes2 == 0)break ;
                terrain_data->terrain_water_height_offset_NE = read_real32(fp) ;
                remaining_bytes2 = remaining_bytes2 - 4 ;
                if(remaining_bytes2 == 0)break ;
                terrain_data->terrain_water_height_offset_NW = read_real32(fp) ;
                remaining_bytes2 = remaining_bytes2 - 4 ;
                if(ip){
                  printf("\n");
                  printf(" terrain_water_height_offset_SW = %f\n",terrain_data->terrain_water_height_offset_SW);
                  printf(" terrain_water_height_offset_SE = %f\n",terrain_data->terrain_water_height_offset_SE);
                  printf(" terrain_water_height_offset_NE = %f\n",terrain_data->terrain_water_height_offset_NE);
                  printf(" terrain_water_height_offset_NW = %f\n",terrain_data->terrain_water_height_offset_NW);
                }
                break ;
              case(TERRAIN_ALWAYSSELECT_MAXDIST) :
                terrain_data->terrain_alwaysselect_maxdist = read_real32(fp) ;
                remaining_bytes2 = remaining_bytes2 - 4 ;
                if(ip){
                  printf("\n");
                  printf(" terrain_alwaysselect_maxdist = %f\n",terrain_data->terrain_alwaysselect_maxdist);
                }
                break;
              case(TERRAIN_SAMPLES) :
                for(;remaining_bytes2>0;){                       //  LOOP 2
                  itoken3          = read_uint16(fp) ;
                  flags3           = read_uint16(fp) ;
                  block_bytes3     = read_uint32(fp) + 8;
                  label_length3    = read_uint8(fp)  ;
                  remaining_bytes3 = block_bytes3 - 9 ;
                  remaining_bytes2 = remaining_bytes2 - block_bytes3 ;
                  if(ip && 0){
                    printf("\n");
                    printf(" CC itoken3 = %i, flags3 = %i, remaining_bytes3 = %i, label_length3 = %i\n",
                              itoken3, flags3, remaining_bytes3, label_length3) ;
                    printf(" CC token_idc[%i] 0x%x = %s\n",itoken3,itoken3,token_idc[itoken3]) ;
                    printf(" AA remaining_bytes3 = %i\n",remaining_bytes3);
                  }
                  switch(itoken3){                                // SWITCH 3
                    case(TERRAIN_NSAMPLES):
                      terrain_data->terrain_nsamples = read_uint32(fp) ;
                      if(ip)printf(" BB remaining_bytes3 = %i\n",remaining_bytes3);
                      remaining_bytes3 = remaining_bytes3 - 4 ;
                      if(ip){
//                        printf(" CC remaining_bytes3 = %i\n",remaining_bytes3);
                        printf("\n");
                        printf(" terrain_nsamples      = %i\n",terrain_data->terrain_nsamples);
                      }
                      break ;
                    case(TERRAIN_SAMPLE_ROTATION):
                      terrain_data->terrain_sample_rotation = read_real32(fp) ;
                      remaining_bytes3 = remaining_bytes3 - 4 ;
                      if(ip){
                        printf("\n terrain_sample_rotation = %f\n",terrain_data->terrain_sample_rotation);
                      }
                      break ;
                    case(TERRAIN_SAMPLE_FLOOR):
                      terrain_data->terrain_sample_floor = read_real32(fp) ;
                      remaining_bytes3 = remaining_bytes3 - 4 ;
                      if(ip){
                        printf(" terrain_sample_floor   = %f\n",terrain_data->terrain_sample_floor);
                      }
                      break ;
                    case(TERRAIN_SAMPLE_SCALE):
                      terrain_data->terrain_sample_scale = read_real32(fp) ;
                      remaining_bytes3 = remaining_bytes3 - 4 ;
                      if(ip){
                        printf(" terrain_sample_scale   = %f\n",terrain_data->terrain_sample_scale);
                      }
                      break ;
                    case(TERRAIN_SAMPLE_SIZE):
                      terrain_data->terrain_sample_size = read_real32(fp) ;
                      remaining_bytes3 = remaining_bytes3 - 4 ;
                      if(ip){
                        printf(" terrain_sample_size     = %f\n",terrain_data->terrain_sample_size);
                      }
                      break ;
                    case(TERRAIN_SAMPLE_YBUFFER):
                      terrain_data->ybuffer = read_ucharz(fp) ;
                      remaining_bytes3 = remaining_bytes3 - 2 - 2*strlen(terrain_data->ybuffer) ;
                      if(ip){
                        printf(" terrain_sample_ybuffer = %s\n",terrain_data->ybuffer);
                      }
                      break ;
                    case(TERRAIN_SAMPLE_EBUFFER):
                      terrain_data->ebuffer = read_ucharz(fp) ;
                      remaining_bytes3 = remaining_bytes3 - 2 - 2*strlen(terrain_data->ebuffer) ;
                      if(ip){
                        printf(" terrain_sample_ebuffer = %s\n",terrain_data->ebuffer);
                      }
                      break ;
                    case(TERRAIN_SAMPLE_NBUFFER):
                      terrain_data->nbuffer = read_ucharz(fp) ;
                      remaining_bytes3 = remaining_bytes3 - 2 - 2*strlen(terrain_data->nbuffer) ;
                      if(ip){
                        printf(" terrain_sample_nbuffer = %s\n",terrain_data->nbuffer);
                      }
                      break ;
                    case(TERRAIN_SAMPLE_ASBUFFER):
                        for(;0<remaining_bytes3;remaining_bytes3--)getc(fp);

//                      terrain_sample_asbuffer = read_unit16(fp) ;
//                      printf("\n");
//                      printf(" terrain_sample_asbuffer = %i\n",terrain_sample_asbuffer);
                      break ;
                    case(TERRAIN_SAMPLE_FBUFFER):
                        for(;0<remaining_bytes3;remaining_bytes3--)getc(fp);
//                      terrain_sample_fbuffer = read_unit16(fp) ;;
//                      printf("\n");
//                      printf(" terrain_sample_fbuffer = %i\n",terrain_sample_fbuffer);
                      break ;
                    case(282):
                        for(;0<remaining_bytes3;remaining_bytes3--)getc(fp);
//                      dummy = read_unit16(fp) ;;
//                      printf("\n");
//                      printf(" dummy = %i\n",dummy);
                      break;
                    default:
                      printf("  Token not recognised.  itoken3 = %i, %s\n",itoken3,token_idc[itoken3]) ;
                      break ;
                  } ;                                 // End of SWITCH 3
                  if(remaining_bytes3 != 0){
                    printf("  Remaining bytes3 = %i, at end of TERRAIN_SAMPLES switch3.\n",remaining_bytes3);
                    exit(1);
                  }
                }                                     // End of LOOP   2
                break ;                               // End of TERRAIN_SAMPLES
/*
 * terrain shaders in array:  TerrainShaderNode *terrain_shader_node
 */
              case(TERRAIN_SHADERS) :                 // Level 2 option
                terrain_data->number_of_shaders= read_uint32(fp) ;
                remaining_bytes2 =remaining_bytes2 - 4;
                if(ip)printf("\n EE TERRAIN_SHADERS number of shaders =  %i\n\n",
                                                                  terrain_data->number_of_shaders) ;

                number_of_shaders = terrain_data->number_of_shaders ;
                terrain_data->tile_shader = (TileShader *)malloc(number_of_shaders*sizeof(TileShader)) ;

                tile_shader = terrain_data->tile_shader ;
                for(j=0;j<number_of_shaders;j++,tile_shader++){                     // LOOP 2
                  if(ip)printf(" EE TERRAIN_SHADERS Loop:  j =  %i %i\n",j, number_of_shaders) ;

                  itoken3          = read_uint16(fp) ;
                  flags3           = read_uint16(fp) ;
                  block_bytes3     = read_uint32(fp) + 8 ;
                  label_length3    = read_uint8(fp)  ;
                  remaining_bytes3 = block_bytes3  - 9 ;
                  remaining_bytes2 = remaining_bytes2  - block_bytes3 ;

                  if(ip && 0){
                    printf("\n");
        printf(" EE itoken3 = %i, flags3 = %i, remaining_bytes3 = %i, label_length3 = %i\n",
                    itoken3, flags3, remaining_bytes3, label_length3) ;
        printf(" EE token_idc[%i] 0x%x = %s\n",itoken3,itoken3,token_idc[itoken3]) ;
                  }
/*
 * =============================================================================
 *   Start processing new Shader
 * =============================================================================
*/
//                  tile_shader = &(terrain_data->tile_shader[j]) ;
                  tile_shader->name = read_ucharz(fp) ;
                  remaining_bytes3 = remaining_bytes3 - 2 - 2*strlen(tile_shader->name);
                  if(ip)printf("   EE shader_name = %s\n\n",tile_shader->name) ;

//===========================================================================================
                  switch(itoken3){                  // SWITCH 3
//--------------------------------------------------------------------------------------------
                    case(TERRAIN_SHADER):           // Level 3 option
                      if(ip)printf("   ENTER TERRAIN_SHADER\n");
                      for(;remaining_bytes3>0;){    // LOOP 3
                        if(ip)printf("    Loop in TERRAIN_SHADER\n");
                        itoken4          = read_uint16(fp) ;
                        flags4           = read_uint16(fp) ;
                        block_bytes4     = read_uint32(fp) + 8 ;
                        label_length4    = read_uint8(fp)  ;
                        remaining_bytes4 = block_bytes4  - 9 ;
                        remaining_bytes3 = remaining_bytes3  - block_bytes4 ;

//        printf(" EE itoken4 = %i, flags4 = %i, remaining_bytes4 = %i, label_length4 = %i\n",
//                  itoken4, flags4, remaining_bytes4, label_length4) ;
//        printf(" EE token_idc[%i] 0x%x = %s\n",itoken4,itoken4,token_idc[itoken4]) ;

//============================================================================================
                        switch(itoken4){              // SWITCH 4
//-------------------------------------------------------------------------------------------
                          case(TERRAIN_TEXSLOTS):     // Level 4 option
                            if(ip)printf("\n   ENTER TERRAIN_TEXSLOTS\n");
                            tile_shader->number_of_texslots = read_uint32(fp) ;
                            remaining_bytes4 = remaining_bytes4 - 4 ;

                            if(ip)printf("   FF number_of_texslots = %i\n",tile_shader->number_of_texslots) ;

                            tile_shader->tex_slot = (TexSlotNode *)malloc(tile_shader->number_of_texslots*sizeof(TexSlotNode)) ;

                            tex_slot = tile_shader->tex_slot ;
                            for(k=0;k<tile_shader->number_of_texslots;k++,tex_slot++){  // LOOP 4
                              itoken5          = read_uint16(fp) ;
                              flags5           = read_uint16(fp) ;
                              block_bytes5     = read_uint32(fp) + 8;
                              label_length5    = read_uint8(fp) ;
                              remaining_bytes5 = block_bytes5  - 9 ;
                              remaining_bytes4 = remaining_bytes4  - block_bytes5 ;

//            printf(" FF itoken5 = %i, flags5 = %i, remaining_bytes5 = %i, label_length5 = %i\n",
//                      itoken5, flags5, remaining_bytes5, label_length5) ;
//            printf(" FF token_idc[%i] 0x%x = %s\n",itoken5,itoken5,token_idc[itoken5]) ;
                              if(ip)printf("   FF k (tex_slot) = %i\n",k) ;

                              tex_slot->filename = read_ucharz(fp) ;
                              tex_slot->A = read_uint32(fp) ;
                              tex_slot->B = read_uint32(fp) ;
                              remaining_bytes5 = remaining_bytes5 - 10 - 2*strlen(tex_slot->filename) ;

                              if(ip){
                                printf("     GG filename = %s %i\n",tex_slot->filename,
                                                    (int)strlen(tex_slot->filename));
                                printf("     GG A        = %i\n",  tex_slot->A);
                                printf("     GG B        = %i\n",tex_slot->B);
//                                printf("  GG remaining_bytes5 = %i\n",remaining_bytes5);
//                                printf("  GG remaining_bytes4 = %i\n\n",remaining_bytes4);
                              }
                            } ;                          // End LOOP 4
                            break ;                      // End Level 4 option
//----------------------------------------------------------------------------------------------
                          case(TERRAIN_UVCALCS):         // level 4 option
//                            printf(" ENTER TERRAIN_UVCALCS\n");
                            tile_shader->number_of_uvcalcs = read_uint32(fp) ;
//            printf(" EE tile_shader->number_of_uvcalcs = %i\n\n",tile_shader->number_of_uvcalcs) ;
                            tile_shader->uvcalc = (TexUVCalcNode *)malloc(tile_shader->number_of_uvcalcs*sizeof(TexUVCalcNode)) ;

                            uvcalc = tile_shader->uvcalc ;
                            for(k=0;k<tile_shader->number_of_uvcalcs;k++,uvcalc++){  // LOOP level 4
//                              printf(" PROCESS TILE_SHADER %i\n",k);

                              itoken5          = read_uint16(fp) ;
                              flags5           = read_uint16(fp) ;
                              block_bytes5     = read_uint32(fp) + 8 ;
                              label_length5    = read_uint8(fp) ;
                              remaining_bytes5 = block_bytes5  - 1 ;
                              remaining_bytes4 = remaining_bytes4  - block_bytes5 ;

//            printf(" FF itoken5 = %i, flags5 = %i, remaining_bytes5 = %i, label_length5 = %i\n",
//                      itoken4, flags5, remaining_bytes5, label_length5) ;
//            printf(" FF token_idc[%i] 0x%x = %s\n",itoken5,itoken5,token_idc[itoken5]) ;
//            printf(" FF k (ter_uvc) = %i\n\n",k) ;

//                              uvcalc = &(tile_shader->uvcalc[k]) ;
                              uvcalc->A = read_uint32(fp) ;
                              uvcalc->B = read_uint32(fp) ;
                              uvcalc->C = read_uint32(fp) ;
                              uvcalc->D = read_real32(fp) ;
                              remaining_bytes5 = remaining_bytes5 - 16;
//                              printf("  GG A        = %i\n",uvcalc->A);
//                              printf("  GG B        = %i\n",uvcalc->B);
//                              printf("  GG C        = %i\n",uvcalc->C);
//                              printf("  GG D        = %f\n\n",uvcalc->D);
//                              printf("  GG remaining_bytes5 = %i\n",remaining_bytes5);
//                              printf("  GG remaining_bytes4 = %i\n",remaining_bytes4);
                            } ;
                            break ;                      // End Level 4 option
//-------------------------------------------------------------------------------------------------
                          default :
                printf("  Level 4 token not recognised.  itoken4 = %i, %s\n",itoken4,token_idc[itoken4]) ;
                          break ;
//-------------------------------------------------------------------------------------------------
                        }                                // End of SWITCH 4
//                        printf(" HH remaining_bytes5 = %i\n",remaining_bytes5);
//                        printf(" HH remaining_bytes4 = %i\n",remaining_bytes4);
//                        printf(" HH remaining_bytes3 = %i\n",remaining_bytes3);
//================================================================================================
                      }                                // End of LOOP 3
//                      printf("  Just before break after switch itoken3\n");
                      break ;                          // End of TERRAIN_SHADER
//-------------------------------------------------------------------------------------------------
                    default:
              printf("  Token not recognised.  itoken3 = %i, %s\n",itoken3,token_idc[itoken3]) ;
                      break ;
                  }                                    // End of SWITCH 3
//                  printf(" GG remaining_bytes4 = %i\n",remaining_bytes4);
//                  printf(" GG remaining_bytes3 = %i\n",remaining_bytes3);
//                  printf("  After switch itoken3\n");
                }                                      // End of LOOP 2
//                printf("  After end of loop in TERRAIN SHADERS : before break\n");
                break ;                                // End of TERRAIN SHADERS option
 /*
 * terrain patches in array:  TerrainPatchNode *terrain_patch_node
 */
              case(TERRAIN_PATCHES) :
//                  printf("\n ENTER TERRAIN_PATCHES\n");

                  itoken3          = read_uint16(fp) ;
                  flags3           = read_uint16(fp) ;
                  block_bytes3     = read_uint32(fp) + 8 ;
                  label_length3    = read_uint8(fp)  ;
                  remaining_bytes3 = block_bytes3  - 9 ;
                  remaining_bytes2 = remaining_bytes2  - block_bytes3 ;

//                printf("\n");
//                printf(" DD itoken3 = %i, flags3 = %i, remaining_bytes3 = %i, label_length3 = %i\n",
//                          itoken3, flags3, remaining_bytes3, label_length3) ;
//                printf(" DD token_idc[%i] 0x%x = %s\n",itoken3,itoken3,token_idc[itoken3]) ;

/*
 *   Allocate storage
 */
//                ter_patch_node_arr = (TerPatchNode *)malloc(l*sizeof(terpatchnode));
/*
 * =============================================================================
 *   Loop over patchsets
 * =============================================================================
 */
                number_of_patchsets  = read_uint32(fp) ;
                terrain_data->number_of_patchsets = number_of_patchsets  ;
                remaining_bytes3 = remaining_bytes3 - 4 ;
//                printf("\n LOOP OVER TERRAIN_PATCHSETS\n");
//                printf("  Number of patchsets = %i\n",number_of_patchsets) ;

                terrain_data->tile_patchset = (TilePatchSet *)malloc(number_of_patchsets*sizeof(TilePatchSet)) ;

                tile_patchset = terrain_data->tile_patchset ;
                for(k=0;k<number_of_patchsets;k++,tile_patchset++){

//                  tile_patchset = &(terrain_data->tile_patchset[k]) ;

                    if(ip)printf("\n ENTER TERRAIN_PATCHSETS k = %i\n",k);
                    if(ip)printf("\n   remaining_bytes3 = %i\n",remaining_bytes3);

                  itoken4          = read_uint16(fp) ;
                  flags4           = read_uint16(fp) ;
                  block_bytes4     = read_uint32(fp) + 8 ;
                  label_length4    = read_uint8(fp)  ;
                  remaining_bytes4 = block_bytes4  - 9 ;
                  remaining_bytes3 = remaining_bytes3  - block_bytes4 ;

                    if(ip)printf("\n");
                    if(ip)printf(" EE itoken4 = %i, flags4 = %i, remaining_bytes4 = %i, label_length4 = %i\n",
                            itoken4, flags4, remaining_bytes4, label_length4) ;
                    if(ip)printf(" EE token_idc[%i] 0x%x = %s\n",itoken4,itoken4,token_idc[itoken4]) ;

                  for(;remaining_bytes4>0;){
                    if(ip)printf("\n ENTER LOOP remaining_bytes4 = %i\\n",remaining_bytes4);

                  itoken5          = read_uint16(fp) ;
                  flags5           = read_uint16(fp) ;
                  block_bytes5     = read_uint32(fp) + 8 ;
                  label_length5    = read_uint8(fp)  ;
                  remaining_bytes5 = block_bytes5  - 9 ;
                  remaining_bytes4 = remaining_bytes4  - block_bytes5 ;

                    if(ip)printf("\n");
                    if(ip)printf(" FF itoken5 = %i, flags5 = %i, remaining_bytes5 = %i, label_length5 = %i\n",
                            itoken5, flags5, remaining_bytes5, label_length5) ;
                    if(ip)printf(" FF token_idc[%i] 0x%x = %s\n\n",itoken5,itoken5,token_idc[itoken5]) ;

/*
 * =============================================================================
 *   Start processing new Patchset
 * =============================================================================
*/
                  switch(itoken5){
                    case(TERRAIN_PATCHSET_DISTANCE) :
                      tile_patchset->terrain_patchset_distance = read_uint32(fp) ;
                      remaining_bytes5 = remaining_bytes5 - 4;
                    if(ip)    printf(" terrain_patchset_distance = %i\n",tile_patchset->terrain_patchset_distance);
                      break ;
                    case(TERRAIN_PATCHSET_NPATCHES) :
                      tile_patchset->terrain_patchset_npatches = read_uint32(fp) ;
                      remaining_bytes5 = remaining_bytes5 - 4;
                    if(ip)    printf(" terrain_patchset_npatches = %i\n",tile_patchset->terrain_patchset_npatches);
                     break ;
                    case(TERRAIN_PATCHSET_PATCHES) :
                    if(ip)    printf("\n Case: TERRAIN_PATCHSET_PATCHES\n") ;
                    if(ip)    printf("\n Read patches\n") ;
                      number_of_patches = tile_patchset->terrain_patchset_npatches ;
                    if(ip)    printf("    number_of_patches    = %i\n",number_of_patches) ;
                      number_of_patches = number_of_patches*number_of_patches ;
                    if(ip)    printf("    number_of_patches**2 = %i\n",number_of_patches) ;

                    if(ip)    printf("  tile_patchset             = %p\n",(void *)tile_patchset);
                    if(ip)    printf("  remaining_bytes4           = %i\n",remaining_bytes4);
                    if(ip)    printf("  remaining_bytes5           = %i\n",remaining_bytes5);
                    if(ip)    tile_patchset->tile_patch = NULL ;
                    if(ip)    printf("  tile_patchset->tile_patch = %p\n",(void *)tile_patchset->tile_patch);
                    if(ip)    printf(" HH number_of_patches = %i\n",number_of_patches);
                    if(ip)    printf(" HH sizeof(TilePatch) = %i\n",(int)sizeof(TilePatch));


                      tile_patch = (TilePatch *)malloc(number_of_patches*sizeof(TilePatch)) ;
                    if(ip)    printf(" HH\n");
                    if(ip)    printf("  tile_patch = %p\n",(void *)tile_patch);
                      tile_patchset->tile_patch = tile_patch ;
                    if(ip)    printf("  tile_patchset->tile_patch = %p\n",(void *)tile_patchset->tile_patch);

                    if(ip)    printf("\n ENTER LOOP TERRAIN_PATCHSET_PATCHES number = %i\n",number_of_patches);
                      for(l=0;l<number_of_patches;l++,tile_patch++) {
//                        tile_patch = &(tile_patchset->tile_patch[l]) ;
//                        printf("\n WITHIN LOOP TERRAIN_PATCHSET_PATCHES l, n = %i %i\n",l,number_of_patches);
//                        printf("  tile_patch = %p\n",(void *)tile_patch);
//                        printf(  " GG  remaining_bytes5 = %i\n",remaining_bytes5);
                        fflush(NULL) ;

                        itoken6          = read_uint16(fp) ;
                        flags6           = read_uint16(fp) ;
                        block_bytes6     = read_uint32(fp) + 8 ;
                        label_length6    = read_uint8(fp)  ;
                        remaining_bytes6 = block_bytes6  - 9 ;
                        remaining_bytes5 = remaining_bytes5  - block_bytes6 ;

                        if(itoken6 != TERRAIN_PATCHSET_PATCH){
                          printf("  Token not recognised.  itoken6 = %i\n",itoken6);
                          continue ;
                        }

//                        printf("\n");
//                        printf(" FF itoken6 = %i, flags6 = %i, remaining_bytes6 = %i, label_length6 = %i\n",
//                                  itoken6, flags6, remaining_bytes6, label_length6) ;
//                        printf(" FF token_idc[%i] 0x%x = %s\n\n",itoken6,itoken6,token_idc[itoken6]) ;
#if 0
                        for(;0<remaining_bytes6;remaining_bytes6--)getc(fp);

#else

                        tile_patch->flags     = read_uint32(fp);
                        tile_patch->centre_x  = read_real32(fp);
                        tile_patch->average_y = read_real32(fp);
                        tile_patch->centre_z  = read_real32(fp);
                        tile_patch->factor_y  = read_real32(fp);
                        tile_patch->range_y   = read_real32(fp);
                        tile_patch->radius_m  = read_real32(fp);
                        tile_patch->index     = read_uint32(fp);
                        tile_patch->X         = read_real32(fp);
                        tile_patch->Y         = read_real32(fp);
                        tile_patch->W         = read_real32(fp);
                        tile_patch->B         = read_real32(fp);
                        tile_patch->C         = read_real32(fp);
                        tile_patch->H         = read_real32(fp);
                        tile_patch->error_bias = read_real32(fp);

                        remaining_bytes6 = remaining_bytes6 - 60 ;
#endif
//                        printf(  " GG  remaining_bytes6 = %i\n",remaining_bytes6);
//                        printf(  " GG  remaining_bytes5 = %i\n",remaining_bytes5);
#if 0
                        if(0 == remaining_bytes6){
                          printf("  flags     = %i\n", tile_patch[l].flags);
                          printf("  centre_x  = %f\n", tile_patch[l].centre_x);
                          printf("  average_y = %f\n", tile_patch[l].average_y);
                          printf("  centre_z  = %f\n", tile_patch[l].centre_z);
                          printf("  factor_y  = %f\n", tile_patch[l].factor_y);
                          printf("  range_y   = %f\n", tile_patch[l].range_y);
                          printf("  radius_m  = %f\n", tile_patch[l].radius_m);
                          printf("  index     = %i\n", tile_patch[l].index);
                          printf("  X         = %f\n", tile_patch[l].X);
                          printf("  Y         = %f\n", tile_patch[l].X);
                          printf("  W         = %f\n", tile_patch[l].X);
                          printf("  B         = %f\n", tile_patch[l].X);
                          printf("  C         = %f\n", tile_patch[l].X);
                          printf("  H         = %f\n", tile_patch[l].X);
                          printf("  error_bias = %f\n", tile_patch[l].error_bias);
                        }
#endif
//                      exit(1) ;
                      }
//                      printf("\n EXIT LOOP TERRAIN_PATCHSET_PATCHES \n");
//                      printf(  " FF  remaining_bytes5 = %i\n",remaining_bytes5);
//                      printf(  " FF  remaining_bytes4 = %i\n",remaining_bytes4);
                      break ;
                    default :
                printf("  Token not recognised.  itoken4 = %i, %s\n",itoken4,token_idc[itoken4]) ;

                      break ;
                  }               // End of switch(itoken5)
//                  printf("\n EXIT SWITCH itoken5 = %i %s\n",itoken5,token_idc[itoken5]);
//                  printf(  " EE  remaining_bytes5 = %i\n",remaining_bytes5);
//                  printf(  " EE  remaining_bytes4 = %i\n",remaining_bytes4);
                }                 // End of k loop
//                  printf("\n EXIT LOOP itoken4 = %i %s\n",itoken4,token_idc[itoken4]);
//                  printf(  " DD  remaining_bytes4 = %i\n",remaining_bytes4);
//                  printf(  " DD  remaining_bytes3 = %i\n",remaining_bytes3);
                }                 // End of j loop
//                printf("\n EXIT LOOP itoken3 = %i %s\n",itoken3,token_idc[itoken3]);
//                printf(  " CC  remaining_bytes4 = %i\n",remaining_bytes3);
//                printf(  " CC  remaining_bytes3 = %i\n",remaining_bytes2);
                break ;
              default :
                printf("  Token not recognised.  itoken2 = %i, %s\n",itoken2,token_idc[itoken2]) ;
                break ;
            }       // End of Main level 2 switch
//            printf("\n EXIT SWITCH itoken1  = %s\n",token_idc[itoken1]);
//            printf(  " DD  remaining_bytes1 = %i\n",remaining_bytes1);

          }  ;        // End of outer for loop
          break ;
        default:
          printf("  Token not recognised.  itoken1 = %i, %s\n",itoken1,token_idc[itoken1]) ;
          break ;
      }            //  End of Outer level1 switch
       if(ip)printf("  Exit  routine : %s\n",myname);

      close_msfile(&msfile) ;
// To keep the compiler happy
      if(0){
        printf("  flags1, label_length1 = %i %i\n",flags1, label_length1);
        printf("  flags2, label_length2 = %i %i\n",flags2, label_length2);
        printf("  flags3, label_length3 = %i %i\n",flags3, label_length3);
        printf("  flags4, label_length4 = %i %i\n",flags4, label_length4);
        printf("  flags5, label_length5 = %i %i\n",flags5, label_length5);
        printf("  flags6, label_length6 = %i %i\n",flags6, label_length6);
      }
      return 0 ;
}

int read_elevations(TileListNode *tlnode){
  int    len, len2, i, j, k, iret;
  int    ip = 0 ;    // 0 = no printing
  size_t isize ;
  unsigned short  *z ;
  char   *name = NULL,
         *full_name ;
  char   *tiles_dir = "/TILES/"        ;
  char   *myname    = "read_elevations" ;
  FILE   *fp = NULL ;

      if(ip)printf("\n  Enter routine : %s\n",myname);
      name = tlnode->terrain_data.ybuffer;
      if(ip)printf("  Name = %s\n",name);
      len = strlen(ORroutedir) + strlen(tiles_dir) +  strlen(name) + 1 ;
      full_name = (char *)malloc(len) ;

      strcpy(full_name, ORroutedir) ;
      strcat(full_name, tiles_dir)  ;
      strcat(full_name, name)       ;
      if(ip){
        printf("  Full name = %s\n",full_name);
        printf("  TILEX = %i, TILEZ = %i\n",tlnode->tilex,tlnode->tiley);
//        ip =  (abs(tlnode->tilex-1449)<3)  && (abs(tlnode->tiley-10331)<3) ;
        printf("  X = %i, Y = %i, N = %i\n",
                    tlnode->terrain_data.nbx,
                    tlnode->terrain_data.nby,
                    tlnode->terrain_data.terrain_nsamples
                    ) ;
      }
/*
 *   Look for file allowing for uppercase/lowercase confision
 */
      iret = zr_find_msfile2(&full_name) ;
/*
 *   Open topography files (*_y.raw)
 *   These are binary files.  The Windows' C runtime routine fread
 *   needs the 'b' flag to prevent the preprocessing/translation
 *   that occurs with Windows text files.  Other systems ignore
 *   the extra flag.
 */
      if(!iret)fp = gopen(full_name,"rb");
      if(iret || NULL == fp){
        printf("   Routine %s,   file not found : %s\n",myname,name);
        free(full_name);
        return 1 ;
      }else{
        if(ip)printf("  File found\n") ;
        len  = tlnode->terrain_data.terrain_nsamples;
        len2 = len*len ;
        tlnode->terrain_data.elevations = (unsigned short *)malloc(len2*sizeof(short)) ;
        z = tlnode->terrain_data.elevations;
/*
 *  Read topography
 */
        isize = fread(z,sizeof(short),(size_t)len2,fp) ;
        if(isize != (size_t)len2){
          printf("  Routine %s\n",myname)  ;
          printf("  Read truncated while reading topography from\n")  ;
          printf("  file : %s\n",full_name) ;
        }
        free(full_name) ;
        gclose(fp) ;
        tlnode->loaded = 1 ;
/*
 *  Print topography ??
 */
        if(ip){
          printf("  TILEX = %i, TILEZ = %i\n",tlnode->tilex,tlnode->tiley);
          z = tlnode->terrain_data.elevations;
          if(ip)printf("  TILEX = %i, TILEZ = %i\n",tlnode->tilex,tlnode->tiley);
          for(j=11;j<15;j++){
            if(ip)printf(" elevation j = %i,  ",j);
            for(i=11;i<15;i++){
              k = j+len * i ;
//              printf("  %p",(void *)&z[k]);
              if(ip)printf("  %6i",(int)z[k]);
            }
            if(ip)printf("\n");
          }
        }
      }
      return 0;
}
