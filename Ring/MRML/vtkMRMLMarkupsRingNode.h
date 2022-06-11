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

#ifndef __vtkmrmlmarkupsringnode_h_
#define __vtkmrmlmarkupsringnode_h_

#include <vtkMRMLMarkupsNode.h>
#include <vtkMRMLNode.h>

#include "vtkSlicerRingModuleMRMLExport.h"

//-----------------------------------------------------------------------------
class VTK_SLICER_RING_MODULE_MRML_EXPORT vtkMRMLMarkupsRingNode
: public vtkMRMLMarkupsNode
{
public:
  enum
  {
    Centered = 0,
    Circumferential
  };
  enum
  {
    WorldProjection = 0,
    WorldIntersection,
    SliceProjection
  };
  static vtkMRMLMarkupsRingNode* New();
  vtkTypeMacro(vtkMRMLMarkupsRingNode, vtkMRMLMarkupsNode);
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
  const char* GetNodeTagName() override {return "MarkupsRing";}

  /// Get markup type internal name
  const char* GetMarkupType() override {return "Ring";}

  // Get markup type GUI display name
  const char* GetTypeDisplayName() override {return "Ring";};

  /// Get markup short name
  const char* GetDefaultNodeNamePrefix() override {return "RI";}

  /// \sa vtkMRMLNode::CopyContent
  vtkMRMLCopyContentDefaultMacro(vtkMRMLMarkupsRingNode);

  vtkSetMacro(RadiusMode, int);
  vtkGetMacro(RadiusMode, int);
  
  vtkSetMacro(DrawMode2D, int);
  vtkGetMacro(DrawMode2D, int);
  
  vtkSetMacro(Resolution, double);
  vtkGetMacro(Resolution, double);
  
  vtkPolyData * GetRingWorld() const {return this->RingWorld;}
  // Used by 3D representation. Needs validation by experts.
  // Is it really RingWorld ? CurveWorld for a markups curve is much more complex.
  void SetRingWorld(vtkPolyData * polydata) {this->RingWorld = polydata;}
  
  vtkSetObjectMacro(ResliceNode, vtkMRMLNode);
  vtkGetObjectMacro(ResliceNode, vtkMRMLNode);
  
  void ResliceToRingPlane();

protected:
  vtkMRMLMarkupsRingNode();
  ~vtkMRMLMarkupsRingNode() override;
  vtkMRMLMarkupsRingNode(const vtkMRMLMarkupsRingNode&);
  void operator=(const vtkMRMLMarkupsRingNode&);

private:
  int RadiusMode { Centered };
  int DrawMode2D { WorldProjection };
  double Resolution { 45.0 };
  vtkPolyData * RingWorld = nullptr;
  vtkMRMLNode * ResliceNode = nullptr;
};

#endif //vtkmrmlmarkupsringnode_h_
