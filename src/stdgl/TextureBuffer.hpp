#ifndef HEADER_TEXTURE_BUFFER
#define HEADER_TEXTURE_BUFFER

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

//! Supported texture buffer formats.
enum TextureBufferType
{
  BufferType_INT,
  BufferType_FLT
};

//! Tool object for management OpenGL texture buffer object (TBO).
class TextureBuffer
{
public:

  //! Creates new OpenGL texture buffer object.
  TextureBuffer (TextureBufferType theBufferType = BufferType_FLT);

  //! Releases resources of OpenGL texture buffer object.
  virtual ~TextureBuffer();

public:

  //! Returns target of OpenGL texture buffer object.
  GLenum Target() const
  {
    return myTarget;
  }

  //! Returns handle of OpenGL texture buffer object.
  GLuint Name() const
  {
    return myHandleTBO;
  }

  //! Returns the number of pixel channels.
  GLint Channels() const
  {
    return myComps;
  }

  //! Returns OpenGL pixel format.
  GLenum Format() const
  {
    if (1 == myComps)
      return GL_RED;

    if (2 == myComps)
      return GL_RG;

    if (3 == myComps)
      return GL_RGB;

    return GL_RGBA;
  }

  //! Returns OpenGL internal format of the pixel data.
  GLint InternalFormat() const
  {
    if (myBufferType == BufferType_FLT)
    {
      if (1 == myComps)
        return GL_R32F;

      if (2 == myComps)
        return GL_RG32F;

      if (3 == myComps)
        return GL_RGB32F;

      return GL_RGBA32F;
    }
    else if (myBufferType == BufferType_INT)
    {
      if (1 == myComps)
        return GL_R32I;

      if (2 == myComps)
        return GL_RG32I;

      if (3 == myComps)
        return GL_RGB32I;

      return GL_RGBA32I;
    }

    return GL_RGBA32F;
  }

  //! Initializes OpenGL texture buffer object and uploads data to the GPU.
  bool Init (const GLint theSize, const GLint theChannels, const GLint* theData = NULL);

  //! Initializes OpenGL texture buffer object and uploads data to the GPU.
  bool Init (const GLint theSize, const GLint theChannels, const GLfloat* theData = NULL);

  //! Binds texture to the target.
  void Bind (const GLenum theUnit = GL_TEXTURE0) const;

  //! Binds default texture (with zero name) to the target.
  void Unbind (const GLenum theUnit = GL_TEXTURE0) const;

  //! Binds texture to the given image unit.
  void BindImage (const GLuint theUnit = 0, const GLenum theAccess = GL_READ_WRITE) const;

  //! Binds default texture (with zero name) to the given image unit.
  void UnbindImage (const GLuint theUnit = 0) const;

protected:

  //! Initializes OpenGL texture buffer object and uploads data to the GPU.
  bool init (const GLint theSize, const GLint theChannels, const GLvoid* theData);

  //! Target of OpenGL texture buffer object.
  GLenum myTarget;

  //! Handle (name) of OpenGL texture buffer object.
  GLuint myHandleTBO;

  //! Handle (name) of OpenGL vertex buffer object.
  GLuint myHandleVBO;

  //! Number of pixel channels (from 1 to 4).
  GLuint myComps;

  //! Type of texture buffer.
  TextureBufferType myBufferType;

};

#endif // HEADER_TEXTURE_BUFFER
