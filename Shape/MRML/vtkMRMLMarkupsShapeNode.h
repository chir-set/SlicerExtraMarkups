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
#include <vtkParametricFunctionSource.h>

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
    Ellipsoid,
    Toroid,
    BohemianDome,
    Bour,
    Boy,
    ConicSpiral,
    CrossCap,
    Kuen,
    Mobius,
    PluckerConoid,
    Roman,
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
  vtkMRMLCopyContentMacro(vtkMRMLMarkupsShapeNode);
  
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
  vtkGetMacro(ScalarVisibility, bool);
  vtkSetMacro(ScalarVisibility, bool);
  vtkBooleanMacro(ScalarVisibility, bool);
  vtkGetMacro(SplineVisibility, bool);
  vtkSetMacro(SplineVisibility, bool);
  vtkBooleanMacro(SplineVisibility, bool);
  // Parametrics.
  vtkGetMacro(ParametricN1, double);
  vtkGetMacro(ParametricN2, double);
  vtkGetMacro(ParametricN, double);
  vtkGetMacro(ParametricRingRadius, double);
  vtkGetMacro(ParametricCrossSectionRadius, double);
  vtkGetMacro(ParametricRadius, double);
  vtkGetMacro(ParametricX, double);
  vtkGetMacro(ParametricY, double);
  vtkGetMacro(ParametricZ, double);
  vtkSetMacro(ParametricMinimumU, double);
  vtkGetMacro(ParametricMinimumU, double);
  vtkSetMacro(ParametricMaximumU, double);
  vtkGetMacro(ParametricMaximumU, double);
  vtkSetMacro(ParametricMinimumV, double);
  vtkGetMacro(ParametricMinimumV, double);
  vtkSetMacro(ParametricMaximumV, double);
  vtkGetMacro(ParametricMaximumV, double);
  vtkSetMacro(ParametricMinimumW, double);
  vtkGetMacro(ParametricMinimumW, double);
  vtkSetMacro(ParametricMaximumW, double);
  vtkGetMacro(ParametricMaximumW, double);
  vtkSetMacro(ParametricJoinU, bool);
  vtkGetMacro(ParametricJoinU, bool);
  vtkBooleanMacro(ParametricJoinU, bool);
  vtkSetMacro(ParametricJoinV, bool);
  vtkGetMacro(ParametricJoinV, bool);
  vtkBooleanMacro(ParametricJoinV, bool);
  vtkSetMacro(ParametricJoinW, bool);
  vtkGetMacro(ParametricJoinW, bool);
  vtkBooleanMacro(ParametricJoinW, bool);
  vtkSetMacro(ParametricTwistU, bool);
  vtkGetMacro(ParametricTwistU, bool);
  vtkBooleanMacro(ParametricTwistU, bool);
  vtkSetMacro(ParametricTwistV, bool);
  vtkGetMacro(ParametricTwistV, bool);
  vtkBooleanMacro(ParametricTwistV, bool);
  vtkSetMacro(ParametricTwistW, bool);
  vtkGetMacro(ParametricTwistW, bool);
  vtkBooleanMacro(ParametricTwistW, bool);
  vtkSetMacro(ParametricClockwiseOrdering, bool);
  vtkGetMacro(ParametricClockwiseOrdering, bool);
  vtkBooleanMacro(ParametricClockwiseOrdering, bool);
  std::pair<double, double> GetParametricRangeU()
  {
    return ParametricRangeU;
  }
  std::pair<double, double> GetParametricRangeV()
  {
    return ParametricRangeV;
  }
  std::pair<double, double> GetParametricRangeW()
  {
    return ParametricRangeW;
  }
  
  vtkSetClampMacro(ParametricScalarMode, int, vtkParametricFunctionSource::SCALAR_NONE, vtkParametricFunctionSource::SCALAR_DISTANCE);
  vtkGetMacro(ParametricScalarMode, int);
  void SetParametricScalarModeToNone() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_NONE); }
  void SetParametricScalarModeToU() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_U); }
  void SetParametricScalarModeToV() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_V); }
  void SetParametricScalarModeToU0() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_U0); }
  void SetParametricScalarModeToV0() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_V0); }
  void SetParametricScalarModeToU0V0() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_U0V0); }
  void SetParametricScalarModeToModulus() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_MODULUS); }
  void SetParametricScalarModeToPhase() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_PHASE); }
  void SetParametricScalarModeToQuadrant() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_QUADRANT); }
  void SetParametricScalarModeToX() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_X); }
  void SetParametricScalarModeToY() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_Y); }
  void SetParametricScalarModeToZ() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_Z); }
  void SetParametricScalarModeToDistance() { this->SetParametricScalarMode(vtkParametricFunctionSource::SCALAR_DISTANCE); }
  
  bool SetParametricN(double value);
  bool SetParametricN1(double value);
  bool SetParametricN2(double value);
  bool SetParametricRadius(double value);
  bool SetParametricRingRadius(double value);
  bool SetParametricCrossSectionRadius(double value);
  
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
  // - The closest spline point to the middle point between the pair of control points.
  bool GetNthControlPointSplineIntersection(int pointIndex, vtkPoints * point);
  bool SnapNthControlPointToTubeSurface(int pointIndex = 0, bool bypassLockedState = false);
  bool SnapAllControlPointsToTubeSurface(bool bypassLockedState = false);
  bool UpdateNumberOfControlPoints(int numberOfControlPoints, bool bypassLockedState = false);
  vtkGetMacro(DisplayCappedTube, bool);
  vtkSetMacro(DisplayCappedTube, bool);
  vtkBooleanMacro(DisplayCappedTube, bool);
  // For Cone, Cylinder shapes.
  void SetHeight(double height);
  // For Cone.
  void SetAperture(double aperture); // Twice the angle of the slant on the axis.
  // For parametric shapes.
  bool IsParametric() const {return ShapeIsParametric;}
  bool SetParametricX(double value, bool moveControlPoint = true)
  {
    return SetParametricAxisValue('x', value, moveControlPoint);
  }
  bool SetParametricY(double value, bool moveControlPoint = true)
  {
    return SetParametricAxisValue('y', value, moveControlPoint);
  }
  bool SetParametricZ(double value, bool moveControlPoint = true)
  {
    return SetParametricAxisValue('z', value, moveControlPoint);
  }
  // Return 0 on success, 'x', 'y' or 'z' on failure.
  int SetParametricXYZ(double value); // Isotropic
  int SetParametricXYZ(double xvalue, double yvalue, double zvalue);
  
  bool SetParametricXYZToActiveControlPoint();

  // Handled in logic; is ignored in the storage node.
  std::string GetUseAlternateColors() {return UseAlternateColors;};
  void SetUseAlternateColors(const std::string& nodeID = "vtkMRMLColorTableNodeLabels");

protected:
  vtkMRMLMarkupsShapeNode();
  ~vtkMRMLMarkupsShapeNode() override;
  vtkMRMLMarkupsShapeNode(const vtkMRMLMarkupsShapeNode&);
  void operator=(const vtkMRMLMarkupsShapeNode&);
  
  void ResliceToPlane(int pointIndex1 = 0, int pointIndex2 = 1, int pointIndex3 = 2);
  void ResliceToLine(int pointIndex1 = 0, int pointIndex2 = 1);
  void ResliceToTubeCrossSection(int pointIndex = 0);
  void ForceDiskMeasurements();
  void ForceRingMeasurements();
  void ForceSphereMeasurements();
  void ForceTubeMeasurements();
  void ForceCylinderMeasurements();
  void ForceConeMeasurements();
  void ForceArcMeasurements();
  // Parametric shapes. Volume may be wrong if UVW values are not default.
  void ForceEllipsoidMeasurements();
  void ForceToroidMeasurements();
  void ForceBohemianDomeMeasurements();
  void ForceConicSpiralMeasurements();
  //   - Parametric shapes resized by a transform, not by object functions..
  void ForceTransformScaledMeasurements(bool withVolume = false);

  void AddCustomMeasurement(const char * name, bool enabled = false,
                            const char * format = "%-#4.4g %s", const char * units = nullptr);
  void AddLengthMeasurement(const char * name, bool enabled = false, bool withUnit = true, const char * suffix = "");
  void AddAreaMeasurement(const char * name, bool enabled = false);
  void AddVolumeMeasurement(const char * name, bool enabled = false);
  
  // Tube
  vtkSmartPointer<vtkCallbackCommand> OnPointPositionUndefinedCallback;
  static void OnPointPositionUndefined(vtkObject *caller,
                                       unsigned long event, void *clientData, void *callData);
  bool GetControlPointPairPosition(vtkPolyData * spline, int pointIndex, vtkPoints * result);

  // Any shape
  vtkSmartPointer<vtkCallbackCommand> OnJumpToPointCallback;
  static void OnJumpToPoint(vtkObject *caller,
                                       unsigned long event, void *clientData, void *callData);
  // Set Parametric{X,Y,Z} and optionally move a point at 'distance' from p1.
  bool SetParametricAxisValue(const char axis, double distance, bool moveControlPoint);

  int ShapeName { ShapeName_Last };
  int RadiusMode { Centered };
  int DrawMode2D { Intersection };
  double Resolution { 50.0 };
  
  int ActiveControlPoint = 0;
  bool DisplayCappedTube = false;
  bool ScalarVisibility = false;
  bool SplineVisibility = false;
  
  bool ShapeIsParametric = false;
  // SuperEllipsoid, SuperToroid.
  double ParametricN1 = 1.0;
  double ParametricN2 = 1.0;
  // ConicSpiral
  double ParametricN = 2.0;
  
  // Can be radius, scale factor, coefficients... Read first, second, third axis.
  double ParametricX = 1.0; // LR; p1 - p2
  double ParametricY = 1.0; // AP; p1 - p3
  double ParametricZ = 1.0; // IS; p1 - p4
  
  // SuperToroid.
  double ParametricRingRadius = 1.0;
  double ParametricCrossSectionRadius = 0.5;
  // Mobius, Roman.
  double ParametricRadius = 1.0;
  
  // Common parametrics.
  double ParametricMinimumU = 0.0;
  double ParametricMaximumU = 0.0;
  double ParametricMinimumV = 0.0;
  double ParametricMaximumV = 0.0;
  double ParametricMinimumW = 0.0;
  double ParametricMaximumW = 0.0;
  bool ParametricJoinU = false;
  bool ParametricJoinV = false;
  bool ParametricJoinW = false;
  bool ParametricTwistU = false;
  bool ParametricTwistV = false;
  bool ParametricTwistW = false;
  bool ParametricClockwiseOrdering = false;
  int ParametricScalarMode = vtkParametricFunctionSource::SCALAR_NONE;

  std::string UseAlternateColors = "vtkMRMLColorTableNodeLabels";

  // For UI control.
  std::pair<double, double> ParametricRangeU;
  std::pair<double, double> ParametricRangeV;
  std::pair<double, double> ParametricRangeW;
  // MinimumW, MaximumW, JoinW and TwistU do not seem to be used anywhere
  // in vtkParametric*.{h,cxx}.
  enum {MinimumU = 0, MaximumU, MinimumV, MaximumV, MinimumW, MaximumW,
        JoinU, JoinV, JoinW, TwistU, TwistV, TwistW,
        ClockwiseOrdering}; // Column names in tuples
  // Default UVW values for one geometry.
  typedef std::tuple<double, double, double, double, double, double,
                    bool, bool, bool, bool, bool, bool,
                    bool> ParametricsTuple;
  // Collection of defaults, from the header files.
  typedef std::map<int, ParametricsTuple> ParametricsMap;
  ParametricsMap Parametrics;
  // Set defaut UVW whenever a shape is selected.
  void ApplyDefaultParametrics();
  
  vtkPolyData * ShapeWorld = nullptr;
  vtkPolyData * CappedTubeWorld = nullptr;
  vtkPolyData * SplineWorld = nullptr;
  vtkMRMLNode * ResliceNode = nullptr;

private:
  bool RemovingPairControlPoint = false; // Tube
};

#endif //vtkmrmlmarkupsshapenode_h_
