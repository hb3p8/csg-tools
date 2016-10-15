#include "ShaderProgram.hpp"

#include <GL/glext.h>

// =======================================================================
// function : ShaderProgram
// purpose  : Creates shader program
// =======================================================================
ShaderProgram::ShaderProgram (const std::string& theVertexShader,
                              const std::string& theFragmentShader)
{
  m_programID = glCreateProgram();

  // Compile the vertex shader
  GLuint m_vertexShaderID = glCreateShader (GL_VERTEX_SHADER);
  {
    const char* aSource = theVertexShader.c_str();
    glShaderSource (m_vertexShaderID, 1, &aSource, NULL);

    GLint isCompiled = 0;
    glCompileShader (m_vertexShaderID);
    glGetShaderiv (m_vertexShaderID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
      GLint aLogLength = 0;
      glGetShaderiv (m_vertexShaderID, GL_INFO_LOG_LENGTH, &aLogLength);
      GLchar aInfoLog[aLogLength];
      glGetShaderInfoLog (m_vertexShaderID, aLogLength, &aLogLength, aInfoLog);
      m_shaderLog = "Vertex shader compilation failed:\n" + std::string (aInfoLog);

      glDeleteShader (m_vertexShaderID);

      return;
    }
  }

  // Compile the fragment shader
  GLuint m_fragmentShaderID = glCreateShader (GL_FRAGMENT_SHADER);
  {
    const char* aSource = theFragmentShader.c_str();
    glShaderSource (m_fragmentShaderID, 1, &aSource, 0);

    GLint isCompiled = 0;
    glCompileShader (m_fragmentShaderID);
    glGetShaderiv (m_fragmentShaderID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
      GLint aLogLength = 0;
      glGetShaderiv (m_fragmentShaderID, GL_INFO_LOG_LENGTH, &aLogLength);
      GLchar aInfoLog[aLogLength];
      glGetShaderInfoLog (m_fragmentShaderID, aLogLength, &aLogLength, aInfoLog);
      m_shaderLog = "Fragment shader compilation failed:\n" + std::string (aInfoLog);

      glDeleteShader (m_fragmentShaderID);

      return;
    }
  }

  glAttachShader (m_programID, m_vertexShaderID);
  glAttachShader (m_programID, m_fragmentShaderID);

  // Link program
  glLinkProgram (m_programID);

  int isLinked = 0;
  glGetProgramiv (m_programID, GL_LINK_STATUS, &isLinked);
  if (isLinked == GL_FALSE) {

    GLint aLogLength = 0;
    glGetProgramiv (m_programID, GL_INFO_LOG_LENGTH, &aLogLength);
    GLchar aInfoLog[aLogLength];
    glGetProgramInfoLog (m_programID, aLogLength, &aLogLength, aInfoLog);
    m_shaderLog = "Shader program link failed:\n" + std::string (aInfoLog);

    glDeleteProgram (m_programID);
    glDeleteShader (m_vertexShaderID);
    glDeleteShader (m_fragmentShaderID);

    return;
  }

  // We don't need the shaders anymore
  glDetachShader (m_programID, m_vertexShaderID);
  glDetachShader (m_programID, m_fragmentShaderID);

  glDeleteShader (m_vertexShaderID);
  glDeleteShader (m_fragmentShaderID);
}

// =======================================================================
// function : ~ShaderProgram
// purpose  : Releases resources of shader program
// =======================================================================
ShaderProgram::~ShaderProgram()
{
  glDeleteProgram (m_programID);
}
