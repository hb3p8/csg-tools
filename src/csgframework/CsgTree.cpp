#include "CsgTree.hpp"

namespace tools
{
  //=======================================================================
  //function : Combine
  //purpose  :
  //=======================================================================
  Box4f Combine (const Box4f& theBox1, const Box4f& theBox2)
  {
    if (!theBox1.IsValid())
      return theBox2;

    if (!theBox2.IsValid())
      return theBox1;

    return Box4f (theBox1.CornerMin().cwiseMin (theBox2.CornerMin()),
                  theBox1.CornerMax().cwiseMax (theBox2.CornerMax()));
  }

  //=======================================================================
  //function : Intersect
  //purpose  :
  //=======================================================================
  Box4f Intersect (const Box4f& theBox1, const Box4f& theBox2)
  {
    Box4f aResult; // <-- empty

    if (!theBox1.IsValid() || !theBox2.IsValid())
      return aResult;

    // Check for no overlap
    if (theBox1.CornerMin().x() > theBox2.CornerMax().x() ||
        theBox1.CornerMax().x() < theBox2.CornerMin().x() ||
        theBox1.CornerMin().y() > theBox2.CornerMax().y() ||
        theBox1.CornerMax().y() < theBox2.CornerMin().y() ||
        theBox1.CornerMin().z() > theBox2.CornerMax().z() ||
        theBox1.CornerMax().z() < theBox2.CornerMin().z())
    {
      return aResult;
    }
    else // Compute AABB of intersection
    {
      aResult = Box4f (theBox1.CornerMin().cwiseMax (theBox2.CornerMin()),
                       theBox1.CornerMax().cwiseMin (theBox2.CornerMax()));
    }

    return aResult;
  }

  //! Describes operation to apply to CSG tree node.
  enum NodeAction
  {
    ACTION_NONE,
    ACTION_COMP
  };

  // =======================================================================
  // function : RemoveDifferences
  // purpose  :
  // =======================================================================
  void RemoveDifferences (CsgNode* theNode, const NodeAction theAction)
  {
    if (theNode->IsLeaf())
    {
      static_cast<CsgPrimitiveNode*> (theNode)->SetComplement (
        theAction == ACTION_COMP);      
    }
    else
    {
      CsgOperationNode* aNode =
        static_cast<CsgOperationNode*> (theNode);

      if (aNode->Operation() == CSG_OP_MINUS)
      {
        aNode->SetOperation (theAction == ACTION_COMP ?
          CSG_OP_UNION : CSG_OP_INTER);

        RemoveDifferences (aNode->Child<0>(),
          theAction == ACTION_COMP ? ACTION_COMP : ACTION_NONE);
        RemoveDifferences (aNode->Child<1>(),
          theAction == ACTION_COMP ? ACTION_NONE : ACTION_COMP);
      }
      else
      {
        if (theAction == ACTION_COMP)
        {
          aNode->SetOperation (aNode->Operation() == CSG_OP_INTER ?
            CSG_OP_UNION : CSG_OP_INTER);
        }

        RemoveDifferences (aNode->Child<0>(), theAction);
        RemoveDifferences (aNode->Child<1>(), theAction);
      }
    }
  }
}

// =======================================================================
// function : Height
// purpose  :
// =======================================================================
int CsgNode::Height() const
{
  if (IsLeaf())
  {
    return 0;
  }

  const CsgOperationNode* anOperation =
    static_cast<const CsgOperationNode*> (this);

  return std::max (anOperation->Child<0>()->Height(),
                   anOperation->Child<1>()->Height()) + 1;
}

// =======================================================================
// function : ClipBounds
// purpose  :
// =======================================================================
bool CsgNode::ClipBounds (const Box4f& theBounds)
{
  float aBaseArea = myBounds.Area();

  myBounds = tools::Intersect (myBounds, theBounds);

  return myBounds.Area() < aBaseArea;
}

// =======================================================================
// function : ToPositiveForm
// purpose  :
// =======================================================================
void CsgNode::ToPositiveForm()
{
  tools::RemoveDifferences (this, tools::ACTION_NONE);
}

// =======================================================================
// function : ToGeneralForm
// purpose  :
// =======================================================================
void CsgNode::ToGeneralForm()
{
  if (IsLeaf())
  {
    return;
  }

  CsgOperationNode* aNode =
    static_cast<CsgOperationNode*> (this);

  CsgNode* aLftChild = aNode->Child<0>();
  CsgNode* aRghChild = aNode->Child<1>();

  if (!aLftChild->IsLeaf())
  {
    aLftChild->ToGeneralForm();
  }

  if (!aRghChild->IsLeaf())
  {
    aRghChild->ToGeneralForm();
  }

  if (aLftChild->IsComplement() || aRghChild->IsComplement())
  {
    if (aLftChild->IsComplement())
    {
      if (aRghChild->IsComplement())
      {
        aNode->SetOperation (aNode->Operation() == CSG_OP_UNION ?
          CSG_OP_INTER : CSG_OP_UNION);

        aNode->SetComplement (true);
      }
      else
      {
        if (aNode->Operation() == CSG_OP_INTER)
        {
          aNode->SwapChildren();
        }
        else
        {
          aNode->SetComplement (true);
        }

        aNode->SetOperation (CSG_OP_MINUS);
      }
    }
    else
    {
      if (aNode->Operation() != CSG_OP_INTER)
      {
        aNode->SwapChildren();

        aNode->SetComplement (true);
      }

      aNode->SetOperation (CSG_OP_MINUS);
    }

    aLftChild->SetComplement (false);
    aRghChild->SetComplement (false);
  }
}

// =======================================================================
// function : IsConvex
// purpose  :
// =======================================================================
bool CsgOperationNode::IsConvex() const
{
  if (myOperation != CSG_OP_INTER)
  {
    return false;
  }
  
  return Child<0>()->IsConvex() && Child<1>()->IsConvex();
}

// =======================================================================
// function : NbPrimitives
// purpose  :
// =======================================================================
int CsgOperationNode::NbPrimitives() const
{
  return Child<0>()->NbPrimitives() + Child<1>()->NbPrimitives();
}

// =======================================================================
// function : NbOperations
// purpose  :
// =======================================================================
int CsgOperationNode::NbOperations() const
{
  int aResult = 1;

  aResult += Child<0>()->NbOperations();
  aResult += Child<1>()->NbOperations();

  return aResult;
}

// =======================================================================
// function : DeepCopy
// purpose  :
// =======================================================================
CsgNode* CsgOperationNode::DeepCopy() const
{
  CsgOperationNode* aCopy = new CsgOperationNode (myOperation);

  aCopy->SetChild<0> (Child<0>()->DeepCopy());
  aCopy->SetChild<1> (Child<1>()->DeepCopy());

  return aCopy;
}

// =======================================================================
// function : UpdateBounds
// purpose  :
// =======================================================================
void CsgOperationNode::InitializeBounds()
{
  Child<0>()->InitializeBounds();
  Child<1>()->InitializeBounds();

  if (myOperation == CSG_OP_UNION)
  {
    myBounds = tools::Combine (Child<0>()->Bounds(), Child<1>()->Bounds());
  }
  else if (myOperation == CSG_OP_INTER)
  {
    myBounds = tools::Intersect (Child<0>()->Bounds(), Child<1>()->Bounds());
  }
  else if (myOperation == CSG_OP_MINUS)
  {
    myBounds = Child<0>()->Bounds();
  }
}

// =======================================================================
// function : GrowBounds
// purpose  :
// =======================================================================
bool CsgOperationNode::GrowBounds()
{
  bool aResult = false;

  if (!Child<0>()->IsLeaf())
  {
    aResult |= static_cast<CsgOperationNode*> (Child<0>())->GrowBounds();
  }

  if (!Child<1>()->IsLeaf())
  {
    aResult |= static_cast<CsgOperationNode*> (Child<1>())->GrowBounds();
  }

  float aBaseArea = myBounds.Area();

  if (myOperation == CSG_OP_UNION)
  {
    myBounds = tools::Combine (Child<0>()->Bounds(), Child<1>()->Bounds());
  }
  else if (myOperation == CSG_OP_INTER)
  {
    myBounds = tools::Intersect (Child<0>()->Bounds(), Child<1>()->Bounds());
  }
  else // minus
  {
    myBounds = Child<0>()->Bounds();
  }

  aResult |= myBounds.Area() < aBaseArea;

  return aResult;
}

// =======================================================================
// function : ClipBounds
// purpose  :
// =======================================================================
bool CsgOperationNode::ClipBounds (const Box4f& theBounds)
{
  bool aResult = CsgNode::ClipBounds (theBounds);

  aResult |= Child<0>()->ClipBounds (myBounds);
  aResult |= Child<1>()->ClipBounds (myBounds);

  return aResult;
}

// =======================================================================
// function : DeepCopy
// purpose  :
// =======================================================================
CsgNode* CsgPrimitiveNode::DeepCopy() const
{
  CsgPrimitiveNode* aCopy = new CsgPrimitiveNode (myTypeId, myTransform, myMaterial);

  aCopy->SetComplement (myIsComplement);

  return aCopy;
}

// =======================================================================
// function : InitializeBounds
// purpose  :
// =======================================================================
void CsgPrimitiveNode::InitializeBounds()
{
  if (!myIsComplement)
  {
    static const Box4f aLocalBounds (
      Vec4f (-1.f, -1.f, -1.f, 1.f),
      Vec4f ( 1.f,  1.f,  1.f, 1.f));

    for (int aX = 0; aX < 2; ++aX)
    {
      for (int aY = 0; aY < 2; ++aY)
      {
        for (int aZ = 0; aZ < 2; ++aZ)
        {
          Vec4f aCorner (
            aX == 0 ? aLocalBounds.CornerMin().x() : aLocalBounds.CornerMax().x(),
            aY == 0 ? aLocalBounds.CornerMin().y() : aLocalBounds.CornerMax().y(),
            aZ == 0 ? aLocalBounds.CornerMin().z() : aLocalBounds.CornerMax().z(),
            1.f);

          aCorner = myTransform * aCorner;

          myBounds.Add (aCorner);
        }
      }
    }
  }
  else
  {
    myBounds = Box4f (
      -Vec4f (std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max(),
              1.f),
       Vec4f (std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max(),
              1.f));
  }
}
