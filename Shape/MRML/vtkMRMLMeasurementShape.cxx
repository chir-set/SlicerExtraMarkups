// MRML includes
#include "vtkMRMLMeasurementShape.h"
#include "vtkMRMLUnitNode.h"

// Markups includes
#include "vtkMRMLMarkupsShapeNode.h"

#include <vtkMath.h>
#include <vtkTriangleFilter.h>
#include <vtkMassProperties.h>

vtkStandardNewMacro(vtkMRMLMeasurementShape);

//----------------------------------------------------------------------------
vtkMRMLMeasurementShape::vtkMRMLMeasurementShape()
{
}

vtkMRMLMeasurementShape::~vtkMRMLMeasurementShape() = default;

void vtkMRMLMeasurementShape::PrintSelf(std::ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::Compute()
{
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!shapeNode)
  {
    this->SetValue(0.0, "#ERR");
    return;
  }
  switch (shapeNode->GetShapeName())
  {
    case vtkMRMLMarkupsShapeNode::Sphere :
      this->ComputeSphere();
      break;
    case vtkMRMLMarkupsShapeNode::Ring:
      this->ComputeRing();
      break;
    case vtkMRMLMarkupsShapeNode::Disk:
      this->ComputeDisk();
      break;
    case vtkMRMLMarkupsShapeNode::Tube:
      this->ComputeTube();
      break;
    case vtkMRMLMarkupsShapeNode::Cone:
      this->ComputeCone();
      break;
    default :
      vtkErrorMacro("Unknown shape.");
      return;
  };
}


//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeDisk()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!shapeNode)
  {
      this->SetValue(measurement, "#ERR");
      return;
  }
  
  double closestPoint[3] = { 0.0 }; // Unused here
  double farthestPoint[3] = { 0.0 };
  double innerRadius = 0.0, outerRadius = 0.0;
  if (!shapeNode->DescribeDiskPointSpacing(closestPoint, farthestPoint, innerRadius, outerRadius))
  {
    vtkDebugMacro("Point proximity description failure.");
    return;
  }
  
  if (this->GetName() == std::string("innerRadius"))
  {
    this->SetValue(innerRadius, this->GetName().c_str());
  }
  else
  if (this->GetName() == std::string("outerRadius"))
  {
    this->SetValue(outerRadius, this->GetName().c_str());
  }
  else
  if (this->GetName() == std::string("width"))
    {
      measurement = outerRadius - innerRadius;
      this->SetValue(measurement, this->GetName().c_str());
    }
  else
  if (this->GetName() == std::string("area"))
  {
    if (!shapeNode->GetShapeWorld())
    {
        this->SetValue(measurement, "#ERR");
        return;
    }
    // vtkMassProperties fails here : <Input data type must be VTK_TRIANGLE not 9>.
    const double innerArea = vtkMath::Pi() * innerRadius * innerRadius;
    const double outerArea = vtkMath::Pi() * outerRadius * outerRadius;
    measurement = outerArea - innerArea;
    this->SetValue(measurement, this->GetName().c_str());
  }
  else
  if (this->GetName() == std::string("innerArea"))
  {
    if (!shapeNode->GetShapeWorld())
    {
      this->SetValue(measurement, "#ERR");
      return;
    }
    const double innerArea = vtkMath::Pi() * innerRadius * innerRadius;
    this->SetValue(innerArea, this->GetName().c_str());
  }
  else
    if (this->GetName() == std::string("outerArea"))
    {
      if (!shapeNode->GetShapeWorld())
      {
        this->SetValue(measurement, "#ERR");
        return;
      }
      const double outerArea = vtkMath::Pi() * outerRadius * outerRadius;
      this->SetValue(outerArea, this->GetName().c_str());
    }
  else
  {
    this->SetValue(measurement, "#ERR");
  }
  
}

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeRing()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * ringNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
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
    if (ringNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
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
    if (ringNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
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

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeSphere()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * sphereNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!sphereNode)
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  sphereNode->GetNthControlPointPositionWorld(0, p1);
  sphereNode->GetNthControlPointPositionWorld(1, p2);
  const double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  
  if (this->GetName() == std::string("radius"))
  {
    if (sphereNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
    {
      measurement = lineLength;
    }
    else
    {
      measurement = lineLength / 2.0;
    }
  }
  else
  if (this->GetName() == std::string("area"))
  {
    if (sphereNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
    {
      measurement = 4.0 * vtkMath::Pi() * (lineLength * lineLength);
    }
    else
    {
      double radius = lineLength / 2.0;
      measurement = 4.0 * vtkMath::Pi() * (radius * radius);
    }
  }
  else
  if (this->GetName() == std::string("volume"))
  {
    if (sphereNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
    {
      measurement = (4.0 / 3.0) * vtkMath::Pi() * (lineLength * lineLength * lineLength);
    }
    else
    {
      double radius = lineLength / 2.0;
      measurement = (4.0 / 3.0) * vtkMath::Pi() * (radius * radius * radius);
    }
  }
  else
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  this->SetValue(measurement, this->GetName().c_str());
}

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeTube()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * tubeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!tubeNode || tubeNode->GetCappedTubeWorld() == nullptr)
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  vtkNew<vtkTriangleFilter> triangleFilter;
  vtkNew<vtkMassProperties> massProperties;
  triangleFilter->SetInputData(tubeNode->GetCappedTubeWorld());
  triangleFilter->Update();
  massProperties->SetInputData(triangleFilter->GetOutput());
  massProperties->Update();
  
  if (this->GetName() == std::string("area"))
  {
    measurement = massProperties->GetSurfaceArea();
  }
  else
  if (this->GetName() == std::string("volume"))
  {
    measurement = massProperties->GetVolume();
  }
  else
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  this->SetValue(measurement, this->GetName().c_str());
}

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeCone()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * coneNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!coneNode)
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  coneNode->GetNthControlPointPositionWorld(0, p1);
  coneNode->GetNthControlPointPositionWorld(1, p2);
  coneNode->GetNthControlPointPositionWorld(2, p3);
  const double radius = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  const double height = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p3));
  const double slant = std::sqrt(vtkMath::Distance2BetweenPoints(p2, p3));
  
  if (this->GetName() == std::string("radius"))
  {
    measurement = radius;
  }
  else
  if (this->GetName() == std::string("height"))
  {
    measurement = height;
  }
  else
  if (this->GetName() == std::string("slant"))
  {
    measurement = slant;
  }
  else
  if (this->GetName() == std::string("area"))
  {
    measurement = (vtkMath::Pi() * radius * slant) + (vtkMath::Pi() * radius * radius);
  }
  else
  if (this->GetName() == std::string("volume"))
  {
    measurement = (vtkMath::Pi() * radius * radius * height) / 3.0;
  }
  else
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  this->SetValue(measurement, this->GetName().c_str());
}
