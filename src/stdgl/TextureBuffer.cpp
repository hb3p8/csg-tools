#include "TextureBuffer.hpp"

#include <GL/glext.h>

#include <cassert>
#include <memory>

// =======================================================================
// function : TextureBuffer
// purpose  :
// =======================================================================
TextureBuffer::TextureBuffer (TextureBufferType theBufferType) : myBufferType (theBufferType)
{
  myTarget = GL_TEXTURE_BUFFER;

  glGenBuffers (1, &myHandleVBO);

  glGenTextures (1, &myHandleTBO);
}

// =======================================================================
// function : ~TextureBuffer
// purpose  :
// =======================================================================
TextureBuffer::~TextureBuffer()
{
  glDeleteTextures (1, &myHandleTBO);

  glDeleteBuffers (1, &myHandleVBO);
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
bool TextureBuffer::init (const GLint   theSize,
                                      const GLint   theComps,
                                      const GLvoid* theData)
{
  if (theComps < 1
   || theComps > 4)
  {
    // unsupported format
    return false;
  }

  myComps = theComps;

  glBindBuffer (GL_ARRAY_BUFFER, myHandleVBO);

  glBufferData (GL_ARRAY_BUFFER,
    theSize * theComps * sizeof (int), theData, GL_STATIC_DRAW);

  if (glGetError() != GL_NO_ERROR) // GL_OUT_OF_MEMORY
  {
    return false;
  }

  Bind (GL_TEXTURE0);
  glTexBuffer (myTarget, InternalFormat(), myHandleVBO);
  Unbind (GL_TEXTURE0);

  glBindBuffer (GL_ARRAY_BUFFER, 0);

  return glGetError() == GL_NO_ERROR;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool TextureBuffer::Init (const GLint    theSize,
                                      const GLint    theComps,
                                      const GLfloat* theData)
{
  return init (theSize, theComps, theData);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool TextureBuffer::Init (const GLint  theSize,
                                      const GLint  theComps,
                                      const GLint* theData)
{
  return init (theSize, theComps, theData);
}

// =======================================================================
// function : Bind
// purpose  :
// =======================================================================
void TextureBuffer::Bind (const GLenum theUnit) const
{
  glActiveTexture (theUnit);

  glBindTexture (myTarget, myHandleTBO);
}

// =======================================================================
// function : Unbind
// purpose  :
// =======================================================================
void TextureBuffer::Unbind (const GLenum theUnit) const
{
  glActiveTexture (theUnit);

  glBindTexture (myTarget, 0);
}

// =======================================================================
// function : BindImage
// purpose  :
// =======================================================================
void TextureBuffer::BindImage (const GLuint theUnit, const GLenum theAccess) const
{
  glBindImageTextureEXT (theUnit,
    myHandleTBO, 0, GL_TRUE, 0, theAccess, InternalFormat());
}

// =======================================================================
// function : UnbindImage
// purpose  :
// =======================================================================
void TextureBuffer::UnbindImage (const GLuint theUnit) const
{
  glBindImageTextureEXT (theUnit,
    0, 0, GL_TRUE, 0, GL_READ_WRITE, InternalFormat());
}