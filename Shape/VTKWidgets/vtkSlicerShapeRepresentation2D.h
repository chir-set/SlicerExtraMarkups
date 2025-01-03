/*==============================================================================

 Distributed under the OSI-approved BSD 3-Clause License.

  Copyright (c) Oslo University Hospital. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

  * Neither the name of Oslo University Hospital nor the names
    of Contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  This file was originally developed by Rafael Palomar (The Intervention Centre,
  Oslo University Hospital) and was supported by The Research Council of Norway
  through the ALive project (grant nr. 311393).

==============================================================================*/

#ifndef __vtkslicerShaperepresentation2d_h_
#define __vtkslicerShaperepresentation2d_h_

#include "vtkSlicerShapeModuleVTKWidgetsExport.h"

// Markups VTKWidgets includes
#include "vtkSlicerMarkupsWidgetRepresentation2D.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkDiskSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkSampleImplicitFunctionFilter.h>
#include <vtkCutter.h>
#include <vtkTubeFilter.h>
#include <vtkParametricSpline.h>
#include <vtkParametricSuperEllipsoid.h>
#include <vtkParametricSuperToroid.h>
#include <vtkParametricBohemianDome.h>
#include <vtkParametricBour.h>
#include <vtkParametricBoy.h>
#include <vtkParametricCrossCap.h>
#include <vtkParametricConicSpiral.h>
#include <vtkParametricKuen.h>
#include <vtkParametricMobius.h>
#include <vtkParametricPluckerConoid.h>
#include <vtkParametricRoman.h>
#include <vtkParametricFunctionSource.h>
#include <vtkConeSource.h>
#include <vtkArcSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

//------------------------------------------------------------------------------
class vtkGlyphSource2D;
class vtkPolyDataMapper2D;
class vtkActor2D;

/**
 * @class   vtkSlicerShapeRepresentation2D
 * @brief   Default representation for the line widget
 *
 * This class provides the default concrete representation for the
 * vtkMRMLAbstractWidget. See vtkMRMLAbstractWidget
 * for details.
 * @sa
 * vtkSlicerMarkupsWidgetRepresentation2D vtkMRMLAbstractWidget
*/
class VTK_SLICER_SHAPE_MODULE_VTKWIDGETS_EXPORT vtkSlicerShapeRepresentation2D
: public vtkSlicerMarkupsWidgetRepresentation2D
{
public:
  static vtkSlicerShapeRepresentation2D* New();
  vtkTypeMacro(vtkSlicerShapeRepresentation2D, vtkSlicerMarkupsWidgetRepresentation2D);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData=nullptr) override;

  /// Methods to make this class behave as a vtkProp.
  void GetActors(vtkPropCollection *) override;
  void ReleaseGraphicsResources(vtkWindow *) override;
  int RenderOverlay(vtkViewport *viewport) override;
  int RenderOpaqueGeometry(vtkViewport *viewport) override;
  int RenderTranslucentPolygonalGeometry(vtkViewport *viewport) override;
  vtkTypeBool HasTranslucentPolygonalGeometry() override;

protected:
  vtkSlicerShapeRepresentation2D();
  ~vtkSlicerShapeRepresentation2D() override;
  
  void SetMarkupsNode(vtkMRMLMarkupsNode *markupsNode) override;
  void UpdateDiskFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData=nullptr);
  void UpdateRingFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData=nullptr);
  void UpdateSphereFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData=nullptr);
  void UpdateTubeFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData=nullptr);
  void UpdateConeFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData=nullptr);
  void UpdateCylinderFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData=nullptr);
  void UpdateArcFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData=nullptr);
  void UpdateParametricFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData=nullptr);

  vtkSmartPointer<vtkGlyphSource2D> MiddlePointSource;
  vtkSmartPointer<vtkPolyDataMapper2D> MiddlePointDataMapper;
  vtkSmartPointer<vtkActor2D> MiddlePointActor;
  
  vtkSmartPointer<vtkPolyDataMapper2D> ParametricMiddlePointMapper;
  vtkSmartPointer<vtkActor2D> ParametricMiddlePointActor;
  vtkSmartPointer<vtkGlyphSource2D> ParametricMiddlePointSource;
  
  vtkSmartPointer<vtkSampleImplicitFunctionFilter> SliceDistance;
  vtkSmartPointer<vtkPlane> WorldPlane;
  vtkSmartPointer<vtkCutter> WorldCutter;
  vtkSmartPointer<vtkPolyDataMapper2D> WorldCutMapper;
  vtkSmartPointer<vtkActor2D> WorldCutActor;
  
  vtkSmartPointer<vtkLineSource> RadiusSource;
  vtkSmartPointer<vtkPolyDataMapper2D> RadiusMapper;
  vtkSmartPointer<vtkActor2D> RadiusActor;
  
  vtkSmartPointer<vtkDiskSource> DiskSource;
  vtkSmartPointer<vtkDiskSource> RingSource;
  vtkSmartPointer<vtkSphereSource> SphereSource;
  vtkSmartPointer<vtkConeSource> ConeSource;
  vtkSmartPointer<vtkTubeFilter> CylinderSource; // Regular tube.
  vtkSmartPointer<vtkLineSource> CylinderAxis;
  vtkSmartPointer<vtkArcSource> ArcSource;
  
  vtkSmartPointer<vtkParametricSpline> Spline;
  vtkSmartPointer<vtkParametricFunctionSource> SplineFunctionSource;
  vtkSmartPointer<vtkTubeFilter> Tube; // Variable radius tube.
  vtkSmartPointer<vtkTubeFilter> CappedTube;
  
  vtkSmartPointer<vtkParametricSuperEllipsoid> Ellipsoid;
  vtkSmartPointer<vtkParametricSuperToroid> Toroid;
  vtkSmartPointer<vtkParametricBohemianDome> BohemianDome;
  vtkSmartPointer<vtkParametricBour> Bour;
  vtkSmartPointer<vtkParametricBoy> Boy;
  vtkSmartPointer<vtkParametricCrossCap> CrossCap;
  vtkSmartPointer<vtkParametricConicSpiral> ConicSpiral;
  vtkSmartPointer<vtkParametricKuen> Kuen;
  vtkSmartPointer<vtkParametricMobius> Mobius;
  vtkSmartPointer<vtkParametricPluckerConoid> PluckerConoid;
  vtkSmartPointer<vtkParametricRoman> Roman;
  vtkSmartPointer<vtkParametricFunctionSource> ParametricFunctionSource;
  vtkSmartPointer<vtkTransform> ParametricTransform;
  vtkSmartPointer<vtkTransformPolyDataFilter> ParametricTransformer;
  
  vtkSmartPointer<vtkTransformPolyDataFilter> ShapeWorldToSliceTransformer;
  vtkSmartPointer<vtkTransformPolyDataFilter> ShapeCutWorldToSliceTransformer;
  vtkSmartPointer<vtkPolyDataMapper2D> ShapeMapper;
  vtkSmartPointer<vtkActor2D> ShapeActor;
  vtkSmartPointer<vtkProperty2D> ShapeProperty;

  vtkSmartPointer<vtkTransformPolyDataFilter> SplineWorldToSliceTransformer;
  vtkSmartPointer<vtkTransformPolyDataFilter> SplineCutWorldToSliceTransformer;
  vtkSmartPointer<vtkPolyDataMapper2D> SplineMapper;
  vtkSmartPointer<vtkActor2D> SplineActor;
  vtkSmartPointer<vtkCutter> SplineWorldCutter;
  vtkSmartPointer<vtkPolyDataMapper2D> SplineWorldCutMapper;
  vtkSmartPointer<vtkActor2D> SplineWorldCutActor;
  
private:
  vtkSlicerShapeRepresentation2D(const vtkSlicerShapeRepresentation2D&) = delete;
  void operator=(const vtkSlicerShapeRepresentation2D&) = delete;
};

#endif // __vtkslicerShaperepresentation3d_h_
