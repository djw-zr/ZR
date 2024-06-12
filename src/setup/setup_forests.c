/*
 *******************************************************************************
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File setup_forests.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *******************************************************************************
 *
 *   File contains routines for setting up the forests.  This is done by
 *   generating a single shape for each forest.
 *
 *******************************************************************************
 */


ShapeNode *unit_tree_shape(void) ;

int  setup_forests(void){

  int  ip = 0 ;  // DEBUG
  int  i, n, mask ;
  int  tile_x ;
  int  tile_y ;
  int  fuid ;       // uid to be used for next forest tree.
  double    scale, ss, cc, xx, yy, xc, yc ;
  double    height ;
  WorldNode *wnode ;
  WorldItem *witem ;
  WorldItem *tree  ;
  ShapeNode *snode ;
  ForestObj *forest_obj ;
  char      *tree_texture ;
  char      *core_name = "Forest_Tree_" ;
  char      *shape_name ;
//  char      **tile_mask_array = NULL ;  // Array of masks for tracks and roads

  static ShapeNode *snode0 = NULL ;
  char         *my_name = "setup_forests";

      if(ip)printf("  Enter routine %s\n",my_name) ;
/*
 *  On entry initialise the tile mask
 */
      if(tile_mask_array == NULL)tile_mask_init() ;
/*
 *  Loop over the world tiles
 */
      for(wnode=worldlist_beg; wnode != NULL; wnode=wnode->next){
        tile_x = wnode->tile_x ;
        tile_y = wnode->tile_y ;
        if(ip)printf("  Routine %s,  tile_x = %i, tile_y = %i\n",
                                         my_name, tile_x, tile_y) ;
/*
 * Set fuid to one more than the largest uid allocated for items on this tile
 */
        fuid = 0 ;
        for(witem=wnode->world_item; witem != NULL; witem=witem->next){
          if(fuid < witem->uid) fuid = witem->uid ;
        }
        fuid++ ;
/*
 *  Loop over the world items in  each tile, looking for forests.
 */
        for(witem=wnode->world_item; witem != NULL; witem=witem->next){
          if(witem->worldtype != FOREST) continue ;
          snode = witem->snode ;
          if(snode) continue ; //  Forest trees have type FOREST but no shape.
          forest_obj = &(witem->u.forest_obj) ;
/*
 *  Debug printing
 *    witem->X, Y, Z : Units m, relative to centre of tile.
 *    forest_obj->X, Y : units m. :  Extent of forest
 *
 *    The extent should be relative to the origin of the forest.
 */
          if(ip){
            printf("    Forest: uid = %i, filename = %s, filename2 = %s\n",
                   witem->uid, witem->filename, witem->filename2) ;
            printf("            snode = %p\n",(void *)snode) ;
            printf("            X,  Y,  Z  = %f %f %f\n",witem->X,witem->Y,witem->Z) ;
            printf("            AX, AY, AZ, ANG = %f %f %f : %f\n",witem->AX,witem->AY,witem->AZ, witem->ANG) ;
            printf("            forest_obj = %p\n",(void *)forest_obj) ;
            printf("            texture    = %p\n",(void *)forest_obj->tree_texture) ;
            printf("            texture = %s, X, Y = %f %f, range min max = %f %f,"
                   "population = %i, width, height = %f %f\n", forest_obj->tree_texture,
                  forest_obj->X,               forest_obj->Y,
                  forest_obj->scale_range_min, forest_obj->scale_range_max,
                  forest_obj->population,      forest_obj->width, forest_obj->height) ;
          }
/*
 *  Generate the shape name for this forest, made up of
 *  "Forest_Tree_" plus the name of the forest texture
 */
          tree_texture =  zr_corename(forest_obj->tree_texture) ;
          if(ip)printf("    %s : texture_name = %s\n",my_name,tree_texture) ;
          n = strlen(core_name) + strlen(tree_texture) + 1 ;
          shape_name = malloc(n*sizeof(char)) ;
          strcpy(shape_name,core_name) ;
          strcat(shape_name,tree_texture) ;
          if(ip)printf("    %s : shape_name   = %s\n",my_name,shape_name) ;
/*
 *  Search for name amongst existing shapes
 */
          for(snode = shapelist_beg; snode != NULL; snode=snode->next){
            if(ip){
              printf("   snode       = %p\n",(void *)snode) ;
              printf("   snode->name = %p\n",(void *)snode->name) ;
              printf("   shape_name  = %p\n",(void *)shape_name) ;
            }
            if(snode->name && !strcmp(snode->name,shape_name)) break ;
          }
          if(ip)printf("    %s : AA shape   = %p\n",my_name,(void *) snode) ;
          if(ip && snode) printf("    %s : AA name   = %s\n",my_name,snode->name) ;
/*
 *  If shape does not already exist create it with the right texture
 *  and add it to the list of shapes
 */
          if(snode == NULL){
            snode                  = unit_tree_shape() ;
            snode->name            = shape_name ;
            snode->texture_name[0] = forest_obj->tree_texture ;
            shapelist_end->next    = snode;
            shapelist_end          = snode ;
//            print_shape_file_data(snode) ;
          }
/*
 *  Generate Forest
 */
#if 0
          if(snode0 == NULL){
            snode0 = unit_tree_shape() ;
            print_shape_file_data(snode0) ;
            shapelist_end->next = snode0;
            shapelist_end = snode ;
          }
#endif
/*
 *  Calculate sine and cosine of forest 'angle'
 */
          cc = cos(radian*witem->ANG) ;
          ss = sin(radian*witem->ANG) ;
/*
 *  Generate a set of trees
 */
          srand(1) ;   //  Ensure tree positions are repeatable
          if(ip) printf("  FUID = %i\n",fuid) ;
          for(i=0; i<forest_obj->population; i++){
            if(ip) printf("  FUID = %i\n",fuid) ;
/*
 *  Position of tree relative to the centre of the unrotated forest object.
 */
            xx = (0.5 -(rand()/(double)RAND_MAX))*forest_obj->X ;
            yy = (0.5 -(rand()/(double)RAND_MAX))*forest_obj->Y ;
/*
 *   Tree position relative to centre of tile (tile_x, tile_y)
 */
            xc = witem->X + xx*cc - yy*ss ;
            yc = witem->Y + yy*cc + xx*ss ;
/*
 *  Check the position is valid
 */
            mask = check_tile_mask( tile_x, tile_y, xc, yc) ;
            if(ip){
              printf("  Routine %s.  %i %i :: %f %f :: %i\n",
                               my_name, tile_x, tile_y, xc, yc, mask) ;
            }
            if(mask)continue ;  //  Position not valid so skip
/*
 *  Position is valid.
 *  Generate new world and shape items for this tree
 *  Note: structures below the ShapeNode are shared by all forest trees
 */
            tree = (WorldItem *)malloc(sizeof(WorldItem)) ;
            world_item_init(tree) ;
            tree->world_node = wnode ;
#if 0
            snode = (ShapeNode *)malloc(sizeof(ShapeNode)) ;
            *(snode) = *(snode0) ;
#endif
            tree->snode = snode ;
/*
 *  Add the tree texture for this forest
 */
//            snode->texture_name[0] = forest_obj->tree_texture ;
            tree->worldtype = FOREST ;
            tree->uid = fuid++ ;
            tree->X = xc ;
            tree->Y = yc ;
            find_height(tile_x, tile_y, xc, yc, &height) ;
            tree->Z  = height ;

            scale = (double)rand()/(double)RAND_MAX ;
            scale = forest_obj->scale_range_min +
                    scale*(forest_obj->scale_range_max - forest_obj->scale_range_min) ;
            tree->SX = scale*forest_obj->width*0.5 ; // Geographic
            tree->SY = scale*forest_obj->width*0.5 ; //    coordinates
            tree->SZ = scale*forest_obj->height ;    //        ...

            tree->next = witem->next ;
            witem->next = tree ;
          }  //  End loop over forest trees
        }    //  End loop over world items
      }      //  End loop over tiles/world nodes
/*
 *   Release memory used by mask arrays
 */
      if(tile_mask_array){
        for(i=0; i<tile_array_num; i++){
          if(tile_mask_array[i])free(tile_mask_array[i]) ;
        }
        free(tile_mask_array) ;
        tile_mask_array = NULL ;
      }

      if(ip)printf("  Exit routine %s\n",my_name) ;

      return 0 ;
}

/*
 *******************************************************************************
 *   Routine to generate a new 'unit tree' shapenode
 *   i.e. a basic tree with width, length and height equal to 1.0.
 *******************************************************************************
 */
#if 1

ShapeNode *unit_tree_shape(void){

  int       i, n ;
  ShapeNode *snode ;
  ShapeVolume *shape_vol  ;
  TexLevelLow *texlevel   ;
  SubObject   *sub_object ;

  static Matrix4x3 matrix     = {  "Tree Matix", MAT_GENERAL, 0 ,
                                     1.0, 0.0, 0.0 ,
                                     0.0, 1.0, 0.0 ,
                                     0.0, 0.0, 1.0 ,
                                     0.0, 0.0, 0.0   } ;
  static Vector3   points[]   =  { { -1.0,  0.0,  0.0},
                                   {  1.0,  0.0,  0.0},
                                   { -1.0,  1.0,  0.0},
                                   {  1.0,  1.0,  0.0},
                                   { -0.707107, 0.0, -0.707107},
                                   {  0.707107, 0.0,  0.707107},
                                   { -0.707107, 1.0, -0.707107},
                                   {  0.707107, 1.0,  0.707107},
                                   {  0.0,  0.0, -1.0},
                                   {  0.0,  0.0,  1.0},
                                   {  0.0,  1.0, -1.0},
                                   {  0.0,  1.0,  1.0},
                                   {  0.707107, 0.0, -0.707107},
                                   { -0.707107, 0.0,  0.707107},
                                   {  0.707107, 1.0, -0.707107},
                                   { -0.707107, 1.0,  0.707107} } ;

  static Vector3   normals[]  =  { { 0.0,       0.0, -1.0},
                                   { 0.0,       0.0,  1.0},
                                   {  0.707107, 0.0, -0.707107},
                                   { -0.707107, 0.0,  0.707107},
                                   {  1.0,      0.0,  0.0},
                                   { -1.0,      0.0,  0.0},
                                   {  0.707107, 0.0,  0.707107},
                                   { -0.707107, 0.0, -0.707107} } ;

  static UVVector2 uvpoints[] =  { {0.0, 1.0},  {1.0, 1.0}, {1.0, 0.0}, {0.0, 0.0},
                                   {0.0, 0.0}, {1.0, 0.0} } ;

#if 0
  static Vertex vertex0[]     = {   {   0,    3,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    1,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    0,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    2,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    1,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    3,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    2,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    0,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    7,    2, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    5,    2, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    4,    2, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    6,    2, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    5,    3, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    7,    3, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    6,    3, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    4,    3, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   11,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    9,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    8,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   10,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    9,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   11,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   10,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    8,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   15,    6, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   13,    6, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   12,    6, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   14,    6, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   13,    7, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   15,    7, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   14,    7, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   12,    7, 0xffffffff, 0xff000000,  1,   NULL}  }  ;

  static int ivertex0[]       = { 2, 1, 0, 3, 1, 5, 4, 0,
                                  2, 1, 0, 3, 1, 5, 4, 0,
                                  2, 1, 0, 3, 1, 5, 4, 0,
                                  2, 1, 0, 3, 1, 5, 4, 0  }  ;
  static VertexIdx ivertex_idx0[] = {
          {0, 1, 2}, {3, 0, 2}, {4, 5, 6}, {7, 4, 6}, {8, 9, 10}, {11, 8, 10}, {12, 13, 14}, {15, 12, 14}, {16, 17, 18}, {19, 16, 18}, {20, 21, 22}, {23, 20, 22}, {24, 25, 26}, {27, 24, 26}, {28, 29, 30}, {31, 28, 30} };

  static uint inormal_idx0[] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7} ;
  static uint iflag0[]       = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ;
#endif

  static Vertex vertex1[]     = {   {   0,    3,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    1,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    0,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    2,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    1,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    3,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    2,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    0,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   11,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    9,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    8,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   10,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    9,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   11,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   10,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    8,    5, 0xffffffff, 0xff000000,  1,   NULL}  }  ;

  static int ivertex1[]       = { 2, 1, 0, 3, 1, 5, 4, 0,
                                  2, 1, 0, 3, 1, 5, 4, 0  }  ;
  static VertexIdx ivertex_idx1[] = {
          {0, 1, 2}, {3, 0, 2}, {4, 5, 6}, {7, 4, 6}, {8, 9, 10}, {11, 8, 10}, {12, 13, 14}, {15, 12, 14} };

  static uint inormal_idx1[] = {0, 0, 1, 1, 4, 4, 5, 5} ;
  static uint iflag1[]       = {0, 0, 0, 0, 0, 0, 0, 0} ;




  char      *my_name = "forest_shape_init" ;

      snode = (ShapeNode *)malloc(sizeof(ShapeNode)) ;
      init_shape_node(snode) ;
      snode->name     = NULL ;

      snode->basic    = 1 ;             //  Assume needed everwhere
      snode->needed   = 1 ;
      snode->nvolumes = 1 ;
      shape_vol = (ShapeVolume *)malloc(sizeof(ShapeVolume)) ;
      snode->shape_vol  = shape_vol ;
      shape_vol->vec.X  = 0.0 ;
      shape_vol->vec.Y  = 0.0 ;
      shape_vol->vec.Z  = 0.0 ;
      shape_vol->radius = 30.0 ;     //  Assume all trees are or order 30m height

      snode->nshaders = 1 ;
      snode->shader = (char **)malloc(sizeof(char **)) ;
      snode->shader[0] = strdup("BlendATexDiff") ;

      snode->nfilters = 1 ;
      snode->filter = (char **)malloc(sizeof(char **)) ;
      snode->filter[0] = strdup("MipLinear") ;

      snode->npoints = n = 16 ;
      snode->point   = (Vector3 *)malloc(n*sizeof(Vector3)) ;
      for(i=0; i<n; i++){
        snode->point[i]   = points[i] ;
      }

      snode->nnormals = n = 8 ;
      snode->normal   = (Vector3 *)malloc(n*sizeof(Vector3)) ;
      for(i=0; i<n; i++){
        snode->normal[i]   = normals[i] ;
      }
      snode->nuvpoints = n = 6 ;
      snode->uvpoint   = (UVVector2 *)malloc(n*sizeof(UVVector2)) ;
      for(i=0; i<n; i++){
        snode->uvpoint[i]   = uvpoints[i] ;
      }

      snode->nsort_vectors = 0 ;
      snode->sort_vector  = NULL ;
      snode->ncolors      = 0 ;
      snode->color        = NULL ;

      snode->nmatrices = 1 ;
      snode->matrix    = (Matrix4x3 *)malloc(sizeof(Matrix4x3)) ;
      *(snode->matrix) = matrix ;

      snode->n_textures   = 1 ;
      snode->texture_name = (char **)malloc(sizeof(char **)) ;
      snode->texture_name[0] = strdup("AU_Ash.ace") ;        //  Default texture
      snode->texture      = (TextureNode **)malloc(sizeof(TextureNode)) ;
      snode->texture[0]   = NULL ;
      snode->n_texlevel_low = 1 ;
      snode->texlevel_low   = (TexLevelLow *)malloc(sizeof(TexLevelLow)) ;
      texlevel = snode->texlevel_low ;
      texlevel->iImage        = 0 ;
      texlevel->FilterMode    = 0 ;
      texlevel->MipMapLODBias = 0.0 ;
      texlevel->BorderColor   = 0 ;

      snode->nlight_materials = 0 ;
      snode->light_material   = NULL ;

      snode->nlight_model_cfgs = 1 ;
      snode->light_model_cfg   = (LightModelCfg *)malloc(sizeof(LightModelCfg)) ;
      snode->light_model_cfg[0].flags      = 0 ;
      snode->light_model_cfg[0].n_uv_ops   = 1 ;
UVOps *uv_ops ;
      uv_ops = (UVOps *)malloc(sizeof(UVOps)) ;
      snode->light_model_cfg[0].uv_ops = uv_ops ;
      uv_ops[0].type            = 84 ;
      uv_ops[0].tex_addr_mode   = 1 ;
      uv_ops[0].src_uv_idx      = 0 ;
      uv_ops[0].unknown_param_3 = 0.0 ;
      uv_ops[0].unknown_param_4 = 0.0 ;

      snode->n_vtx_states = 1 ;
      snode->vtx_state    = (VTXState *)malloc(sizeof(VTXState)) ;
VTXState *vtx_state = snode->vtx_state ;
      vtx_state[0].flags         = 0 ;
      vtx_state[0].imatrix       = 0 ;
      vtx_state[0].light_mat_idx = -9 ;
      vtx_state[0].light_cfg_idx = 0 ;
      vtx_state[0].light_flags   = 2 ;
      vtx_state[0].imatrix2      = 0 ;

      snode->n_prim_states = 1 ;
      snode->prim_state  = (PrimState *)malloc(sizeof(PrimState)) ;
PrimState *prim_state = snode->prim_state ;
      prim_state[0].name = NULL ;
      prim_state[0].flags           = 0 ;
      prim_state[0].ishader         = 0 ;
      prim_state[0].n_tex_idxs      = 1 ;
      prim_state[0].tex_idx = (uint *)malloc(sizeof(uint)) ;
      prim_state[0].tex_idx[0]      = 0 ;
      prim_state[0].zbias           = 0.0 ;
      prim_state[0].ivtx_state      = 0 ;
      prim_state[0].alpha_test_mode = 1 ;
      prim_state[0].light_cfgidx    = 0 ;
      prim_state[0].zbuf_mode       = 1 ;

      snode->dlevel_selection = 0 ;
      snode->nhierarchy       = 0 ;
      snode->hierarchy        = NULL ;
      snode->hierarchy_flag   = NULL ;

      snode->n_lod_controls = 1 ;
      snode->lod_control  = (LodControl *)malloc(sizeof(LodControl)) ;
LodControl *lod_control = snode->lod_control ;
      lod_control[0].dlev_bias = 0 ;
      lod_control[0].n_dist_levels = 1 ;
      lod_control[0].dist_level = (DistLevel *)malloc(sizeof(DistLevel)) ;
DistLevel *dist_level = lod_control[0].dist_level ;
/*
 *******************************************************************************
 *   Distance level 0
 *******************************************************************************
 */
#if 0
      dist_level[0].index = 0 ;
      dist_level[0].dlevel_selection = 500.0 ;
      dist_level[0].n_hierarchy      = 1 ;
      dist_level[0].n_sub_objects    = 1 ;
      dist_level[0].gl_display_list  = 0 ;

      dist_level[0].hierarchy =(int *)malloc(2*sizeof(int)) ;
      dist_level[0].hierarchy[0] = -1 ;

      dist_level[0].sub_object =(SubObject *)malloc(sizeof(SubObject)) ;
SubObject *sub_object = dist_level[0].sub_object  ;

      sub_object[0].flags                = 1024 ;
      sub_object[0].sort_vector_idx      = -1 ;
      sub_object[0].vol_idx              = -1 ;
      sub_object[0].src_vtx_fmt_flags    = 466 ;
      sub_object[0].dst_vtx_fmt_flags    = 452 ;
      sub_object[0].face_normals         = 16 ;
      sub_object[0].tx_light_cmds        = 1 ;
      sub_object[0].nodex_tri_list_idxs  = 0 ;
      sub_object[0].tri_list_idxs        = 48 ;
      sub_object[0].linelist_idxs        = 0 ;
      sub_object[0].nodex_tri_list_idxs2 = 0 ;
      sub_object[0].tri_lists            = 1 ;
      sub_object[0].line_lists           = 0 ;
      sub_object[0].pt_lists             = 0 ;
      sub_object[0].nodex_tri_lists      = 0 ;
      sub_object[0].n_geom_nodes         = 1 ;
      sub_object[0].n_node_maps          = 1 ;
      sub_object[0].n_shaders            = 1 ;
      sub_object[0].n_light_cfgs         = 1 ;
      sub_object[0].n_vertices           = 32 ;
      sub_object[0].n_vertex_sets        = 1 ;
      sub_object[0].n_tri_lists          = 1 ;

      sub_object[0].node_map = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].node_map[0] = 0 ;

      sub_object[0].shader = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].shader[0] = 0 ;

      sub_object[0].light_cfg = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].light_cfg[0] = 0 ;
/*
 *  Sub-object vertices
 */
      n = sub_object[0].n_vertices ;
      sub_object[0].vertex = (Vertex *)malloc(n*sizeof(Vertex)) ;
      for(i=0; i<n; i++){
        sub_object[0].vertex[i] = vertex0[i] ;
        sub_object[0].vertex[i].vertex_uv = (uint *)malloc(sizeof(uint)) ;
        sub_object[0].vertex[i].vertex_uv[0] = ivertex0[i] ;
      }
/*
 *  Sub-object vertex sets
 */
      sub_object[0].vertex_set = (VertexSet *)malloc(sizeof(VertexSet)) ;
      sub_object[0].vertex_set[0].vtx_state_idx = 0 ;
      sub_object[0].vertex_set[0].start_vtx_idx = 0 ;
      sub_object[0].vertex_set[0].vtx_count     = 32 ;
/*
 *  Sub-object Trilists
 */
      n = sub_object[0].n_tri_lists ;
      sub_object[0].tri_list = (TriList *)malloc(n*sizeof(TriList)) ;
#if 0
      sub_object[0].tri_list[0].prim_state_trilist = 0 ;
      sub_object[0].tri_list[0].n_vertex_idxs      = 0 ;
      sub_object[0].tri_list[0].vertex_idx         = NULL ;
      sub_object[0].tri_list[0].n_normal_idxs      = 0 ;
      sub_object[0].tri_list[0].normal_idx         = NULL ;
      sub_object[0].tri_list[0].n_flags            = 0 ;
      sub_object[0].tri_list[0].flag               = NULL ;
#endif
      sub_object[0].tri_list[0].prim_state_trilist = 0 ;
      sub_object[0].tri_list[0].n_vertex_idxs      = 16 ;
      n = sub_object[0].tri_list[0].n_vertex_idxs ;
      sub_object[0].tri_list[0].vertex_idx = (VertexIdx *)malloc(n*sizeof(VertexIdx)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[0].vertex_idx[i] = ivertex_idx0[i] ;
      }

      sub_object[0].tri_list[0].n_normal_idxs   = 16 ;
      n = sub_object[0].tri_list[0].n_normal_idxs ;
      sub_object[0].tri_list[0].normal_idx = (uint *)malloc(n*sizeof(uint)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[0].normal_idx[i] = inormal_idx0[i] ;
      }

      sub_object[0].tri_list[0].n_flags  = 16 ;
      n = sub_object[0].tri_list[0].n_flags ;
      sub_object[0].tri_list[0].flag  = (uint *)malloc(n*sizeof(uint)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[0].flag[i] = iflag0[i] ;
      }
#endif
/*
 *******************************************************************************
 *   Distance level 1
 *******************************************************************************
 */
      dist_level[0].index            = 1 ;
      dist_level[0].dlevel_selection = 2000.0 ;
      dist_level[0].n_hierarchy      = 1 ;
      dist_level[0].n_sub_objects    = 1 ;
      dist_level[0].gl_display_list  = 0 ;

      dist_level[0].hierarchy =(int *)malloc(2*sizeof(int)) ;
      dist_level[0].hierarchy[0] = -1 ;

      dist_level[0].sub_object =(SubObject *)malloc(sizeof(SubObject)) ;
      sub_object = dist_level[0].sub_object  ;

      sub_object[0].flags                = 1024 ;
      sub_object[0].sort_vector_idx      = -1 ;
      sub_object[0].vol_idx              = -1 ;
      sub_object[0].src_vtx_fmt_flags    = 466 ;
      sub_object[0].dst_vtx_fmt_flags    = 452 ;
      sub_object[0].face_normals         = 8 ;
      sub_object[0].tx_light_cmds        = 1 ;
      sub_object[0].nodex_tri_list_idxs  = 0 ;
      sub_object[0].tri_list_idxs        = 24 ;
      sub_object[0].linelist_idxs        = 0 ;
      sub_object[0].nodex_tri_list_idxs2 = 0 ;
      sub_object[0].tri_lists            = 1 ;
      sub_object[0].line_lists           = 0 ;
      sub_object[0].pt_lists             = 0 ;
      sub_object[0].nodex_tri_lists      = 0 ;
      sub_object[0].n_geom_nodes         = 1 ;
      sub_object[0].n_node_maps          = 1 ;
      sub_object[0].n_shaders            = 1 ;
      sub_object[0].n_light_cfgs         = 1 ;
      sub_object[0].n_vertices           = 16 ;
      sub_object[0].n_vertex_sets        = 1 ;
      sub_object[0].n_tri_lists          = 1 ;

      sub_object[0].node_map = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].node_map[0] = 0 ;

      sub_object[0].shader = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].shader[0] = 0 ;

      sub_object[0].light_cfg = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].light_cfg[0] = 0 ;

/*
 *  Sub-object vertices
 */
      n = sub_object[0].n_vertices ;
      sub_object[0].vertex = (Vertex *)malloc(n*sizeof(Vertex)) ;
      for(i=0; i<n; i++){
        sub_object[0].vertex[i] = vertex1[i] ;
        sub_object[0].vertex[i].vertex_uv = (uint *)malloc(sizeof(uint)) ;
        sub_object[0].vertex[i].vertex_uv[0] = ivertex1[i] ;
      }
/*
 *  Sub-object vertex sets
 */
      sub_object[0].vertex_set = (VertexSet *)malloc(sizeof(VertexSet)) ;
      sub_object[0].vertex_set[0].vtx_state_idx = 0 ;
      sub_object[0].vertex_set[0].start_vtx_idx = 0 ;
      sub_object[0].vertex_set[0].vtx_count     = 16 ;

/*
 *  Sub-object Trilists
 */
      n = sub_object[0].n_tri_lists ;
      sub_object[0].tri_list = (TriList *)malloc(n*sizeof(TriList)) ;
#if 0
      sub_object[0].tri_list[0].prim_state_trilist = 0 ;
      sub_object[0].tri_list[0].n_vertex_idxs      = 0 ;
      sub_object[0].tri_list[0].vertex_idx         = NULL ;
      sub_object[0].tri_list[0].n_normal_idxs      = 0 ;
      sub_object[0].tri_list[0].normal_idx         = NULL ;
      sub_object[0].tri_list[0].n_flags            = 0 ;
      sub_object[0].tri_list[0].flag               = NULL ;
#endif
      sub_object[0].tri_list[0].prim_state_trilist = 0 ;
      sub_object[0].tri_list[0].n_vertex_idxs      = 2 ;
      n = sub_object[0].tri_list[0].n_vertex_idxs ;
      sub_object[0].tri_list[0].vertex_idx = (VertexIdx *)malloc(n*sizeof(VertexIdx)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[0].vertex_idx[i] = ivertex_idx1[i] ;
      }

      sub_object[0].tri_list[0].n_normal_idxs   = 8 ;
      n = sub_object[0].tri_list[0].n_normal_idxs ;
      sub_object[0].tri_list[0].normal_idx = (uint *)malloc(n*sizeof(uint)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[0].normal_idx[i] = inormal_idx1[i] ;
      }

      sub_object[0].tri_list[0].n_flags  = 8 ;
      n = sub_object[0].tri_list[0].n_flags ;
      sub_object[0].tri_list[0].flag  = (uint *)malloc(n*sizeof(uint)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[0].flag[i] = iflag1[i] ;
      }

/*
 *******************************************************************************
 *   End Distance Level Processing
 *******************************************************************************
 */

      snode->n_animations = 0 ;
      snode->animation    = NULL ;

      return snode ;
}

#else

/*
 *   This is the original form of 'unit_tree_shape' based on AU_Ash_20.s
 */
ShapeNode *unit_tree_shape(void){

  int       i, n ;
  ShapeNode *snode ;
  ShapeVolume *shape_vol ;
  TexLevelLow *texlevel ;

  static Matrix4x3 matrix     = {  "Tree Matix", MAT_GENERAL, 0 ,
                                     1.0, 0.0, 0.0 ,
                                     0.0, 1.0, 0.0 ,
                                     0.0, 0.0, 1.0 ,
                                     0.0, 0.0, 0.0   } ;
#if 0
  static Vector3   points[]   =  { { -1.0,  0.0,  0.0},
                                   {  1.0,  0.0,  0.0},
                                   { -1.0,  0.0,  1.0},
                                   {  1.0,  0.0,  1.0},
                                   { -0.707107, -0.707107,  0.0},
                                   {  0.707107,  0.707107,  0.0},
                                   { -0.707107, -0.707107,  1.0},
                                   {  0.707107,  0.707107,  1.0},
                                   {  0.0, -1.0,  0.0},
                                   {  0.0,  1.0,  0.0},
                                   {  0.0, -1.0,  1.0},
                                   {  0.0,  1.0,  1.0},
                                   {  0.707107, -0.707107,  0.0},
                                   { -0.707107,  0.707107,  0.0},
                                   {  0.707107, -0.707107,  1.0},
                                   { -0.707107,  0.707107,  1.0} } ;

  static Vector3   normals[]  =  { { 0.0,       -1.0,      0.0},
                                   { 0.0,        1.0,      0.0},
                                   {  0.707107, -0.707107, 0.0},
                                   { -0.707107,  0.707107, 0.0},
                                   {  1.0,       0.0,      0.0},
                                   { -1.0,       0.0,      0.0},
                                   {  0.707107,  0.707107, 0.0},
                                   { -0.707107, -0.707107, 0.0} } ;
#else
  static Vector3   points[]   =  { { -1.0,  0.0,  0.0},
                                   {  1.0,  0.0,  0.0},
                                   { -1.0,  1.0,  0.0},
                                   {  1.0,  1.0,  0.0},
                                   { -0.707107, 0.0, -0.707107},
                                   {  0.707107, 0.0,  0.707107},
                                   { -0.707107, 1.0, -0.707107},
                                   {  0.707107, 1.0,  0.707107},
                                   {  0.0,  0.0, -1.0},
                                   {  0.0,  0.0,  1.0},
                                   {  0.0,  1.0, -1.0},
                                   {  0.0,  1.0,  1.0},
                                   {  0.707107, 0.0, -0.707107},
                                   { -0.707107, 0.0,  0.707107},
                                   {  0.707107, 1.0, -0.707107},
                                   { -0.707107, 1.0,  0.707107} } ;

  static Vector3   normals[]  =  { { 0.0,       0.0, -1.0},
                                   { 0.0,       0.0,  1.0},
                                   {  0.707107, 0.0, -0.707107},
                                   { -0.707107, 0.0,  0.707107},
                                   {  1.0,      0.0,  0.0},
                                   { -1.0,      0.0,  0.0},
                                   {  0.707107, 0.0,  0.707107},
                                   { -0.707107, 0.0, -0.707107} } ;

#endif

  static UVVector2 uvpoints[] =  { {0.0, 1.0},  {1.0, 1.0}, {1.0, 0.0}, {0.0, 0.0},
//                                   {0.0, 0.07751}, {1.0, 0.07751} } ;
                                   {0.0, 0.0}, {1.0, 0.0} } ;

  static Vertex vertex0[]       = {   {   0,    3,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    1,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    0,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    2,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    1,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    3,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    2,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    0,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    7,    2, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    5,    2, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    4,    2, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    6,    2, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    5,    3, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    7,    3, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    6,    3, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    4,    3, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   11,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    9,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    8,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   10,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    9,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   11,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   10,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    8,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   15,    6, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   13,    6, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   12,    6, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   14,    6, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   13,    7, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   15,    7, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   14,    7, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   12,    7, 0xffffffff, 0xff000000,  1,   NULL}  }  ;

  static int ivertex0[]       = { 2, 1, 0, 3, 1, 5, 4, 0,
                                  2, 1, 0, 3, 1, 5, 4, 0,
                                  2, 1, 0, 3, 1, 5, 4, 0,
                                  2, 1, 0, 3, 1, 5, 4, 0  }  ;
  static VertexIdx ivertex_idx0[] = {
          {0, 1, 2}, {3, 0, 2}, {4, 5, 6}, {7, 4, 6}, {8, 9, 10}, {11, 8, 10}, {12, 13, 14}, {15, 12, 14}, {16, 17, 18}, {19, 16, 18}, {20, 21, 22}, {23, 20, 22}, {24, 25, 26}, {27, 24, 26}, {28, 29, 30}, {31, 28, 30} };

  static uint inormal_idx0[] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7} ;
  static uint iflag0[]       = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ;

  static Vertex vertex1[]     = {   {   0,    3,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    1,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    0,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    2,    0, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    1,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    3,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    2,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    0,    1, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   11,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    9,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    8,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   10,    4, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    9,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   11,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,   10,    5, 0xffffffff, 0xff000000,  1,   NULL},
                                    {   0,    8,    5, 0xffffffff, 0xff000000,  1,   NULL}  }  ;

  static int ivertex1[]       = { 2, 1, 0, 3, 1, 5, 4, 0,
                                  2, 1, 0, 3, 1, 5, 4, 0  }  ;
  static VertexIdx ivertex_idx1[] = {
          {0, 1, 2}, {3, 0, 2}, {4, 5, 6}, {7, 4, 6}, {8, 9, 10}, {11, 8, 10}, {12, 13, 14}, {15, 12, 14} };

  static uint inormal_idx1[] = {0, 0, 1, 1, 4, 4, 5, 5} ;
  static uint iflag1[]       = {0, 0, 0, 0, 0, 0, 0, 0} ;




  char      *my_name = "forest_shape_init" ;

      snode = (ShapeNode *)malloc(sizeof(ShapeNode)) ;
      init_shape_node(snode) ;
      snode->name = strdup("Forest_Tree") ;

      snode->nvolumes = 1 ;
      shape_vol = (ShapeVolume *)malloc(sizeof(ShapeVolume)) ;
      snode->shape_vol  = shape_vol ;
      shape_vol->vec.X  = 0.0 ;
      shape_vol->vec.Y  = 0.0 ;
      shape_vol->vec.Z  = 0.0 ;
      shape_vol->radius = 30.0 ;     //  Assume all trees are or order 30m height

      snode->nshaders = 1 ;
      snode->shader = (char **)malloc(sizeof(char **)) ;
      snode->shader[0] = strdup("BlendATexDiff") ;

      snode->nfilters = 1 ;
      snode->filter = (char **)malloc(sizeof(char **)) ;
      snode->filter[0] = strdup("MipLinear") ;

      snode->npoints = n = 16 ;
      snode->point   = (Vector3 *)malloc(n*sizeof(Vector3)) ;
      for(i=0; i<n; i++){
        snode->point[i]   = points[i] ;
      }

      snode->nnormals = n = 8 ;
      snode->normal   = (Vector3 *)malloc(n*sizeof(Vector3)) ;
      for(i=0; i<n; i++){
        snode->normal[i]   = normals[i] ;
      }
      snode->nuvpoints = n = 6 ;
      snode->uvpoint   = (UVVector2 *)malloc(n*sizeof(UVVector2)) ;
      for(i=0; i<n; i++){
        snode->uvpoint[i]   = uvpoints[i] ;
      }

      snode->nsort_vectors = 0 ;
      snode->sort_vector  = NULL ;
      snode->ncolors      = 0 ;
      snode->color        = NULL ;

      snode->nmatrices = 1 ;
      snode->matrix    = (Matrix4x3 *)malloc(sizeof(Matrix4x3)) ;
      *(snode->matrix) = matrix ;

      snode->n_textures   = 1 ;
      snode->texture_name = (char **)malloc(sizeof(char **)) ;
      snode->texture_name[0] = strdup("AU_Ash.ace") ;        //  Default texture
      snode->texture      = (TextureNode **)malloc(sizeof(TextureNode)) ;
      snode->texture[0]   = NULL ;
      snode->n_texlevel_low = 1 ;
      snode->texlevel_low   = (TexLevelLow *)malloc(sizeof(TexLevelLow)) ;
      texlevel = snode->texlevel_low ;
      texlevel->iImage        = 0 ;
      texlevel->FilterMode    = 0 ;
      texlevel->MipMapLODBias = 0.0 ;
      texlevel->BorderColor   = 0 ;

      snode->nlight_materials = 0 ;
      snode->light_material   = NULL ;

      snode->nlight_model_cfgs = 1 ;
      snode->light_model_cfg   = (LightModelCfg *)malloc(sizeof(LightModelCfg)) ;
      snode->light_model_cfg[0].flags      = 0 ;
      snode->light_model_cfg[0].n_uv_ops   = 1 ;
UVOps *uv_ops ;
      uv_ops = (UVOps *)malloc(sizeof(UVOps)) ;
      snode->light_model_cfg[0].uv_ops = uv_ops ;
      uv_ops[0].type            = 84 ;
      uv_ops[0].tex_addr_mode   = 1 ;
      uv_ops[0].src_uv_idx      = 0 ;
      uv_ops[0].unknown_param_3 = 0.0 ;
      uv_ops[0].unknown_param_4 = 0.0 ;

      snode->n_vtx_states = 1 ;
      snode->vtx_state    = (VTXState *)malloc(sizeof(VTXState)) ;
VTXState *vtx_state = snode->vtx_state ;
      vtx_state[0].flags         = 0 ;
      vtx_state[0].imatrix       = 0 ;
      vtx_state[0].light_mat_idx = -9 ;
      vtx_state[0].light_cfg_idx = 0 ;
      vtx_state[0].light_flags   = 2 ;
      vtx_state[0].imatrix2      = 0 ;

      snode->n_prim_states = 1 ;
      snode->prim_state  = (PrimState *)malloc(sizeof(PrimState)) ;
PrimState *prim_state = snode->prim_state ;
      prim_state[0].name = NULL ;
      prim_state[0].flags           = 0 ;
      prim_state[0].ishader         = 0 ;
      prim_state[0].n_tex_idxs      = 1 ;
      prim_state[0].tex_idx = (uint *)malloc(sizeof(uint)) ;
      prim_state[0].tex_idx[0]      = 0 ;
      prim_state[0].zbias           = 0.0 ;
      prim_state[0].ivtx_state      = 0 ;
      prim_state[0].alpha_test_mode = 1 ;
      prim_state[0].light_cfgidx    = 0 ;
      prim_state[0].zbuf_mode       = 0 ;

      snode->dlevel_selection = 0 ;
      snode->nhierarchy       = 0 ;
      snode->hierarchy        = NULL ;
      snode->hierarchy_flag   = NULL ;

      snode->n_lod_controls = 1 ;
      snode->lod_control  = (LodControl *)malloc(sizeof(LodControl)) ;
LodControl *lod_control = snode->lod_control ;
      lod_control[0].dlev_bias = 0 ;
      lod_control[0].n_dist_levels = 2 ;
      lod_control[0].dist_level = (DistLevel *)malloc(2*sizeof(DistLevel)) ;
DistLevel *dist_level = lod_control[0].dist_level ;
/*
 *******************************************************************************
 *   Distance level 0
 *******************************************************************************
 */
      dist_level[0].index = 0 ;
      dist_level[0].dlevel_selection = 500.0 ;
      dist_level[0].n_hierarchy      = 1 ;
      dist_level[0].n_sub_objects    = 1 ;
      dist_level[0].gl_display_list  = 0 ;

      dist_level[0].hierarchy =(int *)malloc(2*sizeof(int)) ;
      dist_level[0].hierarchy[0] = -1 ;

      dist_level[0].sub_object =(SubObject *)malloc(sizeof(SubObject)) ;
SubObject *sub_object = dist_level[0].sub_object  ;

      sub_object[0].flags                = 1024 ;
      sub_object[0].sort_vector_idx      = -1 ;
      sub_object[0].vol_idx              = -1 ;
      sub_object[0].src_vtx_fmt_flags    = 466 ;
      sub_object[0].dst_vtx_fmt_flags    = 452 ;
      sub_object[0].face_normals         = 16 ;
      sub_object[0].tx_light_cmds        = 1 ;
      sub_object[0].nodex_tri_list_idxs  = 0 ;
      sub_object[0].tri_list_idxs        = 48 ;
      sub_object[0].linelist_idxs        = 0 ;
      sub_object[0].nodex_tri_list_idxs2 = 0 ;
      sub_object[0].tri_lists            = 1 ;
      sub_object[0].line_lists           = 0 ;
      sub_object[0].pt_lists             = 0 ;
      sub_object[0].nodex_tri_lists      = 0 ;
      sub_object[0].n_geom_nodes         = 1 ;
      sub_object[0].n_node_maps          = 1 ;
      sub_object[0].n_shaders            = 1 ;
      sub_object[0].n_light_cfgs         = 1 ;
      sub_object[0].n_vertices           = 32 ;
      sub_object[0].n_vertex_sets        = 1 ;
      sub_object[0].n_tri_lists          = 2 ;

      sub_object[0].node_map = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].node_map[0] = 0 ;

      sub_object[0].shader = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].shader[0] = 0 ;

      sub_object[0].light_cfg = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].light_cfg[0] = 0 ;
/*
 *  Sub-object vertices
 */
      n = sub_object[0].n_vertices ;
      sub_object[0].vertex = (Vertex *)malloc(n*sizeof(Vertex)) ;
      for(i=0; i<n; i++){
        sub_object[0].vertex[i] = vertex0[i] ;
        sub_object[0].vertex[i].vertex_uv = (uint *)malloc(sizeof(uint)) ;
        sub_object[0].vertex[i].vertex_uv[0] = ivertex0[i] ;
      }
/*
 *  Sub-object vertex sets
 */
      sub_object[0].vertex_set = (VertexSet *)malloc(sizeof(VertexSet)) ;
      sub_object[0].vertex_set[0].vtx_state_idx = 0 ;
      sub_object[0].vertex_set[0].start_vtx_idx = 0 ;
      sub_object[0].vertex_set[0].vtx_count     = 32 ;
/*
 *  Sub-object Trilists
 */
      n = sub_object[0].n_tri_lists ;
      sub_object[0].tri_list = (TriList *)malloc(n*sizeof(TriList)) ;
      sub_object[0].tri_list[0].prim_state_trilist = 0 ;
      sub_object[0].tri_list[0].n_vertex_idxs      = 0 ;
      sub_object[0].tri_list[0].vertex_idx         = NULL ;
      sub_object[0].tri_list[0].n_normal_idxs      = 0 ;
      sub_object[0].tri_list[0].normal_idx         = NULL ;
      sub_object[0].tri_list[0].n_flags            = 0 ;
      sub_object[0].tri_list[0].flag               = NULL ;

      sub_object[0].tri_list[1].prim_state_trilist = 0 ;
      sub_object[0].tri_list[1].n_vertex_idxs      = 16 ;
      n = sub_object[0].tri_list[1].n_vertex_idxs ;
      sub_object[0].tri_list[1].vertex_idx = (VertexIdx *)malloc(n*sizeof(VertexIdx)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[1].vertex_idx[i] = ivertex_idx0[i] ;
      }

      sub_object[0].tri_list[1].n_normal_idxs   = 16 ;
      n = sub_object[0].tri_list[1].n_normal_idxs ;
      sub_object[0].tri_list[1].normal_idx = (uint *)malloc(n*sizeof(uint)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[1].normal_idx[i] = inormal_idx0[i] ;
      }

      sub_object[0].tri_list[1].n_flags  = 16 ;
      n = sub_object[0].tri_list[1].n_flags ;
      sub_object[0].tri_list[1].flag  = (uint *)malloc(n*sizeof(uint)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[1].flag[i] = iflag0[i] ;
      }
/*
 *******************************************************************************
 *   Distance level 1
 *******************************************************************************
 */
      dist_level[1].index            = 1 ;
      dist_level[1].dlevel_selection = 2000.0 ;
      dist_level[1].n_hierarchy      = 1 ;
      dist_level[1].n_sub_objects    = 1 ;
      dist_level[1].gl_display_list  = 0 ;

      dist_level[1].hierarchy =(int *)malloc(2*sizeof(int)) ;
      dist_level[1].hierarchy[0] = -1 ;

      dist_level[1].sub_object =(SubObject *)malloc(sizeof(SubObject)) ;
      sub_object = dist_level[1].sub_object  ;

      sub_object[0].flags                = 1024 ;
      sub_object[0].sort_vector_idx      = -1 ;
      sub_object[0].vol_idx              = -1 ;
      sub_object[0].src_vtx_fmt_flags    = 466 ;
      sub_object[0].dst_vtx_fmt_flags    = 452 ;
      sub_object[0].face_normals         = 8 ;
      sub_object[0].tx_light_cmds        = 1 ;
      sub_object[0].nodex_tri_list_idxs  = 0 ;
      sub_object[0].tri_list_idxs        = 24 ;
      sub_object[0].linelist_idxs        = 0 ;
      sub_object[0].nodex_tri_list_idxs2 = 0 ;
      sub_object[0].tri_lists            = 1 ;
      sub_object[0].line_lists           = 0 ;
      sub_object[0].pt_lists             = 0 ;
      sub_object[0].nodex_tri_lists      = 0 ;
      sub_object[0].n_geom_nodes         = 1 ;
      sub_object[0].n_node_maps          = 1 ;
      sub_object[0].n_shaders            = 1 ;
      sub_object[0].n_light_cfgs         = 1 ;
      sub_object[0].n_vertices           = 16 ;
      sub_object[0].n_vertex_sets        = 1 ;
      sub_object[0].n_tri_lists          = 2 ;

      sub_object[0].node_map = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].node_map[0] = 0 ;

      sub_object[0].shader = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].shader[0] = 0 ;

      sub_object[0].light_cfg = (uint *)malloc(sizeof(uint)) ;
      sub_object[0].light_cfg[0] = 0 ;

/*
 *  Sub-object vertices
 */
      n = sub_object[0].n_vertices ;
      sub_object[0].vertex = (Vertex *)malloc(n*sizeof(Vertex)) ;
      for(i=0; i<n; i++){
        sub_object[0].vertex[i] = vertex1[i] ;
        sub_object[0].vertex[i].vertex_uv = (uint *)malloc(sizeof(uint)) ;
        sub_object[0].vertex[i].vertex_uv[0] = ivertex1[i] ;
      }
/*
 *  Sub-object vertex sets
 */
      sub_object[0].vertex_set = (VertexSet *)malloc(sizeof(VertexSet)) ;
      sub_object[0].vertex_set[0].vtx_state_idx = 0 ;
      sub_object[0].vertex_set[0].start_vtx_idx = 0 ;
      sub_object[0].vertex_set[0].vtx_count     = 16 ;

/*
 *  Sub-object Trilists
 */
      n = sub_object[0].n_tri_lists ;
      sub_object[0].tri_list = (TriList *)malloc(n*sizeof(TriList)) ;
      sub_object[0].tri_list[0].prim_state_trilist = 0 ;
      sub_object[0].tri_list[0].n_vertex_idxs      = 0 ;
      sub_object[0].tri_list[0].vertex_idx         = NULL ;
      sub_object[0].tri_list[0].n_normal_idxs      = 0 ;
      sub_object[0].tri_list[0].normal_idx         = NULL ;
      sub_object[0].tri_list[0].n_flags            = 0 ;
      sub_object[0].tri_list[0].flag               = NULL ;

      sub_object[0].tri_list[1].prim_state_trilist = 0 ;
      sub_object[0].tri_list[1].n_vertex_idxs      = 8 ;
      n = sub_object[0].tri_list[1].n_vertex_idxs ;
      sub_object[0].tri_list[1].vertex_idx = (VertexIdx *)malloc(n*sizeof(VertexIdx)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[1].vertex_idx[i] = ivertex_idx1[i] ;
      }

      sub_object[0].tri_list[1].n_normal_idxs   = 8 ;
      n = sub_object[0].tri_list[1].n_normal_idxs ;
      sub_object[0].tri_list[1].normal_idx = (uint *)malloc(n*sizeof(uint)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[1].normal_idx[i] = inormal_idx1[i] ;
      }

      sub_object[0].tri_list[1].n_flags  = 8 ;
      n = sub_object[0].tri_list[1].n_flags ;
      sub_object[0].tri_list[1].flag  = (uint *)malloc(n*sizeof(uint)) ;
      for(i=0; i<n; i++){
        sub_object[0].tri_list[1].flag[i] = iflag1[i] ;
      }

/*
 *******************************************************************************
 *   End Distance Level Processing
 *******************************************************************************
 */

      snode->n_animations = 0 ;
      snode->animation    = NULL ;

      return snode ;
}
#endif
