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
  static int  ipp = 0         ;   //  1 = Print first pass only
  int         ic = 0, icc = -1     ;   //  Control
  uint        i, j, k, l, m, n ;
  uint        im              ;
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
  int         gl_display_list ;
  int         gl_tex_ref_no   ;
  int         prim_state_idx  ;
  int         tex_idx         ;
  int         ivtx_state      ;
  char        *name ;

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
//       ip = 0 ;
//       ic = 1 ;
//       printf(" New dlist for %s\n",snode->name) ;
//      }
      if(!ipp)ip = 0 ;
      if(ip) ipp = 0 ;

      if(witem && (witem->anim_value != 0.0) && ip){
        printf("  Routine %s entered for shape %s\n",
                  my_name,snode->name) ;
        printf("    World Item  uid = %i, worldtype = %i, filename = %s, anim_value = %f\n",
                  witem->uid,witem->worldtype,
                  witem->filename,witem->anim_value) ;
      }

      if(ip){
        printf("\n\n =================================================\n") ;
        printf(" Generate display for : %s\n",snode->name) ;
      }

      if(0 == strcmp(snode->name,"underground_marker")) return 0;
      if(0 == strcmp(snode->name,"ctn_point_indication")) return 0;
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
          printf("    prim_state : %2i : pointer = %p",i,(void *)prim_state) ;
          name = prim_state->name ;
          printf("    name   = %p    %-24s",(void *)name,name);
          printf("      n_tex_idxs = %i ",prim_state->n_tex_idxs);
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
        printf(" End of Key Variable Section\n\n") ;
      }
/*
 *==============================================================================
 *   Each shape has a series of 'lod_controls', (usually just 1?)
 *     each of which has a series of distance levels  (usually 1, 2 or 3)
 *       each distance level has a range (metres) and a number of sub-objects
 *          each sub-object then has a series of triangle lists
 *            each triangle list has pointers to textures and the sets of
 *                   triangles to plot
 *
 *   In the current version of this routine the decision on which lod_control
 *   and dist_level to use is left to the calling program.  The loops here
 *   thus start with the sub-objects
 *==============================================================================
 */
      animation = snode->animation ;
      if(animation){
        n_anim_nodes = animation->n_anim_nodes ;
      }else{
        n_anim_nodes = 0 ;
      }
      if(ip)printf("  snode         = %p\n", (void *)snode) ;
      if(ip)printf("  animation     = %p\n", (void *)animation) ;
      if(ip)printf("  n_anim_nodes  = %i\n", n_anim_nodes) ;
/*
 *==============================================================================
 *  Loop over sub-objects
 *==============================================================================
 */
      if(ip)printf("  dist_level    = %p\n", (void *)dist_level) ;
      if(ip)printf("  n_sub_objects = %i\n", dist_level->n_sub_objects) ;

      for(k=0;k<dist_level->n_sub_objects;k++){
        if(ip)printf("  Start loop for sub-object %i\n",k) ;
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
            printf(" ===================================================\n") ;
            printf("    Tri_list:  l = %i,  tri_list = %p,  n_vertex_idxs = %i\n",
                    l, (void *)tri_list, tri_list->n_vertex_idxs) ;
            printf("               ip = %i, ic = %i, icc = %i\n",ip,ic,icc) ;
          }
          if(0 == tri_list->n_vertex_idxs) continue ;
//        ip = 0 ;
/*
 *==============================================================================
 *  Each tri(angle)_list has a prim(ary)_state
 *==============================================================================
 */
          prim_state_idx = tri_list->prim_state_trilist     ;
          prim_state = &(snode->prim_state[prim_state_idx]) ;
/*
 *==============================================================================
 *  Each prim state has a texture if n_tex_idxs > 0
 *==============================================================================
 */
          texture = NULL ;
          if(0 != prim_state->n_tex_idxs){
            tex_idx = prim_state->tex_idx[0]       ;
            texlevel_low = &snode->texlevel_low[tex_idx] ;
            texture      = snode->texture[texlevel_low->iImage]  ;

            gl_tex_ref_no = (texture == NULL ? 0 : texture->gl_tex_ref_no) ;
            if(ip){
              printf(" ...................................................\n") ;
              printf("      Prim_state_idx = %2.2i, prim_state = %p, prim_state->name = %-25s, tex_idx = %2.2i, texture = %p,  gl_tex_ref_no = %i\n",
                                prim_state_idx, (void *)prim_state, prim_state->name,tex_idx,
                                (void *)texture,gl_tex_ref_no) ;
            }

            if(texture == NULL){
//  Used to debug but causess continuous errors with MSTS Route Japan 2.
#if 0
              printf(" **********************shape_dlist*********************************\n");
              printf("  Routine: %s.  Image missing !\n",my_name);
              printf("  Shape = %s   Image = %i\n",snode->name,tex_idx);
              printf(" ******************************************************************\n");
#endif
              break ;
            }else{
              if(GL_TRUE != glIsTexture( texture->gl_tex_ref_no)){
              printf(" **********************shape_dlist*********************************\n");
              printf("  Routine: %s.  Image unloaded !\n",my_name);
              printf("  Shape = %s   Image = %i\n",snode->name,tex_idx);
              printf(" ******************************************************************\n");
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
 *  a rotation.  Three flags are used, with enum values LINEAR_KEY, TCB_KEY
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
            printf("   Prim_state->ivtx_state = %i\n",prim_state->ivtx_state) ;
            printf("                vtx_state = %p\n",
                          (void *)&(snode->vtx_state[prim_state->ivtx_state]) );
            fflush(NULL) ;
          }
          ivtx_state = prim_state->ivtx_state       ;
          vtx_state  = &(snode->vtx_state[ivtx_state]) ;
/*
 *  Initialise array "ima" for this sub-object
 */
          im = 0 ;
          ima[0] = vtx_state->imatrix ;
          if(ip){
            if(ima[im]>=0){
              matrix4x3 = &(snode->matrix[ima[im]])  ;
              printf("  im = %i, ima[im] = %i,  name = %s\n", im, ima[im], matrix4x3->name) ;
            }else{
              printf("  im = %i, ima[im] = %i\n", im, ima[im]) ;
            }
          }
/*
 *   Loop over Matrices
 */
          while(ima[im] >= 0){
            if(im>=10 || ima[im]>(int)snode->nmatrices){
              printf(" Routine %s, matrix out of bounds for shape %s\n", my_name,snode->name);
              printf("   im = %i,  iam[im] = %i,  name = %s\n",im,ima[im],snode->matrix[ima[im]].name);
              abort();
            }
            im++ ;
            ima[im] = dist_level->hierarchy[ima[im-1]] ;
            if(ip){
              printf("  Hierarchy List\n") ;
              if(ima[im]>=0){
                matrix4x3 = &(snode->matrix[ima[im]])  ;
                printf("    im = %i, ima[im] = %i,  name = %s\n", im, ima[im], matrix4x3->name) ;
              }else{
                printf("    im = %i, ima[im] = %i\n", im, ima[im]) ;
              }
            }
          }
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
          if(ip)printf("      Hierarchy  im = %i\n",im) ;
          while(im--){
            matrix4x3   = &(snode->matrix[ima[im]])      ;
            a_lin = a_tcb = 0 ;                  // use matrix flag
            if(animation){
              anim_node = &(animation->anim_node[ima[im]])  ;
              n_controllers = anim_node->n_controllers      ;
              for(i=0;i<n_controllers;i++){
                n_anim_keys = anim_node->controller[i].n_anim_keys ;
                for(j=0; j< n_anim_keys; j++){
                  m = anim_node->controller[i].anim_key->type ;
                  if(m == TCB_KEY)   a_tcb = 1 ;
                  if(m == LINEAR_KEY)a_lin = 1 ;
                }
              }
            }else{
              anim_node = NULL  ;
              n_controllers = 0 ;
            }
            if(ip){
              printf("---------------------------------------------------------\n") ;
              printf("  Hierarchy  im = %i, ima[im] = %i,  Name = %s\n",
                        im,ima[im],matrix4x3->name) ;
                printf("    shape = %s, sub_object = %i, tri_list = %i\n"
                       "                prim_state = %i, prim_state->name = %s,\n"
                       "                vertex_state = %i, hierarchy[vs] = %i,"
                                      " matrix_type = %i\n",
                          snode->name, k, l, prim_state_idx, prim_state->name, ivtx_state,
                          dist_level->hierarchy[ivtx_state],matrix4x3->type) ;
                printf("    Matrix    name = %s\n",matrix4x3->name) ;
                printf("    PrimState name = %s\n",prim_state->name) ;
              printf("  Animation  n_controllers = %i\n",
                     n_controllers) ;
            }
/*
 *   First apply main matrices
 */
            if(matrix4x3->type){
/*
 *  Use Translation only if a_lin is zero (no linear_key animation)
 */
              if(matrix4x3->type == 1 && !a_lin){
                glTranslatef((GLfloat) matrix4x3->DX,
                             (GLfloat) matrix4x3->DY,
                             (GLfloat) matrix4x3->DZ) ;
                if(ip)printf("      Matrix Translate :: %f %f %f\n",matrix4x3->DX, matrix4x3->DY, matrix4x3->DZ) ;
/*
 *  Use Rotation only if a_tcb is zero (no tcb_key animation)
 */
              }else if(matrix4x3->type == 2 && !a_tcb){
                msts4x3_to_opengl4x4(m16,matrix4x3) ;
                glMultMatrixf(m16) ;
                if(ip)printf("      Matrix Rotate\n") ;
/*
 *  Matrix with rotation and translation
 */
              }else if(!(a_lin || a_tcb )){           //  No special action needed
                msts4x3_to_opengl4x4(m16,matrix4x3) ;
                glMultMatrixf(m16) ;
                if(ip)printf("      General matrix operation\n") ;
              }else if(!(a_lin && a_tcb )){
                msts4x3_to_opengl4x4(m16,matrix4x3) ;
                if(a_lin){                            //  Rotate only
                  for(i=12; i<15;i++) m16[i] = 0.0 ;
                  glMultMatrixf(m16) ;
                }else{                                // Translate only
                  glTranslatef((GLfloat) m16[12], (GLfloat) m16[13], (GLfloat) m16[14]) ;
                }
                if(ip)printf("      Modified general matrix operation\n") ;
              }
          }
/*
 *  Then animate controllers
 */
          if(witem && n_controllers){
            if(ip)printf("  Animation : ivtx_state = %i, im = %i, Matrix = %i \n", ivtx_state, im, ima[im]);

// Check number of matrices match
            if(n_anim_nodes != dist_level->n_hierarchy){
              printf("  Routine %s.  Mismatch of n_hierarchy and animation nodes\n",
                                      my_name) ;
              printf("  Shape = %s\n",snode->name) ;
              printf("  n_hierarchy = %i\n",dist_level->n_hierarchy) ;
              printf("  n_anim_nodes = %i\n",n_anim_nodes) ;
              exit(1) ;
            }
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
              if(c0->type == TCB_ROT && c1->type == LINEAR_POS){
                c0 = &(anim_node->controller[1]) ;
                c1 = &(anim_node->controller[0]) ;
              }else if(!(c0->type == TCB_ROT && c1->type == LINEAR_POS)){
                n_controllers = 0 ;
              }
            }
/*
 *  Loop over controllers
 */
            for(i=0;i<n_controllers;i++){
              if(i==0){cc = c0 ;}
              else    {cc = c1 ;}
              a0 = cc->anim_key ;
              n_anim_keys = cc->n_anim_keys    ;

              if(witem){
                wa = witem->anim_value*(n_anim_keys-1)  ;
                dw = 1.0/(n_anim_keys - 1) ;
                if(ip || ipp){
                  printf("  Routine %s entered for shape %s\n", my_name,snode->name) ;
                  printf("    World Item  uid = %i, worldtype = %i, filename = %s,"
                          " anim_value = %f\n", witem->uid,witem->worldtype,
                                                witem->filename,witem->anim_value) ;
                  printf("   anim_value = %f, wa = %f, n_anim_keys = %i, dw = %f\n",
                                                witem->anim_value,wa,n_anim_keys,dw) ;
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
                  printf("      Animate Linear Position\n") ;
                  printf("      Animate 0: dw, wa : %f,  %f\n", dw, wa) ;
                  printf("      Animate 0: Weight    : %f,  %f\n",w0,w1) ;
                  printf("      Animate 0: Translate : %f,  %f,  %f\n",
                                                          a0[i0].X,a0[i0].Y,a0[i0].Z) ;
                  printf("      Animate 0: Translate : %f,  %f,  %f\n",
                                                          a0[i1].X,a0[i1].Y,a0[i1].Z) ;
                  printf("      Animate  : Translate : %f,  %f,  %f\n",x,y,z) ;
                }
              }else if(cc->type == TCB_ROT){
double x=0.0, y=0.0, z=0.0, s, a, b  ;
double qx, qy, qz, qw ;
                if((a0[i0].X*a0[i1].X + a0[i0].Y*a0[i1].Y + a0[i0].Z*a0[i1].Z + a0[i0].W*a0[i1].W) > 0.0){
                  if(ip)printf("      Animate TCB_ROT  1\n") ;
                  qx = w0*a0[i0].X + w1*a0[i1].X ;
                  qy = w0*a0[i0].Y + w1*a0[i1].Y ;
                  qz = w0*a0[i0].Z + w1*a0[i1].Z ;
                  qw = w0*a0[i0].W + w1*a0[i1].W ;
                }else{
                  if(ip)printf("      Animate TCB_ROT  2\n") ;
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
                  if(ip)printf("      Animate Small rotation\n") ;
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
                  printf("      Animate 1: Rotate     : %f : %f,  %f,  %f\n",a,x,y,z) ;
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
                if(ip && 0)printf("  display_wshape : %2i %2i %2i %2i :: %9f %9f %9f ::"
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
                if(ip && 0)printf("  display_wshape : %2i %2i %2i %2i :: %9f %9f %9f ::"
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
          glPopMatrix() ;
//          ip = 0 ;   //  DEBUG
        }
      }

      return 0 ;
}

