#ifndef VTKMRMLMARKUPSLABELJSONSTORAGENODE_H
#define VTKMRMLMARKUPSLABELJSONSTORAGENODE_H

// Templated on ROI storage node.

// Markups includes
#include "vtkSlicerMarkupsModuleMRMLExport.h"
#include "vtkMRMLMarkupsJsonStorageNode.h"

class vtkMRMLMarkupsNode;
class vtkMRMLMarkupsDisplayNode;

class VTK_SLICER_MARKUPS_MODULE_MRML_EXPORT vtkMRMLMarkupsLabelJsonStorageNode
: public vtkMRMLMarkupsJsonStorageNode
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
    
    class vtkInternalLabel;
    friend class vtkInternalLabel;
};

#endif // VTKMRMLMARKUPSLABELJSONSTORAGENODE_H
