#include "VoxelData.hpp"

#include <iostream>
#include <memory>

//=======================================================================
// function : VoxelData
// purpose  : Voxel data for uploading to the GPU
//=======================================================================
VoxelData::VoxelData (const int theSizeX,
                      const int theSizeY,
                      const int theSizeZ,
                      const Vec4f& theMinPoint,
                      const Vec4f& theMaxPoint)
: SizeX (theSizeX),
  SizeY (theSizeY),
  SizeZ (theSizeZ)
{
  Data = new float[SizeX * SizeY * SizeZ];

  const Vec4f aSceneSize = theMaxPoint - theMinPoint;

  MinCorner = Vec4f (theMinPoint.x() - 4.f * aSceneSize.x() / (SizeX - 8),
                     theMinPoint.y() - 4.f * aSceneSize.y() / (SizeY - 8),
                     theMinPoint.z() - 4.f * aSceneSize.z() / (SizeZ - 8),
                     1.f);

  MaxCorner = Vec4f (theMaxPoint.x() + 4.f * aSceneSize.x() / (SizeX - 8),
                     theMaxPoint.y() + 4.f * aSceneSize.y() / (SizeY - 8),
                     theMaxPoint.z() + 4.f * aSceneSize.z() / (SizeZ - 8),
                     1.f);

  CellSize = Vec4f ((MaxCorner.x() - MinCorner.x()) / SizeX,
                    (MaxCorner.y() - MinCorner.y()) / SizeY,
                    (MaxCorner.z() - MinCorner.z()) / SizeZ,
                    0.f);
}

//=======================================================================
// function : ~VoxelData
// purpose  : Releases resources of voxel data
//=======================================================================
VoxelData::~VoxelData()
{
  delete [] Data;
}

//=======================================================================
// function : Clear
// purpose  : Clears internal voxel data
//=======================================================================
void VoxelData::Clear()
{
  delete [] Data;

  Data = NULL;
}

//=======================================================================
// function : Value
// purpose  :
//=======================================================================
float VoxelData::Value (const Vec4f& thePoint) const
{
  Vec4f aLocal = (thePoint - MinCorner).cwiseProduct (
    CellSize.cwiseInverse());

  aLocal.x() = floorf (aLocal.x());
  aLocal.y() = floorf (aLocal.y());
  aLocal.z() = floorf (aLocal.z());

  const int aVoxelX = std::min (std::max (static_cast<int> (aLocal.x()), 0), SizeX - 1);
  const int aVoxelY = std::min (std::max (static_cast<int> (aLocal.y()), 0), SizeY - 1);
  const int aVoxelZ = std::min (std::max (static_cast<int> (aLocal.z()), 0), SizeZ - 1);

  return Value (aVoxelX, aVoxelY, aVoxelZ);
}