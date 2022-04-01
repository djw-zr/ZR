/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  topog_dva.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to set up the display lists for topography
 *   Using vertex arrays and buffer objects.
 *
 *   Routine to display vertex arrays for each of the tiles.
 *
 *==============================================================================
*/


#ifdef use_vertex_arrays

int display_tile_vertex_array(TileListNode *tnode){

int            ip = 0     ;  // Debug printing
char           my_name[] = "display_tile_vertex_arrays" ;

GLfloat        *vertex  ;
#ifdef normal_byte
GLbyte         *normal  ;
#else
GLfloat        *normal  ;
#endif
#ifdef texture_short
  GLshort        *texture, *texture0 = NULL ;
#else
  GLfloat        *texture, *texture0 = NULL ;
#endif
GLuint         *index1, nindex1;

int            nv, iv ;
VANode         *va_node ;

GLfloat  mat_amb_land[] = {0.2, 0.2, 0.2, 1.0};
GLfloat  mat_dif_land[] = {0.9, 0.9, 0.9, 1.0};
GLfloat  mat_spc_land[] = {0.5, 0.5, 0.5, 1.0};


      if(ip)printf(" Enter routine %s : %i %i : %i \n",
        my_name,tnode->tilex,tnode->tiley,tnode->needed) ;
      if(tnode->needed == 0) return 0;
      if(!land_texture->gl_tex_ref_no){
        printf("  ERROR.  Routine %s called when land_texture not installed\n",
                                                                       my_name);
        exit(1) ;
      }
/*
 *  Define material properties of land.
 *  Determines surface light values before textures are applied.
 */
      glMaterialfv(GL_FRONT,GL_AMBIENT,mat_amb_land) ;
      glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_dif_land) ;
      glMaterialfv(GL_FRONT,GL_SPECULAR,mat_spc_land) ;

      glBindTexture(GL_TEXTURE_2D, land_texture->gl_tex_ref_no) ;
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;
#if 0
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR) ;
#endif
/*
 *  Main Loop
 */
      nv = tnode->terrain_data.nbx*tnode->terrain_data.nby ;
      for(iv=0;iv<nv;iv++){
        va_node = &(tnode->terrain_data.va_node[iv]) ;
        if(0 == va_node->in_use) continue ;
/*
 *  Check clip planes
 */
        if(!check_topog_in_scene2(va_node->xa,va_node->ya,va_node->za))continue ;

        vertex  = va_node->va_vertex  ;
        normal  = va_node->va_normal  ;
        texture = va_node->va_texture ;
        nindex1 = va_node->nva_index  ;
        index1  = va_node->va_index   ;
        if(ip>1){
          printf("  CC terrain = %p \n", (void *)va_node);
          printf("  CC vertex  = %p \n", (void *)vertex) ;
          printf("  CC normal  = %p \n", (void *)normal) ;
          printf("  CC texture = %p \n", (void *)texture);
          printf("  CC index1  = %p \n", (void *)index1) ;
          printf("  CC nindex1 = %li\n",(long)nindex1) ;
        }
        if(NULL == vertex || NULL == normal || NULL == texture || NULL == index1){
          printf(" ERROR.  Routine %s.  Vertex array unset. %p %p %p %p\n",my_name,
            (void *)vertex,(void *)normal,(void *)texture,(void *)index1) ;
          printf("   Tile_x = %i, tile_y = %i\n",tnode->tilex,tnode->tiley) ;
          return 0;
        }
/*
 *  Specify Data
 */
        glVertexPointer(3, GL_FLOAT, 0, vertex) ;
#ifdef normal_byte
        glNormalPointer(GL_BYTE, 0, normal)     ;
#else
        glNormalPointer(GL_FLOAT, 0, normal)    ;
#endif

        if(texture != texture0){
#ifdef texture_short
          glTexCoordPointer(2, GL_SHORT, 0, texture) ;
#else
          glTexCoordPointer(2, GL_FLOAT, 0, texture) ;
#endif
          texture0= texture ;
        }
/*
 *  Draw Elements
 */
        n_vanodes_d++ ;
        glDrawElements(GL_TRIANGLES,nindex1,GL_UNSIGNED_INT,index1) ;
      }
      return 0 ;
}

#endif
