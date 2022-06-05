// MRML includes
#include "vtkMRMLMeasurementSphere.h"
#include "vtkMRMLUnitNode.h"

// Markups includes
#include "vtkMRMLMarkupsSphereNode.h"

vtkStandardNewMacro(vtkMRMLMeasurementSphere);

//----------------------------------------------------------------------------
vtkMRMLMeasurementSphere::vtkMRMLMeasurementSphere()
{
}

vtkMRMLMeasurementSphere::~vtkMRMLMeasurementSphere() = default;

void vtkMRMLMeasurementSphere::PrintSelf(std::ostream& os, vtkIndent indent)
{
    Superclass::PrintSelf(os,indent);
}

void vtkMRMLMeasurementSphere::Compute()
{
    double measurement = 0.0;
    vtkMRMLMarkupsSphereNode * sphereNode = vtkMRMLMarkupsSphereNode::SafeDownCast(this->InputMRMLNode);
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
        if (sphereNode->GetMode() == vtkMRMLMarkupsSphereNode::Centered)
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
        if (sphereNode->GetMode() == vtkMRMLMarkupsSphereNode::Centered)
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
        if (sphereNode->GetMode() == vtkMRMLMarkupsSphereNode::Centered)
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
