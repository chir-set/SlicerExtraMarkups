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

#ifndef __vtkmrmlmarkupsshapenode_h_
#define __vtkmrmlmarkupsshapenode_h_

#include <vtkMRMLMarkupsNode.h>

#include "vtkSlicerShapeModuleMRMLExport.h"

//-----------------------------------------------------------------------------
class VTK_SLICER_SHAPE_MODULE_MRML_EXPORT vtkMRMLMarkupsShapeNode
: public vtkMRMLMarkupsNode
{
public:
  enum
  {
    Sphere = 0,
    Ring,
    Disk,
    Tube,
    Cylinder,
    Cone,
    Arc,
    ShapeName_Last
  };
  enum
  {
    Centered = 0,
    Circumferential,
    RadiusMode_Last
  };
  enum
  {
    Intersection = 0,
    Projection,
    DrawMode2D_Last
  };
  static vtkMRMLMarkupsShapeNode* New();
  vtkTypeMacro(vtkMRMLMarkupsShapeNode, vtkMRMLMarkupsNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //--------------------------------------------------------------------------------
  // MRMLNode methods
  //--------------------------------------------------------------------------------
  const char* GetIcon() override {return ":/Icons/MarkupsShape.png";}
  const char* GetAddIcon() override {return ":/Icons/MarkupsShapeMouseModePlace.png";}
  const char* GetPlaceAddIcon() override {return ":/Icons/MarkupsShapeMouseModePlaceAdd.png";}

  vtkMRMLNode* CreateNodeInstance() override;
  /// Get node XML tag name (like Volume, Model)
  ///
  const char* GetNodeTagName() override {return "MarkupsShape";}

  /// Get markup type internal name
  const char* GetMarkupType() override {return "Shape";}

  // Get markup type GUI display name
  const char* GetTypeDisplayName() override {return "Shape";};

  /// Get markup short name
  const char* GetDefaultNodeNamePrefix() override {return "SH";}

  /// \sa vtkMRMLNode::CopyContent
  vtkMRMLCopyContentDefaultMacro(vtkMRMLMarkupsShapeNode);
  
  vtkMRMLStorageNode* CreateDefaultStorageNode() override;
  void CreateDefaultDisplayNodes() override;
  
  vtkGetMacro(ShapeName, int);
  void SetShapeName(int shapeName);
  static const char* GetShapeNameAsString(int shapeName);
  static int GetShapeNameFromString(const char* name);
  static const char* GetRadiusModeAsString(int radiusMode);
  static int GetRadiusModeFromString(const char* mode);
  static const char* GetDrawMode2DAsString(int drawMode2D);
  static int GetDrawMode2DFromString(const char* mode);
  
  vtkSetClampMacro(RadiusMode, int, Centered, Circumferential);
  vtkGetMacro(RadiusMode, int);
  vtkSetClampMacro(DrawMode2D, int, Intersection, Projection);
  vtkGetMacro(DrawMode2D, int);
  vtkSetMacro(Resolution, double);
  vtkGetMacro(Resolution, double);
  
  bool GetCenterWorld(double center[3]);
  vtkPolyData * GetShapeWorld() const {return this->ShapeWorld;}
  // For Tube
  vtkPolyData * GetSplineWorld() const {return this->SplineWorld;}
  bool GetTrimmedSplineWorld(vtkPolyData * trimmedSpline,
                             int numberOfPointsToTrimAtStart = 25, int numberOfPointsToTrimAtEnd = 25);
  // This is to calculate volume with vtkMassProperties, it needs a closed polydata.
  vtkPolyData * GetCappedTubeWorld() const {return this->CappedTubeWorld;}
  // Used by 3D representation.
  void SetShapeWorld(vtkPolyData * polydata) {this->ShapeWorld = polydata;}
  void SetSplineWorld(vtkPolyData * polydata) {this->SplineWorld = polydata;}
  void SetCappedTubeWorld(vtkPolyData * polydata) {this->CappedTubeWorld = polydata;}
  
  vtkSetObjectMacro(ResliceNode, vtkMRMLNode);
  vtkGetObjectMacro(ResliceNode, vtkMRMLNode);
  
  void ResliceToControlPoints();
  void SetRadius(double radius);
  // For disk shape.
  void SetInnerRadius(double radius);
  void SetOuterRadius(double radius);
  bool DescribeDiskPointSpacing(double * closestPoint, double * farthestPoint,
                               double& innerRadius, double& outerRadius);
  // For Tube shape;
  double GetNthControlPointRadius(int n);
  void SetNthControlPointRadius(int n, double radius);
  bool SnapAllControlPointsToTubeSurface(bool bypassLockedState = false);
  vtkGetMacro(DisplayCappedTube, bool);
  vtkSetMacro(DisplayCappedTube, bool);
  vtkBooleanMacro(DisplayCappedTube, bool);
  // For Cone, Cylinder shapes.
  void SetHeight(double height);
  // For Cone.
  void SetAperture(double aperture); // Twice the angle of the slant on the axis.

protected:
  vtkMRMLMarkupsShapeNode();
  ~vtkMRMLMarkupsShapeNode() override;
  vtkMRMLMarkupsShapeNode(const vtkMRMLMarkupsShapeNode&);
  void operator=(const vtkMRMLMarkupsShapeNode&);
  
  void ResliceToPlane(int pointIndex1 = 0, int pointIndex2 = 1, int pointIndex3 = 2);
  void ResliceToLine(int pointIndex1 = 0, int pointIndex2 = 1);
  bool SnapNthControlPointToTubeSurface(int pointIndex = 0, bool bypassLockedState = false);
  void ResliceToTubeCrossSection(int pointIndex = 0);
  void ForceDiskMeasurements();
  void ForceRingMeasurements();
  void ForceSphereMeasurements();
  void ForceTubeMeasurements();
  void ForceCylinderMeasurements();
  void ForceConeMeasurements();
  void ForceArcMeasurements();
  
  void AddMeasurement(const char * name, bool enabled = false,
                            const char * format = "%-#4.4g %s", const char * units = "mm");
  void AddAreaMeasurement(const char * name, bool enabled = false,
                          const char * format = "%-#4.4g %s", // Space separator before unit gets ignored.
                          double coefficient = 0.01,  const char * units = "cm2");
  void AddVolumeMeasurement(const char * name, bool enabled = false,
                            const char * format = "%-#4.4g %s", // Space separator before unit gets ignored.
                            double coefficient = 0.001,  const char * units = "cm3");
  
  // Tube
  vtkSmartPointer<vtkCallbackCommand> OnPointPositionUndefinedCallback;
  static void OnPointPositionUndefined(vtkObject *caller,
                                       unsigned long event, void *clientData, void *callData);
  // Any shape
  vtkSmartPointer<vtkCallbackCommand> OnJumpToPointCallback;
  static void OnJumpToPoint(vtkObject *caller,
                                       unsigned long event, void *clientData, void *callData);

  int ShapeName { Sphere };
  int RadiusMode { Centered };
  int DrawMode2D { Intersection };
  double Resolution { 45.0 };
  
  bool DisplayNodeObserved = false;
  int ActiveControlPoint = 0;
  bool DisplayCappedTube = false;
  
  vtkPolyData * ShapeWorld = nullptr;
  vtkPolyData * CappedTubeWorld = nullptr;
  vtkPolyData * SplineWorld = nullptr;
  vtkMRMLNode * ResliceNode = nullptr;

private:
  bool RemovingPairControlPoint = false; // Tube
  
};

#endif //vtkmrmlmarkupsshapenode_h_
