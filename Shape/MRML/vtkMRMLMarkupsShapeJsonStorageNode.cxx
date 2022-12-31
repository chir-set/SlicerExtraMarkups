// Templated on ROI storage node.

#include "vtkMRMLMarkupsShapeJsonStorageNode.h"
#include "vtkMRMLMarkupsShapeNode.h"

#include <vtkMRMLMarkupsDisplayNode.h>
#include "vtkMRMLScene.h"
#include "vtkObjectFactory.h"
#include "vtkMRMLMarkupsShapeJsonStorageNode_Private.h"

/*
 * Saving and loading a scene : OK for all shapes.
 * Saving and loading individual markups : bad for all shapes,
 * a sphere shape is always saved or shown.
 */

//----------------------------------------------------------------------------
vtkMRMLMarkupsShapeJsonStorageNode::vtkInternalShape::vtkInternalShape(vtkMRMLMarkupsShapeJsonStorageNode* external)
: vtkMRMLMarkupsJsonStorageNode::vtkInternal(external)
{
}

//----------------------------------------------------------------------------
vtkMRMLMarkupsShapeJsonStorageNode::vtkInternalShape::~vtkInternalShape() = default;

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeJsonStorageNode::vtkInternalShape::WriteMarkup(
    rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer, vtkMRMLMarkupsNode* markupsNode)
{
    bool success = true;
    success = success && this->WriteBasicProperties(writer, markupsNode);
    success = success && this->WriteShapeProperties(writer, vtkMRMLMarkupsShapeNode::SafeDownCast(markupsNode));
    success = success && this->WriteControlPoints(writer, markupsNode);
    success = success && this->WriteMeasurements(writer, markupsNode);
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
bool vtkMRMLMarkupsShapeJsonStorageNode::vtkInternalShape::WriteShapeProperties(
    rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer, vtkMRMLMarkupsShapeNode* shapeNode)
{
    if (!shapeNode)
    {
        return false;
    }
    
    writer.Key("shapeName");
    writer.String(shapeNode->GetShapeNameAsString(shapeNode->GetShapeName()));
    writer.Key("radiusMode");
    writer.String(shapeNode->GetRadiusModeAsString(shapeNode->GetRadiusMode()));
    writer.Key("drawMode2D");
    writer.String(shapeNode->GetDrawMode2DAsString(shapeNode->GetDrawMode2D()));
    writer.Key("resolution");
    writer.Double(shapeNode->GetResolution());
    // Ignoring shapeNode->ResliceNode.
    
    return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeJsonStorageNode::vtkInternalShape::UpdateMarkupsNodeFromJsonValue(vtkMRMLMarkupsNode* markupsNode, 
                                                                                          rapidjson::Value& markupsObject)
{
    if (!markupsNode)
    {
        vtkErrorWithObjectMacro(this->External, "vtkMRMLMarkupsJsonStorageNode::vtkInternalShape::UpdateMarkupsNodeFromJsonDocument failed: invalid markupsNode");
        return false;
    }
    
    MRMLNodeModifyBlocker blocker(markupsNode);
    
    vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(markupsNode);
    
    if (markupsObject.HasMember("shapeName"))
    {
        rapidjson::Value& shapeNameItem = markupsObject["shapeName"];
        std::string shapeName = shapeNameItem.GetString();
        shapeNode->SetShapeName(shapeNode->GetShapeNameFromString(shapeName.c_str()));
    }
    if (markupsObject.HasMember("radiusMode"))
    {
        rapidjson::Value& shapeNameItem = markupsObject["radiusMode"];
        std::string radiusMode = shapeNameItem.GetString();
        shapeNode->SetRadiusMode(shapeNode->GetRadiusModeFromString(radiusMode.c_str()));
    }
    if (markupsObject.HasMember("drawMode2D"))
    {
        rapidjson::Value& shapeNameItem = markupsObject["drawMode2D"];
        std::string drawMode2D = shapeNameItem.GetString();
        shapeNode->SetDrawMode2D(shapeNode->GetDrawMode2DFromString(drawMode2D.c_str()));
    }
    if (markupsObject.HasMember("resolution"))
    {
        rapidjson::Value& shapeNameItem = markupsObject["resolution"];
        double resolution = shapeNameItem.GetDouble();
        shapeNode->SetResolution(resolution);
    }
    
    // Reimplemented to observe JumpToPointEvent.
    shapeNode->CreateDefaultDisplayNodes();
    
    return vtkInternal::UpdateMarkupsNodeFromJsonValue(markupsNode, markupsObject);
}

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsShapeJsonStorageNode);

//----------------------------------------------------------------------------
vtkMRMLMarkupsShapeJsonStorageNode::vtkMRMLMarkupsShapeJsonStorageNode()
{
    this->Internal = new vtkInternalShape(this);
}

//----------------------------------------------------------------------------
vtkMRMLMarkupsShapeJsonStorageNode::~vtkMRMLMarkupsShapeJsonStorageNode() = default;

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeJsonStorageNode::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
    return refNode->IsA("vtkMRMLMarkupsShapeNode");
}
