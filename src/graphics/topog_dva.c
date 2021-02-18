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

int display_tile_vertex_array(TileListNode *tnode){

int            i, j, k, l1, l2, n1, n2, n3, n4, i3, j3, m1, m3 ;
int            tile_x, tile_y ;
int            ip = 0     ;  // Debug printing
int            nht        ;  // Number of tile heights in each direction
double         dx_topog   ;  // Horizontal spacing in plot space units
double         dm_topog   ;  // Horizontal spacing in metres
double         dd_texture ;  // Texture repeats per topographic sample
double         floor, scale, v1[3], v2[3], v3[3], vv, av ;
GLdouble       tx1, tx2, ty1, ty2, x1, x2, y1, y2, h1, h2, h3, h4 ;
GLdouble       ttx1, ttx2, tty1, tty2 ; // Texture coordinates
GLdouble       *normals, *heights ;
int            *flata, flat ;         // = 1 if adjacent to a flat(ish) area
unsigned short *elevations  ;
char           my_name[] = "display_tile_vertex_arrays" ;

TerrainData    *terrain = &(tnode->terrain_data) ;
GLfloat        *vertex  ;
#ifdef normal_byte
GLbyte         *normal  ;
#else
GLfloat        *normal  ;
#endif
#ifdef texture_short
  GLshort        *texture ;
#else
  GLfloat        *texture ;
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
      }
/*
 *  Define material properties of land.
 *  Determines surface light values before textures are applied.
 */
      glMaterialfv(GL_FRONT,GL_AMBIENT,mat_amb_land) ;
      glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_dif_land) ;
      glMaterialfv(GL_FRONT,GL_SPECULAR,mat_spc_land) ;

      glBindTexture(GL_TEXTURE_2D, land_texture->gl_tex_ref_no) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
/*
 *  Main Loop
 */
      nv = tnode->terrain_data.nbx*tnode->terrain_data.nby ;
      for(iv=0;iv<nv;iv++){
        va_node = &(tnode->terrain_data.va_node[iv]) ;
        if(0== va_node->in_use) continue ;
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
        glVertexPointer(3,   GL_FLOAT, 0, vertex)  ;
#ifdef normal_byte
        glNormalPointer(GL_BYTE, 0, normal)       ;
#else
        glNormalPointer(GL_FLOAT, 0, normal)       ;
#endif
#ifdef texture_short
        glTexCoordPointer(2, GL_SHORT, 0, texture) ;
#else
        glTexCoordPointer(2, GL_FLOAT, 0, texture) ;
#endif
/*
 *  Draw Elements
 */
        glDrawElements(GL_TRIANGLES,nindex1,GL_UNSIGNED_INT,index1) ;
      }
      return 0 ;
}
