#ifndef VTKMRMLMEASUREMENTRINGAREA_H
#define VTKMRMLMEASUREMENTRINGAREA_H

// MRML includes
#include "vtkMRMLMeasurement.h"

// Markups includes
#include "vtkSlicerMarkupsModuleMRMLExport.h"

class VTK_SLICER_MARKUPS_MODULE_MRML_EXPORT vtkMRMLMeasurementRingArea : public vtkMRMLMeasurement
{
public:
    static vtkMRMLMeasurementRingArea *New();
    vtkTypeMacro(vtkMRMLMeasurementRingArea, vtkMRMLMeasurement);
    void PrintSelf(ostream& os, vtkIndent indent) override;
    
    void Compute() override;

protected:
    vtkMRMLMeasurementRingArea();
    ~vtkMRMLMeasurementRingArea() override;
    vtkMRMLMeasurementRingArea(const vtkMRMLMeasurementRingArea&);
    void operator=(const vtkMRMLMeasurementRingArea&);

};

#endif // VTKMRMLMEASUREMENTRINGAREA_H
