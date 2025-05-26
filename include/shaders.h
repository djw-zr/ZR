/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  shaders.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to setup the shaders used by ZR
 *
 *==============================================================================
 */

/*
 *  Shader for 2-D (screen) areas and lines.
 */

uint shader_00 ;

const GLchar *vertex_shader_2dl[] = {

//  Specify Version
    "#version 120\n"

    "void main(void) {\n"

//  Position
    "    gl_Position = ftransform();\n"

//  Colour
    "    gl_FrontColor = gl_Color;\n"
    "}"
};

const GLchar *fragment_shader_2dl[] = {

//  Specify Version
    "#version 120\n"

    "void main() {\n"
//  Colour
    "    gl_FragColor = gl_Color;\n"
    "}"
};
