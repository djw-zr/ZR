/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  shaders.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines to setup the shaders used by ZR
 *
 *==============================================================================
 */

GLuint shader_program_init(const GLchar **vertex_shader, const GLchar **fragment_shader)
{
GLuint VertexShader, FragmentShader, glProgram ;
GLint  glCompiled,   glLinked,       glValidated ;

/*
 *  Create Vertex Shader
 */
      VertexShader = glCreateShader(GL_VERTEX_SHADER) ;
      glShaderSource(VertexShader,1,vertex_shader,NULL) ;
      glCompileShader(VertexShader) ;

      glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &glCompiled) ;

      if(!glCompiled){
        GLint length ;
        GLchar *log ;
        glGetShaderiv(VertexShader, GL_INFO_LOG_LENGTH, &length) ;
        log = (GLchar *)malloc(length*sizeof(GLchar)) ;
        glGetShaderInfoLog(VertexShader, length, &length, log) ;
        fprintf(stderr, " Compile log = '%s'\n", log) ;
      }
/*
 *  Create Fragment Shader
 */
      FragmentShader = glCreateShader(GL_FRAGMENT_SHADER) ;
      glShaderSource(FragmentShader,1,fragment_shader,NULL) ;
      glCompileShader(FragmentShader) ;

      glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &glCompiled) ;

      if(!glCompiled){
        GLint length ;
        GLchar *log ;
        glGetShaderiv(FragmentShader, GL_INFO_LOG_LENGTH, &length) ;
        log = (GLchar *)malloc(length*sizeof(GLchar)) ;
        glGetShaderInfoLog(FragmentShader, length, &length, log) ;
        fprintf(stderr, " Compile log = '%s'\n", log) ;
      }
/*
 *  Create Program
 */
      glProgram = glCreateProgram() ;
      glAttachShader(glProgram, VertexShader) ;
      glAttachShader(glProgram, FragmentShader) ;
      glLinkProgram(glProgram) ;
      glGetProgramiv(glProgram, GL_LINK_STATUS, &glLinked) ;
      printf(" glLinked = %i\n",glLinked) ;
      if(glLinked) {
//        glUseProgram(glProgram) ;
      }else{
        GLint length ;
        GLchar *log ;
        glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &length) ;
        printf(" length = %i\n",length) ;
        log = (GLchar *)malloc(length*sizeof(GLchar)) ;
        glGetProgramInfoLog(glProgram, length, &length, log) ;
        printf(" Link log = '%s'\n", log) ;
      }
/*
 *  Verify Program
 */
      glValidateProgram(glProgram) ;
      glGetProgramiv(glProgram, GL_VALIDATE_STATUS, &glValidated) ;
      printf(" glValidated = %i\n",glValidated) ;
      if(!glValidated) {
        GLint length ;
        GLchar *log ;
        glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &length) ;
        printf(" length = %i\n",length) ;
        log = (GLchar *)malloc(length*sizeof(GLchar)) ;
        glGetProgramInfoLog(glProgram, length, &length, log) ;
        printf(" Link log = '%s'\n", log) ;
      }

      return glProgram ;
}

int shaders_init()
{
/*
 * Generate Basic Shader for use without lighting
 */
      shader_00 = shader_program_init(vertex_shader_2dl,fragment_shader_2dl) ;
      return 0 ;
}
