/// Storage node for reading/writing properties of vtkMRMLMarkupsLabelNode.
///
/// The implementation is based on vtkMRMLMarkupsROIJsonStorageNode.
///

#ifndef VTKMRMLMARKUPSLABELJSONSTORAGENODE_H
#define VTKMRMLMARKUPSLABELJSONSTORAGENODE_H

// Markups includes
#include "vtkSlicerLabelModuleMRMLExport.h"
#include "vtkMRMLMarkupsJsonStorageNode.h"

class vtkMRMLMarkupsJsonElement;
class vtkMRMLMarkupsJsonWriter;
class vtkMRMLMarkupsNode;

class VTK_SLICER_LABEL_MODULE_MRML_EXPORT vtkMRMLMarkupsLabelJsonStorageNode : public vtkMRMLMarkupsJsonStorageNode
{
public:
    static vtkMRMLMarkupsLabelJsonStorageNode* New();
    vtkTypeMacro(vtkMRMLMarkupsLabelJsonStorageNode, vtkMRMLMarkupsJsonStorageNode);
    
    vtkMRMLNode* CreateNodeInstance() override;
    const char* GetNodeTagName() override { return "MarkupsLabelsJsonStorage"; };
    bool CanReadInReferenceNode(vtkMRMLNode* refNode) override;

protected:
    vtkMRMLMarkupsLabelJsonStorageNode();
    ~vtkMRMLMarkupsLabelJsonStorageNode() override;
    vtkMRMLMarkupsLabelJsonStorageNode(const vtkMRMLMarkupsLabelJsonStorageNode&);
    void operator=(const vtkMRMLMarkupsLabelJsonStorageNode&);

    bool WriteBasicProperties(vtkMRMLMarkupsJsonWriter* writer, vtkMRMLMarkupsNode* markupsNode) override;
    bool UpdateMarkupsNodeFromJsonValue(vtkMRMLMarkupsNode* markupsNode, vtkMRMLMarkupsJsonElement* markupObject) override;
};

#endif // VTKMRMLMARKUPSLABELJSONSTORAGENODE_H
