/*==============================================================================

  Copyright (c) The Intervention Centre
  Oslo University Hospital, Oslo, Norway. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Rafael Palomar (The Intervention Centre,
  Oslo University Hospital) and was supported by The Research Council of Norway
  through the ALive project (grant nr. 311393).

==============================================================================*/

#include "vtkSlicerShapeLogic.h"

// Shape MRML includes
#include "vtkMRMLMarkupsShapeNode.h"
#include "vtkMRMLMarkupsShapeJsonStorageNode.h"

// Shape VTKWidgets includes
#include "vtkSlicerShapeWidget.h"

// MRML includes
#include <vtkMRMLScene.h>

// Markups logic includes
#include <vtkSlicerMarkupsLogic.h>

// Markups MRML includes
#include <vtkMRMLMarkupsDisplayNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkMRMLColorTableNode.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerShapeLogic);

//---------------------------------------------------------------------------
vtkSlicerShapeLogic::vtkSlicerShapeLogic()
{
  this->RegisterJsonStorageNodeForMarkupsType("Shape", "vtkMRMLMarkupsShapeJsonStorageNode");
}

//---------------------------------------------------------------------------
vtkSlicerShapeLogic::~vtkSlicerShapeLogic() = default;

//---------------------------------------------------------------------------
void vtkSlicerShapeLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeLogic::RegisterNodes()
{
  vtkMRMLScene *scene = this->GetMRMLScene();
  if (!scene)
    {
    vtkErrorMacro("RegisterNodes failed: invalid scene");
    return;
    }

  vtkSlicerMarkupsLogic* markupsLogic = vtkSlicerMarkupsLogic::SafeDownCast(this->GetModuleLogic("Markups"));
  if (!markupsLogic)
    {
    vtkErrorMacro("RegisterNodes failed: invalid markups module logic");
    return;
    }

  vtkNew<vtkMRMLMarkupsShapeNode> markupsShapeNode;
  vtkNew<vtkSlicerShapeWidget> ShapeWidget;
  markupsLogic->RegisterMarkupsNode(markupsShapeNode, ShapeWidget);
  
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLMarkupsShapeJsonStorageNode>::New());
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeLogic::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  if (!node)
  {
    return;
  }
  vtkMRMLScene *scene = this->GetMRMLScene();
  if (!scene)
  {
    vtkErrorMacro("OnMRMLSceneNodeAdded failed: invalid scene");
    return;
  }
  
  vtkSlicerMarkupsLogic* markupsLogic = vtkSlicerMarkupsLogic::SafeDownCast(this->GetModuleLogic("Markups"));
  if (!markupsLogic)
  {
    vtkErrorMacro("OnMRMLSceneNodeAdded failed: invalid markups module logic");
    return;
  }
  if (!node->IsA("vtkMRMLMarkupsShapeNode"))
  {
    return;
  }
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(node);
  if (!shapeNode)
  {
    vtkErrorMacro("OnMRMLSceneNodeAdded failed: invalid markups shape node");
    return;
  }

  const std::string colourNodeID = shapeNode->GetUseAlternateColors();
  if (!colourNodeID.empty())
  {
    if (!shapeNode->GetDisplayNode())
    {
      vtkErrorMacro("OnMRMLSceneNodeAdded failed: invalid markups shape display node");
      return;
    }
    double selectedColour[3] = { 1.0, 0.5, 0.5};
    this->GenerateUniqueColor(selectedColour, colourNodeID);
    double colour[3] = { 1.0 - colour[0], 1.0 - colour[1], 1.0 - colour[2]};
    shapeNode->GetDisplayNode()->SetSelectedColor(selectedColour);
    shapeNode->GetDisplayNode()->SetColor(colour);
  }
  shapeNode->SetShapeName(vtkMRMLMarkupsShapeNode::Sphere);
}

//------------------------------------------------------------------------------
// Poked from vtkSlicerMarkupsLogic.cxx .
void vtkSlicerShapeLogic::GenerateUniqueColor(double color[3], const std::string& colorNodeID)
{
  double rgba[4] = { 1.0, 0.5, 0.5, 1.0 };
  vtkMRMLColorTableNode* colorTable = nullptr;
  vtkMRMLScene* scene = this->GetMRMLScene();
  {
    colorTable = vtkMRMLColorTableNode::SafeDownCast(
      scene->GetNodeByID(colorNodeID.c_str()));
  }
  if (colorTable)
  {
    colorTable->GetColor(this->NextColorIndex, rgba);
    this->NextColorIndex++;
    if (this->NextColorIndex >= colorTable->GetNumberOfColors())
    {
      this->NextColorIndex = 1; // 0 is black
    }
  }
  color[0] = rgba[0];
  color[1] = rgba[1];
  color[2] = rgba[2];
}
