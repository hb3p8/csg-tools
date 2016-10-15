#ifndef HEADER_TEXTURE_3D
#define HEADER_TEXTURE_3D

#include "Texture2D.hpp"
#include <GL/gl.h>

//! Tool object for management OpenGL 3D texture.
class Texture3D
{
public:

  //! Creates new OpenGL 3D texture.
  Texture3D (const GLuint theNbChannels = 4);

  //! Releases resources of OpenGL 3D texture.
  virtual ~Texture3D();

public:

  //! Returns target of OpenGL 3D texture.
  GLenum Target() const
  {
    return myTarget;
  }

  //! Returns handle of OpenGL 3D texture.
  GLuint Name() const
  {
    return myHandle;
  }

  //! Returns the number of pixel channels.
  GLint Channels() const
  {
    return myChannels;
  }

  //! Returns width of OpenGL 3D texture.
  GLint SizeX() const
  {
    return mySizeX;
  }

  //! Returns height of OpenGL 3D texture.
  GLint SizeY() const
  {
    return mySizeY;
  }

  //! Returns depth of OpenGL 3D texture.
  GLint SizeZ() const
  {
    return mySizeZ;
  }

  //! Returns OpenGL pixel format.
  GLenum virtual Format() const
  {
    if (1 == myChannels)
      return GL_RED;

    if (2 == myChannels)
      return GL_RG;

    if (3 == myChannels)
      return GL_RGB;

    return GL_RGBA;
  }

  //! Returns OpenGL internal format of the pixel data.
  GLint virtual InternalFormat() const
  {
    if (1 == myChannels)
      return GL_R32F;

    if (2 == myChannels)
      return GL_RG32F;

    if (3 == myChannels)
      return GL_RGB32F;

    return GL_RGBA32F;
  }

  //! Returns texture wrapping mode.
  const TextureWrapMode& Wrapping() const
  {
    return myWrapMode;
  }

  //! Sets texture wrapping mode.
  void SetWrapping (const TextureWrapMode& theWrapping)
  {
    myWrapMode = theWrapping;
  }

  //! Returns texture filtering mode.
  const TextureFilterMode& Filtering() const
  {
    return myFiltMode;
  }

  //! Sets texture filtering mode.
  void SetFiltering (const TextureFilterMode& theFiltering)
  {
    myFiltMode = theFiltering;
  }

  //! Initializes OpenGL 3D texture and uploads data to the GPU.
  bool Init (const GLint    theSizeX,
             const GLint    theSizeY,
             const GLint    theSizeZ,
             const GLfloat* thePixels = NULL);

  //! Updates OpenGL 3D texture data.
  bool Update (const GLfloat* thePixels);
  
  //! Binds texture to the target.
  void Bind (const GLenum theUnit = GL_TEXTURE0) const;
  
  //! Binds default texture (with zero name) to the target.
  void Unbind (const GLenum theUnit = GL_TEXTURE0) const;

protected:

  //! Target of OpenGL 3D texture.
  GLenum myTarget;

  //! Handle (name) of OpenGL 3D texture.
  GLuint myHandle;

  //! Width of OpenGL 3D texture.
  GLint mySizeX;

  //! Height of OpenGL 3D texture.
  GLint mySizeY;

  //! Depth of OpenGL 3D texture.
  GLint mySizeZ;

  //! Number of pixel channels (from 1 to 4).
  GLuint myChannels;

  //! Texture wrapping mode.
  TextureWrapMode myWrapMode;

  //! Texture filtering mode.
  TextureFilterMode myFiltMode;

};

#endif // HEADER_TEXTURE_3D
