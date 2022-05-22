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

#include "qMRMLMarkupsDiskWidget.h"
#include "ui_qMRMLMarkupsDiskWidget.h"

// MRML Nodes includes
#include "vtkMRMLMarkupsDiskNode.h"

// VTK includes
#include <vtkWeakPointer.h>

// --------------------------------------------------------------------------
class qMRMLMarkupsDiskWidgetPrivate:
  public Ui_qMRMLMarkupsDiskWidget
{
  Q_DECLARE_PUBLIC(qMRMLMarkupsDiskWidget);

protected:
  qMRMLMarkupsDiskWidget* const q_ptr;

public:
  qMRMLMarkupsDiskWidgetPrivate(qMRMLMarkupsDiskWidget* object);
  void setupUi(qMRMLMarkupsDiskWidget*);

  vtkWeakPointer<vtkMRMLMarkupsDiskNode> MarkupsDiskNode;
};

// --------------------------------------------------------------------------
qMRMLMarkupsDiskWidgetPrivate::
qMRMLMarkupsDiskWidgetPrivate(qMRMLMarkupsDiskWidget* object)
  : q_ptr(object)
{

}

// --------------------------------------------------------------------------
void qMRMLMarkupsDiskWidgetPrivate::setupUi(qMRMLMarkupsDiskWidget* widget)
{
  Q_Q(qMRMLMarkupsDiskWidget);

  this->Ui_qMRMLMarkupsDiskWidget::setupUi(widget);
  this->diskCollapsibleButton->setCollapsed(true);
  this->diskCollapsibleButton->setVisible(false);
  
  QObject::connect(this->resolutionSliderWidget, SIGNAL(valueChanged(double)),
                   q, SLOT(onResolutionChanged(double)));
}

// --------------------------------------------------------------------------
qMRMLMarkupsDiskWidget::
qMRMLMarkupsDiskWidget(QWidget *parent)
  : Superclass(parent),
    d_ptr(new qMRMLMarkupsDiskWidgetPrivate(this))
{
  this->setup();
}

// --------------------------------------------------------------------------
qMRMLMarkupsDiskWidget::~qMRMLMarkupsDiskWidget() = default;

// --------------------------------------------------------------------------
void qMRMLMarkupsDiskWidget::setup()
{
  Q_D(qMRMLMarkupsDiskWidget);
  d->setupUi(this);
}
// --------------------------------------------------------------------------
void qMRMLMarkupsDiskWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLMarkupsDiskWidget);

  if (!this->canManageMRMLMarkupsNode(d->MarkupsDiskNode))
    {
    d->diskCollapsibleButton->setVisible(false);
    return;
    }

  d->diskCollapsibleButton->setVisible(true);
}


//-----------------------------------------------------------------------------
bool qMRMLMarkupsDiskWidget::canManageMRMLMarkupsNode(vtkMRMLMarkupsNode *markupsNode) const
{
  Q_D(const qMRMLMarkupsDiskWidget);

  vtkMRMLMarkupsDiskNode* diskNode= vtkMRMLMarkupsDiskNode::SafeDownCast(markupsNode);
  if (!diskNode)
    {
    return false;
    }

  return true;
}

// --------------------------------------------------------------------------
void qMRMLMarkupsDiskWidget::setMRMLMarkupsNode(vtkMRMLMarkupsNode* markupsNode)
{
  Q_D(qMRMLMarkupsDiskWidget);

  d->MarkupsDiskNode = vtkMRMLMarkupsDiskNode::SafeDownCast(markupsNode);
  this->setEnabled(markupsNode != nullptr);
  if (d->MarkupsDiskNode)
  {
    d->resolutionSliderWidget->setValue(d->MarkupsDiskNode->GetResolution());
  }
}

// --------------------------------------------------------------------------
void qMRMLMarkupsDiskWidget::onResolutionChanged(double value)
{
  Q_D(qMRMLMarkupsDiskWidget);
  
  if (!d->MarkupsDiskNode)
  {
    return;
  }
  d->MarkupsDiskNode->SetResolution(value);
  d->MarkupsDiskNode->UpdateScene(this->mrmlScene());
}
