#ifndef VTKINTERNALSHAPE_H
#define VTKINTERNALSHAPE_H

// Templated on ROI storage node.

#include "vtkMRMLMarkupsShapeNode.h"
#include "vtkMRMLMarkupsShapeJsonStorageNode.h"
#include "vtkMRMLMarkupsJsonStorageNode_Private.h"

//---------------------------------------------------------------------------
class vtkMRMLMarkupsShapeJsonStorageNode::vtkInternalShape
: public vtkMRMLMarkupsJsonStorageNode::vtkInternal
{
public:
    vtkInternalShape(vtkMRMLMarkupsShapeJsonStorageNode* external);
    ~vtkInternalShape();
    
    bool WriteMarkup(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer,
                     vtkMRMLMarkupsNode* markupsNode) override;
    bool UpdateMarkupsNodeFromJsonValue(vtkMRMLMarkupsNode* markupsNode,
                                        rapidjson::Value& markupObject) override;
    bool WriteShapeProperties(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer,
                              vtkMRMLMarkupsShapeNode* markupsNode);
};

#endif // VTKINTERNALSHAPE_H
