#ifndef VTKMRMLMEASUREMENTSHAPE_H
#define VTKMRMLMEASUREMENTSHAPE_H

// MRML includes
#include "vtkMRMLMeasurement.h"

// Markups includes
#include "vtkSlicerShapeModuleMRMLExport.h"

class VTK_SLICER_SHAPE_MODULE_MRML_EXPORT vtkMRMLMeasurementShape : public vtkMRMLMeasurement
{
public:
    static vtkMRMLMeasurementShape *New();
    vtkTypeMacro(vtkMRMLMeasurementShape, vtkMRMLMeasurement);
    void PrintSelf(ostream& os, vtkIndent indent) override;
    
    // Needed since 3fd4633168
    VTK_NEWINSTANCE
    virtual vtkMRMLMeasurementShape* CreateInstance() const override
    { return vtkMRMLMeasurementShape::New(); }
    void Compute() override;
    
protected:
    vtkMRMLMeasurementShape();
    ~vtkMRMLMeasurementShape() override;
    vtkMRMLMeasurementShape(const vtkMRMLMeasurementShape&);
    void operator=(const vtkMRMLMeasurementShape&);
    
    void ComputeDisk();
    void ComputeRing();
    void ComputeSphere();
    void ComputeTube();
    void ComputeCone();
    void ComputeCylinder();
    void ComputeArc();
    // Parametric geometries.
    void ComputeEllipsoid();
    void ComputeBohemianDome();
    void ComputeToroid();
    void ComputeConicSpiral();
    void ComputeTransformScaledShape();
};

#endif // VTKMRMLMEASUREMENTSHAPE_H
