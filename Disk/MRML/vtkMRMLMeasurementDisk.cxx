// MRML includes
#include "vtkMRMLMeasurementDisk.h"
#include "vtkMRMLUnitNode.h"

// Markups includes
#include "vtkMRMLMarkupsDiskNode.h"

#include <vtkMath.h>

vtkStandardNewMacro(vtkMRMLMeasurementDisk);

//----------------------------------------------------------------------------
vtkMRMLMeasurementDisk::vtkMRMLMeasurementDisk()
{
}

vtkMRMLMeasurementDisk::~vtkMRMLMeasurementDisk() = default;

void vtkMRMLMeasurementDisk::PrintSelf(std::ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

void vtkMRMLMeasurementDisk::Compute()
{
  double measurement = 0.0;
  vtkMRMLMarkupsDiskNode * diskNode = vtkMRMLMarkupsDiskNode::SafeDownCast(this->InputMRMLNode);
  if (!diskNode)
  {
      this->SetValue(measurement, "#ERR");
      return;
  }
  if (this->GetName() == std::string("innerRadius"))
  {
    measurement = diskNode->GetInnerRadius();
    this->SetValue(measurement, this->GetName().c_str());
  }
  else
  if (this->GetName() == std::string("outerRadius"))
  {
    measurement = diskNode->GetOuterRadius();
    this->SetValue(measurement, this->GetName().c_str());
  }
  else
  if (this->GetName() == std::string("width"))
    {
      measurement = diskNode->GetOuterRadius() - diskNode->GetInnerRadius();
      this->SetValue(measurement, this->GetName().c_str());
    }
  else
  if (this->GetName() == std::string("area"))
  {
    if (!diskNode->GetDiskWorld())
    {
        this->SetValue(measurement, "#ERR");
        return;
    }
    // vtkMassProperties fails here : <Input data type must be VTK_TRIANGLE not 9>.
    const double innerRadius = diskNode->GetInnerRadius();
    const double outerRadius = diskNode->GetOuterRadius();
    const double innerArea = vtkMath::Pi() * innerRadius * innerRadius;
    const double outerArea = vtkMath::Pi() * outerRadius * outerRadius;
    measurement = outerArea - innerArea;
    this->SetValue(measurement, this->GetName().c_str());
  }
  else
    if (this->GetName() == std::string("innerArea"))
    {
      if (!diskNode->GetDiskWorld())
      {
        this->SetValue(measurement, "#ERR");
        return;
      }
      const double innerRadius = diskNode->GetInnerRadius();
      const double innerArea = vtkMath::Pi() * innerRadius * innerRadius;
      this->SetValue(innerArea, this->GetName().c_str());
    }
    else
      if (this->GetName() == std::string("outerArea"))
      {
        if (!diskNode->GetDiskWorld())
        {
          this->SetValue(measurement, "#ERR");
          return;
        }
        const double outerRadius = diskNode->GetOuterRadius();
        const double outerArea = vtkMath::Pi() * outerRadius * outerRadius;
        this->SetValue(outerArea, this->GetName().c_str());
      }
  else
  {
    this->SetValue(measurement, "#ERR");
  }
  
}
