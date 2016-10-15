#ifndef HEADER_TYPES
#define HEADER_TYPES

#include <Eigen/Core>
#include <Eigen/StdVector>

typedef Eigen::Matrix3f Mat3f;
typedef Eigen::Matrix4f Mat4f;

typedef Eigen::Vector2f Vec2f;
typedef Eigen::Vector3f Vec3f;
typedef Eigen::Vector4f Vec4f;

typedef Eigen::Vector2i Vec2i;
typedef Eigen::Vector3i Vec3i;
typedef Eigen::Vector4i Vec4i;

typedef std::vector< int > Array1i;
typedef std::vector<float> Array1f;

typedef std::vector<Vec2i, Eigen::aligned_allocator<Vec2i> > Array2i;
typedef std::vector<Vec3i, Eigen::aligned_allocator<Vec3i> > Array3i;
typedef std::vector<Vec4i, Eigen::aligned_allocator<Vec4i> > Array4i;

typedef std::vector<Vec2f, Eigen::aligned_allocator<Vec2f> > Array2f;
typedef std::vector<Vec3f, Eigen::aligned_allocator<Vec3f> > Array3f;
typedef std::vector<Vec4f, Eigen::aligned_allocator<Vec4f> > Array4f;

#endif // HEADER_TYPES
