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

#ifndef __vtkslicerLabelrepresentation2d_h_
#define __vtkslicerLabelrepresentation2d_h_

#include "vtkSlicerLabelModuleVTKWidgetsExport.h"

// Markups VTKWidgets includes
#include "vtkSlicerMarkupsWidgetRepresentation2D.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkSampleImplicitFunctionFilter.h>
#include <vtkLineSource.h>

//------------------------------------------------------------------------------
class vtkGlyphSource2D;
class vtkPolyDataMapper2D;
class vtkActor2D;

/**
 * @class   vtkSlicerLabelRepresentation2D
 * @brief   Default representation for the line widget
 *
 * This class provides the default concrete representation for the
 * vtkMRMLAbstractWidget. See vtkMRMLAbstractWidget
 * for details.
 * @sa
 * vtkSlicerMarkupsWidgetRepresentation2D vtkMRMLAbstractWidget
*/
class VTK_SLICER_LABEL_MODULE_VTKWIDGETS_EXPORT vtkSlicerLabelRepresentation2D
: public vtkSlicerMarkupsWidgetRepresentation2D
{
public:
  static vtkSlicerLabelRepresentation2D* New();
  vtkTypeMacro(vtkSlicerLabelRepresentation2D, vtkSlicerMarkupsWidgetRepresentation2D);
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
  vtkSlicerLabelRepresentation2D();
  ~vtkSlicerLabelRepresentation2D() override;

  void UpdateTagFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData=nullptr);
  void UpdatePointerFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData=nullptr);

  void SetMarkupsNode(vtkMRMLMarkupsNode * markupsNode) override;
  vtkSmartPointer<vtkTransformPolyDataFilter> WorldToSliceTransformer;
  vtkSmartPointer<vtkSampleImplicitFunctionFilter> SliceDistance;
  
  vtkSmartPointer<vtkLineSource> LineSource;
  vtkSmartPointer<vtkPolyDataMapper2D> LineMapper;
  vtkSmartPointer<vtkActor2D> LineActor;

private:
  vtkSlicerLabelRepresentation2D(const vtkSlicerLabelRepresentation2D&) = delete;
  void operator=(const vtkSlicerLabelRepresentation2D&) = delete;
};

#endif // __vtkslicerLabelrepresentation3d_h_
