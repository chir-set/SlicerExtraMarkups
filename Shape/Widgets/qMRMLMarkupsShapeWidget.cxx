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
#include <vtkParametricFunctionSource.h>

#include <qSlicerCoreApplication.h>
#include <QMenu>
#include <QAction>
#include <QWidgetAction>
#include <QSpinBox>

// --------------------------------------------------------------------------
class qMRMLMarkupsShapeWidgetPrivate:
  public Ui_qMRMLMarkupsShapeWidget
{
  Q_DECLARE_PUBLIC(qMRMLMarkupsShapeWidget);

protected:
  qMRMLMarkupsShapeWidget* const q_ptr;

public:
  qMRMLMarkupsShapeWidgetPrivate(qMRMLMarkupsShapeWidget* object);
  ~qMRMLMarkupsShapeWidgetPrivate();
  void setupUi(qMRMLMarkupsShapeWidget*);

  vtkWeakPointer<vtkMRMLMarkupsShapeNode> MarkupsShapeNode;
  QMenu * TubeOptionMenu = nullptr;

  enum TubeMenuAction
  {
    ActionCapTube = 0,
    ActionScalarVisivility,
    ActionSplineVisibility,
    ActionSnapControlPoints
  };
};

// --------------------------------------------------------------------------
qMRMLMarkupsShapeWidgetPrivate::
qMRMLMarkupsShapeWidgetPrivate(qMRMLMarkupsShapeWidget* object)
  : q_ptr(object)
{

}

// --------------------------------------------------------------------------
qMRMLMarkupsShapeWidgetPrivate::~qMRMLMarkupsShapeWidgetPrivate()
{
  delete this->TubeOptionMenu;
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidgetPrivate::setupUi(qMRMLMarkupsShapeWidget* widget)
{
  Q_Q(qMRMLMarkupsShapeWidget);

  this->Ui_qMRMLMarkupsShapeWidget::setupUi(widget);
  this->shapeCollapsibleButton->setVisible(false);
  this->shapeCollapsibleButton->setCollapsed(false);
  this->shapeNameComboBox->addItem("Sphere");
  this->shapeNameComboBox->addItem("Ring");
  this->shapeNameComboBox->addItem("Disk");
  this->shapeNameComboBox->addItem("Tube");
  this->shapeNameComboBox->addItem("Cylinder");
  this->shapeNameComboBox->addItem("Cone");
  this->shapeNameComboBox->addItem("Arc");
  this->shapeNameComboBox->addItem("Ellipsoid");
  this->shapeNameComboBox->addItem("Toroid");
  this->shapeNameComboBox->addItem("BohemianDome");
  this->shapeNameComboBox->addItem("Bour");
  this->shapeNameComboBox->addItem("Boy");
  this->shapeNameComboBox->addItem("ConicSpiral");
  this->shapeNameComboBox->addItem("CrossCap");
  this->shapeNameComboBox->addItem("Kuen");
  this->shapeNameComboBox->addItem("Mobius");
  this->shapeNameComboBox->addItem("PluckerConoid");
  this->shapeNameComboBox->addItem("Roman");
  this->radiusModeComboBox->addItem("Centered");
  this->radiusModeComboBox->addItem("Circumferential");
  this->drawModeComboBox->addItem("Intersection");
  this->drawModeComboBox->addItem("Projection");
  this->resliceInputSelector->setMRMLScene(widget->mrmlScene());

  this->tubeMenuOptionButton->setVisible(false);
  this->TubeOptionMenu = new QMenu(this->tubeMenuOptionButton);
  this->tubeMenuOptionButton->setMenu(this->TubeOptionMenu);
  
  this->parametricScalarModeComboBox->addItem("None", vtkParametricFunctionSource::SCALAR_NONE);
  this->parametricScalarModeComboBox->addItem("U", vtkParametricFunctionSource::SCALAR_U);
  this->parametricScalarModeComboBox->addItem("V", vtkParametricFunctionSource::SCALAR_V);
  this->parametricScalarModeComboBox->addItem("U0", vtkParametricFunctionSource::SCALAR_U0);
  this->parametricScalarModeComboBox->addItem("V0", vtkParametricFunctionSource::SCALAR_V0);
  this->parametricScalarModeComboBox->addItem("U0V0", vtkParametricFunctionSource::SCALAR_U0V0);
  this->parametricScalarModeComboBox->addItem("Modulus", vtkParametricFunctionSource::SCALAR_MODULUS);
  this->parametricScalarModeComboBox->addItem("Phase", vtkParametricFunctionSource::SCALAR_PHASE);
  this->parametricScalarModeComboBox->addItem("Quadrant", vtkParametricFunctionSource::SCALAR_QUADRANT);
  this->parametricScalarModeComboBox->addItem("X", vtkParametricFunctionSource::SCALAR_X);
  this->parametricScalarModeComboBox->addItem("Y", vtkParametricFunctionSource::SCALAR_Y);
  this->parametricScalarModeComboBox->addItem("Z", vtkParametricFunctionSource::SCALAR_Z);
  this->parametricScalarModeComboBox->addItem("Distance", vtkParametricFunctionSource::SCALAR_DISTANCE);
  
  this->parametricScalarVisibilityToolButton->setVisible(false);
  this->parametricIsotropicScalingToolButton->setVisible(false);
  this->parametricNLabel->setVisible(false);
  this->parametricNSliderWidget->setVisible(false);
  this->parametricN1Label->setVisible(false);
  this->parametricN1SliderWidget->setVisible(false);
  this->parametricN2Label->setVisible(false);
  this->parametricN2SliderWidget->setVisible(false);
  this->parametricRadiusLabel->setVisible(false);
  this->parametricRadiusSliderWidget->setVisible(false);
  this->parametricRingRadiusLabel->setVisible(false);
  this->parametricRingRadiusSliderWidget->setVisible(false);
  this->parametricCrossSectionRadiusLabel->setVisible(false);
  this->parametricCrossSectionRadiusSliderWidget->setVisible(false);
  
  this->parametricsMainCollapsibleButton->setCollapsed(true);
  this->parametricsMainCollapsibleButton->setVisible(false);
  this->parametricsMoreCollapsibleButton->setCollapsed(true);

  QObject::connect(this->shapeNameComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onShapeChanged(int)));
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
  QObject::connect(this->tubeMenuOptionButton, SIGNAL(clicked()),
                   q, SLOT(onTubeMenuOptionButtonClicked()));
  QObject::connect(this->parametricScalarVisibilityToolButton, SIGNAL(clicked(bool)),
                   q, SLOT(onScalarVisibilityToggled(bool)));
  
  // Object parameters.
  QObject::connect(this->parametricIsotropicScalingToolButton, SIGNAL(clicked()),
                   q, SLOT(onParametricIsotropicScalingButtonClicked()));
  QObject::connect(this->parametricNSliderWidget, SIGNAL(valueChanged(double)),
                   q, SLOT(onParametricNSliderChanged(double)));
  QObject::connect(this->parametricN1SliderWidget, SIGNAL(valueChanged(double)),
                   q, SLOT(onParametricN1SliderChanged(double)));
  QObject::connect(this->parametricN2SliderWidget, SIGNAL(valueChanged(double)),
                   q, SLOT(onParametricN2SliderChanged(double)));
  QObject::connect(this->parametricRadiusSliderWidget, SIGNAL(valueChanged(double)),
                   q, SLOT(onParametricRadiusSliderChanged(double)));
  QObject::connect(this->parametricRingRadiusSliderWidget, SIGNAL(valueChanged(double)),
                   q, SLOT(onParametricRingRadiusSliderChanged(double)));
  QObject::connect(this->parametricCrossSectionRadiusSliderWidget, SIGNAL(valueChanged(double)),
                   q, SLOT(onParametricCrossSectionRadiusSliderChanged(double)));
  QObject::connect(this->parametricScalarModeComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onParametricScalarModeChanged(int)));
  
  // UVW parameters.
  QObject::connect(this->parametricsURangeWidget, SIGNAL(minimumValueChanged(double)),
                   q, SLOT(onParametricsMinimumURangeChanged(double)));
  QObject::connect(this->parametricsURangeWidget, SIGNAL(maximumValueChanged(double)),
                   q, SLOT(onParametricsMaximumURangeChanged(double)));
  QObject::connect(this->parametricsVRangeWidget, SIGNAL(minimumValueChanged(double)),
                   q, SLOT(onParametricsMinimumVRangeChanged(double)));
  QObject::connect(this->parametricsVRangeWidget, SIGNAL(maximumValueChanged(double)),
                   q, SLOT(onParametricsMaximumVRangeChanged(double)));
  QObject::connect(this->parametricsWRangeWidget, SIGNAL(minimumValueChanged(double)),
                   q, SLOT(onParametricsMinimumWRangeChanged(double)));
  QObject::connect(this->parametricsWRangeWidget, SIGNAL(maximumValueChanged(double)),
                   q, SLOT(onParametricsMaximumWRangeChanged(double)));
  
  QObject::connect(this->parametricsJoinUToolButton, SIGNAL(clicked(bool)),
                   q, SLOT(onParametricsJoinUToogled(bool)));
  QObject::connect(this->parametricsJoinVToolButton, SIGNAL(clicked(bool)),
                   q, SLOT(onParametricsJoinVToogled(bool)));
  QObject::connect(this->parametricsJoinWToolButton, SIGNAL(clicked(bool)),
                   q, SLOT(onParametricsJoinWToogled(bool)));
  QObject::connect(this->parametricsTwistUToolButton, SIGNAL(clicked(bool)),
                   q, SLOT(onParametricsTwistUToogled(bool)));
  QObject::connect(this->parametricsTwistVToolButton, SIGNAL(clicked(bool)),
                   q, SLOT(onParametricsTwistVToogled(bool)));
  QObject::connect(this->parametricsTwistWToolButton, SIGNAL(clicked(bool)),
                   q, SLOT(onParametricsTwistWToogled(bool)));
  QObject::connect(this->parametricsClockwiseOrderingToolButton, SIGNAL(clicked(bool)),
                   q, SLOT(onParametricsClockwiseOrderingToogled(bool)));
}

// --------------------------------------------------------------------------
qMRMLMarkupsShapeWidget::
qMRMLMarkupsShapeWidget(QWidget *parent)
  : Superclass(parent),
    d_ptr(new qMRMLMarkupsShapeWidgetPrivate(this))
{
  this->setMRMLScene(qSlicerCoreApplication::application()->mrmlScene());
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

  d->shapeNameComboBox->setCurrentIndex( d->MarkupsShapeNode->GetShapeName());
  d->radiusModeComboBox->setCurrentIndex(d->MarkupsShapeNode->GetRadiusMode());
  d->drawModeComboBox->setCurrentIndex(d->MarkupsShapeNode->GetDrawMode2D());
  d->resolutionSliderWidget->setValue(d->MarkupsShapeNode->GetResolution());
  d->resliceInputSelector->setCurrentNode(d->MarkupsShapeNode->GetResliceNode());
  // Set tube checkable menu item status here.
  // d->displayCappedTubeToolButton->setChecked(d->MarkupsShapeNode->GetDisplayCappedTube());
  d->parametricScalarVisibilityToolButton->setChecked(d->MarkupsShapeNode->GetScalarVisibility());
  
  if (!d->MarkupsShapeNode->IsParametric())
  {
    return;
  }
  
  // Object parameters.
  d->parametricScalarModeComboBox->setCurrentIndex(d->parametricScalarModeComboBox->findData(d->MarkupsShapeNode->GetParametricScalarMode()));
  d->parametricNSliderWidget->setValue(d->MarkupsShapeNode->GetParametricN());
  d->parametricN1SliderWidget->setValue(d->MarkupsShapeNode->GetParametricN1());
  d->parametricN2SliderWidget->setValue(d->MarkupsShapeNode->GetParametricN2());
  d->parametricRadiusSliderWidget->setValue(d->MarkupsShapeNode->GetParametricRadius());
  d->parametricRingRadiusSliderWidget->setValue(d->MarkupsShapeNode->GetParametricRingRadius());
  d->parametricCrossSectionRadiusSliderWidget->setValue(d->MarkupsShapeNode->GetParametricCrossSectionRadius());
  
  // UVW parameters.
  // Set the range for the current shape, then the current values.
  std::pair<double, double> parametricRangeU = d->MarkupsShapeNode->GetParametricRangeU();
  d->parametricsURangeWidget->setMinimum(parametricRangeU.first);
  d->parametricsURangeWidget->setMaximum(parametricRangeU.second);
  d->parametricsURangeWidget->setMinimumValue(d->MarkupsShapeNode->GetParametricMinimumU());
  d->parametricsURangeWidget->setMaximumValue(d->MarkupsShapeNode->GetParametricMaximumU());
  
  std::pair<double, double> parametricRangeV = d->MarkupsShapeNode->GetParametricRangeV();
  d->parametricsVRangeWidget->setMinimum(parametricRangeV.first);
  d->parametricsVRangeWidget->setMaximum(parametricRangeV.second);
  d->parametricsVRangeWidget->setMinimumValue(d->MarkupsShapeNode->GetParametricMinimumV());
  d->parametricsVRangeWidget->setMaximumValue(d->MarkupsShapeNode->GetParametricMaximumV());
  
  std::pair<double, double> parametricRangeW = d->MarkupsShapeNode->GetParametricRangeW();
  d->parametricsWRangeWidget->setMinimum(parametricRangeW.first);
  d->parametricsWRangeWidget->setMaximum(parametricRangeW.second);
  d->parametricsWRangeWidget->setMinimumValue(d->MarkupsShapeNode->GetParametricMinimumW());
  d->parametricsWRangeWidget->setMaximumValue(d->MarkupsShapeNode->GetParametricMaximumW());
  
  d->parametricsClockwiseOrderingToolButton->setChecked(d->MarkupsShapeNode->GetParametricClockwiseOrdering());
  d->parametricsJoinUToolButton->setChecked(d->MarkupsShapeNode->GetParametricJoinU());
  d->parametricsJoinVToolButton->setChecked(d->MarkupsShapeNode->GetParametricJoinV());
  d->parametricsJoinWToolButton->setChecked(d->MarkupsShapeNode->GetParametricJoinW());
  d->parametricsTwistUToolButton->setChecked(d->MarkupsShapeNode->GetParametricTwistU());
  d->parametricsTwistVToolButton->setChecked(d->MarkupsShapeNode->GetParametricTwistV());
  d->parametricsTwistWToolButton->setChecked(d->MarkupsShapeNode->GetParametricTwistW());
}


//-----------------------------------------------------------------------------
bool qMRMLMarkupsShapeWidget::canManageMRMLMarkupsNode(vtkMRMLMarkupsNode *markupsNode) const
{
  Q_D(const qMRMLMarkupsShapeWidget);

  vtkMRMLMarkupsShapeNode* shapeNode= vtkMRMLMarkupsShapeNode::SafeDownCast(markupsNode);
  if (!shapeNode)
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
  if (d->MarkupsShapeNode)
  {
    this->updateWidgetFromMRML();
  }
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onShapeChanged(int shapeName)
{
  Q_D(qMRMLMarkupsShapeWidget);

  if (d->MarkupsShapeNode == nullptr)
  {
    return;
  }
  if (shapeName < 0)
  {
    shapeName = vtkMRMLMarkupsShapeNode::Sphere;
  }

  d->MarkupsShapeNode->SetShapeName(shapeName);
  
  d->radiusModeLabel->setVisible(shapeName != vtkMRMLMarkupsShapeNode::Disk
                                && shapeName != vtkMRMLMarkupsShapeNode::Tube
                                && shapeName != vtkMRMLMarkupsShapeNode::Cone
                                && shapeName != vtkMRMLMarkupsShapeNode::Cylinder);
  d->radiusModeComboBox->setVisible(shapeName != vtkMRMLMarkupsShapeNode::Disk
                                && shapeName != vtkMRMLMarkupsShapeNode::Tube
                                && shapeName != vtkMRMLMarkupsShapeNode::Cone
                                && shapeName != vtkMRMLMarkupsShapeNode::Cylinder);
  d->tubeMenuOptionButton->setVisible(shapeName == vtkMRMLMarkupsShapeNode::Tube);
  d->parametricScalarVisibilityToolButton->setVisible(d->MarkupsShapeNode->IsParametric());
  
  // Object parameters.
  d->parametricIsotropicScalingToolButton->setVisible(d->MarkupsShapeNode->IsParametric()
                              && d->MarkupsShapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered);
  d->parametricN1Label->setVisible(shapeName == vtkMRMLMarkupsShapeNode::Ellipsoid
                                  || shapeName == vtkMRMLMarkupsShapeNode::Toroid);
  d->parametricN1SliderWidget->setVisible(shapeName == vtkMRMLMarkupsShapeNode::Ellipsoid
                                  || shapeName == vtkMRMLMarkupsShapeNode::Toroid);
  d->parametricN2Label->setVisible(shapeName == vtkMRMLMarkupsShapeNode::Ellipsoid
                                  || shapeName == vtkMRMLMarkupsShapeNode::Toroid);
  d->parametricN2SliderWidget->setVisible(shapeName == vtkMRMLMarkupsShapeNode::Ellipsoid
                                  || shapeName == vtkMRMLMarkupsShapeNode::Toroid);
  d->parametricRingRadiusLabel->setVisible(shapeName == vtkMRMLMarkupsShapeNode::Toroid);
  d->parametricRingRadiusSliderWidget->setVisible(shapeName == vtkMRMLMarkupsShapeNode::Toroid);
  d->parametricCrossSectionRadiusLabel->setVisible(shapeName == vtkMRMLMarkupsShapeNode::Toroid);
  d->parametricCrossSectionRadiusSliderWidget->setVisible(shapeName == vtkMRMLMarkupsShapeNode::Toroid);
  d->parametricNLabel->setVisible(shapeName == vtkMRMLMarkupsShapeNode::ConicSpiral
                                || shapeName == vtkMRMLMarkupsShapeNode::PluckerConoid);
  d->parametricNSliderWidget->setVisible(shapeName == vtkMRMLMarkupsShapeNode::ConicSpiral
                                || shapeName == vtkMRMLMarkupsShapeNode::PluckerConoid);
  d->parametricRadiusLabel->setVisible(shapeName == vtkMRMLMarkupsShapeNode::Mobius
                                || shapeName == vtkMRMLMarkupsShapeNode::Roman);
  d->parametricRadiusSliderWidget->setVisible(shapeName == vtkMRMLMarkupsShapeNode::Mobius
                                || shapeName == vtkMRMLMarkupsShapeNode::Roman);
  
  // UVW parameters.
  d->parametricsMainCollapsibleButton->setVisible(d->MarkupsShapeNode->IsParametric());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onRadiusModeChanged()
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetRadiusMode(d->radiusModeComboBox->currentIndex());
  d->parametricIsotropicScalingToolButton->setVisible(d->MarkupsShapeNode->IsParametric()
                && d->MarkupsShapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onDrawModeChanged()
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetDrawMode2D(d->drawModeComboBox->currentIndex());
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onResolutionChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetResolution(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onResliceNodeChanged(vtkMRMLNode * node)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetResliceNode(node);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onResliceButtonClicked()
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->ResliceToControlPoints();
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onDisplayCappedTubeToggled(bool value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetDisplayCappedTube(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onScalarVisibilityToggled(bool value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetScalarVisibility(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricIsotropicScalingButtonClicked()
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricXYZToActiveControlPoint();
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricNSliderChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricN(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricN1SliderChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricN1(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricN2SliderChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricN2(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricRadiusSliderChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricRadius(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricRingRadiusSliderChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricRingRadius(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricCrossSectionRadiusSliderChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricCrossSectionRadius(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricScalarModeChanged(int value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricScalarMode(d->parametricScalarModeComboBox->itemData(value).toInt());
}


// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsMinimumURangeChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricMinimumU(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsMaximumURangeChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricMaximumU(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsMinimumVRangeChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricMinimumV(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsMaximumVRangeChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricMaximumV(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsMinimumWRangeChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricMinimumW(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsMaximumWRangeChanged(double value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricMaximumW(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsJoinUToogled(bool value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricJoinU(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsJoinVToogled(bool value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricJoinV(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsJoinWToogled(bool value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricJoinW(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsTwistUToogled(bool value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricTwistU(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsTwistVToogled(bool value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricTwistV(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsTwistWToogled(bool value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricTwistW(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onParametricsClockwiseOrderingToogled(bool value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  
  if (!d->MarkupsShapeNode)
  {
    return;
  }
  d->MarkupsShapeNode->SetParametricClockwiseOrdering(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onTubeMenuOptionButtonClicked()
{
  Q_D(qMRMLMarkupsShapeWidget);
  if (!d->MarkupsShapeNode)
  {
    return;
  }

  d->TubeOptionMenu->clear();

  QAction * actionShowCappedTube = d->TubeOptionMenu->addAction("Cap ends");
  QAction * actionShowScalar = d->TubeOptionMenu->addAction("Scalar mapping");
  QAction * actionShowSpline = d->TubeOptionMenu->addAction("Show spline");
  actionShowCappedTube->setToolTip("Close each end of the tube");

  actionShowCappedTube->setData(d->ActionCapTube);
  actionShowScalar->setData(d->ActionScalarVisivility);
  actionShowSpline->setData(d->ActionSplineVisibility);

  actionShowCappedTube->setCheckable(true);
  actionShowScalar->setCheckable(true);
  actionShowSpline->setCheckable(true);

  actionShowCappedTube->setChecked(d->MarkupsShapeNode->GetDisplayCappedTube());
  actionShowScalar->setChecked(d->MarkupsShapeNode->GetScalarVisibility());
  actionShowSpline->setChecked(d->MarkupsShapeNode->GetSplineVisibility());

  QMenu* controlPointsCountMenu = new QMenu("Number of control point pairs", d->tubeMenuOptionButton);
  controlPointsCountMenu->setObjectName("TubeControlPointsCountMenu");
  QSpinBox * controlPointsCountSpinBox = new QSpinBox(controlPointsCountMenu);
  controlPointsCountSpinBox->setRange(2, INT_MAX);
  controlPointsCountSpinBox->setSingleStep(1);
  controlPointsCountSpinBox->setValue(d->MarkupsShapeNode->GetNumberOfControlPoints() / 2);
  QWidgetAction * controlPointsCountWidgetAction = new QWidgetAction(controlPointsCountMenu);
  controlPointsCountWidgetAction->setDefaultWidget(controlPointsCountSpinBox);
  controlPointsCountMenu->addAction(controlPointsCountWidgetAction);
  d->TubeOptionMenu->addMenu(controlPointsCountMenu);

  QAction * actionSnapControlPointsOnTube = d->TubeOptionMenu->addAction("Snap control points on the tube");

  actionSnapControlPointsOnTube->setData(d->ActionSnapControlPoints);

  QObject::connect(actionShowCappedTube, SIGNAL(triggered(bool)),
                   this, SLOT(onDisplayCappedTubeToggled(bool)));
  QObject::connect(actionShowScalar, SIGNAL(triggered(bool)),
                   this, SLOT(onScalarVisibilityToggled(bool)));
  QObject::connect(actionShowSpline, SIGNAL(triggered(bool)),
                   this, SLOT(onSplineVisivilityTogggled(bool)));
  QObject::connect(controlPointsCountSpinBox, SIGNAL(valueChanged(int)),
                   this, SLOT(onControlPointCountSpinBoxChanged(int)));
  QObject::connect(actionSnapControlPointsOnTube, SIGNAL(triggered()),
                   this, SLOT(onSnapControlPoints()));

  d->tubeMenuOptionButton->showMenu();
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onSplineVisivilityTogggled(bool value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  if (!d->MarkupsShapeNode)
  {
    return;
  }

  d->MarkupsShapeNode->SetSplineVisibility(value);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onControlPointCountSpinBoxChanged(int value)
{
  Q_D(qMRMLMarkupsShapeWidget);
  if (!d->MarkupsShapeNode)
  {
    return;
  }

  d->MarkupsShapeNode->UpdateNumberOfControlPoints(value * 2);
}

// --------------------------------------------------------------------------
void qMRMLMarkupsShapeWidget::onSnapControlPoints()
{
  Q_D(qMRMLMarkupsShapeWidget);
  if (!d->MarkupsShapeNode)
  {
    return;
  }

  d->MarkupsShapeNode->SnapAllControlPointsToTubeSurface();
}
