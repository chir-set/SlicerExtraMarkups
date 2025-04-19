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

#ifndef __qSlicerShapeWidget_h_
#define __qSlicerShapeWidget_h_

// Markups widgets includes
#include "qMRMLMarkupsAbstractOptionsWidget.h"
#include "qSlicerShapeModuleWidgetsExport.h"

#include <vtkMRMLNode.h>

class qMRMLMarkupsShapeWidgetPrivate;
class vtkMRMLMarkupsNode;

class Q_SLICER_MODULE_SHAPE_WIDGETS_EXPORT
qMRMLMarkupsShapeWidget : public qMRMLMarkupsAbstractOptionsWidget
{
  Q_OBJECT

  Q_PROPERTY(QString className READ className CONSTANT);

public:

  typedef qMRMLMarkupsAbstractOptionsWidget Superclass;
  qMRMLMarkupsShapeWidget(QWidget* parent=nullptr);
  ~qMRMLMarkupsShapeWidget() override;

  /// Gets the name of the additional options widget type
  const QString className() const override {return "qMRMLMarkupsShapeWidget";}

  /// Updates the widget based on information from MRML.
  void updateWidgetFromMRML() override;

  /// Checks whether a given node can be handled by the widget
  bool canManageMRMLMarkupsNode(vtkMRMLMarkupsNode *markupsNode) const override;

  /// Returns an instance of the widget
  qMRMLMarkupsAbstractOptionsWidget* createInstance() const override
  { return new qMRMLMarkupsShapeWidget(); }

public slots:
/// Set the MRML node of interest
  void setMRMLMarkupsNode(vtkMRMLMarkupsNode* node) override;

protected slots:
  void onShapeChanged(int shapeName);
  void onRadiusModeChanged();
  void onDrawModeChanged();
  void onResolutionChanged(double value);
  void onResliceNodeChanged(vtkMRMLNode * node);
  void onResliceButtonClicked();
  void onTubeMenuOptionButtonClicked();

  void onDisplayCappedTubeToggled(bool value);
  void onScalarVisibilityToggled(bool value); // Shared between Tube and Parametric shapes.
  void onSplineVisivilityTogggled(bool value);
  void onControlPointCountSpinBoxChanged(int value);
  void onSnapControlPoints();

  // Parametric shapes.
  // Object parameters.
  void onParametricIsotropicScalingButtonClicked();
  void onParametricNSliderChanged(double value);
  void onParametricN1SliderChanged(double value);
  void onParametricN2SliderChanged(double value);
  void onParametricRadiusSliderChanged(double value);
  void onParametricRingRadiusSliderChanged(double value);
  void onParametricCrossSectionRadiusSliderChanged(double value);
  void onParametricScalarModeChanged(int value);
  
  // UVW parameters.
  void onParametricsMinimumURangeChanged(double value);
  void onParametricsMaximumURangeChanged(double value);
  void onParametricsMinimumVRangeChanged(double value);
  void onParametricsMaximumVRangeChanged(double value);
  void onParametricsMinimumWRangeChanged(double value);
  void onParametricsMaximumWRangeChanged(double value);
  
  void onParametricsJoinUToogled(bool value);
  void onParametricsJoinVToogled(bool value);
  void onParametricsJoinWToogled(bool value);
  void onParametricsTwistUToogled(bool value);
  void onParametricsTwistVToogled(bool value);
  void onParametricsTwistWToogled(bool value);
  void onParametricsClockwiseOrderingToogled(bool value);

protected:
  void setup();

protected:
  QScopedPointer<qMRMLMarkupsShapeWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLMarkupsShapeWidget);
  Q_DISABLE_COPY(qMRMLMarkupsShapeWidget);
};

#endif // __qSlicerShapeWidget_h_
