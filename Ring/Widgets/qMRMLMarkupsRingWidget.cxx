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

#include "qMRMLMarkupsRingWidget.h"
#include "ui_qMRMLMarkupsRingWidget.h"

// MRML Nodes includes
#include "vtkMRMLMarkupsRingNode.h"

// VTK includes
#include <vtkWeakPointer.h>

#include <qSlicerCoreApplication.h>

// --------------------------------------------------------------------------
class qMRMLMarkupsRingWidgetPrivate:
  public Ui_qMRMLMarkupsRingWidget
{
  Q_DECLARE_PUBLIC(qMRMLMarkupsRingWidget);

protected:
  qMRMLMarkupsRingWidget* const q_ptr;

public:
  qMRMLMarkupsRingWidgetPrivate(qMRMLMarkupsRingWidget* object);
  void setupUi(qMRMLMarkupsRingWidget*);

  vtkWeakPointer<vtkMRMLMarkupsRingNode> MarkupsRingNode;
};

// --------------------------------------------------------------------------
qMRMLMarkupsRingWidgetPrivate::
qMRMLMarkupsRingWidgetPrivate(qMRMLMarkupsRingWidget* object)
  : q_ptr(object)
{

}

// --------------------------------------------------------------------------
void qMRMLMarkupsRingWidgetPrivate::setupUi(qMRMLMarkupsRingWidget* widget)
{
  Q_Q(qMRMLMarkupsRingWidget);

  this->Ui_qMRMLMarkupsRingWidget::setupUi(widget);
  this->ringCollapsibleButton->setVisible(false);
  this->ringCollapsibleButton->setCollapsed(true);
  this->radiusModeComboBox->addItem("Centered");
  this->radiusModeComboBox->addItem("Circumferential");
  this->drawModeComboBox->addItem("World projection");
  this->drawModeComboBox->addItem("World intersection");
  this->drawModeComboBox->addItem("Slice projection");
  this->resliceInputSelector->setMRMLScene(widget->mrmlScene());
  
  QObject::connect(this->radiusModeComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onRadiusModeChanged()));
  QObject::connect(this->drawModeComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onDrawModeChanged()));
  QObject::connect(this->resolutionSliderWidget, SIGNAL(valueChanged(double)),
                   q, SLOT(onResolutionChanged(double)));
  QObject::connect(this->resliceInputSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
                   q, SLOT(onResliceNodeChanged(vtkMRMLNode*)));
  QObject::connect(this->reslicePushButton, SIGNAL(clicked()),
                   q, SLOT(onResliceButtonClicked()));
}

// --------------------------------------------------------------------------
qMRMLMarkupsRingWidget::
qMRMLMarkupsRingWidget(QWidget *parent)
  : Superclass(parent),
    d_ptr(new qMRMLMarkupsRingWidgetPrivate(this))
{
  this->setMRMLScene(qSlicerCoreApplication::application()->mrmlScene());
  this->setup();
}

// --------------------------------------------------------------------------
qMRMLMarkupsRingWidget::~qMRMLMarkupsRingWidget() = default;

// --------------------------------------------------------------------------
void qMRMLMarkupsRingWidget::setup()
{
  Q_D(qMRMLMarkupsRingWidget);
  d->setupUi(this);
}
// --------------------------------------------------------------------------
void qMRMLMarkupsRingWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLMarkupsRingWidget);

  if (!this->canManageMRMLMarkupsNode(d->MarkupsRingNode))
    {
    d->ringCollapsibleButton->setVisible(false);
    return;
    }

  d->ringCollapsibleButton->setVisible(true);
}


//-----------------------------------------------------------------------------
bool qMRMLMarkupsRingWidget::canManageMRMLMarkupsNode(vtkMRMLMarkupsNode *markupsNode) const
{
  Q_D(const qMRMLMarkupsRingWidget);

  vtkMRMLMarkupsRingNode* ringNode= vtkMRMLMarkupsRingNode::SafeDownCast(markupsNode);
  if (!ringNode)
    {
    return false;
    }

  return true;
}

// --------------------------------------------------------------------------
void qMRMLMarkupsRingWidget::setMRMLMarkupsNode(vtkMRMLMarkupsNode* markupsNode)
{
  Q_D(qMRMLMarkupsRingWidget);

  d->MarkupsRingNode = vtkMRMLMarkupsRingNode::SafeDownCast(markupsNode);
  this->setEnabled(markupsNode != nullptr);
  if (d->MarkupsRingNode)
    {
      d->radiusModeComboBox->setCurrentIndex(d->MarkupsRingNode->GetRadiusMode());
      d->drawModeComboBox->setCurrentIndex(d->MarkupsRingNode->GetDrawMode2D());
      d->resolutionSliderWidget->setValue(d->MarkupsRingNode->GetResolution());
      d->resliceInputSelector->setCurrentNode(d->MarkupsRingNode->GetResliceNode());
    }
}

// --------------------------------------------------------------------------
void qMRMLMarkupsRingWidget::onRadiusModeChanged()
{
  Q_D(qMRMLMarkupsRingWidget);
  
  if (!d->MarkupsRingNode)
    {
      return;
    }
  d->MarkupsRingNode->SetRadiusMode(d->radiusModeComboBox->currentIndex());
  d->MarkupsRingNode->UpdateScene(this->mrmlScene());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsRingWidget::onDrawModeChanged()
{
  Q_D(qMRMLMarkupsRingWidget);
  
  if (!d->MarkupsRingNode)
  {
    return;
  }
  d->MarkupsRingNode->SetDrawMode2D(d->drawModeComboBox->currentIndex());
  d->MarkupsRingNode->UpdateScene(this->mrmlScene());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsRingWidget::onResolutionChanged(double value)
{
  Q_D(qMRMLMarkupsRingWidget);
  
  if (!d->MarkupsRingNode)
  {
    return;
  }
  d->MarkupsRingNode->SetResolution(value);
  d->MarkupsRingNode->UpdateScene(this->mrmlScene());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsRingWidget::onResliceNodeChanged(vtkMRMLNode * node)
{
  Q_D(qMRMLMarkupsRingWidget);
  
  if (!d->MarkupsRingNode)
  {
    return;
  }
  d->MarkupsRingNode->SetResliceNode(node);
  d->MarkupsRingNode->UpdateScene(this->mrmlScene());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsRingWidget::onResliceButtonClicked()
{
  Q_D(qMRMLMarkupsRingWidget);
  
  if (!d->MarkupsRingNode)
  {
    return;
  }
  d->MarkupsRingNode->ResliceToRingPlane();
  d->MarkupsRingNode->UpdateScene(this->mrmlScene());
}
