/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerStenosisMeasurement3DFooBarWidget.h"
#include "ui_qSlicerStenosisMeasurement3DFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_StenosisMeasurement3D
class qSlicerStenosisMeasurement3DFooBarWidgetPrivate
  : public Ui_qSlicerStenosisMeasurement3DFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerStenosisMeasurement3DFooBarWidget);
protected:
  qSlicerStenosisMeasurement3DFooBarWidget* const q_ptr;

public:
  qSlicerStenosisMeasurement3DFooBarWidgetPrivate(
    qSlicerStenosisMeasurement3DFooBarWidget& object);
  virtual void setupUi(qSlicerStenosisMeasurement3DFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerStenosisMeasurement3DFooBarWidgetPrivate
::qSlicerStenosisMeasurement3DFooBarWidgetPrivate(
  qSlicerStenosisMeasurement3DFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerStenosisMeasurement3DFooBarWidgetPrivate
::setupUi(qSlicerStenosisMeasurement3DFooBarWidget* widget)
{
  this->Ui_qSlicerStenosisMeasurement3DFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerStenosisMeasurement3DFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerStenosisMeasurement3DFooBarWidget
::qSlicerStenosisMeasurement3DFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerStenosisMeasurement3DFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerStenosisMeasurement3DFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerStenosisMeasurement3DFooBarWidget
::~qSlicerStenosisMeasurement3DFooBarWidget()
{
}
