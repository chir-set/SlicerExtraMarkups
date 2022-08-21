// Templated on ROI storage node.

#include "vtkMRMLMarkupsLabelJsonStorageNode.h"
#include "vtkMRMLMarkupsLabelNode.h"

#include <vtkMRMLMarkupsDisplayNode.h>
#include "vtkMRMLScene.h"
#include "vtkObjectFactory.h"
#include "vtkMRMLMarkupsLabelJsonStorageNode_Private.h"

/*
 * Saving and loading a scene : OK for all firlds.
 * Saving and loading individual markups : OK for tip mode, bad for label text;
 * default 'Label' value is always saved or shown.
 */

//----------------------------------------------------------------------------
vtkMRMLMarkupsLabelJsonStorageNode::vtkInternalLabel::vtkInternalLabel(vtkMRMLMarkupsLabelJsonStorageNode* external)
: vtkMRMLMarkupsJsonStorageNode::vtkInternal(external)
{
}

//----------------------------------------------------------------------------
vtkMRMLMarkupsLabelJsonStorageNode::vtkInternalLabel::~vtkInternalLabel() = default;

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsLabelJsonStorageNode::vtkInternalLabel::WriteMarkup(
    rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer, vtkMRMLMarkupsNode* markupsNode)
{
    bool success = true;
    success = success && this->WriteBasicProperties(writer, markupsNode);
    success = success && this->WriteLabelProperties(writer, vtkMRMLMarkupsLabelNode::SafeDownCast(markupsNode));
    success = success && this->WriteControlPoints(writer, markupsNode);
    //success = success && this->WriteMeasurements(writer, markupsNode);
    if (success)
    {
        vtkMRMLMarkupsDisplayNode* displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(markupsNode->GetDisplayNode());
        if (displayNode)
        {
            success = success && this->WriteDisplayProperties(writer, displayNode);
        }
    }
    
    return success;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsLabelJsonStorageNode::vtkInternalLabel::WriteLabelProperties(
    rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer, vtkMRMLMarkupsLabelNode* labelNode)
{
    if (!labelNode)
    {
        return false;
    }
    
    writer.Key("tipDimensionMode3D");
    writer.String(labelNode->GetTipDimensionMode3DAsString(labelNode->GetTipDimensionMode3D()));
    // Don't use 'label'; JSON keys already contain a 'label' key, though not at the same level.
    writer.Key("labelText");
    writer.String(labelNode->GetLabel().toStdString().c_str());
    
    return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsLabelJsonStorageNode::vtkInternalLabel::UpdateMarkupsNodeFromJsonValue(vtkMRMLMarkupsNode* markupsNode, 
                                                                                          rapidjson::Value& markupsObject)
{
    if (!markupsNode)
    {
        vtkErrorWithObjectMacro(this->External, "vtkMRMLMarkupsJsonStorageNode::vtkInternalLabel::UpdateMarkupsNodeFromJsonDocument failed: invalid markupsNode");
        return false;
    }
    
    MRMLNodeModifyBlocker blocker(markupsNode);
    
    vtkMRMLMarkupsLabelNode* labelNode = vtkMRMLMarkupsLabelNode::SafeDownCast(markupsNode);
    
    if (markupsObject.HasMember("tipDimensionMode3D"))
    {
        rapidjson::Value& labelNameItem = markupsObject["tipDimensionMode3D"];
        std::string tipDimensionMode3D = labelNameItem.GetString();
        labelNode->SetTipDimensionMode3D(labelNode->GetTipDimensionMode3DFromString(tipDimensionMode3D.c_str()));
    }
    if (markupsObject.HasMember("labelText"))
    {
        rapidjson::Value& labelNameItem = markupsObject["labelText"];
        std::string label = labelNameItem.GetString();
        labelNode->SetLabel(QString(label.c_str()));
    }
    
    return vtkInternal::UpdateMarkupsNodeFromJsonValue(markupsNode, markupsObject);
}

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsLabelJsonStorageNode);

//----------------------------------------------------------------------------
vtkMRMLMarkupsLabelJsonStorageNode::vtkMRMLMarkupsLabelJsonStorageNode()
{
    this->Internal = new vtkInternalLabel(this);
}

//----------------------------------------------------------------------------
vtkMRMLMarkupsLabelJsonStorageNode::~vtkMRMLMarkupsLabelJsonStorageNode() = default;

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsLabelJsonStorageNode::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
    return refNode->IsA("vtkMRMLMarkupsLabelNode");
}
