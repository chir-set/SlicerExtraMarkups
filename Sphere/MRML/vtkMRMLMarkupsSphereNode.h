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

#ifndef __vtkmrmlmarkupsSphere_LOWERnode_h_
#define __vtkmrmlmarkupsSphere_LOWERnode_h_

#include <vtkMRMLMarkupsNode.h>
#include <vtkMRMLNode.h>

#include "vtkSlicerSphereModuleMRMLExport.h"

//-----------------------------------------------------------------------------
class VTK_SLICER_SPHERE_MODULE_MRML_EXPORT vtkMRMLMarkupsSphereNode
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
  static vtkMRMLMarkupsSphereNode* New();
  vtkTypeMacro(vtkMRMLMarkupsSphereNode, vtkMRMLMarkupsNode);
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
  const char* GetNodeTagName() override {return "MarkupsSphere";}

  /// Get markup type internal name
  const char* GetMarkupType() override {return "Sphere";}

  // Get markup type GUI display name
  const char* GetTypeDisplayName() override {return "Sphere";};

  /// Get markup short name
  const char* GetDefaultNodeNamePrefix() override {return "SP";}

  /// \sa vtkMRMLNode::CopyContent
  vtkMRMLCopyContentDefaultMacro(vtkMRMLMarkupsSphereNode);

  vtkSetMacro(RadiusMode, int);
  vtkGetMacro(RadiusMode, int);
  
  vtkSetMacro(DrawMode2D, int);
  vtkGetMacro(DrawMode2D, int);
  
  vtkSetMacro(Resolution, double);
  vtkGetMacro(Resolution, double);
  
  vtkPolyData * GetSphereWorld() const {return this->SphereWorld;}
  // Used by 3D representation. Needs validation by experts.
  // Is it really RingWorld ? CurveWorld for a markups curve is much more complex.
  void SetSphereWorld(vtkPolyData * polydata) {this->SphereWorld = polydata;}
  
  vtkSetObjectMacro(ResliceNode, vtkMRMLNode);
  vtkGetObjectMacro(ResliceNode, vtkMRMLNode);
  
  void ResliceToControlPoints();

protected:
  vtkMRMLMarkupsSphereNode();
  ~vtkMRMLMarkupsSphereNode() override;
  vtkMRMLMarkupsSphereNode(const vtkMRMLMarkupsSphereNode&);
  void operator=(const vtkMRMLMarkupsSphereNode&);

private:
  int RadiusMode { Centered };
  int DrawMode2D { WorldProjection };
  double Resolution { 36.0 };
  vtkPolyData * SphereWorld = nullptr;
  vtkMRMLNode * ResliceNode = nullptr;
};

#endif //vtkmrmlmarkupsSphere_LOWERnode_h_
