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

#include "qMRMLMarkupsSphereWidget.h"
#include "ui_qMRMLMarkupsSphereWidget.h"

// MRML Nodes includes
#include "vtkMRMLMarkupsSphereNode.h"

// VTK includes
#include <vtkWeakPointer.h>

#include <qSlicerCoreApplication.h>

// --------------------------------------------------------------------------
class qMRMLMarkupsSphereWidgetPrivate:
  public Ui_qMRMLMarkupsSphereWidget
{
  Q_DECLARE_PUBLIC(qMRMLMarkupsSphereWidget);

protected:
  qMRMLMarkupsSphereWidget* const q_ptr;

public:
  qMRMLMarkupsSphereWidgetPrivate(qMRMLMarkupsSphereWidget* object);
  void setupUi(qMRMLMarkupsSphereWidget*);

  vtkWeakPointer<vtkMRMLMarkupsSphereNode> MarkupsSphereNode;
};

// --------------------------------------------------------------------------
qMRMLMarkupsSphereWidgetPrivate::
qMRMLMarkupsSphereWidgetPrivate(qMRMLMarkupsSphereWidget* object)
  : q_ptr(object)
{

}

// --------------------------------------------------------------------------
void qMRMLMarkupsSphereWidgetPrivate::setupUi(qMRMLMarkupsSphereWidget* widget)
{
  Q_Q(qMRMLMarkupsSphereWidget);

  this->Ui_qMRMLMarkupsSphereWidget::setupUi(widget);
  this->sphereCollapsibleButton->setVisible(false);
  this->sphereCollapsibleButton->setCollapsed(true);
  this->modeComboBox->addItem("Centered");
  this->modeComboBox->addItem("Circumferential");
  this->resliceInputSelector->setMRMLScene(widget->mrmlScene());
  
  QObject::connect(this->modeComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onModeChanged()));
  QObject::connect(this->resolutionSliderWidget, SIGNAL(valueChanged(double)),
                   q, SLOT(onResolutionChanged(double)));
  QObject::connect(this->resliceInputSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
                   q, SLOT(onResliceNodeChanged(vtkMRMLNode*)));
  QObject::connect(this->reslicePushButton, SIGNAL(clicked()),
                   q, SLOT(onResliceButtonClicked()));
}

// --------------------------------------------------------------------------
qMRMLMarkupsSphereWidget::
qMRMLMarkupsSphereWidget(QWidget *parent)
  : Superclass(parent),
    d_ptr(new qMRMLMarkupsSphereWidgetPrivate(this))
{
  this->setMRMLScene(qSlicerCoreApplication::application()->mrmlScene());
  this->setup();
}

// --------------------------------------------------------------------------
qMRMLMarkupsSphereWidget::~qMRMLMarkupsSphereWidget() = default;

// --------------------------------------------------------------------------
void qMRMLMarkupsSphereWidget::setup()
{
  Q_D(qMRMLMarkupsSphereWidget);
  d->setupUi(this);
}
// --------------------------------------------------------------------------
void qMRMLMarkupsSphereWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLMarkupsSphereWidget);

  if (!this->canManageMRMLMarkupsNode(d->MarkupsSphereNode))
    {
    d->sphereCollapsibleButton->setVisible(false);
    return;
    }

  d->sphereCollapsibleButton->setVisible(true);
}


//-----------------------------------------------------------------------------
bool qMRMLMarkupsSphereWidget::canManageMRMLMarkupsNode(vtkMRMLMarkupsNode *markupsNode) const
{
  Q_D(const qMRMLMarkupsSphereWidget);

  vtkMRMLMarkupsSphereNode* sphereNode= vtkMRMLMarkupsSphereNode::SafeDownCast(markupsNode);
  if (!sphereNode)
    {
    return false;
    }

  return true;
}

// --------------------------------------------------------------------------
void qMRMLMarkupsSphereWidget::setMRMLMarkupsNode(vtkMRMLMarkupsNode* markupsNode)
{
  Q_D(qMRMLMarkupsSphereWidget);

  d->MarkupsSphereNode = vtkMRMLMarkupsSphereNode::SafeDownCast(markupsNode);
  this->setEnabled(markupsNode != nullptr);
  if (d->MarkupsSphereNode)
  {
    d->modeComboBox->setCurrentIndex(d->MarkupsSphereNode->GetMode());
    d->resolutionSliderWidget->setValue(d->MarkupsSphereNode->GetResolution());
    d->resliceInputSelector->setCurrentNode(d->MarkupsSphereNode->GetResliceNode());
  }
}

// --------------------------------------------------------------------------
void qMRMLMarkupsSphereWidget::onModeChanged()
{
  Q_D(qMRMLMarkupsSphereWidget);
  
  if (!d->MarkupsSphereNode)
  {
    return;
  }
  d->MarkupsSphereNode->SetMode(d->modeComboBox->currentIndex());
  d->MarkupsSphereNode->UpdateAllMeasurements();
  d->MarkupsSphereNode->UpdateScene(this->mrmlScene());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsSphereWidget::onResolutionChanged(double value)
{
  Q_D(qMRMLMarkupsSphereWidget);
  
  if (!d->MarkupsSphereNode)
  {
    return;
  }
  d->MarkupsSphereNode->SetResolution(value);
  d->MarkupsSphereNode->UpdateScene(this->mrmlScene());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsSphereWidget::onResliceNodeChanged(vtkMRMLNode * node)
{
  Q_D(qMRMLMarkupsSphereWidget);
  
  if (!d->MarkupsSphereNode)
  {
    return;
  }
  d->MarkupsSphereNode->SetResliceNode(node);
  d->MarkupsSphereNode->UpdateScene(this->mrmlScene());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsSphereWidget::onResliceButtonClicked()
{
  Q_D(qMRMLMarkupsSphereWidget);
  
  if (!d->MarkupsSphereNode)
  {
    return;
  }
  d->MarkupsSphereNode->ResliceToControlPoints();
  d->MarkupsSphereNode->UpdateScene(this->mrmlScene());
}
