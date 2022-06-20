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

#include "qMRMLMarkupsShapeWidget.h"
#include "ui_qMRMLMarkupsShapeWidget.h"

// MRML Nodes includes
#include "vtkMRMLMarkupsShapeNode.h"

// VTK includes
#include <vtkWeakPointer.h>

// --------------------------------------------------------------------------
class qMRMLMarkupsShapeWidgetPrivate:
  public Ui_qMRMLMarkupsShapeWidget
{
  Q_DECLARE_PUBLIC(qMRMLMarkupsShapeWidget);

protected:
  qMRMLMarkupsShapeWidget* const q_ptr;

public:
  qMRMLMarkupsShapeWidgetPrivate(qMRMLMarkupsShapeWidget* object);
  void setupUi(qMRMLMarkupsShapeWidget*);

  vtkWeakPointer<vtkMRMLMarkupsShapeNode> MarkupsShapeNode;
};

// --------------------------------------------------------------------------
qMRMLMarkupsShapeWidgetPrivate::
qMRMLMarkupsShapeWidgetPrivate(qMRMLMarkupsShapeWidget* object)
  : q_ptr(object)
{

}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidgetPrivate::setupUi(qMRMLMarkupsShapeWidget* widget)
{
  Q_Q(qMRMLMarkupsShapeWidget);

  this->Ui_qMRMLMarkupsShapeWidget::setupUi(widget);
  this->shapeCollapsibleButton->setVisible(false);
}

// --------------------------------------------------------------------------
qMRMLMarkupsShapeWidget::
qMRMLMarkupsShapeWidget(QWidget *parent)
  : Superclass(parent),
    d_ptr(new qMRMLMarkupsShapeWidgetPrivate(this))
{
  this->setup();
}

// --------------------------------------------------------------------------
qMRMLMarkupsShapeWidget::~qMRMLMarkupsShapeWidget() = default;

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::setup()
{
  Q_D(qMRMLMarkupsShapeWidget);
  d->setupUi(this);
}
// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLMarkupsShapeWidget);

  if (!this->canManageMRMLMarkupsNode(d->MarkupsShapeNode))
    {
    d->shapeCollapsibleButton->setVisible(false);
    return;
    }

  d->shapeCollapsibleButton->setVisible(true);
}


//-----------------------------------------------------------------------------
bool qMRMLMarkupsShapeWidget::canManageMRMLMarkupsNode(vtkMRMLMarkupsNode *markupsNode) const
{
  Q_D(const qMRMLMarkupsShapeWidget);

  vtkMRMLMarkupsShapeNode* Shape_FIRST_LOWERNode= vtkMRMLMarkupsShapeNode::SafeDownCast(markupsNode);
  if (!Shape_FIRST_LOWERNode)
    {
    return false;
    }

  return true;
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::setMRMLMarkupsNode(vtkMRMLMarkupsNode* markupsNode)
{
  Q_D(qMRMLMarkupsShapeWidget);

  d->MarkupsShapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(markupsNode);
  this->setEnabled(markupsNode != nullptr);
}
