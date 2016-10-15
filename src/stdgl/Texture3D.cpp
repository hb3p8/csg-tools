#include "Texture3D.hpp"

// =======================================================================
// function : Texture3D
// purpose  :
// =======================================================================
Texture3D::Texture3D (const GLuint theChannels)
: mySizeX (0),
  mySizeY (0),
  mySizeZ (0),
  myChannels (theChannels)
{
  myTarget = GL_TEXTURE_3D;

  glGenTextures (1, &myHandle);
}

// =======================================================================
// function : ~Texture3D
// purpose  :
// =======================================================================
Texture3D::~Texture3D()
{
  glDeleteTextures (1, &myHandle);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool Texture3D::Init (const GLint    theSizeX,
                      const GLint    theSizeY,
                      const GLint    theSizeZ,
                      const GLfloat* thePixels)
{
  Bind (GL_TEXTURE0);

  myWrapMode.Setup (myTarget);
  myFiltMode.Setup (myTarget);

  glGetError();

  mySizeX = theSizeX;
  mySizeY = theSizeY;
  mySizeZ = theSizeZ;

  glTexImage3D (myTarget, 0, InternalFormat(),
    mySizeX, mySizeY, mySizeZ, 0, Format(), GL_FLOAT, thePixels);

  return glGetError() == GL_NO_ERROR;
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
bool Texture3D::Update (const GLfloat* thePixels)
{
  Bind (GL_TEXTURE0);

  glGetError();

  glTexSubImage3D (myTarget, 0, 0, 0, 0,
    mySizeX, mySizeY, mySizeZ, Format(), GL_FLOAT, thePixels);

  return glGetError() == GL_NO_ERROR;
}

// =======================================================================
// function : Bind
// purpose  :
// =======================================================================
void Texture3D::Bind (const GLenum theUnit) const
{
  glActiveTexture (theUnit);

  glBindTexture (myTarget, myHandle);
}

// =======================================================================
// function : Unbind
// purpose  :
// =======================================================================
void Texture3D::Unbind (const GLenum theUnit) const
{
  glActiveTexture (theUnit);

  glBindTexture (myTarget, 0);
}