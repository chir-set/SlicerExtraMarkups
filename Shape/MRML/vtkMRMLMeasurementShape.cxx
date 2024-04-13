// MRML includes
#include "vtkMRMLMeasurementShape.h"

// Markups includes
#include "vtkMRMLMarkupsShapeNode.h"

#include <vtkMath.h>
#include <vtkTriangleFilter.h>
#include <vtkMassProperties.h>

#include <cmath>

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
  
  if (shapeNode->GetNumberOfControlPoints() < shapeNode->GetRequiredNumberOfControlPoints())
  {
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
    case vtkMRMLMarkupsShapeNode::Cylinder:
      this->ComputeCylinder();
      break;
    case vtkMRMLMarkupsShapeNode::Cone:
      this->ComputeCone();
      break;
    case vtkMRMLMarkupsShapeNode::Arc:
      this->ComputeArc();
      break;
    case vtkMRMLMarkupsShapeNode::Ellipsoid:
      this->ComputeEllipsoid();
      break;
    case vtkMRMLMarkupsShapeNode::Toroid:
      this->ComputeToroid();
      break;
    case vtkMRMLMarkupsShapeNode::BohemianDome:
      this->ComputeBohemianDome();
      break;
    case vtkMRMLMarkupsShapeNode::ConicSpiral:
      this->ComputeConicSpiral();
      break;
    case vtkMRMLMarkupsShapeNode::Roman:
    case vtkMRMLMarkupsShapeNode::PluckerConoid:
    case vtkMRMLMarkupsShapeNode::Mobius:
    case vtkMRMLMarkupsShapeNode::Kuen:
    case vtkMRMLMarkupsShapeNode::CrossCap:
    case vtkMRMLMarkupsShapeNode::Boy:
    case vtkMRMLMarkupsShapeNode::Bour:
      this->ComputeTransformScaledShape();
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
  const double angle = vtkMath::DegreesFromRadians(std::atan(radius / height));
  
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
  if (this->GetName() == std::string("aperture"))
  {
    measurement = angle * 2.0;
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

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeCylinder()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * cylinderNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!cylinderNode)
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  cylinderNode->GetNthControlPointPositionWorld(0, p1);
  cylinderNode->GetNthControlPointPositionWorld(1, p2);
  cylinderNode->GetNthControlPointPositionWorld(2, p3);
  const double radius = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  const double height = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p3));
  
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
  if (this->GetName() == std::string("area"))
  {
    measurement = (2 * vtkMath::Pi() * radius * height);
  }
  else
  if (this->GetName() == std::string("volume"))
  {
    measurement = (vtkMath::Pi() * radius * radius * height);
  }
  else
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  this->SetValue(measurement, this->GetName().c_str());
}

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeArc()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * arcNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!arcNode)
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  arcNode->GetNthControlPointPositionWorld(0, p1);
  arcNode->GetNthControlPointPositionWorld(1, p2);
  arcNode->GetNthControlPointPositionWorld(2, p3);
  const double radius = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  
  double radiusVector1[3] = { 0.0 };
  double radiusVector2[3] = { 0.0 };
  vtkMath::Subtract(p2, p1, radiusVector1);
  vtkMath::Subtract(p3, p1, radiusVector2);
  double angle = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(radiusVector1, radiusVector2));
  
  if (this->GetName() == std::string("radius"))
  {
    measurement = radius;
  }
  else
  if (this->GetName() == std::string("angle"))
  {
    measurement = angle;
  }
  else
  if (this->GetName() == std::string("area"))
  {
    measurement = (vtkMath::Pi() * radius * radius) / angle;
  }
  else
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  this->SetValue(measurement, this->GetName().c_str());
}

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeEllipsoid()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * ellipsoidNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!ellipsoidNode)
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  vtkNew<vtkTriangleFilter> triangleFilter;
  vtkNew<vtkMassProperties> massProperties;
  triangleFilter->SetInputData(ellipsoidNode->GetShapeWorld());
  triangleFilter->Update();
  massProperties->SetInputData(triangleFilter->GetOutput());
  massProperties->Update();
  
  double xRadius = ellipsoidNode->GetParametricX();
  double yRadius = ellipsoidNode->GetParametricY();
  double zRadius = ellipsoidNode->GetParametricZ();
  
  if (this->GetName() == std::string("radius-x"))
  {
    measurement = xRadius;
  }
  else
  if (this->GetName() == std::string("radius-y"))
  {
    measurement = yRadius;
  }
  else
  if (this->GetName() == std::string("radius-z"))
  {
    measurement = zRadius;
  }
  else
  if (this->GetName() == std::string("n1"))
  {
    measurement = ellipsoidNode->GetParametricN1();
  }
  else
  if (this->GetName() == std::string("n2"))
  {
    measurement = ellipsoidNode->GetParametricN2();
  }
  else
  if (this->GetName() == std::string("volume"))
  {
    // Setting UVW values are not friendly to volume calculation.
    measurement = massProperties->GetVolume();
  }
  else
    if (this->GetName() == std::string("area"))
    {
      measurement = massProperties->GetSurfaceArea();
    }
  else
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  this->SetValue(measurement, this->GetName().c_str());
}

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeToroid()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * toroidNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!toroidNode)
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  vtkNew<vtkTriangleFilter> triangleFilter;
  vtkNew<vtkMassProperties> massProperties;
  triangleFilter->SetInputData(toroidNode->GetShapeWorld());
  triangleFilter->Update();
  massProperties->SetInputData(triangleFilter->GetOutput());
  massProperties->Update();
  
  double xRadius = toroidNode->GetParametricX();
  double yRadius = toroidNode->GetParametricY();
  double zRadius = toroidNode->GetParametricZ();
  
  if (this->GetName() == std::string("radius-x-scalefactor"))
  {
    measurement = xRadius;
  }
  else
  if (this->GetName() == std::string("radius-y-scalefactor"))
  {
    measurement = yRadius;
  }
  else
  if (this->GetName() == std::string("radius-z-scalefactor"))
  {
    measurement = zRadius;
  }
  else
  if (this->GetName() == std::string("radius-ring"))
  {
    measurement = toroidNode->GetParametricRingRadius();
  }
  else
  if (this->GetName() == std::string("radius-crosssection"))
  {
    measurement = toroidNode->GetParametricCrossSectionRadius();
  }
  else
  if (this->GetName() == std::string("n1"))
  {
    measurement = toroidNode->GetParametricN1();
  }
  else
  if (this->GetName() == std::string("n2"))
  {
    measurement = toroidNode->GetParametricN2();
  }
  else
  if (this->GetName() == std::string("volume"))
  {
    measurement = massProperties->GetVolume();
  }
  else
  if (this->GetName() == std::string("area"))
  {
    measurement = massProperties->GetSurfaceArea();
  }
  else
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  this->SetValue(measurement, this->GetName().c_str());
}

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeBohemianDome()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * bohemianDomeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!bohemianDomeNode)
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  vtkNew<vtkTriangleFilter> triangleFilter;
  vtkNew<vtkMassProperties> massProperties;
  triangleFilter->SetInputData(bohemianDomeNode->GetShapeWorld());
  triangleFilter->Update();
  massProperties->SetInputData(triangleFilter->GetOutput());
  massProperties->Update();
  
  if (this->GetName() == std::string("a"))
  {
    measurement = bohemianDomeNode->GetParametricX();
  }
  else
  if (this->GetName() == std::string("b"))
  {
    measurement = bohemianDomeNode->GetParametricY();
  }
  else
  if (this->GetName() == std::string("c"))
  {
    measurement = bohemianDomeNode->GetParametricZ();
  }
  else
  if (this->GetName() == std::string("volume"))
  {
    measurement = massProperties->GetVolume(); // Fails; not called.
  }
  else
  if (this->GetName() == std::string("area"))
  {
    measurement = massProperties->GetSurfaceArea();
  }
  else
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  this->SetValue(measurement, this->GetName().c_str());
}

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeConicSpiral()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * conicSpiralNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!conicSpiralNode)
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  vtkNew<vtkTriangleFilter> triangleFilter;
  vtkNew<vtkMassProperties> massProperties;
  triangleFilter->SetInputData(conicSpiralNode->GetShapeWorld());
  triangleFilter->Update();
  massProperties->SetInputData(triangleFilter->GetOutput());
  massProperties->Update();
  
  if (this->GetName() == std::string("x"))
  {
    measurement = conicSpiralNode->GetParametricX();
  }
  else
  if (this->GetName() == std::string("y"))
  {
    measurement = conicSpiralNode->GetParametricY();
  }
  else
  if (this->GetName() == std::string("z"))
  {
    measurement = conicSpiralNode->GetParametricZ();
  }
  else
  if (this->GetName() == std::string("n"))
  {
    measurement = conicSpiralNode->GetParametricN();
  }
  else
  if (this->GetName() == std::string("area"))
  {
    measurement = massProperties->GetSurfaceArea();
  }
  else
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  this->SetValue(measurement, this->GetName().c_str());
}

//----------------------------------------------------------------------------
void vtkMRMLMeasurementShape::ComputeTransformScaledShape()
{
  double measurement = 0.0;
  vtkMRMLMarkupsShapeNode * node = vtkMRMLMarkupsShapeNode::SafeDownCast(this->InputMRMLNode);
  if (!node)
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  vtkNew<vtkTriangleFilter> triangleFilter;
  vtkNew<vtkMassProperties> massProperties;
  triangleFilter->SetInputData(node->GetShapeWorld());
  triangleFilter->Update();
  massProperties->SetInputData(triangleFilter->GetOutput());
  massProperties->Update();
  
  double xRadius = node->GetParametricX();
  double yRadius = node->GetParametricY();
  double zRadius = node->GetParametricZ();
  
  if (this->GetName() == std::string("x-scalefactor"))
  {
    measurement = xRadius;
  }
  else
  if (this->GetName() == std::string("y-scalefactor"))
  {
    measurement = yRadius;
  }
  else
  if (this->GetName() == std::string("z-scalefactor"))
  {
    measurement = zRadius;
  }
  else
  if (this->GetName() == std::string("volume"))
  {
    measurement = massProperties->GetVolume();
  }
  else
  if (this->GetName() == std::string("area"))
  {
    measurement = massProperties->GetSurfaceArea();
  }
  else
  {
    this->SetValue(measurement, "#ERR");
    return;
  }
  this->SetValue(measurement, this->GetName().c_str());
}
