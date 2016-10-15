#include "Box.hpp"

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void Box4f::Clear()
{
  myIsInited = false;
}

// =======================================================================
// function : IsValid
// purpose  :
// =======================================================================
bool Box4f::IsValid() const
{
  return myIsInited;
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void Box4f::Add (const Vec4f& thePoint)
{
  if (!myIsInited)
  {
    myMinPoint = thePoint;
    myMaxPoint = thePoint;

    myIsInited = true;
  }
  else
  {
    myMinPoint = myMinPoint.cwiseMin (thePoint);
    myMaxPoint = myMaxPoint.cwiseMax (thePoint);
  }
}

// =======================================================================
// function : Combine
// purpose  :
// =======================================================================
void Box4f::Combine (const Box4f& theBox)
{
  if (theBox.myIsInited)
  {
    if (!myIsInited)
    {
      myMinPoint = theBox.myMinPoint;
      myMaxPoint = theBox.myMaxPoint;

      myIsInited = true;
    }
    else
    {
      myMinPoint = myMinPoint.cwiseMin (theBox.myMinPoint);
      myMaxPoint = myMaxPoint.cwiseMax (theBox.myMaxPoint);
    }
  }
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
Vec4f Box4f::Size() const
{
  return myMaxPoint - myMinPoint;
}

// =======================================================================
// function : Center
// purpose  :
// =======================================================================
Vec4f Box4f::Center() const
{
  return (myMinPoint + myMaxPoint) * 0.5f;
}

// =======================================================================
// function : Center
// purpose  :
// =======================================================================
float Box4f::Center (const int theAxis) const
{
  return (myMinPoint[theAxis] + myMaxPoint[theAxis]) * 0.5f;
}

// =======================================================================
// function : Area
// purpose  :
// =======================================================================
float Box4f::Area() const
{
  Vec4f aSize = Size();

  return aSize.x() * aSize.y() +
         aSize.y() * aSize.z() +
         aSize.x() * aSize.z();
}
