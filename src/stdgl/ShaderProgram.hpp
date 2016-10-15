#ifndef _ShaderProgram_Header
#define _ShaderProgram_Header

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include <string>
#include <vector>

//! Wrapper for OpenGL program object.
class ShaderProgram
{

public:

  //! Creates uninitialized shader program.
  ShaderProgram (const std::string& theVertexShader, const std::string& theFragmentShader);

  //! Releases resources of shader program.
  virtual ~ShaderProgram();

  //! Fetches info log of program building process.
  std::string infoLog()
  {
    return m_shaderLog;
  }

  //! Returns true if the program has been built successfully.
  bool valid()
  {
    return m_shaderLog.empty();
  }

  //! Returns program ID.
  inline GLuint programId() const
  {
    return m_programID;
  }

protected:

  GLuint m_programID; //!< Handle of OpenGL shader program

  std::string m_shaderLog; //!< Stores info log of program building process

};

#endif // _ShaderProgram_Header
