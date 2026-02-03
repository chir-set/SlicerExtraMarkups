#include "vtkMRMLMarkupsLabelJsonStorageNode.h"

#include "vtkMRMLJsonElement.h"
#include "vtkMRMLMarkupsLabelNode.h"
#include "vtkMRMLScene.h"

#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsLabelJsonStorageNode);

//----------------------------------------------------------------------------
vtkMRMLMarkupsLabelJsonStorageNode::vtkMRMLMarkupsLabelJsonStorageNode()
{
}

//----------------------------------------------------------------------------
vtkMRMLMarkupsLabelJsonStorageNode::~vtkMRMLMarkupsLabelJsonStorageNode() = default;

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsLabelJsonStorageNode::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
    return refNode->IsA("vtkMRMLMarkupsLabelNode");
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsLabelJsonStorageNode::WriteBasicProperties(
  vtkMRMLJsonWriter* writer, vtkMRMLMarkupsNode* markupsNode)
{
  if (!vtkMRMLMarkupsJsonStorageNode::WriteBasicProperties(writer, markupsNode))
    {
    return false;
    }

  vtkMRMLMarkupsLabelNode* labelNode = vtkMRMLMarkupsLabelNode::SafeDownCast(markupsNode);
  if (!labelNode)
    {
    vtkErrorWithObjectMacro(this, "vtkMRMLMarkupsLabelNode::WriteBasicProperties failed: invalid markupsNode");
    return false;
    }

  writer->WriteStringProperty("tipDimensionMode3D", labelNode->GetTipDimensionMode3DAsString(labelNode->GetTipDimensionMode3D()));
  writer->WriteStringProperty("labelText", labelNode->GetLabel());
  writer->WriteIntProperty("labelLocation", labelNode->GetLabelLocation());

  return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsLabelJsonStorageNode::UpdateMarkupsNodeFromJsonValue(vtkMRMLMarkupsNode* markupsNode, vtkMRMLJsonElement* markupsObject)
{
  vtkMRMLMarkupsLabelNode* labelNode = vtkMRMLMarkupsLabelNode::SafeDownCast(markupsNode);
  if (!labelNode)
    {
    vtkErrorWithObjectMacro(this, "vtkMRMLMarkupsLabelJsonStorageNode::UpdateMarkupsNodeFromJsonValue failed: invalid markupsNode");
    return false;
    }

  MRMLNodeModifyBlocker blocker(labelNode);
  if (markupsObject->HasMember("tipDimensionMode3D"))
    {
    labelNode->SetTipDimensionMode3D(labelNode->GetTipDimensionMode3DFromString(markupsObject->GetStringProperty("tipDimensionMode3D").c_str()));
    }
  if (markupsObject->HasMember("labelText"))
    {
    labelNode->SetLabel(markupsObject->GetStringProperty("labelText").c_str());
    }
  if (markupsObject->HasMember("labelLocation"))
    {
      labelNode->SetLabelLocation(markupsObject->GetIntProperty("labelLocation"));
    }

  return Superclass::UpdateMarkupsNodeFromJsonValue(markupsNode, markupsObject);
}
