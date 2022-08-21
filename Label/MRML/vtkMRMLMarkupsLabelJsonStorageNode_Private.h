#ifndef VTKINTERNALLABEL_H
#define VTKINTERNALLABEL_H

// Templated on ROI storage node.

#include "vtkMRMLMarkupsLabelNode.h"
#include "vtkMRMLMarkupsLabelJsonStorageNode.h"
#include "vtkMRMLMarkupsJsonStorageNode_Private.h"

//---------------------------------------------------------------------------
class vtkMRMLMarkupsLabelJsonStorageNode::vtkInternalLabel
: public vtkMRMLMarkupsJsonStorageNode::vtkInternal
{
public:
    vtkInternalLabel(vtkMRMLMarkupsLabelJsonStorageNode* external);
    ~vtkInternalLabel();
    
    bool WriteMarkup(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer,
                     vtkMRMLMarkupsNode* markupsNode) override;
    bool UpdateMarkupsNodeFromJsonValue(vtkMRMLMarkupsNode* markupsNode,
                                        rapidjson::Value& markupObject) override;
    bool WriteLabelProperties(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer,
                              vtkMRMLMarkupsLabelNode* markupsNode);
};

#endif // VTKINTERNALLABEL_H
