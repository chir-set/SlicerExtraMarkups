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

#include "vtkMRMLMarkupsLabelNode.h"

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkMRMLScene.h>

//--------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsLabelNode);

//--------------------------------------------------------------------------------
vtkMRMLMarkupsLabelNode::vtkMRMLMarkupsLabelNode()
{
  this->SetLabel("Label");
  /*
   * p1 : near the label
   * p2 : near what we want to label; is optional.
   */
  this->MaximumNumberOfControlPoints = 2;
  this->RequiredNumberOfControlPoints = 2;
}

//--------------------------------------------------------------------------------
vtkMRMLMarkupsLabelNode::~vtkMRMLMarkupsLabelNode()=default;

//----------------------------------------------------------------------------
vtkMRMLStorageNode* vtkMRMLMarkupsLabelNode::CreateDefaultStorageNode()
{
  vtkMRMLScene* scene = this->GetScene();
  if (scene == nullptr)
  {
    vtkErrorMacro("CreateDefaultStorageNode failed: scene is invalid");
    return nullptr;
  }
  return vtkMRMLStorageNode::SafeDownCast(
    scene->CreateNodeByClass("vtkMRMLMarkupsLabelJsonStorageNode"));
}

//----------------------------------------------------------------------------
const char * vtkMRMLMarkupsLabelNode::GetTipDimensionMode3DAsString(int mode)
{
  switch (mode)
  {
    case vtkMRMLMarkupsLabelNode::ViewScaleFactor:
      return "ViewScaleFactor";
    case vtkMRMLMarkupsLabelNode::LineLength:
      return "LineLength";
    case vtkMRMLMarkupsLabelNode::Fixed:
      return "Fixed";
    default:
      break;
  }
  return "";
}

//----------------------------------------------------------------------------
int vtkMRMLMarkupsLabelNode::GetTipDimensionMode3DFromString(const char* name)
{
  if (name == nullptr)
  {
    // invalid name
    return -1;
  }
  for (int i = 0; i < vtkMRMLMarkupsLabelNode::TipDimensionMode3D_Last; i++)
  {
    if (strcmp(name, vtkMRMLMarkupsLabelNode::GetTipDimensionMode3DAsString(i)) == 0)
    {
      // found a matching name
      return i;
    }
  }
  // unknown name
  return -1;
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsLabelNode::SetUseAlternateColors(const std::string& nodeID)
{
  // Avoid this message when CopyContent is unexpectedly called: markups creation, point placement, moving points.
  if (!this->GetDisableModifiedEvent())
  {
    vtkInfoMacro("The UseAlternateColors property is read from a default scene node only.");
  }
  this->UseAlternateColors = nodeID;
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsLabelNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  vtkMRMLPrintBeginMacro(os, indent);
  vtkMRMLPrintStringMacro(Label);
  vtkMRMLPrintEnumMacro(TipDimensionMode3D);
  vtkMRMLPrintStdStringMacro(UseAlternateColors);
  vtkMRMLPrintEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsLabelNode::CopyContent(vtkMRMLNode* anode, bool deepCopy/*=true*/)
{
  MRMLNodeModifyBlocker blocker(this);
  Superclass::CopyContent(anode, deepCopy);
  
  vtkMRMLCopyBeginMacro(anode);
  vtkMRMLCopyStringMacro(Label);
  vtkMRMLCopyEnumMacro(TipDimensionMode3D);
  vtkMRMLCopyStdStringMacro(UseAlternateColors);
  vtkMRMLCopyEndMacro();
}
