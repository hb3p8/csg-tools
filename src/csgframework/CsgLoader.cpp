#include "CsgLoader.hpp"

#include <Eigen/Geometry>

#include <iostream>

namespace {

  CsgNode* loadNode (const json11::Json theData, const Mat4f& theTransform);

  CsgNode* collectNodes (const CsgOperation theOp,
                         const json11::Json theData,
                         const int theStartIndex,
                         const Mat4f& theTransform) {

    auto& anItems = theData.array_items();

    size_t anActualSize = anItems.size() - theStartIndex;

    if (anActualSize == 0) {
      throw std::runtime_error ("The range should contain at least one element");
    }
    else if (anActualSize == 1) {
      return loadNode (anItems[theStartIndex], theTransform);
    }

    return new CsgOperationNode (theOp,
                                 loadNode (anItems[theStartIndex], theTransform),
                                 collectNodes (theOp, theData, theStartIndex + 1, theTransform));
  }

  CsgNode* loadNode (const json11::Json theData, const Mat4f& theTransform) {

    if (theData.is_null()) {
      throw std::runtime_error ("Unexpected NULL object");
    }

    if (theData.is_array()) {
      return loadNode (theData.array_items()[0], theTransform);
    }

    if (!theData.is_object()) {
      throw std::runtime_error ("Dictionary expected: " + theData.dump());
    }

    std::string aType = theData["type"].string_value();

    if (aType == "CSG file") {

      // theStream << theIndent << "# " << theData["version-name"].string_value() <<
      //                           " " << theData["version-major"].dump() <<
      //                           "." << theData["version-minor"].dump() << std::endl;

      return loadNode (theData["contents"], theTransform);
    }
    else if (aType == "group") {

      return collectNodes (CSG_OP_UNION, theData["objects"], 0, theTransform);
    }
    else if (aType == "multmatrix") {

      Mat4f aMatrix = Mat4f::Identity();

      // OpenScad compatibility matrix
      if (theData["properties"].is_array()) {

        for (int i = 0; i < 4; ++i) {
          auto anInputRow = theData["properties"][i];
          aMatrix.row (i) = Vec4f ((float)anInputRow[0].number_value(),
                                   (float)anInputRow[1].number_value(),
                                   (float)anInputRow[2].number_value(),
                                   (float)anInputRow[3].number_value());
        }
      }
      else {
        // TODO: fetch matrix
      }

      return collectNodes (CSG_OP_UNION, theData["objects"], 0, theTransform * aMatrix);
    }
    else if (aType == "union") {
      CsgNode* aFirstNode = loadNode (theData["objects"][0], theTransform);

      // OpenScad compatibility
      if (theData["objects"].array_items().size() == 1) {
        return aFirstNode;
      }

      CsgNode* aSecondNode = collectNodes (CSG_OP_UNION, theData["objects"], 1, theTransform);
      return new CsgOperationNode (CSG_OP_UNION, aFirstNode, aSecondNode);
    }
    else if (aType == "difference") {
      CsgNode* aFirstNode = loadNode (theData["objects"][0], theTransform);
      CsgNode* aSecondNode = collectNodes (CSG_OP_UNION, theData["objects"], 1, theTransform);
      return new CsgOperationNode (CSG_OP_MINUS, aFirstNode, aSecondNode);
    }
    else if (aType == "intersection") {
      CsgNode* aFirstNode = loadNode (theData["objects"][0], theTransform);
      CsgNode* aSecondNode = collectNodes (CSG_OP_INTER, theData["objects"], 1, theTransform);
      return new CsgOperationNode (CSG_OP_INTER, aFirstNode, aSecondNode);
    }
    // else if (aType == "smin") {
    // }
    else if (aType == "cube") {

      auto aCubeSize = theData["properties"]["size"];
      
      Eigen::Affine3f aBoxTransform;
      if (aCubeSize.is_null()) {
        aBoxTransform = Eigen::Scaling (1.0f, 1.0f, 1.0f);
      }
      else {
        aBoxTransform = Eigen::Scaling ((float)aCubeSize[0].number_value(),
                                        (float)aCubeSize[1].number_value(),
                                        (float)aCubeSize[2].number_value());
      }
      return new CsgPrimitiveNode (CSG_BOX, theTransform * aBoxTransform.matrix());
    }
    else if (aType == "sphere") {

      double aRadius = theData["properties"]["r"].number_value();

      Eigen::Affine3f aSphereTransform;
      aSphereTransform = Eigen::Scaling ((float)(aRadius > 0.0 ? aRadius : 1.0));
      return new CsgPrimitiveNode (CSG_SPHERE, theTransform * aSphereTransform.matrix());
    }
    // else if (aType == "cylinder") {
    // }
    // else if (aType == "cone") {
    // }
    else {
      throw std::runtime_error ("Unknown object type: " + theData["type"].dump());
    }
  }
}

CsgNode* CsgLoader::LoadTree (const json11::Json theSerializedTree)
{
  Mat4f theTransform = Mat4f::Identity();
  return loadNode (theSerializedTree, theTransform);
}