#ifndef HEADER_CSG_LOADER
#define HEADER_CSG_LOADER

#include <csgparser.hpp>

#include <CsgTree.hpp>

class CsgLoader
{
private:

  CsgLoader();

public:

  //! Loads CSG-tree from JSON.
  static CsgNode* LoadTree (const json11::Json theSerializedTree);

};

#endif // HEADER_CSG_LOADER

