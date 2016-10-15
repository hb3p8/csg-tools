#ifndef HEADER_CSG_TREE
#define HEADER_CSG_TREE

#include "Box.hpp"

//! Boolean operation id.
enum CsgOperation
{
  CSG_OP_EMPTY = 100,
  CSG_OP_UNION = 101,
  CSG_OP_INTER = 102,
  CSG_OP_MINUS = 103
};

//! Ids of possible CSG primitives.
enum CsgPrimitiveId
{
  CSG_SPHERE,
  CSG_BOX,
  CSG_CYLINDER
};

//! Describes material of single CSG object.
struct CsgShapeMaterial
{
  //! Diffuse color of CSG object.
  Vec4f Color;

  //! Creates new material of CSG object.
  CsgShapeMaterial (const Vec4f& theColor = Vec4f (1.f, 1.f, 1.f, 1.f))
    : Color (theColor)
  {
    //
  }

};

//! Describes abstract CSG tree node.
class CsgNode
{
public:

  //! Creates new CSG tree node.
  CsgNode() : myIsComplement (false)
  {
    //
  }

  //! Releases resources of CSG tree node.
  virtual ~CsgNode()
  {
    //
  }


public:

  //! Returns height of CSG node.
  int Height() const;

  //! Computes initial bounds of CSG node.
  virtual void InitializeBounds() = 0;

  //! Returns type identifier of CSG node.
  virtual int TypeID() const = 0;

  //! Determines if this CSG node is a leaf.
  virtual bool IsLeaf() const = 0;

  //! Returns total number of CSG primitives.
  virtual int NbPrimitives() const = 0;

  //! Returns total number of CSG operations.
  virtual int NbOperations() const = 0;

  //! Returns deep copy of the given CSG node.
  virtual CsgNode* DeepCopy() const = 0;

  //! Checks if CSG node is convex.
  virtual bool IsConvex() const = 0;

  //! Returns bounding box of CSG node.
  const Box4f& Bounds() const
  {
    return myBounds;
  }

  //! Returns bounding box of CSG node.
  void SetBounds (const Box4f& theBounds)
  {
    myBounds = theBounds;
  }

  //! Checks if CSG tree is complement.
  bool IsComplement() const
  {
    return myIsComplement;
  }

  //! Marks that CSG tree is complement or not.
  void SetComplement (bool theIsComplement)
  {
    myIsComplement = theIsComplement;
  }
  
  //! Converts CSG tree to general form.
  void ToGeneralForm();

  //! Transforms CSG tree to positive form.
  void ToPositiveForm();

  //! Clips the bounds with specified bounding box.
  virtual bool ClipBounds (const Box4f& theBounds);

protected:

  //! Bounds of CSG node.
  Box4f myBounds;

  //! Marks that CSG tree if complement.
  bool myIsComplement;

};


//! Describes specific CSG operation node.
class CsgOperationNode : public CsgNode
{
public:

  //! Creates new CSG operation node.
  CsgOperationNode (CsgOperation theOperation = CSG_OP_UNION)
    : myOperation (theOperation)
  {
    //
  }

  //! Creates new CSG operation node.
  CsgOperationNode (CsgOperation theOperation,
                    CsgNode* theLftNode,
                    CsgNode* theRghNode)
    : myOperation (theOperation)
  {
    myChildren.first = theLftNode;
    myChildren.second = theRghNode;
  }

  //! Releases resources of CSG operation node.
  virtual ~CsgOperationNode()
  {
    delete myChildren.first;
    delete myChildren.second;
  }

public:

  //! Returns type identifier of CSG node.
  virtual int TypeID() const
  {
    return myOperation;
  }

  //! Determines if this CSG node is a leaf.
  virtual bool IsLeaf() const
  {
    return false;
  }

  //! Returns CSG operation to apply.
  CsgOperation Operation() const
  {
    return myOperation;
  }

  //! Sets CSG operation to apply.
  void SetOperation (CsgOperation theOperation)
  {
    myOperation = theOperation;
  }

  //! Returns specified child of CSG node.
  template<int N>
  CsgNode* Child()
  {
    return N == 0 ? myChildren.first : myChildren.second;
  }

  //! Returns specified child of CSG node.
  template<int N>
  const CsgNode* Child() const
  {
    return N == 0 ? myChildren.first : myChildren.second;
  }

  //! Sets specified child of CSG node.
  template<int N>
  void SetChild (CsgNode* theChild)
  {
    (N == 0 ? myChildren.first : myChildren.second) = theChild;
  }

  //! Transposes children of CSG node.
  void SwapChildren()
  {
    std::swap (myChildren.first, myChildren.second);
  }

  //! Checks if CSG node is convex.
  virtual bool IsConvex() const;

  //! Returns total number of CSG primitives.
  virtual int NbPrimitives() const;

  //! Returns total number of CSG operations.
  virtual int NbOperations() const;

  //! Returns deep copy of the given CSG node.
  virtual CsgNode* DeepCopy() const;

  //! Computes initial bounds of CSG node.
  virtual void InitializeBounds();

  //! Performs growing bounds via postorder traversal.
  bool GrowBounds();

  //! Performs clipping bounds via preorder traversal.
  virtual bool ClipBounds (const Box4f& theBounds);

protected:

  //! CSG operation to apply.
  CsgOperation myOperation;

  //! Pair of child CSG nodes.
  std::pair<CsgNode*, CsgNode*> myChildren;

};

//! Describes specific CSG primitive node.
class CsgPrimitiveNode : public CsgNode
{
public:

  //! Creates new CSG primitive node.
  CsgPrimitiveNode (const int theTypeId,
                    const Mat4f& theTransform,
                    const CsgShapeMaterial theMaterial = CsgShapeMaterial())
    : myTypeId (theTypeId),
      myTransform (theTransform),
      myMaterial (theMaterial)
  {
    //
  }

  //! Releases resources of CSG primitive node.
  ~CsgPrimitiveNode()
  {
    //
  }

public:
  
  //! Returns type identifier of CSG node.
  virtual int TypeID() const
  {
    return myTypeId;
  }

  //! Determines if this CSG node is a leaf.
  virtual bool IsLeaf() const
  {
    return true;
  }

  //! Returns total number of CSG primitives.
  virtual int NbPrimitives() const
  {
    return 1;
  }

  //! Returns total number of CSG operations.
  virtual int NbOperations() const
  {
    return 0;
  }

  //! Checks if CSG node is convex.
  virtual bool IsConvex() const
  {
    return true;
  }
  
  //! Returns transformation of CSG node.
  const Mat4f& Transform() const
  {
    return myTransform;
  }

  //! Returns identifier of node type.
  virtual CsgShapeMaterial Material() const
  {
    return myMaterial;
  }

  //! Computes initial bounds of CSG node.
  virtual void InitializeBounds();

  //! Returns deep copy of the given CSG node.
  virtual CsgNode* DeepCopy() const;

protected:

  int myTypeId;
  Mat4f myTransform;
  CsgShapeMaterial myMaterial;

};

#endif // HEADER_CSG_TREE

