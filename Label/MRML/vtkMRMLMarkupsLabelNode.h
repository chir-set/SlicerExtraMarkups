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

#ifndef __vtkmrmlmarkupsLabelnode_h_
#define __vtkmrmlmarkupsLabelnode_h_

#include <vtkMRMLMarkupsNode.h>

#include "vtkSlicerLabelModuleMRMLExport.h"

#include <QString>

//-----------------------------------------------------------------------------
class VTK_SLICER_LABEL_MODULE_MRML_EXPORT vtkMRMLMarkupsLabelNode
: public vtkMRMLMarkupsNode
{
public:
  enum
  {
    ViewScaleFactor = 0,
    LineLength,
    Fixed,
    TipDimensionMode3D_Last
  };
  static vtkMRMLMarkupsLabelNode* New();
  vtkTypeMacro(vtkMRMLMarkupsLabelNode, vtkMRMLMarkupsNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //--------------------------------------------------------------------------------
  // MRMLNode methods
  //--------------------------------------------------------------------------------
  const char* GetIcon() override {return ":/Icons/MarkupsLabel.png";}
  const char* GetAddIcon() override {return ":/Icons/MarkupsLabelMouseModePlace.png";}
  const char* GetPlaceAddIcon() override {return ":/Icons/MarkupsLabelMouseModePlaceAdd.png";}

  vtkMRMLNode* CreateNodeInstance() override;
  /// Get node XML tag name (like Volume, Model)
  ///
  const char* GetNodeTagName() override {return "MarkupsLabel";}

  /// Get markup type internal name
  const char* GetMarkupType() override {return "Label";}

  // Get markup type GUI display name
  const char* GetTypeDisplayName() override {return "Label";};

  /// Get markup short name
  const char* GetDefaultNodeNamePrefix() override {return "LL";}

  /// \sa vtkMRMLNode::CopyContent
  vtkMRMLCopyContentMacro(vtkMRMLMarkupsLabelNode);

  vtkMRMLStorageNode* CreateDefaultStorageNode() override;

  vtkGetStringMacro(Label);
  vtkSetStringMacro(Label);

  vtkSetClampMacro(TipDimensionMode3D, int, ViewScaleFactor, Fixed);
  vtkGetMacro(TipDimensionMode3D, int);
  // Handled in logic; is ignored in the storage node.
  std::string GetUseAlternateColors() {return UseAlternateColors;};
  void SetUseAlternateColors(const std::string& nodeID = "vtkMRMLColorTableNodeLabels");

  static const char* GetTipDimensionMode3DAsString(int mode);
  static int GetTipDimensionMode3DFromString(const char* name);

protected:
  vtkMRMLMarkupsLabelNode();
  ~vtkMRMLMarkupsLabelNode() override;
  vtkMRMLMarkupsLabelNode(const vtkMRMLMarkupsLabelNode&);
  void operator=(const vtkMRMLMarkupsLabelNode&);
  
  char* Label{ nullptr };
  int TipDimensionMode3D{ ViewScaleFactor };
  std::string UseAlternateColors = "vtkMRMLColorTableNodeLabels";
};

#endif //vtkmrmlmarkupsLabelnode_h_
