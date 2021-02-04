/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File test_sphere.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Generate a display list containing a broken sphere of triangles
 *
 *==============================================================================
 */

int  test_sphere(float radius)
{
  int   i, j, klist ;
  float a1, a2, b1, b2, x1, y1, z1, x2, y2, z2 ;

      klist = glGenLists(1) ;
      glNewList(klist, GL_COMPILE) ;
      glBegin(GL_TRIANGLES) ;

      for(i=0;i<35;i++){
        a1 = i*10.0*radian ;
        a2 = (i+1)*10.0*radian ;
        for(j=0;j<17;j++){
          b1 = j*10.0*radian ;
          b2 = (j+1)*10.0*radian ;

          x2 = sin(b1)*cos(a1) ;
          y2 = cos(b1) ;
          z2 = sin(b1)*sin(a1) ;
          x1 = x2*radius ;
          y1 = y2*radius ;
          z1 = z2*radius ;
          glNormal3d(x2,y2,z2) ;
          glVertex3d(x1,y1,z1) ;

          x2 = sin(b2)*cos(a2) ;
          y2 = cos(b2) ;
          z2 = sin(b2)*sin(a2) ;
          x1 = x2*radius ;
          y1 = y2*radius ;
          z1 = z2*radius ;
          glNormal3d(x2,y2,z2) ;
          glVertex3d(x1,y1,z1) ;

          x2 = sin(b2)*cos(a1) ;
          y2 = cos(b2) ;
          z2 = sin(b2)*sin(a1) ;
          x1 = x2*radius ;
          y1 = y2*radius ;
          z1 = z2*radius ;
          glNormal3d(x2,y2,z2) ;
          glVertex3d(x1,y1,z1) ;
        }
      }
      glEnd() ;
      glEndList() ;

      return klist ;
}

