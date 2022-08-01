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

#ifndef __qSlicerLabelWidget_h_
#define __qSlicerLabelWidget_h_

// Markups widgets includes
#include "qMRMLMarkupsAbstractOptionsWidget.h"
#include "qSlicerLabelModuleWidgetsExport.h"

class qMRMLMarkupsLabelWidgetPrivate;
class vtkMRMLMarkupsNode;

class Q_SLICER_MODULE_LABEL_WIDGETS_EXPORT
qMRMLMarkupsLabelWidget : public qMRMLMarkupsAbstractOptionsWidget
{
  Q_OBJECT

  Q_PROPERTY(QString className READ className CONSTANT);

public:

  typedef qMRMLMarkupsAbstractOptionsWidget Superclass;
  qMRMLMarkupsLabelWidget(QWidget* parent=nullptr);
  ~qMRMLMarkupsLabelWidget() override;

  /// Gets the name of the additional options widget type
  const QString className() const override {return "qMRMLMarkupsLabelWidget";}

  /// Updates the widget based on information from MRML.
  void updateWidgetFromMRML() override;

  /// Checks whether a given node can be handled by the widget
  bool canManageMRMLMarkupsNode(vtkMRMLMarkupsNode *markupsNode) const override;

  /// Returns an instance of the widget
  qMRMLMarkupsAbstractOptionsWidget* createInstance() const override
  { return new qMRMLMarkupsLabelWidget(); }

public slots:
/// Set the MRML node of interest
  void setMRMLMarkupsNode(vtkMRMLMarkupsNode* node) override;
  
  void onTextChanged();
  void onThreeDTipDimensionModeChanged(int mode);

protected:
  void setup();

protected:
  QScopedPointer<qMRMLMarkupsLabelWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLMarkupsLabelWidget);
  Q_DISABLE_COPY(qMRMLMarkupsLabelWidget);
};

#endif // __qSlicerLabelWidget_h_
