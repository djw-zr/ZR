/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  display_shapes.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File containing the code to display the one of the shapes
 *
 *==============================================================================
 */
int display_shape(WorldItem *witem, ShapeNode *snode, DistLevel *dist_level){

  uint        ip  = 0         ;   //  0 = no printing
  static int  ipp = 1         ;   //  1 = Print first pass only
  int         ic = 0, icc = -1 ;   //  Control
  uint        i, j, k, l, m, n, ierr ;
  uint        im, iskip       ;
  uint        a_lin, a_tcb    ;   // Flags for linear_key and tcb_key
  int         ima[10]         ;   // Stack for matrices
  SubObject   *sub_object     ;
  TexLevelLow *texlevel_low   ;
  TextureNode *texture        ;
  Vertex      *vtx_list       ;
  TriList     *tri_list       ;
  PrimState   *prim_state     ;
  VTXState    *vtx_state      ;
  Matrix4x3   *matrix4x3      ;
  SignalObj   *signal_obj     ;
  int         gl_display_list ;
  int         gl_tex_ref_no   ;
  int         prim_state_idx  ;
  int         tex_idx         ;
  int         ivtx_state      ;
  char        *name           ;
  static int  ierror = 0      ;

  Animation *animation     ;
  AnimNode  *anim_node     ;
  AnimController *c0, *c1, *cc ;
  AnimKey        *a0       ;
  uint      n_anim_nodes   ;
  uint      n_controllers  ;
  uint      n_anim_keys    ;
  uint       i0, i1        ;
  double    wa, dw, w0, w1 ;
  float     m16[16]        ;  // Storage for rotation matrix

  char     my_name[] = "display_shape" ;
/*
 *==============================================================================
 */
      if(0 == snode->n_lod_controls){
        printf("  No lod_controls for %s\n",snode->name) ;
        printf("  Display list routine exiting ... \n") ;
        return 0 ;
      }

//         ip = witem->world_node->tile_x == 1455 &&
//              witem->world_node->tile_y == 10329 &&
//              strcmp(snode->name,"Forest") == 0 ;

//       ip = !strcmp(snode->name,"ashphaltplat20m500r") ;
//       ip = !strcmp(snode->name,"NSW_SemLQHome") ;
//       ip = !strcmp(snode->name,"A1tPnt6dLft")
//          || !strcmp(snode->name,"UKCrossGate_fix");
//     ip = !strcmp(snode->name,"A1tPnt6dLft");
//       ip = !strcmp(snode->name,"UKCrossGate_fix");
//       ip = !strcmp(snode->name,"JP1CarCrossing");
//     ipp = !strcmp(snode->name,"UKCrossGate_fix");
//     ip = !strcmp(snode->name,"Vduct_1Arch-1Track");
//     ip = !strcmp(snode->name,"AU_Cedar_20m");
//      ip = !strcmp(snode->name,"AU_Ash_20m");
//      ip = !strcmp(snode->name,"underground_marker");
//      ip = !strcmp(snode->name,"1905-KL");
//     ip = !strcmp(snode->name,"BBS-HomesteadC");
//      if(strcmp(snode->name,name_control)== 0){      //  Allow control over items plotted
//       printf(" New dlist for %s\n",snode->name) ;
//      }
//      ip = l_time_1s && witem && (340 == witem->uid) ;
//      ip = !strcmp_ic(snode->name,"Dock");
//      ip = !strcmp_ic(snode->name,"lymtownstabldg");
//      ip = !strcmp_ic(snode->name,"ctn_point_indication") ;
/*
 *  Triple semaphore at entrance to Lymington Harbour, New Forest Route
 */
//      ip =  l_time_5s && 1930 == witem->uid
//         &&   -6141 == witem->world_node->tile_x && 14888 == witem->world_node->tile_y ;
/*
 *  Three light signal at York 440  MECoast route
 */
//      ip =  l_time_5s && 590 == witem->uid
//         &&   -6073 == witem->world_node->tile_x && 15048 == witem->world_node->tile_y ;
/*
 *  Platform signal at Mount Victoria - Zig-Zag Route
 */
//      ip =  410 == witem->uid
//                &&  1454 == witem->world_node->tile_x
//                && 10325 == witem->world_node->tile_y ;
/*
 *  Approach signal at Mount Victoria - Zig-Zag Route
 */
//           ip =  (414 == witem->uid || 415 == witem->uid)
//                 &&  1454 == witem->world_node->tile_x
//                 && 10325 == witem->world_node->tile_y ;
/*
 *  Signal outside Philidelphia :  Route USA1
 */
//           ip =  4353 == witem->uid
//                 && -11009 == witem->world_node->tile_x
//                 &&  14317 == witem->world_node->tile_y ;
/*
 *  Points at Lymington pier :  New Forest Route
 */
//           ip =  202 == witem->uid
//           ip =  180 == witem->uid
//                 && -6141 == witem->world_node->tile_x
//                 && 14888 == witem->world_node->tile_y ;
/*
 *  Turntable at Oxford :  New Forest Route
 *           Number of animation nodes < number of matrices
 */
//      ip = !strcmp(snode->name,"ukfs_tt_42ft");
//      ip = !strcmp(snode->name,"Forest_Tree");


      if(!ipp)ip = 0 ;                        // Print on first pass only.
      if(ip) ipp = 0 ;
//      ip = l_pd ;

#ifdef _Display_Normal
      if((ip && witem && (witem->anim_value != 0.0) )){
        printf("\n\n*******************************************************************\n");
        printf("  Routine %s entered for shape %s\n",
                  my_name,snode->name) ;
        printf("    World Item  uid = %i, worldtype = %i, filename = %s, anim_value = %f\n",
                  witem->uid,witem->worldtype,
                  witem->filename,witem->anim_value) ;
        printf(" ip = %i, ipp = %i\n",ip,ipp) ;
      }

      if(ip){
        printf("\n\n =================================================\n") ;
        printf(" Generate display for : %s\n",snode->name) ;
        printf(" World Item  uid = %i, worldtype = %i %s, filename = %s, anim_value = %f\n",
                  witem->uid,witem->worldtype,token_idc[witem->worldtype],
                  witem->filename,witem->anim_value) ;
        printf(" Shape name = %s\n",snode->name) ;
        printf(" Distance level = %i\n",dist_level->index) ;
      }
#endif
      if(0 == strcmp(snode->name,"underground_marker")) return 0;
//      if(0 == strcmp(snode->name,"ctn_point_indication")) return 0;
      if(0 == strncmp(snode->name,"marker",6)) return 0;

/*
 *==============================================================================
 *  Print section
 *
 *  1.  Each state has a series of primitive states.  These refer to the textures
 *  requied plus various flags.  Here key items are printed out.
 *==============================================================================
 */
      if(ip){
        printf(" List Key Variables:\n") ;
        n = snode->n_prim_states ;
        printf("  Primitive states:  nprim_states = %i\n",n);
        for(i=0;i<n;i++){
          prim_state = &snode->prim_state[i] ;
          printf("  prim_state : %2i : pointer = %p",i,(void *)prim_state) ;
          name = prim_state->name ;
          printf("  name   = %p    %-24s",(void *)name,name);
          printf("  n_tex_idxs = %i ",prim_state->n_tex_idxs);
          for(j=0;j<prim_state->n_tex_idxs;j++){
            printf(":: %i", prim_state->tex_idx[j]);
          }
          printf("\n");
        }
/*
 *==============================================================================
 *  2.  Each shape structure  has a "TexLevelLow" array:
 *  The value of tex_idx above is the index into this array, listed here:
 *==============================================================================
 */
        n = snode->n_texlevel_low ;
        printf("  TexLevelLow objects:     n_texlevel_low = %i\n",n);
        for(i=0;i<n;i++){
          texlevel_low = &snode->texlevel_low[i] ;
          printf("  : %2.2i : pointer = %p ",i,(void *)texlevel_low);
          printf("  : iImage = %2.2i :: %i :: %x :: %f\n",
                 texlevel_low->iImage, texlevel_low->FilterMode,
                 texlevel_low->BorderColor, texlevel_low->MipMapLODBias);
        }
/*
 *==============================================================================
 *  3. The value of iImage above is the index into the shape's array of textures
 *  listed here:
 *==============================================================================
 */
        n = snode->n_textures ;
        printf("  Textures:     n_textures = %i\n",n);
        for(i=0;i<n;i++){
          texture = snode->texture[i] ;
          name    = snode->texture_name[i] ;
          printf("    Texture    : %2.2i : pointer = %p  %p    %s\n",
                                       i,(void *)texture,(void *)name,name);
        }
        printf(" End of List Key Variables\n\n") ;
      }
/*
 *==============================================================================
 *  Check for animation
 *==============================================================================
 */
      if(ip){
        printf("  Check for animation\n") ;
        printf("  snode         = %p\n", (void *)snode) ;
        printf("  dist_level    = %p\n", (void *)dist_level) ;
        printf("  n_sub_objects = %i\n", dist_level->n_sub_objects) ;
        n = snode->n_animations ;
        printf("  n_animations  = %i\n", n) ;
        for(i=0;i<n;i++){
          animation = &(snode->animation[i]) ;
          if(animation){
            n_anim_nodes = animation->n_anim_nodes ;
          }else{
          n_anim_nodes = 0 ;
          }
          printf("    animation    %i = %p\n", i, (void *)animation) ;
          printf("    n_anim_nodes %i = %i\n", i, n_anim_nodes) ;
        }
        printf("\n") ;
      }
      animation = &(snode->animation[0]) ;
      if(animation){
        n_anim_nodes = animation->n_anim_nodes ;
      }else{
        n_anim_nodes = 0 ;
      }

/*
 *==============================================================================
 *   Each shape has a series of 'lod_controls', (usually just 1?)
 *     each of which has a series of distance levels  (usually 1, 2 or 3)
 *       each distance level has a range (metres) and a number of sub-objects
 *          each sub-object then has a series of triangle lists
 *            each triangle list has a primary state
 *              each primary state has pointers to textures, transformation
 *                matrices, animations, and a list of the triangles to plot
 *
 *   In the current version of this routine the decision on which lod_control
 *   and dist_level to use is left to the calling routine.  The loops here
 *   thus start with the sub-objects.  Once all sub-objects are processed
 *   the routine exits.
 *==============================================================================
 */
      for(k=0;k<dist_level->n_sub_objects;k++){
        if(ip){
          printf("\n") ;
          printf("  SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n");
          printf("  Start of loop for sub-object %i\n\n",k) ;
          printf("  Number of sub-objects =      %i\n",dist_level->n_sub_objects) ;
        }
        sub_object = &(dist_level->sub_object[k]) ;
        vtx_list   = sub_object->vertex         ;
        if(ip)printf("    Sub_object:  k = %2i,  sub_object = %p,  vtx_list = %p,"
                      "  n_tri_lists = %i\n",
                k,(void *)sub_object, (void *)vtx_list,sub_object->n_tri_lists) ;
/*
 *==============================================================================
 *  Loop over tri(angle)-lists
 *==============================================================================
 */
        for(l=0;l<sub_object->n_tri_lists;l++){
          tri_list = &(sub_object->tri_list[l]) ;
#if 0
              icc = icc+1 ;
              if(ic && icc>=n_control) break ;  //  n_control (zr.h) limits printing
              if(ic){
                printf("********************************************************************\n");
                printf("  icc = %i,  i_control1 = %i, i_control2 = %i\n",
                                              icc,i_control1[icc],i_control2[icc]) ;
              }
//              if(ic && i_control1[icc]==0)continue ;   // Only plot when == 1
#endif
          if(ip){
            printf("   ===================================================\n") ;
            printf("    Tri_list:  l = %i,  tri_list = %p,  n_vertex_idxs = %i\n",
                    l, (void *)tri_list, tri_list->n_vertex_idxs) ;
            printf("               ip = %i, ic = %i, icc = %i\n",ip,ic,icc) ;
          }
          if(0 == tri_list->n_vertex_idxs) continue ;
/*
 *==============================================================================
 *  Each tri(angle)_list has a prim(ary)_state
 *==============================================================================
 */
          prim_state_idx = tri_list->prim_state_trilist     ;
          prim_state = &(snode->prim_state[prim_state_idx]) ;
//          if(!strcmp(prim_state->name,"TRANS"))continue ;
//          if(340 == witem->uid && 3 != prim_state_idx)continue ;
/*
 *==============================================================================
 *  Each prim state has a texture if n_tex_idxs > 0
 *==============================================================================
 */
          texture = NULL ;
          n_controllers = 0 ;
          if(0 != prim_state->n_tex_idxs){
            tex_idx = prim_state->tex_idx[0]       ;
            texlevel_low = &snode->texlevel_low[tex_idx] ;
            texture      = snode->texture[texlevel_low->iImage]  ;

            gl_tex_ref_no = (texture == NULL ? 0 : texture->gl_tex_ref_no) ;
            if(ip){
              printf("     PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP\n") ;
              printf("     Prim_state_idx = %2.2i, prim_state = %p, prim_state->name = %-25s, tex_idx = %2.2i, texture = %p,  gl_tex_ref_no = %i\n",
                                prim_state_idx, (void *)prim_state, prim_state->name,tex_idx,
                                (void *)texture,gl_tex_ref_no) ;
            }

            if(texture == NULL && ierror++<20){
//  Used to debug but causes continuous errors with MSTS Route Japan 2.
              printf(" **********************%s*********************************\n",
                     my_name);
              printf("  Routine: %s.  Image missing !\n",my_name);
              printf("  Shape = %p,  Name = %s   Image = %i, ierror = %i\n",
                     (void *)snode, snode->name, tex_idx,ierror);
              printf("  texture_name pointer = %p, texture_pointer = %p\n",
                        (void *)snode->texture_name, (void *)snode->texture) ;
uint i ;
              for(i=0; i<snode->n_textures; i++){
                printf("  texture %i / %i, name = %s, texture node = %p\n",
                       i, snode->n_textures, snode->texture_name[i],
                                    (void *) snode->texture[i]) ;
              }
              if(ierror<3)print_shape_file_data(snode) ;
              printf(" ******************************************************************\n");
              break ;
            }else{
              if(GL_TRUE != glIsTexture( texture->gl_tex_ref_no) && ierror++<20){
              printf(" **********************shape_dlist*********************************\n");
              printf("  Routine: %s.  Image unloaded !\n",my_name);
              printf("  Shape = %s   Image = %i, ierror = %i\n",
                             snode->name, tex_idx, ierror);
              printf("  texture_name pointer = %p, texture_pointer = %p\n",
                        (void *)snode->texture_name, (void *)snode->texture) ;
uint i ;
              for(i=0; i<snode->n_textures; i++){
                printf("  texture %i / %i, name = %s, texture node = %p\n",
                       i, snode->n_textures, snode->texture_name[i],
                                    (void *) snode->texture[i]) ;
              }
              if(ierror<3)print_shape_file_data(snode) ;
              printf("  ******************************************************************\n");
              }
            }
          }else{
            gl_tex_ref_no = 0 ;
          }
/*
 *==============================================================================
 *  Each prim state also has one or more matrices which define the final position
 *  of the sub-object.
 *
 *  A wagon may contain four sub-objects describing the wheels.  In principal
 *  these may all use the same set of vertices but use different matrices to
 *  move the wheel to a bogie and then move the bogie into position.  In practice
 *  however they may often use different set of vertices.
 *
 *  The index of the first matrix (if any) is the one pointed to by
 *  "vtx_state->imatrix", the next, if any is given by the
 *  "dist_level->hierarchy" array.
 *
 *  Each matrix also has a corresponding animation node.  For fixed parts
 *  these do nothing but for moving parts they include a translation and/or
 *  a rotation.  Three flags are used, with enum values LINEAR_KEY, TCB_ROT
 *  and SLERP_ROT.  In the first two cases the corresponding animation is used
 *  instead of the translation or rotation component of the main matrix.
 *  In the last case the animation is applied in addition to any rotation due to
 *  the main matix.
 *
 *  Note: The last element of any heirarchy chain has the value -1.  So
 *  the ima array has to be an 'integer' (int) not an 'unsigned integer' (uint)
 *==============================================================================
 */
          if(ip){
            printf("     Prim_state->ivtx_state = %i\n",prim_state->ivtx_state) ;
            printf("                  vtx_state = %p\n",
                          (void *)&(snode->vtx_state[prim_state->ivtx_state]) );
          }
          ivtx_state = prim_state->ivtx_state       ;
          vtx_state  = &(snode->vtx_state[ivtx_state]) ;
/*
 *  Initialise array "ima" for this sub-object
 *  If shape is a signal - skip if this sub-object is not being used
 */
          im = 0 ;
          for(i=0;i<10;i++)ima[i] = 0 ;

          ima[0] = vtx_state->imatrix ;
          if(witem && SIGNAL_ALT == witem->worldtype &&
                                    witem->u.signal_obj.sm_skip[ima[0]])continue ;

          if(ip){
            if(ima[im]>=0){
              matrix4x3 = &(snode->matrix[ima[im]])  ;
              printf("     im = %i, ima[im] = %i,  name = %s\n", im, ima[im], matrix4x3->name) ;
            }else{
              printf("     im = %i, ima[im] = %i\n", im, ima[im]) ;
            }
          }
/*
 *   Loop over Matrices
 */
          if(ip)printf("     Hierarchy List\n") ;
          while(ima[im] >= 0){
            if(ip)printf("  Hierarchy loop. AA  im = %i, ima[im] = %i\n",im,ima[im]) ;
            if(im>=10 || ima[im]>(int)snode->nmatrices){
              printf(" Routine %s, matrix out of bounds for shape %s\n", my_name,snode->name);
              printf("   ip = %i, k = %i, l = %i\n",ip,k,l) ;
              printf("   im = %i,  iam[im] = %i\n",im,ima[im]);
              fflush(NULL) ;
              printf("   name = %s\n",snode->matrix[ima[im]].name);
              abort();
            }
            im++ ;
            ima[im] = dist_level->hierarchy[ima[im-1]] ;
            if(ip)printf("  Hierarchy loop. BB  im = %i, ima[im] = %i\n",im,ima[im]) ;
            if(ip)printf("    dist_level->index = %i nhierarchy = %i\n",
                          dist_level->index,dist_level->n_hierarchy) ;
            ierr = 0 ;

            if(ima[im] > 100){
              if(1) return 1 ;
uint ii ;
              printf("  Routine %s error\n",my_name) ;
              printf("    Hierarchy value greater than size of hierarchy array\n");
              printf("    Distance level = %p : %i\n",(void *)dist_level, dist_level->index) ;
              for(ii = 0; ii<im+1; ii++){
                printf("      im = %i, ima[im] = %i\n", ii, ima[ii]) ;
              }
              ierr = 1;
              return 1 ;
            }

            if(ip){
              if(ima[im]>=0){
                matrix4x3 = &(snode->matrix[ima[im]])  ;
                printf("      im = %i, ima[im] = %i,  matrix4x3 = %p\n", im, ima[im], (void *)matrix4x3) ;
                fflush(NULL) ;
                printf("      im = %i, ima[im] = %i,  name = %s\n", im, ima[im], matrix4x3->name) ;
              }else{
                printf("      im = %i, ima[im] = %i\n", im, ima[im]) ;
              }
            }
          }
          if(ierr) continue ;
          glMatrixMode(GL_MODELVIEW) ;
          glPushMatrix() ;
/*
 *  Cycle backwards through array 'ima' to apply hierarchy and/or animation matrices
 *
 *  For efficiency each matrix is flagged as being one of four matrix types
 *     0 = Unit diagonal, no translation
 *     1 = Translation only
 *     2 = Rotation and/or scaling, no translation
 *     3 = Rotation and/or scaling, translation
 */
          if(ip)printf("       Process hierarchy :: im = %i\n",im) ;
//          im-- ;
          while(im--){
            if(ip)printf("       New hierarchy :: im      = %i\n",im) ;
            if(ip)printf("                     :: ima[im] = %i\n",ima[im]) ;
            if(ip)printf("                     :: hierarchy = %i\n",dist_level->n_hierarchy) ;
            matrix4x3   = &(snode->matrix[ima[im]])      ;
            a_lin = a_tcb = 0 ;                  // use matrix flag
            if(ip)printf("       animation    = %p\n",(void *)animation) ;
            if(ip && animation)
                  printf("       n_anim_nodes = %i\n",animation->n_anim_nodes) ;
/*
 *  Check that the shape is animated and primitive state is not "TRANS"
 *  Note program fails if check on prim_state->name is not done
 */
            if(animation && (uint) ima[im] < dist_level->n_hierarchy
                         && (uint) ima[im] < animation->n_anim_nodes
                         && (!prim_state->name || (prim_state->name &&
                            strcmp(prim_state->name,"TRANS") ) ) ){
              if(ip)printf("       Setup anim_node\n") ;
              anim_node = &(animation->anim_node[ima[im]])  ;
              if(ip)printf("       anim_node = %p\n",(void *)anim_node) ;
              n_controllers = anim_node->n_controllers      ;
              if(ip)printf("  CHECK CONTROLLERS  :: n_controllers = %i/n",n_controllers) ;
              for(i=0;i<n_controllers;i++){
                n_anim_keys = anim_node->controller[i].n_anim_keys ;
                for(j=0; j< n_anim_keys; j++){
                  m = anim_node->controller[i].anim_key[j].type ;
                  if(m == TCB_KEY)   a_tcb = 1 ;
                  if(m == SLERP_ROT) a_tcb = 1 ;
                  if(m == LINEAR_KEY)a_lin = 1 ;
                  if(ip)printf("    controller = %i, anim_ley = %i, type = %i %s,"
                               "  a_lin = %i, a_tcb = %i\n",
                               i, j, m, token_idc[m], a_lin, a_tcb) ;
                }
              }
            }else{
              anim_node = NULL  ;
              n_controllers = 0 ;
              if(ip)printf("            n_controllers = %i\n",n_controllers) ;
            }if(ip)
            if(ip){
              printf("       ---------------------------------------------------------\n") ;
              printf("       Hierarchy  im = %i, ima[im] = %i,  Name = %s\n",
                        im,ima[im],matrix4x3->name) ;
#if 1
              printf("         shape = %s, sub_object = %i, tri_list = %i\n"
                       "                prim_state = %i, prim_state->name = %s,\n"
                       "                vertex_state = %i, matrix_type = %i, %s\n",
                          snode->name, k, l, prim_state_idx, prim_state->name,
                          ivtx_state, matrix4x3->type, token_mat_type[matrix4x3->type]) ;
#else
              printf("         shape = %s, sub_object = %i, tri_list = %i\n",
                          snode->name, k, l ) ;
              printf("                prim_state = %i, prim_state->name = %s,\n",
                                      prim_state_idx, prim_state->name) ;
              printf("                vertex_state = %i,\n", ivtx_state) ;
              printf("                hierarchy[vs] = %i\n",
                         dist_level->hierarchy[ivtx_state]) ;
              printf("                matrix_type   = %i\n", matrix4x3->type) ;
#endif
                printf("         Matrix    name = %s\n",matrix4x3->name) ;
                printf("         PrimState name = %s\n",prim_state->name) ;
                printf("         Animation  n_controllers = %i\n",n_controllers) ;
                printf("         a_lin = %i,  a_tcb = %i\n", a_lin, a_tcb) ;
            }
/*
 *   First apply main matrices
 */
            if(ima[im]>0 && matrix4x3->type){
/*
 *  Use Translation only if a_lin is zero (no linear_key animation)
 */
              if(matrix4x3->type == MAT_TRANSLATE && !a_lin){
//                if(strcmp_ic(snode->name,"Dock"))
                glTranslatef((GLfloat) matrix4x3->DX,
                             (GLfloat) matrix4x3->DY,
                             (GLfloat) matrix4x3->DZ) ;
                if(ip)printf("        Matrix Translate :: %f %f %f\n",matrix4x3->DX, matrix4x3->DY, matrix4x3->DZ) ;
/*
 *  Use Rotation only if a_tcb is zero (no tcb_key animation)
 */
              }else if(matrix4x3->type == MAT_ROTATE && !a_tcb){
                msts4x3_to_opengl4x4(m16,matrix4x3) ;
                glMultMatrixf(m16) ;
                if(ip)printf("        Matrix Rotate\n") ;
/*
 *  Matrix with rotation and translation
 */
              }else if(!(a_lin || a_tcb )){           //  No special action needed
                msts4x3_to_opengl4x4(m16,matrix4x3) ;
                glMultMatrixf(m16) ;
                if(ip)printf("        General matrix operation\n") ;
              }else if(!(a_lin && a_tcb )){
                msts4x3_to_opengl4x4(m16,matrix4x3) ;
                if(a_lin){                            //  Rotate only
                  for(i=12; i<15;i++) m16[i] = 0.0 ;
                  glMultMatrixf(m16) ;
                }else{                                // Translate only
                  glTranslatef((GLfloat) m16[12], (GLfloat) m16[13], (GLfloat) m16[14]) ;
                }
                if(ip)printf("        Modified general matrix operation\n") ;
              }
          }
/*
 *  DEBUG
 */
          if(ip && witem && witem->worldtype == SIGNAL_ALT
//            && (witem->uid == 415 )
            && (!strncmp_ic(witem->snode->matrix[ima[0]].name,"Head",4) ) ) {
            printf("  World item uid  = %i\n",witem->uid) ;
            printf("  n_controllers   = %i\n",n_controllers) ;
            n = witem->u.signal_obj.n_matrices ;
            printf("  witem uid  = %i\n",witem->uid) ;
            printf("  shape      = %s\n",witem->snode->name) ;
            printf("  n_matrices = %i\n",n) ;
            printf("  ima[0]     = %i\n",ima[0]) ;
            printf("  signal     = %p\n",(void *)witem->u.signal_obj.signal) ;
            for(m=0;m<n;m++){
              printf("  matrix %i, signal[i] = %p\n",
                        m,(void *)witem->u.signal_obj.signal[m]) ;
            }
            for(m=0;m<n;m++){
              printf("  matrix %i, animation[m] = %p\n",
                        m,(void *)witem->animations[m]) ;
            }
          }
/*
 *  Then animate controllers
 */
          if(witem && n_controllers){
            if(ip)printf("  Animation : ivtx_state = %i, im = %i, Matrix = %i \n", ivtx_state, im, ima[im]);

// Check number of matrices match
#if 0
            if(n_anim_nodes != dist_level->n_hierarchy){
              printf("  Routine %s.  Mismatch of n_hierarchy and animation nodes\n",
                                      my_name) ;
              printf("  Shape = %s\n",snode->name) ;
              printf("  n_hierarchy = %i\n",dist_level->n_hierarchy) ;
              printf("  n_anim_nodes = %i\n",n_anim_nodes) ;

              wa = sqrt(-1.0) ;
              exit(1) ;
            }
#endif
//  Check for error !!
            if(n_controllers>2){
              printf("  Routine %s.  Error 'n_controllers' too large\n", my_name) ;
              printf("  Shape = %s\n",snode->name) ;
              printf("  n_controllers = %i\n",n_controllers) ;
              exit(1) ;
            }
/*
 *  Order pairs of controllers so that the transforation matrix
 *                 is applied before before the rotation matrix.
 *
 *  Note:  This handles 'linear_key' and 'tcb_key' but not 'slerp_rot'.
 *         See OpenRails file "Shapes.cs" - near line 347
 */
            c0 = c1 = NULL ;
            if(n_controllers == 1){
              c0 = &(anim_node->controller[0]) ;
            }else if(n_controllers == 2){
              c0 = &(anim_node->controller[0]) ;
              c1 = &(anim_node->controller[1]) ;
              if((c0->type == TCB_ROT || c0->type == SLERP_ROT) && c1->type == LINEAR_POS){
                c0 = &(anim_node->controller[1]) ;
                c1 = &(anim_node->controller[0]) ;
              }else if(!((c0->type == TCB_ROT || c0->type == SLERP_ROT) && c1->type == LINEAR_POS)){
                n_controllers = 0 ;
              }
            }
/*
 *  Loop over controllers
 *  With signals, the animation variable is that of the ancestor matrix with name HEAD*
 */
            for(i=0;i<n_controllers;i++){
              if(i==0){cc = c0 ;}
              else    {cc = c1 ;}
              a0 = cc->anim_key ;
              n_anim_keys = cc->n_anim_keys    ;

              if(witem){
                if(ip) printf("  n_animations = %i, animations = %p\n",
                           witem->n_animations, (void *)witem->animations   );
                if(witem->n_animations && witem->animations){
                  if(ip){
                    printf(" Animations:  im = %i, ima[im] = %i, ima[im+1] = %i\n",
                              im, ima[im], ima[im+1]) ;
                    for(j=0;j<(uint)witem->n_animations;j++){
                      printf(" %p ",(void *)witem->animations[j]) ;
                    }
                    printf("\n") ;
                  }
                  if(witem->animations[ima[im]]){
                    wa = *witem->animations[ima[im]] ;
                    if(ip)printf("  AAZ  Using animations array im :: wa = %f\n",wa) ;
                  }else if(ima[im+1] >= 0 && ima[im]< witem->n_animations
                                       && witem->animations[ima[im+1]]){
                    wa = *witem->animations[ima[im+1]] ;
                    if(ip)printf("  AAZ  Using animations array im+1 :: wa = %f\n",wa) ;
                  }else{
                    wa = witem->anim_value  ;
                  if(ip)printf("  AAZ A Using anim value = %f\n",wa) ;
                  }                                      ;
                }else{
                  wa = witem->anim_value  ;
                  if(ip)printf("  AAZ B Using anim value = %f\n",wa) ;
                }
                if(ip){
                  printf("  Anim: shape = %s, im = %i, n_amin = %i, %p, im = %i,"
                         " ima[im] = %i, wa = %f\n",
                         snode->name, im, witem->n_animations,
                         (void *) witem->animations, im, ima[im], wa) ;
                }
                wa = wa*(n_anim_keys-1) ;
                dw = 1.0/(n_anim_keys - 1) ;
                if(ip){
                  printf("  Animation : Controller %i\n",i) ;
                  printf("              Shape  = %s\n", snode->name) ;
                  printf("              World Item  uid = %i, worldtype = %i,"
                         " filename = %s, anim_value = %f\n",
                         witem->uid,witem->worldtype, witem->filename,witem->anim_value) ;
                  printf("              anim_value = %f, wa = %f, n_anim_keys = %i,"
                         "  dw = %f\n", witem->anim_value,wa,n_anim_keys,dw) ;
                  printf("              ip = %i, ipp = %i\n",ip,ipp) ;
                }
                i0 = (wa>=0) ? wa : 0 ;
                if(i0>(n_anim_keys-2))i0 = n_anim_keys-2;
                i1 = i0 + 1 ;
                w1 = wa - i0 ;
                w0 = 1.0 - w1 ;
              }else{
                w0 = w1 = wa = dw = 0.0 ;
                i0 = i1 = 0 ;
              }

              if(ip)printf("   i0 = %i, i1 = %i, w0 = %f, w1 = %f\n",i0,i1,w0,w1) ;

#if 1
              if(cc->type == LINEAR_POS){
double x, y, z ;
                x = w0*a0[i0].X + w1*a0[i1].X ;
                y = w0*a0[i0].Y + w1*a0[i1].Y ;
                z = w0*a0[i0].Z + w1*a0[i1].Z ;
                glTranslatef((GLfloat) x,(GLfloat) y,(GLfloat) z) ;
                if(ip || 0){
                  printf("      Animate LINEAR_POS\n") ;
                  printf("      Animate 0: dw, wa : %f,  %f\n", dw, wa) ;
                  printf("      Animate 0: Weight    : %f,  %f\n",w0,w1) ;
                  printf("      Animate 0: Translate : %f,  %f,  %f\n",
                                                          a0[i0].X,a0[i0].Y,a0[i0].Z) ;
                  printf("      Animate 0: Translate : %f,  %f,  %f\n",
                                                          a0[i1].X,a0[i1].Y,a0[i1].Z) ;
                  printf("      Animate  : Translate : %f,  %f,  %f\n",x,y,z) ;
                }
              }else if(cc->type == TCB_ROT || cc->type == SLERP_ROT){
double x=0.0, y=0.0, z=0.0, s, a, b  ;
double qx, qy, qz, qw ;
                if((a0[i0].X*a0[i1].X + a0[i0].Y*a0[i1].Y + a0[i0].Z*a0[i1].Z + a0[i0].W*a0[i1].W) > 0.0){
                  if(ip)printf("      Animate %s  1\n", token_idc[cc->type]) ;
                  qx = w0*a0[i0].X + w1*a0[i1].X ;
                  qy = w0*a0[i0].Y + w1*a0[i1].Y ;
                  qz = w0*a0[i0].Z + w1*a0[i1].Z ;
                  qw = w0*a0[i0].W + w1*a0[i1].W ;
                }else{
                  if(ip)printf("      Animate %s  2\n", token_idc[cc->type]) ;
                  qx = w0*a0[i0].X - w1*a0[i1].X ;
                  qy = w0*a0[i0].Y - w1*a0[i1].Y ;
                  qz = w0*a0[i0].Z - w1*a0[i1].Z ;
                  qw = w0*a0[i0].W - w1*a0[i1].W ;
                }
                b = 1.0/sqrt(qx*qx + qy*qy + qz*qz + qw*qw) ;
                qx = qx * b ;
                qy = qy * b ;
                qz = qz * b ;
                qw = qw * b ;
                a  = acos(qw) ;
// Skip for very small rotations
                if(fabs(a)>0.001){
                  if(ip)printf("      Animate Rotation\n") ;
                  s = 1.0/sin(a) ;
                  x = qx*s ;
                  y = qy*s ;
                  z = qz*s ;
                  a = -2.0*a*degree ;
                  glRotatef((GLfloat) a, (GLfloat) x,(GLfloat) y,(GLfloat) z) ;
                }
                if(ip || 0){
                  printf("      Animate 1: dw, wa :  %f,  %f\n",dw, wa) ;
                  printf("      Animate 1: Weight    : %f,  %f,  %f\n",w0,w1, w0+w1) ;
                  printf("      Animate 1: Rotate 0  : %f,  %f,  %f,  %f :: %f\n",
                                        a0[i0].X, a0[i0].Y, a0[i0].Z, a0[i0].W,
                                        a0[i0].X*a0[i0].X + a0[i0].Y*a0[i0].Y +
                                        a0[i0].Z*a0[i0].Z + a0[i0].W*a0[i0].W) ;
                  printf("      Animate 1: Rotate 1  : %f,  %f,  %f,  %f :: %f\n",
                                        a0[i1].X, a0[i1].Y, a0[i1].Z, a0[i1].W,
                                        a0[i1].X*a0[i1].X + a0[i1].Y*a0[i1].Y +
                                        a0[i1].Z*a0[i1].Z + a0[i1].W*a0[i1].W) ;
                  printf("      Animate 1: Rotate    : %f,  %f,  %f,  %f :: %f\n",
                                                      qx, qy, qz, qw,
                                                      qx*qx + qy*qy + qz*qz + qw*qw) ;
                  printf("      Animate 1: Rotate     : %f : %f,  %f,  %f\n",
                                                      a, x, y, z) ;
                }
              }           //  End logic over type of Controller
#endif
            }             //  End of loop over Controllers
          }               //  End of logic - No animations/Animations
        }                 //  End of loop over Hierarchies


/*
 *  1.  Check if the tri_list uses textures
 *      - The primitive state should reference one or more textures
 *      - The trilists should have n_vertexuvs > 0 .
 *  2.  If so  : use textures
 *  3.  If not : switch textures off and plot with colours.
 *
 *  In both cases: Loop over triangles in tri_list and plot.
 */
          {      //  LOCAL BLOCK START
  int  va[3]   ;
  int  ipoint, inormal, vertex_uv   ;
  GLfloat x, y, z, nx, ny, nz, u, v ;
  float         cof[4] ;
  unsigned char col[4] ;
  unsigned int  col1   ;

          if((0 != prim_state->n_tex_idxs) && (0 != vtx_list[tri_list->vertex_idx[0].a].n_vertex_uvs) ){
/*
 *  Plot with textures
 */
            glBindTexture(GL_TEXTURE_2D, gl_tex_ref_no) ;
            glBegin(GL_TRIANGLES) ;
            for(m=0;m<tri_list->n_vertex_idxs;m++){
              va[0] =    tri_list->vertex_idx[m].a ;     //  Index in
              va[1] =    tri_list->vertex_idx[m].b ;
              va[2] =    tri_list->vertex_idx[m].c ;
//  Loop over nodes of triangle
              for(n=0;n<3;n++){
                ipoint    = vtx_list[va[n]].ipoint    ;
                inormal   = vtx_list[va[n]].inormal   ;
                vertex_uv = vtx_list[va[n]].vertex_uv[0] ;  // There may be more than one texture
                x  = snode->point[ipoint].X ;
                y  = snode->point[ipoint].Y ;
                z  = snode->point[ipoint].Z ;
                nx = snode->normal[inormal].X ;
                ny = snode->normal[inormal].Y ;
                nz = snode->normal[inormal].Z ;
                u  = snode->uvpoint[vertex_uv].U ;
                v  = snode->uvpoint[vertex_uv].V ;
                if(0 && ip)printf("  display_wshape : %2i %2i %2i %2i :: %9f %9f %9f ::"
                        "%9f %9f %9f :: %9f %9f\n",
                                                    k,l,m,n, x,y,z, nx,ny,nz, u,v) ;
                glTexCoord2f(u, v) ;
                glNormal3f(nx, ny, nz) ;
                glVertex3f(x, y, z) ;
              }              //  End of lop over triangle
            }                //  End of loop over triangles in tri-list
            glEnd() ;
          }else{
            glDisable(GL_TEXTURE_2D) ;
            glShadeModel(GL_FLAT)    ;
            col1 = vtx_list[tri_list->vertex_idx[0].a].color1 ;
//
//  This is a bit of a hack.  1.  RGB may be the wrong way around.
//                            2.  It would be best to use color1 directly.
//
            col[2] = col1 & 0xff ;           //  Blue ??
            col[1] = (col1 >>  8) & 0xff ;   //  Green
            col[0] = (col1 >> 16) & 0xff ;   //  Red ??
//            col[3] = ((col1 >> 24) & 0xff) ;  // Alpha channel
            glColor3ubv(col)     ;
#if 0
            printf(" GL_CURRENT_COLOR (RGB)  = %x  %x  %x \n", col[0], col[1], col[2]) ;
            glGetFloatv(GL_CURRENT_COLOR,cof) ;    //   In order R, G, B, A
            printf(" GL_CURRENT_COLOR (RGBA) = %f  %f  %f  %f\n",(double) cof[0],
                                (double) cof[1],(double) cof[2],(double) cof[3]) ;
#endif
            glBegin(GL_TRIANGLES)    ;
            for(m=0;m<tri_list->n_vertex_idxs;m++){
              va[0] =    tri_list->vertex_idx[m].a ;     //  Index in
              va[1] =    tri_list->vertex_idx[m].b ;
              va[2] =    tri_list->vertex_idx[m].c ;
//  Loop over nodes of triangle
              for(n=0;n<3;n++){
                ipoint    = vtx_list[va[n]].ipoint    ;
                inormal   = vtx_list[va[n]].inormal   ;
                vertex_uv = vtx_list[va[n]].vertex_uv[0] ;  // There may be more than one texture
                x  = snode->point[ipoint].X ;
                y  = snode->point[ipoint].Y ;
                z  = snode->point[ipoint].Z ;
                nx = snode->normal[inormal].X ;
                ny = snode->normal[inormal].Y ;
                nz = snode->normal[inormal].Z ;
                if(0 && ip)printf("  display_wshape : %2i %2i %2i %2i :: %9f %9f %9f ::"
                        "%9f %9f %9f\n",
                                                    k,l,m,n, x,y,z, nx,ny,nz) ;
                glNormal3f(nx, ny, nz) ;
                glVertex3f(x, y, z) ;
              }              //  End of lop over triangle
            }                //  End of loop over triangles in tri-list
            glEnd() ;
            glShadeModel(GL_SMOOTH) ;
            glEnable(GL_TEXTURE_2D) ;
          }
          }      //  LOCAL BLOCK END
/*
 *  At this stage apply any lights
 */
uint  k;
          if(ip && witem && witem->worldtype == SIGNAL_ALT){
            signal_obj = &(witem->u.signal_obj) ;
            printf("  signal_obj = %p, n_matrices = %i, ima[0] = %i\n",
            (void *)signal_obj, signal_obj->n_matrices, ima[0]) ;
            for(k=0;k<signal_obj->n_matrices;k++){
              printf("  k = %i, sm_skip[k] = %i, sm_signal[k] = %p\n",
                     k, signal_obj->sm_skip[k],(void *)signal_obj->sm_signal[k]) ;
            }
          }
          if(witem && witem->worldtype == SIGNAL_ALT
//            && 1  && 0 == n_controllers
             && witem->u.signal_obj.sm_signal[ima[0]] ){
//            && (!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head1")
//            || !strcmp_ic(witem->snode->matrix[ima[0]].name,"Head2") ) ) {
//            && (!strncmp_ic(witem->snode->matrix[ima[0]].name,"Head",4) ) ) {

  int      k, n, m, it, il ;
  double   radius ;
  TrkItem *trk_item ;
  SignalDB  *signal ;
  SigType   *sig_type ;
  SigDraw   *sig_draw ;
  SigLight  *sig_light ;
  LightTab  *light_tab ;
  TextureNode *texture ;
  char      string[2048] ;

           signal   = witem->u.signal_obj.sm_signal[ima[0]] ;
//            ip = 0;
#if 1
//            ip = ip && l_time_1s && (witem->uid == 87) ;
//            ip = l_time_1s && (witem->uid == 340) ;
            if(ip){
              printf(" ++++++++  Display Light ++++++++++\n") ;
              printf(" World item uid = %i, type = %i  %s\n",
                        witem->uid, witem->worldtype, token_idc[witem->worldtype]) ;
              printf(" Sub-object = %i, Matrix = %i, name = %s\n",
                                      l,ima[0], witem->snode->matrix[ima[0]].name) ;
            }
            if(ip){
              n = witem->u.signal_obj.n_signal_units ;
              for(m=0;m<n;m++){
                printf("    signal object %i : sub_object = %i, u_data = %i, track_item = %i\n",m,
                    witem->u.signal_obj.sub_object[m],
                    witem->u.signal_obj.u_data1[m],
                    witem->u.signal_obj.tr_item[m])  ;
              }
            }
#if 0
            if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head1")) m = 0 ;
            else if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head2")) m = 1 ;
            else if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head3")) m = 2 ;
            else if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head4")) m = 3 ;
            else if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head5")) m = 4 ;
            else if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head6")) m = 5 ;
            else if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head7")) m = 6 ;
            else if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head8")) m = 7 ;
            else if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head9")) m = 8 ;
            else if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head10")) m = 9 ;
            else if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head11")) m = 10 ;
            else if(!strcmp_ic(witem->snode->matrix[ima[0]].name,"Head12")) m = 11 ;
            else{
              printf("  Head name 1/2/3.../9 not found.  Name = %s\n",
                                          witem->snode->matrix[ima[0]].name) ;
              m = 0 ;
            }
#endif
//            it = witem->u.signal_obj.tr_item[m] ;
//            trk_item = &(track_db.trk_items_array[it]) ;
#endif
//            if(signal){
              if(ip){
                printf("  Routine %s.  Signal uid = %i, :: names :  shape %s, type %s, world %s\n",
                        my_name, signal->uid,signal->shape_name,signal->type_name,signal->world_name) ;
                printf("               Signal type       = %p\n",(void *)signal->sig_type) ;
                printf("               Signal draw       = %p\n",(void *)signal->sig_type->sig_draw) ;
                printf("               Signal draw_state = %i\n",signal->draw_state) ;
              }
              sig_type = signal->sig_type ;
              sig_draw = &sig_type->sig_draw[signal->draw_state] ;
              texture  = raw_signal_db->light_tex[0].tnode;

//            ip = 0 ;

              if(ip){
                printf("   Signal %i  :: names :  shape %s, type %s, world %s\n",
                      signal->uid,signal->shape_name,signal->type_name,signal->world_name) ;
              }
              if(ip){
                printf("  Draw state = %i\n",signal->draw_state) ;
                printf("    index    = %i\n",sig_draw->index) ;
                printf("    name     = %s\n",sig_draw->name) ;
                printf("    n_draw_lights = %i\n",sig_draw->n_draw_lights) ;
              }
              if(texture){
                gl_tex_ref_no = texture->gl_tex_ref_no ;
                glBindTexture(GL_TEXTURE_2D, gl_tex_ref_no) ;
                if(ip){
                  printf("  Texture    = %s\n",texture->name) ;
                  printf("    basic = %i, needed = %i, loaded = %i, ref no. = %i\n",
                              texture->basic, texture->needed,
                              texture->loaded, texture->gl_tex_ref_no) ;
                }
              }
/*
 *  Draw only if n_draw_lights > 0 and n_sig_lights > 0
 *       and ensure that m is less than both.
 */
              for(m=0;m<sig_draw->n_draw_lights && m<sig_type->n_sig_lights;m++){
                n = sig_draw->draw_light_i[m] ;
                sig_light = &sig_type->sig_light[n] ;
                light_tab = sig_light->light_tab ;
                radius = sig_light->radius ;

                if(ip){
                  printf("  Signal light = %i :: %i\n",m,n) ;
                  printf("    Index %i \n",sig_light->index ) ;
                  printf("    Name  %s \n",sig_light->name ) ;
                  printf("    Light name = %s\n",light_tab->name ) ;
                  printf("    Radius = %f\n",radius ) ;
                  printf("    Position = %f %f %f\n",
                        sig_light->posn[0],sig_light->posn[1],sig_light->posn[2] ) ;
                  printf("    Colour   = %f %f %f : %f\n",
                        light_tab->colour[0],light_tab->colour[1],
                        light_tab->colour[2], light_tab->colour[3] ) ;
                }
  /*
  * Draw the light
  */
                glPushMatrix() ;
                glTranslatef((GLfloat) -sig_light->posn[0],
                            (GLfloat) sig_light->posn[1],
  //                          (GLfloat) -sig_light->posn[2]-0.003);
                            (GLfloat) -sig_light->posn[2]-0.005);

  //              glDisable(GL_BLEND) ;
  //              glDisable(GL_ALPHA_TEST);
  //              glDisable(GL_TEXTURE_2D) ;
                glDisable(GL_LIGHTING) ;
  //  GL_DECAL    - outer decal region is colourde green - rest as texture
  //  GL_REPLACE  - white texture
  //  GL_MODULATE - normal ??
  //  GL_BLEND    - Odd
  //  GL_ADD      - Black region of decal is green - rest white
  //  GL_COMBINE  - Lines are black - rest is green - Correct??
                glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE) ;
                glColor4fv(light_tab->colour) ;   //  Red, Green, Blue, Alpha
  #if 0
                glBegin(GL_TRIANGLE_FAN) ;
                  glVertex3f(0.0,0.0,0.0) ;
                  glVertex3f(0.0,radius,0.0) ;
                  glVertex3f(radius,0.0,0.0) ;
                  glVertex3f(0.0,-radius,0.0) ;
                  glVertex3f(-radius,0.0,0.0) ;
                  glVertex3f(0.0,radius,0.0) ;
                glEnd() ;
  #else
                glBegin(GL_QUADS) ;
                  glTexCoord2f(0.0,0.0) ;
                  glVertex3f(-radius,-radius,0.0) ;
                  glTexCoord2f(0.0,1.0) ;
                  glVertex3f(-radius, radius,0.0) ;
                  glTexCoord2f(1.0,1.0) ;
                  glVertex3f( radius, radius,0.0) ;
                  glTexCoord2f(1.0,0.0) ;
                  glVertex3f( radius,-radius,0.0) ;
                glEnd() ;

  #endif
  /*
  *  Code to identify active signal lights
  */
  #if 0
                glDisable(GL_TEXTURE_2D) ;
                glBegin(GL_LINES)  ;
                  glColor3f(1.0,0.0,0.0)  ;
                  glVertex3f(0.0,0.0,0.0) ;
                  glVertex3f(1.0,0.0,0.0) ;
                  glColor3f(0.0,1.0,0.0)  ;
                  glVertex3f(0.0,0.0,0.0) ;
                  glVertex3f(0.0,1.0,0.0) ;
                  glColor3f(0.0,0.0,1.0)  ;
                  glVertex3f(0.0,0.0,0.0) ;
                  glVertex3f(0.0,0.0,1.0) ;
                glEnd() ;

                glColor3f((GLfloat) 1.0, (GLfloat) 1.0, (GLfloat) 1.0) ;
                sprintf(string,"  Signal : %i :: World : %i :: Tile : %i %i",
                        signal->uid,witem->uid,
                        witem->world_node->tile_x,witem->world_node->tile_y) ;
                print_string_in_window2((GLfloat) 1.0, (GLfloat) 1.0,
                                        (GLfloat) -1.0, string);
  #endif
                glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE) ;
                glEnable(GL_BLEND) ;
                glEnable(GL_ALPHA_TEST) ;
                glEnable(GL_TEXTURE_2D) ;
                glEnable(GL_LIGHTING) ;
                glPopMatrix() ;
              }

          }
          glPopMatrix() ;
        }
      }

      return 0 ;
}

