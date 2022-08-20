#ifndef VTKMRMLMARKUPSSHAPEJSONSTORAGENODE_H
#define VTKMRMLMARKUPSSHAPEJSONSTORAGENODE_H

// Templated on ROI storage node.

// Markups includes
#include "vtkSlicerMarkupsModuleMRMLExport.h"
#include "vtkMRMLMarkupsJsonStorageNode.h"

class vtkMRMLMarkupsNode;
class vtkMRMLMarkupsDisplayNode;

class VTK_SLICER_MARKUPS_MODULE_MRML_EXPORT vtkMRMLMarkupsShapeJsonStorageNode
: public vtkMRMLMarkupsJsonStorageNode
{
public:
    static vtkMRMLMarkupsShapeJsonStorageNode* New();
    vtkTypeMacro(vtkMRMLMarkupsShapeJsonStorageNode, vtkMRMLMarkupsJsonStorageNode);
    
    vtkMRMLNode* CreateNodeInstance() override;
    const char* GetNodeTagName() override { return "MarkupsShapesJsonStorage"; };
    bool CanReadInReferenceNode(vtkMRMLNode* refNode) override;

protected:
    vtkMRMLMarkupsShapeJsonStorageNode();
    ~vtkMRMLMarkupsShapeJsonStorageNode() override;
    vtkMRMLMarkupsShapeJsonStorageNode(const vtkMRMLMarkupsShapeJsonStorageNode&);
    void operator=(const vtkMRMLMarkupsShapeJsonStorageNode&);
    
    class vtkInternalShape;
    friend class vtkInternalShape;
};

#endif // VTKMRMLMARKUPSSHAPEJSONSTORAGENODE_H
