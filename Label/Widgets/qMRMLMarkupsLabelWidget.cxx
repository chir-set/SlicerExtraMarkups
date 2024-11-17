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

#include "qMRMLMarkupsLabelWidget.h"
#include "ui_qMRMLMarkupsLabelWidget.h"

// MRML Nodes includes
#include "vtkMRMLMarkupsLabelNode.h"

// VTK includes
#include <vtkWeakPointer.h>

// --------------------------------------------------------------------------
class qMRMLMarkupsLabelWidgetPrivate:
  public Ui_qMRMLMarkupsLabelWidget
{
  Q_DECLARE_PUBLIC(qMRMLMarkupsLabelWidget);

protected:
  qMRMLMarkupsLabelWidget* const q_ptr;

public:
  qMRMLMarkupsLabelWidgetPrivate(qMRMLMarkupsLabelWidget* object);
  void setupUi(qMRMLMarkupsLabelWidget*);

  vtkWeakPointer<vtkMRMLMarkupsLabelNode> MarkupsLabelNode;
};

// --------------------------------------------------------------------------
qMRMLMarkupsLabelWidgetPrivate::
qMRMLMarkupsLabelWidgetPrivate(qMRMLMarkupsLabelWidget* object)
  : q_ptr(object)
{

}

// --------------------------------------------------------------------------
void qMRMLMarkupsLabelWidgetPrivate::setupUi(qMRMLMarkupsLabelWidget* widget)
{
  Q_Q(qMRMLMarkupsLabelWidget);

  this->Ui_qMRMLMarkupsLabelWidget::setupUi(widget);
  this->labelCollapsibleButton->setVisible(true);
  this->ThreeDTipDimensionModeComboBox->addItem("View scale factor");
  this->ThreeDTipDimensionModeComboBox->addItem("Line length");  
  this->ThreeDTipDimensionModeComboBox->addItem("Fixed");
  
  QObject::connect(this->labelTextEdit, SIGNAL(textChanged()),
                   q, SLOT(onTextChanged()));
  QObject::connect(this->ThreeDTipDimensionModeComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onThreeDTipDimensionModeChanged(int)));
}

// --------------------------------------------------------------------------
qMRMLMarkupsLabelWidget::
qMRMLMarkupsLabelWidget(QWidget *parent)
  : Superclass(parent),
    d_ptr(new qMRMLMarkupsLabelWidgetPrivate(this))
{
  this->setup();
}

// --------------------------------------------------------------------------
qMRMLMarkupsLabelWidget::~qMRMLMarkupsLabelWidget() = default;

// --------------------------------------------------------------------------
void qMRMLMarkupsLabelWidget::setup()
{
  Q_D(qMRMLMarkupsLabelWidget);
  d->setupUi(this);
}
// --------------------------------------------------------------------------
void qMRMLMarkupsLabelWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLMarkupsLabelWidget);

  if (!this->canManageMRMLMarkupsNode(d->MarkupsLabelNode))
    {
    d->labelCollapsibleButton->setVisible(false);
    return;
    }

  int numberOfDefinedControlPoints = d->MarkupsLabelNode->GetNumberOfDefinedControlPoints(false);
  d->ThreeDTipDimensionModeLabel->setVisible(numberOfDefinedControlPoints == 2);
  d->ThreeDTipDimensionModeComboBox->setVisible(numberOfDefinedControlPoints == 2);
  d->labelCollapsibleButton->setVisible(true);
}


//-----------------------------------------------------------------------------
bool qMRMLMarkupsLabelWidget::canManageMRMLMarkupsNode(vtkMRMLMarkupsNode *markupsNode) const
{
  Q_D(const qMRMLMarkupsLabelWidget);

  vtkMRMLMarkupsLabelNode* LabelNode= vtkMRMLMarkupsLabelNode::SafeDownCast(markupsNode);
  if (!LabelNode)
    {
    return false;
    }

  return true;
}

// --------------------------------------------------------------------------
void qMRMLMarkupsLabelWidget::setMRMLMarkupsNode(vtkMRMLMarkupsNode* markupsNode)
{
  Q_D(qMRMLMarkupsLabelWidget);

  d->MarkupsLabelNode = vtkMRMLMarkupsLabelNode::SafeDownCast(markupsNode);
  this->setEnabled(markupsNode != nullptr);
  if (d->MarkupsLabelNode)
  {
    d->labelTextEdit->setPlainText(d->MarkupsLabelNode->GetLabel());
    d->ThreeDTipDimensionModeComboBox->setCurrentIndex( d->MarkupsLabelNode->GetTipDimensionMode3D());
  }
}

// --------------------------------------------------------------------------
void qMRMLMarkupsLabelWidget::onTextChanged()
{
  Q_D(qMRMLMarkupsLabelWidget);
  
  if (!d->MarkupsLabelNode)
  {
    return;
  }
  d->MarkupsLabelNode->SetLabel(d->labelTextEdit->toPlainText().toStdString().c_str());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsLabelWidget::onThreeDTipDimensionModeChanged(int mode)
{
  Q_D(qMRMLMarkupsLabelWidget);
  
  if (d->MarkupsLabelNode == nullptr)
  {
    return;
  }
  d->MarkupsLabelNode->SetTipDimensionMode3D(mode);
}
