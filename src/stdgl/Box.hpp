#ifndef HEADER_BOX
#define HEADER_BOX

#include "Types.hpp"

//! Defines axis aligned bounding box (AABB).
class Box4f
{
public:

  //! Creates uninitialized bounding box.
  Box4f() : myIsInited (false) {}

  //! Creates bounding box of given point.
  Box4f (const Vec4f& thePoint)
  : myMinPoint (thePoint),
    myMaxPoint (thePoint),
    myIsInited (true)
  {
    //
  }

  //! Creates copy of another bounding box.
  Box4f (const Box4f& theBox)
  : myMinPoint (theBox.myMinPoint),
    myMaxPoint (theBox.myMaxPoint),
    myIsInited (theBox.myIsInited)
  {
    //
  }

  //! Creates bounding box from corner points.
  Box4f (const Vec4f& theMinPoint,
       const Vec4f& theMaxPoint)
  : myMinPoint (theMinPoint),
    myMaxPoint (theMaxPoint),
    myIsInited (true)
  {
    //
  }

public:

  //! Clears bounding box.
  void Clear();

  //! Is bounding box valid?
  bool IsValid() const;

  //! Appends new point to the bounding box.
  void Add (const Vec4f& thePoint);

  //! Combines bounding box with another one.
  void Combine (const Box4f& theBox);

  //! Returns minimum point of bounding box.
  const Vec4f& CornerMin() const { return myMinPoint; }

  //! Returns maximum point of bounding box.
  const Vec4f& CornerMax() const { return myMaxPoint; }

  //! Returns minimum point of bounding box.
  Vec4f& CornerMin() { return myMinPoint; }

  //! Returns maximum point of bounding box.
  Vec4f& CornerMax() { return myMaxPoint; }

  //! Returns diagonal of bounding box.
  Vec4f Size() const;

  //! Returns center of bounding box.
  Vec4f Center() const;

  //! Returns center of bounding box along the given axis.
  float Center (const int theAxis) const;

  //! Returns area of the box.
  float Area() const;

protected:

  //! Minimum point of bounding box
  Vec4f myMinPoint;

  //! Maximum point of bounding box
  Vec4f myMaxPoint;

  //! Is bounding box initialized?
  bool myIsInited;

public:

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

};

#endif // HEADER_BOX