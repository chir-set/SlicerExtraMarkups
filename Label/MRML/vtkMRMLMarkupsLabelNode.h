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

#ifndef __vtkmrmlmarkupsLabel_LOWERnode_h_
#define __vtkmrmlmarkupsLabel_LOWERnode_h_

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
  const char* GetIcon() override {return ":/Icons/MarkupsGeneric.png";}
  const char* GetAddIcon() override {return ":/Icons/MarkupsGenericMouseModePlace.png";}
  const char* GetPlaceAddIcon() override {return ":/Icons/MarkupsGenericMouseModePlaceAdd.png";}

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
  vtkMRMLCopyContentDefaultMacro(vtkMRMLMarkupsLabelNode);
  
  vtkMRMLStorageNode* CreateDefaultStorageNode() override;
  
  vtkGetMacro(Label, QString);
  vtkSetMacro(Label, QString);
  vtkSetClampMacro(TipDimensionMode3D, int, this->ViewScaleFactor, this->Fixed);
  vtkGetMacro(TipDimensionMode3D, int);
  static const char* GetTipDimensionMode3DAsString(int mode);
  static int GetTipDimensionMode3DFromString(const char* name);

protected:
  vtkMRMLMarkupsLabelNode();
  ~vtkMRMLMarkupsLabelNode() override;
  vtkMRMLMarkupsLabelNode(const vtkMRMLMarkupsLabelNode&);
  void operator=(const vtkMRMLMarkupsLabelNode&);
  
  QString Label = "Label";
  int TipDimensionMode3D { ViewScaleFactor };

private:
  
};

#endif //vtkmrmlmarkupsLabel_LOWERnode_h_
