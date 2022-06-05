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

#ifndef __qSlicerSphereWidget_h_
#define __qSlicerSphereWidget_h_

// Markups widgets includes
#include "qMRMLMarkupsAbstractOptionsWidget.h"
#include "qSlicerSphereModuleWidgetsExport.h"

#include <vtkMRMLNode.h>

class qMRMLMarkupsSphereWidgetPrivate;
class vtkMRMLMarkupsNode;

class Q_SLICER_MODULE_SPHERE_WIDGETS_EXPORT
qMRMLMarkupsSphereWidget : public qMRMLMarkupsAbstractOptionsWidget
{
  Q_OBJECT

  Q_PROPERTY(QString className READ className CONSTANT);

public:

  typedef qMRMLMarkupsAbstractOptionsWidget Superclass;
  qMRMLMarkupsSphereWidget(QWidget* parent=nullptr);
  ~qMRMLMarkupsSphereWidget() override;

  /// Gets the name of the additional options widget type
  const QString className() const override {return "qMRMLMarkupsSphereWidget";}

  /// Updates the widget based on information from MRML.
  void updateWidgetFromMRML() override;

  /// Checks whether a given node can be handled by the widget
  bool canManageMRMLMarkupsNode(vtkMRMLMarkupsNode *markupsNode) const override;

  /// Returns an instance of the widget
  qMRMLMarkupsAbstractOptionsWidget* createInstance() const override
  { return new qMRMLMarkupsSphereWidget(); }

public slots:
/// Set the MRML node of interest
  void setMRMLMarkupsNode(vtkMRMLMarkupsNode* node) override;
  void onModeChanged();
  void onResolutionChanged(double value);
  void onResliceNodeChanged(vtkMRMLNode * node);
  void onResliceButtonClicked();
  
protected:
  void setup();

protected:
  QScopedPointer<qMRMLMarkupsSphereWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLMarkupsSphereWidget);
  Q_DISABLE_COPY(qMRMLMarkupsSphereWidget);
};

#endif // __qSlicerSphereWidget_h_
