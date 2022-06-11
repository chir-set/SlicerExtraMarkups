// MRML includes
#include "vtkMRMLMeasurementRing.h"
#include "vtkMRMLUnitNode.h"

// Markups includes
#include "vtkMRMLMarkupsRingNode.h"

vtkStandardNewMacro(vtkMRMLMeasurementRing);

//----------------------------------------------------------------------------
vtkMRMLMeasurementRing::vtkMRMLMeasurementRing()
{
}

vtkMRMLMeasurementRing::~vtkMRMLMeasurementRing() = default;

void vtkMRMLMeasurementRing::PrintSelf(std::ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

void vtkMRMLMeasurementRing::Compute()
{
  double measurement = 0.0;
  vtkMRMLMarkupsRingNode * ringNode = vtkMRMLMarkupsRingNode::SafeDownCast(this->InputMRMLNode);
  if (!ringNode)
    {
      this->SetValue(measurement, "#ERR");
      return;
    }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  ringNode->GetNthControlPointPositionWorld(0, p1);
  ringNode->GetNthControlPointPositionWorld(1, p2);
  const double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  
  if (this->GetName() == std::string("radius"))
  {
    if (ringNode->GetRadiusMode() == vtkMRMLMarkupsRingNode::Centered)
    {
      measurement = lineLength;
    }
    else
    {
      measurement = lineLength / 2.0;
    }
  }
  else if (this->GetName() == std::string("area"))
  {
    if (ringNode->GetRadiusMode() == vtkMRMLMarkupsRingNode::Centered)
      {
        measurement = vtkMath::Pi() * (lineLength * lineLength);
      }
    else
      {
        double radius = lineLength / 2.0;
        measurement = vtkMath::Pi() * (radius * radius);
      }
  }
  else
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  this->SetValue(measurement, this->GetName().c_str());
}

