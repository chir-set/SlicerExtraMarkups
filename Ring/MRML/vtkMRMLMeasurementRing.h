#ifndef VTKMRMLMEASUREMENTRING_H
#define VTKMRMLMEASUREMENTRING_H

// MRML includes
#include "vtkMRMLMeasurement.h"

// Markups includes
#include "vtkSlicerMarkupsModuleMRMLExport.h"

class VTK_SLICER_MARKUPS_MODULE_MRML_EXPORT vtkMRMLMeasurementRing : public vtkMRMLMeasurement
{
public:
    static vtkMRMLMeasurementRing *New();
    vtkTypeMacro(vtkMRMLMeasurementRing, vtkMRMLMeasurement);
    void PrintSelf(ostream& os, vtkIndent indent) override;
    
    void Compute() override;

protected:
    vtkMRMLMeasurementRing();
    ~vtkMRMLMeasurementRing() override;
    vtkMRMLMeasurementRing(const vtkMRMLMeasurementRing&);
    void operator=(const vtkMRMLMeasurementRing&);

};

#endif // VTKMRMLMEASUREMENTRING_H
