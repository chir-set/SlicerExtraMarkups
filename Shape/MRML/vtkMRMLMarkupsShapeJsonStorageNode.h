/// Storage node for reading/writing properties of vtkMRMLMarkupsShapeNode.
///
/// The implementation is based on vtkMRMLMarkupsROIJsonStorageNode.
///

#ifndef VTKMRMLMARKUPSSHAPEJSONSTORAGENODE_H
#define VTKMRMLMARKUPSSHAPEJSONSTORAGENODE_H

// Markups includes
#include "vtkSlicerShapeModuleMRMLExport.h"
#include "vtkMRMLMarkupsJsonStorageNode.h"

class vtkMRMLMarkupsJsonElement;
class vtkMRMLMarkupsJsonWriter;
class vtkMRMLMarkupsNode;

class VTK_SLICER_SHAPE_MODULE_MRML_EXPORT vtkMRMLMarkupsShapeJsonStorageNode : public vtkMRMLMarkupsJsonStorageNode
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

    bool WriteBasicProperties(vtkMRMLMarkupsJsonWriter* writer, vtkMRMLMarkupsNode* markupsNode) override;
    bool UpdateMarkupsNodeFromJsonValue(vtkMRMLMarkupsNode* markupsNode, vtkMRMLMarkupsJsonElement* markupObject) override;
};

#endif // VTKMRMLMARKUPSSHAPEJSONSTORAGENODE_H
