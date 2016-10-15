#include "Texture2D.hpp"

#include <cassert>
#include <memory>

// =======================================================================
// function : Setup
// purpose  :
// =======================================================================
void TextureWrapMode::Setup (const GLenum theTarget) const
{
  glTexParameteri (theTarget, GL_TEXTURE_WRAP_S, WrapS);
  glTexParameteri (theTarget, GL_TEXTURE_WRAP_T, WrapT);
  glTexParameteri (theTarget, GL_TEXTURE_WRAP_R, WrapR);
}

// =======================================================================
// function : Setup
// purpose  :
// =======================================================================
void TextureFilterMode::Setup (const GLenum theTarget) const
{
  glTexParameteri (theTarget, GL_TEXTURE_MIN_FILTER, MinFilter);
  glTexParameteri (theTarget, GL_TEXTURE_MAG_FILTER, MagFilter);
}

// =======================================================================
// function : Texture2D
// purpose  :
// =======================================================================
Texture2D::Texture2D (const GLenum theTarget)
: mySizeX (0),
  mySizeY (0)
{
  myTarget = theTarget;

  glGenTextures (1, &myHandle);
}

// =======================================================================
// function : ~Texture2D
// purpose  :
// =======================================================================
Texture2D::~Texture2D()
{
  glDeleteTextures (1, &myHandle);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool Texture2D::Init (const GLint    theSizeX,
                      const GLint    theSizeY,
                      const GLint    theComps,
                      const GLfloat* thePixels)
{
  Bind (GL_TEXTURE0);

  myWrapMode.Setup (myTarget);
  myFiltMode.Setup (myTarget);

  glGetError();

  mySizeX = theSizeX;
  mySizeY = theSizeY;
  myComps = theComps;

  glTexImage2D (myTarget, 0, InternalFormat(),
    mySizeX, mySizeY, 0, Format(), GL_FLOAT, thePixels);

  return glGetError() == GL_NO_ERROR;
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
bool Texture2D::Update (const GLfloat* thePixels)
{
  Bind (GL_TEXTURE0);

  glGetError();

  glTexSubImage2D (myTarget, 0, 0, 0,
    mySizeX, mySizeY, Format(), GL_FLOAT, thePixels);

  return glGetError() == GL_NO_ERROR;
}

// =======================================================================
// function : Bind
// purpose  :
// =======================================================================
void Texture2D::Bind (const GLenum theUnit) const
{
  glActiveTexture (theUnit);

  glBindTexture (myTarget, myHandle);
}

// =======================================================================
// function : Unbind
// purpose  :
// =======================================================================
void Texture2D::Unbind (const GLenum theUnit) const
{
  glActiveTexture (theUnit);

  glBindTexture (myTarget, 0);
}

// =======================================================================
// function : FetchData
// purpose  :
// =======================================================================
GLubyte* Texture2D::FetchImageData()
{
  GLubyte* aPixels = new GLubyte[mySizeX * mySizeY * 4];

  glGetTexImage (myTarget,
    0, Format(), GL_UNSIGNED_BYTE, aPixels);

  return aPixels;
}
