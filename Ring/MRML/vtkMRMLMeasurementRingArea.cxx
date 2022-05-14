// MRML includes
#include "vtkMRMLMeasurementRingArea.h"
#include "vtkMRMLUnitNode.h"

// Markups includes
#include "vtkMRMLMarkupsRingNode.h"

vtkStandardNewMacro(vtkMRMLMeasurementRingArea);

//----------------------------------------------------------------------------
vtkMRMLMeasurementRingArea::vtkMRMLMeasurementRingArea()
{
  this->SetUnits("cm2");
  this->SetDisplayCoefficient(0.01);
  this->SetPrintFormat("%-#4.4g%s");
}

vtkMRMLMeasurementRingArea::~vtkMRMLMeasurementRingArea() = default;

void vtkMRMLMeasurementRingArea::PrintSelf(std::ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

void vtkMRMLMeasurementRingArea::Compute()
{
  double area = 0.0;
  vtkMRMLMarkupsRingNode * ringNode = vtkMRMLMarkupsRingNode::SafeDownCast(this->InputMRMLNode);
  if (!ringNode)
    {
      this->SetValue(area, "area");
      return;
    }

  double lineLength = ringNode->GetLineLengthWorld();
  
  if (ringNode->GetMode() == vtkMRMLMarkupsRingNode::Centered)
    {
      area = vtkMath::Pi() * (lineLength * lineLength);
    }
  else
    {
      double radius = lineLength / 2.0;
      area = vtkMath::Pi() * (radius * radius);
    }
  this->SetValue(area, "area");
}

