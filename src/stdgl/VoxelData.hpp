#ifndef HEADER_VOXEL_DATA
#define HEADER_VOXEL_DATA

#include "Types.hpp"

//! Voxel data for uploading to the GPU.
class VoxelData
{

public:

  //! Size of voxel grid in X dimension.
  int SizeX;

  //! Size of voxel grid in Y dimension.
  int SizeY;

  //! Size of voxel grid in Z dimension.
  int SizeZ;

  //! Minimum corner of voxel grid.
  Vec4f MinCorner;

  //! Maximum corner of voxel grid.
  Vec4f MaxCorner;

  //! Size of single voxel in grid.
  Vec4f CellSize;

  //! Voxel data for uploading to the GPU.
  float* Data;

  //! Data range (use UpdateBounds to actualize values).
  Vec2f Range;

public:

  //! Create new voxel data with the given size.
  VoxelData (const int theSizeX,
             const int theSizeY,
             const int theSizeZ,
             const Vec4f& theMinPoint,
             const Vec4f& theMaxPoint);

  //! Releases resources of voxel data.
  ~VoxelData();

public:

  //! Returns voxel data with the given index.
  float& Value (const int theX,
                const int theY,
                const int theZ)
  {
    return Data[theX + (theY + theZ * SizeY) * SizeX];
  }

  //! Returns voxel data with the given index.
  float Value (const int theX,
               const int theY,
               const int theZ) const
  {
    return Data[theX + (theY + theZ * SizeY) * SizeX];
  }

  //! Returns minimum corner of the given voxel.
  Vec4f VoxelMinCorner (const int theX,
                        const int theY,
                        const int theZ) const
  {
    const Vec4f aVoxel (static_cast<float> (theX),
                        static_cast<float> (theY),
                        static_cast<float> (theZ),
                                    0.f);

    return MinCorner + aVoxel.cwiseProduct (CellSize);
  }

  //! Returns maximum corner of the given voxel.
  Vec4f VoxelMaxCorner (const int theX,
                        const int theY,
                        const int theZ) const
  {
    const Vec4f aVoxel (static_cast<float> (theX + 1),
                        static_cast<float> (theY + 1),
                        static_cast<float> (theZ + 1),
                        0.f);

    return MinCorner + aVoxel.cwiseProduct (CellSize);
  }

  //! Updates data bounds.
  void UpdateRange()
  {
    Range = Vec2f (std::numeric_limits<float>::max(),
                  -std::numeric_limits<float>::max());

    for (int anIndex = 0, aCount = SizeX * SizeY * SizeZ; anIndex < aCount; ++anIndex)
    {
      Range.x() = std::min (Range.x(), Data[anIndex]);
      Range.y() = std::max (Range.y(), Data[anIndex]);
    }
  }

  //! Returns voxel data for the given 3D point.
  float Value (const Vec4f& thePoint) const;

  //! Clears internal voxel data.
  void Clear();

public:

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

};

#endif // HEADER_VOXEL_DATA
