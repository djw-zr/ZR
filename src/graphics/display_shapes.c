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
int display_shape(WagonNode *wagon, ShapeNode *snode,
                  LodControl *lod_control,  DistLevel *dist_level){

  int         ip = 0          ;   //  0 = no printing
  int         ic = 0, icc = -1     ;   //  Control
  int         i, j, k, l, m, n ;
  int         im, ima[10]     ;   // Stack for matrices
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


  char     my_name[] = "display_shape" ;
/*
 *==============================================================================
 */
      if(0 == snode->n_lod_controls){
        printf("  No lod_controls for %s\n",snode->name) ;
        printf("  Display list routine exiting ... \n") ;
        return 0 ;
      }
      if(ip){
        printf("\n\n =================================================\n") ;
        printf(" Generate display for : %s\n",snode->name) ;
      }

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

      if(0 == strcmp(snode->name,"underground_marker")) return 0;
      if(0 == strcmp(snode->name,"ctn_point_indication")) return 0;

/*
 *==============================================================================
 *  Each state has a series of primitive states.  These refer to the textures
 *  requied plus various flags.  Here key items are printed out.
 *==============================================================================
 */
      if(ip){
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
 *  Each shape structure  has a "TexLevelLow" array:
 *  The value of tex_idx above is the index into this array : listed here:
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
 *  The value of iImage above is the index into the shape's array of textures
 *  lister here:
 *==============================================================================
 */
        n = snode->n_textures ;
        printf("  Textures:     n_textures = %i\n",n);
        for(i=0;i<n;i++){
          texture = snode->texture[i] ;
          name      = snode->texture_name[i]     ;
          printf("    Texture    : %2.2i : pointer = %p  %p    %s\n",
                                       i,(void *)texture,(void *)name,name);
        }
      }
/*
 *==============================================================================
 *   Each shape has a series of 'lod_controls', (usually just 1?)
 *        each of which has a series of distance levels  (usually 1, 2 or 3)
 *            each distance level has a range (metres) and a number of sub-objects
 *                each sub-object then has a series of triangle lists
 *                     each triangle list has pointers to textures and the sets of triangles to plot
 *   So it is quite a hierarchy
 *   There are also a lot of variables which at present I am ignoring - at my peril.
 *   These need to be properly identified and addresed.
 *==============================================================================
 */

/*
 *==============================================================================
 * Loop over the lod_controls : Each contains a number of distance levels, it might be fastest if
 *      these were combined into a single 'distance' display list, but for the moment I'll
 *      put the display list pointer in each distance level structure and later call them all.
 *==============================================================================
 */
//      for(i=0; i<snode->n_lod_controls; i++){
//        lod_control = &(snode->lod_control[i]) ;
//        if(ip)printf(" lod_control:  i = %i\n",i) ;
//        for(j=0; j<lod_control->n_dist_levels; j++){
//          dist_level = &(lod_control->dist_level[j]) ;
//          if(ip)printf("   dist_level:  j = %i,   gl_display_list = %i\n",j,gl_display_list) ;
/*
 *==============================================================================
 *  Loop over sub-objects
 *==============================================================================
 */

          for(k=0;k<dist_level->n_sub_objects;k++){
int  iskip = 0;
            if(id_shape >= 0){
              iskip = (k == id_shape) ;
              if(zr_clock_1[1].tv_sec%2)iskip = !iskip ;
            }
//           if(iskip) continue ;

            sub_object = &(dist_level->sub_object[k]) ;
            vtx_list   = sub_object->vertex         ;
            if(ip)printf("\n\n  Sub_object:  k = %2i,  sub_object = %p,  vtx_list = %p\n",
                   k,(void *)sub_object, (void *)vtx_list) ;
/*
 *==============================================================================
 *  Loop over tri-lists
 *==============================================================================
 */
            for(l=0;l<sub_object->n_tri_lists;l++){


              tri_list = &(sub_object->tri_list[l]) ;

              icc = icc+1 ;
              if(ic && icc>=n_control) break ;
              if(ic){
                printf("********************************************************************\n");
                printf("  icc = %i,  i_control1 = %i, i_control2 = %i\n",
                                              icc,i_control1[icc],i_control2[icc]) ;
              }
//              if(ic && i_control1[icc]==0)continue ;   // Only plot when == 1

              if(ip){
                printf("\n ===================================================\n") ;
                printf("    Tri_list:  l = %i,  tri_list = %p,  n_vertex_idxs = %i\n",
                        l, (void *)tri_list, tri_list->n_vertex_idxs) ;
              }
              if(0 == tri_list->n_vertex_idxs) continue ;
              ip = 0 ;
/*
 *==============================================================================
 *  Each tri_list has a prim(ary)_state
 *==============================================================================
 */
              prim_state_idx = tri_list->prim_state_trilist     ;
              prim_state = &(snode->prim_state[prim_state_idx]) ;
//              if(prim_state_idx<27 || prim_state_idx>33) continue ;
/*
 *==============================================================================
 *  Each prim state has a texture
 *==============================================================================
 */
              tex_idx = prim_state->tex_idx[0]       ;
              texlevel_low = &snode->texlevel_low[tex_idx] ;
              texture      = snode->texture[texlevel_low->iImage]      ;

//  Tempory fix for bug (overwriting?) affecting two(?) MSTS textures

//              if((long long)texture < (long long)0x100000 ||
//                 (long long)texture > (long long)0x100000000) texture = NULL ;

              gl_tex_ref_no = (texture == NULL ? 0 : texture->gl_tex_ref_no) ;
              if(ip){
                printf(" ...................................................\n") ;
                printf("      Prim_state_idx = %2.2i, prim_state = %p, prim_state->name = %-25s, tex_idx = %2.2i, texture = %p,  gl_tex_ref_no = %i\n",
                                  prim_state_idx, (void *)prim_state, prim_state->name,tex_idx,
                                  (void *)texture,gl_tex_ref_no) ;
              }

              if(texture == NULL){
                printf(" **********************shape_dlist*********************************\n");
                printf("  Routine: %s.  Image missing !\n",my_name);
                printf("  Shape = %s   Image = %i\n",snode->name,tex_idx);
                printf(" ******************************************************************\n");
                break ;
              }else{
                if(GL_TRUE != glIsTexture( texture->gl_tex_ref_no)){
                printf(" **********************shape_dlist*********************************\n");
                printf("  Routine: %s.  Image unloaded !\n",my_name);
                printf("  Shape = %s   Image = %i\n",snode->name,tex_idx);
                printf(" ******************************************************************\n");
                }
              }

/*
 *==============================================================================
 *  Each prim state also has one or more matrices which define the final position
 *  of the sub-object.
 *  A wagon may contain four sub-objects describing the wheels.  In principal
 *  these may all use the same set of vertices but use different matrices to
 *  move the wheel to a bogie and then move the bogie into position.  In practice
 *  however they may often use different set of vertices.
 *
 *  The index of the first matrix (if any) is the one pointed to by
 *  "vtx_state->imatrix", the next, if any is given by the
 *  "dist_level->hierarchy" array.
 *==============================================================================
 */
              ivtx_state = prim_state->ivtx_state       ;
              vtx_state  = &(snode->vtx_state[ivtx_state]) ;
              glMatrixMode(GL_MODELVIEW) ;
              glPushMatrix() ;

              im = 0 ;
              ima[0] = vtx_state->imatrix ;
              while(ima[im] >= 0){
                if(im>=10 || ima[im]>snode->nmatrices){
                  printf(" Routine %s, matrix out of bounds for shape %s\n", my_name,snode->name);
                  printf("   im = %i,  iam[im] = %i\n",im,ima[im]);
                  abort();
                }
                im++ ;
                ima[im] = dist_level->hierarchy[ima[im-1]] ;
              }

              if(ip && im>0)printf("      Hierarchy  im = %i\n",im) ;
              while(im-->0){
                matrix4x3   = &(snode->matrix[ima[im]])      ;
                if(ic == 0 || i_control2[icc] == 1){
                  if(matrix4x3->type != 0){

                    if(ip)printf("  shape = %s, sub_object = %i, tri_list = %i, prim_state = %i, prim_state->name = %s, vertex_state = %i, hierarchy[vs] = %i, imatrix_type = %i\n",
                                snode->name, k, l, prim_state_idx, prim_state->name, ivtx_state,
                                dist_level->hierarchy[ivtx_state],matrix4x3->type) ;
                    if(ip){
                      printf("    Matrix    name = %s\n",matrix4x3->name) ;
                      printf("    PrimState name = %s\n",prim_state->name) ;
                    }

                    if(matrix4x3->type == 1){
                      glTranslatef((GLfloat) matrix4x3->DX,
                                   (GLfloat) matrix4x3->DY,
                                   (GLfloat) matrix4x3->DZ) ;
                      if(ip)printf("      Translate :: %f %f %f\n",matrix4x3->DX, matrix4x3->DY, matrix4x3->DZ) ;
                    }else{
                      float m[16] ;
                      make_matrix4x4(m,matrix4x3) ;
                      glMultMatrixf(m) ;
                    }
/*
 *  Animations
 *
 *  1.  Wheels :  Apply rotation to wheels around MSTS 'x' axis
 */
                    if(matrix4x3->name[0] == 'W' || matrix4x3->name[0] == 'w'){
                      glRotatef((GLfloat) wagon->wheel_angle,
                                (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0) ;
                      if(ip)printf("      Wheel rotate :: %f\n",wagon->wheel_angle) ;
/*
 *  2.  Bogies ;  Do nothing ?
 */
                    }else if(matrix4x3->name[0] == 'B' || matrix4x3->name[0] == 'b'){
/*
 *  3.  Running gear ?
 */
                    }else if(snode->n_animations != 0){
Animation *animation    ;
AnimNode  *anim_node    ;
AnimController *c0, *c1, *cc ;
AnimKey        *a0, *a1      ;
int       n_anim_nodes   ;
int       n_controllers  ;
int       n_anim_keys    ;
int       i0, i1         ;
double    wa, dw, w0, w1 ;
double    x, y, z, w     ;

                      animation = snode->animation ;
                      n_anim_nodes = animation->n_anim_nodes ;

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
//  Animation for matrix im
                      anim_node = &(animation->anim_node[ima[im]])  ;
                      n_controllers = anim_node->n_controllers ;
//  Check for error !!
                      if(n_controllers>2){
                        printf("  Routine %s.  Error 'n_controllers' too large\n", my_name) ;
                        printf("  Shape = %s\n",snode->name) ;
                        printf("  n_controllers = %i\n",n_controllers) ;
                        exit(1) ;
                      }
/*
 *  Order pairs so that transforation is before rotation
 */
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
                      if(n_controllers>0){
                        glPopMatrix() ;
                        glPushMatrix() ;
                      }
                      for(i=0;i<n_controllers;i++){
                        if(i==0){cc = c0 ;}
                        else    {cc = c1 ;}
                        a0 = cc->anim_key ;
                        n_anim_keys = cc->n_anim_keys    ;
//  Check for error !!
//  Calculate weights assuming wheel angle is key variable
                        dw = 360.0/(n_anim_keys-1) ;
                        wa = wagon->wheel_angle/dw ;
                        i0 = wa ;
                        if(i0<0) i0 = 0 ;
                        if(i0>(n_anim_keys-1))i0 = n_anim_keys-1 ;
                        i1 = i0 + 1 ;
                        w1 = wa - i0 ;
                        w0 = 1.0 - w1        ;
#if 1
                        if(cc->type == LINEAR_POS){
#if 1
double x, y, z ;
                          x = w0*a0[i0].X + w1*a0[i1].X ;
                          y = w0*a0[i0].Y + w1*a0[i1].Y ;
                          z = w0*a0[i0].Z + w1*a0[i1].Z ;
                          glTranslatef((GLfloat) x,(GLfloat) y,(GLfloat) z) ;
                          if(ip || 0){
                            printf("      Animate 0: a, dw, wa : %f,  %f,  %f\n",
                                                             wagon->wheel_angle, dw, wa) ;
                            printf("      Animate 0: Weight    : %f,  %f\n",w0,w1) ;
                            printf("      Animate 0: Translate : %f,  %f,  %f\n",
                                                                   a0[i0].X,a0[i0].Y,a0[i0].Z) ;
                            printf("      Animate 0: Translate : %f,  %f,  %f\n",
                                                                   a0[i0].X,a0[i0].Y,a0[i0].Z) ;
                            printf("      Animate  : Translate : %f,  %f,  %f\n",x,y,z) ;
                          }
#endif
                        }else if(cc->type == TCB_ROT){
double x, y, z, s, a, b  ;
double qx, qy, qz, qw ;
                          if((a0[i0].X*a0[i1].X + a0[i0].Y*a0[i1].Y + a0[i0].Z*a0[i1].Z + a0[i0].W*a0[i1].W) > 0.0){
                            qx = w0*a0[i0].X + w1*a0[i1].X ;
                            qy = w0*a0[i0].Y + w1*a0[i1].Y ;
                            qz = w0*a0[i0].Z + w1*a0[i1].Z ;
                            qw = w0*a0[i0].W + w1*a0[i1].W ;
                          }else{
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
#if 1
// Skip for very small rotations
                          if(fabs(a)>0.001){
                            s = 1.0/sin(a) ;
                            x = qx*s ;
                            y = qy*s ;
                            z = qz*s ;
                            a = -2.0*a*degree ;
                            glRotatef((GLfloat) a, (GLfloat) x,(GLfloat) y,(GLfloat) z) ;
                          }
                          if(ip || 0){
                            printf("      Animate 0: a, dw, wa : %f,  %f,  %f\n",
                                                             wagon->wheel_angle, dw, wa) ;
                            printf("      Animate 0: Weight    : %f,  %f,  %f\n",w0,w1, w0+w1) ;
                            printf("      Animate 0: Rotate 0  : %f,  %f,  %f,  %f :: %f\n",
                                                 a0[i0].X, a0[i0].Y, a0[i0].Z, a0[i0].W,
                                                 a0[i0].X*a0[i0].X + a0[i0].Y*a0[i0].Y +
                                                 a0[i0].Z*a0[i0].Z + a0[i0].W*a0[i0].W) ;
                            printf("      Animate 0: Rotate 1  : %f,  %f,  %f,  %f :: %f\n",
                                                 a0[i1].X, a0[i1].Y, a0[i1].Z, a0[i1].W,
                                                 a0[i1].X*a0[i1].X + a0[i1].Y*a0[i1].Y +
                                                 a0[i1].Z*a0[i1].Z + a0[i1].W*a0[i1].W) ;
                            printf("      Animate 0: Rotate    : %f,  %f,  %f,  %f :: %f\n",
                                                               qx, qy, qz, qw,
                                                               qx*qx + qy*qy + qz*qz + qw*qw) ;
                            printf("      Animate : Rotate     : %f : %f,  %f,  %f\n",a,x,y,z) ;
                          }
#endif
                        }
#endif
                      }
                    }
                  }
                }
              }

              glBindTexture(GL_TEXTURE_2D, gl_tex_ref_no) ;
              glBegin(GL_TRIANGLES) ;
              for(m=0;m<tri_list->n_vertex_idxs;m++){
//                printf("\n         m = %i  n = ",m) ;
  int  va[3] ;
  int ipoint, inormal, vertex_uv ;
  GLfloat x, y, z, nx, ny, nz, u, v ;
                va[0] =    tri_list->vertex_idx[m].a ;     //  Index in
                va[1] =    tri_list->vertex_idx[m].b ;
                va[2] =    tri_list->vertex_idx[m].c ;
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
                  if(ip && 0)printf("  display_list : %2i %2i %2i %2i :: %9f %9f %9f ::"
                         "%9f %9f %9f :: %9f %9f\n",
                                                      k,l,m,n, x,y,z, nx,ny,nz, u,v) ;
                  glTexCoord2f(u, v) ;
                  glNormal3f(nx, ny, nz) ;
                  glVertex3f(x, y, z) ;
                }
              }
              glEnd() ;
              glPopMatrix() ;
              ip = 0 ;   //  DEBUG
            }
          }
//          glEndList();
//        }
//      }

      return 0 ;
}

