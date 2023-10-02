/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  display_wshape.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File containing the code to display the one of the wagon shapes
 *
 *==============================================================================
 */
int display_wshape(WagonNode *wagon, ShapeNode *snode, DistLevel *dist_level){

  uint        ip = 0          ;   //  0 = no printing
  static int  ic = 0,             //  1 = no printing, ic=ic-1 if ip != 0
              icc = 200  ;        //  Control ic = icc if ip and ic = 0
  uint        i, j, k, l, m, n ;
  int         im              ;
  int         ima[10]         ;   // Stack for matrices
  SubObject   *sub_object     ;
  TexLevelLow *texlevel_low   ;
  TextureNode *texture        ;
  Vertex      *vtx_list       ;
  TriList     *tri_list       ;
  PrimState   *prim_state     ;
  VTXState    *vtx_state      ;
  Matrix4x3   *matrix4x3      ;
  int         gl_tex_ref_no   ;
  int         prim_state_idx  ;
  int         tex_idx         ;
  int         ivtx_state      ;
  int         is_wheel        ;   //  = 1 when wheels being processed
  int         is_engine_wheel ;   //  = 1 when engine wheels being processed
  int         is_bogie        ;   //  = 1 when bogie being processed
  int         is_engine = 0   ;
  int         is_wiper  = 0   ;   //  = 1 when wiper being processed
  int         is_mirror = 0   ;   //  = 1 when mirror being processed
  int         is_pantograph = 0 ;   //  = 1 when pantograph being processed
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

  char     my_name[] = "display_wshape" ;
/*
 *==============================================================================
 */
      if(0 == snode->n_lod_controls){
        printf("  No lod_controls for %s\n",snode->name) ;
        printf("  Display list routine exiting ... \n") ;
        return 0 ;
      }

//     ip = !ic && !strcmp(snode->name,"UKRoyalScot");
//     ip = !ic && !strcmp(snode->name,"acelahhl") && (dist_level->dlevel_selection < 110.0) ;
//     ip = !ic && !strcmp(snode->name,"1905-S654");
//      if(strcmp(snode->name,name_control)== 0){      //  Allow control over items plotted
//       ip = 0 ;
//       ic = 1 ;
//       printf(" New dlist for %s\n",snode->name) ;
//      }

      if(ip){
        printf("\n\n =================================================\n") ;
        printf(" Generate display for : %s\n",snode->name) ;
      }

      is_engine = wagon->is_engine ;
/*
 *==============================================================================
 *  Print section
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
 *     each of which has a series of distance_levels  (usually 1, 2 or 3)
 *       each distance_level has a range (metres) and a number of sub_objects
 *         each sub_object then has a series of triangle lists
 *           each triangle_list has pointers to textures and the sets of
 *                  triangles to plot
 *
 *   In the current version of this routine the decision on which lod_control
 *   and dist_level to use is left to the calling program.  The loops here
 *   thus start with the sub_objects
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
 *  Loop oversub-objects
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
          if(ip){
            printf(" ===================================================\n") ;
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
/*
 *==============================================================================
 *  Each prim state has a texture if n_tex_idxs > 0
 *==============================================================================
 */
          texture = NULL ;
          if(0 != prim_state->n_tex_idxs){
            tex_idx = prim_state->tex_idx[0]       ;
            texlevel_low = &snode->texlevel_low[tex_idx] ;
            texture      = snode->texture[texlevel_low->iImage]      ;

            gl_tex_ref_no = (texture == NULL ? 0 : texture->gl_tex_ref_no) ;
            if(ip){
              printf(" ...................................................\n") ;
              printf("   Prim_state_idx = %2.2i, prim_state = %p, prim_state->name = %s\n"
                    "          tex_idx = %2.2i, texture    = %p, gl_tex_ref_no    = %i\n",
                                prim_state_idx, (void *)prim_state, prim_state->name,
                                tex_idx,(void *)texture,gl_tex_ref_no) ;
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
          }else{
            gl_tex_ref_no = 0 ;
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
 *  Cycle backwards through array 'ima' to apply hierarchy matrices
 *
 *  For efficiency each matrix is flagged as being one of four matrix types
 *     0 = Unit diagonal, no translation
 *     1 = Translation only
 *     2 = Rotation and/or scaling, no translation
 *     3 = Rotation and/or scaling, translation
 */
          if(ip){printf("  Hierarchy  im = %i\n",im) ;  fflush(NULL) ; }
          while(im--){
            matrix4x3   = &(snode->matrix[ima[im]])         ;
            if(ip){
              printf("---------------------------------------------------------\n") ;
              printf("  Hierarchy AA  im = %i, ima[im] = %i,  Name = %s\n",
                        im,ima[im],matrix4x3->name) ;
            }
#if 0
            is_wheel = !strncmp_ic(matrix4x3->name,"wheels",6) ;
            is_engine_wheel = is_wheel && 7 == strlen(matrix4x3->name) ;
//            if(is_wheel)printf("   Matrix = %s, is_engine = %i, is_wheel = %i, is_engine_wheel = %i\n",
//                    matrix4x3->name,is_engine,is_wheel,is_engine_wheel) ;
            is_bogie = !strncmp_ic(matrix4x3->name,"bogie",5) ;
            is_wiper = !strncmp_ic(matrix4x3->name,"wiper",5) ;
            is_mirror     = !strncmp_ic(matrix4x3->name,"mirror",6) ;
            is_pantograph = !strncmp_ic(matrix4x3->name,"pantograph",10) ;
#else
             is_engine_wheel = matrix4x3->anim == MAT_DRIVER_WHEEL ;
             is_wheel        = matrix4x3->anim == MAT_WHEEL || is_engine_wheel ;
             is_bogie        = matrix4x3->anim == MAT_BOGIE      ;
             is_wiper        = matrix4x3->anim == MAT_WIPER      ;
             is_mirror       = matrix4x3->anim == MAT_MIRROR     ;
             is_pantograph   = matrix4x3->anim == MAT_PANTOGRAPH ;
#endif
/*
 *  Animation for matrix im
 */
            if(animation){
              anim_node = &(animation->anim_node[ima[im]])  ;
              n_controllers = anim_node->n_controllers      ;
            }else{
              anim_node = NULL  ;
              n_controllers = 0 ;
            }
            if(ip)printf("    n_controllers = %i\n",n_controllers) ;
/*
 *  No animations (except possibly this is a wheel or bogie)
 *  For the moment mirrors and pantographs remain in the default poition (down or closed)
 */
            if(n_controllers == 0  || is_wheel){
              if(ip)printf("    shape = %s, sub_object = %i, tri_list = %i\n"
                           "                prim_state = %i, prim_state->name = %s,\n"
                           "                vertex_state = %i, hierarchy[vs] = %i, matrix_type = %i\n",
                          snode->name, k, l, prim_state_idx, prim_state->name, ivtx_state,
                          dist_level->hierarchy[ivtx_state],matrix4x3->type) ;
              if(ip){
                printf("    Matrix    name = %s\n",matrix4x3->name) ;
                printf("    PrimState name = %s\n",prim_state->name) ;
              }
// Translation only
              if(matrix4x3->type == MAT_TRANSLATE){
                glTranslatef((GLfloat) matrix4x3->DX,
                             (GLfloat) matrix4x3->DY,
                             (GLfloat) matrix4x3->DZ) ;
                if(ip)printf("      Translate :: %f %f %f\n",matrix4x3->DX, matrix4x3->DY, matrix4x3->DZ) ;
// General Matrix but not unit diagonal
              }else if(matrix4x3->type != MAT_UNIT){
                float m[16] ;
                msts4x3_to_opengl4x4(m,matrix4x3) ;
                glMultMatrixf(m) ;
                if(ip)printf("      General matrix operation\n") ;
              }
/*
 *  1.  Wheels :  Apply rotation to wheels around MSTS 'x' axis
 */
              if(is_engine_wheel){
                glRotatef((GLfloat) wagon->driverwheel_angle,
                          (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0) ;
                if(ip)printf("      Driver Wheel angle :: %f\n",wagon->driverwheel_angle) ;
              }else if(is_wheel){
                glRotatef((GLfloat) wagon->wheel_angle,
                          (GLfloat) 1.0, (GLfloat) 0.0, (GLfloat) 0.0) ;
                if(ip)printf("      Wheel angle :: %f\n",wagon->wheel_angle) ;
/*
 *  2.  Bogies ;  Do nothing ?
 */
              }else if(is_bogie){


              }
/*
 *  Process animations for engines
 */
            }else if(is_engine){
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
 *  Order pairs so that transforation matrix is applied before before rotation matrix
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
//  Check for error !!
//  Calculate weights assuming wheel angle is key variable
//ERROR  -----    I1 can be too large >     n_anim_keys-1
//ERROR                After calculating weights both I0 and I1 must be in range 0 to nanim_keys - 1 !!!


//  WIPER
                dw = 0.0 ;
                if(is_wiper){
                  if(ip)printf("    Set wiper angle\n") ;
//                          printf("    wiper angle = %f, dw = %f, wa = %f\n",
//                                 wagon->engine->wiper_ang,dw,wa) ;
//                          dw = 1.0/(n_anim_keys - 1) ;
                  wa = 0.5*(1.0 + sin(radian*wagon->wipers_ang))*(n_anim_keys - 1) ;
                  i0 = wa ;
                  if(i0>n_anim_keys - 2)i0 = n_anim_keys - 2 ;
                  i1 = i0 + 1   ;
//  Pantograph
                }else if(is_pantograph){
//                  printf("  Display pantographs : dist = %f\n", wagon->engine->pantographs_dist ) ;
                  wa = wagon->pantographs_dist*(n_anim_keys - 1) ;
                  i0 = (wa>=0) ? wa : 0 ;
                  if(i0>(n_anim_keys-2))i0 = n_anim_keys-2;
                  i1 = i0 + 1 ;
//  Drivers mirrors
                }else if(is_mirror){
//                  printf("  Display mirrors : dist = %f\n", wagon->engine->mirrors_dist ) ;
                  wa = wagon->mirrors_dist*(n_anim_keys - 1) ;
                  i0 = (wa>=0) ? wa : 0 ;
                  if(i0>(n_anim_keys-2))i0 = n_anim_keys-2;
                  i1 = i0 + 1 ;
//  Assume RUNNING GEAR
                }else{
                  if(ip)printf("    Set running gear angle\n") ;
                  dw = 360.0/(n_anim_keys-1) ;
//                  wagon->wheel_angle = fmod(wagon->wheel_angle,360.0);
//                  if(wagon->wheel_angle<0.0)wagon->wheel_angle += 360.0 ;
                  wa = wagon->driverwheel_angle/dw ;
                  i0 = (wa>=0) ? wa : 0 ;
                  if(i0>(n_anim_keys-1))i0 = n_anim_keys-1 ;
                  i1 = i0 + 1 ;
                  if(i1>n_anim_keys-1)i1 = 0 ;
                }

                w1 = wa - i0 ;
                w0 = 1.0 - w1 ;

                if(ip){
                  printf(" Animation : Shape  = %s\n",snode->name) ;
                  printf("             Matrix = %s,  is_wheel = %i, is_wiper = %i\n",
                                        matrix4x3->name, is_wheel, is_wiper);
                  printf("             Indices = %i %i, weights = %f %f\n",i0,i1,w0,w1) ;
                }
#if 1
                if(cc->type == LINEAR_POS){
double x, y, z ;
                  x = w0*a0[i0].X + w1*a0[i1].X ;
                  y = w0*a0[i0].Y + w1*a0[i1].Y ;
                  z = w0*a0[i0].Z + w1*a0[i1].Z ;
                  glTranslatef((GLfloat) x,(GLfloat) y,(GLfloat) z) ;
                  if(ip || 0){
                    printf("      Animate LINEAR_POS\n") ;
                    printf("      Animate 0: a, dw, wa : %f,  %f,  %f\n",
                                                      wagon->wheel_angle, dw, wa) ;
                    printf("      Animate 0: Weight    : %f,  %f\n",w0,w1) ;
                    printf("      Animate 0: Translate : %f,  %f,  %f\n",
                                                            a0[i0].X,a0[i0].Y,a0[i0].Z) ;
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
                    if(ip)printf("      Animate Rotation\n") ;
                    s = 1.0/sin(a) ;
                    x = qx*s ;
                    y = qy*s ;
                    z = qz*s ;
                    a = -2.0*a*degree ;
                    glRotatef((GLfloat) a, (GLfloat) x,(GLfloat) y,(GLfloat) z) ;
                  }
                  if(ip || 0){
                    printf("      Animate 1: a, dw, wa : %f,  %f,  %f\n",
                                                      wagon->wheel_angle, dw, wa) ;
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
        }                  //  End of loop over tri-lists
        if(ip)printf("  End loop for sub-object %i : %i\n",k,dist_level->n_sub_objects) ;
      }                    //  End of main loop over sub_objects
/*
 *   Logic used to prevent printing every time routine is called
 */
      if(ip){
        printf("  End loop over k\n") ; fflush(NULL) ;
      }
      if(ip){
        ic-- ;
        if(0>=ic) ic=icc ;
        ip = 0 ;
      if(ic==icc){
        printf("  End loop over k\n") ; fflush(NULL) ;
      }
      }
      return 0 ;
}

