#ifndef HEADER_TEXTURE_2D
#define HEADER_TEXTURE_2D

#include "Types.hpp"
#include <GL/gl.h>

//! Describes wrap parameters for texture coordinates.
struct TextureWrapMode
{
  GLint WrapS;
  GLint WrapT;
  GLint WrapR;
    
  //! Creates new texture wrap parameters for all texture coordinates.
  TextureWrapMode (const GLint theWrapS = GL_CLAMP,
                   const GLint theWrapT = GL_CLAMP,
                   const GLint theWrapR = GL_CLAMP)
  : WrapS (theWrapS),
    WrapT (theWrapT),
    WrapR (theWrapR)
  {
    //
  }                        

  //! Sets OpenGL texture wrap parameters.
  void Setup (const GLenum theTarget) const;
};

//! Describes texture minifying and magnification filter.
struct TextureFilterMode
{
  GLint MinFilter;
  GLint MagFilter;

  //! Creates new texture filter parameters.
  TextureFilterMode (const GLint theMinFilter = GL_NEAREST,
                     const GLint theMagFilter = GL_NEAREST)
  : MinFilter (theMinFilter),
    MagFilter (theMagFilter)
  {
    //
  }
    
  //! Sets texture minifying and magnification functions.
  void Setup (const GLenum theTarget) const;
};

//! Tool object for management OpenGL 2D texture.
class Texture2D
{
public:

  //! Creates new OpenGL 2D texture.
  Texture2D (const GLenum theTarget = GL_TEXTURE_2D);

  //! Releases resources of OpenGL 2D texture.
  virtual ~Texture2D();

public:

  //! Returns target of OpenGL 2D texture.
  GLenum Target() const
  {
    return myTarget;
  }

  //! Returns handle of OpenGL 2D texture.
  GLuint Name() const
  {
    return myHandle;
  }

  //! Returns the number of pixel channels.
  GLint Channels() const
  {
    return myComps;
  }

  //! Returns width of OpenGL 2D texture.
  GLint SizeX() const
  {
    return mySizeX;
  }

  //! Returns height of OpenGL 2D texture.
  GLint SizeY() const
  {
    return mySizeY;
  }

  //! Returns OpenGL pixel format.
  GLenum virtual Format() const
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
  GLint virtual InternalFormat() const
  {
    if (1 == myComps)
      return GL_R32F;

    if (2 == myComps)
      return GL_RG32F;

    if (3 == myComps)
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

  //! Initializes OpenGL 2D texture and uploads data to the GPU.
  bool Init (const GLint    theSizeX,
             const GLint    theSizeY,
             const GLint    theComps,
             const GLfloat* thePixels = NULL);

  //! Updates OpenGL 2D texture data.
  bool Update (const GLfloat* thePixels);
  
  //! Binds texture to the target.
  void Bind (const GLenum theUnit = GL_TEXTURE0) const;
  
  //! Binds default texture (with zero name) to the target.
  void Unbind (const GLenum theUnit = GL_TEXTURE0) const;

  //! Loads texture data from GPU memory.
  GLubyte* FetchImageData();

protected:

  //! Target of OpenGL 2D texture.
  GLenum myTarget;

  //! Handle (name) of OpenGL 2D texture.
  GLuint myHandle;

  //! Width of OpenGL 2D texture.
  GLint mySizeX;

  //! Height of OpenGL 2D texture.
  GLint mySizeY;

  //! Number of pixel channels (from 1 to 4).
  GLuint myComps;

  //! Texture wrapping mode.
  TextureWrapMode myWrapMode;

  //! Texture filtering mode.
  TextureFilterMode myFiltMode;

};

#endif // HEADER_TEXTURE_2D
