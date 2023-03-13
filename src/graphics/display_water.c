/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  display_water.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Contains routines to display water
 *
 *   The graphics code is based on openglut amd OpenGL 2.1
 *
 *==============================================================================
 */

/*
 *  Routine to display terrain water height
 */

void display_topog_water(TileListNode *tl_node){

  int          ip = 0 ;
  double       ti = 1.0/tile_size ;
  GLfloat      h_sw, h_se, h_ne, h_nw ;
  GLfloat      tile_x, tile_y ;
  TerrainData  *t_data ;

      if(!tl_node ) return ;

      t_data = &(tl_node->terrain_data) ;
      tile_x = tl_node->tilex - tile_x0 ;
      tile_y = tl_node->tiley - tile_y0 ;
      h_sw = (t_data->terrain_water_height_offset_SW - tile_h0)*ti ;
      h_se = (t_data->terrain_water_height_offset_SE - tile_h0)*ti ;
      h_ne = (t_data->terrain_water_height_offset_NE - tile_h0)*ti ;
      h_nw = (t_data->terrain_water_height_offset_NW - tile_h0)*ti ;

//      ip = (tl_node->tilex == 1448 && tl_node->tiley==10331) ;
//      ip = (tl_node->tilex == 1450 && tl_node->tiley==10331) ;
      if(ip){
        printf("  Water :: %i %i :: %f %f :: %f %f %f %f :: %f\n",
               tl_node->tilex, tl_node->tiley, tile_x, tile_y,
               t_data->terrain_water_height_offset_SW,
               t_data->terrain_water_height_offset_SE,
               t_data->terrain_water_height_offset_NE,
               t_data->terrain_water_height_offset_NW,
               tile_h0) ;
      }
      glDisable(GL_LIGHTING) ;
      glDisable(GL_TEXTURE_2D) ;

      glColor3f(0.5,0.5,1.0) ;
      glBegin(GL_QUADS) ;
        glVertex3f(tile_x,       tile_y,       h_sw) ;
        glVertex3f(tile_x + 1.0, tile_y,       h_se) ;
        glVertex3f(tile_x + 1.0, tile_y + 1.0, h_ne) ;
        glVertex3f(tile_x,       tile_y + 1.0, h_nw) ;
      glEnd() ;

      glEnable(GL_LIGHTING) ;
      glEnable(GL_TEXTURE_2D) ;

      return ;
}


