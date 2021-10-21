/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  shape_dlist.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   File containing the code to generate display lists for the shape files
 *   This is my second attempt - there may/will be more!!
 *
 *==============================================================================
 */
int generate_shape_display_list(ShapeNode *snode){

  LodControl  *lod_control    ;
  DistLevel   *dist_level     ;
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
  int         ip = 0          ;   //  0 = no printing
  int         ic = 0, icc = -1     ;   //  Control
  int         i, j, k, l, m, n ;
  int         im, ima[10]     ;   // Stack for matrices
  char        *name ;


  char     my_name[] = "generate_shape_display_list" ;

      if(0 == snode->n_lod_controls){
        printf("  No lod_controls for %s\n",snode->name) ;
        printf("  Display list routine exiting ... \n") ;
        return 0 ;
      }
      if(ip)printf(" Generate display list for : %s\n",snode->name) ;

//     ip = !strcmp(snode->name,"Vduct_1Arch-1Track");
//     ip = !strcmp(snode->name,"AU_Cedar_20m");
//      ip = !strcmp(snode->name,"AU_Ash_20m");
//      ip = !strcmp(snode->name,"underground_marker");
//      ip = !strcmp(snode->name,"1905-KL");
//     ip = !strcmp(snode->name,"BBS-HomesteadC");
//      if(strcmp(snode->name,name_control)== 0){      //  Allow control over items plotted
//       ip = 1 ;
//       ic = 1 ;
//       printf(" New dlist for %s\n",snode->name) ;
//      }

      if(0 == strcmp(snode->name,"underground_marker")) return 0;
      if(0 == strcmp(snode->name,"ctn_point_indication")) return 0;

/*
 *  Each state has a series of primitive states.  These refer to the textures
 *  requied plus various flags.  Here key items are printed out.
 */
      if(ip){
        n = snode->n_prim_states ;
        printf("  Primitive states:  nprim_states = %i\n",n);
        for(i=0;i<n;i++){
          prim_state = &snode->prim_state[i] ;
          printf("    prim_state : %2i : pointer = %p\n",i,(void *)prim_state) ;
          name = prim_state->name ;
          printf("                       name   = %p %s\n",(void *)name,name);
          printf("      n_tex_idxs = %i ",prim_state->n_tex_idxs);
          for(j=0;j<prim_state->n_tex_idxs;j++){
            printf(":: %i", prim_state->tex_idx[j]);
          }
          printf("\n");
        }
/*
 *  Each shape structure  has a "TexLevelLow" array:
 *  The value of tex_idx above is the index into this array : listed here:
 */
        n = snode->n_texlevel_low ;
        printf("  TexLevelLow objects:     n_texlevel_low = %i\n",n);
        for(i=0;i<n;i++){
          texlevel_low = &snode->texlevel_low[i] ;
          printf("            : %i : pointer = %p \n",i,(void *)texlevel_low);
          printf("          i = %i : iImage = %i :: %i :: %x :: %f\n",i,
                 texlevel_low->iImage, texlevel_low->FilterMode,
                 texlevel_low->BorderColor, texlevel_low->MipMapLODBias);
        }
/*
 *  The value of iImage above is the index into the shape's array of textures
 *  lister here:
 */
        n = snode->n_textures ;
        printf("  Textures:            n_textures = %i\n",n);
        for(i=0;i<n;i++){
          texture = snode->texture[i] ;
          name      = snode->texture_name[i]     ;
          printf("    Texture         : %i : pointer = %p  %p %s\n",
                                       i,(void *)texture,(void *)name,name);
        }
      }
/*
 *   Each shape has a series of 'lod_controls', (usually just 1?)
 *        each of which has a series of distance levels  (usually 1, 2 or 3)
 *            each distance level has a range (metres) and a number of sub-objects
 *                each sub-object then has a series of triangle lists
 *                     each triangle list has pointers to textures and the sets of triangles to plot
 *   So it is quite a hierarchy
 *   There are also a lot of variables which at present I am ignoring - at my peril.
 *   These need to be properly identified and addresed.
 */

/*
 * Loop over the lod_controls : Each contains a number of distance levels, it might be fastest if
 *      these were combined into a single 'distance' display list, but for the moment I'll
 *      put the display list pointer in each distance level structure and later call them all.
 */
      for(i=0; i<snode->n_lod_controls; i++){
        lod_control = &(snode->lod_control[i]) ;
        if(ip)printf(" lod_control:  i = %i\n",i) ;
        for(j=0; j<lod_control->n_dist_levels; j++){
          dist_level = &(lod_control->dist_level[j]) ;

          gl_display_list = glGenLists((GLsizei) 1) ;
          glNewList( gl_display_list, GL_COMPILE) ;             //  TEST FOR ERROR
          dist_level->gl_display_list = gl_display_list ;
          if(ip)printf("   dist_level:  j = %i,   gl_display_list = %i\n",j,gl_display_list) ;

          for(k=0;k<dist_level->n_sub_objects;k++){
            sub_object = &(dist_level->sub_object[k]) ;
            vtx_list   = sub_object->vertex         ;
            if(ip)printf("     sub_object:  k = %2i,  sub_object = %p,  vtx_list = %p\n",
                   k,(void *)sub_object, (void *)vtx_list) ;
            for(l=0;l<sub_object->n_tri_lists;l++){
              tri_list = &(sub_object->tri_list[l]) ;

              icc = icc+1 ;
              if(ic && icc>=n_control) break ;
              if(ic){
                printf("********************************************************************\n");
                printf("  icc = %i,  i_control1 = %i, i_control2 = %i\n",
                                              icc,i_control1[icc],i_control2[icc]) ;
              }
              if(ic && i_control1[icc]==0)continue ;   // Only plot when == 1

              if(ip)printf("     tri_list:  l = %i,  tri_list = %p,  n_vertex_idxs = %i\n",
                        l, (void *)tri_list, tri_list->n_vertex_idxs) ;
              if(0 == tri_list->n_vertex_idxs) continue ;

              prim_state_idx = tri_list->prim_state_trilist     ;
              prim_state = &(snode->prim_state[prim_state_idx]) ;


//  Each prim state has a texture
              tex_idx = prim_state->tex_idx[0]       ;
              texlevel_low = &snode->texlevel_low[tex_idx] ;
              texture      = snode->texture[texlevel_low->iImage]      ;

              if(ip)printf("      prim_state_idx = %i, prim_state = %p, prim_state->name = %s,"
                           " tex_idx = %i, texture = %p\n",
               prim_state_idx, (void *)prim_state, prim_state->name,tex_idx, (void *)texture) ;

//  Tempory fix for bug (overwriting?) affecting two(?) MSTS textures

//              if((long long)texture < (long long)0x100000 ||
//                 (long long)texture > (long long)0x100000000) texture = NULL ;

              gl_tex_ref_no = (texture == NULL ? 0 : texture->gl_tex_ref_no) ;

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

              if(ip)printf("      gl_tex_ref_no = %i\n",gl_tex_ref_no);
//
//  and one or more matrices which define the final position of the sub-object.
//  A wagon may contain one definition of a wheel axel but four sub-objects with
//  different matrices to move the wheel to a bogie and then move the bogie into
//  position.
//
               ivtx_state = prim_state->ivtx_state       ;
               vtx_state  = &(snode->vtx_state[ivtx_state]) ;
               glMatrixMode(GL_MODELVIEW) ;
               glPushMatrix() ;

               im = 0 ;
               ima[0] = vtx_state->imatrix ;
               while(ima[im] >= 0){
                 if(im>=10 || ima[im]>snode->nmatrices){
                   printf(" Routine %s, matrix out of bounds for shape %s\n",
                                                                  my_name,snode->name);
                   printf("   im = %i,  iam[im] = %i\n",im,ima[im]);
                   abort();
                 }
                 im++ ;
                 ima[im] = dist_level->hierarchy[ima[im-1]] ;
               }

               while(im-->0){
                 matrix4x3   = &(snode->matrix[ima[im]])      ;
                 if(ic == 0 || i_control2[icc] == 1){
                 if(matrix4x3->type != 0){

                   if(ip)printf(" shape_dlist2 : shape = %s, sub_object = %i, tri_list = %i, prim_state = %i, prim_state->name = %s, vertex_state = %i, hierarchy[vs] = %i, imatrix_type = %i\n",
                              snode->name, k, l, prim_state_idx, prim_state->name, ivtx_state,
                              dist_level->hierarchy[ivtx_state],matrix4x3->type) ;
                   if(matrix4x3->type == 1){
                     glTranslatef((GLfloat) matrix4x3->DX,
                                  (GLfloat) matrix4x3->DY,
                                  (GLfloat) matrix4x3->DZ) ;
                   }else{
                     float m[16] ;
                     make_matrix4x4(m,matrix4x3) ;
                     glMultMatrixf(m) ;
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
            }
          }
          glEndList();
        }
      }

      return 0 ;
}

int  add_texture_pointers_to_shape_items(ShapeNode *snode){
  int  i       ;
  int  ip = 0  ;   // 0 = no printing, 1 = error printing only
  TextureNode  *tnode        ;
  char         *name  = NULL ;
  char         my_name[] = "add_texture_pointers_to_shape_items" ;

      if(ip>1){
        printf("   Enter routine %s\n",my_name) ;
        printf("    Finding textures for shape %s\n",snode->name) ;
      }
/*
 *  Loop through required textures
 */
      for(i=0;i<snode->n_textures;i++){
        snode->texture[i] = NULL ;           //  Initialise link to NULL
        if(snode->texture_name[i] == NULL)continue ;  //  Skip if name is mising
        free(name) ;
        name = strdup(snode->texture_name[i]) ;
        name[strlen(name)-4] = '\0' ;
        if(ip>1)printf("  Texture required = %s\n",name) ;
/*
 *  Loop through list of textures
 */
        for(tnode=texturelist_beg; tnode!=NULL; tnode=tnode->next){
          if(strcmp_ic(name,tnode->name))continue ;
          snode->texture[i] = tnode     ;          // save link
          tnode->needed       = 1         ;
          if(ip>1)printf("  Texture found    = %s\n",tnode->name) ;
          break ;
        }
        if(ip && snode->texture[i] == NULL){
          printf("  Routine %s :: Shape %s,   texture %s not found\n",
                 my_name,snode->name,name) ;
#if 0
          for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
            printf(" Texture required = %s,  Present = %s\n",name,tnode->name) ;
          }
          printf(" Stopping ... \n") ;
          exit(0) ;
#endif
/*
 *  If texture is not found substitute 'BLANK'
 */
          for(tnode=wtexturelist_beg; tnode!=NULL; tnode=tnode->next){
            if(strcmp_ic("BLANK",tnode->name))continue ;
            snode->texture[i] = tnode     ;          // save link
            tnode->needed       = 1         ;
            printf("            Substitute used = %s\n",tnode->name) ;
            break ;
          }
          if(ip && snode->texture[i] == NULL){
            printf("       Substitute 'BLANK not found \n") ;
          }
        }
      }
      free(name) ;
      return 0;
}


